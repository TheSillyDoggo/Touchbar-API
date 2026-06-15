#include "TBTouchDispatcher.hpp"
#include <algorithm>

using namespace touchbar_api;
using namespace geode::prelude;

TBTouchDispatcher* TBTouchDispatcher::get()
{
    static TBTouchDispatcher* instance = []{
        auto ret = new TBTouchDispatcher();
        ret->init();
        return ret;
    }();
    return instance;
}

/**
 * Used for sort
 */
static int less(const CCObject* p1, const CCObject* p2)
{
    return ((CCTouchHandler*)p1)->getPriority() < ((CCTouchHandler*)p2)->getPriority();
}

bool TBTouchDispatcher::init(void)
{
    m_bDispatchEvents = true;
    m_pTargetedHandlers = CCArray::createWithCapacity(8);
    m_pTargetedHandlers->retain();
    m_pStandardHandlers = CCArray::createWithCapacity(4);
    m_pStandardHandlers->retain();
    m_pHandlersToAdd = CCArray::createWithCapacity(8);
    m_pHandlersToAdd->retain();
    m_pHandlersToRemove = ccCArrayNew(8);

    m_bToRemove = false;
    m_bToAdd = false;
    m_bToQuit = false;
    m_bLocked = false;

    m_sHandlerHelperData[CCTOUCHBEGAN].m_type = CCTOUCHBEGAN;
    m_sHandlerHelperData[CCTOUCHMOVED].m_type = CCTOUCHMOVED;
    m_sHandlerHelperData[CCTOUCHENDED].m_type = CCTOUCHENDED;
    m_sHandlerHelperData[CCTOUCHCANCELLED].m_type = CCTOUCHCANCELLED;

    return true;
}

TBTouchDispatcher::~TBTouchDispatcher(void)
{
    CC_SAFE_RELEASE(m_pTargetedHandlers);
    CC_SAFE_RELEASE(m_pStandardHandlers);
    CC_SAFE_RELEASE(m_pHandlersToAdd);

    ccCArrayFree(m_pHandlersToRemove);
    m_pHandlersToRemove = NULL;    
}

//
// handlers management
//
void TBTouchDispatcher::forceAddHandler(CCTouchHandler *pHandler, CCArray *pArray)
{
    unsigned int u = 0;

    CCObject* pObj = NULL;
    for (auto& pObj : CCArrayExt<CCObject*>(pArray))
    {
        CCTouchHandler *h = (CCTouchHandler *)pObj;
        if (h)
        {
            if (h->getPriority() < pHandler->getPriority())
            {
                ++u;
            }

            if (h->getDelegate() == pHandler->getDelegate())
            {
                CCAssert(0, "");
                return;
            }
        }
    }

    pArray->insertObject(pHandler, u);
}

void TBTouchDispatcher::addStandardDelegate(CCTouchDelegate *pDelegate, int nPriority)
{    
    CCTouchHandler *pHandler = CCStandardTouchHandler::handlerWithDelegate(pDelegate, nPriority);
    if (! m_bLocked)
    {
        forceAddHandler(pHandler, m_pStandardHandlers);
    }
    else
    {
        /* If pHandler is contained in m_pHandlersToRemove, if so remove it from m_pHandlersToRemove and return.
         * Refer issue #752(cocos2d-x)
         */
        if (ccCArrayContainsValue(m_pHandlersToRemove, pDelegate))
        {
            ccCArrayRemoveValue(m_pHandlersToRemove, pDelegate);
            return;
        }

        m_pHandlersToAdd->addObject(pHandler);
        m_bToAdd = true;
    }
}

void TBTouchDispatcher::addTargetedDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallowsTouches)
{    
    CCTouchHandler *pHandler = CCTargetedTouchHandler::handlerWithDelegate(pDelegate, nPriority, bSwallowsTouches);
    if (! m_bLocked)
    {
        forceAddHandler(pHandler, m_pTargetedHandlers);
    }
    else
    {
        /* If pHandler is contained in m_pHandlersToRemove, if so remove it from m_pHandlersToRemove and return.
         * Refer issue #752(cocos2d-x)
         */
        if (ccCArrayContainsValue(m_pHandlersToRemove, pDelegate))
        {
            ccCArrayRemoveValue(m_pHandlersToRemove, pDelegate);
            return;
        }
        
        m_pHandlersToAdd->addObject(pHandler);
        m_bToAdd = true;
    }
}

void TBTouchDispatcher::forceRemoveDelegate(CCTouchDelegate *pDelegate)
{
    CCTouchHandler *pHandler;

    // XXX: remove it from both handlers ???
    
    // remove handler from m_pStandardHandlers
    CCObject* pObj = NULL;
    for (auto& pObj : CCArrayExt<CCObject*>(m_pStandardHandlers))
    {
        pHandler = (CCTouchHandler*)pObj;
        if (pHandler && pHandler->getDelegate() == pDelegate)
        {
            m_pStandardHandlers->removeObject(pHandler);
            break;
        }
    }

    // remove handler from m_pTargetedHandlers
    for (auto& pObj : CCArrayExt<CCObject*>(m_pTargetedHandlers))
    {
        pHandler = (CCTouchHandler*)pObj;
        if (pHandler && pHandler->getDelegate() == pDelegate)
        {
            m_pTargetedHandlers->removeObject(pHandler);
            break;
        }
    }
}

void TBTouchDispatcher::removeDelegate(CCTouchDelegate *pDelegate)
{
    if (pDelegate == NULL)
    {
        return;
    }

    if (! m_bLocked)
    {
        forceRemoveDelegate(pDelegate);
    }
    else
    {
        /* If pHandler is contained in m_pHandlersToAdd, if so remove it from m_pHandlersToAdd and return.
         * Refer issue #752(cocos2d-x)
         */
        CCTouchHandler *pHandler = findHandler(m_pHandlersToAdd, pDelegate);
        if (pHandler)
        {
            m_pHandlersToAdd->removeObject(pHandler);
            return;
        }

        ccCArrayAppendValue(m_pHandlersToRemove, pDelegate);
        m_bToRemove = true;
    }
}

void TBTouchDispatcher::forceRemoveAllDelegates(void)
{
     m_pStandardHandlers->removeAllObjects();
     m_pTargetedHandlers->removeAllObjects();
}

void TBTouchDispatcher::removeAllDelegates(void)
{
    if (! m_bLocked)
    {
        forceRemoveAllDelegates();
    }
    else
    {
        m_bToQuit = true;
    }
}

CCTouchHandler* TBTouchDispatcher::findHandler(CCTouchDelegate *pDelegate)
{
    CCObject* pObj = NULL;
    for (auto& pObj : CCArrayExt<CCObject*>(m_pTargetedHandlers))
    {
        CCTouchHandler* pHandler = (CCTouchHandler*)pObj;
        if (pHandler->getDelegate() == pDelegate)
        {
            return pHandler;
        }
    }

    for (auto& pObj : CCArrayExt<CCObject*>(m_pStandardHandlers))
    {
        CCTouchHandler* pHandler = (CCTouchHandler*)pObj;
        if (pHandler->getDelegate() == pDelegate)
        {
            return pHandler;
        }
    } 

    return NULL;
}

CCTouchHandler* TBTouchDispatcher::findHandler(CCArray* pArray, CCTouchDelegate *pDelegate)
{
    CCAssert(pArray != NULL && pDelegate != NULL, "");

    CCObject* pObj = NULL;
    for (auto& pObj : CCArrayExt<CCObject*>(pArray))
    {
        CCTouchHandler* pHandle = (CCTouchHandler*)pObj;
        if (pHandle->getDelegate() == pDelegate)
        {
            return pHandle;
        }
    }

    return NULL;
}

void TBTouchDispatcher::rearrangeHandlers(CCArray *pArray)
{
    std::sort(pArray->data->arr, pArray->data->arr + pArray->data->num, less);
}

void TBTouchDispatcher::setPriority(int nPriority, CCTouchDelegate *pDelegate)
{
    CCAssert(pDelegate != NULL, "");

    CCTouchHandler *handler = NULL;

    handler = this->findHandler(pDelegate);

    CCAssert(handler != NULL, "");
	
    if (handler->getPriority() != nPriority)
    {
        handler->setPriority(nPriority);
        this->rearrangeHandlers(m_pTargetedHandlers);
        this->rearrangeHandlers(m_pStandardHandlers);
    }
}

//
// dispatch events
//
void TBTouchDispatcher::touches(CCSet *pTouches, CCEvent *pEvent, unsigned int uIndex)
{
    CCAssert(uIndex >= 0 && uIndex < 4, "");

    CCSet *pMutableTouches;
    m_bLocked = true;

    // optimization to prevent a mutable copy when it is not necessary
    unsigned int uTargetedHandlersCount = m_pTargetedHandlers->count();
    unsigned int uStandardHandlersCount = m_pStandardHandlers->count();
    bool bNeedsMutableSet = (uTargetedHandlersCount && uStandardHandlersCount);

    pMutableTouches = (bNeedsMutableSet ? pTouches->mutableCopy() : pTouches);

    struct ccTouchHandlerHelperData sHelper = m_sHandlerHelperData[uIndex];
    //
    // process the target handlers 1st
    //
    if (uTargetedHandlersCount > 0)
    {
        CCTouch *pTouch;
        CCSetIterator setIter;
        for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
        {
            pTouch = (CCTouch *)(*setIter);

            CCTargetedTouchHandler *pHandler = NULL;
            CCObject* pObj = NULL;
            for (auto& pObj : CCArrayExt<CCObject*>(m_pTargetedHandlers))
            {
                pHandler = (CCTargetedTouchHandler *)(pObj);

                if (! pHandler)
                {
                   break;
                }

                bool bClaimed = false;
                if (uIndex == CCTOUCHBEGAN)
                {
                    bClaimed = pHandler->getDelegate()->ccTouchBegan(pTouch, pEvent);

                    if (bClaimed)
                    {
                        pHandler->getClaimedTouches()->addObject(pTouch);
                    }
                } else
                if (pHandler->getClaimedTouches()->containsObject(pTouch))
                {
                    // moved ended canceled
                    bClaimed = true;

                    switch (sHelper.m_type)
                    {
                    case CCTOUCHMOVED:
                        pHandler->getDelegate()->ccTouchMoved(pTouch, pEvent);
                        break;
                    case CCTOUCHENDED:
                        pHandler->getDelegate()->ccTouchEnded(pTouch, pEvent);
                        pHandler->getClaimedTouches()->removeObject(pTouch);
                        break;
                    case CCTOUCHCANCELLED:
                        pHandler->getDelegate()->ccTouchCancelled(pTouch, pEvent);
                        pHandler->getClaimedTouches()->removeObject(pTouch);
                        break;
                    }
                }

                if (bClaimed && pHandler->isSwallowsTouches())
                {
                    if (bNeedsMutableSet)
                    {
                        pMutableTouches->removeObject(pTouch);
                    }

                    break;
                }
            }
        }
    }

    //
    // process standard handlers 2nd
    //
    if (uStandardHandlersCount > 0 && pMutableTouches->count() > 0)
    {
        CCStandardTouchHandler *pHandler = NULL;
        CCObject* pObj = NULL;
        for (auto& pObj : CCArrayExt<CCObject*>(m_pStandardHandlers))
        {
            pHandler = (CCStandardTouchHandler*)(pObj);

            if (! pHandler)
            {
                break;
            }

            switch (sHelper.m_type)
            {
            case CCTOUCHBEGAN:
                pHandler->getDelegate()->ccTouchesBegan(pMutableTouches, pEvent);
                break;
            case CCTOUCHMOVED:
                pHandler->getDelegate()->ccTouchesMoved(pMutableTouches, pEvent);
                break;
            case CCTOUCHENDED:
                pHandler->getDelegate()->ccTouchesEnded(pMutableTouches, pEvent);
                break;
            case CCTOUCHCANCELLED:
                pHandler->getDelegate()->ccTouchesCancelled(pMutableTouches, pEvent);
                break;
            }
        }
    }

    if (bNeedsMutableSet)
    {
        pMutableTouches->release();
    }

    //
    // Optimization. To prevent a [handlers copy] which is expensive
    // the add/removes/quit is done after the iterations
    //
    m_bLocked = false;
    if (m_bToRemove)
    {
        m_bToRemove = false;
        for (unsigned int i = 0; i < m_pHandlersToRemove->num; ++i)
        {
            forceRemoveDelegate((CCTouchDelegate*)m_pHandlersToRemove->arr[i]);
        }
        ccCArrayRemoveAllValues(m_pHandlersToRemove);
    }

    if (m_bToAdd)
    {
        m_bToAdd = false;
        CCTouchHandler* pHandler = NULL;
        CCObject* pObj = NULL;
        for (auto& pObj : CCArrayExt<CCObject*>(m_pHandlersToAdd))
        {
            pHandler = (CCTouchHandler*)pObj;
            if (! pHandler)
            {
                break;
            }

            if (typeinfo_cast<CCTargetedTouchHandler*>(pHandler) != NULL)
            {                
                forceAddHandler(pHandler, m_pTargetedHandlers);
            }
            else
            {
                forceAddHandler(pHandler, m_pStandardHandlers);
            }
        }

        m_pHandlersToAdd->removeAllObjects();    
    }

    if (m_bToQuit)
    {
        m_bToQuit = false;
        forceRemoveAllDelegates();
    }
}

void TBTouchDispatcher::touchesBegan(CCSet *touches, CCEvent *pEvent)
{
    if (m_bDispatchEvents)
    {
        this->touches(touches, pEvent, CCTOUCHBEGAN);
    }
}

void TBTouchDispatcher::touchesMoved(CCSet *touches, CCEvent *pEvent)
{
    if (m_bDispatchEvents)
    {
        this->touches(touches, pEvent, CCTOUCHMOVED);
    }
}

void TBTouchDispatcher::touchesEnded(CCSet *touches, CCEvent *pEvent)
{
    if (m_bDispatchEvents)
    {
        this->touches(touches, pEvent, CCTOUCHENDED);
    }
}

void TBTouchDispatcher::touchesCancelled(CCSet *touches, CCEvent *pEvent)
{
    if (m_bDispatchEvents)
    {
        this->touches(touches, pEvent, CCTOUCHCANCELLED);
    }
}
