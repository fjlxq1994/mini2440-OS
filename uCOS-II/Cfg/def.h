/**************************************************************************
*                                                                         *
*   PROJECT     : ARM port for UCOS-II                                    *
*                                                                         *
*   MODULE      : DEF.h                                                   *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : Any Processor                                           *
*                                                                         *
*   IDE         : Any IDE                                                 *
*                                                                         *
*   DESCRIPTION :                                                         *
*   Common data type definition header file.                              *
*                                                                         *
**************************************************************************/


#ifndef __DEF_H__
#define __DEF_H__

#ifdef __cplusplus
extern "C" {
#endif


/* ********************************************************************* */
/* Module configuration */


/* ********************************************************************* */
/* Interface macro & data definition */

/* ARM modes */
#define USERMODE        0x10
#define FIQMODE         0x11
#define IRQMODE         0x12
#define SVCMODE         0x13
#define ABORTMODE       0x17
#define UNDEFMODE       0x1b
#define MODEMASK        0x1f
#define NOINT           0xc0
#define TBIT            0x20

#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define S16 short int
#define U8  unsigned char
#define S8  signed char

#define False 0
#define True !False

/* Sets the result on bPort */
#define BIT_SET(bPort,bBitMask)        (bPort |= bBitMask)
#define BIT_CLR(bPort,bBitMask)        (bPort &= ~bBitMask)

/* Returns the result */
#define GET_BIT_SET(bPort,bBitMask)    (bPort | bBitMask)
#define GET_BIT_CLR(bPort,bBitMask)    (bPort & ~bBitMask)

/* Returns 0 if the condition is False & a non-zero value if it is True */
#define TEST_BIT_SET(bPort,bBitMask)   (bPort & bBitMask)
#define TEST_BIT_CLR(bPort,bBitMask)   ((~bPort) & bBitMask)


/* ********************************************************************* */
/* Interface function definition */


/* ********************************************************************* */

#ifdef __cplusplus
}
#endif

#endif /*__DEF_H__*/

