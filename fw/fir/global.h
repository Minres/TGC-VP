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
#define FIR_ORDER 40
#define FIR_LEN             (FIR_ORDER+1)  /* length = N+1  */

int FirMain_v( void );

#endif // __GLOBAL_H__

