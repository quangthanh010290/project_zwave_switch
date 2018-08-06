/****************************************************************************
 *
 * Copyright (c) 2001-2012
 * Sigma Designs, Inc.
 * All Rights Reserved
 *
 *---------------------------------------------------------------------------
 * Copyright Zensys A/S, 2001
 *
 * Description: Include file for LEDdimmer module
 *
 * Author:   Johann Sigfredsson
 *
 * Last Changed By:  $Author: sar $
 * Revision:         $Revision: 24585 $
 * Last Changed:     $Date: 2013-02-25 11:46:45 +0100 (ma, 25 feb 2013) $
 *
 ****************************************************************************/
#ifndef _LEDDIM_H_
#define _LEDDIM_H_


/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/* Protection level definitions */
#define PROTECTION_OFF                                  0x00

/* Binary Switch definitions */
#define SWITCHED_ON                                     0xFF
#define SWITCHED_OFF                                    0x00

/* Power level definitions */
#define ZW_TEST_FAILED                                  0x00
#define ZW_TEST_SUCCES                                  0x01
#define ZW_TEST_INPROGRESS                              0x02
#define ZW_TEST_NOT_A_NODEID                            0x00


/* Offsets into frame received */
#define OFFSET_CLASSCMD                       0x00
#define OFFSET_CMD                            0x01
#define OFFSET_PARAM_1                        0x02
#define OFFSET_PARAM_2                        0x03
#define OFFSET_PARAM_3                        0x04
#define OFFSET_PARAM_4                        0x05

/* Size of Report (ALL/LOCK/MULTI/BASIC GET response) frame payload */
#define REPORTSIZE                            3

/* Report command definition - ALL/LOCK/MULTI report use the same value ;-) */


/* dimStatus definitions :
   bit7: Rollover, DS_ROLLOVER
   bit6: up_down, DS_UP_DOWN
   bit5: Changing dim, DS_CHANGING_DIM
   bit4: on_off, DS_ON_OFF
 */

#define DS_ROLLOVER         7   /* if 1 then on reaching boundary change dim
direction, if 0 then stop dimming */
#define DS_UP_DOWN          6   /* if 1 dim down, if 0 dim up */
#define DS_CHANGING_DIM     5   /* are we dimming ? */
#define DS_ON_OFF           4   /* state of dimmer 1 = on , 0 = off */

#define M_DS_ROLLOVER       (1<<DS_ROLLOVER)
#define M_DS_UP_DOWN        (1<<DS_UP_DOWN)
#define M_DS_CHANGING_DIM   (1<<DS_CHANGING_DIM)
#define M_DS_ON_OFF         (1<<DS_ON_OFF)


typedef struct s_nodeStatus_ {
    BYTE status;     /* Dimmer status  */
} t_nodeStatus;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

#endif /* _LEDDIM_H */
