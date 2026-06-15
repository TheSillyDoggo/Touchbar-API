#include "../objcBridge.hpp"
#include "TouchbarNode.hpp"

std::vector<uint8_t> pixels = {};
int touchbarTexWidth = 0;
int touchbarTexHeight = 0;

void touchbar_api::updateSize(float width, float height)
{
    touchbar_api::TouchbarNode::get()->setContentSize(ccp(width, height));
}