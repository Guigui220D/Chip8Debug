#ifndef CHIPMACHINE_H
#define CHIPMACHINE_H
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

class ChipMachine
{
    public:
        //For sound
        sf::SoundBuffer soundBuffer;
        sf::Sound sound;
        bool mute = false;

        static const int MEMSIZE = 4096;
        ChipMachine();
        ~ChipMachine();
        bool loadProgram(char const* filename);
        void init();
        void emulateCycle();
        unsigned char screen[64][32];	//The screen and its 2048 pixels
		unsigned char keys[16];	        //The keys states

		bool hasToStop;

		unsigned char getVxReg(int x);
		unsigned short getIReg();

        unsigned char getTimer();
        unsigned char getSoundTimer();

        unsigned short getOp();
        unsigned short getOp(unsigned short pos);

        unsigned short getPC();

    private:
        unsigned char mem[MEMSIZE];             //The 4 ko ram
        unsigned char v[16];                    //The 16 Vx 8 bits registers
        unsigned short i;
        unsigned char delayTimer, soundTimer;   //Timers that are decremented at a 60Hz rate
        unsigned char sp;                       //Stack pointer
        unsigned short pstack[16];                  //The I 16 bits register
        unsigned short pc;                      //Program counter

        bool pcChanged;

        void clearScreen();

        sf::Clock clk;

        //The characters graphics
        unsigned char chars[80] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0,   //0
            0x20, 0x60, 0x20, 0x20, 0x70,   //1
            0xF0, 0x10, 0xF0, 0x80, 0xF0,   //2
            0xF0, 0x10, 0xF0, 0x10, 0xF0,   //3
            0x90, 0x90, 0xF0, 0x10, 0x10,   //4
            0xF0, 0x80, 0xF0, 0x10, 0xF0,   //5
            0xF0, 0x80, 0xF0, 0x90, 0xF0,   //6
            0xF0, 0x10, 0x20, 0x40, 0x40,   //7
            0xF0, 0x90, 0xF0, 0x90, 0xF0,   //8
            0xF0, 0x90, 0xF0, 0x10, 0xF0,   //9
            0xF0, 0x90, 0xF0, 0x90, 0x90,   //A
            0xE0, 0x90, 0xE0, 0x90, 0xE0,   //B
            0xF0, 0x80, 0x80, 0x80, 0xF0,   //C
            0xE0, 0x90, 0x90, 0x90, 0xE0,   //D
            0xF0, 0x80, 0xF0, 0x80, 0xF0,   //E
            0xF0, 0x80, 0xF0, 0x80, 0x80    //F
        };
};

#endif // CHIPMACHINE_H
