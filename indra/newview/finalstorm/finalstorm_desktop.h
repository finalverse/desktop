/* Copyright 2026 Finalverse contributors. MIT OR Apache-2.0. */
#ifndef FINALSTORM_DESKTOP_H
#define FINALSTORM_DESKTOP_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* ABI 1. All handles belong to one caller thread, no overlapping operations.
 * Callers own all buffers and must provide valid pointers for their lengths.
 * render writes tightly packed top-down RGBA8 sRGB and NUL-terminated JSON.
 * 0=success, -1=invalid input/budget/session, -2=GPU/internal failure.
 * Failure invalidates the displayed frame. No exception/panic crosses the ABI.
 * No credentials, network operations or world writes are accepted by this ABI.
 */
uint32_t fs_desktop_abi(void);
void* fs_desktop_create(uint32_t width, uint32_t height);
int32_t fs_desktop_render(void* handle, const uint8_t* request, size_t request_len,
                         uint8_t* rgba, size_t rgba_capacity,
                         char* report, size_t report_capacity);
void fs_desktop_destroy(void* handle);
#ifdef __cplusplus
}
#endif
#endif
