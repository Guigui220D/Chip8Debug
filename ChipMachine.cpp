//https://github.com/Guigui220D/

#include "ChipMachine.h"
#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>

ChipMachine::ChipMachine()
{

}

ChipMachine::~ChipMachine()
{

}

//Reset everything, except ram
void
ChipMachine::init()
{
    clk = sf::Clock();
    hasToStop = false;
    pc = 0x200;
    i = 0;
    sp = 0;
    clearScreen();  //Clear the screen
    for(int i = 0; i < 16; ++i)
    {
        pstack[i] = 0;
        keys[i] = 0;
        v[i] = 0;
    }
	for (unsigned char i = 0; i < 80; i++)
        mem[i] = chars[i];
    delayTimer = 0;
    soundTimer = 0;
}

//Emulate one cycle, this shoule be done 540 times per second
void
ChipMachine::emulateCycle()
{
    //Both timers are decreased 60 times per second
    if (clk.getElapsedTime().asMilliseconds() >= 16.67)
    {
        clk.restart();
            if (soundTimer > 0)
        soundTimer--;
            if (delayTimer > 0)
        delayTimer--;
    }
    if (hasToStop)
        return;
    pcChanged = false;
    unsigned short op = getOp();
    //These are all the switch for ops
    //I recommend this doc http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    switch (op & 0xF000)
    {
    case (0x0000):
        if (op == 0x00E0)       //CLS
        {
            clearScreen();
            break;
        }
        if (op == 0x00EE)       //RET
        {
            if (sp > 0)
            {
                sp--;
                pc = pstack[sp];
            }
            break;
        }
        std::cout << std::hex << "Unknown OP : " << op;
        break;
        case (0x1000):          //JP addr
            pc = op & 0x0FFF;
            pcChanged = true;
            break;
        case (0x2000):          //CALL addr
            if (sp < 16)
            {
                pstack[sp] = pc;
                sp++;
                pc = op & 0x0FFF;
                pcChanged = true;
            }
            break;
        case (0x3000):          //SE Vx, byte
            if ((op & 0x00FF) == v[(op & 0x0F00) >> 8])
                pc += 2;
            break;
        case (0x4000):          //SNE Vx, byte
            if ((op & 0x00FF) != v[(op & 0x0F00) >> 8])
                pc += 2;
            break;
        case (0x5000):          //SE Vx, Vy
            if (v[(op & 0x00F0) >> 4] == v[(op & 0x0F00) >> 8])
                pc += 2;
            break;
        case (0x6000):          //LD Vx, byte
            v[(op & 0x0F00) >> 8] = op & 0x00FF;
            break;
        case (0x7000):          //ADD Vx, byte
            v[(op & 0x0F00) >> 8] += op & 0x00FF;
            break;
        case (0x8000):
            switch(op & 0x000F)
            {
            case (0x0000):      //LD Vx, Vy
                v[(op & 0x0F00) >> 8] = v[(op & 0x00F0) >> 4];
                break;
            case (0x0001):      //OR Vx, Vy
                v[(op & 0x0F00) >> 8] |= v[(op & 0x00F0) >> 4];
                break;
            case (0x0002):      //AND Vx, Vy
                v[(op & 0x0F00) >> 8] &= v[(op & 0x00F0) >> 4];
                break;
            case (0x0003):      //XOR Vx, Vy
                v[(op & 0x0F00) >> 8] ^= v[(op & 0x00F0) >> 4];
                break;
            case (0x0004):      //ADD Vx, Vy
                v[15] = ((int)v[(op & 0x0F00) >> 8] + v[(op & 0x00F0) >> 4]) > 255;
                v[(op & 0x0F00) >> 8] += v[(op & 0x00F0) >> 4];
                break;
            case (0x0005):      //SUB Vx, Vy
                v[15] = (v[(op & 0x0F00) >> 8] > v[(op & 0x00F0) >> 4]);
                v[(op & 0x0F00) >> 8] -= v[(op & 0x00F0) >> 4];
                break;
            case (0x0006):      //SHR Vx
                v[15] = (v[(op & 0x0F00) >> 8] & 0x0001);
                v[(op & 0x0F00) >> 8] >>= 1;
                break;
            case (0x0007):      //SUBN Vx, Vy
                v[15] = (v[(op & 0x0F00) >> 8] < v[(op & 0x00F0) >> 4]);
                v[(op & 0x0F00) >> 8] = v[(op & 0x00F0) >> 4] - v[(op & 0x0F00) >> 8];
                break;
            case (0x000E):      //SHL Vx
                v[15] = ((v[(op & 0x0F00) >> 8] & 0x8000) == 0x8000);
                v[(op & 0x0F00) >> 8] <<= 1;
                break;
            default:
                std::cout << std::hex << "Unknown OP : " << op;
                break;
            }
            break;
        case (0x9000):          //SNE Vx, Vy
            if (v[(op & 0x00F0) >> 4]!= v[(op & 0x0F00) >> 8])
                pc += 2;
            break;
        case (0xA000):          //LD I, addr
            i = (op & 0x0FFF);
            break;
        case (0xB000):          //JP V0, addr
            pc = (op & 0x0FFF) + v[0];
            pcChanged = true;
            break;
        case (0xC000):          //RND Vx, byte
            v[(op & 0x0F00) >> 8] = ((rand() %  255) & (op & 0x00FF));
            break;
        case (0xD000):          //DRW Vx, Vy, nibble
            v[15] = 0;
            for (unsigned char x = 0; x < 8; x++)
            {
                for (unsigned char y = 0; y < (op & 0x000F); y++)
                {
                    unsigned char xx = (x + v[(op & 0x0F00) >> 8]) % 64;
                    unsigned char yy = (y + v[(op & 0x00F0) >> 4]) % 32;
                    unsigned char pxl = ((mem[i + y] & (0x0080 >> x)) != 0);
                    if ((screen[xx][yy] == 1) & (pxl == 1))
                    v[15] = 1;
                    screen[xx][yy] ^= pxl;
                }
            }
            break;
        case (0xE000):
            if ((op & 0x00FF) == 0x009E)
            {                   //SKP Vx
                if (keys[v[(op & 0x0F00) >> 8]])
                    pc+=2;
                break;
            }
            if ((op & 0x00FF) == 0x00A1)
            {                   //SKNP Vx
                if (!keys[v[(op & 0x0F00) >> 8]])
                    pc+=2;
                break;
            }
            break;
        case (0xF000):
            unsigned char x = ((op & 0x0F00) >> 8);
            switch (op & 0x00FF)
            {
            case (0x0007):      //LD Vx DT
                v[x] = delayTimer;
                break;
            case (0x000A):      //LD Vx, K
                {
                    int pressed = -1;
                    for(int j = 0; j < 16; j++)
                    {
                        if (keys[j])
                        {
                            pressed = j;
                        }
                    }
                    if (pressed == -1)
                    {
                        pcChanged = true;
                    }
                    else
                    {
                        v[x] = pressed;
                    }
                }
                break;
            case (0x0015):      //LD DT, Vx
                delayTimer = v[x];
                break;
            case (0x0018):      //LD ST, Vx
                soundTimer = v[x];
                break;
            case (0x001E):      //ADD I, Vx
                if(i + v[x] > 0x0FFF)
                {
                    v[15] = 1;
                }
				else
                    v[15] = 0;
                i += v[x];
                break;
            case (0x0029):      //LD F, Vx
                i = v[x] * 5;
                break;
            case (0x0033):      //LD B, Vx
                {
                mem[i] = v[x] / 100;
				mem[i + 1] = (v[x] / 10) % 10;
				mem[i + 2] = (v[x] % 100) % 10;
                break;
                }
            case (0x0055):      //LD [I], Vx
                for (int j = 0; j <= x; ++j)
                    mem[i + j] = v[j];
                i += x + 1;
                break;
            case (0x0065):      //LD Vx, [I]
                for (int j = 0; j <= x; ++j)
                    v[j] = mem[i + j];
                i += x + 1;
                break;
            default:
                std::cout << std::hex << "Unknown OP : " << op;
                break;
            }
            break;

    }
    //Pc changed means that the pc has been changed by an op, jp for example
    if (!pcChanged)
        pc += 2;
    if (pc >= MEMSIZE)
        hasToStop = true;
}

void
ChipMachine::clearScreen()
{
    for (unsigned char x = 0; x < 64; x++)
        for (unsigned char y = 0; y < 32; y++)
            screen[x][y] = 0;
    draw = true;
}

//A rom file is loaded from disk using its path, and put in ram from 0x200
bool
ChipMachine::loadProgram(char const* filename)
{
    for(int i = 0; i < 4096; ++i)
		mem[i] = 0;
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    for (unsigned int i = 0; i < result.size(); i++)
        mem[0x200 + i] = result[i];
    return true;
}

//Takes two bytes of the program to make one op, at pc
unsigned short
ChipMachine::getOp()
{
    if (pc < 0 || pc >= MEMSIZE - 1)
        return 0;
    return (mem[pc] << 8) | mem[pc + 1];
}

//Takes two bytes of the program to make one op, at a given pos
unsigned short
ChipMachine::getOp(unsigned short pos)
{
    if (pos < 0 || pos >= MEMSIZE - 1)
        return 0;
    return (mem[pos] << 8) | mem[pos + 1];
}

//Getters
unsigned char
ChipMachine::getVxReg(int x)
{
    if (x < 0 || x >= 16)
        return 0;
    return v[x];
}

unsigned short
ChipMachine::getIReg()
{
    return i;
}

unsigned char
ChipMachine::getTimer()
{
    return delayTimer;
}

unsigned char
ChipMachine::getSoundTimer()
{
    return soundTimer;
}

unsigned short
ChipMachine::getPC()
{
    return pc;
}


