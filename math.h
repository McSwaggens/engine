#ifndef MATH_H
#define MATH_H

#include "general.h"

static f32 Fma(f32 a, f32 b, f32 c) { return __builtin_fma(a, b, c);   }
static f32 Abs(f32 f)               { return __builtin_fabs(f);        }
static f32 Ceil(f32 f)              { return __builtin_ceil(f);        }
static f32 Sin(f32 f)               { return __builtin_sin(f);         }
static f32 Cos(f32 f)               { return __builtin_cos(f);         }
static f32 Tan(f32 f)               { return __builtin_tan(f);         }
static f32 ASin(f32 f)              { return __builtin_asin(f);        }
static f32 ACos(f32 f)              { return __builtin_acos(f);        }
static f32 ATan(f32 f)              { return __builtin_atan(f);        }
static f32 ATan2(f32 y, f32 x)      { return __builtin_atan2(y, x);    }
static f32 SinH(f32 f)              { return __builtin_sinh(f);        }
static f32 CosH(f32 f)              { return __builtin_cosh(f);        }
static f32 TanH(f32 f)              { return __builtin_tanh(f);        }
static f32 Floor(f32 f)             { return __builtin_floor(f);       }
static f32 LogE(f32 f)              { return __builtin_log(f);         }
static f32 Log2(f32 f)              { return __builtin_log2(f);        }
static f32 Log10(f32 f)             { return __builtin_log10(f);       }
static f32 Pow(f32 x, f32 e)        { return __builtin_pow(x, e);      }
static f32 Exp(f32 f)               { return __builtin_exp(f);         }
static f32 Exp2(f32 f)              { return __builtin_exp2(f);        }
static f32 Sqrt(f32 f)              { return __builtin_sqrt(f);        }
static f32 RoundEven(f32 f)         { return __builtin_roundeven(f);   }
static f32 Round(f32 f)             { return __builtin_round(f);       }
static f32 Trunc(f32 f)             { return __builtin_trunc(f);       }
static f32 NearbyInt(f32 f)         { return __builtin_nearbyint(f);   }
static f32 CopySign(f32 x, f32 y)   { return __builtin_copysign(x, y); }
static f32 FMod(f32 x, f32 y)       { return __builtin_fmod(x, y);     }
static f32 Max(f32 a, f32 b)        { return __builtin_fmax(a, b);     }
static f32 Min(f32 a, f32 b)        { return __builtin_fmin(a, b);     }

static inline s8  Ctz8(s8 n)   { return n == 0 ? 8  : __builtin_ctz((u32)n)-24; };
static inline s16 Ctz16(s16 n) { return n == 0 ? 16 : __builtin_ctz((u32)n)-16; };
static inline s32 Ctz32(s32 n) { return n == 0 ? 32 : __builtin_ctz(n); };
static inline s64 Ctz64(s64 n) { return n == 0 ? 64 : __builtin_ctzll(n); };

static inline s8  Clz8(s8 n)   { return n == 0 ? 8  : __builtin_clz((u32)n)-24; };
static inline s16 Clz16(s16 n) { return n == 0 ? 16 : __builtin_clz((u32)n)-16; };
static inline s32 Clz32(s32 n) { return n == 0 ? 32 : __builtin_clz(n); };
static inline s64 Clz64(s64 n) { return n == 0 ? 64 : __builtin_clzll(n); };

static inline s32 Boi(s64 n)      { return 64-Clz64(n); }
static inline s32 PopCount(s32 n) { return __builtin_popcount(n); }
static inline s32 PopCount(s64 n) { return __builtin_popcountll(n); }
static inline s64 NextPow2(s64 n) { return 1llu << Boi(n); }

static inline s64 RoundPow2(s64 n) {
	if (PopCount(n) == 1)
		return n;

	return NextPow2(n);
}

#endif // MATH_H
