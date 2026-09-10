void Dispatcher_Debug_Clear_Quest::send(CUser* user, ParamBase& param)
{
    ReqDebugClearQuestResult& res = static_cast<ReqDebugClearQuestResult&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c9);
    packet.put_byte(1);
    packet.put_short(res.m_index.m_count);
    for (int i = 0; i < res.m_index.m_count; ++i)
    {
        packet.put_short(res.m_index.m_questIdxs[i]);
    }
    packet.finalize(true);
    user->Send(packet);
    user->send_clear_quest_list();

    PacketGuard questList;   // ORIG local_2c：UserQuest 任务列表包
    user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&questList));
    user->Send(questList);
