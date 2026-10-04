// ============================================================================
// DungeonPermission —— ORIG T 0x085b12f8。
// 副本准入结算：逐成员校验等级下限后按当前难度分支记账（diff0→普通 1 次/
// diff1→dm+0x5130 门槛下 2 次/diff2→ultimate 清单内 addDungeonClear），
// 记账成功即发 0x05 准入通知包（short: 1 / dungeonIdx / diff+1）。
// ============================================================================
// [b28d-solo-CParty_KillChain 修法+P167] ORIG 0x085b12f8：本波修法——①删
// dungeonIdx/user/limit/diff 四个具名槽（ORIG 全部表达式直读：0x085b1300/0x085b1371/
// 0x085b13b5/0x085b139f 实证），diff 改 switch(call())（P251）；②门槛改
// KcDataMgrDailyLimitView 具名视图；③diff==1 比较改 P226 语句表达式复现 setle 物化。
// 残余（N 封顶）：帧同 $0x40、指令数 209=209；残差为 i/granted 槽（-0x10/-0x9 vs
// ORIG -0xc/-0xd）与 set find/end/key 槽组整体浅 4（difflib 因重复 call 行对齐放大）；
// 全头部声明（P212）反多一条 movb（210）⇒ N82/N98 槽分配族。P167 CLEAR（29=29）。
void CParty::DungeonPermission(int param)
{
    // P52：ORIG 0x085b1300 先把 m_dungeonIndex 物化 %ebx 跨 G_CDataManager call。
    CDungeon* dungeon = G_CDataManager()->find_dungeon(
        m_battleField.m_dungeonIndex);
    if (dungeon == 0)
        return;

    const int minLevel = dungeon->get_min_level();
    // [M1Free76to90 2026-09-11 · P339] 全头部声明 + 声明序按 ORIG 槽深→浅：
    // ORIG granted=-0xd 先于 i=-0xc（旧形块内声明使 4.4 落 -0x10/-0x9 互换）。
    bool granted;   // 不初始化：ORIG 仅循环体内 $0x0 写（0x085b15bb）
    int i;
    for (i = 0; i <= 3; ++i)
    {
        granted = false;
        if (!_checkValidUser(i))
            continue;
        // P0：ORIG 0x085b1371 无 user 具名槽（直接 this 载入 → call get_charac_level）。
        if (minLevel > static_cast<int>(
                           m_member[i].m_pUser->get_charac_level()))
            continue;

        // P251：ORIG 0x085b139f `cmp $1,%eax; je; cmp $2,%eax; je;
        // test %eax,%eax; jne`（diff 无槽往返）⇒ 源形 switch(call())。
        switch (kcbf(this).get_dungeon_diff())
        {
        case 0:
            granted = m_member[i].m_pUser->AddDungeonClear(
                m_battleField.m_dungeonIndex, 1);
            break;
        case 1:
            // P226：ORIG 0x085b1403 `cmp; setle %al; test %al,%al; je 岛` 物化。
            if (({ (static_cast<int>(
                        G_CDataManager()->m_rankSystem.m_dungeonRank.m_rankLevel[2]) <= param); }))
                granted = m_member[i].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 2);
            break;
        case 2:
            // P167/P183：ORIG 0x085b147e/0x085b1486 两次完整重读
            // getUltimateDungeonList（无指针缓存），0x085b14aa 用 operator==
            // 比较后 jne 跳过授予；ORIG 0x085b1476 调 const 版 set::find。
            if (static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())
                    ->find(dungeon->get_index()) ==
                static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())->end())
                goto DUNGEON_PERM_NEXT;
            granted = addDungeonClear(dungeon, param, i);
        DUNGEON_PERM_NEXT:
            break;
        default:
            break;
        }

        if (granted)
        {
            PacketGuard packet;
            packet.put_header(0, 5);
            packet.put_short(1);
            packet.put_short(m_battleField.m_dungeonIndex);
            packet.put_short(kcbf(this).get_dungeon_diff() + 1);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
}