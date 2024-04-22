#ifndef OPENCL_H
#define OPENCL_H

#pragma GCC diagnostic error "-Wimplicit-function-declaration"

#ifndef __OPENCL_VERSION__
#error __OPENCL_VERSION__ must be defined
#endif

#ifdef __CUDA_ARCH__
#error Cannot include both opencl.h and cuda.h
#endif

#include <bugle.h>

// override the default event_t implementation
#define event_t __bugle_event_t
typedef __SIZE_TYPE__ event_t;
// include libclc headers
#include <clc/clc.h>

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#include <annotations/annotations.h>
#include <opencl_limits.h>
#include <opencl_atomics.h>

/* Images */
#define image2d_t __bugle_image2d_t
#define image3d_t __bugle_image3d_t

#define __write_only
#define write_only
#define __read_only
#define read_only

typedef __global uint4 *image2d_t;
typedef __global uint4 *image3d_t;

#ifndef CL_DEVICE_IMAGE1D_MAX_WIDTH
#define CL_DEVICE_IMAGE1D_MAX_WIDTH (1 << 27)
#endif

#ifndef CL_DEVICE_IMAGE2D_MAX_WIDTH
#define CL_DEVICE_IMAGE2D_MAX_WIDTH (1 << 13)
#endif

#ifndef CL_DEVICE_IMAGE2D_MAX_HEIGHT
#define CL_DEVICE_IMAGE2D_MAX_HEIGHT (1 << 13)
#endif

#ifndef CL_DEVICE_IMAGE3D_MAX_WIDTH
#define CL_DEVICE_IMAGE3D_MAX_WIDTH (1 << 9)
#endif

#ifndef CL_DEVICE_IMAGE3D_MAX_HEIGHT
#define CL_DEVICE_IMAGE3D_MAX_HEIGHT (1 << 9)
#endif

#ifndef CL_DEVICE_IMAGE3D_MAX_DEPTH
#define CL_DEVICE_IMAGE3D_MAX_DEPTH (1 << 9)
#endif

#define __image_clamp(x, MAX) (unsigned)((x) < 0 ? 0 : ( (x) >= (MAX) ? (MAX) - 1 : (x) ))

#define READ_IMAGE_2D(NAME, COLOUR_TYPE, COORD_TYPE) \
_CLC_INLINE _CLC_OVERLOAD COLOUR_TYPE NAME(image2d_t image, sampler_t sampler, COORD_TYPE coord) { \
  unsigned __x = __image_clamp((int)coord.x, CL_DEVICE_IMAGE2D_MAX_WIDTH); \
  unsigned __y = __image_clamp((int)coord.y, CL_DEVICE_IMAGE2D_MAX_HEIGHT); \
  return as_##COLOUR_TYPE(image[__y*CL_DEVICE_IMAGE2D_MAX_WIDTH + __x]); \
}

READ_IMAGE_2D(read_imagef, float4, int2)
READ_IMAGE_2D(read_imagef, float4, float2)
READ_IMAGE_2D(read_imagei, int4, int2)
READ_IMAGE_2D(read_imagei, int4, float2)
READ_IMAGE_2D(read_imageui, uint4, int2)

#define WRITE_IMAGE_2D(NAME, COLOUR_TYPE, COORD_TYPE) \
_CLC_INLINE _CLC_OVERLOAD void NAME(image2d_t image, COORD_TYPE coord, COLOUR_TYPE color) { \
  image[coord.y*CL_DEVICE_IMAGE2D_MAX_WIDTH + coord.x] = as_uint4(color); \
}

WRITE_IMAGE_2D(write_imagef, float4, int2)
WRITE_IMAGE_2D(write_imagei, int4, int2)
WRITE_IMAGE_2D(write_imageui, uint4, int2)

#define READ_IMAGE_3D(NAME, COLOUR_TYPE, COORD_TYPE) \
_CLC_INLINE _CLC_OVERLOAD COLOUR_TYPE NAME(image3d_t image, sampler_t sampler, COORD_TYPE coord) { \
  unsigned __x = __image_clamp((int)coord.x, CL_DEVICE_IMAGE3D_MAX_WIDTH); \
  unsigned __y = __image_clamp((int)coord.y, CL_DEVICE_IMAGE3D_MAX_HEIGHT); \
  unsigned __z = __image_clamp((int)coord.z, CL_DEVICE_IMAGE3D_MAX_DEPTH); \
  return as_##COLOUR_TYPE(image[(__z*CL_DEVICE_IMAGE3D_MAX_HEIGHT + __y)*CL_DEVICE_IMAGE3D_MAX_WIDTH + __x]); \
}

READ_IMAGE_3D(read_imagef, float4, int4)
READ_IMAGE_3D(read_imagef, float4, float4)
READ_IMAGE_3D(read_imagei, int4, int4)
READ_IMAGE_3D(read_imagei, int4, float4)
READ_IMAGE_3D(read_imageui, uint4, int4)
READ_IMAGE_3D(read_imageui, uint4, float4)

#define WRITE_IMAGE_3D(NAME, COLOUR_TYPE, COORD_TYPE) \
_CLC_INLINE _CLC_OVERLOAD void NAME(image3d_t image, COORD_TYPE coord, COLOUR_TYPE color) { \
  image[(coord.z*CL_DEVICE_IMAGE3D_MAX_HEIGHT + coord.y)*CL_DEVICE_IMAGE3D_MAX_WIDTH + coord.x] = as_uint4(color); \
}

WRITE_IMAGE_3D(write_imagef, float4, int4)
WRITE_IMAGE_3D(write_imagei, int4, int4)
WRITE_IMAGE_3D(write_imageui, uint4, int4)

int get_image_height(image2d_t image);
int get_image_width(image2d_t image);

// Must define a dimension

#ifndef __1D_WORK_GROUP
#ifndef __2D_WORK_GROUP
#ifndef __3D_WORK_GROUP

#error You must specify the dimension of a work group by defining one of __1D_WORK_GROUP, __2D_WORK_GROUP or __3D_WORK_GROUP

#endif
#endif
#endif

// Must define only one dimension

#ifdef __1D_WORK_GROUP
#ifdef __2D_WORK_GROUP
#error Cannot define __1D_WORK_GROUP and __2D_WORK_GROUP
#endif
#ifdef __3D_WORK_GROUP
#error Cannot define __1D_WORK_GROUP and __3D_WORK_GROUP
#endif
#endif

#ifdef __2D_WORK_GROUP
#ifdef __1D_WORK_GROUP
#error Cannot define __2D_WORK_GROUP and __1D_WORK_GROUP
#endif
#ifdef __3D_WORK_GROUP
#error Cannot define __2D_WORK_GROUP and __3D_WORK_GROUP
#endif
#endif

#ifdef __3D_WORK_GROUP
#ifdef __1D_WORK_GROUP
#error Cannot define __3D_WORK_GROUP and __1D_WORK_GROUP
#endif
#ifdef __2D_WORK_GROUP
#error Cannot define __3D_WORK_GROUP and __2D_WORK_GROUP
#endif
#endif

// Generate axioms for different work group sizes

#ifdef __1D_WORK_GROUP
__axiom(get_local_size(1) == 1)
__axiom(get_local_size(2) == 1)
#endif

#ifdef __2D_WORK_GROUP
__axiom(get_local_size(2) == 1)
#endif


/* Work group grid dimensions */

// Must define a dimension

#ifndef __1D_GRID
#ifndef __2D_GRID
#ifndef __3D_GRID

#error You must specify the dimension of the grid of work groups by defining one of __1D_GRID, __2D_GRID or __3D_GRID

#endif
#endif
#endif

// Must define only one dimension

#ifdef __1D_GRID
#ifdef __2D_GRID
#error Cannot define __1D_GRID and __2D_GRID
#endif
#ifdef __3D_GRID
#error Cannot define __1D_GRID and __3D_GRID
#endif
#endif

#ifdef __2D_GRID
#ifdef __1D_GRID
#error Cannot define __2D_GRID and __1D_GRID
#endif
#ifdef __3D_GRID
#error Cannot define __2D_GRID and __3D_GRID
#endif
#endif

#ifdef __3D_GRID
#ifdef __1D_GRID
#error Cannot define __3D_GRID and __1D_GRID
#endif
#ifdef __2D_GRID
#error Cannot define __3D_GRID and __2D_GRID
#endif
#endif

// Generate axioms for different grid sizes

#ifdef __1D_GRID
__axiom(get_num_groups(1) == 1)
__axiom(get_num_groups(2) == 1)
__axiom(get_work_dim() == 1)
#endif

#ifdef __2D_GRID
__axiom(get_num_groups(2) == 1)
__axiom(get_work_dim() == 2)
#endif

#ifdef __3D_GRID
__axiom(get_work_dim() == 3)
#endif

// Generate axioms for input values

#if defined(__LOCAL_SIZE_0) && defined(__LOCAL_SIZE_0_FREE)
#error Cannot define __LOCAL_SIZE_0 and __LOCAL_SIZE_0_FREE
#elif defined(__LOCAL_SIZE_0)
__axiom(get_local_size(0) == __LOCAL_SIZE_0)
#elif defined(__LOCAL_SIZE_0_FREE)
__axiom(get_local_size(0) > 0)
#endif

#if defined(__LOCAL_SIZE_1) && defined(__LOCAL_SIZE_1_FREE)
#error Cannot define __LOCAL_SIZE_1 and __LOCAL_SIZE_1_FREE
#elif defined(__LOCAL_SIZE_1)
__axiom(get_local_size(1) == __LOCAL_SIZE_1)
#elif defined(__LOCAL_SIZE_1_FREE)
__axiom(get_local_size(1) > 0)
#endif

#if defined(__LOCAL_SIZE_2) && defined(__LOCAL_SIZE_2_FREE)
#error Cannot define __LOCAL_SIZE_2 and __LOCAL_SIZE_2_FREE
#elif defined(__LOCAL_SIZE_2)
__axiom(get_local_size(2) == __LOCAL_SIZE_2)
#elif defined(__LOCAL_SIZE_2_FREE)
__axiom(get_local_size(2) > 0)
#endif

#if defined(__NUM_GROUPS_0) && defined(__NUM_GROUPS_0_FREE)
#error Cannot define __NUM_GROUPS_0 and __NUM_GROUPS_0_FREE
#elif defined(__NUM_GROUPS_0)
__axiom(get_num_groups(0) == __NUM_GROUPS_0)
#elif defined(__NUM_GROUPS_0_FREE)
#ifndef __GLOBAL_SIZE_0
__axiom(get_num_groups(0) > 0)
#else
__axiom(get_local_size(0) <= __GLOBAL_SIZE_0)
__axiom(__GLOBAL_SIZE_0 % get_local_size(0) == 0)
__axiom(get_num_groups(0) == __GLOBAL_SIZE_0 / get_local_size(0))
#endif
#endif

#if defined(__NUM_GROUPS_1) && defined(__NUM_GROUPS_1_FREE)
#error Cannot define __NUM_GROUPS_1 and __NUM_GROUPS_1_FREE
#elif defined(__NUM_GROUPS_1)
__axiom(get_num_groups(1) == __NUM_GROUPS_1)
#elif defined(__NUM_GROUPS_1_FREE)
#ifndef __GLOBAL_SIZE_1
__axiom(get_num_groups(1) > 0)
#else
__axiom(get_local_size(1) <= __GLOBAL_SIZE_0)
__axiom(__GLOBAL_SIZE_1 % get_local_size(1) == 0)
__axiom(get_num_groups(1) == __GLOBAL_SIZE_1 / get_local_size(1))
#endif
#endif

#if defined(__NUM_GROUPS_2) && defined(__NUM_GROUPS_2_FREE)
#error Cannot define __NUM_GROUPS_2 and __NUM_GROUPS_2_FREE
#elif defined(__NUM_GROUPS_2)
__axiom(get_num_groups(2) == __NUM_GROUPS_2)
#elif defined(__NUM_GROUPS_2_FREE)
#ifndef __GLOBAL_SIZE_2
__axiom(get_num_groups(2) > 0)
#else
__axiom(get_local_size(2) <= __GLOBAL_SIZE_2)
__axiom(__GLOBAL_SIZE_2 % get_local_size(2) == 0)
__axiom(get_num_groups(2) == __GLOBAL_SIZE_2 / get_local_size(2))
#endif
#endif

// Global id and offset

size_t get_global_offset(uint dim);

#define get_global_id(X) __bugle_get_global_id(X)

_CLC_INLINE size_t get_global_id(uint dim) {
#if defined(__GLOBAL_OFFSET_0) || defined(__GLOBAL_OFFSET_1) || defined(__GLOBAL_OFFSET_2)
  return get_group_id(dim)*get_local_size(dim) + get_local_id(dim) + get_global_offset(dim);
#else
  return get_group_id(dim)*get_local_size(dim) + get_local_id(dim);
#endif
}

#ifdef __GLOBAL_OFFSET_0
__axiom(get_global_offset(0) == __GLOBAL_OFFSET_0)
#else
__axiom(get_global_offset(0) == 0)
#endif

#ifdef __GLOBAL_OFFSET_1
__axiom(get_global_offset(1) == __GLOBAL_OFFSET_1)
#else
__axiom(get_global_offset(1) == 0)
#endif

#ifdef __GLOBAL_OFFSET_2
__axiom(get_global_offset(2) == __GLOBAL_OFFSET_2)
#else
__axiom(get_global_offset(2) == 0)
#endif

#endif
