// [b28d-lane2] verdict=DIFF ndiff=179（ORIG 0x084db878）：槽序/帧差(N82/P212/P221)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=226（ORIG 0x84db878）：N82/P112 帧槽平移+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→-1（236 vs 237，TU 121/1331/5/133 不回退）：真修四处——
//   ① do-while+return 改 for + 循环后 `if(m_b!=0)` 广播（复现 ORIG 底部条件 `seta;jne` 与 fall-through 广播块）；
//   ② 删 itemIdx 具名局部改实参内联两次（P0）；③ cMyTrace 全表达式临时链（P221/P227）；
//   ④ IsAvatarItem 极性翻正 `if(IsAvatarItem()!=0) goto next_item;`（消 ORIG 无的 `xor $1`）。
//   残余=N4/P30 add-vs-lea（ORIG `mov 0x13(%eax,%ecx,1)` scaled disp vs 4.4 常量先加链）。
int Inter_ItemLimitEdition_Sell_Start::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_Sell_StartPktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_Sell_StartPktView*>(data);
    if (pPkt->m_flag != 0)
    {
        if (!G_CDataManager()->getItemLimitEditionMgr()->empty())
        {   // 广播移除旧限购信息
            PacketGuard packet;
            packet.put_header(0, 0xe1);
            packet.put_byte(1);
            G_CDataManager()->getItemLimitEditionMgr()->makeItemLimitEditionRemoveInfo(packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        G_CDataManager()->getItemLimitEditionMgr()->clear();
    }

    for (unsigned int i = 0; i < pPkt->m_b; ++i)
    {
        // [K3 2026-09-11] P0：ORIG 8 组访问各发 `mov %edx,%eax; shl $0x3,%eax; add %edx,%eax;
        //   shl $0x3,%eax`（= i*0x48 逐处重算，共 4 条/处），无 offset 具名槽 ⇒ 去槽内联
        //   （旧稿具名槽每处仅 `mov -0x20(%ebp),%eax` 1 条，正是 gap +24 的来源）。
        CItem* found = G_CDataManager()->find_item(
            *reinterpret_cast<int*>(data + i * 0x48 + 0x13));
        if (found == 0)
        {
            if (*reinterpret_cast<int*>(data + i * 0x48 + 0x13) == 1)   // cmp $1 je（P15）
            {
                goto regist;
            }
        }
        else
        {
            if (found->IsAvatarItem() != 0)            // ORIG vptr+0x10 槽（P5/P62）正极性 `test; jne`
                goto next_item;
regist:
            G_CDataManager()->getItemLimitEditionMgr()->registItem(
                *reinterpret_cast<stItemLimitEditionItemInfo_t*>(data + i * 0x48 + 0xf));
            cMyTrace(__PRETTY_FUNCTION__, 0x4638, 0)(
                "ipg: %d, itemIdx: %d, totalCnt: %d/%d, sellEndTime: %d",
                *reinterpret_cast<int*>(data + i * 0x48 + 0xf),
                *reinterpret_cast<int*>(data + i * 0x48 + 0x13),
                *reinterpret_cast<unsigned int*>(data + i * 0x48 + 0x27),
                *reinterpret_cast<unsigned int*>(data + i * 0x48 + 0x2b),
                *reinterpret_cast<unsigned int*>(data + i * 0x48 + 0x37));
        }
next_item:;
    }

    if (pPkt->m_b != 0)
    {   // 广播新限购信息
        PacketGuard packet;
        packet.put_header(0, 0xe1);
        packet.put_byte(0);
        G_CDataManager()->getItemLimitEditionMgr()->makeItemLimitEditionInfo(packet);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
    }
    return 0;
}