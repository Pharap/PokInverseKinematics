#include "Pokitto.h"
#include <stdint.h>
Pokitto::Core pok;

static const uint16_t palette[] = {
0x0, 0xffff, 0x4b44, 0x9f29, 0xf81f, 0xc1c7, 0xc307, 0x4a49, 0x6b4d, 0x7bcf, 0xc5f1, 0x948d, 0x5ae8,
};

int screen = 0;
int cursorX = 0;
int cursorY = 0;

float flenDirX(float len, int dir) {
    float pi = 3.14159265359;
    dir = ((-dir % 360)+ 360)% 360;
    float fDir = dir;
    return len * cos(pi * fDir / 180);
}

float flenDirY(float len, int dir) {
    float pi = 3.14159265359;
    dir = ((-dir % 360)+ 360)% 360;
    float fDir = dir;
    return len * sin(pi * fDir / 180);
}

float fgetAngle(float x1, float y1, float x2, float y2) {
    float pi = 3.14159265359;
    int angle = (atan2(y1 - y2, x1 - x2))*180 / pi;
    angle = (((-angle) % 360)+ 360)% 360;
    return angle;
}

class Segment {
    public:
    float x;
    float y;
    float dir;
    float index;

    Segment(int x_, int y_, int dir_, int i) {
        x = x_;
        y = y_;
        dir = dir_;
        index = i;
    }

    Segment(){}

    void drawSegment() {
        pok.display.drawLine(x, y, x + flenDirX(7, dir), y + flenDirY(7, dir));
    }

    void moveSegment(float gx, float gy) {
        dir = fgetAngle(gx, gy, x, y);

        x = gx - flenDirX(8, dir);
        y = gy + flenDirY(8, -dir);
    }
};

class Rope {
    public:
    Segment segments[10];

    Rope(int x_, int y_) {
        for (int i = 0; i < 10; i++) {
            segments[i] = Segment(x_, y_, 90, i);
            y_ -= 8;
        }
    }

    void drawRope() {
        for (int i = 0; i < 10; i++) {
            segments[i].drawSegment();
        }
    }

    void moveRope(int cx, int cy) {
        segments[9].moveSegment(cx, cy);
        for (int i = 8; i >= 0; i--) {
            segments[i].moveSegment(segments[i+1].x, segments[i+1].y);
        }
    }
};

void moveCursor() {
    if (pok.buttons.rightBtn()) {
        cursorX += 3;
    }
    if (pok.buttons.upBtn()) {
        cursorY -= 3;
    }
    if (pok.buttons.leftBtn()) {
        cursorX -= 3;
    }
    if (pok.buttons.downBtn()) {
        cursorY += 3;
    }
}

void drawCursor() {
    pok.display.drawPixel(cursorX, cursorY);
}

int main() {
    pok.begin();
    pok.setFrameRate(30);
    pok.display.load565Palette(palette);
    pok.display.bgcolor = 4;
    pok.display.setInvisibleColor(4);
    Rope trope = Rope(50, 87);
    while (pok.isRunning()) {
        if( pok.update()) {
            pok.buttons.pollButtons();
            if (screen == 0) {
                moveCursor();
                trope.moveRope(cursorX, cursorY);
                pok.display.setColor(0);
                trope.drawRope();
                pok.display.setColor(1);
                drawCursor();
            }
        }
    }
}
