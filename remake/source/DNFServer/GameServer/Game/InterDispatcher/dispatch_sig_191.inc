// §6.2 巨件核销：P167 106/106 [0缺0多]；gap -39→-39；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。batch-20「Inter 巨兽」已核销，本次维持。
// [b28d-lane2] verdict=DIFF ndiff=716（ORIG 0x084dbef0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-41。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=527（ORIG 0x84dbef0）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
int Inter_ItemLimitEdition_Reply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_ReplyPktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_ReplyPktView*>(data);
    if (user->get_state() < (ch_state)3)              // setle 界：state<3 → return 0
        return 0;

    // [K3 2026-09-11] P0/P183：ORIG 084d.be 对 msg+0x0e/0x12 无缓存、跨调用寄存器保活
    //   （`mov 0x12(%eax),%esi; mov 0xe(%eax),%ebx`），头部只有 2 个零初始化 ⇒ 删两具名槽。
    int specialRestrictCode = 0;
    CItemLimitEdition* edition = 0;                    // 前后半段共享（try 内赋值）
    CItem* ci = 0;
    try
    {
        // ORIG 四处独立 G_CDataManager：updateItem / getItemInfo / find_item /
        // GetRestrictCode。
        if (!G_CDataManager()->getItemLimitEditionMgr()->updateItem(
                pPkt->m_e, pPkt->m_12))
            throw static_cast<ENUM_ERROR>(0x16);
        const unsigned char buyType = pPkt->m_16;
        if (buyType == '\x01')
            throw static_cast<ENUM_ERROR>(0x16);
        if (buyType == '\x02')
            throw static_cast<ENUM_ERROR>(0x5f);
        if (buyType != '\0')
            throw static_cast<ENUM_ERROR>(1);

        edition = G_CDataManager()->getItemLimitEditionMgr()->getItemInfo(pPkt->m_e);
        if (edition == 0)
            throw static_cast<ENUM_ERROR>(0x11);

        // 售罄后广播信息包（头 0xe1）
        if (edition->isSellComplete())
        {
            PacketGuard packet;
            packet.put_header(0, 0xe1);
            packet.put_byte(0);
            packet.put_byte(1);
            edition->makeInfoPacket(packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }

        if ((unsigned int)user->GetCera() <
            (unsigned int)edition->getPriceCera())
            throw static_cast<ENUM_ERROR>(0xb);
        if (user->getCurCharacInvenR()->get_money() < edition->getPriceGold())
            throw static_cast<ENUM_ERROR>(0xa);
        if ((unsigned int)user->GetWinPoint() <
            (unsigned int)edition->getPriceWinPoint())
            throw static_cast<ENUM_ERROR>(0xc);

        // ORIG 6 处独立 `_ZNK17CItemLimitEdition14getRelateditemEv`（无缓存）。
        if (edition->getRelateditem() != 1)
        {
            ci = G_CDataManager()->find_item(edition->getRelateditem());
            if (ci == 0)
                throw static_cast<ENUM_ERROR>(0x11);
        }
    }
    catch (const ENUM_ERROR& e)   // ORIG try1 @ 084dbf3b→084dc2c4，catch @ 084dc2c7
    {   // 前半段所有 ENUM_ERROR 抛出点收敛于此（SendCmdErrorPacket 0x100）。
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x100, (unsigned char)e & 0xff);
        return 0;
    }
    {   // 后半段（ORIG try3 @ 084dc3da，不 throw；guard/packet 异常走清理 resume）：

        bool specialFlag = false;
        int specialValue = 0;
        int specialType = 0;
        int featuredIdx = edition->getFeaturedIdx();
        bool isSpecial = false;
        if (featuredIdx == 0)
        {
            isSpecial = GlobalData::s_pCeraShop->IsSpecialItem(
                edition->getRelateditem(), specialValue, specialType, specialFlag);
        }
        if (isSpecial)
        {   // 特别物品借用 restrict code 作为 featured 字段。ORIG 直读
            // CDataManager+0xc（m_itemList）成员再 GetRestrictCode（无 getItemList 调用）。
            specialRestrictCode =
                G_CDataManager()->m_itemList->GetRestrictCode(
                    edition->getRelateditem());
        }

        user->resetUsedCera();
        Stream* stream = GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x46d9);
        CStreamGuard guard(stream, false);
        stCeraShopIPGRequest_t* reqBuf = guard->GetInBuffer<stCeraShopIPGRequest_t>();
        stCeraShopIPGRequest_t& rq = *reqBuf;

        rq.m_productNo = static_cast<unsigned int>(pPkt->m_e);
        rq.m_groupIdx = 10;                                            // IPG 请求类型
        rq.m_itemIdx = edition->getRelateditem();
        rq.m_stackCount = edition->getRelatedItemCnt();
        rq.m_ceraPrice = static_cast<unsigned int>(edition->getPriceCera());
        rq.m_winPoint = static_cast<unsigned int>(edition->getPriceWinPoint());
        rq.m_goldPrice = static_cast<unsigned int>(edition->getPriceGold());
        rq.m_avatarField = 0;
        rq.m_itemCount = 1;
        rq.m_seq = 1;
        rq.m_featuredIdx = specialRestrictCode;
        rq.m_avatarType0 = pPkt->m_17;
        rq.m_avatarType1 = edition->getAvatarTypeSelect();
        rq.m_curSec = static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
        rq.m_buyFlag = 0;
        rq.m_characNo = user->getCurCharacNo();
        rq.m_accId = static_cast<unsigned int>(user->get_acc_id());
        const unsigned int optionCnt =                                 // msg+0x18
            pPkt->m_18;
        rq.m_subCount = static_cast<unsigned char>(optionCnt);
        for (unsigned int i = 0; i < optionCnt; ++i)                   // stride 8 子项@0x48
        {
            // [Lane7GlobalData · Main 批准跨 TU 迁移 2026-09-10 · 「视图 cast 反模式」]
            // 目的侧改具名数组视角（SigTypes_E.h 匿名 union，sizeof/偏移/mangled 不变）：
            // 旧 `*reinterpret_cast<T*>(&rq.m_subEntries[i][0])=` 目的侧 cast 使 4.4 折
            // add+shl 普通 store；ORIG 为 stride-8 scaled 直存。源侧仍是 5 字节步长
            // 线格式缓冲读，保留 cast（P156 线格式豁免）。
            rq.m_subItems[i].m_itemIdx =
                *reinterpret_cast<int*>(data + 0x19 + i * 5);   // idx 槽 +0x00
            rq.m_subItems[i].m_value = *(data + 0x1d + i * 5);  // val 槽 +0x04
        }
        const unsigned int avatarCnt =                                 // msg+200(0xc8)
            pPkt->m_c8;
        rq.m_optCount = static_cast<unsigned char>(avatarCnt);
        for (unsigned int j = 0; j < avatarCnt; ++j)                   // stride 8 选项@0x164
        {
            // 同上（ORIG stride-8 scaled 直存）：目的侧改具名视角 m_optItems。
            rq.m_optItems[j].m_itemUid =
                static_cast<unsigned int>(*reinterpret_cast<int*>(data + 9 + j * 8));
            rq.m_optItems[j].m_a =
                *reinterpret_cast<unsigned short*>(data + 0xd + j * 8);
            rq.m_optItems[j].m_b =
                *reinterpret_cast<unsigned short*>(data + 0xf + j * 8);
        }

        char priceText[36] = { '\0' };                                 // ORIG memset(0x24)
        char uidText[21]   = { '\0' };                                 // -0x75..-0x59 区清零
        char itemNameText[0x62];
        OS_API::snprintf(priceText, sizeof(priceText), "%d",
                         edition->getPriceCera());
        OS_API::snprintf(uidText, 8, "%d", pPkt->m_e);

        const unsigned short relatedCnt = rq.m_stackCount;   // +0x0c
        if (edition->getRelateditem() == 1)
        {
            // [H2·2026-09-11] §6.5 语义轴：ORIG .rodata 实字节 `\xc4\xda\xc0\xce %d\xb0\xb3`（EUC-KR）；
            //   旧写 UTF-8 韩文 ⇒ 玩家人名/物品名显示串发射字节不同。
            OS_API::snprintf(itemNameText, 0x62, "\xc4\xda\xc0\xce %d\xb0\xb3", (unsigned int)relatedCnt);
        }
        else if (ci->is_stackable() && relatedCnt >= 2)
        {
            // [H2·2026-09-11] §6.5：ORIG 实字节 `%s %d\xb0\xb3`（EUC-KR）。
            OS_API::snprintf(itemNameText, 0x62, "%s %d\xb0\xb3",
                             ci->GetItemName(), (unsigned int)relatedCnt);
        }
        else
        {
            strcpy(itemNameText, ci->GetItemName());
        }

        char uniqueNo[0x15] = { '\0' };
        if (edition->getPriceCera() == 0)
        {   // vtable+0x18 = slot6 IPGOutputSimulation。ORIG：GenUniqueNo 静态
            // 直调（无构造），返回值作 IPG 实参。
            GlobalData::s_pIPGHelper->IPGOutputSimulation(
                user, uidText, priceText,
                WongWork::CGenUniqueNo::GenUniqueNo(uniqueNo), itemNameText,
                user->getCurCharacName(), guard.Get());
        }
        else
        {   // vtable+0x14 = slot5 IPGOutput
            GlobalData::s_pIPGHelper->IPGOutput(
                user, uidText, priceText,
                WongWork::CGenUniqueNo::GenUniqueNo(uniqueNo), itemNameText,
                user->getCurCharacName(), guard.Get());
        }

        PacketGuard packet;
        packet.put_header(1, 0x100);
        packet.put_byte(1);
        packet.put_int(edition->getIPGNO());
        packet.put_int(edition->getCurSellNum());
        packet.put_int(edition->getTotalSellNum());
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}