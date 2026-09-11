// ORIG T 0x08143ae2：
//   user==0 → 0；CBossPlay::handleLeaveUser 为假 → 0；
//   记 LeaveDungeon 历史（dungeon=m_dungeon(+0x9f0)、stage=m_difficulty(+0xae8)、
//   成员名表、partyType 低字节）；trace "member_count : %d"；
//   无成员 → _destroy 并返回 0；（party 空或全员死）→ onFinishStage +
//   _onPrepareFinishTower(false)；否则按人数 switch 写 m_difficulty(+0xae8)
//   （1..4 → 0..3）；返回 1。
// [b27 DL3R 复核判词 N 封顶] 177:176。已翻案两处（diff 行 135→129）：
//   ① CBossPlay::handleLeaveUser 返回类型 int→bool（ORIG 本函数 0x08143b04
//     `call; test %al,%al; je` 单字节直判 ⇒ callee bool/uchar；callee 行数
//     128:131 int/bool 实测无差，零成本）；头段 `==0`/`!f()` 在 4.4 恒产 xor
//     物形，改正向包裹形（body 入 if 块 + 尾共享 `return 0` 岛）后 xor 消失、
//     je 直达尾岛 fall-through 与 ORIG 同形。② switch 前 `int members =
//     get_member_count()` 具名局部复现 ORIG `mov %eax,-0x1c; mov -0x1c,%eax`
//     槽往返（P0）。残余均为在册 N：成员名表清零展开 1B peel 段+基址 -0x11d
//     奇数槽（N82/P95 帧槽基差）；finishNow 布尔物化 ORIG 落 eax 常量岛无槽
//     vs 4.4 bool 局部必占槽（N82/N18b 族）；`call *%eax` vs ORIG `*%edx`
//     （N7）；m_difficulty 读 ORIG eax→ebx 两步 vs 4.4 直落 ebx（N18a）；
//     histTrace/partyType 槽号 ±1 平移（N82）。
// [b28d-54 N] ORIG 0x08143ae2：成员名表清零展开段槽基差（N82/P95：ORIG
//   -0x11d/0xff vs 4.4 -0x11a/0xfb）+ `call *%eax` vs ORIG *%edx（N7）+
//   finishNow 布尔物化落槽（P197④）；控制流/调用序列全等。
// [b28d-solo-CBossTower] AE（177:177，本波翻正）：①memberNames `""`→`{0}`
//   复现 ORIG 对齐剥离 + rep stos 慢路（P312）；②P226 语句表达式 finishNow；
//   ③CBossStage.h m_difficulty 匿名 union 双视角（P235/P254）+ 实参走
//   m_difficultyE（enum→int 转换 ⇒ 两段 `mov 0xae8(%eax),%eax; mov %eax,%ebx`）。
bool CBossTower::handleLeaveUser(CUser* user)
{
    if (user == 0)
        return 0;
    // [b27 DL3R 形探针 f1] ORIG 0x08143b04 `call; test %al,%al; je <尾部共享
    // mov$0 岛>`（无 xor、je 直达 fall-through 岛）。4.4 对 `==0`/`!f()` 均产
    // xor 物形 ⇒ 改正向包裹：body 落 if 内，`return 0` 收敛为块后岛。
    if (CBossPlay::handleLeaveUser(user))
    {
    // [b28d-solo-CBossTower] P312 试形：槽 -0x11d（非 4 对齐）⇒ `{0}` 应发
    // ORIG 的对齐剥离 + rep stos 慢路（`lea;mov $0xff,%ebx;…and $1`）。
    char memberNames[255] = {0};              // ebp-0x11d（ORIG 对齐剥离慢路）

    const char partyTypeByte =
        static_cast<char>(m_party->m_battleField.m_bloodState);   // ORIG 读 +0xcd8 低字节
    m_party->getMemberNames(memberNames);

    // ORIG 0x08143bb0 段 m_difficulty 读为两段（`mov 0xae8(%eax),%eax;
    // mov %eax,%ebx`）⇒ enum 视角 → int 形参（P265 反向）。
    user->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(), m_difficultyE,
                                    memberNames,
                                    static_cast<int>(partyTypeByte));

    cHistoryTrace histTrace;
    histTrace("member_count : %d", m_party->get_member_count());

    if (m_party == 0 || m_party->get_member_count() < 1)
        {
        _destroy();
        return 0;
        }

    // ORIG 0x08143c1a..0x08143c70：finishNow 物化无槽（mov$0/jmp + mov$1 岛）
    // ⇒ P226 语句表达式包条件（具名 bool 恒槽化 +2 条）。
    if (({ (m_party == 0) ||
           (m_party->get_member_count() > 0 && _checkMemberDie()); }))
        {
        onFinishStage();
        _onPrepareFinishTower(false);
        }
    else
        {
            // ORIG 0x08143cbc `call; mov %eax,-0x1c; mov -0x1c,%eax; cmp $2`：
            // 槽往返 ⇒ 原码具名局部（P0），switch 条件不直调。
            int members = m_party->get_member_count();
            switch (members)
        {
        case 1:
            m_difficulty = 0;
            break;
        case 2:
            m_difficulty = 1;
            break;
        case 3:
            m_difficulty = 2;
            break;
        case 4:
            m_difficulty = 3;
            break;
        default:
            break;
    }
        }
    return 1;
    }
        return 0;
}