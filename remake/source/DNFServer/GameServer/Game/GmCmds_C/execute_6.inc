// ORIG execute 0x084aa858：GM 且槽位 ∈ (9,0x15]（cmp $0x15;jg 允许 ==0x15）时改写背包物品耐久并回发更新包。
void CEnduranceCmd::execute()
{
    // ORIG 0x084aa866 起只有单个具名局部（-0x10 槽，GetUser 结果，P0）；原实现
    // 多声明一个 CUserCharacInfo* 具名局部，多出 -0x14 槽 + 两条 mov 往返。
    CUser* user = GetUser();
    // [b28d-63] ORIG 0x084aa879 起：bool 取反守卫 + 两次独立槽位比较各落
    // 独立 nop 岛（0x15f/0x162/0x165/0x168）⇒ P223 负条件 goto + 尾标签串；
    // m_slot 每次直读（无具名局部，P0）。
    if (!user->isGMUser())
        goto END1;
    if (m_slot <= 9)
        goto END2;
    if (m_slot > 0x15)
        goto END3;

    {
        Inven_Item* item =
            const_cast<CInventory*>(
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW())
                ->GetInvenRef(0, m_slot);
        // ORIG 0x084aa8ca：item==0 直跳 epilogue（0x084aa9c1，无 nop 岛）⇒ 原码是
        // 正向 `if (item != 0) { ... }` 包裹（写成 if(item==0)return 会多一个岛）。
        if (item != 0)
        {
            if (item->m_addInfo == 0)
                goto END4;

            item->m_fieldb = m_endurance;

            PacketGuard packet;
            packet.put_header(0, 0xe);
            packet.put_byte(3);
            packet.put_short(1);
            static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                ->MakeItemPacket(INVEN_TYPE_INVEN, m_slot, packet);
            packet.finalize(true);
            user->Send(packet);
        }
    }
END1:;
END2:;
END3:;
END4:;
}