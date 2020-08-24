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
// This sample app uses the VEX message_link class to communicate data between two V5 robots.
// Each robot will need two V5 robot radios. One radio will be used to communicate with the Controller.
// The other radio will be used to link to the second robot. The radio used for the robot link
// will need to be connected on the port defined below. But the radio that is to be connected
// to the controller can be connected to any unused port.
//
// This same code can be used on both robots, but you will need to compile one version as the
// link "Manager" and the other as link "Worker". This is accomplished by uncommenting the link
// below that looks like this {#define MANAGER} to compile the code as link "Manager". And then
// Comment out that line of code to compile for worker.
//
// Both robots will pass simple commands to the other robot based on controller events.
// The app also displays a link data summary on the brain's screen.


// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Motor1               motor         1               
// Controller1          controller    N/A                
// Link                 radio         11                


// commant out for worker code
#define MANAGER

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;
vex::controller  Controller;
vex::motor       Motor1( vex::PORT1 );


#ifdef MANAGER
vex::message_link       Link( PORT11, "BotLink", linkType::manager );
#else
vex::message_link       Link( PORT11, "BotLink", linkType::worker  );
#endif


uint32_t  txTotal       = 0;
uint32_t  rxTotal       = 0;
uint32_t  txMessages    = 0;
uint32_t  rxMessages    = 0;
uint32_t  startTime = 0;

int displayStats() {
    while(1) {
      if( Link.isLinked())
        Brain.Screen.printAt( 90, 30, "Linked" );
      else
        Brain.Screen.printAt( 90, 30, "------" );

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
sendMessage( const char *message ) {
    int len = Link.send( message );
    if( len > 0 ) {
      txMessages++;
      txTotal += len;
    }
}
void
sendMessage( const char *message, int32_t index, double value ) {
    int len = Link.send( message, index, value );
    if( len > 0 ) {
      txMessages++;
      txTotal += len;
    }
}

void rxcallback( const char *message, const char *linkname, int32_t index, double value ) {
    rxTotal += strlen(message) + 1;
    rxMessages++;

    printf("%s %s %ld %f\n", message, linkname, index, value );

    if( !strcmp( message, "forward") )
        Motor1.spin( forward );
    else
    if( !strcmp( message, "stop") )
        Motor1.stop();
    else
    if( !strncmp( message, "axis2", 5) ) {
      Motor1.spin( forward, value, rpm );
    }
    else
    if( !strcmp( message, "test") ) {
#ifdef MANAGER
      printf("%ld\n", Brain.Timer.system()-startTime );
#else
      sendMessage("test", Brain.Timer.system(), 0.0 );
#endif
    }
}

void stopcallback( const char *message, const char *linkname, int32_t index, double value ) {
    printf("%s stop callback\n", message);
}
void playcallback( const char *message, const char *linkname, double value ) {
    printf("%s play callback\n", message);
}


void
sendForward() {
    sendMessage("forward");
    txMessages++;
    this_thread::sleep_for(25);
    Motor1.spin( forward );
}

void
sendStop() {
    sendMessage("stop");
    this_thread::sleep_for(25);
    Motor1.stop();
}

void
sendTest() {
    sendMessage("test");
    startTime = Brain.Timer.system();
}
void
sendAxis() {
    int value = Controller.Axis2.value();

    sendMessage( "axis2", Motor1.index(), value );
    Motor1.spin( forward, value, rpm );
}

int main() {   

#ifdef MANAGER
    Brain.Screen.printAt( 10, 30, "Manager" );
#else
    Brain.Screen.printAt( 10, 30, "Workder " );
#endif

    Link.received( rxcallback );


    Controller.ButtonA.pressed( sendForward );
    Controller.ButtonB.pressed( sendStop );
    Controller.Axis2.changed( sendAxis );
    Controller.ButtonX.pressed( sendTest );

    // display status
    task t1( displayStats );
        
    while(1) {
        // Allow other tasks to run
        this_thread::sleep_for(500);
    }
}
