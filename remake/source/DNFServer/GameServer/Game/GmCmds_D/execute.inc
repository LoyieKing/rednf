void CItemInfoCmd::execute()
{
    struct InfoBuf { char m_b[0xff]; };                      // -0x157（ORIG char info[0xff]）
    InfoBuf info;
    CItem* item = G_CDataManager()->find_item(m_str0.c_str());
    if (item == 0)
    {
        info = InfoBuf();                                    // ORIG 分支内 memset 慢路
        sprintf(info.m_b, "cannot find item (%s) ", m_str0.c_str());
        PacketGuard packet;                                  // -0x50
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(static_cast<int>(strlen(info.m_b)));
        packet.put_str(info.m_b, static_cast<int>(strlen(info.m_b)));
        packet.finalize(true);
        GetUser()->Send(packet);
        return;
    }

    info = InfoBuf();                                        // ORIG else 分支 memset 慢路
    sprintf(info.m_b, "item_index(%d) ", item->get_index()); // ORIG get_index 0x8110c48
    {
        PacketGuard packet;                                  // -0x58（ORIG 析构在 else 块中段）
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(static_cast<int>(strlen(info.m_b)));
        packet.put_str(info.m_b, static_cast<int>(strlen(info.m_b)));
        packet.finalize(true);
        GetUser()->Send(packet);
    }

    const int itemIdx = item->get_index();                   // local_2c
    CUser* user = GetUser();                                 // local_28
    int count = 100;                                         // local_20
    // ORIG 0x084aa642/0x084aa657：getStackableLimit 两次独立调用（比较+赋值，P48 无 CSE）；
    // 嵌套 if 形（Ghidra C 同构）⇒ 末项发 ORIG 的 `cmp mem,%eax; setl %al; test; je`。
    if (item != 0)
    {
        if (item->is_stackable())
        {
            if (static_cast<CStackableItem*>(item)->getStackableLimit() < count)
            {
                count = static_cast<CStackableItem*>(item)->getStackableLimit();
            }
        }
    }
    ENUM_ITEMSPACE space;                                    // local_44（AddItem 出参）
    const int added = user->AddItem(itemIdx, count,
                                    static_cast<eItemAddReason>(6), space, 0);
    // ORIG `js 084aa72e(nop); body; jmp 084aa72f` = 4.1 if-join 尾岛 ⇒ P223 负条件
    // goto 到共享尾空标签（4.4 对正向 if 恒 jump-thread，给不出 nop 岛）。
    if (added < 0) goto trace_end;
    // P227 全表达式临时链：实参在 ctor 前求值并以 callee-saved 跨 ctor 保活
    // （ORIG getCurCharacName→%esi、NumberToString→%ebx，无栈槽往返）。
    cMyTrace(__PRETTY_FUNCTION__, 0x539, 0)(
        "%s:'%s' GM Mode Add Item(%d).",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacName(), added);
    user->SendUpdateItemList(CUser::eSendTarget_1, space, added);
trace_end:;
