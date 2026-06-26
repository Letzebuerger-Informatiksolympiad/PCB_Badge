# PCB_Badge

## Overview

This repository includes everything you need to make the most of your badge. It has the files needed to flash the badge PCB yourself, but you can also use [the handy emulator](https://registration.weoi.org/flasher/) You can write and test code directly in the web app, then flash it to your hardware or download the generated `.uf2` file for manual installation.

**Important:** The web app runs on a remote server. Any changes you make inside the browser are **temporary** and will **not be saved** after you leave or refresh the page. Save the files you are working on to your laptop!

---
## "Flashing" the PCB:
* Press and hold the `RESET` button
* Press the `BOOT` button
* Release both
* The PCB will then be wiped and ready for a new file!

## Using the Web Emulator

The web emulator has some samples you can test and play around with, as well as the key SDK files.
The web emulator allows you to:

* Test your code before flashing the PCB
* Flash the PCB directly without downloading software (using Chrome)
* Generate firmware images (`.uf2`)

Because compilation and emulation run on the server, the browser interface should be treated as a *temporary workspace*.

To add your own file to the emulator, paste your code in `start_here.cpp`. This will refresh when you reload the browser so store your code on your laptop.
[Link to emulator](https://registration.weoi.org/flasher/)

---

## Writing Code Alternative

If you want to develop your own applications, from scrach, you can:

1. Clone or download this GitHub repository to your computer.
2. Write and save your code locally on your own machine.
3. Use an IDE such as `VS Code` or `Arduino IDE` to compile your files and write the PCB.

Arduino IDE is the most beginner friendly option if you do not have experience flashing PCBs.
We recommend if you are writting code from scratch to still use `badge.cpp` for the bit maps and pins.

Arduino IDE board manager: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
Board is named Raspberry Pi Pico
---


## Power Consumption

The device's CPU can consume a significant amount of power if not used carefully. (There is little risk that the PCB will overheat or get damaged, but you might find that the cell battery drains quickly) If you want your project to run from a battery for extended periods, you should design your software with power efficiency in mind.

Some general recommendations (provided by ChatGPT):

* Avoid busy-wait loops whenever possible.
* Put the CPU into sleep or low-power modes when idle.
* Reduce the frequency of unnecessary updates or polling.
* Minimize floating-point calculations and expensive operations.
* Turn off peripherals when they are not needed.

---

## Getting Started

1. Open the web emulator.
2. Test the provided examples.
3. Create your own program in `start_here.cpp`.
4. Paste your code into the web editor.
5. Click **Flash** or download the generated `.uf2` file.
6. Enjoy building!

---

## Learn about the PCB Badge

If you opened this GitHub and your first thought was _wait what is a PCB_, this section is for you! A printed circuit board (PCB) is a sandwitch of conducting and insulating layers with electrical components on top.

| Front | Back |
|-------|------|
| <img src="images/PCB front.png" width="400"> | <img src="images/PCB back.png" width="400"> |

This particular PCB has a microprocessor, an `RP2040` and a small memory module. This means that it acts as a tiny computer. It can output information through the USB-C port, and has a display in the form of a 12x20 LED matirx. It receives information through the six buttons on board and the USB-C port.



---

## 
