// [b28d-solo-TaiwanInternalDispatcher] AE（10 链式块 + p=0；CreateDnfEventInfo ctor nothrow）
bool TaiwanInternalDispatcher::init()
{
    TaiwanInterfaceInterDispatcher* p = 0;
    p = new InterLoadServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterLoadGeolocation();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterCheckStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectCreateDnfEventInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectMobileAuthReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectPcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterUpdateP2pHolePunchingState();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterInsertGiveBoxEventReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterLoadHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    return true;
}