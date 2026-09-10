int CUser::quest_reward(int questId, int selectIdx, int repeatN)
{
    PacketGuard ack;
    ack.put_header(1, 0x24);
    ack.put_byte(1);
    ack.put_short(questId);

    if (!getCurCharacQuestR()->check_cond_clear(questId)) {
        return 0x16;
    }
    Quest* q = G_CDataManager()->m_questList  // +0x18
               ->find_quest(questId);
    if (q == 0) return 0x15;

    bool gm = IsGameMasterMode();
    if (!gm && !q->isRepeatableQuest()) {
        if (getCurCharacQuestR()->m_cleared.isClearedQuest(questId))
            return 0x16;
    }
    if (!(q->m_type == 4 || repeatN == 1)) return 0x16;

 // 需求材料区（needItems +0x7c）：count==0/25 时按 repeatN 缩放校验与预演
    std::vector<std::pair<int,int> > delCond;                  // -0x100
    std::pair<int,int> pairTmp;

    if (!(q->m_triggerType == 0 || q->m_triggerType == 0x19))
        return 0x11;
    {
        // ORIG 无 __normal_iterator<pair<int,int>const*> 实例，也无迭代器默认构造
        // 调用点：迭代器在 for-init 内由 begin() 直接构造（非 const 重载）。
        for (std::vector<std::pair<int,int> >::iterator di = q->m_triggerDataPairs.begin();
             di != q->m_triggerDataPairs.end(); ++di) {
            // ORIG:经 operator*() 解引用读字段（de 站点），非 operator->()。
            pairTmp.first  = (*di).first;
            pairTmp.second = (*di).second * repeatN;
            delCond.push_back(pairTmp);
        }
        if (!getCurCharacInvenR()->CheckNeedItemList(delCond))
            return 0x11;                                       // ORIG +0x25d
    }

    // [CUserOnlyOrig 2026-09-09 真缺口] ORIG 0x0866ecf9-0x0866ed3d 守卫（Ghidra C 行 208-209
    // `if ((((t == 0) || (b8 == 0)) || (t == 0x19)) && (b8 != 0x19))`）：扣料/发奖预演块
    // 只有该条件成立才执行；旧重建漏此守卫，无条件执行（b8==0x19 等路径多发预演与扣料）。
    if ((q->m_triggerType == 0 || q->m_fieldB8 == 0 ||
         q->m_triggerType == 0x19) && q->m_fieldB8 != 0x19)
    {
        // [CUserOnlyOrig 2026-09-09] ORIG 0x0866ed3d/0x0866ed46 先构造 CInventory(-0x9f8)
        // 再构造 InventoryMemory(-0xcb37) ⇒ 原码声明序 CInventory 在前（旧重建相反）。
        CInventory shadow;                                     // -0x9f8 副本
        InventoryMemory qmem;                                  // -0xcb37（ORIG C1 0x822d3b8）
        shadow.SetInventoryMemory(&qmem);                      // ORIG 0x822d4f6
        shadow.setCopy(*getCurCharacInvenR());
        std::vector<int> swapList;                             // -0x158
        bool okAll = false;

        if (!shadow.tryDeleteEventItems(delCond))              // 扣料预演失败
            return 0x11;                                       // -0x158 两出口同码

        // [CUserOnlyOrig 2026-09-09 真 bug：条件反向] ORIG 0x0866edfa `jne 866f133`
        // （b8 != 0 直接跳到 okAll=true，跳过本块）⇒ 原码是 `if (b8 == 0)`；旧重建写成
        // `!= 0`，使普通任务(b8==0)不插奖励、特殊类型(b8!=0)误插。
        if (q->m_fieldB8 /*+0xb8 计数哨兵*/ == 0) {
            // ORIG 走 vector<RewardItemInfo> 的 const begin()/end()（×3 各），
            // 无 iterator→const_iterator 转换构造、无迭代器默认构造调用点。
            const std::vector<RewardItemInfo>* pool =
                getUserQuestRewardItem(q, this);               // -0x88
            std::vector<std::pair<int,int> > insertList;       // -0x10c
            for (std::vector<RewardItemInfo>::const_iterator ri = pool->begin();
                 ri != pool->end(); ++ri) {
                if (ri->m_job == 0xb ||
                    (ri->m_job == (int)get_charac_job() &&
                     (ri->m_growType == -1 ||
                      ri->m_growType == getCurCharFirstGrowType()))) {
                    pairTmp.first  = ri->m_itemIdx;
                    pairTmp.second = ri->m_count * repeatN;
                    insertList.push_back(pairTmp);
                }
            }
            if (shadow.tryInsertEventItems(insertList) !=
                (int)insertList.size())
                return 4;

            selectIdx = q->ConvertRewardSelectIndex(this, selectIdx);
            if ((int)q->m_selectReward.size() <= selectIdx)
                return 0x15;
            if (selectIdx >= 0 && selectIdx != 0xffff) {
                Inven_Item selItem;                            // -0x205..-0x1c5
                // ORIG:两次 operator[] 分别读 itemIdx/m_count（ix 站点 ×2）。
                selItem.m_addInfo = q->m_selectReward[selectIdx].m_itemIdx;
                G_CDataManager()->m_itemList->create_item(   // ORIG 0x866f069：接收者 = CDataManager+0xc 的 CItemList
                    (ENUM_ITEM_CREATE_TYPE)4, selItem,
                    q->m_selectReward[selectIdx].m_count * repeatN);
                if (shadow.tryInsertItemIntoInventory(selItem) == -1)
                    return 4;
            }
        }
        okAll = true;
    }

    int expGain = 0, goldGain = 0, qpGain = 0, pieceGain = 0;  // -0x118..-0x124
    quest_basic_reward(q, expGain, goldGain, qpGain, pieceGain, false);
    if (expGain != 0) {
        int curLevelExp = 0, newExp = 0;                       // -0x15c/-0x160
        set_charac_party_bonus_exp(0);
        set_charac_member_bonus_exp(0);
        set_charac_fatigue_buf_bonus_exp(0);
        char leveledUp = gain_exp_sp(expGain, curLevelExp, newExp,
                                     (CUser::eExpAddReason)13, 0, false);
        if (leveledUp) {
            if (getCurCharacStamina() < 100)
                setCurCharacStamina((unsigned char)'d');
            // ORIG:else 分支重新调用 get_state()（两站点）。
            if (get_state() == 3) {
                SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
                SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
            } else {
                if (get_state() > 4) {
                    SendNotiPacket((eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
        }
        if (m_chState == 3)
            SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
        if ((m_chState == 3) && leveledUp)
            decide_growth_power_reward_system();
    }

    bool touchedSlotEquip = false;                             // -0x125
    std::vector<int> updDelTriple;                             // -0x134(kind,tgt,cnt)*n
    std::vector<int> updAddQuad;                               // -0x140(idx,cnt,…)*n 四元组
    // ORIG（quest_reward.md Ghidra C 行 2537-2541）在两个 vector<int> 构造之后
    // 紧接 __gnu_cxx::__normal_iterator<int*,vector<int> >::__normal_iterator(local_148)：
    // 这是 ORIG 真实存在的迭代器默认构造（ORIG 调用点 ×1），供下方三元组回包循环使用。
    std::vector<int>::iterator tripleIt;                       // -0x148（ORIG C1Ev ×1）
    int delObjCnt   = 0;                                       // -0x9c
    int delAddObjCnt= 0;                                       // -0x98

    if ((q->m_triggerType == 0 || q->m_triggerType == 0x19) &&
        q->m_fieldB8 /*+0xb8 计数哨兵*/ != 0x19) {
        delObjCnt = getCurCharacInvenW()->delete_event_items(
            delCond, updDelTriple, touchedSlotEquip,
            (eMoneySubReason)4, (eItemDelReason)4, false);
        if (delObjCnt == 0) {
            cMyTrace tr((char*)__PRETTY_FUNCTION__,
                        0x59ef, 5);
            tr("CUser::quest_reward, QuestID : %d delete_object_count == 0 ",
               questId);
            return 0x11;
        }
        if (getGmQuestFlag())                                  // gm 免扣展示位归零
            delObjCnt = 0;
    }

    long moneyPaidSum  = 0;                                    // -0x94 已发金额
    long moneyOfferSum = 0;                                    // -0x90 应发金额
    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0) {                                  // +0xb8 金钱型奖励
        int moneyLimit = G_CDataManager()->GetMoneyLimitPerLevel(
                            get_charac_level(), get_acc_name()); // -0x80
        long curMoney = getCurCharacInvenR()->get_money();     // -0x7c
        std::vector<std::pair<int,int> > gains;                // -0x16c
        const std::vector<RewardItemInfo>* pool =
            getUserQuestRewardItem(q, this);                   // -0x74
        for (std::vector<RewardItemInfo>::const_iterator ri = pool->begin();
             ri != pool->end(); ++ri) {
            if (ri->m_job == 0xb ||
                (ri->m_job == (int)get_charac_job() &&
                 (ri->m_growType == -1 ||
                  ri->m_growType == getCurCharFirstGrowType()))) {
                pairTmp.first  = ri->m_itemIdx;
                pairTmp.second = ri->m_count * repeatN;
                gains.push_back(pairTmp);
            }
        }
        if (selectIdx >= 0 && selectIdx < (int)q->m_selectReward.size()) {
            pairTmp.first  = q->m_selectReward[selectIdx].m_itemIdx;
            pairTmp.second = q->m_selectReward[selectIdx].m_count * repeatN;
            gains.push_back(pairTmp);
        }
        for (std::vector<std::pair<int,int> >::iterator gi = gains.begin();
             gi != gains.end(); ++gi) {
            if (gi->first == 0) {                              // 金钱项：上限封顶
                long offer = goldGain * repeatN;               // local_120*rpt
                moneyOfferSum = offer;
                if (curMoney + offer > (long)moneyLimit) {
                    if ((long)moneyLimit > curMoney) {
                        int allowed = moneyLimit - curMoney;   // -0x70
                        gi->second      = allowed;
                        moneyPaidSum   += allowed;
                        curMoney        = moneyLimit;
                    } else {
                        gi->second = 0;
                    }
                } else {
                    curMoney      += offer;
                    moneyPaidSum  += offer;
                    gi->second     = offer;
                }
            } else {
                SendAradEventItemLog(*(unsigned long*)&gi->first, gi->second);
            }
        }
        delAddObjCnt = getCurCharacInvenW()->insert_event_items(
            gains, updAddQuad, (eMoneyAddReason)3, (eItemAddReason)8,
            false, "", "");
        eventDeliveryQuestClear(questId, repeatN, gains,
                                expGain, goldGain, qpGain);
    }

    ack.put_byte(q->m_triggerType & 0xff);
    ack.put_int(expGain);
    if (q->m_triggerType == 0 || q->m_triggerType == 0x19) {
        if (q->m_triggerType == 0x19) {                       // 每日固定响应
            ack.put_byte(1);
            ack.put_byte(7);
            ack.put_short(0);
            ack.put_int(0);
        } else {
            ack.put_byte(delObjCnt);
            // ORIG（Ghidra C 2719-2745）：put_byte 之后有 0<delObjCnt 守卫，
            // 循环体用共享迭代器逐 int 前进（operator* + 后置 operator++ ×3），
            // 而非 operator[] 索引。
            if (delObjCnt > 0) {
                for (tripleIt = updDelTriple.begin();
                     tripleIt != updDelTriple.end();) {        // 三元组
                    ack.put_byte(*tripleIt);
                    tripleIt++;
                    ack.put_short(*tripleIt);
                    tripleIt++;
                    ack.put_int(*tripleIt);
                    tripleIt++;
                }
            }
        }
    }
    ack.put_byte(q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ & 0xff);
    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0 && delAddObjCnt >= 0) {
        ack.put_byte(delAddObjCnt);
        // ORIG 0x866fbf2-0x866fda3:updAddQuad 实为 [INVEN_TYPE, slot, addInfo2]
        // 三元组(delete_event_items.md:814-871 push [X, slot, item+0x07])。
        // 消费:put_short(e0)/put_int(e1)/put_int(e2) + find_item(e1) +
        // GetInvenSlot(ampFlag, e0) + amp 段,共享迭代器 de×6/pp×3。
        if (delAddObjCnt > 0) {
            std::vector<int>::iterator quadIt = updAddQuad.begin();
            while (quadIt != updAddQuad.end()) {
                // ORIG:put_short 实参内联 de;GetInvenSlot 复用 de 结果槽 -0x68。
                ack.put_short(*quadIt);
                int slotE0 = *quadIt;
                quadIt++;
                // ORIG:e1 先 de 入局部(-0x64),put_int 再次 de,pp 后 de 第三元素。
                int itemE1 = *quadIt;
                ack.put_int(*quadIt);
                quadIt++;
                ack.put_int(*quadIt);
                quadIt++;
                // [CUserOnlyOrig 2026-09-09 真缺口] ORIG 0x0866fd13 `movl $0x1,-0x60(%ebp)`
                // 在 find_item 调用**之前**且为 4 字节写 ⇒ 原码 `int ampFlag = 1;` 声明先于
                // addedDef（旧重建 `unsigned char ampFlag = 1;` 且顺序颠倒）。
                int ampFlag = 1;
                CItem* addedDef = G_CDataManager()->find_item(itemE1);
                if (addedDef != 0 &&
                    addedDef->IsAvatarItem())                // vtable+0x10 无参 bool 虚调用（ORIG 0x866fd47）
                    ampFlag = 2;
                Inven_Item chkItem =
                    getCurCharacInvenRefR()->GetInvenSlot(ampFlag, slotE0);
                stAmplifyOption_t& amp = *&chkItem.m_amp;
                ack.put_byte(amp.getAbilityType() & 0xff);
                ack.put_short(amp.getAbilityValue() & 0xffff);
                if (addedDef == 0 || !addedDef->is_stackable())
                    ack.put_short(0);
                else
                    ack.put_short(chkItem.m_addInfo2 & 0xffff);
            }
        }
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 1) {                                  // 二转
        unsigned char secondType =
            (unsigned char)q->m_fieldC0;
        ack.put_byte(secondType);
        set_grow_type(secondType, getCurCharSecondGrowType(), (char*)&ack, (eChangeGrowTypeReason)0);
        PacketGuard bp;
        bp.put_header(0, 2);
        bp.put_byte(0);
        bp.put_short(1);
        make_basic_info((char*)&bp, 0);
        bp.finalize(true);
        G_GameWorld()->send_all(bp, this);

        Stream* ss = GlobalData::s_stream_pool->Acquire("user.cpp", 0x5aed);
        CStreamGuard sig(ss, true);
        // ORIG:经 operator*()/operator->() 链（de/pt 站点）。
        sig.operator*()->operator<<((int)0x24);
        sig.operator*()->operator<<((int)GetUID());
        SIG_LOAD_SKILL* sk =
            (SIG_LOAD_SKILL*)sig.operator->()->GetInBuffer<SIG_LOAD_SKILL>();
        memset(sk, 0, 0x414);
        sk->m_characNo = static_cast<unsigned int>(getCurCharacNo());  // +0x00（ORIG 0x867003b）
        sk->m_flag410 = 1;
        sk->m_field411 = *reinterpret_cast<unsigned char*>(   // CDataManager+0xa850 表区未建模（CDataManager 域）
            reinterpret_cast<char*>(G_CDataManager()) + 0xa850);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, sig);
    }
    else if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 2) {                             // 一转
        unsigned char firstType =
            (unsigned char)q->m_fieldC0;
        ack.put_byte(firstType);
        set_grow_type(getCurCharFirstGrowType(), firstType, (char*)&ack, (eChangeGrowTypeReason)0);
        PacketGuard bp;
        bp.put_header(0, 2);
        bp.put_byte(0);
        bp.put_short(1);
        make_basic_info((char*)&bp, 0);
        bp.finalize(true);
        G_GameWorld()->send_all(bp, this);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 10 && q->m_fieldC0 > 0) {
        int evoInto  = q->m_fieldFC;
        int evoFrom  = q->m_fieldC0;
        user_creature::CCreatureMgr* cm =
            getCurCharacInvenW()->GetCreatureMgrW();
        if (!cm->Evolute(evoFrom, evoInto, false))
            return 0x16;
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x14) {                               // 专家职业
        if (get_charac_level() < q->m_levelMin) return 0x16;
        int expertType = q->m_fieldC0;             // -0x4c
        SetCurCharacExpertJobType(expertType);
        ack.put_byte(expertType);
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x22, 0, (ENUM_PACKETCLASS)0);
        LogHistory("ExpertJob+,%d,%d", get_charac_level(), expertType);
        int posMarker = ack.get_index();                       // 计数回填点一
        int cnt = 0;
        ack.put_byte(0);
        STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(expertType);   // ORIG 0x8670387
        if (script != 0) {
            for (size_t s = 0; s < script->m_growSkillVec.size(); ++s) {
                if (getCurCharacR() != 0) {
                    // ORIG:两字段读各自 operator[](ix 站点 ×2)。
                    int gained = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(),
                        script->m_growSkillVec[s].first,
                        script->m_growSkillVec[s].second,
                        (ENUM_SKILL_TREE_KIND)0);
                    if (gained > 0) {
                        ack.put_byte(gained);
                        ack.put_byte(script->m_growSkillVec[s].first);
                        ack.put_byte(script->m_growSkillVec[s].second);
                        ++cnt;
                    }
                }
            }
            ack.put_byte(posMarker, cnt);   // 回填计数（ORIG put_byteERii 写位重载）
            posMarker = ack.get_index();                       // 回填点二
            cnt = 0;
            ack.put_byte(0);
            for (size_t s = 0; s < script->m_growSkillVec.size(); ++s) {
                if (getCurCharacR() != 0) {
                    int gained = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(),
                        script->m_growSkillVec[s].first,
                        script->m_growSkillVec[s].second,
                        (ENUM_SKILL_TREE_KIND)1);
                    if (gained > 0) {
                        ack.put_byte(gained);
                        ack.put_byte(script->m_growSkillVec[s].first);
                        ack.put_byte(script->m_growSkillVec[s].second);
                        ++cnt;
                    }
                }
            }
            ack.put_byte(posMarker, cnt);   // 回填计数（ORIG put_byteERii 写位重载）
            expert_job::CCharacExpertJob* ej =
                (expert_job::CCharacExpertJob*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(0));
            ej->InitScript(script);
            SetCurCharacExpertJobExp(0);
        }
        PacketGuard bb;
        bb.put_header(0, 2);
        bb.put_byte(0);
        bb.put_short(1);
        make_basic_info((char*)&bb, 0);
        bb.finalize(true);
        G_GameWorld()->send_all(bb, this);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x15) {                               // 副本次数++
        int dungeonId = q->m_fieldC0;
        ack.put_short(dungeonId);
        AddDungeonClear(dungeonId, 3);
        PacketGuard dp;
        dp.put_header(0, 5);
        dp.put_short(1);
        dp.put_short(dungeonId);
        dp.put_short(4);
        dp.finalize(true);
        Send(dp);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x16) {                               // 扩充栏位
        int slotMode = q->m_fieldC0;
        char curStat = GetCurExEquipSlotStat();
        if (slotMode == 0x14) {
            if (curStat == 0 || curStat == 2)
                SetCurExpEquipSlotStat(curStat + 1);
        } else if (slotMode == 0x15) {
            if (curStat == 0 || curStat == 1)
                SetCurExpEquipSlotStat(curStat + 2);
        }
        ack.put_byte(GetCurExEquipSlotStat() & 0xff);
        ack.put_byte(slotMode);
    }

    if (q->m_fieldB8 /*Quest 对象内 rewardType 实测读 +0xb8（quest_reward.md）*/ == 0x19) {                               // 宠物进化任务
        std::vector<std::pair<int,int> > evoList;              // -0x1bc
        const std::vector<RewardItemInfo>* pool =
            getUserQuestRewardItem(q, this);                   // -0x20
        for (std::vector<RewardItemInfo>::const_iterator ri = pool->begin();
             ri != pool->end(); ++ri) {
            if (ri->m_job == 0xb ||
                (ri->m_job == (int)get_charac_job() &&
                 (ri->m_growType == -1 ||
                  ri->m_growType == getCurCharFirstGrowType()))) {
                pairTmp.first  = ri->m_itemIdx;
                pairTmp.second = ri->m_count * repeatN;
                evoList.push_back(pairTmp);
            }
        }
        if (selectIdx >= 0 && selectIdx < (int)q->m_selectReward.size()) {
            pairTmp.first  = q->m_selectReward[selectIdx].m_itemIdx;
            pairTmp.second = q->m_selectReward[selectIdx].m_count * repeatN;
            evoList.push_back(pairTmp);
        }
        if (evoList.size() == 1) {
            for (std::vector<std::pair<int,int> >::iterator ei = evoList.begin();
                 ei != evoList.end(); ++ei) {
                    CItem* tgt = G_CDataManager()->find_item(ei->first);
                if (tgt == 0) return 0x16;
                if (tgt->GetItemType() == 0x16) {              // vtable+0xc==22
                    user_creature::CCreatureMgr* cm =
                        getCurCharacInvenW()->GetCreatureMgrW();
                    if (!cm->EventEvoluteCreature(ei->first,
                            q->m_fieldFC, ei->second))
                        return 0x16;
                    break;
                }
            }
        } else {
            return 0x16;
        }
    }

    ack.finalize(true);
    Send(ack);
    if (touchedSlotEquip)
        send_equip(0x16);
    if (moneyPaidSum < moneyOfferSum)
        SendMoneyFullReason((ENUM_MONEY_FULL_REASON)4,
                            moneyOfferSum, moneyPaidSum);
    return 0;
