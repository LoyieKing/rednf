bool CDungeon_TowerOfDespair::_do_after_dungeon_start(CUser* user) const  // [b28d-Main 代落·S326 P292] int→bool
{
    // [b28d-solo-CDungeon2 判词 · IDENTICAL_AE] ORIG 0x0834d8ec / our 254 = ORIG 254；
    // p167 43=43 CLEAR。历史修法：①cMyTrace 全表达式临时链
    //   `cMyTrace(f,l,v)("...", user->getCharacNo(), slot, m_itemCount)`（P227 翻 N80）；
    //   ②proxy 链式内联实参（P0/P52）。本轮修法：
    //   ③`TOD_Layer todayLayer = ...; todayLayer.GetLayer();` 改全表达式链
    //     `todState->getTodayEnterLayer().GetLayer()`（P278/P0：ORIG 无 todayLayer 具名槽，
    //     sret 临时直接落 -0x16）⇒ gts 槽 -0x3b→-0x3f、sret -0x3e→-0x16、trace -0x24→-0x28
    //     三处一次对齐；
    //   ④TowerOfDespairStatistic_Key 写入口由 m_0(+0) 改 m_4(+4)（语义更正：
    //     m_4@+4 = packet+0xe；证据：ORIG ctor 0x08374cba `add $0xa` 后 call Key C1、
    //     权威头 shared/packet/include/Packet_TowerOfDespair_Statistic_GTS.h 的
    //     m_padA@+0xa / m_layer@+0xe）⇒ 写入口落 gts+0xe，与 ORIG 一致。
    //     成员名保持与 TowerOfDespairStatistic_Key.h 逐字一致（ODR）。
    // 结果：TU 口径 near 1→0、ae +1（本件 AE）。
    // ORIG 0x0834d8ec：进入绝望之塔——递增计数、二次进入扣门票、发包回执、统计。
    // 全调用点成立（GetCharacExpandData/IncreaseEnterCount/getEnterCount/check_item_exist/
    // GetInvenSlot/delete_item/cMyTrace×2/PacketGuard 族/getTodayEnterLayer/
    // GetServerProxy/SendPacket）。语义与 Ghidra C 逐条对应。
    if (user == 0)
        return false;

    TOD_UserState* todState =
        (TOD_UserState*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd);
    todState->IncreaseEnterCount();
    if (todState->getEnterCount() > 1)
    {
        if (m_entryRequiredItem.m_enableFlag != 0)
        {
        // 二次及以上进入需持有门票物品（ORIG this+0x7f4/+0x7f8/+0x7fc）。
        int slot = user->getCurCharacInvenR()->check_item_exist(
            m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
            return false;
        Inven_Item item =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo2 < m_entryRequiredItem.m_itemCount)
            return false;

        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, m_entryRequiredItem.m_itemCount,
                (eItemDelReason)0xe, true))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x96f, 5)(
                "CDungeon_TowerOfDespair::_do_after_dungeon_start, delete_item failed , User ch =%d , %d %d",
                user->getCurCharacNo(), slot, m_entryRequiredItem.m_itemCount);
            return false;
        }

        PacketGuard pg;
        pg.put_header(0, 0xe);
        pg.put_byte(0);
        pg.put_short(1);
        pg.put_short(slot);
        int rest = item.m_addInfo2 - m_entryRequiredItem.m_itemCount;
        if (rest == 0)
            pg.put_int(-1);
        else
            pg.put_int(item.m_addInfo);   // ORIG local_8b（item idx 字段）
        pg.put_int(rest);
        pg.put_byte(item.GetItemAttr());
        pg.put_short(item.m_fieldb);
        pg.put_byte(item.m_field0);
        pg.put_int(item.m_fieldd);
        pg.put_byte(item.m_amp.getAbilityType());
        pg.put_short(item.m_amp.getAbilityValue());
        pg.put_byte(0);
        pg.put_packet(item);
        pg.finalize(true);
        user->Send(pg);
        user->SaveInventory();
        }
    }

    Packet_TowerOfDespair_Statistic_GTS gts;
    gts.m_key.m_4 = todState->getTodayEnterLayer().GetLayer();
    gts.m_flag = 1;
    gts.m_accId = user->get_acc_id();

    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)->SendPacket(
        (char*)&gts, sizeof(Packet_TowerOfDespair_Statistic_GTS));
    return true;
}