/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) Innovation First 2020, All rights reserved.               */
/*                                                                            */
/*    Module:     jetson.h                                                    */
/*    Author:     James Pearman                                               */
/*    Created:    3 Aug 2020                                                  */
/*                                                                            */
/*    Revisions:  V0.1                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef V5_JETSON_H_
#define V5_JETSON_H_

/*----------------------------------------------------------------------------*/
/** @file    jetson.h
  * @brief   Header for jetson communication
*//*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#define	MAX_OBJECT		50		// Maximum number of detected objects

typedef struct {
	int32_t		x, y;			      // The center position of this object where 0,0 is the upper left
	int32_t		width, height;	// The width and height of the bounding box of this object
	int32_t		classID;		    // The class id of this object
	float	    depth;			    // The depth of this object
	float	    prob;		  	    // The probability that this object is in this class
} fifo_object_box;

typedef struct {
	int32_t		framecnt;
	int32_t		status;
	float	    x, y, z;
	float	    az, el, rot;
} POS_RECORD;

typedef struct {
	int32_t		age;		        // The number of iterations since the last valid measurement
	int32_t		classID;	      // The class ID of the object  0: red,  1: blue
	float	    p[3];		        // Position
} MAP_OBJECTS;

// The MAP_RECORD contains everything
typedef struct {
	int32_t					  boxnum;
	int32_t					  mapnum;
	POS_RECORD			  pos;
	fifo_object_box		boxobj[MAX_OBJECT];
	MAP_OBJECTS			  mapobj[MAX_OBJECT];
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

namespace vex {
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
        #define   SYNC1   0xAA
        #define   SYNC2   0x55
        #define   SYNC3   0xCC
        #define   SYNC4   0x33

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
    };
};


#endif /* V5_JETSON_H_ */