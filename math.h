#ifndef MATH_H
#define MATH_H

#include "general.h"

static float32 Fma(float32 a, float32 b, float32 c) { return __builtin_fma(a, b, c);   }
static float32 Abs(float32 f)                       { return __builtin_fabs(f);        }
static float32 Ceil(float32 f)                      { return __builtin_ceil(f);        }
static float32 Sin(float32 f)                       { return __builtin_sin(f);         }
static float32 Cos(float32 f)                       { return __builtin_cos(f);         }
static float32 Tan(float32 f)                       { return __builtin_tan(f);         }
static float32 ASin(float32 f)                      { return __builtin_asin(f);        }
static float32 ACos(float32 f)                      { return __builtin_acos(f);        }
static float32 ATan(float32 f)                      { return __builtin_atan(f);        }
static float32 ATan2(float32 y, float32 x)          { return __builtin_atan2(y, x);    }
static float32 SinH(float32 f)                      { return __builtin_sinh(f);        }
static float32 CosH(float32 f)                      { return __builtin_cosh(f);        }
static float32 TanH(float32 f)                      { return __builtin_tanh(f);        }
static float32 Floor(float32 f)                     { return __builtin_floor(f);       }
static float32 LogE(float32 f)                      { return __builtin_log(f);         }
static float32 Log2(float32 f)                      { return __builtin_log2(f);        }
static float32 Log10(float32 f)                     { return __builtin_log10(f);       }
static float32 Pow(float32 x, float32 e)            { return __builtin_pow(x, e);      }
static float32 Exp(float32 f)                       { return __builtin_exp(f);         }
static float32 Exp2(float32 f)                      { return __builtin_exp2(f);        }
static float32 Sqrt(float32 f)                      { return __builtin_sqrt(f);        }
static float32 RoundEven(float32 f)                 { return __builtin_roundeven(f);   }
static float32 Round(float32 f)                     { return __builtin_round(f);       }
static float32 Trunc(float32 f)                     { return __builtin_trunc(f);       }
static float32 NearbyInt(float32 f)                 { return __builtin_nearbyint(f);   }
static float32 CopySign(float32 x, float32 y)       { return __builtin_copysign(x, y); }
static float32 FMod(float32 x, float32 y)           { return __builtin_fmod(x, y);     }
static float32 Max(float32 a, float32 b)            { return __builtin_fmax(a, b);     }
static float32 Min(float32 a, float32 b)            { return __builtin_fmin(a, b);     }

static inline s8  Ctz8(s8 n)   { return n == 0 ? 8  : __builtin_ctz((u32)n)-24; };
static inline s16 Ctz16(s16 n) { return n == 0 ? 16 : __builtin_ctz((u32)n)-16; };
static inline s32 Ctz32(s32 n) { return n == 0 ? 32 : __builtin_ctz(n); };
static inline s64 Ctz64(s64 n) { return n == 0 ? 64 : __builtin_ctzll(n); };

static inline s8  Clz8(s8 n)   { return n == 0 ? 8  : __builtin_clz((u32)n)-24; };
static inline s16 Clz16(s16 n) { return n == 0 ? 16 : __builtin_clz((u32)n)-16; };
static inline s32 Clz32(s32 n) { return n == 0 ? 32 : __builtin_clz(n); };
static inline s64 Clz64(s64 n) { return n == 0 ? 64 : __builtin_clzll(n); };

static inline s32 Boi(s64 n)      { return 64-Clz64(n); }
static inline s32 NextPow2(s64 n) { return 1llu << Boi(n); }
static inline s32 PopCount(s32 n) { return __builtin_popcount(n); }
static inline s32 PopCount(s64 n) { return __builtin_popcountll(n); }

#endif // MATH_H
