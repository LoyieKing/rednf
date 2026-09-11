// ============================================================================
// _MakeNotiPacketDieMonster —— ORIG T 0x0830b8cc。
// 怪物死亡通知包（0x26）：uid、掉落列表逐项（idx/数量/属性/avatar 周期·徽章/
// 强化选项/裸 Inven_Item/头像标志）、mapIndex、APC 击杀数、终点坐标。
// ============================================================================
void CBattle_Field::_MakeNotiPacketDieMonster(
    PacketGuard& packet, std::list<map_item>& itemList, int uid,
    unsigned short mapIndex, const map_monster& mobSrc, map_monster& mobOut)
{
    packet.put_header(0, 0x26);
    packet.put_short(uid);
    packet.put_byte(static_cast<int>(itemList.size()));

    // [b28d-20] ORIG 0x0830b923/0x0830ba38 两处均为 `emblem.init()`（单参数成员调用，
    // stAvatarEmblemInfo_t sizeof=0x1e）；memset 三参数形与引用缓存槽（-0x24）均为偏差。
    stAvatarEmblemInfo_t emblem;
    emblem.init();

    for (std::list<map_item>::iterator it = itemList.begin();
         it != itemList.end(); it++)   // [b28d-20] ORIG 0x0830bb79 调 ppEi（后置，P28 增量侧）
    {
        map_item& mi = *it;
        packet.put_short(mi.m_itemIndex);
        packet.put_int(mi.m_item.m_addInfo);
        packet.put_byte(mi.m_item.GetItemAttr());
        // [b28d-20] ORIG 0x0830b9b0 test%al/je<else>（正极性：isAvatarItemType() 为真走
        // avatar 路径）—— `!isAvatarItemType()` 会多一条 xor $1 并反转分支布局。
        if (mi.m_item.isAvatarItemType())
        {
            CItem* found = G_CDataManager()->find_item(mi.m_item.m_addInfo);
            if (found == 0)
            {
                packet.put_int(mi.m_item.get_add_info());
            }
            else
            {
                CEquipItem* equip = static_cast<CEquipItem*>(found);
                // [b28d-20] ORIG 0x0830ba05 movzwl+movzbl（uchar 实参）/ 0x0830ba40
                // movzwl+movzwl（ushort→int 实参）：字段每处重读，无 slot/period 具名局部；
                // getAvatarPeriod 的 this 取自 -0x10（find_item 结果槽），getAvatarSocket 取 -0xc。
                packet.put_int(static_cast<CEquipItem*>(found)->getAvatarPeriod(
                    (unsigned char)mi.m_item.m_fieldb) * 0x15180u);
                emblem.init();
                equip->getAvatarSocket(
                    (unsigned short)mi.m_item.m_fieldb, emblem);
            }
        }
        else
        {
            packet.put_int(mi.m_item.get_add_info());
        }
        packet.put_short(
            static_cast<unsigned int>(mi.m_item.m_fieldb));
        packet.put_byte(mi.m_item.m_amp.getAbilityType());
        packet.put_short(mi.m_item.m_amp.getAbilityValue());
        packet.put_packet(mi.m_item);
        if (mi.m_item.isAvatarItemType())
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            // [b28d-20] ORIG 调 put_binary(char const*, int)（p167 实证
            // _ZN18InterfacePacketBuf10put_binaryEPKci）；传 char* 会选到
            // put_binary(char*, int) 重载（EXTRA Pci）。
            packet.put_binary(reinterpret_cast<const char*>(&emblem), 0x1e);
        }
        else
        {
            packet.put_byte(0);
        }
        packet.put_short(static_cast<unsigned int>(mapIndex));
    }

    packet.put_byte(0);
    // [b28d-20] ORIG 0x0830bbdc test%al/setne%al/test%al/je：条件直判 callee 返回值，
    // 无 roleCvt 具名槽（具名 char 局部会产 mov %al,-N; cmpb $0,-N 两段）。
    if (convertAPCRoleType2MonsterRoleType(
            static_cast<char>(mobSrc.m_roleType)) != 0)
        packet.put_byte(m_apcKillCount);
    else
        packet.put_byte(-1);

    packet.put_byte(static_cast<unsigned char>(mobOut.m_flag34));
    if (mobOut.m_flag34 != 0)
    {
        // 终点坐标（m_endX/m_endY，check_end_point 同源）。
        packet.put_byte(m_endX);
        packet.put_byte(m_endY);
    }
    packet.finalize(true);
}