/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

$input v_color0, v_texcoord0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"

SAMPLER2D(tex_0, 0);
SAMPLER2D(tex_1, 1);
SAMPLER2D(tex_2, 2);

uniform vec4 VSFlags;
uniform vec4 FSAlphaFlags;
uniform vec4 FSMiscFlags;
uniform vec4 FSHDRFlags;
uniform vec4 FSTexFlags;
uniform vec4 FSMovieFlags;
uniform vec4 TimeColor;
uniform vec4 TimeData;

#define isTLVertex VSFlags.x > 0.0
#define isFBTexture VSFlags.z > 0.0
#define isTexture VSFlags.w > 0.0
// ---
#define inAlphaRef FSAlphaFlags.x

#define isAlphaNever abs(FSAlphaFlags.y - 0.0) < 0.00001
#define isAlphaLess abs(FSAlphaFlags.y - 1.0) < 0.00001
#define isAlphaEqual abs(FSAlphaFlags.y - 2.0) < 0.00001
#define isAlphaLEqual abs(FSAlphaFlags.y - 3.0) < 0.00001
#define isAlphaGreater abs(FSAlphaFlags.y - 4.0) < 0.00001
#define isAlphaNotEqual abs(FSAlphaFlags.y - 5.0) < 0.00001
#define isAlphaGEqual abs(FSAlphaFlags.y - 6.0) < 0.00001

#define doAlphaTest FSAlphaFlags.z > 0.0


// ---
#define isFullRange FSMiscFlags.x > 0.0
#define isYUV FSMiscFlags.y > 0.0
#define modulateAlpha FSMiscFlags.z > 0.0
#define isMovie FSMiscFlags.w > 0.0

#define isHDR FSHDRFlags.x > 0.0
#define monitorNits FSHDRFlags.y

#define doGamutOverride FSHDRFlags.z > 0.0


#define isBT601ColorMatrix abs(FSMovieFlags.x - 0.0) < 0.00001
#define isBT709ColorMatrix abs(FSMovieFlags.x - 1.0) < 0.00001
#define isBRG24ColorMatrix abs(FSMovieFlags.x - 2.0) < 0.00001

#define isSRGBColorGamut abs(FSMovieFlags.y - 0.0) < 0.00001
#define isNTSCJColorGamut abs(FSMovieFlags.y - 1.0) < 0.00001
#define isSMPTECColorGamut abs(FSMovieFlags.y - 2.0) < 0.00001
#define isEBUColorGamut abs(FSMovieFlags.y - 3.0) < 0.00001

#define isSRGBGamma abs(FSMovieFlags.z - 0.0) < 0.00001
#define is2pt2Gamma abs(FSMovieFlags.z - 1.0) < 0.00001
#define is170MGamma abs(FSMovieFlags.z - 2.0) < 0.00001
#define isToelessSRGBGamma abs(FSMovieFlags.z - 3.0) < 0.00001
#define is2pt8Gamma abs(FSMovieFlags.z - 4.0) < 0.00001

#define isOverallSRGBColorGamut abs(FSMovieFlags.w - 0.0) < 0.00001
#define isOverallNTSCJColorGamut abs(FSMovieFlags.w - 1.0) < 0.00001

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

void main()
{
	vec4 color = vec4(toLinear(v_color0.rgb), v_color0.a);

    if (isTexture)
    {
        if (isYUV)
        {
            vec3 yuv = vec3(
                texture2D(tex_0, v_texcoord0.xy).r,
                texture2D(tex_1, v_texcoord0.xy).r,
                texture2D(tex_2, v_texcoord0.xy).r
            );

// d3d9 doesn't support textureSize()
#if BGFX_SHADER_LANGUAGE_HLSL > 300 || BGFX_SHADER_LANGUAGE_GLSL || BGFX_SHADER_LANGUAGE_SPIRV
            if (!(isFullRange)){
                // dither prior to range conversion
                ivec2 ydimensions = textureSize(tex_0, 0);
                ivec2 udimensions = textureSize(tex_1, 0);
                ivec2 vdimensions = textureSize(tex_2, 0);
                yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, 255.0, 1.0);
                // clamp back to tv range
                yuv = clamp(yuv, vec3_splat(16.0/255.0), vec3(235.0/255.0, 240.0/255.0, 240.0/255.0));
            }
#endif
            
            if (isBT601ColorMatrix){
                yuv.g = yuv.g - (128.0/255.0);
                yuv.b = yuv.b - (128.0/255.0);
                if (isFullRange){
                    color.rgb = toRGB_bt601_fullrange(yuv);
                }
                else {
                    yuv.r = saturate(yuv.r - (16.0/255.0));
                    color.rgb = toRGB_bt601_tvrange(yuv);
                }
            }
            else if (isBT709ColorMatrix){
                yuv.g = yuv.g - (128.0/255.0);
                yuv.b = yuv.b - (128.0/255.0);
                if (isFullRange){
                    color.rgb = toRGB_bt709_fullrange(yuv);
                }
                else {
                    yuv.r = saturate(yuv.r - (16.0/255.0));
                    color.rgb = toRGB_bt709_tvrange(yuv);
                }
            }
            else if (isBRG24ColorMatrix){
                color.rgb = yuv;
            }
            // default should be unreachable
            else {
                color.rgb = vec3_splat(0.5);
            }

            // Use a different inverse gamma function depending on the FMV's metadata
            if (isToelessSRGBGamma){
                color.rgb = toLinearToelessSRGB(color.rgb);
            }
            else if (is2pt2Gamma){
                color.rgb = toLinear2pt2(color.rgb);
            }
            else if (is170MGamma){
                color.rgb = toLinearSMPTE170M(color.rgb);
            }
            else if (is2pt8Gamma){
                color.rgb = toLinear2pt8(color.rgb);
            }
            else {
                color.rgb = toLinear(color.rgb);
            }
            
            // Convert gamut to BT709/SRGB or NTSC-J, depending on what we're going to do in post.
            // This approach has the unfortunate drawback of resulting in two gamut conversions for some inputs.
            // But it seems to be the only way to avoid breaking stuff that has expectations about the texture colors (like animated field textures).
            // Use of NTSC-J as the source gamut  for the original videos and their derivatives is a *highly* probable guess:
            // It looks correct, is consistent with the PS1's movie decoder chip's known use of BT601 color matrix, and conforms with Japanese TV standards of the time.
            if (isOverallNTSCJColorGamut){
                // do nothing for NTSC-J
                if ((isSRGBColorGamut) || (isSMPTECColorGamut) || (isEBUColorGamut)){
                    color.rgb = GamutLUT(color.rgb);
                    // dither after the LUT operation
                    #if BGFX_SHADER_LANGUAGE_HLSL > 300 || BGFX_SHADER_LANGUAGE_GLSL || BGFX_SHADER_LANGUAGE_SPIRV
                    ivec2 dimensions = textureSize(tex_0, 0);
                    color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 255.0, 4320.0);
                    #endif
                }
                // TODO: bring this back for HDR if out-of-bounds colors gets the OK
                /*
                if (isSRGBColorGamut){
                    color.rgb = convertGamut_SRGBtoNTSCJ(color.rgb);
                }
                else if (isSMPTECColorGamut){
                    color.rgb = convertGamut_SMPTECtoNTSCJ(color.rgb);
                }
                else if (isEBUColorGamut){
                    color.rgb = convertGamut_EBUtoNTSCJ(color.rgb);
                }*/
            }
            // overall sRGB
            else {
                // do nothing for sRGB
                if ((isNTSCJColorGamut) || (isSMPTECColorGamut) || (isEBUColorGamut)){
                    color.rgb = GamutLUT(color.rgb);
                    // dither after the LUT operation
                    #if BGFX_SHADER_LANGUAGE_HLSL > 300 || BGFX_SHADER_LANGUAGE_GLSL || BGFX_SHADER_LANGUAGE_SPIRV
                    ivec2 dimensions = textureSize(tex_0, 0);
                    color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 255.0, 4320.0);
                    #endif
                }
                // TODO: bring this back for HDR if out-of-bounds colors gets the OK
                /*
                if (isNTSCJColorGamut){
                    color.rgb = convertGamut_NTSCJtoSRGB(color.rgb);
                }
                else if (isSMPTECColorGamut){
                    color.rgb = convertGamut_SMPTECtoSRGB(color.rgb);
                }
                else if (isEBUColorGamut){
                    color.rgb = convertGamut_EBUtoSRGB(color.rgb);
                }
                */
            }
            
            color.a = 1.0;
        }
        else
        {
            vec4 texture_color = texture2D(tex_0, v_texcoord0.xy);

            if (doAlphaTest)
            {
                //NEVER
                if (isAlphaNever) discard;

                //LESS
                if (isAlphaLess)
                {
                    if (!(texture_color.a < inAlphaRef)) discard;
                }

                //EQUAL
                if (isAlphaEqual)
                {
                    if (!(texture_color.a == inAlphaRef)) discard;
                }

                //LEQUAL
                if (isAlphaLEqual)
                {
                    if (!(texture_color.a <= inAlphaRef)) discard;
                }

                //GREATER
                if (isAlphaGreater)
                {
                    if (!(texture_color.a > inAlphaRef)) discard;
                }

                //NOTEQUAL
                if (isAlphaNotEqual)
                {
                    if (!(texture_color.a != inAlphaRef)) discard;
                }

                //GEQUAL
                if (isAlphaGEqual)
                {
                    if (!(texture_color.a >= inAlphaRef)) discard;
                }
            }

            if (isFBTexture)
            {
                if(all(equal(texture_color.rgb,vec3_splat(0.0)))) discard;

                // This was previously in gamma space, so linearize again.
                texture_color.rgb = toLinear(texture_color.rgb);
            }
            // This stanza currently does nothing because there's no way to set doGamutOverride.
            // Hopefully the future will bring a way to set this for types of textures (e.g., world, model, field, spell, etc.) or even for individual textures based on metadata.
            else if (doGamutOverride){
                texture_color.rgb = GamutLUT(texture_color.rgb);
                #if BGFX_SHADER_LANGUAGE_HLSL > 300 || BGFX_SHADER_LANGUAGE_GLSL || BGFX_SHADER_LANGUAGE_SPIRV
                ivec2 dimensions = textureSize(tex_0, 0);
                texture_color.rgb = QuasirandomDither(texture_color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 255.0, 1.0);
                #endif
                // TODO: bring this back for HDR if given the OK for out-of-bounds colors
                /*
                if (isOverallNTSCJColorGamut){
                    texture_color.rgb = convertGamut_SRGBtoNTSCJ(texture_color.rgb);
                }
                else {
                    texture_color.rgb = convertGamut_NTSCJtoSRGB(texture_color.rgb);
                }
                */
            }

            if (isMovie) texture_color.a = 1.0;

            if (texture_color.a == 0.0) discard;

            if (modulateAlpha) color *= texture_color;
            else
            {
                color.rgb *= texture_color.rgb;
			    color.a = texture_color.a;
            }
        }
    }

    if (isTimeFilterEnabled) color.rgb *= TimeColor.rgb;


    // return to gamma space so we can do alpha blending the same way FF7/8 did.
    color.rgb = toGamma(color.rgb);


    gl_FragColor = color;
}
