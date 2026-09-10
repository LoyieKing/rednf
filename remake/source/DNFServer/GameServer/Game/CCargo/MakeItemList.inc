int CCargo::MakeItemList(PacketGuard* packet) const
{
    // [b28d-solo-CCargo] P183：ORIG 无 buf 具名指针（每处 `mov 0xc(%ebp),%eax` 直用
    // 形参）；P0：无 world 局部（`call G_GameWorld; mov %eax,(%esp); call IsEnchant…`
    // 右值内联），且正极性 `if (IsEnchantRevisionChannel())`（ORIG `test; je <m_fieldd>`）。
    ((InterfacePacketBuf*)packet)->put_header(0, 0xd);
    ((InterfacePacketBuf*)packet)->put_byte(2);
    // [b28d-solo-CCargo] ORIG 0x0850bf41 `mov 0xc(%ebp),%edx; mov %eax,0x4(%esp);
    // mov %edx,(%esp)`（receiver 入 edx、值留 eax）；4.4 恒 `mov %eax,%edx;
    // mov 0xc(%ebp),%eax`（值入 edx）⇒ 唯余 1 条 N（N7/N18a 族寄存器角色）。
    // P248 档①两端冲突：callee 返回类型是唯一旋钮——改 int 可翻正此点（同函数
    // GetItemCount int 处实测 LHS-first），但 CUser.cpp:10586
    // `sig->m_cargoCapacity = GetCapacity()`（SigTypes m_cargoCapacity=unsigned int）
    // 需 unsigned 才同型直存（b28d-solo-CUser 定案）⇒ 禁单段翻正，封 N。
    // 实测：(int) cast / (int)(unsigned int) / P226 语句表达式三形均无效；
    // GetItemCount unsigned 化后两处同变 RHS-first（已回退）。
    ((InterfacePacketBuf*)packet)->put_short(GetCapacity());
    ((InterfacePacketBuf*)packet)->put_short(GetItemCount());
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_addInfo != 0) {
            ((InterfacePacketBuf*)packet)->put_short(i);
            ((InterfacePacketBuf*)packet)->put_int(m_pItems[i].m_addInfo);
            ((InterfacePacketBuf*)packet)->put_int(m_pItems[i].m_addInfo2);
            ((InterfacePacketBuf*)packet)->put_byte(
                (int)GetIntegratedPvPItemAttr(m_pItems[i]) & 0xff);
            ((InterfacePacketBuf*)packet)->put_short(m_pItems[i].m_fieldb);
            ((InterfacePacketBuf*)packet)->put_byte(m_pItems[i].m_field0);
            if (G_GameWorld()->IsEnchantRevisionChannel())
                ((InterfacePacketBuf*)packet)->put_int(0);
            else
                ((InterfacePacketBuf*)packet)->put_int(m_pItems[i].m_fieldd);
            ((InterfacePacketBuf*)packet)->put_byte(
                m_pItems[i].m_amp.getAbilityType() & 0xff);
            ((InterfacePacketBuf*)packet)->put_short(
                m_pItems[i].m_amp.getAbilityValue() & 0xffff);
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, m_pItems[i], packet);
            ((InterfacePacketBuf*)packet)->put_packet(m_pItems[i]);
        }
    }
    ((InterfacePacketBuf*)packet)->finalize(true);
    return 1;
