// ---- OnGiveup（报告，ORIG T 0x0849e60c sz 0x70d）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（N82附/P75 求值序 + 帧槽差）+ 真结构还原。
// 还原：①删除 TU-local SendGiveupError helper（**发明符号**）⇒ 6 处
//   `throw (ENUM_ERROR)<code>` + `catch (ENUM_ERROR err){ SendCmdErrorPacket
//   (0xf3,(uchar)err); }`（P6/P229；p167 修前 MISSING __cxa_throw×6/
//   __cxa_begin_catch/__cxa_end_catch/__cxa_allocate_exception×6/GetCurCharac
//   ExpertJobType/getCurCharacQuestR/vector<pair>::operator[]/… + EXTRA
//   SendGiveupError×5 → 修后 95/95 CLEAR）；②补 jobType==0 独立守卫（ORIG
//   0x0849e6d0，第二次独立调用取 GetExpertJobScript 实参）；③get_quest_info
//   receiver 改 getCurCharacQuestR（ORIG 实证，非 QuestW）；④m_growSkillVec
//   每轮**两次** operator[] + 两次 getCurCharacSkillW（P48）；⑤删具名 inven
//   局部（P0，getCurCharacInvenW() 结果不落槽）；⑥两循环改 P283 goto 尾标签形
//   （ORIG begin/end 后无条件 jmp 底部条件块，for 恒条件置顶）。
// 残差：4.4 实参右→左求值序 vs ORIG 左→右（skillIdx 先于 get_charac_job，
// P75/N82附）、帧 0x50 vs ORIG 0x60、PacketGuard 槽 -0x34 vs -0x30；
// 指令数 460 vs 458，调用点 95/95 CLEAR，语义完成。
void CExpertJobMgr::OnGiveup(CUser* user)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
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
        // [L3Free25to36 2026-09-11 **真语义 bug 修复**] 实参序为 (job, skillIdx)：
        //   ①ORIG 0x0849e9ef-0x0849ea0f：`mov (%eax),%esi`（先取 vector[i].first）
        //     → `call get_charac_job; mov %eax,%ebx` → `call getCurCharacSkillW`
        //     → `movl $0,0xc(%esp); mov %esi,0x8(%esp); mov %ebx,0x4(%esp)`，
        //     即 param1=job(%ebx)、param2=skillIdx(%esi)；
        //   ②callee 语义直证：`_ZN10CSkillList10find_skillEii` 0x08351b74
        //     `cmpl $0xa,0xc(%ebp); jle → return 0` + `param1*0x14 + this` 取 hash_map
        //     组、`hash_map.find(&param2)` ⇒ **param1 = 职业组号(job)**，param2 = 技能 id；
        //     全仓其余调用点亦均为 `find_skill(get_charac_job(), skillId)`。
        //   ⇒ SkillSlot.h:290 / SkillSlot.cpp:925 的形参命名 (skillIdx, job) 与 ORIG
        //     相反；本 TU 按 ORIG 实参序调用（我方链 find_skill(job, idx) 与 ORIG 同效）。
        for (unsigned int i = 0; i < (unsigned int)script->m_growSkillVec.size();
             ++i)
        {
            ((SkillSlot*)user->getCurCharacSkillW())->delete_skill(
                user->get_charac_job(), script->m_growSkillVec[i].first,
                (ENUM_SKILL_TREE_KIND)0);
            ((SkillSlot*)user->getCurCharacSkillW())->delete_skill(
                user->get_charac_job(), script->m_growSkillVec[i].first,
                (ENUM_SKILL_TREE_KIND)1);
        }
        user->send_skill_info();

        user->LogHistory("ExpertJob-,%d,%d,%d", user->get_charac_level(),
                         user->GetCurCharacExpertJobType(), giveupCount);
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
}