//Code by Guigui220D
//https://github.com/Guigui220D/

#include <iostream>
#include "ChipMachine.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <sstream>
#include <string>


//The machine
ChipMachine cm;
//The windows
sf::RenderWindow* debug;
sf::RenderWindow* window;
//For the debugging window
sf::Font font;
sf::Text text;
//If debugmode, debug info is displayed on the debug window
bool debugMode = true;
bool pause = false;
bool step = false;
//For the ram explorer in the debugger
unsigned short explorerPos = 0x200;

bool smallPixels = false;
unsigned colorMode = 2;
const unsigned maxModes = 6;
//Background color
sf::Color bColors[] = {sf::Color::Black, sf::Color::White, sf::Color(0, 50, 0), sf::Color(230, 255, 213), sf::Color::Cyan, sf::Color::Red};
//Foreground (ON pixels) color
sf::Color fColors[] = {sf::Color::White, sf::Color::Black, sf::Color::Green, sf::Color(8, 30, 7), sf::Color::Magenta, sf::Color::Yellow};

sf::Keyboard::Key azertyKeys[16] = {sf::Keyboard::X, sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3, sf::Keyboard::A, sf::Keyboard::Z, sf::Keyboard::E, sf::Keyboard::Q, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::W, sf::Keyboard::C, sf::Keyboard::Num4, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::V};

//Give the name of an op, for debugging
std::stringstream decomp(unsigned short op)
{
    std::stringstream s;
    switch (op & 0xF000)
    {
    case (0x0000):
        if (op == 0)
        {
            s << "NOP";
            break;
        }
        if (op == 0x00E0)       //CLS
        {
            s << "CLS";
            break;
        }
        if (op == 0x00EE)       //RET
        {
            s << "RET";
            break;
        }
        s << "UNKNOWN OP";
        break;
        case (0x1000):          //JP addr
            s << std::hex << "JP " << (op & 0x0FFF);
            break;
        case (0x2000):          //CALL addr
            s << std::hex << "CALL " << (op & 0x0FFF);
            break;
        case (0x3000):          //SE Vx, byte
            s << std::hex << "SE V" << ((op & 0x0F00) >> 8) << ", " << (op & 0x00FF);
            break;
        case (0x4000):          //SNE Vx, byte
            s << std::hex << "SNE V" << ((op & 0x0F00) >> 8) << ", " << (op & 0x00FF);
            break;
        case (0x5000):          //SE Vx, Vy
            s << std::hex << "SE V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
            break;
        case (0x6000):          //LD Vx, byte
            s << std::hex << "LD V" << ((op & 0x0F00) >> 8) << ", " << (op & 0x00FF);
            break;
        case (0x7000):          //ADD Vx, byte
            s << std::hex << "ADD V" << ((op & 0x0F00) >> 8) << ", " << (op & 0x00FF);
            break;
        case (0x8000):
            switch(op & 0x000F)
            {
            case (0x0000):      //LD Vx, Vy
                s << std::hex << "LD V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x0001):      //OR Vx, Vy
                s << std::hex << "OR V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x0002):      //AND Vx, Vy
                s << std::hex << "AND V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x0003):      //XOR Vx, Vy
                s << std::hex << "XOR V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x0004):      //ADD Vx, Vy
                s << std::hex << "ADD V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x0005):      //SUB Vx, Vy
                s << std::hex << "SUB V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x0006):      //SHR Vx
                s << std::hex << "SHR V" << ((op & 0x0F00) >> 8);
                break;
            case (0x0007):      //SUBN Vx, Vy
                s << std::hex << "SUBN V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
                break;
            case (0x000E):      //SHL Vx
                s << std::hex << "SHL V" << ((op & 0x0F00) >> 8);
                break;
            default:
                s << "UNKNOWN OP";
                break;
            }
            break;
        case (0x9000):          //SNE Vx, Vy
            s << std::hex << "SNE V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4);
            break;
        case (0xA000):          //LD I, addr
            s << std::hex << "LD I, " << (op & 0x0FFF);
            break;
        case (0xB000):          //JP V0, addr
            s << std::hex << "LD V0, " << (op & 0x0FFF);
            break;
        case (0xC000):          //RND Vx, byte
            s << std::hex << "RND V" << ((op & 0x0F00) >> 8) << ", " << (op & 0x00FF);
            break;
        case (0xD000):          //DRW Vx, Vy, nibble
            s << std::hex << "DRW V" << ((op & 0x0F00) >> 8) << ", V" << ((op & 0x00F0) >> 4) << ", " << (op & 0x000F);
            break;
        case (0xE000):
            if ((op & 0x00FF) == 0x009E)
            {                   //SKP Vx
                s << std::hex << "SKP V" << ((op & 0x0F00) >> 8);
                break;
            }
            if ((op & 0x00FF) == 0x00A1)
            {                   //SKNP Vx
                s << std::hex << "SKNP V" << ((op & 0x0F00) >> 8);
                break;
            }
            s << "UNKNOWN OP";
            break;
        case (0xF000):
            unsigned short x = ((op & 0x0F00) >> 8);
            switch (op & 0x00FF)
            {
            case (0x0007):      //LD Vx DT
                s << std::hex << "LD V" << x << ", DT";
                break;
            case (0x000A):      //LD Vx, K
                s << std::hex << "LD V" << x << ", K";
                break;
            case (0x0015):      //LD DT, Vx
                s << std::hex << "LD DT, V" << x;
                break;
            case (0x0018):      //LD ST, Vx
                s << std::hex << "LD ST, V" << x;
                break;
            case (0x001E):      //ADD I, Vx
                s << std::hex << "ADD I, V" << x;
                break;
            case (0x0029):      //LD F, Vx
                s << std::hex << "LD F, V" << x;
                break;
            case (0x0033):      //LD B, Vx
                s << std::hex << "LD B, V" << x;
                break;
            case (0x0055):      //LD [I], Vx
                s << std::hex << "LD [I], V" << x;
                break;
            case (0x0065):      //LD Vx, [I]
                s << std::hex << "LD V" << x << ", [I]";
                break;
            default :
                s << "UNKNOWN OP";
                break;
            }
            break;
    }
    return s;
}

//The game draw
void draw()
{
    int pcount = 0;
    for (int x = 0; x < 64; x++)
    for (int y = 0; y < 32; y++)
    {
        if (cm.screen[x][y])
            pcount++;
    }
    if (pcount > 0)
    {
        sf::VertexArray pixels = sf::VertexArray(sf::Quads, pcount * 4);
        int i = 0;
        for (int x = 0; x < 64; x++)
        for (int y = 0; y < 32; y++)
        {
            if (cm.screen[x][y])
            {
                pixels[i * 4 + 0].position = sf::Vector2f(x * 10 + smallPixels, y * 10 + smallPixels);
                pixels[i * 4 + 1].position = sf::Vector2f(x * 10 + 10 - smallPixels, y * 10 + smallPixels);
                pixels[i * 4 + 2].position = sf::Vector2f(x * 10 + 10 - smallPixels, y * 10 + 10 - smallPixels);
                pixels[i * 4 + 3].position = sf::Vector2f(x * 10 + smallPixels, y * 10 + 10 - smallPixels);
                i++;
            }
        }
        for (int x = 0; x < pcount * 4; x++)
            pixels[x].color = fColors[colorMode];
        window->draw(pixels);
    }
}

//The debug window draw
void drawDebug()
{
    if (debugMode)
    {
        std::stringstream s;
        text.setPosition(sf::Vector2f(10, 370));
        text.setString("PAUSE : P, STEP : M, RESET : L, DEBUG MODE : K (ON)");
        debug->draw(text);
        if (pause)
        {
            text.setPosition(sf::Vector2f(520, 0));
            text.setString("PAUSED");
            debug->draw(text);
        }
        text.setPosition(sf::Vector2f(10, 386));
        text.setString("CHANGE COLOR MODE : O, CHANGE PIXEL MODE : I");
        debug->draw(text);

        s << "PC : 0x " << std::hex << cm.getPC();
        text.setPosition(sf::Vector2f(10, 0));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        s << "OP : 0x" << std::hex << cm.getOp();
        if (pause)
            s << "    \"" << decomp(cm.getOp()).str() << "\"";
        text.setPosition(sf::Vector2f(10, 16));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        s << "I : 0x" << std::hex << cm.getIReg();
        text.setPosition(sf::Vector2f(10, 48));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        for (int x = 0; x < 16; x++)
        {
            s << "V" << x << " : 0x" << std::hex << (int)cm.getVxReg(x);
            text.setPosition(sf::Vector2f(10, 74 + x * 16));
            text.setString(s.str());
            debug->draw(text);
            s.str(std::string());
        }

        text.setPosition(sf::Vector2f(200, 48));
        text.setString("KEYS : ");
        debug->draw(text);
        s.str(std::string());

        for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
        {
            s << '[' << (x + y * 4) << ":" << std::hex << (int)cm.keys[x + y * 4] << ']';
            text.setPosition(sf::Vector2f(x * 64 + 200, y * 16 + 64));
            text.setString(s.str());
            debug->draw(text);
            s.str(std::string());
        }

        s << "SOUND TIMER : 0x" << std::hex << (int)cm.getSoundTimer();
        text.setPosition(sf::Vector2f(200, 144));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        s << "DELAY TIMER : 0x" << std::hex << (int)cm.getTimer();
        text.setPosition(sf::Vector2f(200, 160));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        text.setPosition(sf::Vector2f(200, 224));
        text.setString("RAM EXPLORER");
        debug->draw(text);

        s << "AT 0x" << std::hex << explorerPos;
        text.setPosition(sf::Vector2f(200, 240));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        s << "<-    0x" << std::hex << cm.getOp(explorerPos) << "    ->    (\"" << decomp(cm.getOp(explorerPos)).str() << "\")";
        text.setPosition(sf::Vector2f(184, 272));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        text.setPosition(sf::Vector2f(200, 304));
        text.setString("GOTO PC : G, USE LEFT AND RIGHT");
        debug->draw(text);
    }
    else
    {
        text.setPosition(sf::Vector2f(10, 0));
        text.setString("DEBUG DISABLED");
        debug->draw(text);
        text.setPosition(sf::Vector2f(10, 32));
        text.setString("PRESS K TO SWITCH IT ON");
        debug->draw(text);
    }
}

void registerAllKeys(sf::Keyboard::Key keys[16])
{
    //Register all keys, from 0 to f
    for (int i = 0; i < 16; i++)
    {
        cm.keys[i] = sf::Keyboard::isKeyPressed(keys[i]);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "You must specify a rom to load\n";
        system("pause");
        return 1;
    }

    window = new sf::RenderWindow(sf::VideoMode(640, 320), "Chip Eight Emu");
    debug = new sf::RenderWindow(sf::VideoMode(600, 432), "Debug");

    debug->setPosition(sf::Vector2i());
    window->setPosition(sf::Vector2i(600, 0));

    //The text for the debugger
    font.loadFromFile("lucon.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(16);

    //Init the machine and load the rom
    cm = ChipMachine();
    cm.init();
    cm.loadProgram(argv[1]);

    //The cpu runs 540 cycles per second
    window->setFramerateLimit(540);

    //The main loop
    while (window->isOpen())
    {
        //Register events for the game window
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
            if (event.type == sf::Event::KeyPressed)
            {
                //Pause
                if (event.key.code == sf::Keyboard::P)
                    pause = !pause;
                //Step
                if (event.key.code == sf::Keyboard::M && pause)
                    step = true;
                //Reset
                if (event.key.code == sf::Keyboard::L)
                    cm.init();
                //Change color mode
                if (event.key.code == sf::Keyboard::O)
                {
                    colorMode++;
                    colorMode %= maxModes;
                }
                //Change pixel mode
                if (event.key.code == sf::Keyboard::I)
                {
                    smallPixels = !smallPixels;
                }
            }
        }
        //Same for the debugger
        sf::Event devent;
        while (debug->pollEvent(devent))
        {
            if (devent.type == sf::Event::Closed)
                window->close();
            if (devent.type == sf::Event::KeyPressed)
            {
                //Pause
                if (devent.key.code == sf::Keyboard::P)
                    pause = !pause;
                //Step
                if (devent.key.code == sf::Keyboard::M && pause)
                    step = true;
                //Navigate in the ram explorer
                if (devent.key.code == sf::Keyboard::Left && debugMode)
                    explorerPos -= 2;
                if (devent.key.code == sf::Keyboard::Right && debugMode)
                    explorerPos += 2;
                //Goto pc in the ram explorer
                if (devent.key.code == sf::Keyboard::G && debugMode)
                    explorerPos = cm.getPC();
                //Reset
                if (devent.key.code == sf::Keyboard::L)
                    cm.init();
                //Enable/disable the debugger
                if (devent.key.code == sf::Keyboard::K)
                    debugMode = !debugMode;
                //Change color mode
                if (devent.key.code == sf::Keyboard::O)
                {
                    colorMode++;
                    colorMode %= maxModes;
                }
                //Change pixel mode
                if (devent.key.code == sf::Keyboard::I)
                {
                    smallPixels = !smallPixels;
                }
            }
        }
        if (!pause || step)
        {
            registerAllKeys(azertyKeys);
            cm.emulateCycle();
        }
        step = false;
        //Draw the game
        window->clear(bColors[colorMode]);
        draw();
        //Draw the debugger
        debug->clear(sf::Color::White);
        drawDebug();
        debug->display();
        window->display();
    }
    delete(window);
    delete(debug);
    return 0;
}
