// [b28d-11b] 修法：内联 bool 条件（P198②）+ 内联 makeRequest 实参（P52/P87）→ IDENTICAL 109 @0x086cc992
void GameWorld::ResetAllPassPadFailCount()
{
    // ORIG 0x086cc992：needReset bool 显式物化（mov $1/$0; test; je，P84）；
    // makeRequest 实参求值序 web→fail→acc（register 缓存 %ebx/%esi，P87/P52）
    int resetCount = 0;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
        {
            continue;   // ORIG 086cc9e5 setle/jne skip 形态
        }
        if (user->getPad()->isActivate() &&
            user->getPad()->getFailCnt() > 0)
        {
            user->getPad()->setFailCnt(0);
            DB_PassPadUpdateFailCnt::makeRequest(
                user->get_acc_id(),
                user->getPad()->getFailCnt(),
                (char*)user->getWebAddress());
            resetCount += 1;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x139d, 0)(
        "RESET %d USERS GOBLIN PAD RESETT OK!!!!", resetCount);
}