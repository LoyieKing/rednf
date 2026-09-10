void CQuickParty::destroy()
{
    // ORIG 0x082695ca：G_CGameManager 四独立重调（P48）+ 成员直调——
    // +0x2d4 hash_map::size→ebx；+0x1cc StaticPool Size→esi、Remain→eax、
    // sub ⇒ esi=Size-Remain（占用数）。原经 GetActive*/GetOccupied* wrapper
    // 遮蔽直调形（LinkClosureB GetDBHandle 同案），已展开。
    G_CGameManager()->PutQuickParty(this);
    // b28c-lane3：P227 全表达式临时链——ORIG 0x082695ca 把 size()→%ebx、
    // Size()→%esi、Remain() 结果直接相减（无 registeredCnt/occupiedCnt 具名槽）；
    // 具名局部使 4.4 落 -0x10/-0xc 两槽并多 4 条 mov，改临时链后复现。
    // ORIG fmt @0x8bf5380（GBK，objdump -s .rodata 实证逐字节一致）
    cMyTrace("void QuickParty::CQuickParty::destroy()", 0x32, 0)(
        "QUICK_PARTY_LOG : QUICK \xc6\xc4\xc6\xbc\xc7\xd8\xc1\xa6 \xb5\xcb\xb4\xcf\xb4\xd9. QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>, \xb5\xee\xb7\xcf QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>",
        G_CGameManager()->m_quickPartyPool.Size() -
            static_cast<int>(G_CGameManager()->m_quickPartyPool.Remain()),
        static_cast<int>(G_CGameManager()->m_quickPartyMap.size()));
