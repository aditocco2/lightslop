# Lightslop: A Bare Metal Snake Game

This is my final project for Embedded Systems Design. It is a bare metal Snake game in embedded C using the Raspberry Pi Pico 2 and two MAX7219 LED matrix modules.

For technical details, read the project report [here](https://github.com/user-attachments/files/24152910/report.pdf).

<img width="400" height="600" alt="lsr" src="https://github.com/user-attachments/assets/646a870e-dca8-4e56-ba69-61454af7ab8b" /> <img width="400" height="600" alt="lsl" src="https://github.com/user-attachments/assets/2543c1d2-d071-42ae-ac7d-f434a49100cb" /> 

## Quick Setup
- Wire up the components as shown in the diagram below (the red squares are the LED matrices):
  <img width="600" height="1088" alt="wiring" src="https://github.com/user-attachments/assets/5ab38fae-212c-4076-b607-47124744a34b" />
- Clone the repository
- Plug the Pi Pico 2 into your computer while holding the `BOOTSEL` button
- Drag `lightslop.uf2` into the folder that pops up

## Gameplay
The same starts up in the mode selection screen. Use the left and right buttons to switch between slow speed (small arrow), medium (big arrow), fast (two big arrows), and impossible (skull). Then press the center button to start gameplay.

During gameplay, use the appropriate buttons to steer the snake left or right. Collecting a piece of food (stationary pixel) earns you one point and spawns a new piece of food. Running into yourself or going out of bounds ends the game. After one second, you can press the center button again to go back to the mode selection screen.

## Build Command
To re-compile the game, run the following command (on Linux or WSL) from the base directory of the project:

`LIBS="snake_data.o snake_buffer.o digits.o snake_symbols.o prng.o led_matrices.o dma.o timer0.o five_buttons.o systick.o watchdog.o" make lightslop.uf2`

## Credits
Much credit goes to Dr. Summerville for teaching the class, providing the course environment, and writing basic drivers for DMA, Timer0, SysTick, and Watchdog.
