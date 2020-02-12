
#if !defined(__CMDLCAT)
#  define __CMDLCAT(...)
#endif
#if !defined(__CMDLITEM1)
#  define __CMDLITEM1(...)
#endif
#if !defined(__CMDLITEM2)
#  define __CMDLITEM2(...)
#endif

__CMDLCAT(  0,       opt,         "Base") 
__CMDLITEM2(0, "-w", wlanip,      "Using Wlan interface for REST server")
__CMDLITEM2(0, "-p", port,        "REST server port")
__CMDLITEM2(0, "-l", loglevel,    "Logging level, valid: DEBUG, INFO, WARN, ERROR")
__CMDLITEM2(0, "-f", logfile,     "Logging file name")
__CMDLITEM2(0, "-a", autosave,    "Macro autosave")
__CMDLCAT(  1,       info,        "Information") 
__CMDLITEM2(1, "-h", help,        "This information")
__CMDLITEM1(1,       infoip,      "Wlan IP address and port")
__CMDLITEM1(1,       infodisplay, "Display information")
__CMDLITEM1(1,       infoinput,   "Input device information")
__CMDLITEM1(1,       inforestapi, "REST server information")

#undef __CMDLITEM1
#undef __CMDLITEM2
#undef __CMDLCAT
