int Inter_ItemLimitEdition_Update::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_UpdatePktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_UpdatePktView*>(data);
    // ORIG 0x084dbd2x 入口直接进 PacketGuard ctor（无 count 槽、无 `mov 0xe(%eax),%eax` 预取）
    // ⇒ 原码循环界内联 pPkt->m_e（P0：无槽往返即无具名局部）；旧 `const unsigned int entryCount`
    // 提前物化多 3 条 + 帧 0x50 vs ORIG 0x30。
    PacketGuard packet;
    packet.put_header(0, 0xe1);
    packet.put_byte(0);
    int countIdx = packet.get_index();         // 计数回填位
    int putCount = 0;                                // local_18/asm -0x14(%ebp)
    packet.put_byte(0);

    for (unsigned int i = 0; i < (unsigned int)pPkt->m_e; ++i)   // asm seta 无符号比较（界直读）
    {
        CDataManager* mgrData = G_CDataManager();
        CItemLimitEditionMgr* mgr = mgrData->getItemLimitEditionMgr();
        CItemLimitEdition* info =
            mgr->getItemInfo(*reinterpret_cast<unsigned int*>(data + i * 9 + 0x12));
        if (info == 0 || info->isSellComplete())
            continue;                                // ORIG mov $1/mov $0 归并 true

        // 二旋钮实验留痕：mgrData/mgr/count/itemIdx/mgr2 全内联实测 **our 130（ORIG 138，gap 2→−8）
        // 反劣** ⇒ 原码确有这些具名槽（每点重算反而少 8 条）；维持 P0 保留具名形。
        const unsigned int count =
            *reinterpret_cast<unsigned int*>(data + i * 9 + 0x16);
        const unsigned int itemIdx =
            *reinterpret_cast<unsigned int*>(data + i * 9 + 0x12);
        CItemLimitEditionMgr* mgr2 = G_CDataManager()->getItemLimitEditionMgr();
        // ORIG xor $0x1,%eax; test %al（bool 取反语义）
        if (mgr2->updateItem(itemIdx, count))
        {
            info->makeInfoPacket(packet);
            ++putCount;
        }
    }

    if (putCount != 0)
    {
        packet.put_byte(countIdx, putCount);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
    }
    return 0;
