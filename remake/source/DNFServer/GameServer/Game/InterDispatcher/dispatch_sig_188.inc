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
        CItem* found = G_CDataManager()->find_item(
            *reinterpret_cast<int*>(data + i * 0x48 + 0x13));
        if (found == 0)
        {
            if (*reinterpret_cast<int*>(data + i * 0x48 + 0x13) == 1)  // cmp $1 je（P15）
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
