/****************************************************************************
 *
 * Copyright (c) 2001-2012
 * Sigma Designs, Inc.
 * All Rights Reserved
 *
 *---------------------------------------------------------------------------
 *
 * Description: Slave application for evaluation kit LED dimmer
 *
 * Author:   Peter Shorty
 *
 * Last Changed By:  $Author: jfr $
 * Revision:         $Revision: 25290 $
 * Last Changed:     $Date: 2013-04-05 17:19:50 +0200 (fr, 05 apr 2013) $
 *
 ****************************************************************************/
/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "config_app.h"
#include <ZW_slave_api.h>
#include <ZW_pindefs.h>
#include <ZW_evaldefs.h>
#include <ZW_debug_api.h>
#include <BinarySwitch.h>
#include <eeprom.h>
#include <slave_learn.h>
#include <ZW_uart_api.h>
#include <ZW_mem_api.h>

#include <ZW_TransportLayer.h>
#include "MyButtons.h"
//#include "Stubs.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/* SetLightLevel level defines */


/* Poll function button state defines */
#define POLLIDLE          0

#define POLLLEARNMODE     2 /* We're in learnmode */


/* Debounce count on button press detection. How many times must we see button */

#define DEBOUNCECOUNT       100


/* Timer repeat define */
#define FOREVER             TIMER_FOREVER
#define ONE_SHOT            TIMER_ONE_TIME
#define SET_PRODUCTIONTEST_PIN  PIN_IN(SSN, 0)
#define IN_PRODUCTIONTEST       (!PIN_GET(SSN))



#define SECONDS_PER_MIN 60
#define TICK_PER_SECOND TIMER_ONE_SECOND
#define TICK_PER_MINUTE (TICK_PER_SECOND * SECONDS_PER_MIN)
#define FACTORY_DEFAULT_DIMMING_DURATION 100  //Factory default diming duration value (in timer ticks) 100 -> 1 Sec
#define TIMER_DURATION_ENCODED_FACTORY_DEFAULT_DIMMING_DURATION 0xFF
#define TIMER_DURATION_ENCODED_MINUTES_DIMMING_DURATION_LIMIT 0x7F

#define DEFAULT_PRIMARY_SWITCH_TYPE     0x00
#define DEFAULT_SECONDARY_SWITCH_TYPE   0x00
#define FACTORY_DEFAULT_STEP_SIZE       0x64

/* Values used for Protection Report command */
#define PROTECTION_REPORT_UNPROTECTED_V2                                             0x00
#define PROTECTION_REPORT_PROTECTION_BY_SEQUENCE_V2                                  0x01
#define PROTECTION_REPORT_NO_OPERATION_POSSIBLE_V2                                   0x02

#define RF_PROTECTION_REPORT_UNPROTECTED_V2                                          0x00
#define RF_PROTECTION_REPORT_NO_RF_CONTROL_V2                                        0x01
#define RF_PROTECTION_REPORT_NO_RF_RESPONCE_V2                                       0x02

#define RF_PROTETION_MODE_INFINITELY                                                 0xFFFF

#define TIMER_DURATION_ENCODED_FACTORY_DEFAULT_DIMMING_DURATION 0xFF
#define TIMER_DURATION_ENCODED_MINUTES_DIMMING_DURATION_LIMIT 0x7F

#define TIMER_DURATION_ENCODED_SECONDS_PRORECTION_TO_HI_LIMIT 0x3C
#define TIMER_DURATION_ENCODED_MINUTES_PRORECTION_TO_LO_LIMIT 0x41
#define TIMER_DURATION_ENCODED_MINUTES_PRORECTION_TO_HI_LIMIT 0xFE
#define TIMER_DURATION_ENCODED_PROTECTION_INFINITELY 0xFF



#define PROTECTION_CHILD_NUM 6
#define PROTECTION_CHILD_TIMEOUT 0xFF

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/



/* node status */
/* A list of the known command classes. Except the basic class which allways */
/* should be supported. Used when node info is send */
static code BYTE nodeInfo[] =
{
  COMMAND_CLASS_SWITCH_BINARY,
  COMMAND_CLASS_SWITCH_ALL,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_PROTECTION
};

#define nodeInfoForTransport nodeInfo
BYTE TimerProtectionTimeoutHandle = 0;
BYTE pollState = POLLIDLE;
ZW_APPLICATION_TX_BUFFER txBuf;

BYTE ignoreAllOnOff = SWITCH_ALL_SET_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY; /* Default we do allow ALL ON/OFF */
static BYTE protected_On_Off = PROTECTION_REPORT_UNPROTECTED_V2; /* 0 means not protected and 1 child protected */
static BYTE rf_Protected_On_Off = RF_PROTECTION_REPORT_UNPROTECTED_V2; /* 0 means not protected and 1 child protected */
BYTE txOption;
BYTE testNodeID = ZW_TEST_NOT_A_NODEID;
WORD testFrameCount;
BYTE bMyNodeID;

BYTE myNodeID = 0;
BYTE myHomeID[4];
static BYTE protExclusiveNodeID = 0;
static WORD protTimeout = 0; //SDS11060-11.pdf Ch 4.67.6.1 "Factory default setting for the Timeout parameter must be 0x00" //RF_PROTETION_MODE_INFINITELY;


BYTE currentGroupIndex;
BYTE currentGroupNodeIndex;
/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
/*==============================   cbVoidByte   ============================
**
**  Function:  stub for callback
**
**  Side effects: None
**
**--------------------------------------------------------------------------*/
static void cbVoidByte(BYTE b)
{
}

#define DECODE_TIMER_DURATION(c_time)\
  (c_time == TIMER_DURATION_ENCODED_FACTORY_DEFAULT_DIMMING_DURATION)?\
    FACTORY_DEFAULT_DIMMING_DURATION:\
      (c_time <= TIMER_DURATION_ENCODED_MINUTES_DIMMING_DURATION_LIMIT)?\
       (long)c_time * TICK_PER_SECOND:\
       ((long)c_time - TIMER_DURATION_ENCODED_MINUTES_DIMMING_DURATION_LIMIT) * TICK_PER_MINUTE
static void                   /*RET  Nothing                  */
SwitchON( void )              /* IN  Nothing                  */
{
	ZW_MEM_PUT_BYTE( EEOFFSET_STATUS, SWITCHED_ON );
  PIN_ON(ZEROXpin);
}
static void                   /*RET  Nothing                  */
SwitchOFF( void )             /* IN  Nothing                  */
{
	ZW_MEM_PUT_BYTE( EEOFFSET_STATUS, SWITCHED_OFF );
	PIN_OFF(ZEROXpin);
}


static void                 /*RET  Nothing                  */
TimerProtectionTimeout( void ) /* IN  Nothing                  */
{
  if ((protTimeout == RF_PROTETION_MODE_INFINITELY) || (protTimeout == 0))
  {
    ZW_TIMER_CANCEL(TimerProtectionTimeoutHandle);
  }
  else
  {
    protTimeout--;
  }
}

/*=============================  ApplicationPoll   =========================
**    Application poll function for LED dimmer
**
**    Side effects:
**
**--------------------------------------------------------------------------*/
void                    /*RET  Nothing                  */
ApplicationPoll( void ) /* IN  Nothing                  */
{
  BYTE lastAction;
  lastAction = OneButtonLastAction();
	
  ZW_DEBUG_CMD_POLL();
/*
  if ( bNWIStartup )
  {
    StartLearnModeNow( ZW_SET_LEARN_MODE_NWI );
    bNWIStartup = FALSE;
    pollState = POLLLEARNMODE;
  }
*/
	if (buttonGetFlag())
	{
		if (buttonGetValue())
		{
				SwitchOFF();
		}
		else
		{
				SwitchON();
		}
		buttonClearFlag();
	}
  switch ( pollState )
  {
    case POLLIDLE:
      if ( lastAction == BUTTON_WAS_PRESSED )
      {
        StartLearnModeNow( ZW_SET_LEARN_MODE_CLASSIC );
        pollState = POLLLEARNMODE;
      }
			
      break;
    case POLLLEARNMODE:
      /* Just wait... */
      if ( lastAction == BUTTON_WAS_PRESSED )
      {
        /* Stop learn mode */
        StartLearnModeNow( FALSE );
        pollState = POLLIDLE;
      }
      break;
  }
}


/*==============================   SendReport   ==============================
**
**  Function:  SendReport
**
**    Sends a report frame to source node.
**
**  Side effects: None
**
**--------------------------------------------------------------------------*/
void              /*RET Nothing */
SendReport(
  BYTE sourceNode,  /* IN Source node - to whom report is to be send */
  BYTE classcmd,    /* IN Command class, which the report belongs to */
  BYTE reportparam ) /* IN paramter to include in report frame */
{
  txBuf.ZW_BasicReportFrame.cmdClass = classcmd;
  /* Report command definition - BASIC/ALL/PROTECTION/MULTI report use the same value ;-) */
  txBuf.ZW_BasicReportFrame.cmd      = BASIC_REPORT;
  txBuf.ZW_BasicReportFrame.value    = reportparam;
  /* Size of Report (BASIC/ALL/PROTECTION/MULTI GET response) frame are equal for BASIC/ALL/PROTECTION/MULTI */
  Transport_SendReport(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_BasicReportFrame), txOption, NULL, FALSE);
}
BYTE fw_updateSourceNode;
BYTE firmwareCrc1;
BYTE firmwareCrc2;
BYTE fw_numOfRetries=0;
WORD fw_actualFrameSize;
WORD firstAddr;
WORD fw_crc;

/*========================   ApplicationCommandHandler   ====================
**    Handling of a received application commands and requests
**
**    This is an application function example
**
**--------------------------------------------------------------------------*/

void                              /*RET Nothing                  */
Transport_ApplicationCommandHandler(
  BYTE  rxStatus,                 /* IN Frame header info */
  BYTE  sourceNode,               /* IN Command sender Node ID */
  ZW_APPLICATION_TX_BUFFER *pCmd, /* IN Payload from the received frame, the union */
  /*    should be used to access the fields */
  BYTE   cmdLength )              /* IN Number of command bytes including the command */
{
  ZW_APPLICATION_TX_BUFFER CmdPacket;
  __ZW_memcpy( cmdLength, &CmdPacket, pCmd );
  txOption = (( rxStatus & RECEIVE_STATUS_LOW_POWER ) ?
              TRANSMIT_OPTION_LOW_POWER : 0) | TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE | TRANSMIT_OPTION_AUTO_ROUTE;

  if ( CmdPacket.ZW_Common.cmdClass == COMMAND_CLASS_PROTECTION )
  {
    pollState = POLLIDLE;
    switch ( CmdPacket.ZW_Common.cmd ){
      case PROTECTION_SET_V2:
        if ( cmdLength == sizeof( ZW_PROTECTION_SET_FRAME ) )
        {
          CmdPacket.ZW_ProtectionSetV2Frame.level2 = RF_PROTECTION_REPORT_UNPROTECTED_V2;
        }

        if ( CmdPacket.ZW_ProtectionSetV2Frame.level <= PROTECTION_REPORT_NO_OPERATION_POSSIBLE_V2 )
        {
          protected_On_Off = CmdPacket.ZW_ProtectionSetV2Frame.level;
          ZW_MEM_PUT_BYTE( EEOFFSET_PROTECTED, protected_On_Off );
        }

        if ( CmdPacket.ZW_ProtectionSetV2Frame.level2 <= RF_PROTECTION_REPORT_NO_RF_RESPONCE_V2 )
        {
          rf_Protected_On_Off = CmdPacket.ZW_ProtectionSetV2Frame.level2;
          ZW_MEM_PUT_BYTE( EEOFFSET_PROTECTED_RF, rf_Protected_On_Off );
        }
        break;

      case PROTECTION_GET_V2:
      {
        txBuf.ZW_ProtectionReportV2Frame.cmdClass = COMMAND_CLASS_PROTECTION_V2;
        txBuf.ZW_ProtectionReportV2Frame.cmd      = PROTECTION_REPORT_V2;
        txBuf.ZW_ProtectionReportV2Frame.level    = protected_On_Off;
        txBuf.ZW_ProtectionReportV2Frame.level2   = rf_Protected_On_Off;
        /* Size of Report (BASIC/ALL/PROTECTION/MULTI GET response) frame are equal for BASIC/ALL/PROTECTION/MULTI */
        ZW_SEND_DATA( sourceNode, ( BYTE * )&txBuf, sizeof( txBuf.ZW_ProtectionReportV2Frame ), txOption, cbVoidByte );
      }
      break;

      case PROTECTION_SUPPORTED_GET_V2:
      {
        union
        {
          WORD word;
          struct
          {
            BYTE b1;
            BYTE b2;
          } byte;
        } state;

        txBuf.ZW_ProtectionSupportedReportV2Frame.cmdClass = COMMAND_CLASS_PROTECTION_V2;
        txBuf.ZW_ProtectionSupportedReportV2Frame.cmd      = PROTECTION_SUPPORTED_REPORT_V2;

        txBuf.ZW_ProtectionSupportedReportV2Frame.level = PROTECTION_SUPPORTED_REPORT_LEVEL_TIMEOUT_BIT_MASK_V2 | PROTECTION_SUPPORTED_REPORT_LEVEL_EXCLUSIVE_CONTROL_BIT_MASK_V2;

        state.word = 0x1 << protected_On_Off;
        txBuf.ZW_ProtectionSupportedReportV2Frame.localProtectionState1 = state.byte.b1;
        txBuf.ZW_ProtectionSupportedReportV2Frame.localProtectionState2 = state.byte.b2;

        state.word = 0x1 << rf_Protected_On_Off;
        txBuf.ZW_ProtectionSupportedReportV2Frame.rfProtectionState1    = state.byte.b1;
        txBuf.ZW_ProtectionSupportedReportV2Frame.rfProtectionState2    = state.byte.b2;

        ZW_SEND_DATA( sourceNode, ( BYTE * )&txBuf, sizeof( txBuf.ZW_ProtectionSupportedReportV2Frame ), txOption, cbVoidByte );
      }
      break;

      case PROTECTION_EC_SET_V2:
        if ( CmdPacket.ZW_ProtectionEcSetV2Frame.nodeId <= ZW_MAX_NODES )
        {
          protExclusiveNodeID = CmdPacket.ZW_ProtectionEcSetV2Frame.nodeId;
        }
        break;

      case PROTECTION_EC_GET_V2:
      {
        txBuf.ZW_ProtectionEcReportV2Frame.cmdClass = COMMAND_CLASS_PROTECTION_V2;
        txBuf.ZW_ProtectionEcReportV2Frame.cmd      = PROTECTION_EC_REPORT_V2;
        txBuf.ZW_ProtectionEcReportV2Frame.nodeId   = protExclusiveNodeID;
        ZW_SEND_DATA( sourceNode, ( BYTE * )&txBuf, sizeof( txBuf.ZW_ProtectionEcReportV2Frame ), txOption, cbVoidByte );
      }
      break;

      case PROTECTION_TIMEOUT_SET_V2:
        if ( CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout == 0 )
        {
          //No timer is set. All “normal operation” Commands must be accepted.
          protTimeout = 0;
        }
        else if ( CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout <= TIMER_DURATION_ENCODED_SECONDS_PRORECTION_TO_HI_LIMIT )
        {
          //Timeout is set from 1 second (0x01) to 60 seconds (0x3C) in 1-second resolution
          protTimeout = ( WORD )CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout;
          ZW_TIMER_CANCEL(TimerProtectionTimeoutHandle);
          TimerProtectionTimeoutHandle = ZW_TIMER_START( TimerProtectionTimeout, TICK_PER_SECOND, TIMER_FOREVER );
        }
        else if (( CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout >= TIMER_DURATION_ENCODED_MINUTES_PRORECTION_TO_LO_LIMIT ) && ( CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout <= TIMER_DURATION_ENCODED_MINUTES_PRORECTION_TO_HI_LIMIT ) )
        {
          //Timeout is set from 2 minutes (0x41) to 191 minutes (0xFE) in 1-minute resolution
          protTimeout = (( WORD )CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout - TIMER_DURATION_ENCODED_MINUTES_PRORECTION_TO_LO_LIMIT + 2 ) * SECONDS_PER_MIN;
          ZW_TIMER_CANCEL(TimerProtectionTimeoutHandle);
          TimerProtectionTimeoutHandle = ZW_TIMER_START( TimerProtectionTimeout, TICK_PER_SECOND, TIMER_FOREVER );
        }
        else if ( CmdPacket.ZW_ProtectionTimeoutSetV2Frame.timeout == TIMER_DURATION_ENCODED_PROTECTION_INFINITELY )
        {
          //No Timeout – The Device will remain in RF Protection mode infinitely
          protTimeout = RF_PROTETION_MODE_INFINITELY;
        }
        else
        {
          //invalid cmd param
        }
        break;

      case PROTECTION_TIMEOUT_GET_V2:
      {
        txBuf.ZW_ProtectionTimeoutReportV2Frame.cmdClass = COMMAND_CLASS_PROTECTION_V2;
        txBuf.ZW_ProtectionTimeoutReportV2Frame.cmd      = PROTECTION_TIMEOUT_REPORT_V2;
        if (protTimeout == RF_PROTETION_MODE_INFINITELY)
        {
          txBuf.ZW_ProtectionTimeoutReportV2Frame.timeout  = TIMER_DURATION_ENCODED_PROTECTION_INFINITELY;
        }
        else
        {
          txBuf.ZW_ProtectionTimeoutReportV2Frame.timeout  = protTimeout > SECONDS_PER_MIN ? ( protTimeout + SECONDS_PER_MIN - 1) / SECONDS_PER_MIN : protTimeout;
        }

        ZW_SEND_DATA( sourceNode, ( BYTE * )&txBuf, sizeof( txBuf.ZW_ProtectionTimeoutReportV2Frame ), txOption, cbVoidByte );
      }
      break;

      // PROTECTION_TIMEOUT_GET_V2

      default:
        break;

    }
    return;
  }

  //    if ((protExclusiveNodeID != 0)&&(sourceNode != protExclusiveNodeID)&& (protTimeout != 0))
  if (( sourceNode != protExclusiveNodeID ) && ( protTimeout != 0 ) )
  {
    if ( rf_Protected_On_Off == RF_PROTECTION_REPORT_NO_RF_CONTROL_V2 )
    {
      //EC protection we should send Application rejected Request Command
      txBuf.ZW_ApplicationRejectedRequestFrame.cmdClass = COMMAND_CLASS_APPLICATION_STATUS;
      txBuf.ZW_ApplicationRejectedRequestFrame.cmd      = APPLICATION_REJECTED_REQUEST;
      txBuf.ZW_ApplicationRejectedRequestFrame.status   = 0;
      ZW_SEND_DATA( sourceNode, ( BYTE * )&txBuf, sizeof( txBuf.ZW_ApplicationRejectedRequestFrame ), txOption, cbVoidByte );
      return;
    }
    else if ( rf_Protected_On_Off == RF_PROTECTION_REPORT_NO_RF_RESPONCE_V2 )
    {
      return;
    }
  }

  if (CmdPacket.ZW_Common.cmdClass == COMMAND_CLASS_MANUFACTURER_SPECIFIC){
    switch (CmdPacket.ZW_Common.cmd)
    {
      case MANUFACTURER_SPECIFIC_GET:
        txBuf.ZW_ManufacturerSpecificReportFrame.cmdClass = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        txBuf.ZW_ManufacturerSpecificReportFrame.cmd = MANUFACTURER_SPECIFIC_REPORT;
        txBuf.ZW_ManufacturerSpecificReportFrame.manufacturerId1 = 0x00; // Zensys manufacturer ID
        txBuf.ZW_ManufacturerSpecificReportFrame.manufacturerId2 = 0x00;
        txBuf.ZW_ManufacturerSpecificReportFrame.productTypeId1 = 0x00; // Assigned by manufacturer
        txBuf.ZW_ManufacturerSpecificReportFrame.productTypeId2 = 0x00;
        txBuf.ZW_ManufacturerSpecificReportFrame.productId1 = 0x00; // Assigned by manufacturer
        txBuf.ZW_ManufacturerSpecificReportFrame.productId2 = 0x00;
        Transport_SendReport(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_ManufacturerSpecificReportFrame), txOption, NULL, FALSE);
        break;
    }
  }

  else if (CmdPacket.ZW_Common.cmdClass == COMMAND_CLASS_VERSION){
    switch (CmdPacket.ZW_Common.cmd)
    {
      case VERSION_GET:
        txBuf.ZW_VersionReportFrame.cmdClass = COMMAND_CLASS_VERSION;
        txBuf.ZW_VersionReportFrame.cmd = VERSION_REPORT;
        txBuf.ZW_VersionReportFrame.zWaveLibraryType = ZW_TYPE_LIBRARY();
        txBuf.ZW_VersionReportFrame.zWaveProtocolVersion = ZW_VERSION_MAJOR;
        txBuf.ZW_VersionReportFrame.zWaveProtocolSubVersion = ZW_VERSION_MINOR;
        txBuf.ZW_VersionReportFrame.applicationVersion = APP_VERSION;
        txBuf.ZW_VersionReportFrame.applicationSubVersion = APP_REVISION;
        Transport_SendReport(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_VersionReportFrame), txOption, NULL, FALSE);
        break;

      case VERSION_COMMAND_CLASS_GET:
        txBuf.ZW_VersionCommandClassReportFrame.cmdClass = COMMAND_CLASS_VERSION;
        txBuf.ZW_VersionCommandClassReportFrame.cmd = VERSION_COMMAND_CLASS_REPORT;
        txBuf.ZW_VersionCommandClassReportFrame.requestedCommandClass = CmdPacket.ZW_VersionCommandClassGetFrame.requestedCommandClass;
        switch (CmdPacket.ZW_VersionCommandClassGetFrame.requestedCommandClass)
        {
          case COMMAND_CLASS_BASIC:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = BASIC_VERSION;
            break;

          case COMMAND_CLASS_SWITCH_BINARY:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = SWITCH_BINARY_VERSION;
            break;
            
          case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = FIRMWARE_UPDATE_MD_VERSION_V2;
            break;

          case COMMAND_CLASS_SWITCH_ALL:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = SWITCH_ALL_VERSION;
            break;

          case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = MANUFACTURER_SPECIFIC_VERSION;
            break;

          case COMMAND_CLASS_VERSION:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = VERSION_VERSION;
            break;

          case COMMAND_CLASS_PROTECTION:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = PROTECTION_VERSION_V2;
            break;

          case COMMAND_CLASS_POWERLEVEL:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = POWERLEVEL_VERSION;
            break;

          default:
            txBuf.ZW_VersionCommandClassReportFrame.commandClassVersion = UNKNOWN_VERSION;
            break;
        }
        Transport_SendReport(sourceNode, (BYTE *)&txBuf, sizeof(txBuf.ZW_VersionCommandClassReportFrame), txOption, NULL, FALSE);
        break;
    }
  }


  else if ( CmdPacket.ZW_Common.cmdClass == COMMAND_CLASS_BASIC ){
      switch ( CmdPacket.ZW_Common.cmd ){
        case BASIC_GET:
          SendReport( sourceNode, CmdPacket.ZW_Common.cmdClass,ZW_MEM_GET_BYTE(EEOFFSET_STATUS)  );
          break;
        case BASIC_SET:
          if ( CmdPacket.ZW_BasicSetFrame.value == SWITCHED_OFF )
          {
            SwitchOFF();
          }
          else SwitchON();
          break;
      } /* switch */
	}
	else if ( CmdPacket.ZW_Common.cmdClass == COMMAND_CLASS_SWITCH_BINARY ){
      /* Single switch command */
      switch ( CmdPacket.ZW_Common.cmd )
      {
        case SWITCH_BINARY_GET:
					SendReport( sourceNode, CmdPacket.ZW_Common.cmdClass,ZW_MEM_GET_BYTE(EEOFFSET_STATUS)  );
          break;
          /* Jump to the specified level param1 */
        case SWITCH_BINARY_SET:
				
					if(CmdPacket.ZW_SwitchBinarySetFrame.switchValue  == SWITCHED_OFF){
						SwitchOFF();
					}
					else {						
						SwitchON();
					}

          break;
      } /* switch */
    } /* if */
	else if ( CmdPacket.ZW_Common.cmdClass == COMMAND_CLASS_SWITCH_ALL ){
      switch ( CmdPacket.ZW_Common.cmd )
      {
        case SWITCH_ALL_ON:
          if (( ignoreAllOnOff != SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_ALL_OFF_FUNCTIONALITY ) && ( ignoreAllOnOff != SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_FUNCTIONALITY_BUT_NOT_ALL_OFF ) )
          {
            SwitchON();
          }
          break;

        case SWITCH_ALL_OFF:
          if (( ignoreAllOnOff != SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_ALL_OFF_FUNCTIONALITY ) && ( ignoreAllOnOff != SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_OFF_FUNCTIONALITY_BUT_NOT_ALL_ON ) )
          {
           SwitchOFF();
          }
          break;

        case SWITCH_ALL_SET:
          ignoreAllOnOff = CmdPacket.ZW_SwitchAllSetFrame.mode;
          /* Store it in the EEPROM */
          ZW_MEM_PUT_BYTE( EEOFFSET_IGNORE_ALL_ON_OFF, ignoreAllOnOff );
          break;

        case SWITCH_ALL_GET:
          SendReport( sourceNode, COMMAND_CLASS_SWITCH_ALL, ignoreAllOnOff );
          break;

        default:
          break;

      } /* switch */
    } /* else if == COMMAND_CLASS_SWITCH_ALL */
}


void FirmwareUpdateStatusCalback(BYTE txStatus)
{
  ZW_WATCHDOG_ENABLE; 
  while(1);
}

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/*============================   ApplicationInitHW   ========================
**    Non Z-Wave hardware initialization
**
**    This is an application function example
**
**--------------------------------------------------------------------------*/
BYTE                       /*RET  TRUE        */
ApplicationInitHW( BYTE bStatus )  /* IN  Nothing     */
{
#ifdef __C51__
  IBYTE i;
#endif
#ifdef ZW_ISD51_DEBUG
  ZW_UART_INIT(1152);
#endif
#ifdef __C51__

  SET_PRODUCTIONTEST_PIN;
  for ( i = 0; i < 10; i++ ) ;  /* Short delay... */
  if ( IN_PRODUCTIONTEST ) /* Anyone forcing it into productiontest mode ? */
  {
    return( FALSE );  /*Enter production test mode*/
  }
#endif

  /* hardware initialization */
  PIN_IN( Button, 1 );
	ZW_SetExtIntLevel(ZW_INT1, FALSE);

  /* Setup specifics for current dim module */
  PIN_OUT( ZEROXpin );
	SwitchOFF();
  Transport_OnApplicationInitHW(bStatus);

  return( TRUE );
}


/*===========================   ApplicationInitSW   =========================
**    Initialization of the Application Software
**
**    This is an application function example
**
**--------------------------------------------------------------------------*/
BYTE                      /*RET  TRUE       */
ApplicationInitSW( void ) /* IN   Nothing   */
{
  /* Do not reinitialize the UART if already initialized for ISD51 in ApplicationInitHW() */
#ifndef ZW_ISD51_DEBUG
  ZW_DEBUG_INIT(1152);
#endif
  ZW_DEBUG_SEND_NL();

#ifdef AUTO_INCLUSION
  learnModeActiveHandle = 0xFF;
#endif

  /* get stored values */
  if ( ZW_MEM_GET_BYTE( EEOFFSET_MAGIC ) == MAGIC_VALUE )
  {
    /* Get Ignore all On/Off frames state */
    ignoreAllOnOff = ZW_MEM_GET_BYTE( EEOFFSET_IGNORE_ALL_ON_OFF );
    /* Get protection state */
    protected_On_Off = ZW_MEM_GET_BYTE(EEOFFSET_PROTECTED);
    rf_Protected_On_Off = ZW_MEM_GET_BYTE(EEOFFSET_PROTECTED_RF);
  }
  else
  { /* Its alive */
    ZW_MEM_PUT_BYTE( EEOFFSET_STATUS, SWITCHED_OFF );  /* Switch OFF */
    ZW_MEM_PUT_BYTE(EEOFFSET_IGNORE_ALL_ON_OFF, SWITCH_ALL_SET_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY);
    ZW_MEM_PUT_BYTE(EEOFFSET_PROTECTED, PROTECTION_REPORT_UNPROTECTED_V2); /* Not protected */
    ZW_MEM_PUT_BYTE(EEOFFSET_PROTECTED_RF, RF_PROTECTION_REPORT_UNPROTECTED_V2); /* Not protected */
    ZW_MEM_PUT_BYTE( EEOFFSET_MAGIC, MAGIC_VALUE ); /* Now EEPROM should be OK */
    /* ignoreAllOnOff and protected_On_Off are initialized on startup */
  }
  // ZW_DEBUG_CMD_INIT(1152);
  OneButtonInit();
  /* Check if we have a node id */
  MemoryGetID( NULL, &bMyNodeID );

  /* Set Nwtwork wide inclusion active if we dont have aa node ID */
  if ( bMyNodeID ){
		// Chua Co ID
		if(ZW_MEM_GET_BYTE(EEOFFSET_STATUS)) SwitchON();
		else SwitchOFF();
		
		}
  else {
		// Da duoc add vao network
		SwitchOFF();
	}

  Transport_OnApplicationInitSW(nodeInfoForTransport, sizeof(nodeInfoForTransport),
                                FALSE, EEOFFSET_TRANSPORT_SETTINGS_START, EEOFFSET_TRANSPORT_SETTINGS_SIZE, NULL);

  
  return( TRUE );
}


/*============================   ApplicationTestPoll   ======================
**    Function description
**      This function is called when the dimmer enters test mode.
**    Side effects:
**       Code will not exit until it is reset
**--------------------------------------------------------------------------*/
void ApplicationTestPoll( void )
{
  /*Send constant out*/
  ZW_SEND_CONST();
  while ( 1 );
}


/*======================   ApplicationNodeInformation   =====================
**    Request Node information and current status
**    Called by the the Z-Wave application layer before transmitting a
**    "Node Information" frame.
**
**    This is an application function example
**
**--------------------------------------------------------------------------*/
extern void                  /*RET  Nothing */
ApplicationNodeInformation(
  BYTE   *deviceOptionsMask,      /*OUT Bitmask with application options     */
  APPL_NODE_TYPE  *nodeType,  /*OUT  Device type Generic and Specific   */
  BYTE       **nodeParm,      /*OUT  Device parameter buffer pointer    */
  BYTE       *parmLength )    /*OUT  Number of Device parameter bytes   */
{
  /* this is a listening node and it supports optional CommandClasses */
  *deviceOptionsMask = APPLICATION_NODEINFO_LISTENING | APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY;
  nodeType->generic = GENERIC_TYPE_SWITCH_BINARY; /* Generic device type */
  nodeType->specific = SPECIFIC_TYPE_POWER_SWITCH_BINARY; /* Specific class */
  *nodeParm = nodeInfo;                 /* Send list of known command classes. */
  *parmLength = sizeof( nodeInfo );     /* Set length*/
}


/*=========================   ApplicationSlaveUpdate   =======================
**   Inform a slave application that a node information is received.
**   Called from the slave command handler when a node information frame
**   is received and the Z-Wave protocol is not in a state where it is needed.
**
**--------------------------------------------------------------------------*/
void
ApplicationSlaveUpdate(
  BYTE bStatus,     /*IN  Status event */
  BYTE bNodeID,     /*IN  Node id of the node that send node info */
  BYTE* pCmd,       /*IN  Pointer to Application Node information */
  BYTE bLen )      /*IN  Node info length                        */
{
}





/*============================   LearnCompleted   ========================
**    Function description
**      Called from learnmode.c when learnmode completed
**
**    Side effects:
**
**--------------------------------------------------------------------------*/
void                                    /*RET   Nothing */
LearnCompleted(
  BYTE nodeID )                   /* IN resulting nodeID */
{
  if ( nodeID == 0 )    /* Was it reset? */
  {
    ignoreAllOnOff = SWITCH_ALL_SET_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY;
    protected_On_Off = PROTECTION_REPORT_UNPROTECTED_V2;
    rf_Protected_On_Off = RF_PROTECTION_REPORT_UNPROTECTED_V2;
    /* Store it in the EEPROM */
    ZW_MEM_PUT_BYTE( EEOFFSET_IGNORE_ALL_ON_OFF, ignoreAllOnOff );
    ZW_MEM_PUT_BYTE(EEOFFSET_PROTECTED, protected_On_Off);
    ZW_MEM_PUT_BYTE(EEOFFSET_PROTECTED_RF, rf_Protected_On_Off);
  }
  Transport_OnLearnCompleted(nodeID);
}
