/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

// We need to set WINVER and _WIN32_WINNT to Win10 in order for DISPLAYCONFIG_SDR_WHITE_LEVEL to be defined.
// Since the relevant functions have been defined since Win7, and we're only using a johnny-come-lately *parameter*,
// the SDR-white-level-for-HDR autodetection code should fail gracefully with a bad parameter error on older versions of Windows.
// In theory at least. (Not tested yet.)
// It does fail gracefully on Wine.
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <vector>

#include "lighting.h"
#include "ff7/widescreen.h"
#include "image/image.h"
#include "gl.h"
#include "log.h"
#include "cfg.h"
#include "utils.h"
#include "renderer.h"

CMRC_DECLARE(FFNx);

Renderer newRenderer;
RendererCallbacks bgfxCallbacks;

// BGFX CALLBACKS
void RendererCallbacks::fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str)
{
    std::string error;

    switch (_code) {
    case bgfx::Fatal::Enum::DebugCheck: error = "Debug Check";
    case bgfx::Fatal::Enum::InvalidShader: error = "Invalid Shader";
    case bgfx::Fatal::Enum::UnableToInitialize: error = "Unable To Initialize";
    case bgfx::Fatal::Enum::UnableToCreateTexture: error = "Unable To Create Texture";
    case bgfx::Fatal::Enum::DeviceLost: error = "Device Lost";
    }

    ffnx_error("[%s] %s\n", error.c_str(), _str);
}

void RendererCallbacks::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList)
{
    if (renderer_debug)
    {
        char buffer[16 * 1024];

        va_list argListCopy;
        va_copy(argListCopy, _argList);
        vsnprintf(buffer, sizeof(buffer), _format, argListCopy);
        va_end(argListCopy);

        ffnx_trace("%s", buffer);
    }
}

uint32_t RendererCallbacks::cacheReadSize(uint64_t _id)
{
    // Return 0 if shader is not found.
    return 0;
}

bool RendererCallbacks::cacheRead(uint64_t _id, void* _data, uint32_t _size)
{
    // Shader is not found in cache, needs to be rebuilt.
    return false;
}

void RendererCallbacks::cacheWrite(uint64_t _id, const void* _data, uint32_t _size)
{
}

// PRIVATE

// Via https://stackoverflow.com/a/14375308
uint32_t Renderer::createBGRA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((b & 0xff) << 24) + ((g & 0xff) << 16) + ((r & 0xff) << 8) + (a & 0xff);
}

void Renderer::setCommonUniforms()
{
    internalState.VSFlags = {
        (float)internalState.bIsTLVertex,
        (float)internalState.blendMode,
        (float)internalState.bIsFBTexture,
        (float)internalState.bIsTexture
    };
    if (uniform_log) ffnx_trace("%s: VSFlags XYZW(isTLVertex %f, blendMode %f, isFBTexture %f, isTexture %f)\n", __func__, internalState.VSFlags[0], internalState.VSFlags[1], internalState.VSFlags[2], internalState.VSFlags[3]);

    internalState.FSAlphaFlags = {
        (float)internalState.alphaRef,
        (float)internalState.alphaFunc,
        (float)internalState.bDoAlphaTest,
        NULL
    };
    if (uniform_log) ffnx_trace("%s: FSAlphaFlags XYZW(inAlphaRef %f, inAlphaFunc %f, bDoAlphaTest %f, NULL)\n", __func__, internalState.FSAlphaFlags[0], internalState.FSAlphaFlags[1], internalState.FSAlphaFlags[2]);

    internalState.FSMiscFlags = {
        (float)internalState.bIsMovieFullRange,
        (float)internalState.bIsMovieYUV,
        (float)internalState.bModulateAlpha,
        (float)internalState.bIsMovie
    };
    if (uniform_log) ffnx_trace("%s: FSMiscFlags XYZW(isMovieFullRange %f, isMovieYUV %f, modulateAlpha %f, isMovie %f)\n", __func__, internalState.FSMiscFlags[0], internalState.FSMiscFlags[1], internalState.FSMiscFlags[2], internalState.FSMiscFlags[3]);

    internalState.FSHDRFlags = {
        (float)internalState.bIsHDR,
        (float)hdr_max_nits,
        (float)internalState.bIsOverrideGamut,
        NULL
    };
    if (uniform_log) ffnx_trace("%s: FSMiscFlags XYZW(isHDR %f, monitorNits %f, NULL, NULL)\n", __func__, internalState.FSHDRFlags[0], internalState.FSHDRFlags[1]);

    internalState.FSTexFlags = {
        (float)(internalState.texHandlers[RendererTextureSlot::TEX_NML].idx != bgfx::kInvalidHandle),
        (float)(internalState.texHandlers[RendererTextureSlot::TEX_PBR].idx != bgfx::kInvalidHandle),
        (float)(specularIblTexture.idx != bgfx::kInvalidHandle && diffuseIblTexture.idx != bgfx::kInvalidHandle && envBrdfTexture.idx != bgfx::kInvalidHandle),
        NULL
    };
    if (uniform_log) ffnx_trace("%s: FSTexFlags XYZW(isNmlTextureLoaded %f, isPbrTextureLoaded %f, isIblTextureLoaded %f, NULL)\n", __func__, internalState.FSTexFlags[0], internalState.FSTexFlags[1], internalState.FSTexFlags[2]);

    internalState.WMFlags = {
        (float)internalState.sphericalWorldRate,
        (float)internalState.bIsFogEnabled,
        NULL,
        NULL
    };
    if (uniform_log) ffnx_trace("%s: VSFlags XYZW(isTLVertex %f, blendMode %f, isFBTexture %f, isTexture %f)\n", __func__, internalState.VSFlags[0], internalState.VSFlags[1], internalState.VSFlags[2], internalState.VSFlags[3]);

    internalState.FSMovieFlags = {
        (float)internalState.bIsMovieColorMatrix,
        (float)internalState.bIsMovieColorGamut,
        (float)internalState.bIsMovieGammaType,
        (float)internalState.bIsOverallColorGamut,
    };
    if (uniform_log) ffnx_trace("%s: FSMovieFlags XYZW(color matrix %f, color gamut %f, gamma type %f, overall color gamut %f)\n", __func__, internalState.FSMovieFlags[0], internalState.FSMovieFlags[1], internalState.FSMovieFlags[2], internalState.FSMovieFlags[3]);

    internalState.gameLightingFlags = {
        (float)game_lighting,
        NULL,
        NULL,
        NULL,
    };

    setUniform(RendererUniform::VS_FLAGS,  internalState.VSFlags.data());
    setUniform(RendererUniform::FS_ALPHA_FLAGS, internalState.FSAlphaFlags.data());
    setUniform(RendererUniform::FS_MISC_FLAGS, internalState.FSMiscFlags.data());
    setUniform(RendererUniform::FS_HDR_FLAGS, internalState.FSHDRFlags.data());
    setUniform(RendererUniform::FS_TEX_FLAGS, internalState.FSTexFlags.data());
    setUniform(RendererUniform::FS_MOVIE_FLAGS, internalState.FSMovieFlags.data());
    setUniform(RendererUniform::WM_FLAGS, internalState.WMFlags.data());
    setUniform(RendererUniform::TIME_COLOR, internalState.TimeColor.data());
    setUniform(RendererUniform::TIME_DATA, internalState.TimeData.data());

    setUniform(RendererUniform::D3D_VIEWPORT, internalState.d3dViewMatrix);
    setUniform(RendererUniform::D3D_PROJECTION, internalState.d3dProjectionMatrix);
    setUniform(RendererUniform::WORLD_VIEW, internalState.worldViewMatrix);
    setUniform(RendererUniform::NORMAL_MATRIX, internalState.normalMatrix);
    setUniform(RendererUniform::VIEW_MATRIX, internalState.viewMatrix);
    setUniform(RendererUniform::INV_VIEW_MATRIX, internalState.invViewMatrix);

    setUniform(RendererUniform::GAME_LIGHTING_FLAGS,  internalState.gameLightingFlags.data());
    setUniform(RendererUniform::GAME_GLOBAL_LIGHT_COLOR, internalState.gameGlobalLightColor);
    setUniform(RendererUniform::GAME_LIGHT_COLOR1, internalState.gameLightColor1);
    setUniform(RendererUniform::GAME_LIGHT_COLOR2, internalState.gameLightColor2);
    setUniform(RendererUniform::GAME_LIGHT_COLOR3, internalState.gameLightColor3);
    setUniform(RendererUniform::GAME_LIGHT_DIR1, internalState.gameLightDir1);
    setUniform(RendererUniform::GAME_LIGHT_DIR2, internalState.gameLightDir2);
    setUniform(RendererUniform::GAME_LIGHT_DIR3, internalState.gameLightDir3);
    setUniform(RendererUniform::GAME_SCRIPTED_LIGHT_COLOR, internalState.gameScriptedLightColor);
}

void Renderer::setLightingUniforms()
{
    auto lightingState = lighting.getLightingState();

    setUniform(RendererUniform::LIGHTING_SETTINGS, lightingState.lightingSettings);
    setUniform(RendererUniform::LIGHT_DIR_DATA, lightingState.lightDirData);
    setUniform(RendererUniform::LIGHT_DATA, lightingState.lightData);
    setUniform(RendererUniform::AMBIENT_LIGHT_DATA, lightingState.ambientLightData);
    setUniform(RendererUniform::SHADOW_DATA, lightingState.shadowData);
    setUniform(RendererUniform::FIELD_SHADOW_DATA, lightingState.fieldShadowData);
    setUniform(RendererUniform::MATERIAL_DATA, lightingState.materialData);
    setUniform(RendererUniform::MATERIAL_SCALE_DATA, lightingState.materialScaleData);
    setUniform(RendererUniform::LIGHTING_DEBUG_DATA, lightingState.lightingDebugData);
    setUniform(RendererUniform::IBL_DATA, lightingState.iblData);

    setUniform(RendererUniform::LIGHT_VIEW_PROJ_MATRIX, lightingState.lightViewProjMatrix);
    setUniform(RendererUniform::LIGHT_VIEW_PROJ_TEX_MATRIX, lightingState.lightViewProjTexMatrix);
    setUniform(RendererUniform::LIGHT_INV_VIEW_PROJ_TEX_MATRIX, lightingState.lightInvViewProjTexMatrix);
}

bgfx::RendererType::Enum Renderer::getUserChosenRenderer() {
    bgfx::RendererType::Enum ret;

    switch (renderer_backend)
    {
    case RENDERER_BACKEND_AUTO:
        ret = bgfx::RendererType::Count;
        break;
    case RENDERER_BACKEND_OPENGL:
        ret = bgfx::RendererType::OpenGL;
        break;
    case RENDERER_BACKEND_DIRECT3D11:
        ret = bgfx::RendererType::Direct3D11;
        break;
    case RENDERER_BACKEND_DIRECT3D12:
        ret = bgfx::RendererType::Direct3D12;
        break;
    case RENDERER_BACKEND_VULKAN:
        ret = bgfx::RendererType::Vulkan;
        break;
    default:
        ret = bgfx::RendererType::Noop;
        break;
    }

    return ret;
}

void Renderer::updateRendererShaderPaths()
{
    std::string shaderSuffix;

    switch (getCaps()->rendererType)
    {
    case bgfx::RendererType::OpenGL:
        currentRenderer = "OpenGL";
        shaderSuffix = ".gl";
        break;
    case bgfx::RendererType::Direct3D11:
        currentRenderer = "Direct3D11";
        shaderSuffix = ".d3d11";
        break;
    case bgfx::RendererType::Direct3D12:
        currentRenderer = "Direct3D12";
        shaderSuffix = ".d3d12";
        break;
    case bgfx::RendererType::Vulkan:
        currentRenderer = "Vulkan";
        shaderSuffix = ".vk";
        break;
    }

    vertexPathFlat += ".flat" + shaderSuffix + ".vert";
    fragmentPathFlat += ".flat" + shaderSuffix + ".frag";
    vertexPathSmooth += ".smooth" + shaderSuffix + ".vert";
    fragmentPathSmooth += ".smooth" + shaderSuffix + ".frag";
    vertexPostPath += ".smooth" + shaderSuffix + ".vert";
    fragmentPostPath += ".smooth" + shaderSuffix + ".frag";
    vertexOverlayPath += ".smooth" + shaderSuffix + ".vert";
    fragmentOverlayPath += ".smooth" + shaderSuffix + ".frag";
    vertexLightingPathFlat += ".flat" + shaderSuffix + ".vert";
    fragmentLightingPathFlat += ".flat" + shaderSuffix + ".frag";
    vertexLightingPathSmooth += ".smooth" + shaderSuffix + ".vert";
    fragmentLightingPathSmooth += ".smooth" + shaderSuffix + ".frag";
    vertexShadowMapPath += ".smooth" + shaderSuffix + ".vert";
    fragmentShadowMapPath += ".smooth" + shaderSuffix + ".frag";
    vertexFieldShadowPath += ".smooth" + shaderSuffix + ".vert";
    fragmentFieldShadowPath += ".smooth" + shaderSuffix + ".frag";
    vertexBlitPath += ".flat" + shaderSuffix + ".vert";
    fragmentBlitPath += ".flat" + shaderSuffix + ".frag";
}

// Via https://dev.to/pperon/hello-bgfx-4dka
bgfx::ShaderHandle Renderer::getShader(const char* filePath)
{
    bgfx::ShaderHandle handle = BGFX_INVALID_HANDLE;

    FILE* file = fopen(filePath, "rb");

    if (file == NULL)
    {
        char tmp[1024]{ 0 };

        sprintf(tmp, "Oops! Something very bad happened.\n\nCould not find shader file:\n%s\n\nMake sure all the provided files are installed correctly.", filePath);

        MessageBoxA(gameHwnd, tmp, "Error", MB_ICONERROR | MB_OK);

        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory* mem = bgfx::alloc(fileSize);
    fread(mem->data, 1, fileSize, file);
    fclose(file);

    handle = bgfx::createShader(mem);

    if (bgfx::isValid(handle))
    {
        bgfx::setName(handle, filePath);
    }

    return handle;
}

bgfx::UniformHandle Renderer::createUniform(std::string uniformName, bgfx::UniformType::Enum uniformType)
{
    bgfx::UniformHandle handle;
    handle = bgfx::createUniform(uniformName.c_str(), uniformType);
    return handle;
}

bgfx::UniformHandle Renderer::setUniform(RendererUniform uniform, const void* uniformValue)
{
    bgfx::UniformHandle handle = bgfxUniformHandles[uniform];

    if (bgfx::isValid(handle))
    {
        bgfx::setUniform(handle, uniformValue);
    }

    return handle;
}

void Renderer::destroyUniforms()
{
    for (const auto& handle : bgfxUniformHandles)
    {
        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }
}

void Renderer::destroyAll()
{
    destroyUniforms();

    for (auto& handle : internalState.texHandlers)
    {
        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }

    bgfx::destroy(vertexBufferHandle);

    bgfx::destroy(indexBufferHandle);

    bgfx::destroy(backendFrameBuffer);

    bgfx::destroy(shadowMapFrameBuffer);

    for (auto& handle : backendProgramHandles)
    {
        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }

    if (enable_devtools)
        overlay.destroy();
};

void Renderer::resetState()
{
    setBackgroundColor();

    doAlphaTest();
    doDepthTest();
    doDepthWrite();
    doScissorTest();
    setCullMode();
    setBlendMode();
    setAlphaRef();
    setInterpolationQualifier();
    isTLVertex();
    isYUV();
    isFullRange();
    isFBTexture();
    isTexture();
    doModulateAlpha();
    doTextureFiltering();
    isExternalTexture();
    setColorMatrix();
    setColorGamut();
    setOverallColorGamut(enable_ntscj_gamut_mode ? COLORGAMUT_NTSCJ : COLORGAMUT_SRGB);
    setGamutOverride();
    setGammaType();
    setGameLightData();

    doMirrorTextureWrap();
    setSphericalWorldRate();
    setFogEnabled();

    resetViewMatrixFlag();
};

void Renderer::renderFrame()
{
    /*  y0    y2
     x0 +-----+ x2
        |    /|
        |   / |
        |  /  |
        | /   |
        |/    |
     x1 +-----+ x3
        y1    y3
    */

    // 0
    float x0 = framebufferVertexOffsetX;
    if (aspect_ratio == AR_STRETCH) x0 = 0.0f;
    float y0 = 0.0f;
    float u0 = 0.0f;
    float v0 = getCaps()->originBottomLeft ? 1.0f : 0.0f;
    // 1
    float x1 = x0;
    float y1 = game_height;
    float u1 = u0;
    float v1 = getCaps()->originBottomLeft ? 0.0f : 1.0f;
    // 2
    float x2 = x0 + framebufferVertexWidth;
    if (aspect_ratio == AR_STRETCH) x2 = x0 + game_width;
    float y2 = y0;
    float u2 = 1.0f;
    float v2 = v0;
    // 3
    float x3 = x2;
    float y3 = y1;
    float u3 = u2;
    float v3 = v1;

    struct nvertex vertices[] = {
        {x0, y0, 1.0f, 1.0f, 0xff000000, 0, u0, v0},
        {x1, y1, 1.0f, 1.0f, 0xff000000, 0, u1, v1},
        {x2, y2, 1.0f, 1.0f, 0xff000000, 0, u2, v2},
        {x3, y3, 1.0f, 1.0f, 0xff000000, 0, u3, v3},
    };
    WORD indices[] = {
        0, 1, 2,
        1, 3, 2
    };

    backendProgram = RendererProgram::POSTPROCESSING;
    backendViewId++;
    {
        bool needsToDraw = internalState.bHasDrawBeenDone;

        if (internalState.bHasDrawBeenDone)
            useTexture(
                bgfx::getTexture(backendFrameBuffer).idx
            );
        else
            useTexture(0);

        setClearFlags(true, true);

        bindVertexBuffer(vertices, 0, 4);
        bindIndexBuffer(indices, 6);

        setBlendMode(RendererBlendMode::BLEND_DISABLED);
        setPrimitiveType();

        if (needsToDraw) draw();

        setBlendMode();
    }
};

void Renderer::printMatrix(char* name, float* mat)
{
    ffnx_trace("%s: 0 [%f, %f, %f, %f]\n", name, mat[0], mat[1], mat[2], mat[3]);
    ffnx_trace("%s: 1 [%f, %f, %f, %f]\n", name, mat[4], mat[5], mat[6], mat[7]);
    ffnx_trace("%s: 2 [%f, %f, %f, %f]\n", name, mat[8], mat[9], mat[10], mat[11]);
    ffnx_trace("%s: 3 [%f, %f, %f, %f]\n", name, mat[12], mat[13], mat[14], mat[15]);
};

bool Renderer::doesItFitInMemory(size_t size)
{
    if (size <= 0) ffnx_glitch("Unexpected texture size while checking if it fits in memory.\n");

    // We need to check this value as much as in real time as possible, to avoid possible crashes
    GlobalMemoryStatusEx(&last_ram_state);

    return size < last_ram_state.ullAvailVirtual;
}

void Renderer::recalcInternals()
{
    scalingFactor = internal_resolution_scale;

    viewWidth = window_size_x;
    viewHeight = window_size_y;

    // aspect correction
    switch(aspect_ratio){
        case AR_ORIGINAL:
            if (viewWidth * 3 != viewHeight * 4)
            {
                if (viewHeight * 4 > viewWidth * 3)
                {
                    viewOffsetY = viewHeight - (viewWidth * 3) / 4;
                    viewHeight = (viewWidth * 3) / 4;

                    y_offset = viewOffsetY;
                }
                else if (viewWidth * 3 > viewHeight * 4)
                {
                    viewOffsetX = (viewWidth - (viewHeight * 4) / 3) / 2;
                    viewWidth = (viewHeight * 4) / 3;

                    x_offset = viewOffsetX;
                }
            }
            break;
        case AR_WIDESCREEN_16X9:
            if (viewWidth * 9 != viewHeight * 16)
            {
                if (viewHeight * 16 > viewWidth * 9)
                {
                    viewOffsetY = viewHeight - (viewWidth * 9) / 16;
                    viewHeight = (viewWidth * 9) / 16;

                    y_offset = viewOffsetY;
                }
                else if (viewWidth * 9 > viewHeight * 16)
                {
                    viewOffsetX = (viewWidth - (viewHeight * 16) / 9) / 2;
                    viewWidth = (viewHeight * 16) / 9;

                    x_offset = viewOffsetX;
                }
            }
            break;
        case AR_WIDESCREEN_16X10:
            if (viewWidth * 10 != viewHeight * 16)
            {
                if (viewHeight * 16 > viewWidth * 10)
                {
                    viewOffsetY = viewHeight - (viewWidth * 10) / 16;
                    viewHeight = (viewWidth * 10) / 16;

                    y_offset = viewOffsetY;
                }
                else if (viewWidth * 10 > viewHeight * 16)
                {
                    viewOffsetX = (viewWidth - (viewHeight * 16) / 10) / 2;
                    viewWidth = (viewHeight * 16) / 10;

                    x_offset = viewOffsetX;
                }
            }
            break;
    }

    // If internal_resolution_scale from settings is less than one, calculate the closest fit for the output resolution, otherwise use the value directly
    if (internal_resolution_scale < 1)
    {
        long scaleW = ::round(viewWidth / (float)game_width);
        long scaleH = ::round(viewHeight / (float)game_height);

        if (scaleH > scaleW) scaleW = scaleH;
        if (scaleW > internal_resolution_scale) scalingFactor = scaleW;
        if (scalingFactor < 1) scalingFactor = 1;
    }

    // Use the set or calculated scaling factor to determine the width and height of the framebuffer according to the original resolution
    if(widescreen_enabled)
    {
        framebufferWidth = wide_game_width * scalingFactor;
        framebufferHeight = wide_game_height * scalingFactor;

        framebufferVertexWidth = (viewWidth * wide_game_width) / window_size_x;
        framebufferVertexOffsetX = (wide_game_width - framebufferVertexWidth) / 2;
    }
    else
    {
        framebufferWidth = game_width * scalingFactor;
        framebufferHeight = game_height * scalingFactor;

        framebufferVertexWidth = (viewWidth * game_width) / window_size_x;
        framebufferVertexOffsetX = (game_width - framebufferVertexWidth) / 2;
    }

    // Let the user know about chosen resolutions
    ffnx_info("Original resolution %ix%i, Scaling factor %ix, Internal resolution %ix%i, Output resolution %ix%i\n", game_width, game_height, scalingFactor, framebufferWidth, framebufferHeight, window_size_x, window_size_y);
}

void Renderer::calcBackendProjMatrix()
{
    // Used by the game
    bx::mtxOrtho(
        internalState.backendProjMatrix,
        widescreen_enabled ? wide_viewport_x : 0.0f,
        widescreen_enabled ? wide_viewport_width + wide_viewport_x : game_width,
        widescreen_enabled ? wide_game_height : game_height,
        0.0f,
        getCaps()->homogeneousDepth ? -1.0f : 0.0f,
        1.0f,
        0.0,
        getCaps()->homogeneousDepth
    );

    // Used by postprocessing ( rendering the game framebuffer on screen)
    bx::mtxOrtho(
        internalState.postprocessingProjMatrix,
        0.0f,
        widescreen_enabled ? wide_game_width : game_width,
        widescreen_enabled ? wide_game_height : game_height,
        0.0f,
        getCaps()->homogeneousDepth ? -1.0f : 0.0f,
        1.0f,
        0.0,
        getCaps()->homogeneousDepth
    );
}

void Renderer::prepareFramebuffer()
{
    // If already existing, destroy
    if (bgfx::isValid(backendFrameBuffer))
        bgfx::destroy(backendFrameBuffer);

    uint64_t fbFlags = BGFX_TEXTURE_RT;

    if (enable_antialiasing > 0)
    {
        if (enable_antialiasing <= 2)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X2;
        else if (enable_antialiasing <= 4)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X4;
        else if (enable_antialiasing <= 8)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X8;
        else if (enable_antialiasing <= 16)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X16;
    }

    backendFrameBufferRT[0] = bgfx::createTexture2D(
        framebufferWidth,
        framebufferHeight,
        false,
        1,
        internalState.bIsHDR ? bgfx::TextureFormat::RGB10A2 : bgfx::TextureFormat::RGBA16,
        fbFlags | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    backendFrameBufferRT[1] = bgfx::createTexture2D(
        framebufferWidth,
        framebufferHeight,
        false,
        1,
        bgfx::TextureFormat::D32F,
        fbFlags | BGFX_TEXTURE_RT_WRITE_ONLY
    );

    backendFrameBuffer = bgfx::createFrameBuffer(
        backendFrameBufferRT.size(),
        backendFrameBufferRT.data(),
        true
    );
}

void Renderer::bindTextures()
{
    if (!internalState.bTexturesBound)
    {
        for (uint32_t idx = RendererTextureSlot::TEX_Y; idx < RendererTextureSlot::COUNT; idx++)
        {
            bgfx::TextureHandle handle = internalState.texHandlers[idx];

            if (bgfx::isValid(handle))
            {
                uint32_t flags = 0;

                switch(idx)
                {
                    case RendererTextureSlot::TEX_Y:
                    case RendererTextureSlot::TEX_U:
                    case RendererTextureSlot::TEX_V:
                        if (!internalState.bIsMovie && idx > RendererTextureSlot::TEX_Y) handle = BGFX_INVALID_HANDLE;

                        if (backendProgram == RendererProgram::POSTPROCESSING)
                        {
                            flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
                        }
                        else
                        {
                            if (internalState.bDoMirrorTextureWrap) flags |= BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR | BGFX_SAMPLER_W_CLAMP;

                            if (internalState.bIsMovie) flags |= BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP;

                            if (!internalState.bDoTextureFiltering || !internalState.bIsExternalTexture) flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT;
                        }
                        break;
                    case RendererTextureSlot::TEX_S:
                    case RendererTextureSlot::TEX_D:
                        // Specially handled, move on
                        continue;
                    case RendererTextureSlot::TEX_G_LUT:
                        flags |= BGFX_SAMPLER_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP;
                        break;
                    default:
                        break;
                }

                if (flags == 0) flags = UINT32_MAX;

                bgfx::setTexture(idx, bgfxTexUniformHandles[idx], handle, flags);
            }
        }

        internalState.bTexturesBound = true;
    }
}

void Renderer::AssignGamutLUT()
{
	// Since HDR uses the super-wide rec2020 gamut, it doesn't need a gamut (compression) mapping algorithm,
	// so it would be better to use the old matrix-based conversions instead of the LUTs that embody a GMA.
	// That's what we do in post-processing.
	// However, in two cases we have two serial conversions happening:
	// (1) Movies where the movie's gamut isn't the same as the selected gamut mode
	// (2) The implemented but as-yet unused internalState.bIsOverrideGamut
	// What I'd *like* to do is to do matrix-based conversions for the first round,
	// then *tolerate* the out-of-bounds values until post processing,
	// then the final conversion to rec2020 will bring those values back in bounds.
	// Unfortunately, I don't think the lighting code could tolerate out-of-bounds values.
	// (Also, the srgb gamma function would need to be changed to avoid calling pow() on a negative input.)
	// So, for now I'm using the LUTs for the first step for both SDR and HDR,
	// and I'm putting this comment here in case we ever figure out how to tolerate out-of-bounds values
	//if (internalState.bIsHDR) return;


	// NTSCJ mode post-processing
	if ((backendProgram == RendererProgram::POSTPROCESSING) && (internalState.bIsOverallColorGamut == COLORGAMUT_NTSCJ)){
		LoadGamutLUT(INDEX_LUT_NTSCJ_TO_SRGB); // load LUT if not already loaded
		useTexture(GLUTHandleNTSCJtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
	}
	// Movies and override flag
	// Note: Override flag currently does nothing because it's never set to true anywhere
	// The intent is to eventually have a way to say "I want to display a NTSCJ asset in sRGB mode" or "I want to display a sRGB asset in NTSCJ mode."
	else {
		if (internalState.bIsOverallColorGamut == COLORGAMUT_SRGB){
			if ((internalState.bIsMovieColorGamut == COLORGAMUT_NTSCJ) || internalState.bIsOverrideGamut){
				LoadGamutLUT(INDEX_LUT_NTSCJ_TO_SRGB); // load LUT if not already loaded
				useTexture(GLUTHandleNTSCJtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
			}
			else if (internalState.bIsMovieColorGamut == COLORGAMUT_SMPTEC){
				LoadGamutLUT(INDEX_LUT_SMPTEC_TO_SRGB); // load LUT if not already loaded
				useTexture(GLUTHandleSMPTECtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
			}
			else if (internalState.bIsMovieColorGamut == COLORGAMUT_EBU){
				LoadGamutLUT(INDEX_LUT_EBU_TO_SRGB); // load LUT if not already loaded
				useTexture(GLUTHandleEBUtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
			}
		}
		else if (internalState.bIsOverallColorGamut == COLORGAMUT_NTSCJ){
			// SDR should use the "inverse" conversions created with gamutthingy's "expand" flag, to compensate for the compression later,
			// but HDR should not because the conversion to rec2020 doesn't involve compression
			// This isn't exactly kosher for the SMPTEC and EBU cases, but they're close enough to sRGB
			// that doing the expansion probably gives closer to accurate results than not doing it.
			if (internalState.bIsHDR){
				if ((internalState.bIsMovieColorGamut == COLORGAMUT_SRGB) || internalState.bIsOverrideGamut){
					LoadGamutLUT(INDEX_LUT_SRGB_TO_NTSCJ); // load LUT if not already loaded
					useTexture(GLUTHandleSRGBtoNTSCJ.idx, RendererTextureSlot::TEX_G_LUT);
				}
				else if (internalState.bIsMovieColorGamut == COLORGAMUT_SMPTEC){
					LoadGamutLUT(INDEX_LUT_SMPTEC_TO_NTSCJ); // load LUT if not already loaded
					useTexture(GLUTHandleSMPTECtoNTSCJ.idx, RendererTextureSlot::TEX_G_LUT);
				}
				else if (internalState.bIsMovieColorGamut == COLORGAMUT_EBU){
					LoadGamutLUT(INDEX_LUT_EBU_TO_NTSCJ); // load LUT if not already loaded
					useTexture(GLUTHandleEBUtoNTSCJ.idx, RendererTextureSlot::TEX_G_LUT);
				}
			}
			else{
				if ((internalState.bIsMovieColorGamut == COLORGAMUT_SRGB) || internalState.bIsOverrideGamut){
					LoadGamutLUT(INDEX_LUT_INVERSE_NTSCJ_TO_SRGB); // load LUT if not already loaded
					useTexture(GLUTHandleInverseNTSCJtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
				}
				else if (internalState.bIsMovieColorGamut == COLORGAMUT_SMPTEC){
					LoadGamutLUT(INDEX_LUT_INVERSE_NTSCJ_TO_SMPTEC); // load LUT if not already loaded
					useTexture(GLUTHandleInverseNTSCJtoSMPTEC.idx, RendererTextureSlot::TEX_G_LUT);
				}
				else if (internalState.bIsMovieColorGamut == COLORGAMUT_EBU){
					LoadGamutLUT(INDEX_LUT_INVERSE_NTSCJ_TO_EBU); // load LUT if not already loaded
					useTexture(GLUTHandleInverseNTSCJtoEBU.idx, RendererTextureSlot::TEX_G_LUT);
				}
			}
		}

	}
	return;
}

// PUBLIC

void Renderer::init()
{
    recalcInternals();

    // Init renderer
    bgfxInit.platformData.nwh = gameHwnd;
    bgfxInit.type = getUserChosenRenderer();
    bgfxInit.resolution.width = window_size_x;
    bgfxInit.resolution.height = window_size_y;

    if (enable_anisotropic)
        bgfxInit.resolution.reset |= BGFX_RESET_MAXANISOTROPY;

    if (enable_vsync)
        bgfxInit.resolution.reset |= BGFX_RESET_VSYNC;

    bgfxInit.debug = renderer_debug;
    bgfxInit.callback = &bgfxCallbacks;

    if (!bgfx::init(bgfxInit)) exit(1);

    // If HDR support is present, make use of it
    if (getCaps()->supported & BGFX_CAPS_HDR10)
    {
        internalState.bIsHDR = true;
        ffnx_info("HDR monitor detected. HDR enabled.\n");

        bgfxInit.resolution.reset |= BGFX_RESET_HDR10;
        bgfxInit.resolution.format = bgfx::TextureFormat::RGB10A2;

        if (hdr_max_nits <= 0) {
            ffnx_info("Attempting to autodetect SDR white level for HDR...\n");
            // The goal here is to autodetect "the brightness level that SDR 'white' is rendered at within an HDR monitor."
            // See: https://www.pyromuffin.com/2018/07/how-to-do-nothing-in-hdr.html
            // See also: https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-displayconfig_sdr_white_level
            // Autodetection may fail on some monitor models.
            // Autodetection WILL fail on Windows older than Win10 Fall Creators Update (Version 1709) and WILL fail on WINE (at least as of WINE 7.22)

            // First, enumerate the DISPLAYCONFIG_PATH_INFO for all monitors.
            // Code borrowed from Microsoft's example: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-querydisplayconfig#examples
            std::vector<DISPLAYCONFIG_PATH_INFO> paths;
            std::vector<DISPLAYCONFIG_MODE_INFO> modes;
            UINT32 flags = QDC_ONLY_ACTIVE_PATHS; // | QDC_VIRTUAL_MODE_AWARE; // MS's example is wrong. Causes bad param error on WINE. See: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdisplayconfigbuffersizes
            LONG monresult = ERROR_SUCCESS;
            bool gotbuffersizes = false;
            bool usedefaultnits = true;
            do {
                // Determine how many path and mode structures to allocate
                UINT32 pathCount, modeCount;
                monresult = GetDisplayConfigBufferSizes(flags, &pathCount, &modeCount);

                if (monresult != ERROR_SUCCESS){
                    if (trace_all || trace_renderer) ffnx_trace("Renderer::init(): Autodetection of SDR white level for HDR failed. Cannot enumerate monitors. GetDisplayConfigBufferSizes() failed with error code %i.\n", monresult);
                    break;
                }
                gotbuffersizes = true;

                // Allocate the path and mode arrays
                paths.resize(pathCount);
                modes.resize(modeCount);

                // Get all active paths and their modes
                monresult = QueryDisplayConfig(flags, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);

                // The function may have returned fewer paths/modes than estimated
                paths.resize(pathCount);
                modes.resize(modeCount);

                // It's possible that between the call to GetDisplayConfigBufferSizes and QueryDisplayConfig
                // that the display state changed, so loop on the case of ERROR_INSUFFICIENT_BUFFER.
            } while (monresult == ERROR_INSUFFICIENT_BUFFER);

            if (monresult != ERROR_SUCCESS){
                if (gotbuffersizes){
                    if (trace_all || trace_renderer) ffnx_trace("Renderer::init(): Autodetection of SDR white level for HDR failed. Cannot enumerate monitors. QueryDisplayConfig() failed with error code %i.\n", monresult);
                }
            }
            else {
                // loop over monitors and take the SDR white level of the first HDR monitor we find.
                int pathidx = -1;
                for (auto& path : paths) {
                    pathidx++;

                    // Check if this monitor is HDR capable and HDR enabled
                    // Borrowed from these two examples:
                    // https://forum.doom9.org/showthread.php?s=33dc0ad84a0997fce56710b3959e0415&p=1897630#post1897630
                    // https://stackoverflow.com/a/66160049
                    DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO getColorInfo = {};
                    getColorInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
                    getColorInfo.header.id = path.targetInfo.id;
                    getColorInfo.header.adapterId = path.targetInfo.adapterId;
                    getColorInfo.header.size = sizeof(getColorInfo);
                    monresult = DisplayConfigGetDeviceInfo(&getColorInfo.header);
                    if (monresult != ERROR_SUCCESS){
                        if (trace_all || trace_renderer) ffnx_trace("Renderer::init(): Autodetection of SDR white level for HDR skipping monitor #%i because DisplayConfigGetDeviceInfo() DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO check failed with error code %i.\n", pathidx, monresult);
                        continue;
                    }
                    if (!getColorInfo.advancedColorSupported || !getColorInfo.advancedColorEnabled){
                        if (trace_all || trace_renderer) ffnx_trace("Renderer::init(): Autodetection of SDR white level for HDR determined that monitor #%i is not an HDR monitor or HDR is not enabled. Checking next monitor (if any)...\n", pathidx);
                        continue;
                    }

                    // If we found an HDR monitor, then query its SDR white level
                    // Code borrowed from Google Chrome: https://chromium.googlesource.com/chromium/src/+/c71f15ab1ace78c7efeeeda9f8552b4af9db2877/ui/display/win/screen_win.cc#112
                    DISPLAYCONFIG_SDR_WHITE_LEVEL white_level = {};
                    white_level.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
                    white_level.header.id = path.targetInfo.id;
                    white_level.header.adapterId = path.targetInfo.adapterId;
                    white_level.header.size = sizeof(white_level);
                    monresult = DisplayConfigGetDeviceInfo(&white_level.header);
                    if (monresult != ERROR_SUCCESS){
                        if (trace_all || trace_renderer) ffnx_trace("Renderer::init(): Autodetection of SDR white level for HDR failed. Monitor #%i appears to be an HDR monitor, but DisplayConfigGetDeviceInfo() DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL check failed with error code %i.\n", pathidx, monresult);
                        break;
                    }
                    if (white_level.SDRWhiteLevel == 0){
                        if (trace_all || trace_renderer) ffnx_trace("Renderer::init(): Autodetection of SDR white level for HDR failed. Monitor #%i appears to be an HDR monitor, but SDR white level is reported as 0.\n", pathidx);
                        break;
                    }
                    // SDRWhiteLevel is stored in units of 2/25 nits (but with steps of 80 nits), because Microsoft.
                    // See: https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-displayconfig_sdr_white_level
                    hdr_max_nits = white_level.SDRWhiteLevel * 80.0 / 1000.0;
                    ffnx_info("SDR white level for HDR successfully autodetected as %i nits.\n", (int)hdr_max_nits);
                    usedefaultnits = false;
                    break;

                } // end for (auto& path : paths)

            } // end else (monresult == ERROR_SUCCESS)
            if (usedefaultnits){
                ffnx_info("Autodetection of SDR white level for HDR failed. Assuming default value of 200 nits.\n");
                // Google thinks 200 nits is a good default. Who are we to argue?
                hdr_max_nits = 200.0;
            }
        } // end if  (hdr_max_nits <= 0)

        bgfx::reset(window_size_x, window_size_y, bgfxInit.resolution.reset, bgfxInit.resolution.format);
    } //end if HDR

    internalState.texHandlers.resize(RendererTextureSlot::COUNT, BGFX_INVALID_HANDLE);

    updateRendererShaderPaths();

    calcBackendProjMatrix();

    prepareFramebuffer();

    prepareShadowMap();

    // Create Program
    backendProgramHandles[RendererProgram::POSTPROCESSING] = bgfx::createProgram(
        getShader(vertexPostPath.c_str()),
        getShader(fragmentPostPath.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::FLAT] = bgfx::createProgram(
        getShader(vertexPathFlat.c_str()),
        getShader(fragmentPathFlat.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::SMOOTH] = bgfx::createProgram(
        getShader(vertexPathSmooth.c_str()),
        getShader(fragmentPathSmooth.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::LIGHTING_FLAT] = bgfx::createProgram(
        getShader(vertexLightingPathFlat.c_str()),
        getShader(fragmentLightingPathFlat.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::LIGHTING_SMOOTH] = bgfx::createProgram(
        getShader(vertexLightingPathSmooth.c_str()),
        getShader(fragmentLightingPathSmooth.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::FIELD_SHADOW] = bgfx::createProgram(
        getShader(vertexFieldShadowPath.c_str()),
        getShader(fragmentFieldShadowPath.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::SHADOW_MAP] = bgfx::createProgram(
        getShader(vertexShadowMapPath.c_str()),
        getShader(fragmentShadowMapPath.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::BLIT] = bgfx::createProgram(
        getShader(vertexBlitPath.c_str()),
        getShader(fragmentBlitPath.c_str()),
        true
    );

    vertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .end();

    bgfx::setDebug(BGFX_DEBUG_TEXT);

    bgfx::frame();

    if (enable_devtools)
    {
        backendProgramHandles[RendererProgram::OVERLAY] = bgfx::createProgram(
            getShader(vertexOverlayPath.c_str()),
            getShader(fragmentOverlayPath.c_str()),
            true
        );
        overlay.init(backendProgramHandles[RendererProgram::OVERLAY], window_size_x, window_size_y);
    }

    bgfxUniformHandles[RendererUniform::VS_FLAGS] = createUniform("VSFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::FS_ALPHA_FLAGS] = createUniform("FSAlphaFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::FS_MISC_FLAGS] = createUniform("FSMiscFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::FS_HDR_FLAGS] = createUniform("FSHDRFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::FS_TEX_FLAGS] = createUniform("FSTexFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::FS_MOVIE_FLAGS] = createUniform("FSMovieFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::WM_FLAGS] = createUniform("WMFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::TIME_COLOR] = createUniform("TimeColor", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::TIME_DATA] = createUniform("TimeData", bgfx::UniformType::Vec4);

    bgfxUniformHandles[RendererUniform::D3D_VIEWPORT] = createUniform("d3dViewport", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::D3D_PROJECTION] = createUniform("d3dProjection", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::WORLD_VIEW] = createUniform("worldView", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::NORMAL_MATRIX] = createUniform("normalMatrix", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::VIEW_MATRIX] = createUniform("viewMatrix", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::INV_VIEW_MATRIX] = createUniform("invViewMatrix", bgfx::UniformType::Mat4);

    bgfxUniformHandles[RendererUniform::LIGHTING_SETTINGS] = createUniform("lightingSettings", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::LIGHT_DIR_DATA] = createUniform("lightDirData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::LIGHT_DATA] = createUniform("lightData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::AMBIENT_LIGHT_DATA] = createUniform("ambientLightData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::SHADOW_DATA] = createUniform("shadowData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::FIELD_SHADOW_DATA] = createUniform("fieldShadowData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::MATERIAL_DATA] = createUniform("materialData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::MATERIAL_SCALE_DATA] = createUniform("materialScaleData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::LIGHTING_DEBUG_DATA] = createUniform("lightingDebugData", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::IBL_DATA] = createUniform("iblData", bgfx::UniformType::Vec4);

    bgfxUniformHandles[RendererUniform::LIGHT_VIEW_PROJ_MATRIX] = createUniform("lightViewProjMatrix", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::LIGHT_VIEW_PROJ_TEX_MATRIX] = createUniform("lightViewProjTexMatrix", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::LIGHT_INV_VIEW_PROJ_TEX_MATRIX] = createUniform("lightInvViewProjTexMatrix", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::VIEW_OFFSET_MATRIX] = createUniform("viewOffsetMatrix", bgfx::UniformType::Mat4);
    bgfxUniformHandles[RendererUniform::INV_VIEW_OFFSET_MATRIX] = createUniform("invViewOffsetMatrix", bgfx::UniformType::Mat4);

    bgfxUniformHandles[RendererUniform::GAME_LIGHTING_FLAGS] = createUniform("gameLightingFlags", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_GLOBAL_LIGHT_COLOR] = createUniform("gameGlobalLightColor", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_LIGHT_COLOR1] = createUniform("gameLightColor1", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_LIGHT_COLOR2] = createUniform("gameLightColor2", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_LIGHT_COLOR3] = createUniform("gameLightColor3", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_LIGHT_DIR1] = createUniform("gameLightDir1", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_LIGHT_DIR2] = createUniform("gameLightDir2", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_LIGHT_DIR3] = createUniform("gameLightDir3", bgfx::UniformType::Vec4);
    bgfxUniformHandles[RendererUniform::GAME_SCRIPTED_LIGHT_COLOR] = createUniform("gameScriptedLightColor", bgfx::UniformType::Vec4);

    for(int i = 0; i < RendererTextureSlot::COUNT; ++i)
    {
        bgfxTexUniformHandles[i] = createUniform("tex_" + std::to_string(i), bgfx::UniformType::Sampler);
    }

    // Set defaults
    show();
};

void Renderer::reset()
{
    recalcInternals();

    calcBackendProjMatrix();

    prepareFramebuffer();

    if(!ff8 && enable_lighting) prepareShadowMap();

    bgfx::reset(window_size_x, window_size_y, bgfxInit.resolution.reset, bgfxInit.resolution.format);
}

#define FFNX_LOGO_PATH ".logo/logo_nobg.png"

void Renderer::prepareFFNxLogo()
{
    if (bgfx::isValid(FFNxLogoHandle))
        bgfx::destroy(FFNxLogoHandle);

    auto fs = cmrc::FFNx::get_filesystem();
    auto logo = fs.open(FFNX_LOGO_PATH);

    uint32_t width, height, mipCount = 0;
    FFNxLogoHandle = createTextureHandle(&logo, FFNX_LOGO_PATH, &width, &height, &mipCount, true);
    if (!FFNxLogoHandle.idx) FFNxLogoHandle = BGFX_INVALID_HANDLE;
}

void Renderer::prepareShadowMap()
{
    if (bgfx::isValid(shadowMapFrameBuffer))
        bgfx::destroy(shadowMapFrameBuffer);

    auto shadowMapResolution = lighting.getShadowMapResolution();
    shadowMapTexture = bgfx::createTexture2D(
        shadowMapResolution,
        shadowMapResolution,
        false,
        1,
        bgfx::TextureFormat::D32F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP
    );

    shadowMapFrameBuffer = bgfx::createFrameBuffer(
        1,
        &shadowMapTexture,
        true
    );
}

void Renderer::prepareSpecularIbl(char* fullpath)
{
    if (bgfx::isValid(specularIblTexture))
        bgfx::destroy(specularIblTexture);

    uint32_t width, height, mipCount = 0;
    specularIblTexture = createTextureHandle(fullpath, &width, &height, &mipCount, false);
    if (!specularIblTexture.idx) specularIblTexture = BGFX_INVALID_HANDLE;
    lighting.setIblMipCount(mipCount);
}

void Renderer::prepareDiffuseIbl(char* fullpath)
{
    if (bgfx::isValid(diffuseIblTexture))
        bgfx::destroy(diffuseIblTexture);

    uint32_t width, height, mipCount = 0;
    diffuseIblTexture = createTextureHandle(fullpath, &width, &height, &mipCount, false);
    if (!diffuseIblTexture.idx) diffuseIblTexture = BGFX_INVALID_HANDLE;
}

void Renderer::prepareEnvBrdf()
{
    static char fullpath[MAX_PATH];

    if (bgfx::isValid(envBrdfTexture))
        bgfx::destroy(envBrdfTexture);

    sprintf(fullpath, "%s/%s/ibl/envBrdf.dds", basedir, external_lighting_path.c_str());

    uint32_t width, height, mipCount = 0;
    envBrdfTexture = createTextureHandle(fullpath, &width, &height, &mipCount, false);
    if (!envBrdfTexture.idx) envBrdfTexture = BGFX_INVALID_HANDLE;
}

void Renderer::prepareGamutLUTs()
{

	// flush everything (they should have all initialized to BGFX_INVALID_HANDLE, but let's be careful)
	if (bgfx::isValid(GLUTHandleNTSCJtoSRGB))
		bgfx::destroy(GLUTHandleNTSCJtoSRGB);
	if (bgfx::isValid(GLUTHandleSMPTECtoSRGB))
		bgfx::destroy(GLUTHandleSMPTECtoSRGB);
	if (bgfx::isValid(GLUTHandleEBUtoSRGB))
		bgfx::destroy(GLUTHandleEBUtoSRGB);
	if (bgfx::isValid(GLUTHandleInverseNTSCJtoSRGB))
		bgfx::destroy(GLUTHandleInverseNTSCJtoSRGB);
	if (bgfx::isValid(GLUTHandleInverseNTSCJtoSMPTEC))
		bgfx::destroy(GLUTHandleInverseNTSCJtoSMPTEC);
	if (bgfx::isValid(GLUTHandleInverseNTSCJtoEBU))
		bgfx::destroy(GLUTHandleInverseNTSCJtoEBU);
	if (bgfx::isValid(GLUTHandleSRGBtoNTSCJ))
		bgfx::destroy(GLUTHandleSRGBtoNTSCJ);
	if (bgfx::isValid(GLUTHandleSMPTECtoNTSCJ))
		bgfx::destroy(GLUTHandleSMPTECtoNTSCJ);
	if (bgfx::isValid(GLUTHandleEBUtoNTSCJ))
		bgfx::destroy(GLUTHandleEBUtoNTSCJ);

	// load only the LUTs we are likely to need
	// consult the global setting so we don't get tripped up by renderer state changing to accomodate the FFNx logo
	if (enable_ntscj_gamut_mode){
		if (internalState.bIsHDR){
			// Final NTSC-J to rec2020 conversion will be handled by matrix math in the shader (no gamut compression mapping needed)
			// We will probably have some sRGB videos that must go sRGB -> NTSC-J -> rec2020.
			// Use the compress-only (non "inverse") LUT because the final step for HDR won't invert expansions
			LoadGamutLUT(INDEX_LUT_SRGB_TO_NTSCJ);
		}
		// SDR
		else {
			// Final NTSC-J to sRGB conversion needs gamut compression mapping LUT
			LoadGamutLUT(INDEX_LUT_NTSCJ_TO_SRGB);
			// We will probably have some sRGB videos that must go sRGB -> NTSC-J -> sRGB.
			// Use expanding "inverse" LUT to counteract the compression in the final conversion.
			LoadGamutLUT(INDEX_LUT_INVERSE_NTSCJ_TO_SRGB);
		}
	}
	// Most FF7 movies will need NTSC-J to sRGB conversion for sRGB mode
	// (FF8 Steam edition movies were already converted)
	else if(!ff8){
		LoadGamutLUT(INDEX_LUT_NTSCJ_TO_SRGB);
	}

	// Any other LUTs we end up needing will be lazy loaded by AssignGamutLUT()

	return;
}

void Renderer::LoadGamutLUT(GamutLUTIndexType whichLUT)
{

	static char fullpath[MAX_PATH];
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t mipCount = 0;

	// Note: It's important that the final parameter to createTextureHandle() -- isSrgb -- is false.
	// Otherwise the sRGB gamma function will be applied to convert sRGB to linear RGB.
	// But we don't want that because these LUTs are already in linear RGB.

	switch (whichLUT){
		case INDEX_LUT_NTSCJ_TO_SRGB:
			if (!bgfx::isValid(GLUTHandleNTSCJtoSRGB)){
				sprintf(fullpath, "%s/shaders/glut_ntscj_to_srgb.png", basedir);
				GLUTHandleNTSCJtoSRGB = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleNTSCJtoSRGB.idx) GLUTHandleNTSCJtoSRGB = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_SMPTEC_TO_SRGB:
			if (!bgfx::isValid(GLUTHandleSMPTECtoSRGB)){
				sprintf(fullpath, "%s/shaders/glut_smptec_to_srgb.png", basedir);
				GLUTHandleSMPTECtoSRGB = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleSMPTECtoSRGB.idx) GLUTHandleSMPTECtoSRGB = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_EBU_TO_SRGB:
			if (!bgfx::isValid(GLUTHandleEBUtoSRGB)){
				sprintf(fullpath, "%s/shaders/glut_ebu_to_srgb.png", basedir);
				GLUTHandleEBUtoSRGB = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleEBUtoSRGB.idx) GLUTHandleEBUtoSRGB = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_INVERSE_NTSCJ_TO_SRGB:
			if (!bgfx::isValid(GLUTHandleInverseNTSCJtoSRGB)){
				sprintf(fullpath, "%s/shaders/glut_inverse_ntscj_to_srgb.png", basedir);
				GLUTHandleInverseNTSCJtoSRGB = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleInverseNTSCJtoSRGB.idx) GLUTHandleInverseNTSCJtoSRGB = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_INVERSE_NTSCJ_TO_SMPTEC:
			if (!bgfx::isValid(GLUTHandleInverseNTSCJtoSMPTEC)){
				sprintf(fullpath, "%s/shaders/glut_inverse_ntscj_to_smptec.png", basedir);
				GLUTHandleInverseNTSCJtoSMPTEC = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleInverseNTSCJtoSMPTEC.idx) GLUTHandleInverseNTSCJtoSMPTEC = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_INVERSE_NTSCJ_TO_EBU:
			if (!bgfx::isValid(GLUTHandleInverseNTSCJtoEBU)){
				sprintf(fullpath, "%s/shaders/glut_inverse_ntscj_to_ebu.png", basedir);
				GLUTHandleInverseNTSCJtoEBU = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleInverseNTSCJtoEBU.idx) GLUTHandleInverseNTSCJtoEBU = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_SRGB_TO_NTSCJ:
			if (!bgfx::isValid(GLUTHandleSRGBtoNTSCJ)){
				sprintf(fullpath, "%s/shaders/glut_srgb_to_ntscj.png", basedir);
				GLUTHandleSRGBtoNTSCJ = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleSRGBtoNTSCJ.idx) GLUTHandleSRGBtoNTSCJ = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_SMPTEC_TO_NTSCJ:
			if (!bgfx::isValid(GLUTHandleSMPTECtoNTSCJ)){
				sprintf(fullpath, "%s/shaders/glut_smptec_to_ntscj.png", basedir);
				GLUTHandleSMPTECtoNTSCJ = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleSMPTECtoNTSCJ.idx) GLUTHandleSMPTECtoNTSCJ = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_EBU_TO_NTSCJ:
			if (!bgfx::isValid(GLUTHandleEBUtoNTSCJ)){
				sprintf(fullpath, "%s/shaders/glut_ebu_to_ntscj.png", basedir);
				GLUTHandleEBUtoNTSCJ = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleEBUtoNTSCJ.idx) GLUTHandleEBUtoNTSCJ = BGFX_INVALID_HANDLE;
			}
			break;
		default:
			ffnx_error("LoadGamutLUT: called with invalid index: %i\n", whichLUT);
			break;
	}
	return;
}

void Renderer::shutdown()
{
    destroyAll();

    bgfx::shutdown();
}

void Renderer::clearShadowMap()
{
    bgfx::setViewClear(0, BGFX_CLEAR_DEPTH, internalState.clearColorValue, 1.0f, 0);
    bgfx::touch(0);
}

void Renderer::drawToShadowMap(bool uniformsAlreadyAttached, bool texturesAlreadyAttached)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s with backendProgram %d\n", __func__, backendProgram);

    // Lighting state
    auto lightingState = lighting.getLightingState();

    // Set view to render in the framebuffer
    bgfx::setViewFrameBuffer(0, shadowMapFrameBuffer);

    // Set current view rect
    int shadowMapResolution = lighting.getShadowMapResolution();
    bgfx::setViewRect(0, 0, 0, shadowMapResolution, shadowMapResolution);

    // Set current view transform
    bgfx::setViewTransform(0, lightingState.lightViewMatrix, lightingState.lightProjMatrix);

    // Set uniforms
    if(!uniformsAlreadyAttached)
    {
        setLightingUniforms();
        setCommonUniforms();
    }

    // Bind textures in pipeline
    if (!texturesAlreadyAttached)
    {
        bindTextures();
    }

    // Set state
    internalState.state = BGFX_STATE_DEPTH_TEST_LEQUAL | BGFX_STATE_WRITE_Z;

    // Face culling
    if (lighting.isShadowFaceCullingEnabled())
    {
        switch (internalState.cullMode)
        {
        case RendererCullMode::FRONT: internalState.state |= BGFX_STATE_CULL_CCW;
        case RendererCullMode::BACK: internalState.state |= BGFX_STATE_CULL_CW;
        }
    }
    bgfx::setState(internalState.state);

    bgfx::submit(0, backendProgramHandles[RendererProgram::SHADOW_MAP], 0, BGFX_DISCARD_NONE);
};

void Renderer::drawWithLighting(bool uniformsAlreadyAttached, bool texturesAlreadyAttached, bool keepBindings)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s with backendProgram %d\n", __func__, backendProgram);

    // Set lighting program
    backendProgram = backendProgram == SMOOTH ? LIGHTING_SMOOTH : LIGHTING_FLAT;

    // Re-Bind shadow map with comparison sampler
    bgfx::setTexture(RendererTextureSlot::TEX_S, bgfxTexUniformHandles[RendererTextureSlot::TEX_S], bgfx::getTexture(shadowMapFrameBuffer));

    // Bind specular IBL cubemap
    if (bgfx::isValid(specularIblTexture))
    {
        bgfx::setTexture(RendererTextureSlot::TEX_IBL_SPEC, bgfxTexUniformHandles[RendererTextureSlot::TEX_IBL_SPEC], specularIblTexture);
    }

    // Bind diffuse IBL cubemap
    if (bgfx::isValid(diffuseIblTexture))
    {
        bgfx::setTexture(RendererTextureSlot::TEX_IBL_DIFF, bgfxTexUniformHandles[RendererTextureSlot::TEX_IBL_DIFF], diffuseIblTexture);
    }

    // Bind environment BRDF texture
    if (bgfx::isValid(envBrdfTexture))
    {
        bgfx::setTexture(RendererTextureSlot::TEX_BRDF, bgfxTexUniformHandles[RendererTextureSlot::TEX_BRDF], envBrdfTexture, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    }

    // Draw with lighting
    draw(uniformsAlreadyAttached, texturesAlreadyAttached, keepBindings);
}

void Renderer::drawFieldShadow()
{
    backendProgram = RendererProgram::FIELD_SHADOW;

    // Re-Bind shadow map with comparison sampler
    bgfx::setTexture(RendererTextureSlot::TEX_S, bgfxTexUniformHandles[RendererTextureSlot::TEX_S], bgfx::getTexture(shadowMapFrameBuffer));

    // Re-Bind shadow map for direct depth sampling
    bgfx::setTexture(RendererTextureSlot::TEX_D, bgfxTexUniformHandles[RendererTextureSlot::TEX_D], bgfx::getTexture(shadowMapFrameBuffer), BGFX_TEXTURE_RT | BGFX_SAMPLER_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP);

    draw();
}

void Renderer::draw(bool uniformsAlreadyAttached, bool texturesAlreadyAttached, bool keepBindings)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s with backendProgram %d\n", __func__, backendProgram);

    // Set current view rect
    if (backendProgram == RendererProgram::POSTPROCESSING)
    {
        bgfx::setViewRect(backendViewId, 0, 0, window_size_x, window_size_y);

        // Set current view transform
        bgfx::setViewTransform(backendViewId, NULL, internalState.postprocessingProjMatrix);
    }
    else
    {
        // Set view to render in the framebuffer
        bgfx::setViewFrameBuffer(backendViewId, backendFrameBuffer);

        bgfx::setViewRect(backendViewId, 0, 0, framebufferWidth, framebufferHeight);

        if (internalState.bDoScissorTest) bgfx::setScissor(scissorOffsetX, scissorOffsetY, scissorWidth, scissorHeight);

        // Set current view transform
        bgfx::setViewTransform(backendViewId, NULL, internalState.backendProjMatrix);
    }

    // Skip uniform attachment as it has been done already
    if (!uniformsAlreadyAttached)
    {
        setCommonUniforms();
        setLightingUniforms();
    }

    // set up a gamut LUT if we need one
    AssignGamutLUT();

    // Bind textures in pipeline
    if (!texturesAlreadyAttached)
    {
        bindTextures();
    }

    // Set state
    {
        internalState.state = BGFX_STATE_LINEAA | BGFX_STATE_MSAA | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

        switch (internalState.cullMode)
        {
        case RendererCullMode::FRONT: internalState.state |= BGFX_STATE_CULL_CW;
        case RendererCullMode::BACK: internalState.state |= BGFX_STATE_CULL_CCW;
        }

        switch (internalState.blendMode)
        {
        case RendererBlendMode::BLEND_AVG:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
            break;
        case RendererBlendMode::BLEND_ADD:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            break;
        case RendererBlendMode::BLEND_SUB:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_REVSUB);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            break;
        case RendererBlendMode::BLEND_25P:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE);
            break;
        case RendererBlendMode::BLEND_NONE:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            if (internalState.bIsExternalTexture && !ff8) internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
            else internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ZERO);
            break;
        }

        switch (internalState.primitiveType)
        {
        case RendererPrimitiveType::PT_LINES:
            internalState.state |= BGFX_STATE_PT_LINES;
            break;
        case RendererPrimitiveType::PT_POINTS:
            internalState.state |= BGFX_STATE_PT_POINTS;
            break;
        }

        if (internalState.bDoDepthTest) internalState.state |= BGFX_STATE_DEPTH_TEST_LEQUAL;

        if (internalState.bDoDepthWrite) internalState.state |= BGFX_STATE_WRITE_Z;
    }
    bgfx::setState(internalState.state);

    auto flags = keepBindings ? BGFX_DISCARD_STATE : BGFX_DISCARD_ALL;
    bgfx::submit(backendViewId, backendProgramHandles[backendProgram], 0, flags);

    internalState.bHasDrawBeenDone = true;
    internalState.bTexturesBound = false;
};

void Renderer::discardAllBindings()
{
    bgfx::discard(BGFX_DISCARD_ALL);
}

void Renderer::drawOverlay()
{
    if (enable_devtools)
        overlay.draw();
}

void Renderer::drawFFNxLogo(float fade)
{
    setClearFlags(true, false);

	/*  y0    y2
	 x0 +-----+ x2
		|    /|
		|   / |
		|  /  |
		| /   |
		|/    |
	 x1 +-----+ x3
		y1    y3
	*/

	// 0
	float x0 = 0.0f;
	float y0 = 0.0f;
	float u0 = 0.0f;
	float v0 = 0.0f;
	// 1
	float x1 = x0;
	float y1 = game_height;
	float u1 = u0;
	float v1 = 1.0f;
	// 2
	float x2 = game_width;
	float y2 = y0;
	float u2 = 1.0f;
	float v2 = v0;
	// 3
	float x3 = x2;
	float y3 = y1;
	float u3 = u2;
	float v3 = v1;

	struct nvertex vertices[] = {
		{x0, y0, 1.0f, 1.0f, 0xffffffff, 0, u0, v0},
		{x1, y1, 1.0f, 1.0f, 0xffffffff, 0, u1, v1},
		{x2, y2, 1.0f, 1.0f, 0xffffffff, 0, u2, v2},
		{x3, y3, 1.0f, 1.0f, 0xffffffff, 0, u3, v3},
	};

    for(int i = 0; i < 4; ++i)
    {
        vertices[i].color.a = 255 * fade;
    }

	WORD indices[] = {
		0, 1, 2,
		1, 3, 2
	};

    bindVertexBuffer(vertices, 0, 4);
	bindIndexBuffer(indices, 6);

    resetState();
    setOverallColorGamut(COLORGAMUT_SRGB); // always draw the logo in sRGB mode. The old setting is restored in drawFFNxLogo() in common.cpp
	setPrimitiveType();
	isTLVertex(true);
	setCullMode(RendererCullMode::DISABLED);
	setBlendMode(RendererBlendMode::BLEND_AVG);
    doTextureFiltering(true);
    isExternalTexture(true);
	isTexture(true);
	doDepthTest(false);
	doDepthWrite(false);
    doModulateAlpha(true);
    useTexture(FFNxLogoHandle.idx);

	draw();
}

void Renderer::show()
{
    // Reset internal state
    resetState();

    renderFrame();

    bgfx::update(
        vertexBufferHandle,
        0,
        bgfx::copy(
            vertexBufferData.data(),
            vectorSizeOf(vertexBufferData)
        )
    );

    bgfx::update(
        indexBufferHandle,
        0,
        bgfx::copy(
            indexBufferData.data(),
            vectorSizeOf(indexBufferData)
        )
    );

    bgfx::frame(doCaptureFrame);

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s\n", __func__);

    bgfx::dbgTextClear();

    if(!ff8 && getmode_cached()->driver_mode == MODE_BATTLE) {
        for(int i = 0; i <= backendViewId; i++)
            bgfx::resetView(i);
    }

    backendViewId = 1;

    vertexBufferData.clear();
    vertexBufferData.shrink_to_fit();

    indexBufferData.clear();
    indexBufferData.shrink_to_fit();

    bgfx::setViewMode(backendViewId, bgfx::ViewMode::Sequential);
}

void Renderer::printText(uint16_t x, uint16_t y, uint32_t color, const char* text)
{
    bgfx::dbgTextPrintf(
        x,
        y,
        color,
        text
    );
}

void Renderer::toggleCaptureFrame()
{
    doCaptureFrame = !doCaptureFrame;
}

const bgfx::Caps* Renderer::getCaps()
{
    return bgfx::getCaps();
};

const bgfx::Stats* Renderer::getStats()
{
    return bgfx::getStats();
}

const bgfx::VertexLayout& Renderer::GetVertexLayout()
{
    return vertexLayout;
}

void Renderer::bindVertexBuffer(struct nvertex* inVertex, vector3<float>* normals, uint32_t inCount)
{
    if (!bgfx::isValid(vertexBufferHandle)) vertexBufferHandle = bgfx::createDynamicVertexBuffer(inCount, vertexLayout, BGFX_BUFFER_ALLOW_RESIZE);

    uint32_t currentOffset = vertexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        vertexBufferData.push_back(Vertex());

        vertexBufferData[currentOffset + idx].x = inVertex[idx]._.x;
        vertexBufferData[currentOffset + idx].y = inVertex[idx]._.y;
        vertexBufferData[currentOffset + idx].z = inVertex[idx]._.z;
        vertexBufferData[currentOffset + idx].w = ( ::isinf(inVertex[idx].color.w) ? 1.0f : inVertex[idx].color.w );
        vertexBufferData[currentOffset + idx].bgra = inVertex[idx].color.color;
        vertexBufferData[currentOffset + idx].u = inVertex[idx].u;
        vertexBufferData[currentOffset + idx].v = inVertex[idx].v;

        if (normals)
        {
            vertexBufferData[currentOffset + idx].nx = normals[idx].x;
            vertexBufferData[currentOffset + idx].ny = normals[idx].y;
            vertexBufferData[currentOffset + idx].nz = normals[idx].z;
        }

        if (vertex_log && idx == 0) ffnx_trace("%s: %u [XYZW(%f, %f, %f, %f), BGRA(%08x), UV(%f, %f)]\n", __func__, idx, vertexBufferData[currentOffset + idx].x, vertexBufferData[currentOffset + idx].y, vertexBufferData[currentOffset + idx].z, vertexBufferData[currentOffset + idx].w, vertexBufferData[currentOffset + idx].bgra, vertexBufferData[currentOffset + idx].u, vertexBufferData[currentOffset + idx].v);
        if (vertex_log && idx == 1) ffnx_trace("%s: See the rest on RenderDoc.\n", __func__);
    }

    bgfx::setVertexBuffer(0, vertexBufferHandle, currentOffset, inCount);
};

void Renderer::bindIndexBuffer(WORD* inIndex, uint32_t inCount)
{
    if (!bgfx::isValid(indexBufferHandle)) indexBufferHandle = bgfx::createDynamicIndexBuffer(inCount, BGFX_BUFFER_ALLOW_RESIZE);

    uint32_t currentOffset = indexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        indexBufferData.push_back(inIndex[idx]);
    }

    bgfx::setIndexBuffer(indexBufferHandle, currentOffset, inCount);
};

void Renderer::setScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    scissorOffsetX = getInternalCoordX(x);
    scissorOffsetY = getInternalCoordY(y);
    scissorWidth = getInternalCoordX(width);
    scissorHeight = getInternalCoordY(height);

    // This removes the black bars on the top and bottom of the screen
    if (enable_uncrop)
    {
        bool is_movie_playing = *ff7_externals.word_CC1638 && !ff7_externals.modules_global_object->BGMOVIE_flag;
        if(!(is_movie_playing && widescreen.getMovieMode() == WM_DISABLED))
        {
            if(y == 16 && height == 448)
            {
                scissorOffsetY = getInternalCoordY(0.0);
                scissorHeight = getInternalCoordY(480);
            }
        }
    }

    if(!widescreen_enabled) return;

    struct game_mode* mode = getmode_cached();
    switch(mode->driver_mode)
    {
        case MODE_CREDITS:
            {
                // Keep the default scissor for credits mode
                scissorOffsetX = getInternalCoordX(x + abs(wide_viewport_x));
            }
            break;
        case MODE_FIELD:
            {
                // Keep the default scissor for widescreen disabled movies and fields
                bool isKeepDefaultScissor = false;
                if (!ff8)
                {
                     bool is_movie_playing = *ff7_externals.word_CC1638 && !ff7_externals.modules_global_object->BGMOVIE_flag;
                     isKeepDefaultScissor = (is_movie_playing && widescreen.getMovieMode() == WM_DISABLED) || widescreen.getMode() == WM_DISABLED;
                }

                if (isKeepDefaultScissor)
                {
                    return;
                }

                // This changes the scissor width and makes it bigger to fit widescreen
                if(x == 0 && width == game_width)
                    scissorWidth = getInternalCoordX(wide_viewport_width);
                else
                    scissorOffsetX = getInternalCoordX(x + abs(wide_viewport_x));
            }
            break;
        case MODE_SWIRL:
            {
                // This changes the scissor width and makes it bigger to fit widescreen
                if(x == 0 && width == game_width)
                    scissorWidth = getInternalCoordX(wide_viewport_width);
                else
                    scissorOffsetX = getInternalCoordX(x + abs(wide_viewport_x));
            }
            break;
        case MODE_BATTLE:
            {
                if(x == 0 && width == game_width)
                    scissorWidth = getInternalCoordX(wide_viewport_width);
                else if(internalState.bIsTLVertex)
                    scissorOffsetX = getInternalCoordX(x + abs(wide_viewport_x));

                if (ff8)
                {
                    // This removes the black bars on the top and bottom of the screen
                    if(y == 24 && height == 432)
                    {
                        scissorOffsetY = getInternalCoordY(0.0);
                        scissorHeight = getInternalCoordY(480);
                    }
                }
            }
            break;
        default:
            {
                // This changes the scissor width and makes it bigger to fit widescreen
                if(x == 0 && width == game_width)
                    scissorWidth = getInternalCoordX(wide_viewport_width);
                else
                    scissorOffsetX = getInternalCoordX(x + abs(wide_viewport_x));

                if (ff8)
                {
                    // This removes the black bars on the top and bottom of the screen
                    if(y == 16 && height == 448)
                    {
                        scissorOffsetY = getInternalCoordY(0.0);
                        scissorHeight = getInternalCoordY(480);
                    }
                }
            }
            break;
    }
}

void Renderer::setClearFlags(bool doClearColor, bool doClearDepth)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s clearColor=%d,clearDepth=%d\n", __func__, doClearColor, doClearDepth);

    uint16_t clearFlags = BGFX_CLEAR_NONE;

    if (doClearColor)
        clearFlags |= BGFX_CLEAR_COLOR;

    if (doClearDepth)
        clearFlags |= BGFX_CLEAR_DEPTH;

    bgfx::setViewClear(backendViewId, clearFlags, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);

    internalState.bHasDrawBeenDone = false;
}

void Renderer::setBackgroundColor(float r, float g, float b, float a)
{
    internalState.clearColorValue = createBGRA(r * 255, g * 255, b * 255, a * 255);
}

uint32_t Renderer::createTexture(uint8_t* data, size_t width, size_t height, int stride, RendererTextureType type, bool isSrgb, bool copyData)
{
    bgfx::TextureHandle ret = FFNX_RENDERER_INVALID_HANDLE;

    bgfx::TextureFormat::Enum texFormat = bgfx::TextureFormat::R8;
    bimg::TextureFormat::Enum imgFormat = bimg::TextureFormat::R8;

    if (type == RendererTextureType::BGRA)
    {
        texFormat = bgfx::TextureFormat::BGRA8;
        imgFormat = bimg::TextureFormat::BGRA8;
    }

    bimg::TextureInfo texInfo;
    bimg::imageGetSize(&texInfo, width, height, 0, false, false, 1, imgFormat);

    // If the texture we are going to create does not fit in memory, return an empty one.
    // Will prevent the game from crashing, while allowing the player to not loose its progress.
    if (doesItFitInMemory(texInfo.storageSize) && (data != NULL))
    {
        const bgfx::Memory* mem;

        if (copyData)
        {
            mem = bgfx::alloc(texInfo.storageSize);
            // Protect from crashes
            if (mem != NULL) {
                bx::memCopy(mem->data, data, texInfo.storageSize);
            }
        }
        else
        {
            mem = bgfx::makeRef(data, texInfo.storageSize, RendererReleaseData, (void *)data);
        }

        uint64_t flags = BGFX_SAMPLER_NONE;

        if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
        else flags |= BGFX_TEXTURE_NONE;

        ret = bgfx::createTexture2D(
            width,
            height,
            false,
            1,
            texFormat,
            flags,
            stride > 0 ? NULL : mem
        );

        if (stride > 0)
            bgfx::updateTexture2D(
                ret,
                0,
                0,
                0,
                0,
                width,
                height,
                mem,
                stride
            );

        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from data with stride %u\n", __func__, ret.idx, width, height, stride);
    }

    return ret.idx;
};

uint32_t Renderer::createTexture(char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb)
{
    bgfx::TextureHandle handle = createTextureHandle(filename, width, height, mipCount, isSrgb);
    return handle.idx;
}

bimg::ImageContainer* Renderer::createImageContainer(const char* filename, bimg::TextureFormat::Enum targetFormat)
{
    return loadImageContainer(&defaultAllocator, filename, targetFormat);
}

bimg::ImageContainer* Renderer::createImageContainer(cmrc::file* file, bimg::TextureFormat::Enum targetFormat)
{
    bimg::ImageContainer* img = nullptr;

    if (file->size() > 0)
    {
        img = bimg::imageParse(&defaultAllocator, file->begin(), file->size(), targetFormat);
    }

    return img;
}

bgfx::TextureHandle Renderer::createTextureHandle(char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb)
{
    bgfx::TextureHandle ret = FFNX_RENDERER_INVALID_HANDLE;
    bimg::ImageContainer* img = createImageContainer(filename);

    if (img != nullptr)
    {
        if (gl_check_texture_dimensions(img->m_width, img->m_height, filename) && doesItFitInMemory(img->m_size))
        {
            uint64_t flags = BGFX_SAMPLER_NONE;

            if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
            else flags |= BGFX_TEXTURE_NONE;

            const bgfx::Memory* mem = bgfx::makeRef(img->m_data, img->m_size, RendererReleaseImageContainer, img);
            if (img->m_cubeMap)
            {
                ret = bgfx::createTextureCube(
                    img->m_width,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    flags,
                    mem
                );
            }
            else
            {

                ret = bgfx::createTexture2D(
                    img->m_width,
                    img->m_height,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    flags,
                    mem
                );
            }

            *width = img->m_width;
            *height = img->m_height;
            *mipCount = img->m_numMips;

            if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from filename %s\n", __func__, ret.idx, width, height, filename);
        }
    }

    return ret;
}

bgfx::TextureHandle Renderer::createTextureHandle(cmrc::file* file, char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb)
{
    bgfx::TextureHandle ret = FFNX_RENDERER_INVALID_HANDLE;
    bimg::ImageContainer* img = createImageContainer(file);

    if (img != nullptr)
    {
        if (gl_check_texture_dimensions(img->m_width, img->m_height, filename) && doesItFitInMemory(img->m_size))
        {
            uint64_t flags = BGFX_SAMPLER_NONE;

            if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
            else flags |= BGFX_TEXTURE_NONE;

            const bgfx::Memory* mem = bgfx::makeRef(img->m_data, img->m_size, RendererReleaseImageContainer, img);
            if (img->m_cubeMap)
            {
                ret = bgfx::createTextureCube(
                    img->m_width,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    flags,
                    mem
                );
            }
            else
            {

                ret = bgfx::createTexture2D(
                    img->m_width,
                    img->m_height,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    flags,
                    mem
                );
            }

            *width = img->m_width;
            *height = img->m_height;
            *mipCount = img->m_numMips;

            if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from filename %s\n", __func__, ret.idx, width, height, filename);
        }
    }

    return ret;
}

uint32_t Renderer::createTextureLibPng(char* filename, uint32_t* width, uint32_t* height, bool isSrgb)
{
    bgfx::TextureHandle ret = FFNX_RENDERER_INVALID_HANDLE;
    bimg::ImageMip mip;

    if (!loadPng(filename, mip)) {
        return ret.idx;
    }

    const bgfx::Memory* mem = bgfx::makeRef(mip.m_data, mip.m_size, RendererReleaseData, (void *)mip.m_data);

    uint64_t flags = BGFX_SAMPLER_NONE;

    if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
    else flags |= BGFX_TEXTURE_NONE;

    ret = bgfx::createTexture2D(
        mip.m_width,
        mip.m_height,
        false,
        1,
        bgfx::TextureFormat::Enum(mip.m_format),
        flags,
        mem
    );

    *width = mip.m_width;
    *height = mip.m_height;

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from filename %s\n", __func__, ret.idx, *width, *height, filename);

    return ret.idx;
}

bool Renderer::saveTexture(const char* filename, uint32_t width, uint32_t height, const void* data)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %ux%u with filename %s\n", __func__, width, height, filename);

    if (bx::open(&defaultWriter, filename, false))
    {
        bimg::imageWritePng(
            &defaultWriter,
            width,
            height,
            width * 4,
            data,
            bimg::TextureFormat::BGRA8,
            false
        );

        bx::close(&defaultWriter);

        return true;
    }

    return false;
}

void Renderer::deleteTexture(uint16_t rt)
{
    if (rt > 0)
    {
        bgfx::TextureHandle handle = { rt };

        if (bgfx::isValid(handle)) {
            bgfx::destroy(handle);

            if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u Texture was valid and is now destroyed!\n", __func__, rt);
        }
    }
};

void Renderer::useTexture(uint16_t rt, uint32_t slot)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: [%u] => %u\n", __func__, slot, rt);

    if (rt > 0)
    {
        internalState.texHandlers[slot] = { rt };
        if (slot == RendererTextureSlot::TEX_Y) isTexture(true);
    }
    else
    {
        internalState.texHandlers[slot] = BGFX_INVALID_HANDLE;
        if (slot == RendererTextureSlot::TEX_Y) isTexture(false);
    }
};

uint32_t Renderer::createBlitTexture(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint16_t newX = getInternalCoordX(x);
    uint16_t newY = getInternalCoordY(y);
    uint16_t newWidth = getInternalCoordX(width);
    uint16_t newHeight = getInternalCoordY(height);

    bgfx::TextureHandle ret = bgfx::createTexture2D(newWidth, newHeight, false, 1, internalState.bIsHDR ? bgfx::TextureFormat::RGB10A2 : bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_BLIT_DST);

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => XY(%u,%u) WH(%u,%u)\n", __func__, ret.idx, newX, newY, newWidth, newHeight);

    return ret.idx;
}

void Renderer::blitTexture(uint16_t dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint16_t newX = getInternalCoordX(x);
    uint16_t newY = getInternalCoordY(y);
    uint16_t newWidth = getInternalCoordX(width);
    uint16_t newHeight = getInternalCoordY(height);

    uint16_t dstY = 0;

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => XY(%u,%u) WH(%u,%u)\n", __func__, dest, newX, newY, newWidth, newHeight);

    if (getCaps()->originBottomLeft)
    {
        int _newY = framebufferHeight - (newY + newHeight);

        // If the new Y is a positive value, we can use it as it is
        if (_newY > 0)
        {
            newY = _newY;
            dstY = 0;
        }
        // Otherwise, it means we have to copy the whole source texture
        // but shift the result on the Y axis of the dest texture of the absolute negative difference
        else
        {
            newY = 0;
            dstY = ::abs(_newY);
        }
    }

    backendViewId++;

    bgfx::TextureHandle texHandle = { dest };
    bgfx::blit(backendViewId, texHandle, 0, dstY, bgfx::getTexture(backendFrameBuffer, 0), newX, newY, newWidth, newHeight);
    bgfx::touch(backendViewId);
    setClearFlags(false, false);

    backendViewId++;
    setClearFlags(false, false);
}

void Renderer::zoomBackendFrameBuffer(int x, int y, int width, int height)
{
    if(!internalState.bHasDrawBeenDone) return;

    bgfx::TextureHandle textureHandle = bgfx::createTexture2D(width, height, false, 1, internalState.bIsHDR ? bgfx::TextureFormat::RGB10A2 : bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_BLIT_DST);

    backendViewId++;
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_NONE, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);
    bgfx::blit(backendViewId, textureHandle, 0, 0, bgfx::getTexture(backendFrameBuffer, 0), x, y, width, height);
    backendViewId++;
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_NONE, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);

    /*  y0    y2
     x0 +-----+ x2
        |    /|
        |   / |
        |  /  |
        | /   |
        |/    |
     x1 +-----+ x3
        y1    y3
    */

    // 0
    float x0 = wide_viewport_x;
    float y0 = wide_viewport_y;
    float u0 = 0.0f;
    float v0 = getCaps()->originBottomLeft ? 1.0f : 0.0f;
    // 1
    float x1 = x0;
    float y1 = wide_viewport_height;
    float u1 = u0;
    float v1 = getCaps()->originBottomLeft ? 0.0f : 1.0f;
    // 2
    float x2 = x0 + wide_viewport_width;
    float y2 = y0;
    float u2 = 1.0f;
    float v2 = v0;
    // 3
    float x3 = x2;
    float y3 = y1;
    float u3 = u2;
    float v3 = v1;

    struct nvertex vertices[] = {
        {x0, y0, 1.0f, 1.0f, 0xff000000, 0, u0, v0},
        {x1, y1, 1.0f, 1.0f, 0xff000000, 0, u1, v1},
        {x2, y2, 1.0f, 1.0f, 0xff000000, 0, u2, v2},
        {x3, y3, 1.0f, 1.0f, 0xff000000, 0, u3, v3},
    };
    WORD indices[] = {
        0, 1, 2,
        1, 3, 2
    };

    backendProgram = RendererProgram::BLIT;

    useTexture(textureHandle.idx);

    bindVertexBuffer(vertices, 0, 4);
    bindIndexBuffer(indices, 6);

    doDepthTest(false);
    setCullMode(DISABLED);
    setBlendMode(RendererBlendMode::BLEND_DISABLED);
    setPrimitiveType();

    draw();

    if (bgfx::isValid(textureHandle)) bgfx::destroy(textureHandle);
}

void Renderer::clearDepthBuffer()
{
    backendViewId++;
    bgfx::setViewMode(backendViewId, bgfx::ViewMode::Sequential);
    bgfx::setViewRect(backendViewId, 0, 0, framebufferWidth, framebufferHeight);
    bgfx::setViewFrameBuffer(backendViewId, backendFrameBuffer);
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_DEPTH);
    bgfx::touch(backendViewId);

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: Clearing depth\n", __func__, backendViewId);
}

void Renderer::isMovie(bool flag)
{
    internalState.bIsMovie = flag;
};

void Renderer::isTLVertex(bool flag)
{
    internalState.bIsTLVertex = flag;
};

void Renderer::setBlendMode(RendererBlendMode mode)
{
    internalState.blendMode = mode;
};

void Renderer::isTexture(bool flag)
{
    internalState.bIsTexture = flag;
};

void Renderer::isFBTexture(bool flag)
{
    internalState.bIsFBTexture = flag;
};

void Renderer::isFullRange(bool flag)
{
    internalState.bIsMovieFullRange = flag;
};

void Renderer::isYUV(bool flag)
{
    internalState.bIsMovieYUV = flag;
};

void Renderer::doModulateAlpha(bool flag)
{
    internalState.bModulateAlpha = flag;
};

void Renderer::doTextureFiltering(bool flag)
{
    internalState.bDoTextureFiltering = flag;
};

void Renderer::doMirrorTextureWrap(bool flag)
{
    internalState.bDoMirrorTextureWrap = flag;
}

void Renderer::isExternalTexture(bool flag)
{
    internalState.bIsExternalTexture = flag;
}

bool Renderer::isHDR()
{
    return internalState.bIsHDR;
}

void Renderer::setColorMatrix(ColorMatrixType cmtype){
    internalState.bIsMovieColorMatrix = cmtype;
}

void Renderer::setColorGamut(ColorGamutType cgtype){
    internalState.bIsMovieColorGamut = cgtype;
}

void Renderer::setOverallColorGamut(ColorGamutType cgtype){
    internalState.bIsOverallColorGamut = cgtype;
}

void Renderer::setGamutOverride(bool flag)
{
    internalState.bIsOverrideGamut = flag;
}

void Renderer::setGammaType(InverseGammaFunctionType gtype)
{
    internalState.bIsMovieGammaType = gtype;
};

void Renderer::setAlphaRef(RendererAlphaFunc func, float ref)
{
    internalState.alphaFunc = func;
    internalState.alphaRef = ref;
};

void Renderer::doAlphaTest(bool flag)
{
    internalState.bDoAlphaTest = flag;
};

void Renderer::setInterpolationQualifier(RendererInterpolationQualifier qualifier)
{
    switch (qualifier)
    {
    case RendererInterpolationQualifier::FLAT:
        backendProgram = RendererProgram::FLAT;
        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: FLAT\n", __func__);
        break;
    case RendererInterpolationQualifier::SMOOTH:
        backendProgram = RendererProgram::SMOOTH;
        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: SMOOTH\n", __func__);
        break;
    }
}

void Renderer::setPrimitiveType(RendererPrimitiveType type)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u\n", __func__, type);

    internalState.primitiveType = type;
};

void Renderer::setCullMode(RendererCullMode mode)
{
    internalState.cullMode = mode;
}

void Renderer::doDepthTest(bool flag)
{
    internalState.bDoDepthTest = flag;
}

void Renderer::doDepthWrite(bool flag)
{
    internalState.bDoDepthWrite = flag;
}

void Renderer::doScissorTest(bool flag)
{
    internalState.bDoScissorTest = flag;
}

void Renderer::setWireframeMode(bool flag)
{
    if (flag) bgfx::setDebug(BGFX_DEBUG_WIREFRAME);
}

void Renderer::setViewMatrix(struct matrix* matrix)
{
    ::memcpy(internalState.viewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    bx::mtxInverse(internalState.invViewMatrix, internalState.viewMatrix);

    internalState.isViewMatrixSet = true;

    if (uniform_log) printMatrix(__func__, internalState.viewMatrix);
}

float* Renderer::getViewMatrix()
{
    return internalState.viewMatrix;
}

float* Renderer::getInvViewMatrix()
{
    return internalState.invViewMatrix;
}

bool Renderer::isViewMatrixSet()
{
    return internalState.isViewMatrixSet;
}

void Renderer::resetViewMatrixFlag()
{
    internalState.isViewMatrixSet = false;
}

void Renderer::setWorldViewMatrix(struct matrix *matrix, bool calculateNormalMatrix)
{
    ::memcpy(internalState.worldViewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    if (uniform_log) printMatrix(__func__, internalState.worldViewMatrix);

    if(calculateNormalMatrix)
    {
        struct matrix transpose;
        transpose_matrix(matrix, &transpose);
        struct matrix invTranspose;
        inverse_matrix(&transpose, &invTranspose);
        invTranspose._41 = 0.0;
        invTranspose._42 = 0.0;
        invTranspose._43 = 0.0;
        invTranspose._44 = 1.0;

        ::memcpy(internalState.normalMatrix, &invTranspose.m[0][0], sizeof(invTranspose.m));

        if (uniform_log) printMatrix(__func__, internalState.normalMatrix);
    }
}

float* Renderer::getWorldViewMatrix()
{
    return internalState.worldViewMatrix;
}

float* Renderer::getNormalMatrix()
{
    return internalState.normalMatrix;
}

void Renderer::setD3DViweport(struct matrix* matrix)
{
    ::memcpy(internalState.d3dViewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    if (uniform_log) printMatrix(__func__, internalState.d3dViewMatrix);
};

void Renderer::setD3DProjection(struct matrix* matrix)
{
    ::memcpy(internalState.d3dProjectionMatrix, &matrix->m[0][0], sizeof(matrix->m));

    // Modify the projection matrix to prevent stretching
    if(widescreen_enabled)
    {
        float widescreenScale = round(float(game_width) / wide_viewport_width * 100) / 100.f;
        internalState.d3dProjectionMatrix[0] *= widescreenScale;
        internalState.d3dProjectionMatrix[8] *= widescreenScale;
    }

    // Modify the projection matrix to extend view distance
    if (enable_worldmap_external_mesh)
    {
        struct game_mode* mode = getmode_cached();
        if(mode->driver_mode == MODE_WORLDMAP)
        {
            const float f_offset = 0.0035f;
            const float n_offset = 0.0f;

            float a = internalState.d3dProjectionMatrix[10];
            float b = internalState.d3dProjectionMatrix[11];

            float f = b / (a + 1.0f) + f_offset;
            float n = b / (a - 1.0f) + n_offset;

            internalState.d3dProjectionMatrix[10] = -(f + n) / (f -n);
            internalState.d3dProjectionMatrix[11] = -(2*f*n) / (f - n);
        }
    }

    if (uniform_log) printMatrix(__func__, internalState.d3dProjectionMatrix);
};

uint16_t Renderer::getInternalCoordX(uint16_t inX)
{
    if(widescreen_enabled)
        return (inX * framebufferWidth) / (wide_viewport_width);
    else
        return (inX * framebufferWidth) / (game_width);
}

uint16_t Renderer::getInternalCoordY(uint16_t inY)
{
    return (inY * framebufferHeight) / game_height;
}

uint16_t Renderer::getScalingFactor()
{
    return scalingFactor;
}

void Renderer::setTimeColor(bx::Vec3 color)
{
    internalState.TimeColor[0] = color.x;
    internalState.TimeColor[1] = color.y;
    internalState.TimeColor[2] = color.z;
}

void Renderer::setTimeEnabled(bool flag)
{
    internalState.TimeData[0] = static_cast<float>(flag);
}

void Renderer::setTimeFilterEnabled(bool flag)
{
    internalState.TimeData[1] = static_cast<float>(flag);
}

bool Renderer::isTimeFilterEnabled()
{
    return static_cast<bool>(internalState.TimeData[1]);
}

void Renderer::setSphericalWorldRate(float value)
{
    internalState.sphericalWorldRate = value;
}

void Renderer::setFogEnabled(bool flag)
{
    internalState.bIsFogEnabled = flag;
}

bool Renderer::isFogEnabled()
{
    return internalState.bIsFogEnabled;
}

void Renderer::setGameLightData(light_data* lightdata)
{
    struct game_mode* mode = getmode_cached();

    bool useGameLighting = lightdata != nullptr;
    if (enable_worldmap_external_mesh && mode->driver_mode == MODE_WORLDMAP)
    {
        useGameLighting = false;
    }

    if (useGameLighting)
    {
        internalState.gameGlobalLightColor[0] = lightdata->global_light_color.r;
        internalState.gameGlobalLightColor[1] = lightdata->global_light_color.g;
        internalState.gameGlobalLightColor[2] = lightdata->global_light_color.b;
        internalState.gameGlobalLightColor[3] = enable_lighting ? 2.5f : 1.0f;

        internalState.gameLightColor1[0] = lightdata->light_color_1.r;
        internalState.gameLightColor1[1] = lightdata->light_color_1.g;
        internalState.gameLightColor1[2] = lightdata->light_color_1.b;
        internalState.gameLightColor1[3] = 1.0;

        internalState.gameLightColor2[0] = lightdata->light_color_2.r;
        internalState.gameLightColor2[1] = lightdata->light_color_2.g;
        internalState.gameLightColor2[2] = lightdata->light_color_2.b;
        internalState.gameLightColor2[3] = 1.0;

        internalState.gameLightColor3[0] = lightdata->light_color_3.r;
        internalState.gameLightColor3[1] = lightdata->light_color_3.g;
        internalState.gameLightColor3[2] = lightdata->light_color_3.b;
        internalState.gameLightColor3[3] = 1.0;

        if (mode->driver_mode == MODE_WORLDMAP)
        {
            internalState.gameLightDir1[0] = lightdata->light_dir_1.x;
            internalState.gameLightDir1[1] = lightdata->light_dir_1.z;
            internalState.gameLightDir1[2] = lightdata->light_dir_1.y;

            internalState.gameLightDir2[0] = lightdata->light_dir_2.x;
            internalState.gameLightDir2[1] = lightdata->light_dir_2.z;
            internalState.gameLightDir2[2] = lightdata->light_dir_2.y;

            internalState.gameLightDir3[0] = lightdata->light_dir_3.x;
            internalState.gameLightDir3[1] = lightdata->light_dir_3.z;
            internalState.gameLightDir3[2] = lightdata->light_dir_3.y;
        }
        else
        {
            internalState.gameLightDir1[0] = -lightdata->light_dir_1.x;
            internalState.gameLightDir1[1] = -lightdata->light_dir_1.y;
            internalState.gameLightDir1[2] = -lightdata->light_dir_1.z;

            internalState.gameLightDir2[0] = -lightdata->light_dir_2.x;
            internalState.gameLightDir2[1] = -lightdata->light_dir_2.y;
            internalState.gameLightDir2[2] = -lightdata->light_dir_2.z;

            internalState.gameLightDir3[0] = -lightdata->light_dir_3.x;
            internalState.gameLightDir3[1] = -lightdata->light_dir_3.y;
            internalState.gameLightDir3[2] = -lightdata->light_dir_3.z;
        }

        internalState.gameScriptedLightColor[0] = lightdata->scripted_light_color.r;
        internalState.gameScriptedLightColor[1] = lightdata->scripted_light_color.g;
        internalState.gameScriptedLightColor[2] = lightdata->scripted_light_color.b;
    }
    else
    {
        internalState.gameGlobalLightColor[0] = 1.0;
        internalState.gameGlobalLightColor[1] = 1.0;
        internalState.gameGlobalLightColor[2] = 1.0;
        internalState.gameGlobalLightColor[3] = 1;

        internalState.gameLightColor1[0] = 0;
        internalState.gameLightColor1[1] = 0;
        internalState.gameLightColor1[2] = 0;
        internalState.gameLightColor1[3] = 0;

        internalState.gameLightColor2[0] = 0;
        internalState.gameLightColor2[1] = 0;
        internalState.gameLightColor2[2] = 0;
        internalState.gameLightColor2[3] = 0;

        internalState.gameLightColor3[0] = 0;
        internalState.gameLightColor3[1] = 0;
        internalState.gameLightColor3[2] = 0;
        internalState.gameLightColor3[3] = 0;

        internalState.gameLightDir1[0] = 0;
        internalState.gameLightDir1[1] = 0;
        internalState.gameLightDir1[2] = 0;

        internalState.gameLightDir2[0] = 0;
        internalState.gameLightDir2[1] = 0;
        internalState.gameLightDir2[2] = 0;

        internalState.gameLightDir3[0] = 0;
        internalState.gameLightDir3[1] = 0;
        internalState.gameLightDir3[2] = 0;

        internalState.gameScriptedLightColor[0] = 1.0;
        internalState.gameScriptedLightColor[1] = 1.0;
        internalState.gameScriptedLightColor[2] = 1.0;
    }
}

