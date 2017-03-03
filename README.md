# avrdude-gui
avrdude front-end written in Qt

##How to use
- Select programmer and device, click read can get the current FUSE bits, drag and drop .hex file to program window can flash into device
- User has the option to choose updating FUSE bits or not

##How to compile
- Create folder named [bin] under source code, copy avrdude executable and avrdude.conf files inside

##Disclaimer
- Tested works under Windows with mingw or msvc compiler, should also work under OS X or Linux
- **Some version of avrdude.exe shipped with Arduino IDE shows HFUSE bits as LFUSE, be extremely careful**
