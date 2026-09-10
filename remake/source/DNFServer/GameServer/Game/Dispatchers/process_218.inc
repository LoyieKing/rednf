int Dispatcher_PcRoomPlayTimeReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e4f5c
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return 3;
    }
    // ORIG 0x081e4f7x：p 先落槽 -0x10（`mov 0x14,%eax; mov %eax,-0x10`），随后三级
    // 查找链全部「call 结果免槽直用」（技巧36/37：不建 dm/mng 中转变量，
    // 否则各占 4B 槽并把 entity 挤到 -0xc 之外）。
    PcRoomPlayTimeRewardParam& p = static_cast<PcRoomPlayTimeRewardParam&>(param);
    EventClassify::CPcRoomPlayTime* entity =
        static_cast<EventClassify::CPcRoomPlayTime*>(
            static_cast<EventClassify::CEventScriptMng*>(
                G_CDataManager()->get_event_script_mng())->get_event_entity(0x5c));
    if (entity == 0)
    {
        p.m_result = 0xb2;
        return 0;
    }
    p.m_result = entity->verifyUserPcRoomPlayEvent(user);
    // ORIG 0x081e4fxx：`test %eax,%eax; je <assign 岛>` + fall 到 `mov $0;jmp`
    // ——赋值块必须跳远（技巧23 goto 钉块序；写成 if{} 被 4.4 折成 jne fall）。
    if (p.m_result == 0)
        goto LPROCESS;
    return 0;
LPROCESS:
    p.m_result = entity->processEventPcRoomPlayTime(user);
    return 0;
