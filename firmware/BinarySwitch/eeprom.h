/****************************************************************************
 *
 * Copyright (c) 2001-2012
 * Sigma Designs, Inc.
 * All Rights Reserved
 *
 *---------------------------------------------------------------------------
 *
 * Description: Internal EEPROM address definitions
 *
 * Author:   Peter Shorty
 *
 * Last Changed By:  $Author: efh $
 * Revision:         $Revision: 23890 $
 * Last Changed:     $Date: 2012-12-18 16:07:07 +0100 (ti, 18 dec 2012) $
 *
 ****************************************************************************/
#ifndef _EEPROM_H_
#define _EEPROM_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/* EEPROM address definitions */

/* EEPROM LED dimmer node layout */
#define EEOFFSET_STATUS             0x00
#define EEOFFSET_IGNORE_ALL_ON_OFF  EEOFFSET_STATUS + 1
#define EEOFFSET_PROTECTED          EEOFFSET_IGNORE_ALL_ON_OFF + 1
#define EEOFFSET_PROTECTED_RF       EEOFFSET_PROTECTED + 1

#define EEOFFSET_METER_UNSOLICITED_HIBYTE EEOFFSET_PROTECTED_RF + 1 //EEOFFSET_TRANSPORT_SETTINGS_START + EEOFFSET_TRANSPORT_SETTINGS_SIZE +1
#define EEOFFSET_METER_UNSOLICITED_LOBYTE EEOFFSET_METER_UNSOLICITED_HIBYTE + 1 //EEOFFSET_METER_UNSOLICITED_HIBYTE + 1

#define EEOFFSET_MAGIC              EEOFFSET_METER_UNSOLICITED_LOBYTE + 1  /* MAGIC */

#define EEOFFSET_ASSOCIATION_START        EEOFFSET_MAGIC + 1
#define EEOFFSET_ASSOCIATION_MAGIC        EEOFFSET_ASSOCIATION_START + ASSOCIATION_SIZE

#define EEOFFSET_TRANSPORT_SETTINGS_START EEOFFSET_ASSOCIATION_MAGIC + 1
#define EEOFFSET_TRANSPORT_SETTINGS_SIZE  TRANSPORT_EEPROM_SETTINGS_SIZE


//#define EEOFFSET_TRANSPORT_SETTINGS_START EEOFFSET_MAGIC + 1
//#define EEOFFSET_TRANSPORT_SETTINGS_SIZE  TRANSPORT_EEPROM_SETTINGS_SIZE

#define EEOFFSET_APPLICATION_DATA_END  EEOFFSET_TRANSPORT_SETTINGS_START + TRANSPORT_EEPROM_SETTINGS_SIZE

/* Default values */
#define DEFAULT_STATUS              SWITCHED_ON
#define DEFAULT_IGNORE_ALL_ON_OFF   SWITCH_ALL_EXCLUDE_ON_OFF
#define DEFAULT_PROTECTED           0
#define MAGIC_VALUE                 0x42


#endif /* _EEPROM_H_ */
