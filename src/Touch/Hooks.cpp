#include "TBTouchDispatcher.hpp"
#include <Geode/modify/CCTouchDispatcher.hpp>
#include "../Cpp/TouchbarNode.hpp"

using namespace touchbar_api;
using namespace geode::prelude;

bool isTouchbarNode(CCTouchDelegate* delegate)
{
    auto node = typeinfo_cast<CCNode*>(delegate);

    if (TBTouchDispatcher::get()->findHandler(delegate))
        return true;

    if (!node)
        return false;

    if (node->getParent() == TouchbarNode::get())
        return true;

    // 怖い...
    return isTouchbarNode(reinterpret_cast<CCTouchDelegate*>(node->getParent()));
}


class $modify (TouchBarTouchDispatcher, CCTouchDispatcher)
{
    void addStandardDelegate(CCTouchDelegate *pDelegate, int nPriority)
    {
        if (isTouchbarNode(pDelegate))
            return TBTouchDispatcher::get()->addStandardDelegate(pDelegate, nPriority);

        CCTouchDispatcher::addStandardDelegate(pDelegate, nPriority);
    }

    void addTargetedDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallowsTouches)
    {
        if (isTouchbarNode(pDelegate))
            return TBTouchDispatcher::get()->addTargetedDelegate(pDelegate, nPriority, bSwallowsTouches);

        CCTouchDispatcher::addTargetedDelegate(pDelegate, nPriority, bSwallowsTouches);
    }

    void removeDelegate(CCTouchDelegate *pDelegate)
    {
        if (isTouchbarNode(pDelegate))
            return TBTouchDispatcher::get()->removeDelegate(pDelegate);

        CCTouchDispatcher::removeDelegate(pDelegate);
    }

    void removeAllDelegates(void)
    {
        TBTouchDispatcher::get()->removeAllDelegates();
        CCTouchDispatcher::removeAllDelegates();
    }

    void setPriority(int nPriority, CCTouchDelegate *pDelegate)
    {
        if (isTouchbarNode(pDelegate))
            return TBTouchDispatcher::get()->setPriority(nPriority, pDelegate);

        CCTouchDispatcher::setPriority(nPriority, pDelegate);
    }

    CCTouchHandler* findHandler(CCTouchDelegate *pDelegate)
    {
        if (isTouchbarNode(pDelegate))
            return TBTouchDispatcher::get()->findHandler(pDelegate);

        return CCTouchDispatcher::findHandler(pDelegate);
    }
};