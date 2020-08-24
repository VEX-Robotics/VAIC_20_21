/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Mon Aug 03 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

// ---- START VEXCODE CONFIGURED DEVICES ----
// ---- END VEXCODE CONFIGURED DEVICES ----

using namespace vex;

// define your global instances of motors and other devices here
jetson  jetson_comms;

MAP_RECORD       local_map;

/*----------------------------------------------------------------------------*/
//
// Some other activity while the comms is running
// Just for demo/test porposes
int
UserTestTask1() {
		int i;
		double x, y;
		
		while(1) {
  		Brain.Screen.setPenColor( white );
			for(i=360;i<1800;i++) {
				x = cos( (double)i * (3.14159 / 720) ) * 50;				
				y = sin( (double)i * (3.14159 / 720) ) * 50;				
	
				Brain.Screen.drawLine( 50, 170, 50-x, 170-y );
				this_thread::sleep_for(1);
			}
  		Brain.Screen.setPenColor( black );
			for(i=360;i<1800;i++) {
				x = cos( (double)i * (3.14159 / 720) ) * 50;				
				y = sin( (double)i * (3.14159 / 720) ) * 50;				
	
        Brain.Screen.drawLine( 50, 170, 50-x, 170-y );
				this_thread::sleep_for(1);
			}
		}
}

int main() {
    int32_t last_data = 0;
    int32_t last_packets = 0;
    int32_t loops = 0;
    int32_t loop_time = 20;

    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();
 
     thread t1(UserTestTask1);
 
     while(1) {
         // status
         Brain.Screen.printAt( 10, 20, "Packets   %d", jetson_comms.get_packets() );
         Brain.Screen.printAt( 10, 40, "Errors    %d", jetson_comms.get_errors() );
         Brain.Screen.printAt( 10, 60, "Timeouts  %d", jetson_comms.get_timeouts() );
 
         // once per second, update data rate
         if( ++loops == 1000/loop_time ) {
           loops = 0;
           Brain.Screen.printAt( 10, 80, "data/sec  %d             ", jetson_comms.get_total() - last_data);
           Brain.Screen.printAt( 10,100, "pkts/sec  %d             ", jetson_comms.get_packets() - last_packets);
           last_data = jetson_comms.get_total();
           last_packets = jetson_comms.get_packets();
         }
 
         // get last map data
         jetson_comms.get_data( &local_map );
 
         // debug a few values
         Brain.Screen.printAt( 180, 20, "boxnum  %d", local_map.boxnum );
         Brain.Screen.printAt( 180, 40, "mapnum  %d", local_map.mapnum );
         Brain.Screen.printAt( 180, 60, "box 0: x:%04d y:%04d", local_map.boxobj[0].x, local_map.boxobj[0].y );
         Brain.Screen.printAt( 180, 80, "       w:%04d h:%04d", local_map.boxobj[0].width, local_map.boxobj[0].height );
         Brain.Screen.printAt( 180,100, "       d:%.2f", local_map.boxobj[0].depth );
 
         Brain.Screen.printAt( 180,120, "map 0: a:%04d c:%04d", local_map.mapobj[0].age, local_map.mapobj[0].classID );
         Brain.Screen.printAt( 180,140, "       0:%5.2f 1:%5.2f", local_map.mapobj[0].p[0], local_map.mapobj[0].p[1]  );
         Brain.Screen.printAt( 180,160, "       2:%5.2f", local_map.mapobj[0].p[2]  );
         
         // Allow other tasks to run
         this_thread::sleep_for(loop_time);
     }
 }
 