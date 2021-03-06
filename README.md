# Chip8Debug
__A chip8 interpreter / debugger__
*Uses sfml*

![alt text](https://github.com/Guigui220D/Chip8Debug/blob/master/screenshots/debug.PNG)
![alt text](https://github.com/Guigui220D/Chip8Debug/blob/master/screenshots/pong.PNG)
![alt text](https://github.com/Guigui220D/Chip8Debug/blob/master/screenshots/tetris.PNG)
![alt text](https://github.com/Guigui220D/Chip8Debug/blob/master/screenshots/title.PNG)
![alt text](https://github.com/Guigui220D/Chip8Debug/blob/master/screenshots/blackwhite.PNG)

**To compile**
You need SFML Graphics, Window, System and Audio

**To execute**
You must specify a rom to load in the arguments

**Controls :**
The controls uses the azerty keyboard :
1 2 3 4
A Z E R
Q S D F
W X C V
But you can change that

**Other controls :**
P : Pause
M : Step (during pause)
L : Reset (restarts the game but keeps the rom)
O : Change color theme
I : Change pixel mode
Only on the debug window :
K : Enable/Disable debug info
G : The ram explorer goes to the current pc
Left & Right arrows : Navigate in the ram explorer

**Sound**
The sound can be muted by changing a bool

**Tested ROMS (in the rom folder)**
Working:
tetris
invaders
pong2
pong
breakout
missile
15puzzle
Not working / not sure:
connect4
tictac
blitz
blinky

**This project is under Mozilla Public License 2.0**

