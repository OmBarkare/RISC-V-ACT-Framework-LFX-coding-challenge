# Coding Challenge - RISC-V ACT Framework Enablement and M-Mode Firmware Validation on Hardware Board

## What It Does
- configure UART parameters using `termios` API
- transmit test message over the UART interface
- receive incoming data using poll() and non-blocking read()
- respond to incoming message according to specified protocol
- gracefully handle errors

## How to Run
It has two parts, the `main.c` and a `hardware_sim.c`. Due to lack of actual hardware, `socat` was used to simulate serial ports.



1. installing utilities `socat`
on debian/ubuntu based machines:
```bash
sudo apt install socat
```
