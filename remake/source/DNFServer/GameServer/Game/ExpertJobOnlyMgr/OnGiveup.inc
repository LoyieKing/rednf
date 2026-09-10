void CExpertJobMgr::OnGiveup(CUser* user)
{
    try
    {
        if (user->getCurCharacR() == 0)
        {
            throw (ENUM_ERROR)3;
        }
        // ORIG 0x0849e6d0：jobType==0 单独守卫（第二次独立调用取实参）。
        if (user->GetCurCharacExpertJobType() == 0)
        {
            throw (ENUM_ERROR)8;
        }
        STExpertJobScript* script =
            (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(
                user->GetCurCharacExpertJobType());
        if (script == 0)
        {
            throw (ENUM_ERROR)8;
        }

        unsigned int giveupCount =
            GetSelfExpand(user)->get_giveup_count() & 0xffff;
        if ((unsigned int)script->m_giveupCost.size() <= giveupCount)
        {
            throw (ENUM_ERROR)1;
        }
        int giveupCost = script->m_giveupCost.at(giveupCount);
        if (user->getCurCharacMoney() < giveupCost)
        {
            throw (ENUM_ERROR)0x15;
        }

        // 前置校验：[clear quest] 清单全部已达标，任一未清 → 错误 8。
        // b28d-solo（P283）：ORIG begin/end 求值后**无条件 jmp 底部条件块**，
        // 用 goto 尾标签复现（4.4 对 for 恒条件置顶）；两循环复用同一对迭代器槽
        // （ORIG -0x20/-0x24 实证）。
        std::vector<int>::iterator it = script->m_clearQuests.begin();
        std::vector<int>::iterator itEnd = script->m_clearQuests.end();
        goto clearCheckCond;
    clearCheckBody:
        if (user->getCurCharacQuestRefR()->isClearQuest(*it) != true)
        {
            throw (ENUM_ERROR)8;
        }
        ++it;
    clearCheckCond:
        if (it != itEnd)
        {
            goto clearCheckBody;
        }

        it = script->m_clearQuests.begin();
        itEnd = script->m_clearQuests.end();
        goto clearResetCond;
    clearResetBody:
        user->getCurCharacQuestW()->resetClearQuest(*it);
        ++it;
    clearResetCond:
        if (it != itEnd)
        {
            goto clearResetBody;
        }

        PacketGuard packet;
        // ORIG 0x0849e915：getCurCharacInvenW() 结果**不落槽**（P0 无具名局部），
        // 直接作 receiver 调 use_money（具名 inven 会多占一槽致全帧下移 4）。
        user->getCurCharacInvenW()->use_money(giveupCost, (eMoneySubReason)0x1e,
                                              true);

        packet.clear();
        packet.put_header(0, 0xe);
        packet.put_byte(0);
        packet.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, packet);
        packet.finalize(true);
        user->Send(packet);

        // 删除双树技能（[skill] 表 first = skillIdx）。ORIG 每轮**两次**
        // operator[] + 两次 getCurCharacSkillW（P48 逐点重取，无具名 skillIdx）。
        for (unsigned int i = 0; i < (unsigned int)script->m_growSkillVec.size();
             ++i)
        {
            ((SkillSlot*)user->getCurCharacSkillW())->delete_skill(
                script->m_growSkillVec[i].first, user->get_charac_job(),
                (ENUM_SKILL_TREE_KIND)0);
            ((SkillSlot*)user->getCurCharacSkillW())->delete_skill(
                script->m_growSkillVec[i].first, user->get_charac_job(),
                (ENUM_SKILL_TREE_KIND)1);
        }
        user->send_skill_info();

        user->LogHistory("ExpertJob-,%d,%d,%d", user->get_charac_level(),
                         user->GetCurCharacExpertJobType(), (int)giveupCount);
        user->SetCurCharacExpertJobType(0);
        user->SetCurCharacExpertJobExp(-1);

        // ORIG 0x0849ead5 用 getCurCharacQuestR（非 QuestW）调 get_quest_info。
        user->getCurCharacQuestR()->get_quest_info(
            reinterpret_cast<char*>(&packet));   // 复用包缓冲
        user->Send(packet);

        ++giveupCount;
        if ((unsigned int)script->m_giveupCost.size() <= giveupCount)
        {
            giveupCount = script->m_giveupCost.size() - 1;
        }
        ((charac_expand::CData*)user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0))->reset();
        GetSelfExpand(user)->set_giveup_count((unsigned short)giveupCount);

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);

        packet.clear();
        packet.put_header(1, 0xf3);
        packet.put_byte(1);
        packet.put_int(user->getCurCharacMoney());
        packet.put_byte((int)giveupCount);
        packet.finalize(true);
        user->Send(packet);
    }
    catch (ENUM_ERROR err)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xf3, (unsigned char)err);
    }
