#include "keyboard.h"

bool isLeftshift = false;
bool isRightshift = false;

void HandleKeyboard(uint8_t scancode){

    switch(scancode){
        case LeftShift:
            isLeftshift = true;
            return;
        case LeftShift + 0x80:
            isLeftshift = false;
            return;
        case RightShift:
            isRightshift = true;
            return;
        case RightShift + 0x80:
            isRightshift = false;
            return;
        case Enter:
            globalrenderer->inc();
            return;
        case BackSpace:
            globalrenderer->clearchar();
            return;
        case SpaceBar:
            globalrenderer->drawchar(' ');
            return;

    }

    char ascii = QWERTKeyboard::Translate(scancode, isLeftshift | isRightshift);

    if(ascii != 0){
        globalrenderer->drawchar(ascii);
    }
}