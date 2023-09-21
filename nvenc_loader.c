/*
** Copyright 2023 John R. Patek Sr.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#include "nvenc_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define DEFAULT_LIBRARY_PATH "nvEncodeAPI.dll"
typedef HMODULE dll_handle_t;
#define DLL_LOAD(PATH) LoadLibraryA(PATH)
#define DLL_LINK(HANDLE, FUNCTION) GetProcAddress(HANDLE, FUNCTION)
#define DLL_UNLOAD(HANDLE) FreeLibrary(HANDLE)
#else
#include <dlfcn.h>
#define DEFAULT_LIBRARY_PATH "libnvidia-encode.so"
typedef void *dll_handle_t;
#define DLL_LOAD(PATH) dlopen(PATH, RTLD_LAZY)
#define DLL_LINK(HANDLE, FUNCTION) dlsym(HANDLE, FUNCTION)
#define DLL_UNLOAD(HANDLE) dlclose(HANDLE)
#endif

typedef NVENCSTATUS(NVENCAPI *nvenc_create_instance_func)(NV_ENCODE_API_FUNCTION_LIST *);
typedef NVENCSTATUS(NVENCAPI *nvenc_max_supported_version_func)(uint32_t *);

dll_handle_t dll_handle;
NV_ENCODE_API_FUNCTION_LIST api;

int nvenc_load(const char *const library_path, const char **error)
{
    int result;
    nvenc_create_instance_func create_instance;
    NVENCSTATUS status;

    result = NVENC_LOAD_SUCCESS;

    if (library_path != NULL)
    {
        dll_handle = DLL_LOAD(library_path);
    }
    else
    {
        dll_handle = DLL_LOAD(DEFAULT_LIBRARY_PATH);
    }

    if (dll_handle == NULL)
    {
        result = NVENC_LOAD_FAILURE;
        if (error != NULL)
        {
            *error = "failed to load nvenc library";
        }
        goto done;
    }

    create_instance = (nvenc_create_instance_func)DLL_LINK(dll_handle, "NvEncodeAPICreateInstance");
    if (create_instance == NULL)
    {
        result = NVENC_LOAD_FAILURE;
        if (error != NULL)
        {
            *error = "failed to link NvEncodeAPICreateInstance";
        }
        goto done;
    }

    (void)memset(&api, 0, sizeof(NV_ENCODE_API_FUNCTION_LIST));
    api.version = NV_ENCODE_API_FUNCTION_LIST_VER;

    status = create_instance(&api);
    if (status != NV_ENC_SUCCESS)
    {
        result = NVENC_LOAD_FAILURE;
        if (error != NULL)
        {
            *error = "failed to initialize API";
        }
    }

done:
    return result;
}

void nvenc_unload()
{
    if (dll_handle != NULL)
    {
        DLL_UNLOAD(dll_handle);
    }
}

NVENCSTATUS NVENCAPI NvEncOpenEncodeSession(void *device, uint32_t deviceType, void **encoder)
{
    return api.nvEncOpenEncodeSession(device, deviceType, encoder);
}

NVENCSTATUS NVENCAPI NvEncGetEncodeGUIDCount(void *encoder, uint32_t *encodeGUIDCount)
{
    return api.nvEncGetEncodeGUIDCount(encoder, encodeGUIDCount);
}

NVENCSTATUS NVENCAPI NvEncGetEncodeGUIDs(void *encoder, GUID *GUIDs, uint32_t guidArraySize, uint32_t *GUIDCount)
{
    return api.nvEncGetEncodeGUIDs(encoder, GUIDs, guidArraySize, GUIDCount);
}

NVENCSTATUS NVENCAPI NvEncGetEncodeProfileGUIDCount(void *encoder, GUID encodeGUID, uint32_t *encodeProfileGUIDCount)
{
    return api.nvEncGetEncodeProfileGUIDCount(encoder, encodeGUID, encodeProfileGUIDCount);
}

NVENCSTATUS NVENCAPI NvEncGetEncodeProfileGUIDs(void *encoder, GUID encodeGUID, GUID *profileGUIDs, uint32_t guidArraySize, uint32_t *GUIDCount)
{
    return api.nvEncGetEncodeProfileGUIDs(encoder, encodeGUID, profileGUIDs, guidArraySize, GUIDCount);
}

NVENCSTATUS NVENCAPI NvEncGetInputFormatCount(void *encoder, GUID encodeGUID, uint32_t *inputFmtCount)
{
    return api.nvEncGetInputFormatCount(encoder, encodeGUID, inputFmtCount);
}

NVENCSTATUS NVENCAPI NvEncGetInputFormats(void *encoder, GUID encodeGUID, NV_ENC_BUFFER_FORMAT *inputFmts, uint32_t inputFmtArraySize, uint32_t *inputFmtCount)
{
    return api.nvEncGetInputFormats(encoder, encodeGUID, inputFmts, inputFmtArraySize, inputFmtCount);
}

NVENCSTATUS NVENCAPI NvEncGetEncodeCaps(void *encoder, GUID encodeGUID, NV_ENC_CAPS_PARAM *capsParam, int *capsVal)
{
    return api.nvEncGetEncodeCaps(encoder, encodeGUID, capsParam, capsVal);
}

NVENCSTATUS NVENCAPI NvEncGetEncodePresetCount(void *encoder, GUID encodeGUID, uint32_t *encodePresetGUIDCount)
{
    return api.nvEncGetEncodePresetCount(encoder, encodeGUID, encodePresetGUIDCount);
}

NVENCSTATUS NVENCAPI NvEncGetEncodePresetGUIDs(void *encoder, GUID encodeGUID, GUID *presetGUIDs, uint32_t guidArraySize, uint32_t *encodePresetGUIDCount)
{
    return api.nvEncGetEncodePresetGUIDs(encoder, encodeGUID, presetGUIDs, guidArraySize, encodePresetGUIDCount);
}

NVENCSTATUS NVENCAPI NvEncGetEncodePresetConfig(void *encoder, GUID encodeGUID, GUID presetGUID, NV_ENC_PRESET_CONFIG *presetConfig)
{
    return api.nvEncGetEncodePresetConfig(encoder, encodeGUID, presetGUID, presetConfig);
}

NVENCSTATUS NVENCAPI NvEncGetEncodePresetConfigEx(void *encoder, GUID encodeGUID, GUID presetGUID, NV_ENC_TUNING_INFO tuningInfo, NV_ENC_PRESET_CONFIG *presetConfig)
{
    return api.nvEncGetEncodePresetConfigEx(encoder, encodeGUID, presetGUID, tuningInfo, presetConfig);
}

NVENCSTATUS NVENCAPI NvEncInitializeEncoder(void *encoder, NV_ENC_INITIALIZE_PARAMS *createEncodeParams)
{
    return api.nvEncInitializeEncoder(encoder, createEncodeParams);
}

NVENCSTATUS NVENCAPI NvEncCreateInputBuffer(void *encoder, NV_ENC_CREATE_INPUT_BUFFER *createInputBufferParams)
{
    return api.nvEncCreateInputBuffer(encoder, createInputBufferParams);
}

NVENCSTATUS NVENCAPI NvEncDestroyInputBuffer(void *encoder, NV_ENC_INPUT_PTR inputBuffer)
{
    return api.nvEncDestroyInputBuffer(encoder, inputBuffer);
}

NVENCSTATUS NVENCAPI NvEncSetIOCudaStreams(void *encoder, NV_ENC_CUSTREAM_PTR inputStream, NV_ENC_CUSTREAM_PTR outputStream)
{
    return api.nvEncSetIOCudaStreams(encoder, inputStream, outputStream);
}

NVENCSTATUS NVENCAPI NvEncCreateBitstreamBuffer(void *encoder, NV_ENC_CREATE_BITSTREAM_BUFFER *createBitstreamBufferParams)
{
    return api.nvEncCreateBitstreamBuffer(encoder, createBitstreamBufferParams);
}

NVENCSTATUS NVENCAPI NvEncDestroyBitstreamBuffer(void *encoder, NV_ENC_OUTPUT_PTR bitstreamBuffer)
{
    return api.nvEncDestroyBitstreamBuffer(encoder, bitstreamBuffer);
}

NVENCSTATUS NVENCAPI NvEncEncodePicture(void *encoder, NV_ENC_PIC_PARAMS *encodePicParams)
{
    return api.nvEncEncodePicture(encoder, encodePicParams);
}

NVENCSTATUS NVENCAPI NvEncLockBitstream(void *encoder, NV_ENC_LOCK_BITSTREAM *lockBitstreamBufferParams)
{
    return api.nvEncLockBitstream(encoder, lockBitstreamBufferParams);
}

NVENCSTATUS NVENCAPI NvEncUnlockBitstream(void *encoder, NV_ENC_OUTPUT_PTR bitstreamBuffer)
{
    return api.nvEncUnlockBitstream(encoder, bitstreamBuffer);
}

NVENCSTATUS NVENCAPI NvEncLockInputBuffer(void *encoder, NV_ENC_LOCK_INPUT_BUFFER *lockInputBufferParams)
{
    return api.nvEncLockInputBuffer(encoder, lockInputBufferParams);
}

NVENCSTATUS NVENCAPI NvEncUnlockInputBuffer(void *encoder, NV_ENC_INPUT_PTR inputBuffer)
{
    return api.nvEncUnlockInputBuffer(encoder, inputBuffer);
}

NVENCSTATUS NVENCAPI NvEncGetEncodeStats(void *encoder, NV_ENC_STAT *encodeStats)
{
    return api.nvEncGetEncodeStats(encoder, encodeStats);
}

NVENCSTATUS NVENCAPI NvEncGetSequenceParams(void *encoder, NV_ENC_SEQUENCE_PARAM_PAYLOAD *sequenceParamPayload)
{
    return api.nvEncGetSequenceParams(encoder, sequenceParamPayload);
}

NVENCSTATUS NVENCAPI NvEncGetSequenceParamEx(void *encoder, NV_ENC_INITIALIZE_PARAMS *encInitParams, NV_ENC_SEQUENCE_PARAM_PAYLOAD *sequenceParamPayload)
{
    return api.nvEncGetSequenceParamEx(encoder, encInitParams, sequenceParamPayload);
}

NVENCSTATUS NVENCAPI NvEncRegisterAsyncEvent(void *encoder, NV_ENC_EVENT_PARAMS *eventParams)
{
    return api.nvEncRegisterAsyncEvent(encoder, eventParams);
}

NVENCSTATUS NVENCAPI NvEncUnregisterAsyncEvent(void *encoder, NV_ENC_EVENT_PARAMS *eventParams)
{
    return api.nvEncUnregisterAsyncEvent(encoder, eventParams);
}

NVENCSTATUS NVENCAPI NvEncMapInputResource(void *encoder, NV_ENC_MAP_INPUT_RESOURCE *mapInputResParams)
{
    return api.nvEncMapInputResource(encoder, mapInputResParams);
}

NVENCSTATUS NVENCAPI NvEncUnmapInputResource(void *encoder, NV_ENC_INPUT_PTR mappedInputBuffer)
{
    return api.nvEncUnmapInputResource(encoder, mappedInputBuffer);
}

NVENCSTATUS NVENCAPI NvEncDestroyEncoder(void *encoder)
{
    return api.nvEncDestroyEncoder(encoder);
}

NVENCSTATUS NVENCAPI NvEncInvalidateRefFrames(void *encoder, uint64_t invalidRefFrameTimeStamp)
{
    return api.nvEncInvalidateRefFrames(encoder, invalidRefFrameTimeStamp);
}

NVENCSTATUS NVENCAPI NvEncOpenEncodeSessionEx(NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS *openSessionExParams, void **encoder)
{
    return api.nvEncOpenEncodeSessionEx(openSessionExParams, encoder);
}

NVENCSTATUS NVENCAPI NvEncRegisterResource(void *encoder, NV_ENC_REGISTER_RESOURCE *registerResParams)
{
    return api.nvEncRegisterResource(encoder, registerResParams);
}

NVENCSTATUS NVENCAPI NvEncUnregisterResource(void *encoder, NV_ENC_REGISTERED_PTR registeredResource)
{
    return api.nvEncUnregisterResource(encoder, registeredResource);
}

NVENCSTATUS NVENCAPI NvEncReconfigureEncoder(void *encoder, NV_ENC_RECONFIGURE_PARAMS *reInitEncodeParams)
{
    return api.nvEncReconfigureEncoder(encoder, reInitEncodeParams);
}

NVENCSTATUS NVENCAPI NvEncCreateMVBuffer(void *encoder, NV_ENC_CREATE_MV_BUFFER *createMVBufferParams)
{
    return api.nvEncCreateMVBuffer(encoder, createMVBufferParams);
}

NVENCSTATUS NVENCAPI NvEncDestroyMVBuffer(void *encoder, NV_ENC_OUTPUT_PTR mvBuffer)
{
    return api.nvEncDestroyMVBuffer(encoder, mvBuffer);
}

NVENCSTATUS NVENCAPI NvEncRunMotionEstimationOnly(void *encoder, NV_ENC_MEONLY_PARAMS *meOnlyParams)
{
    return api.nvEncRunMotionEstimationOnly(encoder, meOnlyParams);
}

NVENCSTATUS NVENCAPI NvEncodeAPIGetMaxSupportedVersion(uint32_t *version)
{
    nvenc_max_supported_version_func max_supported_version;
    max_supported_version = (nvenc_max_supported_version_func)DLL_LINK(dll_handle, "NvEncodeAPIGetMaxSupportedVersion");
    return max_supported_version(version);
}

const char *NVENCAPI NvEncGetLastErrorString(void *encoder)
{
    return api.nvEncGetLastErrorString(encoder);
}
