/*
 * Copyright (c) 2016-2020, VEX Robotics, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of VEX Robotics Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Mon Nov 04 2019                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include <sys/fcntl.h>
#include <string.h>

#include <functional>
#include <iterator> 
#include <map>
#include <iostream>

using namespace vex;

// Program Summary:
// This sample app uses the VEX serial_link class to communicate data between two V5 robots.
// Each robot will need two V5 robot radios. One radio will be used to communicate with the Controller.
// The other radio will be used to link to the second robot. The radio used for the robot link
// will need to be connected on the port defined below. But the radio that is to be connected
// to the controller can be connected to any unused port.
//
// This same code can be used on both robots, but you will need to compile one version as the
// link "Manager" and the other as link "Worker". This is accomplished by uncommenting the link
// below that looks like this {#define Manager} to compile the code as link "Manager". And then
// comment out that line of code to compile for worker.
//
// Both robots will pass the state of its' controller to the other robot when buttons A,B,X or Y are pressed
// The message format is deffined by the structure: myPayload. We use the first 4 bytes to validate and sync the data.
// The app also displays a link data summary on the brain's screen.


// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Motor1               motor         1               
// Controller1          controller    N/A                
// Link                 radio         11   

// commant out for worker code
#define MANAGER    true

#define BUTTON_A      0x0001
#define BUTTON_B      0x0002
#define BUTTON_X      0x0004
#define BUTTON_Y      0x0008

#define BUTTON_UP     0x0010
#define BUTTON_DN     0x0020
#define BUTTON_LT     0x0040
#define BUTTON_RT     0x0080

#define BUTTON_L1     0x0100
#define BUTTON_L2     0x0200
#define BUTTON_R1     0x0400
#define BUTTON_R2     0x0800

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;
vex::controller  Controller;
vex::motor       Motor1( vex::PORT1 );


#ifdef MANAGER
vex::serial_link       Link( PORT11, "BotLink", linkType::manager );
#else
vex::serial_link       Link( PORT11, "BotLink", linkType::worker  );
#endif

#define SYNC_VALUE    0xAA55D021

struct myPayload
{
  int32_t SyncValue; // Set value to mark the begining of the data. Always = 0xAA55D021
  int32_t Axis1;
  int32_t Axis2;
  int32_t Axis3;
  int32_t Axis4;
  uint16_t buttons;
  uint16_t packetNum;         
  uint32_t timestamp;
};

#define FIFO_MAX_SIZE   2048

uint8_t fifoBuffer[FIFO_MAX_SIZE];
int32_t fifoIn = 0;
int32_t fifoOut = 0;

uint32_t  txTotal       = 0;
uint32_t  rxTotal       = 0;
uint32_t  txMessages    = 0;
uint32_t  rxMessages    = 0;
uint32_t  startTime = 0;

uint16_t myPacketCount = 0;
uint16_t lastPacketCount = 0;
bool linked = false;

int displayStats() {
    while(1) {
      if( Link.isLinked())
      {
        if(!linked)
          printf("Established Link!\n");

        Brain.Screen.printAt( 90, 30, "Linked" );
        linked = true;
      }
      else
      {
        if(linked)
          printf("Lost Link!\n");
        Brain.Screen.printAt( 90, 30, "------" );
        linked = false;
      }

      Brain.Screen.printAt( 250, 70, "Transmit stats");
      Brain.Screen.printAt( 250, 90, "Msg     :%d", txMessages );
      Brain.Screen.printAt( 250,110, "bytes   :%d", txTotal );

      Brain.Screen.printAt(  10, 70, "Receive stats" );
      Brain.Screen.printAt(  10, 90, "Msg     :%d", rxMessages  );
      Brain.Screen.printAt(  10,110, "bytes   :%d", rxTotal );

      this_thread::sleep_for(100);
    }
    return 0;
}

void
sendMessage( uint8_t *message, int32_t len ) {
    int sent = Link.send( message, len );
    if( sent > 0 ) {
      txMessages++;
      txTotal += sent;
    }
}

void handleControls(myPayload newData)
{
  
  rxTotal += sizeof(myPayload);
  rxMessages++;

  if(lastPacketCount+1 != newData.packetNum)
    printf("LOST PACKET: Expected: %d Found: %d\n", lastPacketCount+1, newData.packetNum);
  else {
    printf("%d\n", newData.packetNum);
  }

  lastPacketCount = newData.packetNum;

  //Drive control
  Motor1.setVelocity(newData.Axis1, percent);
  Motor1.spin(forward);
}

int32_t fifoCount()
{
  if(fifoIn == fifoOut) return 0;

  if(fifoIn < fifoOut)
    return FIFO_MAX_SIZE - fifoOut + fifoIn;

  return fifoIn - fifoOut;
}

void pushFifo(uint8_t data)
{
  fifoBuffer[fifoIn] = data;
  fifoIn = (fifoIn + 1) % FIFO_MAX_SIZE;
}

bool fifoGet(uint8_t* outByte)
{
  uint8_t retVal = false;
  if(fifoIn != fifoOut)
  {
    *outByte = fifoBuffer[fifoOut];
    fifoOut = (fifoOut + 1) % FIFO_MAX_SIZE;
    retVal = true;
  }

  return retVal;
}


uint8_t state = 0;
uint8_t inbuffer[sizeof(myPayload)];
uint8_t readIndex = 0;
myPayload inPayload;
void handleData()
{

  uint8_t dataByte;
  while(fifoGet(&dataByte))
  {
    printf("Process byte: %x State: %d\n", dataByte, state);
    switch(state)
    {
      case 0:
        if(dataByte == 0x21)
          state++;
        
        readIndex = 0;
        inbuffer[readIndex++] = dataByte;
        break;
      case 1:
        if(dataByte == 0xD0)
        {
          state++;
          inbuffer[readIndex++] = dataByte;
        }
        else 
          state = 0;
        break;
      case 2:
        if(dataByte == 0x55)
        {
          state++;
          inbuffer[readIndex++] = dataByte;
        }
        else 
          state = 0;
        break;
      case 3:
        if(dataByte == 0xAA)
        {
          state++;
          inbuffer[readIndex++] = dataByte;
        }
        else 
          state = 0;
        break;
      case 4:
        inbuffer[readIndex++] = dataByte;

        if(readIndex == sizeof(myPayload))
        {
          memcpy(&inPayload, inbuffer, sizeof(myPayload));
          handleControls(inPayload);
          state = 0;
        }
      break;
    }
  }

}

void rxcallback( uint8_t *message, int32_t length) {

    for(int i = 0; i < length; i++)
      pushFifo(message[i]);

    handleData();
}

uint16_t getButtons()
{
  uint16_t retVal = 0;

  retVal |= Controller.ButtonA.pressing() ? BUTTON_A : 0;
  retVal |= Controller.ButtonB.pressing() ? BUTTON_B : 0;
  retVal |= Controller.ButtonX.pressing() ? BUTTON_X : 0;
  retVal |= Controller.ButtonY.pressing() ? BUTTON_Y : 0;
  retVal |= Controller.ButtonUp.pressing() ? BUTTON_UP : 0;
  retVal |= Controller.ButtonDown.pressing() ? BUTTON_DN : 0;
  retVal |= Controller.ButtonLeft.pressing() ? BUTTON_LT : 0;
  retVal |= Controller.ButtonRight.pressing() ? BUTTON_RT : 0;

  retVal |= Controller.ButtonL1.pressing() ? BUTTON_L1 : 0;
  retVal |= Controller.ButtonL2.pressing() ? BUTTON_L2 : 0;

  retVal |= Controller.ButtonR1.pressing() ? BUTTON_R1 : 0;
  retVal |= Controller.ButtonR2.pressing() ? BUTTON_R2 : 0;

  return retVal;
}

void controllerChange()
{
  myPayload controllerState;

  memset(&controllerState, 0, sizeof(controllerState));

  controllerState.buttons = getButtons();
  controllerState.Axis1 = Controller.Axis1.position();
  controllerState.Axis2 = Controller.Axis2.position();
  controllerState.Axis3 = Controller.Axis3.position();
  controllerState.Axis4 = Controller.Axis4.position();

  controllerState.timestamp = Brain.Timer.system();
  controllerState.SyncValue = SYNC_VALUE; 
  controllerState.packetNum = myPacketCount++;
  sendMessage(((uint8_t*)&controllerState), sizeof(controllerState));

}

int main() {   
#ifdef MANAGER
    Brain.Screen.printAt( 10, 30, "Manager" );
#else
    Brain.Screen.printAt( 10, 30, "Worker " );
#endif

    Link.received( rxcallback );

    Controller.ButtonA.pressed( controllerChange );
    Controller.ButtonB.pressed( controllerChange );
    Controller.ButtonY.pressed( controllerChange );
    Controller.ButtonX.pressed( controllerChange );

    // display status
    task t1( displayStats );
        
    while(1) {
      // Allow other tasks to run
      this_thread::sleep_for(1);
    }
}
