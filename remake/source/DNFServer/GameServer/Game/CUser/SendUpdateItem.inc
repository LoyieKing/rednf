// 0x0867c2d8
// ORIG：单槽位物品更新包(0xe)。GetInvenRef 空 → return 0；否则写槽头 +
// 物品字段（addInfo2 为 0 时首 int 写 -1）、强化选项、avatar 宝石槽 0x1e 字节
// 与扩展信息 4 字节、生物装备剩余天数（type==5），追加三转编年史与整件包体。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c2d8 | _ZN5CUser14SendUpdateItemENS_11eSendTargetE14ENUM_ITEMSPACEi
// [b28d-GrpD 判词·N] ORIG 0x0867c2d8：残差为 item 指针具名槽化（我方 -0xS 槽 vs
//   ORIG `GetInvenRef` 的 %eax 直传 `je`）+ 未解析重定位 call 标注；控制流、
//   put_* 序列、字段偏移（+2/+7/+b/+d/+11/0x1e/4/0x5）全一致 ⇒ 无语义缺口。
int CUser::SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot)
{
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(invenType, slot);
    if (item == 0)
        return 0;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    int uid = item->m_addInfo2;   // +0x07
    if (uid == 0)
        ((InterfacePacketBuf*)&packet)->put_int(-1);
    else
        ((InterfacePacketBuf*)&packet)->put_int(item->m_addInfo);   // +0x02
    ((InterfacePacketBuf*)&packet)->put_int(uid);
    ((InterfacePacketBuf*)&packet)->put_byte((int)item->GetItemAttr());
    ((InterfacePacketBuf*)&packet)->put_short((int)item->m_fieldb);
    ((InterfacePacketBuf*)&packet)->put_byte((int)(unsigned char)item->m_field0);
    ((InterfacePacketBuf*)&packet)->put_int(item->m_fieldd);
    ((InterfacePacketBuf*)&packet)->put_byte(
        (int)item->m_amp.getAbilityType());
    ((InterfacePacketBuf*)&packet)->put_short(
        (int)item->m_amp.getAbilityValue());
    if (item->isAvatarItemType())
    {
        ((InterfacePacketBuf*)&packet)->put_int(0x1e);
        // P183：ORIG 对 get_add_info 各次访问独立重物化 getCurCharacInvenR()
        // ->GetAvatarItemMgrR()（socket 一次、expansion 再一次），禁 mgr 缓存。
        const char* socketData =
            getCurCharacInvenR()->GetAvatarItemMgrR()
                ->getJewelSocketData(item->get_add_info());
        ((InterfacePacketBuf*)&packet)->put_binary(socketData, 0x1e);
        const char* expansion =
            static_cast<const char*>(
                getCurCharacInvenR()->GetAvatarItemMgrR()
                    ->GetExpansionInfo(item->get_add_info()));
        ((InterfacePacketBuf*)&packet)->put_int(4);
        if (expansion == 0)
        {
            stAvatarExpansionInfo_t init4;
            init4.init();
            // ORIG 3 处 put_binary 均走 const 重载（asm 0815096a）。
            ((InterfacePacketBuf*)&packet)->put_binary(
                reinterpret_cast<const char*>(&init4), 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_binary(expansion, 4);
        }
    }
    if (item->m_field1 == (unsigned char)5)
    {
        int tick = 0;
        tick = OS_API::GetDateTimeTick();
        int remainDate = getCurCharacInvenR()->GetCreatureMgrR()->GetRemainDate(
            item->get_add_info(), tick);
        ((InterfacePacketBuf*)&packet)->put_int(remainDate);
    }
    G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(this, *item,
                                                            &packet);
    ((InterfacePacketBuf*)&packet)->put_packet(*item);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return 1;
}