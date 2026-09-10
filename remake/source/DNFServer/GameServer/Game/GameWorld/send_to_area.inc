void GameWorld::send_to_area(int vill, int area, PacketGuard& packet)
{
    // ORIG 0x86c8168：各失败条件独立落岛 ⇒ void 早退形 + register bool 物化（技巧22）
    if (vill == 0 && area == 0)
    {
        return;
    }
    // b28d-lane11：P0/N96 清绑（原 register bool asm("al")）。
    // verdict=AE（P198② 谓词内联 + 全 47=47 助记符对齐）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_areaEiiR11PacketGuard：P198② 谓词
    // 内联（ORIG 0x86c8189 `call size; cmp; setl %al; test; jne` 无槽往返）。
    if ((int)g_townScriptFileList.size() < vill)   // ORIG 0x86c8189 setl %al
    {
        return;
    }
    if (vill <= 0)                     // ORIG 0x86c8194 jle
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)       // ORIG 0x86c819e jge
    {
        return;
    }
    Village* pVillage = &m_pVill[vill]; // ORIG 0x86c81af 具名槽 -0xc
    if (area >= pVillage->m_areaCount) // ORIG 0x86c81bb jle
    {
        return;
    }
    if (area <= -1)                    // ORIG 0x86c81c1 js
    {
        return;
    }
    pVillage->m_pAreas[area].send_to_all(packet);
