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
// [R19a15 2026-09-13 ④ 修复 ×2 · 我方多出 find_item 判空守卫]
//   ①groupIdx∈{2,7,9}：ORIG 0x08322537..0x08322551 直证「Inven_Item::C1Ev 之后
//     `mov -0x40(%ebp),%eax; mov (%eax),%eax; add $0x8,%eax; mov (%eax),%edx;
//     call *%edx`」——无任何 test/je；我方旧稿 `if (src != 0){…}` 多出
//     `cmpl $0x0,-0x78(%ebp); je`（跳过的正是 ORIG 顺序执行的 make_item +
//     m_addInfo/m_fieldb 写块）。
//   ②groupIdx==3：ORIG 0x0832295a..0x08322971 同形（find_item → C1Ev → 直接
//     vtable slot2 调用）；我方旧稿 `if (src != 0){…}` 多出 `cmpl $0x0,-0x68; je`。
//   处置：两处守卫移除，完全复刻 ORIG（src==0 时按 ORIG 行为直接解引用）。
//   判据：§14 #33（ORIG 无守卫、我方有）+ §5 #17（我方多出的防御性守卫）。
//   四闸门：①备份 md5 44304e99a03274b749f2a8c3ad56ca73；②A/B TU 四数
//   identical=61 ae=120 near=0 diff=2 missing=45 逐值不变（.o 指令 1711→1707
//   = −2×2）；③新 .o 两处与 ORIG 逐条同形（无 test/je）；④nm --defined-only
//   317=317 逐名零差。
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
        // [R17a15 形态修复·ORIG 直证] ORIG 0x08321fa3/0x08321faf 为两条独立装载
        //   （`mov 0x8(%eax),%eax` int → -0x80 槽；`movzwl 0xc(%eax); mov %ax,-0x7c`
        //   ushort → -0x7c 槽），**无** or/64 位构造；全部消费点只取低 32 位
        //   （唯一 64 位形参 _processAdditionalItem 第 2 参是 `unsigned long`，ORIG
        //   调用点 0x08322bca `mov -0x80(%ebp),%eax; mov %eax,0x8(%esp)` 只传 4 字节）
        //   ⇒ 原码无 64 位局部，删除我方多余的 or 物化。
        const int itemIdx = q->m_itemIdx;                    // +0x08
        const unsigned short stackCount = q->m_stackCount;   // +0x0c
        // [R20a15 §26.1] ORIG 消费形直证：3 处 char 形参调用点读点为单条
        // `movsbl -0x79/-0x7a(%ebp),%r`（0x08322279/0x0832227d、0x083222cf/0x083222d3、
        // 0x0832237e/0x08322382）⇒ 这些点是 signed char 直传；唯一 uchar 形参点
        // （_processAdditionalItem，ORIG 0x08322b90 `movzbl -0x79; movzbl %al,%ecx`）
        // 保持零扩展语义。故局部声明 char + 该点显式 (unsigned char) 转换。
        const char avatarSelB = q->m_avatarType0;            // +0x2c
        const char avatarSelA = q->m_avatarType1;            // +0x2d
        const unsigned int ceraPriceReq = q->m_ceraPrice;    // +0x10
        const unsigned int winPointReq = q->m_winPoint;      // +0x14
        const int goldSpend = static_cast<int>(q->m_goldPrice);  // +0x18
        const long curSec = static_cast<long>(q->m_curSec);  // +0x30
        std::vector<Inven_Item> resultItems;              // local_ec（RAII）
        // [R20a15 §26.1 读位宽/符号性直证] ORIG 0x0832201f 存槽后 5 处读点全为
        // `movsbl -0x65(%ebp),%r`（单条符号扩展）⇒ 原码该局部是 signed char；
        // 我方旧稿 `unsigned char` 使读点为 `movzbl -0xb5(%ebp),%eax; movsbl %al,%eax`
        // （两段，且各调用点多 1~2 条）。改 char 后与 ORIG 同形（cmpb $1 消费点不变）。
        const char buyFlag = q->m_buyFlag;                // +0x34
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
        // [R20a15 §26.3 多余物化移除] ORIG 全函数**无 bool 槽**（movb $0x0 仅 1 处、
        // movb $0x1 0 处；退出路径为 0x08322232 `mov $0x0,%ebx` + `jmp <尾 0x083237dc>`）
        // ⇒ 原码各失败分支直接 `return;`，没有 `success` 具名 bool + 末尾 `if(!success)`。
        // 旧稿 `bool success` 多出 4×movb + movzbl/xor$1 归一化（我方 .o 0x82e0253/
        // 0x82e0913/0x82e18bb/0x82e1976 段）。
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
            return;
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
                    _processAvatarGift(user, itemIdx, featuredIdxReq,
                                       static_cast<char>(avatarSelB),
                                       static_cast<char>(avatarSelA),
                                       BufAt(reqData, 0x7d), accIdReq, characNoReq);
                }
                else
                {
                    CInventory* invW = user->getCurCharacInvenW();
                    insertSlot = invW->AddAvatarItem(
                        itemIdx, featuredIdxReq, false,
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
                                G_CDataManager()->find_item(itemIdx);
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
                            itemIdx, featuredIdxReq,
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
                CItem* src = G_CDataManager()->find_item(itemIdx);
                Inven_Item item;
                // [R19a15 ④] ORIG 0x08322537..0x08322551 直证：find_item 后**无判空**，
                // 直接 `mov -0x40(%ebp),%eax; mov (%eax),%eax; add $0x8,%eax;
                // call *%edx`（vtable slot2 = make_item）。我方旧稿多 `cmpl $0x0,
                // -0x78(%ebp); je` 跳过块（§14 #33「ORIG 无守卫、我方有」+ §5 #17）
                // ⇒ 移除，完全复刻 ORIG（src==0 时空指针解引用，与 ORIG 行为一致）。
                src->make_item(item);     // ORIG vtable slot2（make_item 填 Inven_Item）
                item.m_addInfo = itemIdx;
                item.m_fieldb = stackCount;
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
                CItem* src = G_CDataManager()->find_item(itemIdx);
                if (src == 0)
                {
                    return;   // ORIG 无 success 槽：该失败路径直接退出
                }
                else
                {
                    Inven_Item item;
                    src->make_item(item);
                    item.m_addInfo = itemIdx;
                    item.m_fieldb = stackCount;
                    const bool okSpecial = ProcessSpecialItem(
                        user, itemIdx, featuredIdxReq, restrictCode,
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
                CItem* src = G_CDataManager()->find_item(itemIdx);
                Inven_Item item;
                // [R19a15 ④] ORIG 0x0832295a..0x08322971 直证：find_item 后无判空，
                // 直接 vtable slot2 调用（`mov -0x38(%ebp),%eax; mov (%eax),%eax;
                // add $0x8,%eax; call *%edx`）⇒ 移除我方多出的 `cmpl $0x0; je`
                // 整块守卫（§14 #33 / §5 #17），完全复刻 ORIG。
                src->make_item(item);
                item.m_addInfo = itemIdx;
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
            else if (groupIdx == 10)
            {
                _processAdditionalItem(user, static_cast<unsigned long>(itemIdx), stackCount,
                                       static_cast<unsigned char>(avatarSelB),
                                       static_cast<unsigned char>(avatarSelA), resultItems, itemParam,
                                       '\0');
                int followGroup = -1;
                CItem* pkg = G_CDataManager()->find_item(itemIdx);
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
                // （原 success=true 删除：ORIG 成功路径无标志写）
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
                  __PRETTY_FUNCTION__,
                  0x955);
                return;
            }
        }
    }

    outPkt.finalize( true);
    user->Send(outPkt);
}