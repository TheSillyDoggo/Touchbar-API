#pragma once

#include <Geode/Geode.hpp>

// yes, this is just cocos's touch dispatcher
// yes, i am lazy
// have some milk 🥛🥛🥛
// (i was drinking milk while writing this)

namespace touchbar_api
{
    class TBTouchDispatcher : public cocos2d::CCObject, public cocos2d::EGLTouchDelegate
    {
    public:
        ~TBTouchDispatcher();
        bool init(void);
        TBTouchDispatcher()
            : m_pTargetedHandlers(NULL)
            , m_pStandardHandlers(NULL)
            , m_pHandlersToAdd(NULL)
            , m_pHandlersToRemove(NULL)
            
        {}

        static TBTouchDispatcher* get();

    public:
        void addStandardDelegate(cocos2d::CCTouchDelegate *pDelegate, int nPriority);
        void addTargetedDelegate(cocos2d::CCTouchDelegate *pDelegate, int nPriority, bool bSwallowsTouches);

        void removeDelegate(cocos2d::CCTouchDelegate *pDelegate);
        void removeAllDelegates(void);

        void setPriority(int nPriority, cocos2d::CCTouchDelegate *pDelegate);

        void touches(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent, unsigned int uIndex);
        virtual void touchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* pEvent);
        virtual void touchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* pEvent);
        virtual void touchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* pEvent);
        virtual void touchesCancelled(cocos2d::CCSet* touches, cocos2d::CCEvent* pEvent);

    public:
        cocos2d::CCTouchHandler* findHandler(cocos2d::CCTouchDelegate *pDelegate);

    protected:
        void forceRemoveDelegate(cocos2d::CCTouchDelegate *pDelegate);
        void forceAddHandler(cocos2d::CCTouchHandler *pHandler, cocos2d::CCArray* pArray);
        void forceRemoveAllDelegates(void);
        void rearrangeHandlers(cocos2d::CCArray* pArray);
        cocos2d::CCTouchHandler* findHandler(cocos2d::CCArray* pArray, cocos2d::CCTouchDelegate *pDelegate);

    protected:
        cocos2d::CCArray* m_pTargetedHandlers;
        cocos2d::CCArray* m_pStandardHandlers;

        bool m_bLocked;
        bool m_bToAdd;
        bool m_bToRemove;
        cocos2d::CCArray* m_pHandlersToAdd;
        struct cocos2d::_ccCArray *m_pHandlersToRemove;
        bool m_bToQuit;
        bool m_bDispatchEvents;

        // 4, 1 for each type of event
        struct cocos2d::ccTouchHandlerHelperData m_sHandlerHelperData[cocos2d::ccTouchMax];
    };
};