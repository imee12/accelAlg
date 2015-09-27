  /*
 * main.c
 * Constructs a Window housing an output TextLayer to show data from 
 * either modes of operation of the accelerometer.
 */

#include <pebble.h>

#define TAP_NOT_DATA false


  
static Window *s_main_window;
static TextLayer *s_output_layer;
time_t shotTime = 0;
time_t triggerRotationTime = 0;
time_t  triggerTimeoutTime = 0;   

//TextLayer *text_layer;


DataLoggingSessionRef my_data_log;

void handle_init(void) {
  my_data_log = data_logging_create(
    /* tag */                 42,
    /* DataLoggingItemType */ DATA_LOGGING_BYTE_ARRAY,
    /* length */              sizeof(AccelData),
    /* resume */              true
  );
 }

// typedef enum {
//   DATA_LOGGING_SUCCESS = 0, //!< Successful operation
//   DATA_LOGGING_BUSY, //!< Someone else is writing to this logging session
//   DATA_LOGGING_FULL, //!< No more space to save data
//   DATA_LOGGING_NOT_FOUND, //!< The logging session does not exist
//   DATA_LOGGING_CLOSED, //!< The logging session was made inactive
//   DATA_LOGGING_INVALID_PARAMS //!< An invalid parameter was passed to one of the functions
// } DataLoggingResult;


// void accel_data_handler(AccelData *data, uint32_t num_samples) {
//   DataLoggingResult r = data_logging_log(my_data_log, data, num_samples);
// }



 static void data_handler(AccelData *data, uint32_t num_samples) {
 
   
//static void data_handler(AccelRawData *data, uint32_t num_samples, uint64_t timestamp) { 

  
 // int currentTime = time_t;
   time_t now = time(NULL); 
  // Long lived buffer
  static char s_buffer[128];

  //determine accel change
  int accelx = data[1].x  - data[0].x;
  int accely = data[1].y - data[0].y;
  int accelz = data[1].z - data[0].z;
  
    
// time_t shotTime = 0;
// //time_t triggerRotationTime = 0;
// //time_t  triggerTimeoutTime = 0;   

  // Compose string of all data
  snprintf(s_buffer, sizeof(s_buffer), 
    "N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %lu, %lu, %d, %d, %d", 
    data[0].x, data[0].y, data[0].z, 
    data[1].x, data[1].y, data[1].z, 
   // data[2].x, data[2].y, data[2].z, 
           now, shotTime, accelx, accely, accelz
           );
 
    
  //Show the data
   text_layer_set_text(s_output_layer, s_buffer);
  
 
   

   if ( data[1].y > 2500)
     {
     shotTime = now; 
   //  triggerRotationTime = shotTime + .25;
     vibes_short_pulse();
   //  triggerTimeoutTime = shotTime + 1;
   }
       
//      if(now > triggerRotationTime && now < triggerTimeoutTime) {
//      //  if(data[1].z  > 3000 ) {
//          vibes_short_pulse();
//     //    triggerRotationTime = 0;
//     //    triggerTimeoutTime = 0;   
//       // }
//      }
     
   
     
//      if ( data[1].z  > 300 && now > triggerRotationTime && now < triggerTimeoutTime )
//        {
//        vibes_short_pulse();
//      }
//      if (now > triggerTimeoutTime) 
//      {
       
//      }
//    }
   
 

  
  }



//output log data to phone
//data_logging_log(data->logging_session, (uint8_t *)&now, 1);




void handle_deinit(void) {
  data_logging_finish(my_data_log);
}





static void tap_handler(AccelAxisType axis, int32_t direction) {
  switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
      text_layer_set_text(s_output_layer, "X axis positive.");
    } else {
      text_layer_set_text(s_output_layer, "X axis negative.");
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
      text_layer_set_text(s_output_layer, "Y axis positive.");
    } else {
      text_layer_set_text(s_output_layer, "Y axis negative.");
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
      text_layer_set_text(s_output_layer, "Z axis positive.");
    } else {
      text_layer_set_text(s_output_layer, "Z axis negative.");
    }
    break;
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer
  s_output_layer = text_layer_create(GRect(5, 0, window_bounds.size.w - 10, window_bounds.size.h));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(s_output_layer, "No data yet.");
  text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
}

static void init() {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  // Use tap service? If not, use data service
  if (TAP_NOT_DATA) {
    // Subscribe to the accelerometer tap service
    accel_tap_service_subscribe(tap_handler);
  } else {
    // Subscribe to the accelerometer data service
    int num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
  }
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);

  if (TAP_NOT_DATA) {
    accel_tap_service_unsubscribe();
  } else {
    accel_data_service_unsubscribe();
  }
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

