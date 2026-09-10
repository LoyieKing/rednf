void CUser::increase_status(short slotNo)
{
    PacketGuard packet;

    if (CheckInTrade() != 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
        return;                                              // L086686d4
    }

    // ORIG 0x086657fc 区内无 _ZN10Inven_ItemC1Ev 调用点，仅 1×GetInvenSlot：
    // 按值返回 sret 直写拷贝初始化，无默认构造。
    Inven_Item slotItem = getCurCharacInvenR()->GetInvenSlot(1, slotNo);
    char type = -1;                                          // -0xd1
    unsigned int value = 0;                                  // -0xd8
    register unsigned int itemIdx = (unsigned int)slotItem.m_addInfo;

    // ---- itemIdx → (type,value) 判定表 ----
    // ORIG 0x0866589d 起是 4.1 的 switch 二分决策树（单次载入 itemIdx 后
    // `cmp $K,%eax` + je/ja）；4.4 同源形（/tmp/sw_probe.cpp 4.4.7 探针实证），
    // if-else 链恒退化为 `cmpl $K,mem`+jne（P251/P261/P286）。
    // case 书写序 = ORIG 叶块地址序（P324）；体尾 goto L08666990 按 ORIG 汇编补齐。
    switch (itemIdx)
    {
    case 0x292091ad: type = 0x19; value = 1; goto L08666990;      // 0x08665ba5
    case 0x407:  type = 0; value = 5; goto L08666990;             // 0x08665bbb
    case 0x40e:  type = 0; value = 0x14; goto L08666990;          // 0x08665bd1
    case 0x4b4:  type = 0x11; value = 1; goto L08666990;          // 0x08665be7
    case 0x4b5:  type = 0x11; value = 5; goto L08666990;          // 0x08665bfd
    case 0x40a:  type = 1; value = 100; goto L08666990;           // 0x08665c13
    case 0x40b:  type = 1; value = 1000; goto L08666990;          // 0x08665c29
    case 0x40c:  type = 1; value = 10000; goto L08666990;         // 0x08665c3f
    case 0x40d:  type = 1; value = 100000; goto L08666990;        // 0x08665c55
    case 0x4cf:  type = 1; value = 1000000; goto L08666990;       // 0x08665c6b
    case 0x4d0:  type = 1; value = 10000000; goto L08666990;      // 0x08665c81
    case 0x1f71: case 0x204b: case 0x204c: case 0x204e: case 0x204f:
    case 0x28d2bc: goto L08665c97;                                // 0x08665c97
    case 0x989a13: case 0x292090e1: goto L08665d24;               // 0x08665d24
    case 0x40f:  type = 4; value = 0x32; goto L08666990;          // 0x08665d88
    case 0x410:  type = 6; value = 0x32; goto L08666990;          // 0x08665d9e
    case 0x411:  type = 5; value = 0x32; goto L08666990;          // 0x08665db4
    case 0x412:  type = 7; value = 0x32; goto L08666990;          // 0x08665dca
    case 0x413:  type = 2; value = 0xfa; goto L08666990;          // 0x08665de0
    case 0x414:  type = 3; value = 0xfa; goto L08666990;          // 0x08665df6
    case 0x415:  type = 8; value = 10; goto L08666990;            // 0x08665e0c
    case 0x416:  type = 9; value = 10; goto L08666990;            // 0x08665e22
    case 0xc9: case 0x28bdb8: goto L08665e38;                     // 0x08665e38
    case 0xc84:                                                   // 0x08665fd3
                        if (get_charac_guildkey() == 0)
        {
                            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x40, packet);
            return;
        }
                        type = 0xb; value = 10;
        goto L08666990;
    case 0x1bc1:                                                  // 0x08666027
                    if (get_charac_guildkey() == 0)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x40, packet);
            return;
        }
                    type = 0xb; value = 0x32;
        goto L08666990;
    case 3: case 0x1c: case 0x6d: case 0xa1: case 0xa2:
    case 0x394: case 0x3c0: case 0x3c3: case 0x4b6: case 0x4e5: case 0x28982c:
        goto L0866607b;                                           // 0x0866607b
    case 0x28974b: case 0x289788: goto L08666320;                 // 0x08666320
    case 0x28974c:                                                // 0x08666425
            if (G_GameWorld()->IsPvPSkilTreeChannel() != 1)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
            return;
        }
        {
            unsigned char errSecu =
                (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                    this, (SECURITY_PROTCTION)0x2c);
            if (errSecu != 0)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                return;
            }
        }
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
        {
                            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd8, packet);
            return;
        }
                value = 0; type = 0x17;
        goto L08666990;
    case 0x1bbd:                                                  // 0x08666517
                if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
                type = 0xd; value = 8;
        goto L08666990;
    case 0x1c82:                                                  // 0x0866656c
                if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
                type = 0xd; value = 2;
        goto L08666990;
    case 0x1c0c: type = 0xd; value = 1; goto L08666990;           // 0x086665c1
    case 0x1c0d: type = 0xd; value = 5; goto L08666990;           // 0x086665d7
    case 0x2a: case 0x28d14d: type = 0x15; value = 1; goto L08666990;  // 0x086665ed
    case 0x4df:  type = 0x13; value = 10; goto L08666990;         // 0x08666603
    case 0x1f16: type = 0x14; value = 0; goto L08666990;          // 0x08666619
    case 0x4b0: case 0x4b1: case 0x4b2: case 0x2920915c:
        goto L0866662f;                                           // 0x0866662f
    default:
            goto L08666751;
    }

L08665c97:                                                        // 0x08665c97
    {
            bool reject = false;
            if (!CalLevelUpItemCheck(itemIdx))
        {
                reject = false;                              // asm: 不报错，走公共段
        }
        else
        {
                int valueTmp = (int)value;
                reject = (CalLevelUpItemState(type, valueTmp, 0x13, 0x3b) != 1);
                value = (unsigned int)valueTmp;
        }
            if (reject)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }
                    goto L08666990;

L08665d24:                                                        // 0x08665d24
    {
        int valueTmp = (int)value;
        bool ok54 = (CalLevelUpItemState(type, valueTmp, 1, 0x54) == 1);
        value = (unsigned int)valueTmp;
        if (!ok54)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }
                    goto L08666990;

L08665e38:                                                        // 0x08665e38
    {
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if ((repeatEvent == 0) || (!repeatEvent->IsEventing(0)))
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x13);
            packet.finalize(true);
            Send(packet);
            return;
        }
        short totalFatigue = (short)getCurCharacTotalFatigue();
        if (totalFatigue == 0)
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x43);
            packet.finalize(true);
            Send(packet);
            return;
        }
        type = 0x0e; value = 0;
    }
                goto L08666990;

L0866607b:                                                        // 0x0866607b
    {
                    unsigned char errSecu =
                        (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                            this, (SECURITY_PROTCTION)0x2c);
                    if (errSecu != 0)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
                        return;  // L086686d4
        }
                    if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
            return;
        }
                    if (CheckItemLock(1, (int)slotNo) != 0)
        {
                        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd5, packet);
            return;
        }
                    if (!WongWork::CSkillChanger::CheckCondition(this))
        {
                        setCurCharacExp(G_CDataManager()->get_level_exp(get_charac_level()));
        }
                    if ((itemIdx == 0x4b6) || (itemIdx == 0x4e5))
        {
                        if (get_charac_level() < 0x32) return;    // asm 直跳 L086686d4
                        if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd8, packet);
                return;
            }
                        value = 1; type = 0x12;
        }
                    else if (itemIdx == 0x394) { value = 1; type = 0xa; }
                    else if (itemIdx == 0x3c0)
        {
                        value = 1; type = 0x18;
                        CQuestShop* questShop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
                        if (questShop != 0)
            {
                            questShop->clearQP(this);
                            questShop->sendCharacQp(this, '\0');
            }
        }
                    else if (itemIdx == 0x3c3) { value = 1; type = 0xa; }
                    else if (itemIdx == 0x28982c) { value = 1; type = 0xa; }
                    else { value = 0; type = 0xa; }
        goto L08666990;
    }

L08666320:                                                        // 0x08666320
    {
                if (G_GameWorld()->IsPvPSkilTreeChannel() != 1)
        {
                    SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x17, packet);
            return;
        }
            unsigned char errSecu =
                (unsigned char)GlobalData::s_pSecuProtectionField->Check(
                    this, (SECURITY_PROTCTION)0x2c);
            if (errSecu != 0)
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, errSecu, packet);
            return;
        }
            if (!WongWork::CSkillChanger::CheckCondition(this))
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 1, packet);
            return;
        }
            value = 0; type = 0x16;
            if (itemIdx == 0x289788) value = 1;
    }
        goto L08666990;

L0866662f:                                                        // 0x0866662f
    {
            if (!IsGuildMaster())
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xdb, packet);
            return;
        }
            STGuildDBInfoOnly* guildDB = (STGuildDBInfoOnly*)GetGuildDBInfo();
            int needFund =
                G_CDataManager()->m_guildParameter.getGuildLevelUpParam(guildDB->m_fundLevel + 1);
            if (needFund == 0)
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd2, packet);
            return;
        }
            if ((unsigned int)needFund <= guildDB->m_fundExp)      // +0x29 无符号比较
        {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0xd2, packet);
            return;
        }
            type = 0x10;
            value = G_CDataManager()->m_guildParameter.getGuildExpBook(guildDB->m_fundLevel + 1);
        goto L08666990;
    }

L08666751:
    // 默认路径：查 CDataManager::m_field50cc 命中或 GetIncreaseStatus 描述。
    // ORIG:每次使用都重新调用 G_CDataManager()（无缓存，4 个静态站点）。
    {
        if (itemIdx == (unsigned int)G_CDataManager()->m_growthCapsule.m_capsuleId)
        {
            CItem* itemA = G_CDataManager()->find_item(
                G_CDataManager()->m_growthCapsule.m_capsuleId);
            if (itemA == 0)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (!itemA->is_stackable())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (get_charac_level() < itemA->getUsableLevel())
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            if (get_charac_level() > 0x47)
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            type = 1; value = 0;
            int vtmp = 0;
            if (!itemA->GetIncreaseStatusIntData(0, vtmp))
            {
                SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
                return;
            }
            value = (unsigned int)vtmp;
        }
        else
        {
            CItem* itemB = G_CDataManager()->find_item(itemIdx);
            if ((itemB != 0) && (itemB->GetIncreaseStatusType() != 0) &&
                (itemB->GetIncreaseStatusType() == 1))
            {
                int vtmp = 0;
                itemB->GetIncreaseStatusIntData(0, vtmp);
                value = (unsigned int)vtmp;
                type = 1;
            }
        }
    }

L08666990:
    if (type == -1)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
        return;                                              // L086686d4
    }
    if (type == 0x0f)
    {
        if (get_state() != 3)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
        if ((int)(getCurCharacFatigue() & 0xffff) < (int)value)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x5f, packet);
            return;
        }
    }
    if (((int)type == 0) || ((int)type == 0x11) || ((int)type == 1))
    {
        if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
            return;
        }
    }

    CItem* actionItem = G_CDataManager()->find_item(itemIdx);
    if (actionItem == 0) return;                             // L086686d4
    if (actionItem->GetUsablePvPRank() > get_pvp_grade())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 1, packet);
        return;
    }
    if (actionItem->GetItemType() != 0x15)
    {
        CInventory* invenW = getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, slotNo, 1, (eItemDelReason)10, true))
        {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x11, packet);
            return;
        }
    }

    switch ((unsigned char)type)
    {
    case 0:                                                  // SP（asm +0x1435）
    {
        gain_sp(value);
        history_log_sp(-1, value, (eSPAddReason)1);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(0);
        packet.put_short(0);
        packet.finalize(true);
        Send(packet);
        return;                                              // asm jmp L086686d4
    }
    case 1:                                                  // EXP 类（asm +0x167f）
    {
        int bonusA = 0;                                      // -0xdc
        int bonusB = 0;                                      // -0xe0
        int spBefore = 0;                                    // -0xe4
        int spAfter = 0;                                     // -0xe8
        if (get_charac_level() > 0x54) value = 0;
        if (itemIdx == (unsigned int)G_CDataManager()->m_growthCapsule.m_capsuleId)
            rewardExp(value, bonusA, bonusB, spBefore, spAfter, (::eExpAddReason)3, true);
        else
            rewardExp(value, bonusA, bonusB, spBefore, spAfter, (::eExpAddReason)0, true);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(spAfter - spBefore);
        packet.put_short(bonusB);
        packet.finalize(true);
        Send(packet);
        return;
    }
    case 2:                                                  // AddInfoRef +0x00（int）
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field00 += value;
        break;
    }
    case 3:                                                  // +0x04（int）
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field04 += value;
        break;
    }
    case 4:                                                  // +0x08（short）
    {
        // [b28d-solo-CUserOnlyOrig P167 真形] ORIG 每 case 仅 1 次 getCurCharacAddInfoRefW
        // （8667065..866714d 共 8 处）；`f = f + v` 两读形多 5 个调用点 ⇒ 改 `+=`。
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field08 += static_cast<short>(value);
        break;
    }
    case 5:                                                  // +0x0a
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field0a += static_cast<short>(value);
        break;
    }
    case 6:                                                  // +0x0c
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field0c += static_cast<short>(value);
        break;
    }
    case 7:                                                  // +0x0e
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field0e += static_cast<short>(value);
        break;
    }
    case 8:                                                  // +0x42（int）
    {
        reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field42 += value;
        break;
    }
    case 9:                                                  // 四属性循环 (+8)*2
    {
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoRefW())->m_field10[i] += static_cast<short>(value);
        }
        break;
    }
    case 0xa:
    case 0x18:                                               // 转职/成长洗点重建技能树
    {
        bool ok = false;
        if (value == 1)
        {
            SkillSlot* skillSlot = (SkillSlot*)getCurCharacSkillW();
            skillSlot->clear_all_skills_both();
            skillSlot = (SkillSlot*)getCurCharacSkillW();
            skillSlot->set_parent(this);
            addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot,
                                      get_charac_job());
            addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot2,
                                      get_charac_job());
            std::vector<std::pair<int,int> > giveVec;
            std::pair<int,int> pr;
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int job2 = get_charac_job();
            bool got = G_CDataManager()->m_characList->m_characs[job2].
                           get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
            if (got)
            {
                // [A06 2026-09-10 轴B 试行(P196 已前置：ORIG 对应循环尾为 setg/setl 有符号序)]
                // 旧 `unsigned int i` ⇒ 4.4 `cmp -0xb4(%ebp),%eax; seta`（无符号）；
                // 改 int + `(int)size()` 复现 ORIG 的有符号序。
                for (int i = 0; i < static_cast<int>(giveVec.size()); ++i)
                {
                    int j = get_charac_job();
                    pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second,
                                                                       (ENUM_SKILL_TREE_KIND)0);
                    j = get_charac_job();
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second,
                                                                       (ENUM_SKILL_TREE_KIND)1);
                }
                STExpertJobScript* expert = (STExpertJobScript*)
                    G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
                if (expert != 0)
                {
                    // [A06 2026-09-10 轴B 试行记录·已回退] 本处（含下面 giveVec/第二 expert 循环共 3 处）
                    // 改 int 实测 delta 2282→2287（+5）反劣 ⇒ 该 3 处 ORIG 侧非 setg（仅 9391 处命中）⇒ 维持 unsigned。
                    for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
                    {
                        int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        int j = get_charac_job();
                        std::pair<int,int>& pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)0);
                        lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        j = get_charac_job();
                        pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                WongWork::CSkillChanger changer;
                changer.SkillInitialize(this, 0, false);
                send_skill_info();
                CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    if (lethe != 0)
                    {
                        lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)0);
                        lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)0);
                    lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)1);
                }
                ok = true;
            }
            else
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x4aee,
                    "User %s - CUser::set_grow_type %d", getCurCharacName(), (int)type);
                ok = false;
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(treeKind);
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            bool firstTree = ((GetCurCharacSkillTreeIndex() == -1) ||
                              (GetCurCharacSkillTreeIndex() == 0));
            if (firstTree)
            {
                addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot,
                                          get_charac_job());
            }
            else
            {
                addSkillOnCreateCharacter(reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->m_skillSlot2,
                                          get_charac_job());
            }
            std::vector<std::pair<int,int> > giveVec;
            std::pair<int,int> pr;
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int job2 = get_charac_job();
            bool got = G_CDataManager()->m_characList->m_characs[job2].
                           get_give_skill((int)firstGrow, (int)secondGrow, giveVec, 0);
            if (got)
            {
                // [A06 2026-09-10 轴B 试行记录·已回退]（同上：本处反劣）
                for (unsigned int i = 0; i < giveVec.size(); ++i)
                {
                    ENUM_SKILL_TREE_KIND kindI =
                        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
                    int j = get_charac_job();
                    pr = giveVec[i];
                    ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, pr.second, kindI);
                }
                STExpertJobScript* expert = (STExpertJobScript*)
                    G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
                if (expert != 0)
                {
                    // [A06 2026-09-10 轴B 试行记录·已回退]（同上：本处反劣）
                    for (unsigned int i = 0; i < expert->m_growSkillVec.size(); ++i)
                    {
                        int lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        int j = get_charac_job();
                        std::pair<int,int>& pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)0);
                        lvl = GetCurExpertJobLevel(GetCurCharacExpertJobExp());
                        j = get_charac_job();
                        pr = expert->m_growSkillVec[i];
                        ((SkillSlot*)getCurCharacSkillW())->growtype_skill(j, pr.first, lvl,
                                                                           (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                WongWork::CSkillChanger changer;
                int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                                (GetCurCharacSkillTreeIndex() == 0)) ? 1 : 2;
                changer.SkillInitialize(this, initKind, false);
                send_skill_info();
                CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
                if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    if (lethe != 0)
                    {
                        lethe->UpdateBackupSkillFlag(
                            this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                    }
                }
                else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
                {
                    lethe->InitLetheSkill(
                        this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                }
                ok = true;
            }
            else
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x4b48,
                    "User %s - CUser::set_grow_type %d", getCurCharacName(), (int)type);
                ok = false;
            }
        }
        if (!ok) return;                                     // 失败：仅析构 packet 返回
        break;
    }
    case 0xb:                                                // 公会经验（+0xc92）
    {
        if (get_charac_guildkey() != 0) add_guild_exp(value);
        break;
    }
    case 0xc:                                                // stamina（+0xcb3）
    {
        setCurCharacStamina((unsigned char)value);
        break;
    }
    case 0xd:                                                // event coin（+0xcd9）
    {
        int coinNow = getCurCharacInvenR()->GetEventCoin();
        getCurCharacInvenW()->SetEventCoin(coinNow + value);
        m_historyLog.EventCoinAdd(getCurCharacInvenR()->GetEventCoin(), value,
                                  (eCoinAddReason)2);
        SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 1);
        break;
    }
    case 0x0e:                                               // fatigue 回复（+0xd43）
    {
        RecoverFatigue(value);
        SendFatigue();
        break;
    }
    case 0x0f:                                               // fatigue 扣减（+0xd55）
    {
        int remain = (int)(getCurCharacFatigue() & 0xffff) - (int)value;
        // ORIG:显式 std::max<int>(remain, 0) 调用（_ZSt3maxIiERKT_S2_S2_ 0x8668143）。
        remain = std::max(remain, 0);
        setCurCharacFatigue(remain);
        SendFatigue();
        break;
    }
    case 0x10:                                               // 公会经验上报（+0xdd8）
    {
        if (get_charac_guildkey() != 0)
        {
            unsigned int characNo = get_charac_no(-1);
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            proxy->SendIncreaseGuildExp((unsigned int)get_charac_guildkey(), characNo,
                                        value, 1);
        }
        break;
    }
    case 0x11:                                               // SFP（+0xe64）
    {
        gain_sfp(value);
        history_log_sfp(-1, value, (eSPAddReason)1);
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(1);
        packet.put_short(slotNo);
        packet.put_byte(type);
        packet.put_int(value);
        packet.put_short(0);
        packet.put_short(0);
        packet.finalize(true);
        Send(packet);
        return;
    }
    case 0x12:                                               // SFP 技能树重建（+0xf12 区）
    {
        ((SkillSlot*)getCurCharacSkillW())->clear_sfp_skills(
            (char)get_charac_job(),
            (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
        ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
        // ORIG:initKind 判定重新调用两次（不缓存 treeKind）。
        int initKind =
            ((GetCurCharacSkillTreeIndex() == (ENUM_SKILL_TREE_KIND)-1) ||
             (GetCurCharacSkillTreeIndex() == (ENUM_SKILL_TREE_KIND)0))
                ? 3
                : 4;
        WongWork::CSkillChanger changer;
        changer.SkillInitialize(this, initKind, false);
        send_skill_info();
        break;
    }
    case 0x13:                                               // power war point（+0x1180）
    {
        IncreasePowerWarPoint((short)value);
        break;
    }
    case 0x14:                                               // GM 状态校正（+0x119c）
    {
        adjust_charac_stat();
        if (m_chState == 3)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            Send(packet);
        }
        break;
    }
    case 0x15:                                               // coin（+0x125b）
    {
        int coinNow = getCurCharacInvenR()->GetCoin();
        getCurCharacInvenW()->SetCoin(coinNow + value);
        m_historyLog.CoinAdd(getCurCharacInvenR()->GetCoin(), value, (eCoinAddReason)3);
        SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 1);
        break;
    }
    case 0x16:                                               // PvP 技能树（+0x1303）
    {
        if (value == 1)
        {
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            givePvPSkillTree(0, true, 3);
            int pvpGrade = get_pvp_grade();
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                get_charac_job(), (int)firstGrow, (int)secondGrow, pvpGrade, false);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(point,
                                                                       (ENUM_SKILL_TREE_KIND)0);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(point,
                                                                       (ENUM_SKILL_TREE_KIND)1);
            send_skill_info();
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                if (lethe != 0)
                {
                    lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)0);
                    lethe->UpdateBackupSkillFlag(this, (ENUM_SKILL_TREE_KIND)1);
                }
            }
            else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)0);
                lethe->InitLetheSkill(this, (ENUM_SKILL_TREE_KIND)1);
            }
        }
        else
        {
            ENUM_SKILL_TREE_KIND treeKind =
                (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex();
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills(treeKind);
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            int initKind = ((GetCurCharacSkillTreeIndex() == -1) ||
                            (GetCurCharacSkillTreeIndex() == 0)) ? 1 : 2;
            givePvPSkillTree(0, true, initKind);
            int pvpGrade = get_pvp_grade();
            char secondGrow = getCurCharSecondGrowType();
            char firstGrow = getCurCharFirstGrowType();
            int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                get_charac_job(), (int)firstGrow, (int)secondGrow, pvpGrade, false);
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
                point, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
            send_skill_info();
            CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
            if (!isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                if (lethe != 0)
                {
                    lethe->UpdateBackupSkillFlag(
                        this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
                }
            }
            else if ((lethe != 0) && isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
            {
                lethe->InitLetheSkill(
                    this, (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
            }
        }
        break;
    }
    case 0x17:                                               // one day lethe（+0x1800）
    {
        CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
        // ORIG:单表达式链(实参 treeIdx/charNo/name 先于 cMyTrace 构造求值)。
        cMyTrace(__PRETTY_FUNCTION__, 0x4c0a, 0)(
            "ONE_DAY_LETHE : BUY_SKILL_CONFIRM_TICKET, char(%s), char_no(%d), style(%d)",
            getCurCharacName(), getCurCharacNo(), GetCurCharacSkillTreeIndex());
        if (lethe != 0)
        {
            lethe->ConfirmSkillReq(this);
        }
        break;
    }
    default:
        break;
    case 0x19:                                               // avatar coin（+0x13e1）
    {
        AvatarCoin* avatarCoin =
            &reinterpret_cast<CInventory*>(getCurCharacInvenW())->m_avatarCoin;  // +0x658
        avatarCoin->Add(value);
        AvatarCoin::SaveToDB(this);
        AvatarCoin::SendSyncPacket(this);
        AvatarCoin::HistoryLog::AddLog(this, value);
        break;
    }
    }

L_common_tail_6751_end_return:
    // ---- 公共回包与队伍/战房广播（ORIG +0x2c39 尾段）----
    packet.clear();
    packet.put_header(1, 0x20);
    packet.put_byte(1);
    packet.put_short(slotNo);
    packet.put_byte(type);
    packet.put_int(value);
    packet.put_short(0);
    packet.put_short(0);
    packet.finalize(true);
    Send(packet);

    if (m_chState == 5)
    {
        CParty* party = (CParty*)GetParty();
        if (party != 0)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            party->send_to_party(packet);
        }
    }
    else if (m_chState == 8)
    {
        WarRoom* warRoom = (WarRoom*)GetWarRoom();
        if (warRoom != 0)
        {
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(1);
            packet.put_short(1);
            make_basic_info((char*)&packet, (char)1);
            packet.finalize(true);
            warRoom->SendToRoom(packet);
        }
    }
