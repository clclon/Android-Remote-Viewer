@echo off

IF NOT "%1" EQU "" (
cd %1
)

del /Q /S tmp
del /Q /S doxygen.log
del /Q /S ..\AREMOTE.chm
E:\__Bin64\doxygen\bin\doxygen.exe Doxyfile
start ..\AREMOTE.chm
