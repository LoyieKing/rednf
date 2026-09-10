int Dispatcher_SaveGameOption_ChattingEmoticon::process(CUser* user, MSG_BASE& msgBase, ParamBase& param)
{
    (void)param;
    // ORIG 0x081cf52a：把 msg 的表情槽字符串逐条 strncpy 进 CChattingEmoticonConfig，
    // 与当前 GameOption 不同才 SetEmoticonInfo 并组 SIG_SAVE_CHATTING_EMOTICON 入 DB_Q。
    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON& msg =
        *reinterpret_cast<MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON*>(&msgBase);
    CGameOption* gameOption = user->GetGameOptionRef();
    CChattingEmoticonConfig config = gameOption->getEmoticonInfo();
    for (unsigned int i = 0; i < msg.m_slots.size(); ++i)
    {
        MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String& elem = msg.m_slots[i];
        const char* str = elem.m_str.c_str();
        int slot = static_cast<int>(*reinterpret_cast<short*>(&elem));
        strncpy(reinterpret_cast<char*>(&config) + slot * 0x16, str, 0x14);
    }
    if (!gameOption->isEqualEmoticon(config))
    {
        gameOption->SetEmoticonInfo(config);
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x2959);
        CStreamGuard guard(stream, true);
        // ORIG 0x81cf661：每次 << 前先 operator*（返回 this）——P132 保留 operator* 链。
        (*guard)->operator<<(0x2d7);
        (*guard)->operator<<(static_cast<int>(user->GetUID()));
        SIG_SAVE_CHATTING_EMOTICON* sig = guard->GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
        char* buf = reinterpret_cast<char*>(sig);
        *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
        // ORIG 0x81cf6a2：rep movsl 0x84 dword；4.4 对该类型结构体赋值退化为逐成员
        // 拷贝（+42 条），故保留 memcpy（N78 族，形态不可复现）。
        memcpy(buf + 4, reinterpret_cast<const char*>(&config), 0x210);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
    return 0;
