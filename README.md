# avrdude-gui
avrdude front-end written in Qt
![alt tag](https://raw.githubusercontent.com/fay2003hiend/avrdude-gui/07880af2ba8e25fdbcc5dd242fced32a99235221/bin/snapshot.png)

## Pre-compiled mingw32 binary download:
[Download](http://blog.zocmod.com/wp-content/uploads/2017/03/Qavrdude-release.zip)

##How to compile
- The folder named [bin] under source code, you can update avrdude executable and avrdude.conf files inside for other platforms
- Files can be downloaded from http://download.savannah.gnu.org/releases/avrdude/

##How to use
- Select programmer and device, click read can get the current FUSE bits, drag and drop .hex file to program window can flash into device
- User has the option to choose updating FUSE bits or not

##Disclaimer
- Tested works under Windows with mingw or msvc compiler, should also work under OS X or Linux
- **Some version of avrdude.exe shipped with Arduino IDE shows HFUSE bits as LFUSE, be extremely careful**
