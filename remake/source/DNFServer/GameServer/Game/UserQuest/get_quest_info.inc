void UserQuest::get_quest_info(char* buf) const
{
    // b28d-40: N82 封顶——帧/槽平移（-0x54 vs -0x50 等），语义闭合。ORIG 0x086abba8。
    // 【b28e-M1S2 本波复核】136/136；残差逐 hunk 归类 = 实参装载序（ORIG 先装接收者
    //   %eax→(%esp) 后装 %edx→0x4(%esp)；4.4 反之）+ 寄存器中转选择（我方多
    //   `mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)` vs ORIG `mov -0x10(%ebp),%edx;
    //   mov %edx,0x4(%esp)`）+ 数据地址重定位（0x941f7f4 = CTimeGate 单例，AE 盲区）。
    //   P329 核过：本件无中间具名局部（全内联表达式）⇒ 无该旋钮可用；P331 核过：
    //   -0x54 vs -0x50 为 4B 单段平移但该区含 std::list 类对象 ⇒ 明文不可翻 ⇒ N82。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①prett2 缺口 0、strall 串差 0
    //   （仅 base64 假阳性）、p167 本函数 CLEAR；②444 后端变体逐项相同；③本波复核
    //   5 块 13 行差**全为实参装载序 + 寄存器中转**：ORIG 0x086abba8 区段先写常量
    //   栈槽后载接收者（`movl $0x15,0x8(%esp); movl $0x0,0x4(%esp); mov -0x14(%ebp),
    //   %eax; mov %eax,(%esp)`），4.4 恒先载 this/pb（-0x14）再写常量；以及 ORIG
    //   `mov -0x10(%ebp),%edx; mov %edx,0x4(%esp)` 的 %edx 中转 vs 4.4 经 %eax。
    //   P329 已核全内联无具名局部可删 ⇒ 无新旋钮，维持（新=DIFF 136/136 gap=0,
    //   blocks=5）。
    stSelectQuestParam selectParam(m_user);
#if !remake_verify
    cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
        "[DNF-DIAG QPARAM] job=%d grow=%d/%d lv=%d pvp=%d side=%d expert=%d/%d tg_npc=%d tg_q=%d tg_oq=%d tg_open=%d",
        selectParam.m_job, selectParam.m_firstGrow, selectParam.m_secondGrow,
        selectParam.m_level, selectParam.m_pvpGrade, (int)selectParam.m_powerSide,
        selectParam.m_expertJobType, selectParam.m_expertJobLevel,
        GlobalData::s_timeGate_->getNpcIndex(),
        GlobalData::s_timeGate_->getQuestIndex(),
        GlobalData::s_timeGate_->getOpenedQuestIndex(),
        (int)GlobalData::s_timeGate_->isOpen());
#endif

    InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
    pb->put_header(0, 0x15);

    std::list<int> questList;
    G_CDataManager()->m_questList->select_quest(questList, selectParam, m_cleared);
    if (selectParam.m_questCount != -1)
    {
        questList.push_back(selectParam.m_questCount);
    }

    pb->put_byte(m_user->get_charac_level());
    int index = pb->get_index();
    int count = 0;
    pb->put_short(count);

    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); it++)
    {
        int questIdx = *it;
        // ORIG get_quest_info 读取全局 s_timeGate_ 指针（mov 0x941f7f4），
        // 而非把指针变量的地址当作对象；改用 GlobalData::s_timeGate_。
        if (GlobalData::s_timeGate_->setBlindTimeGateStateQuest(questIdx))
        {
            // ORIG 0x086abcb5 `test %al,%al; jne 0x086abccf`：真值走空 then 岛
            // （P223 尾空标签复现 jmp+nop）。
            goto L_skip;
        }
        ++count;
#if !remake_verify
        {
            Quest* q = G_CDataManager()->m_questList->find_quest(questIdx);
            if (q)
                cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
                    "[DNF-DIAG QUESTINFO] id=%d expose=%d type=%d lv=%d-%d lim=%d ahead=%u npc=%d open=%d",
                    questIdx, (int)q->m_exposeQuest, q->m_type,
                    q->m_levelMin, q->m_levelMax, q->m_questCountLimit,
                    (unsigned)q->m_intDataList.size(), q->m_npcIdx,
                    (int)q->IsOpenScheduleQuest());
            else
                cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
                    "[DNF-DIAG QUESTINFO] id=%d MISSING", questIdx);
        }
#endif
        pb->put_short(questIdx);
    L_skip: ;
    }

    pb->put_short(index, count);
    pb->finalize(true);
#if !remake_verify
    cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
        "[DNF-DIAG QPKT] size=%d count=%d questCount=%d",
        pb->get_len(), count, selectParam.m_questCount);
#endif
}