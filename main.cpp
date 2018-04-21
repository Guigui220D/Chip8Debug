#include <iostream>
#include "ChipMachine.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <sstream>
#include <string>

ChipMachine cm;
sf::RenderWindow* debug = new sf::RenderWindow(sf::VideoMode(600, 400), "Debug");
sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(640, 320), "Chip Eight Emu");
sf::Font font;
sf::Text text;
bool debugMode = true;
bool pause = false;
bool step = false;
unsigned short explorerPos = 0x200;

std::stringstream decomp(unsigned short op)
{
    std::stringstream s;
    switch (op & 0xF000)
    {
    case (0x0000):
        if (op == 0x00E0)       //CLS
        {
            s << "CLS";
        }
        if (op == 0x00EE)       //RET
        {
            s << "RET";
        }
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
            break;
        case (0xF000):
            unsigned char x = ((op & 0x0F00) >> 8);
            switch (op & 0x00FF)
            {
            case (0x0007):      //LD Vx DT
                s << std::hex << "LD V" << ((op & 0x0F00) >> 8) << ", DT";
                break;
            case (0x000A):      //LD Vx, K
                s << std::hex << "LD V" << ((op & 0x0F00) >> 8) << ", K";
                break;
            case (0x0015):      //LD DT, Vx
                s << std::hex << "LD DT, " << ((op & 0x0F00) >> 8);
                break;
            case (0x0018):      //LD ST, Vx
                s << std::hex << "LD ST, " << ((op & 0x0F00) >> 8);
                break;
            case (0x001E):      //ADD I, Vx
                s << std::hex << "ADD I, " << ((op & 0x0F00) >> 8);
                break;
            case (0x0029):      //LD F, Vx
                s << std::hex << "LD F, " << ((op & 0x0F00) >> 8);
                break;
            case (0x0033):      //LD B, Vx
                s << std::hex << "LD B, " << ((op & 0x0F00) >> 8);
                break;
            case (0x0055):      //LD [I], Vx
                s << std::hex << "LD [I], " << ((op & 0x0F00) >> 8);
                break;
            case (0x0065):      //LD Vx, [I]
                s << std::hex << "LD V" << ((op & 0x0F00) >> 8) << ", [I]";
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
    sf::RectangleShape rs;
    rs.setSize(sf::Vector2f(10, 10));
    rs.setFillColor(sf::Color::Green);
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 32; y++)
        {
            if (cm.screen[x][y] != 0)
            {
                rs.setPosition(sf::Vector2f(10 * x, 10 * y));
                window->draw(rs);
            }
        }
    }
}

//The debug window draw
void debugdraw()
{
    if (debugMode)
    {
        std::stringstream s;
        text.setPosition(sf::Vector2f(10, 370));
        text.setString("PAUSE : P, STEP : S, RESET : R, DEBUG MODE : M (ON)");
        debug->draw(text);
        if (pause)
        {
            text.setPosition(sf::Vector2f(520, 0));
            text.setString("PAUSED");
            debug->draw(text);
        }

        s << "PC : 0x " << std::hex << cm.pc;
        text.setPosition(sf::Vector2f(10, 0));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        s << "OP : 0x" << std::hex << cm.getOp() << "    \"" << decomp(cm.getOp()).str() << "\"";
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

        s << "SOUND TIMER : 0x" << std::hex << (int)cm.getTimer();
        text.setPosition(sf::Vector2f(200, 144));
        text.setString(s.str());
        debug->draw(text);
        s.str(std::string());

        s << "DELAY TIMER : 0x" << std::hex << (int)cm.getSoundTimer();
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

        s << "<-    0x" << std::hex << cm.getOp(explorerPos) << "    ->";
        text.setPosition(sf::Vector2f(232, 272));
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
        text.setString("PRESS M TO SWITCH IT ON");
        debug->draw(text);
    }
}

int main()
{
    debug->setPosition(sf::Vector2i());
    window->setPosition(sf::Vector2i(600, 0));

    font.loadFromFile("lucon.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(16);

    cm = ChipMachine();
    cm.init();
    cm.loadProgram("roms/pong2.c8");

    window->setFramerateLimit(540);
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::P)
                    pause = !pause;
                if (event.key.code == sf::Keyboard::S)
                    step = true;
                if (event.key.code == sf::Keyboard::R)
                    cm.init();
            }
        }
        sf::Event devent;
        while (debug->pollEvent(devent))
        {
            if (devent.type == sf::Event::Closed)
                window->close();
            if (devent.type == sf::Event::KeyPressed)
            {
                if (devent.key.code == sf::Keyboard::P)
                    pause = !pause;
                if (devent.key.code == sf::Keyboard::S)
                    step = true;
                if (devent.key.code == sf::Keyboard::Left && debugMode)
                    explorerPos -= 2;
                if (devent.key.code == sf::Keyboard::Right && debugMode)
                    explorerPos += 2;
                if (devent.key.code == sf::Keyboard::G && debugMode)
                    explorerPos = cm.pc;
                if (devent.key.code == sf::Keyboard::R)
                    cm.init();
                if (devent.key.code == sf::Keyboard::M)
                    debugMode = !debugMode;
            }
        }
        if (!pause || step)
        {
            cm.keys[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0);
            cm.keys[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1);
            cm.keys[2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2);
            cm.keys[3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3);
            cm.keys[4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4);
            cm.keys[5] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5);
            cm.keys[6] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6);
            cm.keys[7] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7);
            cm.keys[8] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8);
            cm.keys[9] = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9);
            cm.keys[10] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
            cm.keys[11] = sf::Keyboard::isKeyPressed(sf::Keyboard::B);
            cm.keys[12] = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
            cm.keys[13] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
            cm.keys[14] = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
            cm.keys[15] = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
            cm.emulateCycle();
        }
        step = false;
        //system("pause");
        window->clear(sf::Color(0, 50, 0));
        draw();
        window->display();
        debug->clear(sf::Color::White);
        debugdraw();
        debug->display();
    }
    delete(window);
    return 0;
}
