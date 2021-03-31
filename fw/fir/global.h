/******************************************************************************
 * @file: global.h
 * @purpose: Core Map
 * @version: V1.0
 * @date:
 * @author:  Xinyi Yang, AE/EIS3, Robert Bosch GmbH
 *----------------------------------------------------------------------------
 * global definition
 *
 ******************************************************************************/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

typedef signed short        s16_t;  /* _s16 */
typedef unsigned short      u16_t;  /* _u16 */
typedef signed int          s32_t;  /* _s32 */
typedef unsigned int        u32_t;  /* _u32 */

/* number of inputs that is stored in ROM */
#define FILTER_INPUT_LEN    80

/*************************** FIR related *********************************/
#define FIR_MAX_INPUT_LEN   80
#define FIR_MAX_OUTPUT_LEN  80
#define FIR_ORDER 20
#define FIR_LEN             (FIR_ORDER+1)  /* length = N+1  */
#define ISAX
#ifdef ISAX
/// Fake ISAX for testing /////////////////////////////////////////////////////
static        s32_t __isax_acc;
static inline void  __isax_reset()                       { __isax_acc = 0; }
static inline void  __isax_mac(s32_t coeff, s16_t input) { __isax_acc += coeff * input; }
//static inline void  __isax_mac(s32_t coeff, s16_t input) {
//    asm("mov    %[result], %[value], ror #1"
//
//               : [result]"=r" (y) /* Rotation result. */
//               : [value]"r"   (x) /* Rotated value. */
//               : /* No clobbers */
//        );
//}
static inline s16_t __isax_quant()                       { return (s16_t) (__isax_acc >> 15); }
///////////////////////////////////////////////////////////////////////////////
#endif

int FirMain_v( void );

#endif // __GLOBAL_H__

