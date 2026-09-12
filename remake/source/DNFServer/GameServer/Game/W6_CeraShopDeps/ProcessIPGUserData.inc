// ---------------------------------------------------------------------------
// ProcessIPGUserData —— ORIG T 0x8321d2e (0x1ac7)
// 【§6.2 验收标注】orig=1564 our≈1634；P167 调用点差集归零（含本批补全：
// _ProcessIPGError 双调用点、creature 双 expiration/usable、get_acc_id 现场、
// ChildrensDay period 双调用、GetRepeatEvent 双调用、make_pair<ulong&,uchar&>
// 转换形、const_iterator 显式转换对、m_itemList 成员直读、opt push_back 直引）。
// we-bigger 残余为 4.4 EH/cleanup 物化与 N82 槽号形态。
// IPG 结果回包处理：resData->status(+0x1c) 4=回填 cera、0=状态包、2=业务结算
// （avatar/coin/item/item-special/creature/additional 六路分派）→ 成功路径
// 扣款与响应包 0x43/0x35/0xe 广播 + ChildrensDay 补发 + Bonus 概率奖励。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N82 封顶（orig 1564 / our 1627，
// gap=63=4.0%；p167 CLEAR 192/192）。本轮修法：删 bSend 标志局部（ORIG 无该槽：
// type==4/0 处理完直落尾部 send 块 08321ea3/08321f0d jmp 08323791，type==2 由
// success 的 bool 归一化决定 08323755，user==0 早返 08321d5b je 083237dc）、
// P235 具名 m_type 消 add+mov。残余：帧 0x31c vs ORIG 0x2cc + 全帧槽号平移（N82，
// difflib 大面积窗口错位）+ 4.4 EH cleanup 物化（N91/N104 族）⇒ §6.2 三件套判
// 语义完成、byte 层 N 封顶。
// [N1A 2026-09-11 语义轴复核 · 无新旋钮 ⇒ 维持封顶]
// ①结构偏移轴：`0xNN(%eax/%ebx/%ecx/%edx/%esi/%edi)` 立即数差集 = ORIG-only
//   {0x1c:1, 0x8:1}、OURS-only {} ⇒ 无字段错位/布局轴 bug（对比 PBR 的
//   `[o+0x16]` 真 bug 即由此轴暴露：当时差集为 0xb0 级）；
// ②常量轴：立即数差集仅 {ORIG 0x2cc} vs {OURS 0x31c}（帧 −0x50）⇒ 无常量错位；
// ③残余 = 帧/槽整体平移（N82，我方最深 -0x2b0 vs ORIG -0x264）+ 4.4 临时
//   spill 深度（如 -0x2b0/-0x2ac 跨调用保活）+ 实参装载序 ⇒ 源形不可控。
void CCeraShop::ProcessIPGUserData(CUser* user, IPG::SIPGData* reqData,
                                   IPG::SIPGData* resData, Stream* stream)
{
    PacketGuard outPkt;                                   // local_d8
    // [b28d-S278] ORIG 无 bSend 标志局部：type==4/0 路径处理完直接落尾部 send 块
    // （08321ea3/08321f0d jmp 08323791），type==2 由 success 的 bool 归一化决定
    // （08323755 test/normalize），user==0 早返（08321d5b je 083237dc）⇒ 删标志槽。

    if (resData->m_type == 4)
    {
        if (user == 0)
        {
            return;
        }
        const int total =
            AtoiAt(resData, 0x99) + AtoiAt(resData, 0xa5) + AtoiAt(resData, 0x81) +
            AtoiAt(resData, 0x75) + AtoiAt(resData, 0x8d) + AtoiAt(resData, 0x69);
        user->SetCera(static_cast<unsigned int>(total));

        outPkt.put_header( 0, 0x35);
        outPkt.put_byte( 1);
        outPkt.put_int( user->GetCera());
    }

    if (resData->m_type == 0)
    {
        if (atoi(BufAt(resData, 0x26e)) == 1)
        {
            int a = 0;
            int b = 0;
            *stream >> a;
            *stream >> b;
            if (user != 0)
            {
                GlobalData::s_pIPGHelper->IPGQuery(user);   // ORIG vtable slot11
            }
            return;
        }
        outPkt.put_header( 0, 0x35);
        outPkt.put_byte( 0);
        outPkt.put_int( 0);
    }
    else if (resData->m_type == 2)
    {
        if (user->getCurCharacR() == 0)
        {
            return;
        }

        stCeraShopIPGRequest_t* q = stream->GetOutBuffer<stCeraShopIPGRequest_t>();
        const int groupIdx = q->m_groupIdx;              // +0x04
        const int seq = q->m_seq;                        // +0x24
        const int itemCountField = q->m_itemCount;       // +0x20
        const int featuredIdxReq = q->m_featuredIdx;     // +0x28
        const unsigned int productNo = q->m_productNo;   // +0x00
        // +0x08 起 8 字节镜像装载：低 32 位 m_itemIdx，bit32..47 为 m_stackCount
        // （ORIG C 整读 ulong；其后消费仅用低 32 位物品号）。
        const unsigned long long itemIdx64 =
            static_cast<unsigned long long>(
                static_cast<unsigned int>(q->m_itemIdx)) |
            (static_cast<unsigned long long>(q->m_stackCount) << 32);
        const unsigned short stackCount = q->m_stackCount;   // +0x0c
        const unsigned char avatarSelB = q->m_avatarType0;   // +0x2c
        const unsigned char avatarSelA = q->m_avatarType1;   // +0x2d
        const unsigned int ceraPriceReq = q->m_ceraPrice;    // +0x10
        const unsigned int winPointReq = q->m_winPoint;      // +0x14
        const int goldSpend = static_cast<int>(q->m_goldPrice);  // +0x18
        const long curSec = static_cast<long>(q->m_curSec);  // +0x30
        std::vector<Inven_Item> resultItems;              // local_ec（RAII）
        const unsigned char buyFlag = q->m_buyFlag;       // +0x34
        const int characNoReq = q->m_characNo;            // +0x38
        const unsigned int accIdReq = q->m_accId;         // +0x40

        stCeraShopItemParam_t itemParam;                  // local_104（RAII）
        for (int s = 0; s < static_cast<int>(q->m_subCount); ++s)
        {
            // ORIG 0x8322086：make_pair(ulong&, uchar&) 左值绑定 → pair<int,int>
            // 转换临时（P61/P135/P178 形）→ push_back。
            unsigned long& subUid = *reinterpret_cast<unsigned long*>(
                &q->m_subEntries[s][0]);
            unsigned char& subVal = q->m_subEntries[s][4];
            itemParam.m_pairs.push_back(
                std::pair<int, int>(std::make_pair(subUid, subVal)));
        }
        for (int o = 0; o < static_cast<int>(q->m_optCount); ++o)
        {
            // ORIG 0x83220xx：push_back 直接引用 req 内元素（无本地拷贝/memcpy）。
            itemParam.m_sel.push_back(*reinterpret_cast<tagPackageItemSelection*>(
                &q->m_optEntries[o][0]));
        }

        int restrictCode = atoi(BufAt(resData, 0x173));   // local_108
        bool success = false;
        const int errArgB = groupIdx;
        if (!(restrictCode == 1 && user != 0 && user->getCurCharacR() != 0))
        {
            // ORIG 0x83221d3/0x832222d：两独立调用点（-0xc → 0x1a，否则 0x19）。
            if (restrictCode == -0xc)
            {
                _ProcessIPGError(user, reqData, resData, '\x1a',
                                 errArgB, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
            }
            else
            {
                _ProcessIPGError(user, reqData, resData, '\x19',
                                 errArgB, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
            }
        }
        else
        {
            int resultState = -1;                         // local_10c → 0
            int slotKind = 2;                             // local_5c
            int insertSlot = -1;                          // local_60
            restrictCode = 0;
            if (groupIdx == 0)
            {
                if (buyFlag == 1)
                {
                    _processAvatarGift(user, static_cast<int>(itemIdx64), featuredIdxReq,
                                       static_cast<char>(avatarSelB),
                                       static_cast<char>(avatarSelA),
                                       BufAt(reqData, 0x7d), accIdReq, characNoReq);
                }
                else
                {
                    CInventory* invW = user->getCurCharacInvenW();
                    insertSlot = invW->AddAvatarItem(
                        static_cast<int>(itemIdx64), featuredIdxReq, false,
                        static_cast<char>(avatarSelB),
                        static_cast<char>(avatarSelA), BufAt(reqData, 0x7d),
                        eAvatarItemAddReason(0), false, 0);
                    if (insertSlot >= 0)
                    {
                        user->SendUpdateItemList(CUser::eSendTarget(1),
                                                 ENUM_ITEMSPACE(1), insertSlot);
                        if (WongWork::CEventMgr::IsChildrensDayEventing())
                        {
                            CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(
                                G_CDataManager()->FindGoods(groupIdx));
                            CItem* found =
                                G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                            // [R9a15] ORIG 0x08322439 直证：无 goods/found 判空，
                            // 直接 `mov -0x48(%ebp),%eax; call GetSticker`（我方旧稿多
                            // 两级短路守卫 + bool 归一岛，属 §14 #33「ORIG 无守卫我方有」）。
                            if (goods->GetSticker() == 0x16)
                            {
                                const int kindValue = found->GetItemType();
                                user->SetChildrensDayEventValue(kindValue, insertSlot,
                                                                featuredIdxReq);
                            }
                        }
                    }
                    else
                    {
                        const char* errMsg = g_scriptStringManager_.findString(
                            4, "game_server_msg_104", 0);
                        CMailBoxHelper::ReqDBSendNewAvatarMail(
                            user, user->getCurCharacNo(),
                            static_cast<int>(itemIdx64), featuredIdxReq,
                            static_cast<char>(avatarSelB),
                            static_cast<char>(avatarSelA), BufAt(reqData, 0x7d),
                            errMsg, static_cast<int>(strlen(errMsg)));
                    }
                }
            }
            else if (groupIdx == 1)
            {
                if (buyFlag == 1)
                {
                    _processCoinGift(user, stackCount, accIdReq, characNoReq);
                }
                else
                {
                    _processCoin(user, stackCount, true);
                }
            }
            else if (groupIdx == 2 || groupIdx == 7 || groupIdx == 9)
            {
                slotKind = 1;
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                Inven_Item item;
                if (src != 0)
                {
                    src->make_item(item);     // ORIG vtable slot2（make_item 填 Inven_Item）
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                }
                if (buyFlag == 1)
                {
                    _processItemGift(user, item, accIdReq, characNoReq);
                }
                else
                {
                    _processItem(user, item, item.m_addInfo2, restrictCode, curSec,
                                 false, true, resultItems, itemParam);
                }
            }
            else if (groupIdx == 4)
            {
                slotKind = 1;
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                if (src == 0)
                {
                    success = false;
                }
                else
                {
                    Inven_Item item;
                    src->make_item(item);
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                    const bool okSpecial = ProcessSpecialItem(
                        user, static_cast<int>(itemIdx64), featuredIdxReq, restrictCode,
                        curSec, false, resultItems, itemParam, '\0', '\0', false, 1);
                    if (!okSpecial)
                    {
                        CInventory* invW = user->getCurCharacInvenW();
                        insertSlot = invW->insertItemIntoInventory(
                            item, eItemAddReason(0x1e), true, false);
                        if (insertSlot == -1)
                        {
                            restrictCode = 4;
                        }
                        else
                        {
                            user->SendUpdateItemList(CUser::eSendTarget(1),
                                                     ENUM_ITEMSPACE(0), insertSlot);
                            user->SaveInventory();
                        }
                    }
                }
            }
            else if (groupIdx == 3)
            {
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                Inven_Item item;
                if (src != 0)
                {
                    src->make_item(item);
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                    if (buyFlag == 1)
                    {
                        // ORIG 0x83229af/0x83229bc：两分支各自重发调用（P48）。
                        const int expirationDate = src->getExpirationDate();
                        const int usablePeriod = src->getUsablePeriod();
                        _processCreatureGift(user, item, accIdReq, characNoReq,
                                             usablePeriod, expirationDate);
                    }
                    else
                    {
                        const int expirationDate = src->getExpirationDate();
                        const int usablePeriod = src->getUsablePeriod();
                        _processCreature(user, item, restrictCode, resultState,
                                         true, usablePeriod, expirationDate);
                    }
                }
            }
            else if (groupIdx == 10)
            {
                _processAdditionalItem(user, itemIdx64, stackCount, avatarSelB,
                                       avatarSelA, resultItems, itemParam, '\0');
                int followGroup = -1;
                CItem* pkg = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                if (pkg != 0 && pkg->isCerashopPackageItem())
                {
                    followGroup = 7;
                }
                if (followGroup != 7)
                {
                    resultItems.clear();
                }

                CDataManager* dm = G_CDataManager();
                CItemLimitEditionMgr* limitMgr = dm->getItemLimitEditionMgr();
                CItemLimitEdition* limit = limitMgr->getItemInfo(productNo);
                if (limit == 0)
                {
                    throw ENUM_ERROR(0x11);   // ORIG __cxa_throw(int 0x11)
                }
                // ORIG：restrictLocal 初值 = getFeaturedIdx()；仅当其==0 且
                // IsSpecialItem(relateditem) 命中时才改取 GetRestrictCode。
                int restrictLocal = limit->getFeaturedIdx();
                if (restrictLocal == 0)
                {
                    int hitVal[2] = { 0, 0 };
                    bool isSpecialFlag = false;
                    if (IsSpecialItem(limit->getRelateditem(), hitVal[1],
                                      hitVal[0], isSpecialFlag))
                    {
                        // ORIG 0x8322d5f `mov 0xc(%eax)` 成员直读（4.1 内联 accessor
                        // 展开形，4.4 产 call ⇒ 用成员直读复刻）。
                        restrictLocal = G_CDataManager()->m_itemList->GetRestrictCode(
                            limit->getRelateditem());
                    }
                }
                if (restrictLocal != 0)
                {
                    SaveFeaturedIdx(user, limit->getFeaturedIdx(),
                                    GlobalData::s_systemTime_.getCurSec(), 0);
                }
            }

            if (restrictCode == 0)
            {
                if (ceraPriceReq != 0)
                {
                    // ORIG 0x8322e4a：get_acc_id(user) 现场调用（非 req+0x40 值）。
                    GlobalData::s_BreakAwaySys->updateUsedCera(
                        user->get_acc_id(), ceraPriceReq);
                }
                if (goldSpend != 0)
                {
                    GetInstanceCirculationStatistic()->AddCirculationStatistic(
                        CIRCULATION_STATISTIC_FIELD(0x26), user,
                        static_cast<unsigned int>(goldSpend));
                }
                user->getCurCharacInvenW()->use_money(goldSpend, eMoneySubReason(8), true);
                user->UseWinPoint(winPointReq, eWPSubReason(2));
                user->UseCera(ceraPriceReq);
                user->addUsedCera(ceraPriceReq);
                if (featuredIdxReq != 0 &&
                    !user->IsRestrictedGoods(featuredIdxReq))
                {
                    user->SetRestrictedGoods(featuredIdxReq);
                    SaveFeaturedIdx(user, featuredIdxReq, static_cast<int>(curSec), 0);
                }

                outPkt.put_header( 1, 0x43);
                outPkt.put_byte( 1);
                outPkt.put_byte( static_cast<int>(
                    static_cast<signed char>(buyFlag)));
                outPkt.put_int( groupIdx);
                outPkt.put_int( static_cast<int>(productNo));
                outPkt.put_int( seq);
                outPkt.put_int( itemCountField);
                outPkt.put_int( resultState);
                outPkt.put_short(
                                              static_cast<int>(resultItems.size()));
                {
                    // ORIG：begin/end 各一次 iterator→const_iterator 转换构造，
                    // while 循环 operator!= 逐轮调用（Ghidra local_11c/local_120）。
                    std::vector<Inven_Item>::const_iterator itBeg(resultItems.begin());
                    std::vector<Inven_Item>::const_iterator itEnd(resultItems.end());
                    while (itBeg != itEnd)
                    {
                        outPkt.put_int(itBeg->m_addInfo);
                        outPkt.put_int(itBeg->m_addInfo2);
                        ++itBeg;
                    }
                }

                unsigned long bonusItem = 0xffffffffUL;     // local_124
                unsigned int bonusCount = 0;                // local_128
                if (buyFlag == 1 && accIdReq != 0)
                {
                    DB_UpdateGiftCount::makeRequest(accIdReq, 1);
                }
                if (seq == itemCountField)
                {
                    if (WongWork::CEventMgr::IsChildrensDayEventing() &&
                        user->IsOKChildrensDayEventCondition())
                    {
                        WongWork::CWebEvent::ReqSaveEventItem(
                            user->get_acc_id(),
                            static_cast<unsigned int>(user->getCurCharacNo()),
                            0x65ae, 1u, 0x30d47, 0u, true);
                        PacketGuard notiPkt;                // local_140
                        notiPkt.put_header( 0, 0xe);
                        notiPkt.put_byte( 1);
                        int idxPos = notiPkt.get_index();
                        int filled = 0;
                        notiPkt.put_short( 10);
                        for (int d = 0; d < 10; ++d)
                        {
                            const int targetSlot =
                                user->GetChildrensDayEventTargetItemSlot(d);
                            if (targetSlot < 0 ||
                                user->GetChildrensDayEventTargetItemPeriod(d) == -1)
                            {
                                break;
                            }
                            // ORIG 0x83232bf：条件后再次现场调用（P48 双调用点）。
                            const int targetPeriod =
                                user->GetChildrensDayEventTargetItemPeriod(d);
                            const Inven_Item bufItem =
                                user->getCurCharacInvenR()->GetInvenSlot(
                                    2, targetSlot);
                            CInventory* invW = user->getCurCharacInvenW();
                            const int newSlot = invW->AddAvatarItem(
                                bufItem.m_addInfo, targetPeriod, false,
                                static_cast<char>(bufItem.m_field1), '\xff',
                                "E_CHILDRENS_DAY", eAvatarItemAddReason(2), false, 0);
                            if (newSlot >= 0)
                            {
                                user->getCurCharacInvenR()->MakeItemPacket(
                                    INVEN_TYPE(2), newSlot, notiPkt);
                                ++filled;
                            }
                        }
                        notiPkt.put_short(idxPos, filled);
                        notiPkt.finalize( true);
                        user->Send(notiPkt);
                    }

                    // ORIG 0x8323408/0x8323439：条件与体内各一次现场调用；
                    // vtable+0x34 直调无判空（P171 照抄；bonus 事件该槽空体）。
                    if (GlobalData::s_event_manager->GetRepeatEvent(0x11)
                            ->IsEventing(0))
                    {
                        CCeraShopBonusItemEvent* bonus =
                            static_cast<CCeraShopBonusItemEvent*>(
                                GlobalData::s_event_manager->GetRepeatEvent(0x11));
                        const int rate = bonus->getBonusRate();
                        if (get_rand_int(1000) <= rate)
                        {
                            const int usedCera = static_cast<int>(user->getUsedCera());
                            G_CDataManager()->m_itemList->getCeraShopBonusItem(
                                usedCera, bonusItem, bonusCount);
                            if (bonusItem != 0xffffffffUL)
                            {
                                std::vector<Inven_Item> bonusItems;    // local_150
                                std::vector<std::pair<int,int> > bonusPairs; // local_15c
                                _processAdditionalItem(user, bonusItem,
                                                       static_cast<unsigned short>(bonusCount),
                                                       '\0', '\xff', bonusItems,
                                                       itemParam, '\0');
                            }
                        }
                    }
                    outPkt.put_int( static_cast<int>(bonusItem));
                    outPkt.put_int( static_cast<int>(bonusCount));

                    PacketGuard ceraPkt;                    // local_134
                    ceraPkt.put_header( 0, 0x35);
                    ceraPkt.put_byte( 1);
                    ceraPkt.put_int( user->GetCera());
                    ceraPkt.finalize( true);
                    user->Send(ceraPkt);

                    ceraPkt.clear();
                    ceraPkt.put_header( 0, 0xe);
                    ceraPkt.put_byte( 0);
                    ceraPkt.put_short( 2);
                    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE(1), 0, ceraPkt);
                    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE(1), 2, ceraPkt);
                    ceraPkt.finalize( true);
                    user->Send(ceraPkt);
                }
                success = true;
            }
            else
            {
                _ProcessIPGError(user, reqData, resData,
                                 static_cast<unsigned char>(restrictCode),
                                 groupIdx, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
                cMyTrace t(
                    __PRETTY_FUNCTION__,
                    0x955, 0);
                t("[%s][%d]",
                  "void WongWork::CCeraShop::ProcessIPGUserData(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)",
                  0x955);
                success = false;
            }
        }

        if (!success)
        {
            return;
        }
    }

    outPkt.finalize( true);
    user->Send(outPkt);
}