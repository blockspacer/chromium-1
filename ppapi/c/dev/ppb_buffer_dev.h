/* Copyright (c) 2010 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef PPAPI_C_DEV_PPB_BUFFER_DEV_H_
#define PPAPI_C_DEV_PPB_BUFFER_DEV_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_BUFFER_DEV_INTERFACE "PPB_Buffer(Dev);0.3"

struct PPB_Buffer_Dev {
  // Allocates a buffer of the given size in bytes. The return value will have
  // a non-zero ID on success, or zero on failure. Failure means the module
  // handle was invalid. The buffer will be initialized to contain zeroes.
  PP_Resource (*Create)(PP_Module module, uint32_t size_in_bytes);

  // Returns PP_TRUE if the given resource is a Buffer. Returns PP_FALSE if the
  // resource is invalid or some type other than a Buffer.
  PP_Bool (*IsBuffer)(PP_Resource resource);

  // Gets the size of the buffer. Returns PP_TRUE on success, PP_FALSE
  // if the resource is not a buffer. On failure, |*size_in_bytes| is not set.
  PP_Bool (*Describe)(PP_Resource resource, uint32_t* size_in_bytes);

  // Maps this buffer into the plugin address space and returns a pointer to the
  // beginning of the data.
  void* (*Map)(PP_Resource resource);

  void (*Unmap)(PP_Resource resource);
};

#endif  /* PPAPI_C_DEV_PPB_BUFFER_DEV_H_ */

