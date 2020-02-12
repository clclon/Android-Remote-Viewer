
rem android-elf-cleaner.exe libs\armeabi-v7a\aremote
adb.exe push libs\armeabi-v7a\aremote /data/local/tmp/aremote
adb.exe shell /system/bin/chmod 0777 /data/local/tmp/aremote
rem adb.exe shell /data/local/tmp/aremote
