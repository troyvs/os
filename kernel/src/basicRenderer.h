#pragma once
#include "math.h"
#include "frameBuffer.h"
#include "simpleFonts.h"

class BasicRenderer{
    public:
        BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font);
        BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font, unsigned int color);
        BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font, unsigned int startx, unsigned int starty);
        BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font, unsigned int color, unsigned int startx, unsigned int starty);
        void print(const char* str);
        void setcolor(unsigned int color);
        void println(const char* str);
        void drawchar(char chr,unsigned int xoff,unsigned int yoff);

    protected:

    private:
        FrameBuffer* _targetframebuffer;
        PSF1_FONT* _font;
        point _CursorPos;
        unsigned int _color;
        unsigned int _xstart;

};