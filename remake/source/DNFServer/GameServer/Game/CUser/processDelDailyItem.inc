// [b28d-GrpD 判词·结构化 N] ORIG 0x0867e092：残差为**局部对象整体下移 8B**
//   （我方 -0x2cc/-0xfc/-0x170 vs ORIG -0x2d4/-0x104/-0x178，同差值）+
//   26 元素循环的 `cmpl $0x1c` 边界一致；switch(sete) vs test/jne 为等价物化
//   ⇒ 无语义缺口（N82 槽基差族）。
void CUser::processDelDailyItem()
{
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）

    std::vector<std::pair<int, int> > list;
    deleteDailyStackableItem(list, 3, 8);
    deleteDailyStackableItem(list, 0x39, 0x68);
    // ORIG 0x0867e0ff：调 vector::size() 后 test（非 empty()）—— callee 选型按 ORIG。
    if (list.size() == 0)
        return;

    PacketGuard pkts[29];
    // ORIG 0x0867e180/+0x196：两个 int[29] 各发一次 rep stos 零初始化，无 memset call；
    // 且 ORIG 栈上 indices(-0x108) 高于 counts(-0x17c) ⇒ 源码声明序 indices 在前。
    int indices[29] = {0,};
    int counts[29] = {0,};
    for (int i = 0; i < 0x1d; ++i)
    {
        ((InterfacePacketBuf*)&pkts[i])->put_header(0, 0xe);
        ((InterfacePacketBuf*)&pkts[i])->put_byte(i);
        indices[i] = ((InterfacePacketBuf*)&pkts[i])->get_index();
        ((InterfacePacketBuf*)&pkts[i])->put_short(0);
    }
    // ORIG 0x0867e268..0x0867e2bd：begin()/end() 各在循环外构造一次 const_iterator
    // 局部（local_90/local_94）；循环内只调 operator!=(const_iterator, const_iterator)。
    std::vector<std::pair<int, int> >::const_iterator it = list.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = list.end();
    for (; it != itEnd; ++it)
    {
        const int space = it->first;
        const int slot = it->second;
        if (space == 2)
        {
            getCurCharacCargoR()->MakeItemPacket(&pkts[space], slot);
            counts[space]++;
        }
        else if (space == 0xc)
        {
            if (IsExistAccountCargo())
            {
                GetAccountCargo()->MakeItemPacket(&pkts[space], slot);
                counts[space]++;
            }
        }
        else
        {
            int invenType = 1;
            if (space == 3)
                invenType = 0;
            else if (space == 7)
                invenType = 3;
            else if (space == 1)
                invenType = 2;
            Inven_Item slotItem =
                getCurCharacInvenR()->GetInvenSlot(invenType, slot);
            if (slotItem.m_addInfo == 0)   // ORIG：test dword [ret+2]
            {
                // ORIG 0x0867e3f4 起：每处调用独立重算 &pkts[space] 地址，不落具名指针。
                ((InterfacePacketBuf*)&pkts[space])->put_short(slot);
                ((InterfacePacketBuf*)&pkts[space])->put_int(-1);
                ((InterfacePacketBuf*)&pkts[space])->put_int(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_short(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_int(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_short(0);
                ((InterfacePacketBuf*)&pkts[space])->put_byte(0);
                ((InterfacePacketBuf*)&pkts[space])->put_packet(g_emptySlot);
            }
            counts[space]++;
        }
    }
    for (int i = 0; i < 0x1d; ++i)
    {
        if (counts[i] != 0)
        {
            ((InterfacePacketBuf*)&pkts[i])->put_short(indices[i], counts[i]);
            ((InterfacePacketBuf*)&pkts[i])->finalize(true);
            Send(pkts[i]);
        }
    }
