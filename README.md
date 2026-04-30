I do not have a device which I can connect and it they communicates from a USB port.
My laptop also does not have physical serial pins, so I simulate this serial port to serial port connection by using socat.
socat is a utility that lets you create two bidirectional byte streams and you can transfer data between them.
socat creates two pts in /dev/pts and connects them together, so that they act like serial uart terminals.

### limitations
the configurations we set using tts_config() are then completely ignored. this can only accurately verify the non-blocking logic implemented using poll()