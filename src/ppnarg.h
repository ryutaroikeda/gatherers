#ifndef __ppnarg_h__
#define __ppnarg_h__

#define ppnarg(...) \
ppnarg_(__VA_ARGS__,pprseqn())

#define ppnarg_(...) \
ppargn(__VA_ARGS__)

#define ppargn( \
_1, _2, _3, _4, _5, _6, _7, _8, \
_9,_10,_11,_12,_13,_14,_15,_16, \

)

#endif
