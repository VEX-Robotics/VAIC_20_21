/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) Innovation First 2017, All rights reserved.               */
/*                                                                            */
/*    Module:     v5_crc.h                                                    */
/*    Author:     James Pearman                                               */
/*    Created:    7 Feb 2017                                                  */
/*                                                                            */
/*    Revisions:  V0.1                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef V5_CRC32_H_
#define V5_CRC32_H_

/*-----------------------------------------------------------------------------*/
/** @file    v5_crc.h
  * @brief   Header for crc code
*//*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

uint32_t  Crc32Generate(uint8_t *pData, uint32_t numberOfBytes, uint32_t accumulator);

#ifdef __cplusplus
}
#endif
#endif /* V5_CRC32_H_ */
