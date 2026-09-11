void GameWorld::DeleteDailyItem()
{
    // b28d-lane11：P227 全表达式临时链（ORIG 0x086c7b5c 先求 size/tick 实参再
    // ctor，且 trace 槽 -0x30/-0x1c）。
    // [b28d-solo-GameWorld] _ZN9GameWorld15DeleteDailyItemEv verdict=AE 82=82
    // 修法：删 size 实参的冗余 (int) cast（P41 变体）⇒ 4.4.6 复现 ORIG
    // tick→%esi / size→%ebx（0x086ccca2/0x086cccb4）；cast 形恒 tick→%ebx/size→%esi。
    cMyTrace(__PRETTY_FUNCTION__, 0x13da, 0)("DeleteDailyItem size(%d) start tick(%u)",
        m_UserInWorld.size(), GlobalData::s_systemTime_.getCurTickCount());
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            user->processDelDailyItem();
        }
    }
    GlobalData::s_systemTime_.update();
    cMyTrace(__PRETTY_FUNCTION__, 0x13e6, 0)("DeleteDailyItem end tick(%u)",
        GlobalData::s_systemTime_.getCurTickCount());
}