#include <Geode/Geode.hpp>
#include "../objcBridge.hpp"
#include "TBTouchDispatcher.hpp"

using namespace geode::prelude;

std::unordered_map<long, geode::Ref<CCTouch>> __globalTouches = {};

int getUnusedID()
{
    return __globalTouches.size();
}

void touchbar_api::touchbarHandleTouchBegan(long id, double x, double y)
{
    // touchbars already dont have vertical inputs, so this prevents software / hardware error
    y = 15;

    auto touch = new CCTouch();
    touch->autorelease();
    touch->setTouchInfo(getUnusedID(), x, CCDirector::get()->getWinSize().height - y);
    __globalTouches[id] = touch;

    CCSet set;
    set.addObject(touch);
    touchbar_api::TBTouchDispatcher::get()->touchesBegan(&set, nullptr);
}

void touchbar_api::touchbarHandleTouchMoved(long id, double x, double y)
{
    y = 15;
    auto touch = __globalTouches[id];

    touch->setTouchInfo(touch->getID(), x, CCDirector::get()->getWinSize().height - y);

    CCSet set;
    set.addObject(touch);
    touchbar_api::TBTouchDispatcher::get()->touchesMoved(&set, nullptr);
}

void touchbar_api::touchbarHandleTouchEnded(long id, double x, double y)
{
    y = 15;
    auto touch = __globalTouches[id];

    touch->setTouchInfo(touch->getID(), x, CCDirector::get()->getWinSize().height - y);

    CCSet set;
    set.addObject(touch);
    touchbar_api::TBTouchDispatcher::get()->touchesEnded(&set, nullptr);    
    __globalTouches[id] = nullptr;
}

void touchbar_api::touchbarHandleTouchCancelled(long id, double x, double y)
{
    // who really cares what the difference is
    touchbarHandleTouchEnded(id, x, y);
}