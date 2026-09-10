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
