http://192.168.200.3:53777/stream/player
http://127.0.0.1:53777/stream/player
http://127.0.0.1:53777/stream/live.mjpg

adb forward tcp:53777 tcp:53777
adb.exe push libs\armeabi-v7a\aremote-tester /data/local/tmp/aremote-tester
adb.exe shell /system/bin/chmod 0777 /data/local/tmp/aremote-tester
adb.exe shell /data/local/tmp/aremote-tester

<Option compiler="gcc" use="1" buildCommand="$compiler -S -masm=intel -c $file -o $object\n" />
<Option compiler="gcc" use="1" buildCommand="$compiler $options $includes -c $file -o $object -Wa,-alhds=$objects_output_dir$file_name.list" />
gcc -S -masm=intel test.c
objdump -d --disassembler-options=intel a.out
clang++ -S -mllvm --x86-asm-syntax=intel test.cpp
.intel_syntax noprefix

..\..\..\..\__LIB__\android-aosp-header\android-6.0.0_r1\system\core\include

..\..\..\..\__LIB__\LUA\lib\
