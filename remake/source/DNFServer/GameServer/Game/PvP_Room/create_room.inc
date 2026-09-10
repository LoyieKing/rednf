// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::create_room（原 TU 第 1186 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// ----------------------------------------------------------------------------
// 房间生命周期
// ----------------------------------------------------------------------------

// [b28d-7 N82] ORIG 0x085d6908：形参 spill 槽 -0x3c/-0x40/-0x44/-0x48，4.4 -0x4c/-0x50/-0x54/-0x24（帧 0x5c vs 0x6c）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（207→198，ORIG 195）：真语义 bug 修复 —— RequestMap 实参
// 应为栈上 short 形参 b + 形参 mode（ORIG `lea -0x40(%ebp)`/`lea 0x28(%ebp)`），旧形传
// 成员 m_requestedMapNo/m_pvpBattleMode；P77/P0 形参 a/b/c 直用（消 4.4 双层复制 6 条）；
// P0 strlen 内联。残余 P211（m_matching int 字段 + (int)void* 异型 store 序）+ N82（local_48 槽）。
// [G8InvenPvP G8PvPAll 2026-09-10] P136/P32 bool 字段直写：`m_isExistPassword = c ? 1 : 0`
// → `m_isExistPassword = c;`。ORIG 0x085d6af1/0x085d6af5 `movzbl -0x44(%ebp),%edx;
// mov %dl,0x604(%eax)` 自形参 home slot 字节直搬、**无 setne 物化**（2 条）；旧三目在
// 4.4 恒产 `test %al,%al; setne %al`（4 条）⇒ 预期本件 -2 条。字段 bool 同宽同偏移。
int PvP_Room::create_room(CUser* user, char a, char* name, short b, bool c,
                          const char* password, int d, PVP_BATTLE_MODE mode)
{
    // [b28d-solo-PvP_Room] P77/P0：ORIG 0x085d6908 的 -0x3c/-0x40/-0x44/-0x48 是形参
    // a/b/c/mode 的 home slot（4.1 形参 spill），后续一律从 spill 槽读（m_mode8←-0x3c、
    // m_requestedMapNo←-0x40、m_isExistPassword←-0x44）⇒ 原码**无具名局部**，直接用形参
    // （旧形多一层局部复制，+6 条）。
    // mode 例外：ORIG 在 a/b/c spill 之后立即 `mov 0x28(%ebp),%eax; mov %eax,-0x48`
    // ⇒ mode 有具名 int 局部（日志/m_pvpBattleMode 均经 -0x48 读）。
    int local_48 = (int)mode;
    // ORIG +0x25..+0x41：三值以 callee-saved（%edi/%esi/%ebx）跨后续调用缓存
    // （P87 register 形），日志首参为 __PRETTY_FUNCTION__ 局部静态（P118/P139）。
    register int roomIdx = m_index;
    register const char* characName = user->getCurCharacName();
    register const char* accName = user->get_acc_name();
    {
        CSwitchLog(__PRETTY_FUNCTION__, 0x43a, 0, 0)(
            "pvp@log %s,%s,room(%d),%d", accName, characName, roomIdx, local_48);
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    // [L10b P211 档③] 同上：返回 int，无 cast。
    m_matching = m_pMatchingSystem->GetMatchingSystem((ENUM_PVP_MATCHING_TYPE)GetMatchingType());
    // [b28d-solo-PvP_Room] 真语义 bug（实参取错对象）：ORIG 0x085d6908 调用点
    // `lea -0x40(%ebp),%edx; mov %edx,0x4(%esp)`（=&local_44 短整局部）+
    // `lea 0x28(%ebp),%edx; mov %edx,0x8(%esp)`（=&mode 第 8 形参）⇒ 原码传
    // 栈上局部/形参引用，非成员 `m_requestedMapNo`/`m_pvpBattleMode`（旧形传
    // this+0x5c4/this+4，被调侧读的是成员地址，语义与 ORIG 不同）。
    ((IMatch*)m_matching)->RequestMap(b,
                                      reinterpret_cast<PVP_BATTLE_MODE&>(mode));
    if (m_state != 0)
    {
        return 0x13;
    }
    m_state = 1;
    m_seats[7] = user;
    m_managerSeat = 0;
    P_USERS(0) = user;
    P_TEAMS(0) = 1;
    m_relayMgr.OnChangeTeam(0, 1);
    m_mode8 = a;
    if (m_mode8 == 0)
    {
        // [b28d-solo-PvP_Room] P0：ORIG 0x085d6908 `call strlen; mov %eax,0x8(%esp)`
        // （结果无槽往返直压实参）⇒ 原码是内联 `strlen(name)`，无具名 len 局部
        // （旧形多 mov %eax,-0x1c / 回读 两条）。
        strncpy(m_roomName, name, strlen(name));
    }
    user->SetPvpIndex((short)m_index);
    m_joinCount = 1;
    m_requestedMapNo = b;
    m_playMapNo = m_requestedMapNo;
    // [G8InvenPvP 2026-09-10 · P136/P32 本件实测中性，已回滚] 曾试
    // `m_isExistPassword = c;`（ORIG 0x085d6aee `movzbl -0x44(%ebp),%edx;
    // mov %dl,0x604(%eax)` 字节直搬形）；编译后本件 ndiff 仍 11、指令 198 不变
    // ⇒ 4.4 仍插 test/setne 物化，非本处形态差 ⇒ 维持三目形。
    m_isExistPassword = c ? 1 : 0;
    if (m_isExistPassword != 0)
    {
        m_passwordLen = d;
        memset(m_password, 0, 9);
        memcpy(m_password, password, m_passwordLen);
    }
    m_relayMgr.OnCreateRoom(m_managerSeat);
    m_deathMatch.OnCreateRoom(m_managerSeat);
    // ORIG `mov 0x28(%ebp),%edx`（形参槽）⇒ 此处用形参 mode，local_48 仅日志用。
    m_pvpBattleMode = mode;
    int ret = 0;
    if ((ret = ((IMatch*)m_matching)->OnCreateRoom(user)) != 0)
    {
        return ret;
    }
    return 0;
}
