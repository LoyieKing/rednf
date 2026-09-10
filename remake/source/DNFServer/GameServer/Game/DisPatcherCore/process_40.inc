int DisPatcher_DieCharacInDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // ORIG 0x081c2a1a `mov 0x14(%ebp),%eax; mov %eax,-0x1c(%ebp)`：param 槽在函数头；
    // 0x081c2a20 `mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)`：check_error 的 msg 直压栈
    // （此时 msg 尚无具名槽）。
    stDieCharacInDungeon& out = static_cast<stDieCharacInDungeon&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xcbb, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // [DPC_Process] ORIG 0x081c2a91 `mov 0x10(%ebp),%eax; mov %eax,-0x18(%ebp)`：msg
    // 具名视图槽（errNo 检查之后声明，占 -0x18）；随后 0x081c2a97/0x081c2aa1
    // `mov -0x18(%ebp),%eax; movzwl 0xf(%eax),%eax`（+0xf 先入 esi）、
    // `movzwl 0xd(%eax),%eax`（+0xd 后入 ebx，实参右→左）——**直偏移单步寻址**，
    // 旧文 `->m_pad + 2` 使 4.4 产 `add $0xd,%eax; add $0x2,%eax; movzwl (%eax)` 三步。
    #pragma pack(push, 1)
    struct DieCharacReqView
    {
        char m_head[0xd];
        unsigned short m_0d;   // +0x0d（CharacDie 第 2 参，零扩展 movzwl）
        unsigned short m_0f;   // +0x0f（CharacDie 第 3 参）
    };
    #pragma pack(pop)
    const DieCharacReqView& req = *reinterpret_cast<const DieCharacReqView*>(&msg);
    user->m_historyLog.CharacDie(
        user->getCurCharacName(),
        req.m_0d,
        req.m_0f);
    // ORIG thunk：0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、0x8294f32=GetParty(int)
    CParty* partyByIdx = G_CGameManager()->GetParty(user->GetPartyIndex());

    // [J5-ProcD 2026-09-11 核账] 本轮 3 处试形（①`!=0` 早返回 ②`>0` 替 `>=1`
    // ③then 尾独立 `return 0`）**已实测劣化 + 全量回滚**：ours 123→**126（+2/+3）**，
    // ORIG 119。ORIG 0x081c2b00 `test %al,%al; je +0xfc` 与 0x081c2b60 `jle +0x173`
    // 的布局虽为直证，但 4.4 -O0 在本处三旋钮叠加后整体劣化 ⇒ **勿再试本组合**；
    // 恢复旧形 `if (… == 0) { 主体 }`（记 N）。
    if (Global::CBossTowerFunc::DieCharacInDungeon(user) == 0)
    {
        if (user->get_state() == 0xd)
        {
            advancealtar::Manager::dieCharac(user);
        }

        CParty* myParty = static_cast<CParty*>(user->GetParty());
        // [DPC_Process] ORIG 0x081c2b34 `call GetParty; mov %eax,-0x10(%ebp)` 后
        // 0x081c2b3c `cmpl $0x0,-0x10(%ebp)`/`je`，再 0x081c2b42
        // `mov -0x10(%ebp),%eax; mov 0xcac(%eax),%eax; mov %eax,-0xc(%ebp);
        // cmpl $0x0,-0xc(%ebp)` —— -0xc 是 **m_dungeon 值**的具名槽（无零初始化），
        // 非 view 指针槽（旧文 `view = (CPartyDungeonView*)myParty` 多 store 且
        // 使 0xcac 解引用两次：+2 条）。
        CDungeon* myDungeon;
        // [DPC_Process] ORIG 0x081c2b3c/0x081c2b4e/0x081c2b60 三跳全落 0x081c2b85
        // （die_user 段）：`cmpl $0x0,-0x10/je`、`cmpl $0x0,-0xc/je`、
        // `test %al,%al; jle` ⇒ 原码为 **`&&` 链** `party!=0 && dungeon!=0 &&
        // (signed char)type>=1`（任一假 → 跳 else），Blood 体为 fall-through；
        // 旧文 `||` 链 + 取反次序使 4.4 产 `jg <Blood>`（块布局相反）。
        // [J5-ProcD 2026-09-11 核账] `> 0` 试形随本件一并回滚（整体 +2 劣化）：
        // ORIG 0x081c2b60 `test %al,%al; jle +0x173` 为单向跳转直证，但 4.4 在本处
        // 无论 `>= 1` 还是 `> 0` 都产 `mov $1/$0` 布尔物化 ⇒ 属 4.4/ORIG codegen 族，
        // **勿再试**；恢复 `>= 1`。
        if (myParty != 0 &&
            (myDungeon = reinterpret_cast<CPartyDungeonView*>(myParty)->m_dungeon) != 0 &&
            static_cast<signed char>(myDungeon->m_bloodDungeonType) >= 1)
        {
            myParty->BloodHandleDieUser(user);
            out.m_errNo = 0x7fffffff;
        }
        else
        {
            if (partyByIdx != 0)
            {
                partyByIdx->die_user(user);
            }
        }
    }
    return 0;
}
