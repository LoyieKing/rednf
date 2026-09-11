void GameWorld::send_to_area(int vill, int area, PacketGuard& packet,
                             CBelong* belong)
{
    // 同上族（ORIG 0x86c8… send_group 版）
    if (vill == 0 && area == 0)
    {
        return;
    }
    // b28d-lane11：P0/N96 清绑。
    // verdict=AE（P198② 谓词内联）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_areaEiiR11PacketGuardP7CBelong：
    // P198② 谓词内联（ORIG setl 无槽往返，具名 sizeBad 多 mov/cmpb 两条）。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= 0)
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
    pVillage->m_pAreas[area].send_group(packet, belong);
}