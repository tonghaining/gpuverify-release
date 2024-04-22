#ifndef CUDA_MATH_H
#define CUDA_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __THROW
#define __THROW throw()
#endif

/* INTEGER INTRINSICS */
__device__ unsigned int __brev(unsigned int x);
__device__ unsigned long long int __brevll(unsigned long long int x);
__device__ unsigned int __byte_perm (unsigned int x,unsigned int y, unsigned int s);
__device__ int __clz(int x);
__device__ int __clzll(long long int x);

static __device__ __inline__ int __ffs(int x) {
  return (sizeof(int) * 8) - __clz(x & (-x));
}

__device__ int __ffsll(long long int x);
__device__ long long int __mul64hi(long long int x, long long int y);
__device__ int __mulhi(int x, int y);
__device__ int __popc(unsigned int x);
__device__ int __popcll(unsigned long long int x);
__device__ unsigned int __sad(int x, int y, unsigned int z);
__device__ unsigned long long int __umul64hi(unsigned long long int x, unsigned long long int y);
__device__ unsigned int __umulhi(unsigned int x, unsigned int y);
__device__ unsigned int __usad(unsigned int x, unsigned int y, unsigned int z);

__device__ static __inline__ int __mul24(int x, int y) {
  /* assumptions:
     - result only well-defined for input values in range [-2^23, 2^23-1]
     - truncation to the 32 least significant bits of the result is automatic
  */
  //__unsafe_assert(x >= -8388608 && x < 8388608);
  //__unsafe_assert(y >= -8388608 && y < 8388608);
  return x * y;
}

static __device__ __inline__ unsigned int __umul24(unsigned int x, unsigned int y) {
  /* assumptions:
     - result only well-defined for input values in range [0, 2^24-1]
     - truncation to the 32 least significant bits of the result is automatic
  */
  //__unsafe_assert(x < 16777216);
  //__unsafe_assert(y < 16777216);
  return x * y;
}

/* TABLE C-1 */
__device__ float rsqrtf(float x);
__device__ float sqrtf(float x);
__device__ float cbrtf(float x);
__device__ float rcbrtf(float x);
__device__ float hypotf(float x, float y);
__device__ float expf(float x);
__device__ float exp2f(float x);
__device__ float exp10f(float x);
__device__ float expm1f(float x);
__device__ float logf(float x);
__device__ float log2f(float x);
__device__ float log10f(float x);
__device__ float log1pf(float x);
__device__ float sinf(float x);
__device__ float cosf(float x);
__device__ float tanf(float x);

static __device__ __inline__ void sincosf(float x, float *sptr, float *cptr) {
  *sptr = sinf(x);
  *cptr = cosf(x);
}

__device__ float sinpif(float x);
__device__ float cospif(float x);
__device__ float asinf(float x);
__device__ float acosf(float x);
__device__ float atanf(float x);
__device__ float atan2f(float y, float x);
__device__ float sinhf(float x);
__device__ float coshf(float x);
__device__ float tanhf(float x);
__device__ float asinhf(float x);
__device__ float acoshf(float x);
__device__ float atanhf(float x);
__device__ float powf(float x, float y);
__device__ float erff(float x);
__device__ float erfcf(float x);
__device__ float erfinvf(float x);
__device__ float erfcinvf(float x);
__device__ float erfcxf(float x);
__device__ float lgammaf(float x);
__device__ float tgammaf(float x);
__device__ float fmaf(float x, float y, float z);

__device__ int __bugle_frexpf_exp(float x);
__device__ float __bugle_frexpf_frac(float x);

static __device__ __inline__ float frexpf(float x, int *exp) {
  *exp = __bugle_frexpf_exp(x);
  return __bugle_frexpf_frac(x);
}

__device__ float ldexpf(float x, int exp);
__device__ float scalbnf(float x, int n);
__device__ float scalblnf(float x, long int l);
__device__ float logbf(float x);
__device__ int   ilogbf(float x);
__device__ float j0f(float x);
__device__ float j1f(float x);
__device__ float jnf(float x);
__device__ float y0f(float x);
__device__ float y1f(float x);
__device__ float ynf(int x, float y);
__device__ float fmodf(float x, float y);
__device__ float remainderf(float x, float y);

__device__ int __bugle_remquof_quo(float x, float y);

static __device__ __inline__ float remquof(float x, float y, int *iptr) {
  *iptr = __bugle_remquof_quo(x, y);
  return remainderf(x, y);
}

__device__ float __bugle_modff_ipart(float x);
__device__ float __bugle_modff_frac(float x);

static __device__ __inline__ float modff(float x, float *iptr) {
  *iptr = __bugle_modff_ipart(x);
  return __bugle_modff_frac(x);
}

__device__ float fdimf(float x, float y);
__device__ float truncf(float x);
__device__ float roundf(float x);
__device__ float rintf(float x);
__device__ float nearbyintf(float x);
__device__ float ceilf(float x);
__device__ float floorf(float x);
__device__ long int lrintf(float x);
__device__ long int  lroundf(float x);
__device__ long long int llrintf(float x);
__device__ long long int llroundf(float x);
__device__ float fabsf(float x);
__device__ float fmaxf(float x, float y);
__device__ float fminf(float x, float y);

__device__ int isnan(float x);

/* TABLE C-2 */
__device__ double rsqrt(double x);
__device__ double sqrt(double x) __THROW;
__device__ double cbrt(double x);
__device__ double rcbrt(double x);
__device__ double hypot(double x, double y);
__device__ double exp(double x) __THROW;
__device__ double exp2(double x);
__device__ double exp10(double x);
__device__ double expm1(double x);
__device__ double log(double x) __THROW;
__device__ double log2(double x);
__device__ double log10(double x) __THROW;
__device__ double log1p(double x);
__device__ double sin(double x) __THROW;
__device__ double cos(double x) __THROW;
__device__ double tan(double x) __THROW;

static __device__ __inline__ void sincos(double x, double *sptr, double *cptr) {
  *sptr = sin(x);
  *cptr = cos(x);
}

__device__ double sinpi(double x);
__device__ double cospi(double x);
__device__ double asin(double x) __THROW;
__device__ double acos(double x) __THROW;
__device__ double atan(double x) __THROW;
__device__ double atan2(double y, double x) __THROW;
__device__ double sinh(double x) __THROW;
__device__ double cosh(double x) __THROW;
__device__ double tanh(double x) __THROW;
__device__ double asinh(double x);
__device__ double acosh(double x);
__device__ double atanh(double x);
__device__ double pow(double x, double y) __THROW;
__device__ double erf(double x);
__device__ double erfc(double x);
__device__ double erfinv(double x);
__device__ double erfcinv(double x);
__device__ double erfcx(double x);
__device__ double lgamma(double x);
__device__ double tgamma(double x);
__device__ double fma(double x, double y, double z);

__device__ int __bugle_frexp_exp(double x) __THROW;
__device__ double __bugle_frexp_frac(double x) __THROW;

static __device__ __inline__ double frexp(double x, int *exp) __THROW {
  *exp = __bugle_frexp_exp(x);
  return __bugle_frexp_frac(x);
}

__device__ double ldexp(double x, int exp) __THROW;
__device__ double scalbn(double x, int n);
__device__ double scalbln(double x, long int l);
__device__ double logb(double x);
__device__ int    ilogb(double x);
__device__ double j0(double x);
__device__ double j1(double x);
__device__ double jn(int x, double y);
__device__ double y0(double x);
__device__ double y1(double x);
__device__ double yn(int x, double y);
__device__ double fmod(double x, double y) __THROW;
__device__ double remainder(double x, double y);

__device__ int __bugle_remquo_quo(double x, double y);

static __device__ __inline__ double remquo(double x, double y, int *iptr) {
  *iptr = __bugle_remquo_quo(x, y);
  return remainder(x, y);
}

__device__ double __bugle_modf_ipart(double x) __THROW;
__device__ double __bugle_modf_frac(double x) __THROW;

static __device__ __inline__ double modf(double x, double *iptr) __THROW {
  *iptr = __bugle_modf_ipart(x);
  return __bugle_modf_frac(x);
}

__device__ double fdim(double x, double y);
__device__ double trunc(double x);
__device__ double round(double x);
__device__ double rint(double x);
__device__ double nearbyint(double x);
__device__ double ceil(double x) __THROW;
__device__ double floor(double x) __THROW;
__device__ long int lrint(double x);
__device__ long int  lround(double x);
__device__ long long int llrint(double x);
__device__ long long int llround(double x);
__device__ double fabs(double x) __THROW;
__device__ double fmax(double x, double y);
__device__ double fmin(double x, double y);

/* Table C-3 and C-4 */
__device__ float __fdividef(float x, float y);
__device__ float __sinf(float x);
__device__ float __cosf(float x);
__device__ float __tanf(float x);

static __device__ __inline__ void __sincosf(float x, float *sptr, float *cptr) {
  *sptr = sinf(x);
  *cptr = cosf(x);
}

__device__ float __logf(float x);
__device__ float __log2f(float x);
__device__ float __log10f(float x);
__device__ float __expf(float x);
__device__ float __exp10f(float x);
__device__ float __powf(float x, float y);
__device__ float __saturatef(float x);

__device__ float __fadd_rn(float x, float y);
__device__ float __fadd_rz(float x, float y);
__device__ float __fadd_ru(float x, float y);
__device__ float __fadd_rd(float x, float y);
__device__ float __fmul_rn(float x, float y);
__device__ float __fmul_rz(float x, float y);
__device__ float __fmul_ru(float x, float y);
__device__ float __fmul_rd(float x, float y);
__device__ float __fmaf_rn(float x, float y, float z);
__device__ float __fmaf_rz(float x, float y, float z);
__device__ float __fmaf_ru(float x, float y, float z);
__device__ float __fmaf_rd(float x, float y, float z);
__device__ float __frcp_rn(float x);
__device__ float __frcp_rz(float x);
__device__ float __frcp_ru(float x);
__device__ float __frcp_rd(float x);
__device__ float __fsqrt_rn(float x);
__device__ float __fsqrt_rz(float x);
__device__ float __fsqrt_ru(float x);
__device__ float __fsqrt_rd(float x);
__device__ float __fdiv_rn(float x, float y);
__device__ float __fdiv_rz(float x, float y);
__device__ float __fdiv_ru(float x, float y);
__device__ float __fdiv_rd(float x, float y);
// __dividef above
// __expf above
// __exp10f above
// __logf above
// __log2f above
// __log10f above
// __sinf above
// __cosf above
// __sincosf above
// __tanf above
// __powf above

/* Table C-5 */
__device__ double __dadd_rn(double x, double y);
__device__ double __dadd_rz(double x, double y);
__device__ double __dadd_ru(double x, double y);
__device__ double __dadd_rd(double x, double y);
__device__ double __dmul_rn(double x, double y);
__device__ double __dmul_rz(double x, double y);
__device__ double __dmul_ru(double x, double y);
__device__ double __dmul_rd(double x, double y);
__device__ double __fma_rn(double x, double y, double z);
__device__ double __fma_rz(double x, double y, double z);
__device__ double __fma_ru(double x, double y, double z);
__device__ double __fma_rd(double x, double y, double z);
__device__ double __ddiv_rn(double x, double y);
__device__ double __ddiv_rz(double x, double y);
__device__ double __ddiv_ru(double x, double y);
__device__ double __ddiv_rd(double x, double y);
__device__ double __drcp_rn(double x);
__device__ double __drcp_rz(double x);
__device__ double __drcp_ru(double x);
__device__ double __drcp_rd(double x);
__device__ double __dsqrt_rn(double x);
__device__ double __dsqrt_rz(double x);
__device__ double __dsqrt_ru(double x);
__device__ double __dsqrt_rd(double x);

/* Overloaded functions */

__device__ __attribute__((overloadable)) float abs(float x);
__device__ __attribute__((overloadable)) int abs(int x);
__device__ float saturate(float x);
__device__ int max(int x, int y);

/* min functions */

__device__ __attribute__((overloadable)) int min(int x, int y);
__device__ __attribute__((overloadable)) unsigned int min(unsigned int x, unsigned int y);
__device__ __attribute__((overloadable)) unsigned int min(int x, unsigned int y);
__device__ __attribute__((overloadable)) unsigned int min(unsigned int x, int y);
__device__ __attribute__((overloadable)) long long int min(long long int x, long long int y);
__device__ __attribute__((overloadable)) unsigned long long int min(unsigned long long int x, unsigned long long int y);
__device__ __attribute__((overloadable)) unsigned long long int min(long long int x, unsigned long long int y);
__device__ __attribute__((overloadable)) unsigned long long int min(unsigned long long int x, long long int y);
__device__ __attribute__((overloadable)) float min(float x, float y);
__device__ __attribute__((overloadable)) double min(double x, double y);
__device__ __attribute__((overloadable)) double min(float x, double y);
__device__ __attribute__((overloadable)) double min(double x, float y);

__device__ unsigned int umin(unsigned int x, unsigned int y);
__device__ long long int llmin(long long int x, long long int y);
__device__ unsigned long long int ullmin(unsigned long long int x, unsigned long long int y);

/* Type Casting Intrinsics */

__device__ int __float2int_rn(float x);
__device__ float __int_as_float(int x);

#ifdef __cplusplus
}
#endif

#endif
