// IsRunning
__ERRITEM(running_name_empty,        "program name empty")
__ERRITEM(running_open_proc,         "open proc directory")
__ERRITEM(running_get_pid,           "get program pid")
// IsRunningFound
__ERRITEM(running_found,             "already running!")

// Display
__ERRITEM(display_android_builtin,   "android.getBuiltInDisplay error")
__ERRITEM(display_open_file,         "error open")
__ERRITEM(display_android_capture,   "android.capture error")
__ERRITEM(display_not_size,          "display not return size")
__ERRITEM(display_not_type,          "display not return type")
__ERRITEM(display_android_getconfig, "display get configuration")
__ERRITEM(display_image_buffer_size, "image input buffer size is null")
__ERRITEM(display_image_out_type,    "image output type is unknown")
__ERRITEM(display_image_pixel_format,"image pixel format error")
__ERRITEM(display_image_create_bmz,  "BMZ image create error")
__ERRITEM(display_lodepng,           "lodepng error")
__ERRITEM(display_buffer_jpeg,       "convert to JPEG buffer error")
__ERRITEM(display_not_rotate,        "rotate select!")
__ERRITEM(display_not_ratio,         "bad ratio!")
// DisplayNoFatal 
__ERRITEM(display_notfatal_not_equal,"buffers not equals!")
__ERRITEM(display_notfatal_padding,  "PADDING buffer overflow!")
__ERRITEM(display_notfatal_input,    "INPUT buffer overflow!")
__ERRITEM(display_notfatal_output,   "OUTPUT buffer overflow!")

//Input
__ERRITEM(input_event_open,          "open event directory")
__ERRITEM(input_not_key_type,        "bad key type")
__ERRITEM(input_not_key_value,       "bad key value")
__ERRITEM(input_not_init_device,     "tap device id not initialized")
__ERRITEM(input_not_rotate_id,       "input rotate id incorrect")
__ERRITEM(input_not_orient_id,       "bad orientation id")
__ERRITEM(input_not_tap_support,     "device not support tap")
__ERRITEM(input_not_touch_support,   "touch pad not find in device..")
// InputMacro
__ERRITEM(input_macro_empty,         "macro event list empty!")
__ERRITEM(input_macro_running,       "macro record already running!")
__ERRITEM(input_macro_not_saved,     "not saved macro to file")
__ERRITEM(input_macro_not_found,     "saved macro not found")
__ERRITEM(input_macro_bad_xml,       "bad XML macro format")
__ERRITEM(input_macro_bad_index,     "bad XML macro index")

// HTTPServer
__ERRITEM(server_http_not_start,     "error start HTTP REST")
__ERRITEM(server_http_uri_empty,     "bad request, URI empty")
__ERRITEM(server_http_not_image_type,"bad image type request")
__ERRITEM(server_http_not_image_create,"not create capture image")
__ERRITEM(server_http_not_capture_send, "capture send error")
__ERRITEM(server_http_not_capture_send_part, "captured image not property send..")
__ERRITEM(server_http_not_display_run,"not Display event running")


#undef __ERRITEM
