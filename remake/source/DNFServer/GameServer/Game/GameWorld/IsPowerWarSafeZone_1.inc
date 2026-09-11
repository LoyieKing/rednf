bool GameWorld::IsPowerWarSafeZone(int vill, int area)
{
    // ORIG 0x86c59e4-0x86c5a0c：双 bool 物化 mov$1/mov$0+共享 test ⇒
    // `!(A&&B)` 取反早退形（技巧1 xor 来自 !boolcall）
    if (!(vill == 7 && GlobalData::s_power_manager->IsPowerWarEventOn()))
    {
        return false;
    }
    {
        if (vill < 0 || vill >= MAX_VILLAGE_NUM)
        {
            return true;
        }
        // ORIG 0x86c5a2c-0x86c5a48：Village* 独立槽 -0x10（lea+mov slot+reload）
        Village* pVill = &m_pVill[vill];
        Area* pArea = pVill->getArea(area);
        if (pArea == NULL)
        {
            return true;
        }
        if (pArea->m_areaType == 5 || pArea->m_areaType == 6)
        {
            return true;
        }
    }
    return false;
}