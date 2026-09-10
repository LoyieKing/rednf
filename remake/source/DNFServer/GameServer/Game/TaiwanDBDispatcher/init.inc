bool TaiwanDBDispatcher::init()
{
    TaiwanInterfaceDBDispatcher* p = 0;
    p = new LoadServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new SaveServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new LoadGeolocation();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new CheckStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new ResetStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBUpdateExitTime();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectCreateDnfEventInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBInsertCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectMobileAuthRewardTw();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectPcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBUpdatePcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBInsertGiveBoxEventReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBLoadHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSaveHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    return true;
