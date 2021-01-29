/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) Innovation First 2020, All rights reserved.               */
/*    Licensed under the MIT license.                                         */
/*                                                                            */
/*    Module:     ai_jetson.h                                                 */
/*    Author:     James Pearman                                               */
/*    Created:    3 Aug 2020                                                  */
/*                                                                            */
/*    Revisions:  V0.1                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef AI_JETSON_H_
#define AI_JETSON_H_

/*----------------------------------------------------------------------------*/
/** @file    jetson.h
  * @brief   Header for jetson communication
*//*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#define	MAX_OBJECT		50		// Maximum number of detected objects


/// This structure represents a visual detection of a VEX object from the forward facing depth camera.
/// These objects are in refference to the video image frame 
typedef struct {
	int32_t		x, y;			      // The center position of this object where 0,0 is the upper left. Max X = 320. Max Y = 240
	int32_t		width, height;	// The width and height of the bounding box of this object
	int32_t		classID;		    // The class id of this object (0 = Red 1 = Blue 2 = Goal)
	float	    depth;			    // The depth of this object in mm from the camera
	float	    prob;		  	    // The probability that this object is in this class (1.0 == 100%)
} fifo_object_box;

/// This structure represents a the robots location in refference to the center of the playing field
typedef struct {
	int32_t		framecnt;       // This counter increments each frame
	int32_t		status;         // 0 = All Good, != 0 = Not All Good
	float	    x, y, z;        // X,Y,Z field coordinates in millimeters. Position 0,0 is in the middle of the field.
                            // Z is mm from the field tiles.
                            // NOTE: These coordinates are for the GPS sensor (FLIR Camera) If you want to know the location of the 
                            // center of your robot you will have add an offset. 
	float	    az;             // Rotation of the robot in radians (Heading)
  float     el;             // Elevation of the robot in radians (Pitch)
  float     rot;            // Rotation/Tilt of the robot in radians (Roll)
} POS_RECORD;

/// This structure represents a visual detection of a VEX object from the forward facing depth camera.
/// These objects are in refference to the playing field.
typedef struct {
	int32_t		age;		        // The number of iterations since the last valid measurement
	int32_t		classID;	      // The class ID of the object  0: red,  1: blue
  float     positionX;      // X position field coordinates in millimeters. Position 0,0 is in the middle of the field.
  float     positionY;      // Y position field coordinates in millimeters. Position 0,0 is in the middle of the field.
  float     positionZ;      // Z position field coordinates in millimeters. Z represents height above the field tiles.
} MAP_OBJECTS;

// The MAP_RECORD contains everything
typedef struct {
	int32_t					  boxnum;     // Number of objects in the boxobj array
	int32_t					  mapnum;     // Number of objects in the mapobj array 
	POS_RECORD			  pos;        // Position record for the robots position
	fifo_object_box		boxobj[MAX_OBJECT];     // Detected image objects
	MAP_OBJECTS			  mapobj[MAX_OBJECT];     // Detected map objects
} MAP_RECORD;

#define	MAP_POS_SIZE	(2 * sizeof(int32_t) + sizeof(POS_RECORD))

// packet from V5
typedef struct __attribute__((__packed__)) _map_packet {
    // 12 byte header
    uint8_t     sync[4];    // 4 unique bytes that act as sync
    uint16_t    length;     // length of map record payload, does not include header
    uint16_t    type;       // type of packet, 
    uint32_t    crc32;      // crc32 of payload
    
    // map data
    MAP_RECORD  map;
} map_packet;

#ifdef __cplusplus
}
#endif

namespace ai {
  class jetson  {
      public:
        jetson();
        ~jetson();

        int32_t    get_packets(void);
        int32_t    get_errors(void);
        int32_t    get_timeouts(void);
        int32_t    get_total(void);
        int32_t    get_data( MAP_RECORD *map );
        void       request_map();


      private:
        // packet sync bytes
        enum class sync_byte {
            kSync1 = 0xAA,
            kSync2 = 0x55,
            kSync3 = 0xCC,
            kSync4 = 0x33
        };

        #define   MAP_PACKET_TYPE     0x0001

        enum class jetson_state {
            kStateSyncWait1   = 0,
            kStateSyncWait2,
            kStateSyncWait3,
            kStateSyncWait4,
            kStateLength,
            kStateSpare,
            kStateCrc32,
            kStatePayload,
            kStateGoodPacket,
            kStateBadPacket,
        };

        jetson_state  state;
        int32_t       index;
        vex::timer    timer;
        uint32_t      packets;
        uint32_t      errors;
        uint32_t      timeouts;
        uint16_t      payload_length; 
        uint16_t      payload_type; 
        uint32_t      payload_crc32;
        uint32_t      calc_crc32;
        uint32_t      last_packet_time;
        uint32_t      total_data_received;
        
        union {
          MAP_RECORD  map;
          uint8_t     bytes[4096];
        } payload;

        vex::mutex    maplock;

        MAP_RECORD    last_map;
        uint32_t      last_payload_length;

        bool          parse( uint8_t data );

        static int    receive_task( void *arg );

        static  uint32_t _crc32_table[256];
        static  uint32_t  crc32( uint8_t *pData, uint32_t numberOfBytes, uint32_t accumulator );
    };
};


#endif /* AI_JETSON_H_ */