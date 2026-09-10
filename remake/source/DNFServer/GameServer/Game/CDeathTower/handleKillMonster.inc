int CDeathTower::handleKillMonster(CUser* user, unsigned short monsterIdx, unsigned short a3, const MSG_MONSTER_DIE& msg, int* a5)
{
    if (m_stage.getState() != 1)
    {
        // ORIG 0x08464876..0x0846488c：无 user 判空，直接调用（P140/P171 照抄原码）
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2a, 0x13);
        return 0;
    }

    // Monster kill checker (skill material time)
    // ORIG 0x0846489b..0x084648eb：this+0xb58 每使用点重算（无 checker 具名槽，P183）；
    // 两条件经 && 物化于 %eax（无 bSkillMaterial 槽，P0）；tick 内联实参、
    // dieMonster 返回值直接无符号比较（jbe）。
    if (((stMapMonsterKillChecker_t*)&m_timerKeys[7])->getUseSkillMaterialTime() == 0 &&
        ((stMapMonsterKillChecker_t*)&m_timerKeys[7])->dieMonster(
            GlobalData::s_systemTime_.getCurTickCount(), 1000) > 2)
    {
        bool isPremium = false;
        if (user && user->get_charac_level() >= 0x1e)
        {
            isPremium = true;
        }
        else if (user && user->get_charac_level() >= 0x19)
        {
            // [b28d-SCT] P140/P171：ORIG 0x08464954 无 GetPremiumInfo 判空，返回值
            // 直接作 receiver 传 GetPremiumInfoList（链式，ORIG 无 -0x70 槽）；
            // 旧 `if (premium)` 多 1 je + 1 槽。
            // [b28d-solo-CDeathTower] ORIG 0x0846496e 调按值返回版
            // _ZNK8WongWork12CUserPremium18GetPremiumInfoListEi（sret -0xdc，type=0x270f）。
            std::vector<SUserPremiumInfo> premiumList =
                user->GetPremiumInfo()->GetPremiumInfoList(0x270f);
            // ORIG 0x084649xx：`cmp i,size; seta`（无符号）⇒ 索引 unsigned（P196/P175）
            for (unsigned int i = 0; i < premiumList.size(); i++)
            {
                // [b28d-solo-CDeathTower] P157/P183：ORIG 0x0846498f/ab/c7 三次
                // 独立 operator[]（每字段重读，无 type 缓存）。
                if (premiumList[i].m_type >= 0x17 &&
                    premiumList[i].m_type <= 0x1b &&
                    premiumList[i].m_state == 1)
                {
                    isPremium = true;
                    break;
                }
            }
        }

        if (isPremium)
        {
            ((stMapMonsterKillChecker_t*)&m_timerKeys[7])->initMonsterInfo();
            // [b28d-SCT] P140/P171：ORIG 0x08464aXX 无 getHackAnalyzer 判空，
            // 返回值直作 receiver（ORIG 无 -0x68 槽）；旧 `if (hackAnalyzer)` 多 1 je。
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACK_TYPE)0xd6, 1, 0, 0);
        }
    }

    // Kill monster in stage
    map_monster monster;
    int killResult = m_stage.killMonster(monsterIdx, monster);
    if (killResult == 0)
    {
        return 0;
    }

    // ORIG 0x08464ab4：调用 getMemberLevelGap()，返回值 ORIG 亦弃用（汇编仅存栈）
    int memberLevelGap = m_party->getMemberLevelGap();
    int winPoint = 0;
    std::vector<unsigned long> dropItemIds;
    std::list<map_item> dropList;
    // [b28d-solo-CDeathTower] ORIG 0x08464ae8：stGenerateResult_t 在 dropItemIds/
    // dropList 之后构造（p167：C1 ×1 / D1 ×2），掉落项存 result.m_dropItems。
    stGenerateResult_t result;
    // ORIG 0x08464aed：三个容器构造完成后才把 mobIndex 写回 a5（if (a5)）
    if (a5)
    {
        *a5 = monster.m_mobIndex;
    }

    // [b28d-SCT] P59/jcc 判据（ORIG 0x08464afe..0x08464b97）：`movzbl -0x528;
    // cmp $0x4,%al; jle 0x08464b97` ⇒ fall-through（roleType > 4）= Normal(AI char)
    // 分支物理在前，Boss(find_monster) 分支在 0x08464b97。旧写法 `if (roleType < 5)
    // {Boss} else {Normal}` 令 4.4 把两分支体互换、call 序错位（find_monster 段
    // @24 vs ORIG@29）。改正向条件 + 交换分支体，语义不变。
    if (monster.m_roleType > 4)
    {
        // Normal monster drop
        // [b28d-SCT] P140/P171：ORIG 0x08464b15 `call G_CDataManager; mov 0x869c
        // (%eax),%eax; ... call get` —— m_aiCharacterList 直作 receiver，无
        // aiCharList 判空/槽；旧 `if (aiCharList)` 多 1 je。
        CAICharacter* aiChar =
            G_CDataManager()->m_aiCharacterList->get(monster.m_mobIndex);
        if (aiChar)
        {
            // [b28d-solo-CDeathTower] ORIG 0x08464b15 段：无 dungeon 判空。
            if (m_dungeonMgr.getDungeon()->hasTowerItemDrop())
                aiChar->deathTowerSpecifyItemDrop(dropItemIds);
            if (monster.m_dropCnt == 100 || monster.m_dropCnt == 200)
            {
                winPoint = aiChar->getPvPWinPoint();
            }
        }
    }
    else
    {
        // Boss monster drop
        CMonster* monsterPtr = (CMonster*)G_CDataManager()->find_monster(monster.m_mobIndex);
        // [b28d-solo-CDeathTower] ORIG 0x08464c1a..c60：monsterPtr 非空后短路求值
        // getDungeon()->hasTowerItemDrop()（ORIG 无 dungeon 判空，P140/P171）。
        unsigned long dropId = 0xfffffffe;
        if (monsterPtr != 0 && m_dungeonMgr.getDungeon()->hasTowerItemDrop())
        {
            dropId = monsterPtr->deathTowerSpecifyItemDrop();
        }
        // [b28d-solo-CDeathTower] ORIG 0x08464d63：push_back 传左值 ⇒ const& 版。
        if (dropId != 0xfffffffe)
        {
            dropItemIds.push_back(dropId);
        }
    }

    // Create items from drop ids（ORIG 0x08464c97 段：const_iterator 遍历 + empty 前置判定）
    if (!dropItemIds.empty())
    {
        // [b28d-solo-CDeathTower] ORIG 0x08464c97 段：begin/end 各转 const_iterator
        // 具名持有（local_e8/local_ec），再比较两个 const_iterator（p167 差集实证）。
        std::vector<unsigned long>::const_iterator it = dropItemIds.begin();
        std::vector<unsigned long>::const_iterator itEnd = dropItemIds.end();
        for (; it != itEnd; ++it)
        {
            int itemId = (int)*it;
            CItem* item = G_CDataManager()->m_itemList->find_item(itemId);
            if (item)
            {
                Inven_Item invenItem;
                // ORIG 0x08464ce1 第二次 operator*（P157 重读，非缓存 itemId）
                invenItem.m_addInfo = (int)*it;
                G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)1, invenItem, 1);
                unsigned int memberCnt = m_party->get_member_count();
                for (unsigned int j = 0; j < memberCnt; j++)
                {
                    result.m_dropItems.push_back(invenItem);
                }
            }
        }
    }

    // Exp rewards for party members
    for (int i = 0; i < 4; i++)
    {
        CUser* partyUser = m_party->get_user(i);
        if (!partyUser || !m_party->checkValidUser(i))
        {
            continue;
        }

        // [b28d-solo-CDeathTower] ORIG 0x08464dXX 段：`*(int*)(*(int*)this+0xcd8)!=1`
        // = m_party->m_battleField.m_bloodState != 1（不是 get_party_type，真语义修正）。
        if (m_party->m_battleField.m_bloodState != 1)
        {
            unsigned int mobExp = 0;
            int charLevel = partyUser->get_charac_level();
            G_CDataManager()->get_mob_reward(charLevel, mobExp);
            // ORIG 0x08464e69：getMonsterTotalExp 后经 set_random_reward 取回
            mobExp = m_party->getMonsterTotalExp(mobExp, false, 0, 1.0f, 1.0f, false);
            QuickParty::RewardData_Param rewardParam(m_party);
            QuickParty::RewardData_Result rewardResult;
            rewardResult.m_value00 = mobExp;
            m_party->set_random_reward((QuickParty::RandomBuffType)0, rewardParam,
                                       rewardResult);
            mobExp = rewardResult.m_value00;

            if (monster.m_roleType == 5)
            {
                // ORIG 0x08464f2c：+0x5198 = CDataManager::m_deathTower.m_apcExpWeight
                // [N85/N97 封顶·实测回退] ORIG 0x08464f06..0x08464f4b：`fildll -0x550;
                // fstps -0x56c; call; flds 0x5198(%eax); fmuls -0x56c; … fistpl`
                // （右操作数先求值 + float 中介槽 + 32 位 fistpl）；P252 具名 float
                // 局部两形实测：our 1043→1046、帧 +0x10（-0x150→-0x164）⇒ 反劣回退。
                // 4.4 对 `(unsigned int)(float*float)` 恒 fistpll（64 位）且左操作数先
                // 求值 ⇒ 与 4.1 fistpl/右先 属跨版本 x87 lowering 差（N85 邻族）。
                mobExp = (unsigned int)(G_CDataManager()->m_deathTower.m_apcExpWeight *
                                        (float)mobExp);
            }

            // ORIG 0x08464f80：+0x5e1c = m_deathTower.m_monsterExpWeight（无条件）
            // 同族（ORIG -0x568 槽 + fistpl，同上封顶）。
            mobExp = (unsigned int)(G_CDataManager()->m_deathTower.m_monsterExpWeight *
                                    (float)mobExp);

            // ORIG 0x08465000：STGainedEachExpData(m_exp=mobExp) 后
            // sumGainedEachExp 取回
            STGainedEachExpData expData;
            expData.m_exp = mobExp;
            mobExp = m_party->sumGainedEachExp(partyUser, expData);

            if (winPoint != 0)
            {
                // ORIG 0x08465017：GetRepeatEvent(0x27) 后直接调 getBonusRate
                // （不判空，与 ORIG 一致）；rate==0 → 原值，否则乘率取整
                CDeathTowerWinPointEvent* event =
                    (CDeathTowerWinPointEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x27);
                float bonusRate = event->getBonusRate();
                if (bonusRate == 0.0f)
                    partyUser->gainWinPoint(winPoint, (eWPAddReason)2);
                else
                    partyUser->gainWinPoint((int)((float)winPoint * bonusRate),
                                            (eWPAddReason)2);
            }

            // ORIG 0x08465090：calcHelpAbuseRatio(mobExp, false)
            partyUser->calcHelpAbuseRatio(mobExp, false);

            // ORIG 0x084650XX：expGain(-0x120)=0 → notiType(-0x34)=0xb → spGain(-0x124)=0
            int expGain = 0;
            int notiType = 0xb;
            if (monster.m_roleType == 5)
            {
                notiType = 0xc;
            }

            int spGain = 0;
            if (partyUser->gain_exp_sp(mobExp, expGain, spGain,
                                       (CUser::eExpAddReason)notiType,
                                       monster.m_mobIndex, false))
            {
                partyUser->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
            }

            // ORIG 0x0846515f：cUserHistoryLog（CUser 嵌套成员 m_historyLog，+0x79700）
            // KillMonster 日志；killValue1=msg+0xa1e / killValue2=msg+0xa1a /
            // a=+0xa24 / b=+0xa26；curExp=getCurCharacExp()
            partyUser->m_historyLog.KillMonster(
                monster.m_mobIndex, msg.m_killValue1, msg.m_killValue2,
                msg.m_killPairA24, msg.m_killPairA26, msg,
                static_cast<char>(monster.m_roleType), monster.m_roleType == 5,
                partyUser->getCurCharacExp(), (int)mobExp,
                (CUser::eExpAddReason)notiType);

            // ORIG 0x08465244：CheckQuestMonster(dungeonIndex, mobIndex, type)
            // type：roleType 5..8 → 2，否则 1
            {
                int questType = 1;
                if (monster.m_roleType > 4 && monster.m_roleType < 9)
                    questType = 2;
                // ORIG 0x08465244：无 dungeon 判空（P140/P171）
                partyUser->CheckQuestMonster(m_dungeonMgr.getDungeon()->get_index(),
                                             monster.m_mobIndex,
                                             (ENUM_QUEST_ENEMY_TYPE)questType);
            }
        }
    }

    // Drop items to stage and send packet（ORIG 0x084653XX：const_iterator 遍历 result.m_dropItems）
    std::vector<Inven_Item>::const_iterator it = result.m_dropItems.begin();
    std::vector<Inven_Item>::const_iterator itEnd = result.m_dropItems.end();
    for (; it != itEnd; ++it)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = a3;
        item.m_createTick = GlobalData::s_systemTime_.getCurSec();
        item.m_item = *it;
        m_stage.dropItem(item);
        dropList.push_back(item);
    }

    // Send drop item packet to party
    // [b28d-solo-CDeathTower] p167：ORIG 用嵌套 CPacketHandler（C1 0x08469a8c / D1 ×2）。
    CPacketHandler packet;
    packet.put_header(0, 0x26);
    packet.put_short(monsterIdx);
    int itemCountPos = packet.get_index();
    packet.put_byte(0);  // placeholder for count
    // ORIG 0x084654XX：emblemInfo 在循环前声明并 init 一次，循环内复用。
    stAvatarEmblemInfo_t emblemInfo;
    emblemInfo.init();

    int itemCount = 0;
    // [b28d-SCT] P13：ORIG 0x084655XX 段 begin(-0x10)/end(-0x14) 各一次，end 在
    // 循环外缓存（call 序 begin,end 相邻）；旧写法条件位每次求值 end（我方 call 序
    // end 落到 ++it 之后）。
    std::list<map_item>::const_iterator dropIt = dropList.begin();
    std::list<map_item>::const_iterator dropItEnd = dropList.end();
    for (; dropIt != dropItEnd; ++dropIt)
    {
        const map_item& item = *dropIt;
        packet.put_short(item.m_itemIndex);
        packet.put_int(item.m_item.m_addInfo);
        unsigned char itemAttr = item.m_item.GetItemAttr();
        packet.put_byte(itemAttr);

        // [b28d-SCT] P59：ORIG call 序 find_item→B(!equipItem 的 put_int)→C(avatar
        // 字段)→A(非 avatar 的 put_int 在段尾 111-112) ⇒ 外层条件正向
        // `isAvatarItemType()`（then 体物理在前），旧反向写法令 A 落到段首。
        if (item.m_item.isAvatarItemType())
        {
            CEquipItem* equipItem = (CEquipItem*)G_CDataManager()->find_item(item.m_item.m_addInfo);
            if (!equipItem)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                // ORIG 0x08465554：天数(m_fieldb 低字节) * 0x15180(86400s)；同 WarField 发包
                packet.put_int(equipItem->getAvatarPeriod(
                                   (unsigned char)item.m_item.m_fieldb) *
                               0x15180);
                emblemInfo.init();
                equipItem->getAvatarSocket(item.m_item.m_fieldb, emblemInfo);
            }
        }
        else
        {
            packet.put_int(item.m_item.get_add_info());
        }

        packet.put_short(item.m_item.m_fieldb);
        unsigned char amplifyType = item.m_item.m_amp.getAbilityType();
        packet.put_byte(amplifyType);
        unsigned short amplifyValue = item.m_item.m_amp.getAbilityValue();
        packet.put_short(amplifyValue);
        packet.put_packet(item.m_item);

        // [b28d-SCT] P59：ORIG 0x084658XX `isAvatarItemType; je L_else; put_byte(1);
        // put_int(0x1e); put_binary; jmp; L_else: put_byte(0)` ⇒ 正向条件。
        if (item.m_item.isAvatarItemType())
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            // [b28d-solo-CDeathTower] p167：ORIG 调 put_binary(char const*, int)。
            packet.put_binary((const char*)&emblemInfo, 0x1e);
        }
        else
        {
            packet.put_byte(0);
        }

        packet.put_short(item.m_dropIndex);
        itemCount++;
    }

    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(itemCountPos, itemCount);
    packet.finalize(true);
    m_party->send_to_party(packet);

    // Check stage clear
    if (m_stage.checkClearStage())
    {
        _onFinishStage();
        // [b28d-SCT] P0/P52：ORIG 0x0846585x `call getCurrentStage` 结果留
        // callee-saved %ebx 跨 getEndStage 调用，`cmp %eax,%ebx; sete/test; je`
        // 直接比较；旧 curStage/endStage 两局部多 2 槽（我方 -0x1c/-0x20）。
        if (m_stage.getCurrentStage() == m_dungeonMgr.getEndStage())
        {
            _onPrepareFinishTower();
        }
    }

    return 1;
