bool CParty::IsEventCharacParty()
{
    // b28d-solo-CParty @0x85b9fd4：ORIG 的条件汇聚值全程寄存器（无 bool 槽）、
    // 且 IsEventing 调用点带 `xor $1`（P292 bool callee）⇒ 源码用短路表达式，
    // 具名 bool 局部会给 -0x11 槽（P0 反证）。P226 语句表达式消槽。
    // b28d-solo-CParty：ORIG 0x085ba092 处 this 直用 `mov -0x10(%ebp),%eax`
    // （无 edx 中转）⇒ 局部指针类型直接是 EventNewCharacterReward*，调用点
    // 不再有 reinterpret_cast（cast 形会令 4.4 提前物化到 %edx）。
    EventNewCharacterReward* reward =
        reinterpret_cast<EventNewCharacterReward*>(GlobalData::s_event_manager->GetRepeatEvent(0xa2));
    // b28d-solo-CParty @0x85b9ffd：P282 槽序还原 —— ORIG `add $0x34,%eax; mov (%eax),%edx;
    // call *%edx` 与 checkFreeRevivalCondition 0x085be466 的 IsEventing 同槽
    //（vptr+0x34）⇒ 真身 = CEventBase::IsEventing(0)。
    if (({ reward == 0 || !reward->IsEventing(0); }))
    {
        return 0;
    }
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x085ba04c：checkValidUser（外部转发 _checkValidUser）
        if (checkValidUser(i) != 1)
            continue;
        // ORIG 0x085ba06f/0x085ba08d：pUser 判空与 isEventCharacter 汇聚到寄存器
        // （mov$1/mov$0 岛），无具名 bool 槽 ⇒ P226 语句表达式。
        if (({ m_member[i].m_pUser != 0 &&
               reward->isEventCharacter(m_member[i].m_pUser->get_charac_job()); }))
        {
            return 1;
        }
    }
    return 0;
}