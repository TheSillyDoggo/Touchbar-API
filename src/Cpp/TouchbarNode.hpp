#pragma once

#include <Geode/Geode.hpp>

namespace touchbar_api
{
    class TouchbarNode : public cocos2d::CCNode
    {
        protected:
            cocos2d::CCRenderTexture* rTex = nullptr;
            cocos2d::CCSize rTexSize = ccp(0, 0);

        public:
            static TouchbarNode* get();

            std::vector<uint8_t> visitRT();
    };
};