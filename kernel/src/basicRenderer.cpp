#include "basicRenderer.h"
#include <stdint.h>

void BasicRenderer::drawchar(char chr,unsigned int xoff,unsigned int yoff){
    unsigned int* pixPtr = (unsigned int*)_targetframebuffer->baseAddress;
    char* fontPtr = (char*)_font->glyphBuffer + (chr * _font->header->charsize);
    for (unsigned long y = yoff; y < yoff + 16; y++){
        for (unsigned long x = xoff; x < xoff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * _targetframebuffer->pixelsperscanline)) = _color;
                }

        }
        fontPtr++;
    }
}
void BasicRenderer::print(const char* str){
	const char* chr = str;
	while(*chr != 0){
		drawchar(*chr,_CursorPos.x,_CursorPos.y);
		_CursorPos.x+=8;
		if(_CursorPos.x + 8 > _targetframebuffer->width){
			_CursorPos.x = 0;
			_CursorPos.y += 16;
		} 
		chr++;
	}
}
void BasicRenderer::println(const char* str){
	const char* chr = str;
	while(*chr != 0){
		drawchar(*chr,_CursorPos.x,_CursorPos.y);
		_CursorPos.x+=8;
		if(_CursorPos.x + 8 > _targetframebuffer->width){
			_CursorPos.x = 0;
			_CursorPos.y += 16;
		} 
		chr++;
	}
    _CursorPos = {_xstart,_CursorPos.y+16};
}
void BasicRenderer::inc(){
    _CursorPos = {_xstart,_CursorPos.y+16};
}
BasicRenderer::BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font){
    _targetframebuffer = framebuffer;
    _font = font;
    _color = 0xffffff;
    _CursorPos = {0,0};
    _xstart = 0;
}
BasicRenderer::BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font, unsigned int color){
    _targetframebuffer = framebuffer;
    _font = font;
    _color = color;
    _CursorPos = {0,0};
    _xstart = 0;
}
BasicRenderer::BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font, unsigned int startx, unsigned int starty){
    _targetframebuffer = framebuffer;
    _font = font;
    _color = 0xffffff;
    _CursorPos = {startx,starty};
    _xstart = startx;
}
BasicRenderer::BasicRenderer(FrameBuffer* framebuffer, PSF1_FONT* font, unsigned int color, unsigned int startx, unsigned int starty){
    _targetframebuffer = framebuffer;
    _font = font;
    _color = color;
    _CursorPos = {startx,starty};
    _xstart = startx;
}
void BasicRenderer::setcolor(unsigned int color){
    _color = color;
}
