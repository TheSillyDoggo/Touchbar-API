#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include "../objcBridge.hpp"
#include "TouchbarNode.hpp"

using namespace geode::prelude;

class $modify (TouchBarDirector, CCDirector)
{
    void drawScene(void)
    {
        CCDirector::drawScene();
        
        pixels = std::move(touchbar_api::TouchbarNode::get()->visitRT());
        touchbar_api::dirtifyTouchbar();
    }
};

$execute
{
    Loader::get()->queueInMainThread([]
    {
        touchbar_api::setupTouchbar();
    });
}