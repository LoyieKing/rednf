void UserQuest::get_quest_info(char* buf) const
{
    // b28d-40: N82 封顶——帧/槽平移（-0x54 vs -0x50 等），语义闭合。ORIG 0x086abba8。
    stSelectQuestParam selectParam(m_user);

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
        pb->put_short(questIdx);
    L_skip: ;
    }

    pb->put_short(index, count);
    pb->finalize(true);
