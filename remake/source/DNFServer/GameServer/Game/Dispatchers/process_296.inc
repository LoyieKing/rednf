// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cf52a orig/ours=140/135 ndiff-blocks=31
//   帧差 sub $24c vs $250 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x24c,%esp | mov    %eax,-0x28(%ebp)
// [SweepDispatchers3 2026-09-09] 修真缺口：两次 << 前补 operator* 链（P132，
//   (*guard)->operator<<），p167 MISSING operator*×2 归零；config 拷贝保留 memcpy
//   （ORIG 是 rep movsl 0x84 dword；结构体赋值在 4.4 退化为逐成员 +42 条，已试并回退）。
//   gap 4→1（140 vs 139），残差 = rep movsl vs call memcpy（N78 族）。
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
        // [G5·P157/P48/P183 语义优先件] ORIG 对同一元素发**两次独立 operator[]**：
        //   0x081cf57b（取 m_str 的 c_str 源）与 0x081cf59d（取 m_slot），两次都重发
        //   `mov -0x28(%ebp),%eax; lea 0xf(%eax),%edx; mov -0x20(%ebp),%eax; mov %eax,0x4(%esp);
        //   mov %edx,(%esp); call vector<…_slot_String>::ixEj`；c_str 结果只经
        //   0x081cf58b `mov %eax,%ebx` 跨第二次 operator[] 保活，**无 elem/str/slot 具名槽**。
        //   slot 读法照抄（0x081cf5a2 `movzwl (%eax),%eax; cwtl`）。
        strncpy(reinterpret_cast<char*>(&config) +
                    static_cast<int>(*reinterpret_cast<short*>(&msg.m_slots[i])) * 0x16,
                msg.m_slots[i].m_str.c_str(), 0x14);
    }
    if (!gameOption->isEqualEmoticon(config))
    {
        gameOption->SetEmoticonInfo(config);
        // [R19a01 P259 形态复刻] ORIG 0x081cf62f `call Acquire` 后**直接**把 %eax 压成
        //   CStreamGuard ctor 的第 2 实参（0x081cf63c `mov %eax,0x4(%esp)`），**无具名槽**；
        //   我方原写具名 `Stream* stream = …`（-0x10 槽：1 store + 1 reload = 多 2 条）⇒ 改为内联实参。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x2959), true);
        // ORIG 0x81cf661：每次 << 前先 operator*（返回 this）——P132 保留 operator* 链。
        (*guard)->operator<<(0x2d7);
        (*guard)->operator<<(static_cast<int>(user->GetUID()));
        SIG_SAVE_CHATTING_EMOTICON* sig = guard->GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
        // [G5·P183] ORIG 只有 sig 单槽（0x081cf69d `mov %eax,-0x1c(%ebp)`），随后
        //   0x081cf6ab `mov -0x1c(%ebp),%edx; mov %eax,(%edx)`、0x081cf6b0
        //   `mov -0x1c(%ebp),%eax; lea 0x4(%eax),%ebx`；我方多 buf 槽（-0xc，+2 条）⇒ 删。
        *reinterpret_cast<int*>(sig) = static_cast<int>(user->get_acc_id());
        // ORIG 0x81cf6a2：rep movsl 0x84 dword；4.4 对该类型结构体赋值退化为逐成员
        // 拷贝（+42 条），故保留 memcpy（N78 族，形态不可复现）。
        // [Q4DispatchBig 2026-09-11 负结论·复测回滚] 照抄 CUser.cpp:7630 的等宽
        //   align-4 局部视图范式（`*(EmoticonCfgA4View*)(sig+4) = *(const ...*)&config`）
        //   实测本件 our 138→**143**（gap +1→−4）劣化、TU 四数不变 ⇒ 回退。
        //   与 CUser 场景的差别：此处源 `config` 是 pack(1) 的局部量（align 1），
        //   4.4 需先做对齐剥离，赚不到单级 rep movsl。
        memcpy(reinterpret_cast<char*>(sig) + 4, reinterpret_cast<const char*>(&config), 0x210);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
    return 0;
}