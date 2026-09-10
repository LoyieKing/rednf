bool GameWorld::IsPowerWarSafeZone(int vill, int area, ENUM_POWER_SIDE_TYPE side)
{
    // ORIG 同 0x86c59e4 族（见 2 参重载注释）：! 取反早退 + 独立 Village* 槽
    if (!(vill == 7 && GlobalData::s_power_manager->IsPowerWarEventOn()))
    {
        return false;
    }
    {
        if (vill < 0 || vill >= MAX_VILLAGE_NUM)
        {
            return true;
        }
        Village* pVill = &m_pVill[vill];
        Area* pArea = pVill->getArea(area);
        if (pArea == NULL)
        {
            return true;
        }
        if ((side == 1 && pArea->m_areaType == 5) ||
            (side == 2 && pArea->m_areaType == 6))
        {
            return true;
        }
    }
    return false;
