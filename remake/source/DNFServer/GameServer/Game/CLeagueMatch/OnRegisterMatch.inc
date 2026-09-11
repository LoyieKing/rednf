// ORIG T 0x0855db58。ORIG 返回 int：m_state==1 → 1；失败路径 → 0；成功 → 1。
int CLeagueMatch::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_state == 1)
        return 1;

    if (!bMatchable_) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199), 0x13);
        return 0;
    }

    // ORIG movl $0x0,-0xc 预清（P198③ 声明=0+独立赋值两句式）。
    int err = 0;
    err = _CheckMatchable(room);
    // P225：ORIG `cmpl $0,slot; setne %al; test %al; je <主流程>` bool 四段物化
    // → while+return 形精确复现（if 直判被 4.4 折成 cmp;je 两段）。
    while (err != 0) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    if (m_state != 2)
        _RegisterWaitingRoom(room);

    m_state = 1;
    // N96 清绑：ORIG mov 0x18(%eax),%ebx 跨 get_index call 缓存 m_authValue（P87）；
    // 成员表达式直传（P0 无具名局部）。
    TimerStartMatching::registNextTimer(10, room->get_index(), m_authValue);
    _SendMatchStatePacket(0, 1);
    return 1;
}