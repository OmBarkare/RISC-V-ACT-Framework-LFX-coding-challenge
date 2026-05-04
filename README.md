# Coding Challenge - RISC-V ACT Framework Enablement and M-Mode Firmware Validation on Hardware Board

## What It Does
- configure UART parameters using `termios` API
- transmit test message over the UART interface
- receive incoming data using poll() and immediate returning read()
- respond to incoming message according simple application level protocol
- gracefully handle configuration, read/write and polling errors

## How to Run
It has two parts, the `main.c` and a `hardware_sim.c`
`socat` was used to simulate serial ports.


1. install utility `socat`
on debian/ubuntu based machines:
```bash
sudo apt install socat
```

2. compile `main.c` and `hardware_sim.c`:
go into the project directory and then compile 
```bash
gcc main.c -o ./build/main
gcc hardware_sim.c -o ./build/hardware_sim
```

3. start socat:
```bash
socat pty,raw,echo=0,link=/tmp/tty0 pty,raw,echo=0,link=/tmp/ttyHw
```
if you want debug to be on in socat, you can run it with the `-dd` flag

4. run `main` and `hardware_sim`:
then in a new terminal instance run
```bash
./build/main
```

again in another terminal instance run
```bash
./build/hardware_sim
```
make sure the working directory for terminals is the project directory to run using these exact commands

## limitations
- hardware specific configurations (like baud rate, parity bit, stop bit) are not meaningfully used for by psuedo-terminals