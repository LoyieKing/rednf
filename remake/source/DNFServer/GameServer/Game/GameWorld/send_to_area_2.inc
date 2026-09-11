void GameWorld::send_to_area(int vill, int area, PacketGuard& packet,
                             std::vector<CUser*>& users)
{
    // 同上族（vector 版）
    // b28d-lane11：P0/N96 清绑。
    // verdict=AE（P198② 谓词内联）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_areaEiiR11PacketGuardRSt6vectorIP5CUserSaIS4_EE：
    // P198② 谓词内联（ORIG setl 无槽往返）。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= -1)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    if (area >= pVillage->m_areaCount)
    {
        return;
    }
    if (area <= -1)
    {
        return;
    }
    pVillage->m_pAreas[area].send_to_all(packet, users);
}