/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

// TEX_S
SAMPLER2DSHADOW(tex_3, 3);
// TEX_D
SAMPLER2D(tex_4, 4);

uniform mat4 invViewMatrix;
uniform mat4 lightInvViewProjTexMatrix;
uniform vec4 fieldShadowData;
uniform vec4 shadowData;

float sampleShadowMap(vec2 base_uv, float u, float v, float shadowMapSizeInv, float lightDepth, vec3 worldSpacePos)
{
    vec2 uv = base_uv + vec2(u, v) * shadowMapSizeInv;

    vec3 shadowUv = vec3(uv, lightDepth);
    if(shadowUv.x < 0.0 || shadowUv.x > 1.0 || shadowUv.y < 0.0 || shadowUv.y > 1.0 || lightDepth < 0.0 || lightDepth > 1.0) return 1.0;

    float shadowFactor = shadow2D(tex_3, shadowUv);

#ifdef FIELD_SHADOW
#if BGFX_SHADER_LANGUAGE_HLSL > 400
    // Fade out shadows as the vertical distance between occluder and walkmesh increases
    // This is to prevent shadows being projected to multiple floors

    float shadowDistance = 0.0;
    vec4 lightDepths = textureGather(tex_4, shadowUv.xy, 0);
    for(int i = 0; i < 4; ++i)
    {
        vec4 shadowPos = vec4(shadowUv.xy, lightDepths[i], 1.0);
        vec4 worldSpaceShadowPos = mul(mul(invViewMatrix, lightInvViewProjTexMatrix), shadowPos);
        worldSpaceShadowPos.xyz /= worldSpaceShadowPos.w;

        shadowDistance = max(shadowDistance, worldSpaceShadowPos.z - worldSpacePos.z);
    }

    float shadowFadeStartDistance = fieldShadowData.y;
    float fadeRange = fieldShadowData.z;
    float fadeFactor = max(0.0, min(1.0, (shadowDistance - shadowFadeStartDistance) / fadeRange));

    shadowFactor = mix(shadowFactor, 1.0, fadeFactor);
#endif
#endif

    return shadowFactor;
}

// Shadow filtering (OptimizedPCF)
// https://github.com/TheRealMJP/Shadows/blob/master/Shadows/Mesh.hlsl
float sampleShadowMapPCF7x7(vec3 shadowPos, vec3 worldSpacePos)
{
    float lightDepth = shadowPos.z;
    lightDepth -= shadowData.x;

    float shadowMapSize = shadowData.w;
    float shadowMapSizeInv = 1.0 / shadowMapSize;

    vec2 uv = shadowPos.xy * shadowMapSize;

    vec2 base_uv;
    base_uv.x = floor(uv.x + 0.5);
    base_uv.y = floor(uv.y + 0.5);

    float s = (uv.x + 0.5 - base_uv.x);
    float t = (uv.y + 0.5 - base_uv.y);

    base_uv -= vec2(0.5, 0.5);
    base_uv *= shadowMapSizeInv;

    float uw0 = (5 * s - 6);
    float uw1 = (11 * s - 28);
    float uw2 = -(11 * s + 17);
    float uw3 = -(5 * s + 1);

    float u0 = (4 * s - 5) / uw0 - 3;
    float u1 = (4 * s - 16) / uw1 - 1;
    float u2 = -(7 * s + 5) / uw2 + 1;
    float u3 = -s / uw3 + 3;

    float vw0 = (5 * t - 6);
    float vw1 = (11 * t - 28);
    float vw2 = -(11 * t + 17);
    float vw3 = -(5 * t + 1);

    float v0 = (4 * t - 5) / vw0 - 3;
    float v1 = (4 * t - 16) / vw1 - 1;
    float v2 = -(7 * t + 5) / vw2 + 1;
    float v3 = -t / vw3 + 3;

    float sum = 0.0;

    sum += uw0 * vw0 * sampleShadowMap(base_uv, u0, v0, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw1 * vw0 * sampleShadowMap(base_uv, u1, v0, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw2 * vw0 * sampleShadowMap(base_uv, u2, v0, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw3 * vw0 * sampleShadowMap(base_uv, u3, v0, shadowMapSizeInv, lightDepth, worldSpacePos);

    sum += uw0 * vw1 * sampleShadowMap(base_uv, u0, v1, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw1 * vw1 * sampleShadowMap(base_uv, u1, v1, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw2 * vw1 * sampleShadowMap(base_uv, u2, v1, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw3 * vw1 * sampleShadowMap(base_uv, u3, v1, shadowMapSizeInv, lightDepth, worldSpacePos);

    sum += uw0 * vw2 * sampleShadowMap(base_uv, u0, v2, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw1 * vw2 * sampleShadowMap(base_uv, u1, v2, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw2 * vw2 * sampleShadowMap(base_uv, u2, v2, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw3 * vw2 * sampleShadowMap(base_uv, u3, v2, shadowMapSizeInv, lightDepth, worldSpacePos);

    sum += uw0 * vw3 * sampleShadowMap(base_uv, u0, v3, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw1 * vw3 * sampleShadowMap(base_uv, u1, v3, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw2 * vw3 * sampleShadowMap(base_uv, u2, v3, shadowMapSizeInv, lightDepth, worldSpacePos);
    sum += uw3 * vw3 * sampleShadowMap(base_uv, u3, v3, shadowMapSizeInv, lightDepth, worldSpacePos);

    vec2 baseUvShifted = abs(base_uv * 2.0 - 1.0);
    float fadeCoeff = saturate(max(baseUvShifted.x, baseUvShifted.y) - 0.9) / 0.1;

    return mix(sum * 1.0 / 2704.0, 1.0, fadeCoeff);
}