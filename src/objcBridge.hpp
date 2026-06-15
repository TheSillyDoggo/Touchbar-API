#pragma once

#include <vector>
#include <stdint.h>

extern std::vector<uint8_t> pixels;
extern int touchbarTexWidth;
extern int touchbarTexHeight;

namespace touchbar_api
{
    void dirtifyTouchbar();
    void setupTouchbar();
    void updateSize(float width, float height);

    void touchbarHandleTouchBegan(long id, double x, double y);
    void touchbarHandleTouchMoved(long id, double x, double y);
    void touchbarHandleTouchEnded(long id, double x, double y);
    void touchbarHandleTouchCancelled(long id, double x, double y);
};