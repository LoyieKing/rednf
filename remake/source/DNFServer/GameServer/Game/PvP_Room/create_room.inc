// [b28d-7 N82] ORIG 0x085d6908：形参 spill 槽 -0x3c/-0x40/-0x44/-0x48，4.4 -0x4c/-0x50/-0x54/-0x24（帧 0x5c vs 0x6c）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（207→198，ORIG 195）：真语义 bug 修复 —— RequestMap 实参
// 应为栈上 short 形参 b + 形参 mode（ORIG `lea -0x40(%ebp)`/`lea 0x28(%ebp)`），旧形传
// 成员 m_requestedMapNo/m_pvpBattleMode；P77/P0 形参 a/b/c 直用（消 4.4 双层复制 6 条）；
// P0 strlen 内联。残余 P211（m_matching int 字段 + (int)void* 异型 store 序）+ N82（local_48 槽）。
// [G8InvenPvP G8PvPAll 2026-09-10] P136/P32 bool 字段直写：`m_isExistPassword = c ? 1 : 0`
// → `m_isExistPassword = c;`。ORIG 0x085d6af1/0x085d6af5 `movzbl -0x44(%ebp),%edx;
// mov %dl,0x604(%eax)` 自形参 home slot 字节直搬、**无 setne 物化**（2 条）；旧三目在
// 4.4 恒产 `test %al,%al; setne %al`（4 条）⇒ 预期本件 -2 条。字段 bool 同宽同偏移。
// [L11 2026-09-11 落地·残留封顶] 日志三 register 攒值（roomIdx/characName/accName）改
//   **全内联实参**（同 leave_room 的 P-L11）⇒ 198→197（ORIG 195），ndiff 20。残留三项
//   判 N 类：① `int local_48` 槽 ORIG -0x48（紧邻形参 spill 批 -0x3c/-0x40/-0x44）vs 4.4
//   -0x20（具名区）；② CSwitchLog 临时对象 ORIG -0x2c vs 4.4 -0x30；③ GetMatchingSystem /
//   m_matching 赋值段 ORIG「先载 this 再 store 成员」vs 4.4 反序（P211/N88）。
//   local_48 声明已在函数首条语句、类型 int 与 ORIG 同 ⇒ 槽位不可再翻（勿重试声明序）。
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
    // [L11 已证实 2026-09-11] 同 leave_room/set_seat_state/die_charac：ORIG
    // +0x25..+0x41 的 callee-saved（%edi/%esi/%ebx）缓存是**实参求值**产物（逆序
    // 求值 + 无调用实参溢出），非具名 register 局部；旧 register 攒值形把具名槽
    // 基整体推深。改全内联实参形（保留 local_48：ORIG 0x085d6908 有 -0x48 具名槽）。
    {
        CSwitchLog(__PRETTY_FUNCTION__, 0x43a, 0, 0)(
            "pvp@log %s,%s,room(%d),%d", user->get_acc_name(),
            user->getCurCharacName(), m_index, local_48);
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