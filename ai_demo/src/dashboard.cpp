/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) Innovation First 2020 All rights reserved.                */
/*    Licensed under the MIT license.                                         */
/*                                                                            */
/*    Module:     dashboard.cpp                                               */
/*    Author:     James Pearman                                               */
/*    Created:    20 August 2020                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

//
// Display various useful information about the Jetson
//
static void
dashboardJetson( int ox, int oy, int width, int height ) {
  static int32_t     last_data = 0;
  static int32_t     last_packets = 0;
  static int32_t     total_data = 0;
  static int32_t     total_packets = 0;
  static uint32_t    update_time = 0;
  static MAP_RECORD  local_map;
  color grey = vex::color(0x404040);

  Brain.Screen.setClipRegion( ox, oy, width, height);
  Brain.Screen.setFont( mono15 );
  // border and titlebar
  Brain.Screen.setPenColor( yellow );
  Brain.Screen.drawRectangle(ox, oy, width, height, black );
  Brain.Screen.drawRectangle( ox, oy, width, 20, grey );

  Brain.Screen.setPenColor( yellow );
  Brain.Screen.setFillColor( grey );
  Brain.Screen.printAt(ox + 10, oy + 15, "Jetson" );
  oy += 20;
  
  Brain.Screen.setPenColor( white );
  Brain.Screen.setFillColor( black );
  
  // get last map data
  jetson_comms.get_data( &local_map );

  Brain.Screen.printAt( ox + 10, oy += 15, "Packets   %d", jetson_comms.get_packets() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Errors    %d", jetson_comms.get_errors() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Timeouts  %d", jetson_comms.get_timeouts() );
  Brain.Screen.printAt( ox + 10, oy += 15, "data/sec  %d             ", total_data );
  Brain.Screen.printAt( ox + 10, oy += 15, "pkts/sec  %d             ", total_packets );
  Brain.Screen.printAt( ox + 10, oy += 15, "boxnum    %d", local_map.boxnum );
  Brain.Screen.printAt( ox + 10, oy += 15, "mapnum    %d", local_map.mapnum );

  // once per second, update data rate stats
  if( Brain.Timer.system() > update_time ) {
    update_time = Brain.Timer.system() + 1000;
    total_data = jetson_comms.get_total() - last_data;
    total_packets = jetson_comms.get_packets() - last_packets;
    last_data = jetson_comms.get_total();
    last_packets = jetson_comms.get_packets();
  }
  
  Brain.Screen.setFont( mono12 );
  for(int i=0;i<4;i++ ) {
    if( i < local_map.boxnum ) {
      Brain.Screen.printAt( ox + 10, oy += 12, "box %d: c:%d x:%d y:%d w:%d h:%d prob:%.1f",i,
                           (local_map.boxobj[i].classID), //Class ID (0 = Red 1 = Blue 2 = Goal)
                           (local_map.boxobj[i].x), //in pixels
                           (local_map.boxobj[i].y), //in pixels
                           (local_map.boxobj[i].width), //in pixels
                           (local_map.boxobj[i].height), //in pixels
                           (local_map.boxobj[i].prob)); //percent likely to be in this catagory
    }
    else {
      Brain.Screen.printAt( ox + 10, oy += 12, "---");
    }
  }
  for(int i=0;i<4;i++ ) {
    if( i < local_map.mapnum ) {
      Brain.Screen.printAt( ox + 10, oy += 12, "map %d: a:%4d c:%4d X:%.2f Y:%.2f Z:%.1f",i,
                           local_map.mapobj[i].age,
                           local_map.mapobj[i].classID,
                           (local_map.mapobj[i].positionX / -25.4),  // mm -> inches
                           (local_map.mapobj[i].positionY / -25.4),  // mm -> inches
                           (local_map.mapobj[i].positionZ / 25.4)); // mm -> inches
    }
    else {
      Brain.Screen.printAt( ox + 10, oy += 12, "---");
    }
  }

}

//
// Display various useful information about VEXlink
//
static void
dashboardVexlink( int ox, int oy, int width, int height ) {
  static int32_t last_data = 0;
  static int32_t last_packets = 0;
  static int32_t total_data = 0;
  static int32_t total_packets = 0;
  static uint32_t update_time = 0;  

  color darkred = vex::color(0x800000);
  color darkgrn = vex::color(0x008000);

  Brain.Screen.setClipRegion( ox, oy, width, height);
  Brain.Screen.setFont( mono15 );

  // border and titlebar
  Brain.Screen.setPenColor( yellow );
  Brain.Screen.drawRectangle(ox, oy, width, height, black );
  Brain.Screen.drawRectangle( ox, oy, width, 20 );

  // Link status in titlebar
  if( link.isLinked() ) {
    Brain.Screen.setPenColor(darkgrn);
    Brain.Screen.setFillColor(darkgrn);
    Brain.Screen.drawRectangle( ox+1, oy+1, width-2, 18 );
    Brain.Screen.setPenColor(yellow);
    Brain.Screen.printAt( ox + 10, oy + 15, "VEXlink: Good" );
  }
  else {
    Brain.Screen.setPenColor(darkred);
    Brain.Screen.setFillColor(darkred);
    Brain.Screen.drawRectangle( ox+1, oy+1, width-2, 18 );
    Brain.Screen.setPenColor(yellow);
    Brain.Screen.printAt(ox + 10, oy + 15, "VEXlink: Disconnected" );
  }
  oy += 20;

  Brain.Screen.setFillColor(black);
  Brain.Screen.setPenColor(white);
  Brain.Screen.printAt( ox + 10, oy += 15, "Packets   %d", link.get_packets() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Errors    %d", link.get_errors() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Timeouts  %d", link.get_timeouts() );
  Brain.Screen.printAt( ox + 10, oy += 15, "data/sec  %d  ", total_data);
  Brain.Screen.printAt( ox + 10, oy += 15, "pkts/sec  %d  ", total_packets);

  // once per second, update data rate stats
  if( Brain.Timer.system() > update_time ) {
    update_time = Brain.Timer.system() + 1000;    
    total_data = link.get_total() - last_data;
    total_packets = link.get_packets() - last_packets;
    last_data = link.get_total();
    last_packets = link.get_packets();
  }

  oy += 10;
  Brain.Screen.printAt( ox + 10, oy += 15, "Location: local");
  
  float x,y,heading;
  link.get_local_location(x, y, heading);
  
Brain.Screen.printAt( ox + 10, oy += 15, " X:   %.2f", x / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " Y:   %.2f", y / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " H:   %.2f", 180 - (heading / (-2 * M_PI ) * 360) ); // rads to deg

  oy += 5;
  Brain.Screen.printAt( ox + 10, oy += 15, "Location: remote");
  
  link.get_remote_location(x, y, heading);
  
Brain.Screen.printAt( ox + 10, oy += 15, " X:   %.2f", x / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " Y:   %.2f", y / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " H:   %.2f", 180 - (heading / (-2 * M_PI ) * 360) ); // rads to deg
}

//
// Task to update screen with status
//
int
dashboardTask() {
  while(true) {
    // status
    dashboardJetson(    0, 0, 280, 240 );
    dashboardVexlink( 279, 0, 201, 240 );
    // draw, at 30Hz
    Brain.Screen.render();
    this_thread::sleep_for(16);
  }
  return 0;
}