adb.exe forward tcp:59999 tcp:59999
start "armeabi-v7a GDB server" /MIN adb.exe shell /data/local/tmp/gdbserver :59999 /data/local/tmp/ainput 
exit
