#include "TouchbarNode.hpp"
#include "../objcBridge.hpp"

using namespace geode::prelude;
using namespace touchbar_api;

TouchbarNode* TouchbarNode::get()
{
    static TouchbarNode* instance = []
    {
        auto ret = new TouchbarNode();
        ret->onEnter();
        return ret;
    }();
    
    return instance;
}

std::vector<uint8_t> TouchbarNode::visitRT()
{
    if (rTexSize != getContentSize())
    {
        CC_SAFE_RELEASE(rTex);
        rTex = CCRenderTexture::create(getContentWidth(), getContentHeight(), kCCTexture2DPixelFormat_RGBA8888);
        touchbarTexWidth = rTex->m_nWidth;
        touchbarTexHeight = rTex->m_nHeight;
        rTexSize = getContentSize();
        rTex->retain();
    }

    if (!rTex)
        return {};

    rTex->beginWithClear(0, 0, 0, 0);
    visit();
    rTex->end();

    CCImage* img = rTex->newCCImage();

    std::vector<uint8_t> pixelData(touchbarTexWidth * touchbarTexHeight * 4);
    memcpy(pixelData.data(), img->getData(), touchbarTexWidth * touchbarTexHeight * 4);
    img->release();

    return pixelData;
}