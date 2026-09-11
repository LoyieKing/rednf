// ORIG T 0x0855dc64。ORIG 返回 int：失败路径 → 0；成功 → 1。
// [b28d-solo-CLeagueMatch] DIFF 277/277·ndiff 102·真语义 bug 已修 + N95
// （S217 新鲜 .o 口径；p167 29/29 CLEAR）：①0x2c2 守卫
// 原用 theirRoom->get_player_count()，ORIG 0x0855de5e `mov 0xc(%ebp),%eax` +
// Ghidra C `get_player_count(param_1)` 实证为**形参 room** ⇒ 改回 room（真 bug）；
// ②leader/theirRoom 按 ORIG 0x0855dca0/dca7 函数头预清 0（P198③，gap 2→0）；
// ③五处 cMyTrace 改全表达式临时链（P221）翻正槽方向（ORIG -0x6c→-0x2c 由深到浅，
// 原具名形 -0x38→-0x78 反向）；残差为全帧槽基差 8（N95：4.4 槽基恒 ebp-8 起、
// 4.1 可顶到 ebp）与 setg/test 物化差；p167 CLEAR（29/29），语义闭合。
// [b28d-solo-CLeagueMatch·S408] 复验 ndiff 102 不变。本轮实测**回滚**一项：
// ORIG 0x0855e009/e010 双预清（theirTeam=-0x14 / seat=-0x10）的 P191 双声明形
// （`int theirTeam=0; unsigned int seat=0; for(seat=0;…)`）⇒ ndiff 102→121
// （+2 insn，槽次数反劣）⇒ 回滚保持 for 内声明。剩余槽差 2 枚：badSize 具名槽
// （ORIG 无槽，纯寄存器物化，S217 试 P226 语句表达式亦反劣）+ leader/member 槽
// 复用（ORIG 共用 -0x1c(9)，4.4 分开 -0x24(4)/-0x18(3)）⇒ N95+N82 封顶。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮再补两旋钮（均**中性**⇒回滚）：
// ① 循环内三处 `static_cast<int>(seat)` 去除（改为 unsigned→int 隐式转换）⇒
//    278/278、ORIG-only/OURS-only=177/177 逐项不变（4.4 转换 pseudo 等价）；
// ② `int theirTeam` → `unsigned int`（P290 局部 unsigned 升格）⇒ 同上完全不变。
// 残差构成经 -dr 归因复核：N125（lea-vs-shl ×10）+ N95 槽基平移 + trace 立即数拆分
// （P227 全表达式链已用）+ setg/test 物化，无语义缺口【可翻项已试尽】。
// [LeaTimer28Run 2026-09-11 复核 · N126 维持 + .h 待办] ORIG T 0x0855dc64（279 条）vs 我方 278：
// ① 全帧恒定平移 8：ORIG 槽 -0x6c/-0x70/-0x1c/-0x18 vs 我方 -0x74/-0x78/-0x24/-0x20
//    （`lea -0x6c(%ebp),%eax` ↔ `lea -0x74(%ebp),%eax`，逐槽差 8）⇒ N126（4.1 具名局部首槽 -0x4
//    vs 4.4 -0xc）族，非源级可翻（P332）；
// ② ORIG .rodata 串直证 `virtual bool CLeagueMatch::OnMatchRoom(PvP_Room*, int)`（二进制实串），
//    我方发射 `virtual int CLeagueMatch::OnMatchRoom(PvP_Room*, int)`（见 .o .rodata，§6.5 盲区）
//    ⇒ 声明返回类型应为 bool（P174 串定；须 IMatch 基类链同步，属 .h 待办，未落地）。
bool CLeagueMatch::OnMatchRoom(PvP_Room* room, int authValue)
{
    if (m_authValue != authValue) {
        // [b28d-solo-CLeagueMatch] P221：ORIG 五处 trace 槽 -0x6c/-0x5c/-0x4c/
        // -0x3c/-0x2c（由深到浅），具名声明形 4.4 反向（-0x38→-0x78）⇒ 改
        // 「构造即调用」全表达式临时链翻槽方向。
        cMyTrace(__PRETTY_FUNCTION__, 0x295, 0)("OnMatchRoom timeout");
        return false;
    }
    if (m_state != 1)
        return false;

    // [b28d-solo-CLeagueMatch] P198③：ORIG 0x0855dca0/dca7/dcb4 三处预清
    // （Ghidra C：leader=0; theirRoom=0; opponent=0 在 Match 调用前）
    // ⇒ 函数头声明并置 0（双槽置 0 形），后文赋值；补 gap 2。
    CUser* leader = 0;
    PvP_Room* theirRoom = 0;
    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);   // ORIG 丢弃返回值，仅判 opponent==0
    if (opponent == 0) {
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return false;
    }

    leader = opponent->GetUser(0);
    if (leader == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b0, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b0);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return false;
    }

    theirRoom = reinterpret_cast<PvP_Room*>(leader->GetPVPRoom());
    if (theirRoom == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b9, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b9);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return false;
    }

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C `get_player_count(param_1)`
    // + ORIG 0x0855de5e `mov 0xc(%ebp),%eax` 实证）：此守卫检查的是**形参 room**
    // 的人数，非 theirRoom（theirRoom 仅用于下方 badSize）。
    if (room->get_player_count() > (int)nPlayerCountPerOneTeam_) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2c2, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2c2);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return false;
    }

    bool badSize =
        (m_teamA.size() > (int)nPlayerCountPerOneTeam_) ||
        (theirRoom->get_player_count() > (int)nPlayerCountPerOneTeam_);
    // [SweepLeagueMatch4·L11 round1 补充实测] badSize 直接内联进 if（去具名 bool）
    // ⇒ ndiff 79→**111**、insn 277→276（劣化，已回滚）：ORIG 该处虽无槽，但 4.4 的
    // 具名 bool 槽恰与 ORIG 的匿名物化区重合 ⇒ 保留具名形为最优可接受形（N82/P332）。
    if (badSize) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2ca, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2ca);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return false;
    }

    _BackupRoomInfo(theirRoom);
    m_teamB.reset();
    theirRoom->get_user_list(&m_teamB);
    _SendMatchStatePacket(3, 2);
    m_state = 3;
    // [b28d-solo-CLeagueMatch·S408 实测] ORIG 0x0855e009/e010 双预清（-0x14/-0x10）
    // 的 P191 双声明形（theirTeam/seat 前置 + for 再初始化）本轮实测 ndiff
    // 102→121（+2 insn，槽次数反劣）⇒ **回滚**，保持 for 内声明形。
    for (unsigned int seat = 0; seat <= 7; ++seat) {     // setbe 无符号 <=7
        CUser* member = reinterpret_cast<CUser*>(theirRoom->get_user_seat(static_cast<int>(seat)));
        if (member == 0)
            continue;
        int theirTeam = theirRoom->get_team(static_cast<int>(seat));
        _PullOutRoom(member, theirRoom, static_cast<int>(seat));
        int newSeat = _PutInRoom(member, room, false);
        if (theirTeam == 3)
            room->OnSetPVPSeatState(member, newSeat,
                                    static_cast<ENUM_SEAT_STATE>(3),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
    }
    pWaitingRoomList_->DelRoom(this);                   // IRoomList vtable+0x04
    m_state = 4;
    _SendMatchStatePacket(2, 2);
    return true;
}