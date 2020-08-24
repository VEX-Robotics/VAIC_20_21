/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) Innovation First 2017, All rights reserved.               */
/*                                                                            */
/*    Module:     v5_crc.c                                                    */
/*    Author:     James Pearman                                               */
/*    Created:    7 Feb 2017                                                  */
/*                                                                            */
/*    Revisions:  V0.1                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdint.h>

/*-----------------------------------------------------------------------------*/
/** @file    v5_crc.c
  * @brief   crc32 code
*//*---------------------------------------------------------------------------*/


// Polynomial definition
// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + x^0
// (as used by ethernet)
const uint32_t POLYNOMIAL_CRC32 = 0x04C11DB7;

// Static variables
static uint32_t _crc32_table[256];
static uint8_t  _table32Generated = 0;

/*---------------------------------------------------------------------------*/
/** @brief  Generates the CRC32 lookup table.                                */
/*---------------------------------------------------------------------------*/

static void
Crc32GenerateTable(void) {
  uint32_t i, j;
  uint32_t crc_accum;

  for(i = 0; i < 256; i++) {
    crc_accum = i << 24;

    for(j = 0; j < 8; j++) {
      if(crc_accum & 0x80000000L)
        crc_accum = (crc_accum << 1) ^ POLYNOMIAL_CRC32;
      else
        crc_accum = (crc_accum << 1);
      }
    _crc32_table[i] = crc_accum;
    }

  _table32Generated = 1;
}

/*----------------------------------------------------------------------------*/
/** @brief   Calculate CRC32                                                  */
/** @param  *pData        A pointer to a uint8_t array.                       */
/** @param  numberOfBytes Num of bytes on which the CRC should be calculated. */
/** @param  accumulator   zero, or the result of a previous CRC calculation.  */
/*----------------------------------------------------------------------------*/
/** @details
 *   Calculate CRC32.
 *   The accumulator should be 0 or the result of a previous CRC calculation
 */
 
uint32_t
Crc32Generate(uint8_t *pData, uint32_t numberOfBytes, uint32_t accumulator) {
  uint32_t i, j;

  // On first call generate the table
  if(!_table32Generated)
    Crc32GenerateTable();

  for(j = 0; j < numberOfBytes; j++) {
    i = ((accumulator >> 24) ^ *pData++) & 0xFF;
    accumulator = (accumulator << 8) ^ _crc32_table[i];
    }

  return accumulator;
}
