/******************************************************************************
 * @file: fir.c
 * @purpose: Core Map
 * @version: V1.0
 * @date:
 * @author:  Xinyi Yang, Andreas Mauderer, AE/EID2, Robert Bosch GmbH
 *----------------------------------------------------------------------------
 * implement the Nth-Order FIR Filter (N=40)
 * number of samples: 80
 *
 ******************************************************************************/
#include "global.h"

/***********************************************************
**  Filter Code Definitions
***********************************************************/
/* FIR default input data is stored in ROM
 * Maximum 80 samples can be processed */
extern const s16_t FilterInputDefault_as16[FILTER_INPUT_LEN];
extern const s16_t FilterOutputDefault_as16[FILTER_INPUT_LEN];

/* array to hold coefficients h(i) (data format [Q15.16]) */
const s32_t FirCoeffs_as32[FIR_LEN] =
{
 -1468, 1058,   594,   287,    186,  284,   485,   613,
   495,   90,  -435,  -762,   -615,   21,   821,  1269,
   982,    9,  -132, -1721,  -1296,    1,   145,  2136,
  1570,    0,  -166,  -213,  -1735,   -2,   170,  2512,
  1770,   -2,  -735,  -213,  -1666,    0,   570,  2136,
   445
};

/* ring buffers */
s16_t FirInput_as16[FIR_LEN*2];
s16_t FirOutput_as16[FIR_MAX_OUTPUT_LEN];

/*----------------------------------------------------------------------------
    initialize internal input table in RAM with '0', load first default data
 *----------------------------------------------------------------------------*/
void FirInputTableInit_v(u32_t RingBufPos_u32)
{
    u32_t i;
    for (i = 0; i < FIR_LEN; i++)
    /* initialize the input table all with '0' */
    {
        FirInput_as16[i] = 0;
    }
    /* load first input data x(0) (previous data are all '0's) */
    FirInput_as16[RingBufPos_u32] = FilterInputDefault_as16[0];
}

/*******************************************************************************
 * s16_t FirFixed_s16()
 * Parameters   : CurrIndex_u32 - Current index in input array
 *                RingBufPos_u32 - Current index in ring buffer
 * Return value : s16_t
 * Description  : the FIR filter function. It returns the output signal y(n)
 *                Note[TUDA]: changed this to s16. The MAC is 32-bit internally,
 *                but the FIR's I/O in this testcase is only 16-bit
 *******************************************************************************/
#ifndef ISAX

s16_t FirFixed_s16(u32_t CurrIndex_u32, u32_t RingBufPos_u32)
{
    u32_t k;                    /* filter length   index   */
    u32_t i   = RingBufPos_u32; /* ring buffer index */
    s32_t acc = 0;              /* accumulator for MACs */

    /* apply the filter to the input samples and calculate current output y[n] */
    for (k = 0; k < FIR_LEN; k++)
    {
        /* [Q0.31] += [Q0.15]*[Q0.15] */
        acc += ((s32_t)FirCoeffs_as32[k] * (s32_t)FirInput_as16[i]);

        /* update ring buffer index */
        i++;
        if (i == FIR_LEN) {
            i = 0;
        }
    }

    acc = acc >> 15; // Output Q0.15 value
    return acc;      // truncate upper bits!
}

#else

s16_t FirFixed_s16(u32_t CurrIndex_u32, u32_t RingBufPos_u32)
{
    u32_t k;                    /* filter length   index   */
    u32_t i   = RingBufPos_u32; /* ring buffer index */

    __isax_reset();

    /* apply the filter to the input samples and calculate current output y[n] */
    for (k = 0; k < FIR_LEN; k++)
    {
        /* [Q0.31] += [Q0.15]*[Q0.15] */
        __isax_mac(FirCoeffs_as32[k], FirInput_as16[i]);

        /* update ring buffer index */
        i++;
        if (i == FIR_LEN) {
            i = 0;
        }
    }

    return __isax_quant(); // Output Q0.15 value
}

#endif // ISAX

/*******************************************************************************
 * char CheckFirOutput(void)
 * Parameters   :
 * Return value : char
 * Description  : Compare results of the filter with default results
 *******************************************************************************/

char CheckFirOutput(void)
{
    int i;

    for (i = 0; i < FILTER_INPUT_LEN; i++)
    {
        if (FirOutput_as16[i] != FilterOutputDefault_as16[i])
            return 1;
    }
    return 0;
}


/*******************************************************************************
 * void FirMain_v( void )
 * Parameters  :
 * Description : the main function calling all KPI functions
 *******************************************************************************/
int FirMain_v( void )
{
    u32_t CurrIndex_u32;                /* the index n of input sample */
    u32_t RingBufPos_u32 = FIR_LEN - 1; /* current starting position in FIR ring buffer */
    s16_t firOutput;                    /* current FIR output */

    /* initialize the FIR filter input  */
    FirInputTableInit_v(RingBufPos_u32);

    /* perform the filtering and shift new sample in */
    for (CurrIndex_u32 = 0; CurrIndex_u32 < (FIR_MAX_INPUT_LEN); CurrIndex_u32++)
    {
        /* calculate one FIR sample */
        firOutput = FirFixed_s16(CurrIndex_u32, RingBufPos_u32);

        /* update starting position in ring buffer */
        if (RingBufPos_u32 == 0)
            RingBufPos_u32 = FIR_LEN - 1;
        else
            RingBufPos_u32 = RingBufPos_u32 - 1;

        /* load new input sample into ring buffer */
        FirInput_as16[RingBufPos_u32] = FilterInputDefault_as16[(CurrIndex_u32+1)%FIR_MAX_INPUT_LEN];

        /* write FIR output to output array in RAM */
        FirOutput_as16[CurrIndex_u32] = firOutput;
    }
    return CheckFirOutput();
}
