#pragma once

//#include <softfloat.hpp>
//#include <dipc/dipc.hpp>

//#include <softfloat.hpp>
#include <softfloat_types.h>

#include <stdint.h>

using uint128_t = unsigned __int128;
using int128_t =  __int128;

#ifdef __cplusplus
extern "C" {
#endif
   __int128 ___fixdfti(uint64_t);
   __int128 ___fixsfti(uint32_t);
   __int128 ___fixtfti( float128_t);
   unsigned __int128 ___fixunsdfti(uint64_t);
   unsigned __int128 ___fixunssfti(uint32_t);
   unsigned __int128 ___fixunstfti(float128_t);
   double ___floattidf(__int128);
   double ___floatuntidf(unsigned __int128);
   //for test
   //void __testInt128(__int128& ret);
   //void __ashlti3(__int128& ret, uint64_t low, uint64_t high, uint32_t shift);
#ifdef __cplusplus
}
#endif

