// [b28d-22 N82] ORIG 0x086cbc85：p167=M0/E1:_ZN9__gnu_cxxneIPKSt4pairIiiEPS2_St6vectorIS2_SaIS2_EEEEbRKNS_17__normal_iteratorIT_T1_EERKNS9_IT0_SB_EE，gap=-21；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086cbc85：N82 封顶——帧/槽差 + 迭代器族（p167 M0/E1，gap -21）。
// [b28d-PoolCUOO2 P326 重试 2026-09-09] 旧形（`it != delList.end()`）：453 条，p167
// M0(转换ctor,同型ne)/E1(混型ne)，ndiff=452。新形（end 侧显式
// `const_iterator(delList.end())` 右值临时）：**p167 CLEAR**（45/45 calls 全等），
// 458 条（gap 21→16，逐轮与 ORIG 相同的转换 ctor+同型 ne 结构）；ndiff 读数 594 系
// difflib 错位窗口失真（§6.2：本函数级差集 ORIG-only 已无 call 族）。采新形。
int deleteSpecificItems(CUser* user,
                        const std::vector<std::pair<int, int> >* list)
{
    if (user->get_state() <= 2)
        return 1;

    std::vector<std::pair<int, int> > delList;
    user->deleteSpecificItem(*list, delList);

    // [P171/P273] ORIG 0x086cbce8 `movl $0x1d,-0x34(%ebp)`（写后未读死局部，
    // 位于 deleteSpecificItem 之后、guards 数组构造之前）⇒ 恢复原形声明。
    int n29 = 0x1d;
    (void)n29;
    PacketGuard guards[0x1d];
    int index[0x1d] = { 0 };
    int count[0x1d] = { 0 };

    for (int i = 0; i < 0x1d; ++i)
    {
        guards[i].put_header(0, 0xe);
        guards[i].put_byte(i);
        index[i] = guards[i].get_index();
        guards[i].put_short(0);
    }

    // [b28d P326 重试] end 侧不缓存、比较点显式 const_iterator 转换临时
    // （ORIG 0x086cbc85 p167：转换 ctor MISSING×1 + 同型 ne MISSING×1 + 混型 ne EXTRA×1）。
    for (std::vector<std::pair<int, int> >::const_iterator it =
             delList.begin();
         it != std::vector<std::pair<int, int> >::const_iterator(delList.end());
         ++it)
    {
        int space = it->first;
        int slot  = it->second;

        if (space == 2)
        {
            user->getCurCharacCargoR()->MakeItemPacket(&guards[2], slot);
            count[2] += 1;
        }
        else if (space == 0xc)
        {
            if (user->IsExistAccountCargo())
            {
                user->GetAccountCargo()->MakeItemPacket(&guards[0xc], slot);
                count[0xc] += 1;
            }
        }
        else
        {
            int invenType = 1;
            switch (space)
            {
            case 3: invenType = 0; break;
            case 7: invenType = 3; break;
            case 1: invenType = 2; break;
            default: break;
            }

            Inven_Item item =
                user->getCurCharacInvenR()->GetInvenSlot(invenType, slot);
            if (item.m_addInfo == 0)
            {
                guards[space].put_short(slot);
                guards[space].put_int(-1);
                guards[space].put_int(0);
                guards[space].put_byte(0);
                guards[space].put_short(0);
                guards[space].put_byte(0);
                guards[space].put_int(0);
                guards[space].put_byte(0);
                guards[space].put_short(0);
                guards[space].put_byte(0);
                guards[space].put_packet(g_emptySlot);
            }
            else
            {
                user->getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)invenType, slot, guards[space]);
            }
            count[space] += 1;
        }
    }

    for (int i = 0; i < 0x1d; ++i)
    {
        if (count[i] != 0)
        {
            guards[i].put_short(index[i], count[i]);
            guards[i].finalize(true);
            user->Send(guards[i]);
        }
    }

    return 1;
}