// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ddd00 orig/ours=89/93 ndiff-blocks=15
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    0x10(%ebp),%eax
int Dispatcher_Mercenary_Competition::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddd00 (0xd4 insn)
    // [g02 试形B] ORIG 0x081ddd17 段之后（两条 guard 之后）才物化 req(-0x10)/result(-0xc)
    //   引用槽（`jmp` 后的 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 一对）⇒ 声明后移；
    //   旧形在函数头物化 ⇒ 槽序前移、我方多 -0x14/-0x10 两槽、帧 0x38 vs ORIG 0x24。
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);

    // [A4 P0] ORIG 0x081ddd61/0x081ddd84/0x081ddd98/0x081dddb8/0x081dddc7/0x081dddd4/
    //   0x081ddde1..0x081dddf5 每处均 `mov -0x10(%ebp),%eax; mov 0x10/0x14/0x15(%eax),%..`
    //   从 req 引用槽直读并**重读**（P48 无 CSE），全文无 targetNo/b0x15/b0x14 槽写入
    //   （我方 -0x18/-0x12/-0x11）⇒ 原码无这三个具名局部。
    if (user->GetParty() != 0 &&
        user->getCurCharacNo() ==
            static_cast<int>(*reinterpret_cast<unsigned int*>(req.m_pad + 3)))
    {
        // [g02 试形B] result 侧裸偏移 → 具名成员（P-g03-b 反向：ORIG `movb $0x0,0x4(%eax)`
        //   单条 disp 折叠；裸 `(char*)&result + 0x04` 恒产 `add $0x4,%eax; movb $0x0,(%eax)`）。
        result.m_isMyCharac = 0;                        // +0x04
        return -1;
    }

    // ORIG: Competition_Mercenary_Charac(user, msg+0x10, (char)msg+0x15, (char)msg+0x14)
    const int ret = user->Competition_Mercenary_Charac(
        *reinterpret_cast<unsigned int*>(req.m_pad + 3),
        static_cast<char>(req.m_pad[8]),
        static_cast<char>(req.m_pad[7]));
    // [g02 试形B] 同上：result 具名成员（ORIG 0x081dddbb/…dc1/…dce/…ddb 全为 disp 折叠形）
    result.m_isMyCharac = static_cast<unsigned char>(ret & 0xff);   // +0x04
    result.m_characNo = *reinterpret_cast<unsigned int*>(req.m_pad + 3);  // +0x08
    result.m_charac = static_cast<unsigned char>(req.m_pad[8]);     // +0x0c
    result.m_charac2 = static_cast<unsigned char>(req.m_pad[7]);    // +0x0d
    // ORIG: cUserHistoryLog::MercenaryCompetition(user->m_historyLog, targetNo, b0x15, b0x14)
    user->m_historyLog.MercenaryCompetition(
        *reinterpret_cast<unsigned int*>(req.m_pad + 3),
        static_cast<char>(req.m_pad[8]),
        static_cast<char>(req.m_pad[7]));
    return 0;
}