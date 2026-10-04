// ---------------------------------------------------------------------------
// ORIG 0x081c2a12 T _ZN29DisPatcher_DieCharacInDungeon7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_DieCharacInDungeon/process.md 全文转写，含汇编分支次序）。
// m_errNo=check_error；>0 返 0；<0 LineFunc(0xcbb,pretty,err,acc_id) 返回其值；
// ==0：CharacDie(getCurCharacName, msg+0xd, msg+0xf) → GetPartyIndex→G_CGameManager()->GetParty
// 备用队 → CBossTowerFunc::DieCharacInDungeon 为假时：state==0xd 先 advancealtar dieCharac；
// 无队/无 dungeon/dungeon->m_bloodDungeonType(signed)<1：备用队非空则 die_user，返 0；
// [b28d-29] P30/N4 + N82：ORIG 成员偏移两步寻址（movzwl 0xf(%eax) 折叠）vs 4.4 add 链；参数副本槽差，语义集合全等  (ORIG 0x081c2a12 insn 119->126)
// 否则 BloodHandleDieUser + m_errNo=0x7fffffff，返 0。CBossTowerFunc 为真直接返 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 51（ORIG 119 / ours 126），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
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
    const MSG_DIE_CHARACTER& req = *reinterpret_cast<const MSG_DIE_CHARACTER*>(&msg);
    user->m_historyLog.CharacDie(
        user->getCurCharacName(),
        req.m_0d,
        req.m_0f);
    // ORIG thunk：0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、0x8294f32=GetParty(int)
    CParty* partyByIdx = G_CGameManager()->GetParty(user->GetPartyIndex());

    // [K6-DpcProcess 2026-09-11] 拆 J5 的三旋钮叠加，本轮**单旋钮①**：ORIG 0x081c2afb
    // `call DieCharacInDungeon` / 0x081c2b00 `test %al,%al` / 0x081c2b02 `je +0xfc(主体)`
    // / 0x081c2b04 `mov $0x0,%eax` / 0x081c2b09 `jmp +0x190` ⇒ 原码是**正向守卫早返回**
    // `if (f(user)) return 0;`（主体 fall-through），非 `if (f(user)==0){主体}`（旧形产
    // `test %eax,%eax; sete %al; test %al,%al; je 尾` = 多 2 条）。
    // [K6-DpcProcess 第2轮] 第1轮裸形实测 4 条（`test %al,%al; setne %al; test %al,%al; je`）
    // 是因为当时 callee 声明为 char（4.4 对非 bool 返回的 if 条件做 bool 归一化）；
    // 父泳道已落 `BossTowerFunc.h:48` char→bool（证据 0x081c2b00 `call; test %al,%al; je 主体`）
    // ⇒ 按 P168a①「callee 返回 bool 时一律裸形」恢复裸形；ORIG 0x081c2b00 即 2 条直判。
    if (Global::CBossTowerFunc::DieCharacInDungeon(user))
    {
        return 0;
    }
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
    // [K6-DpcProcess 2026-09-11] 单旋钮②：ORIG 三条件各自直跳同一出口
    // （0x081c2b40 `je +0x173`、0x081c2b52 `je +0x173`、0x081c2b60
    // `test %al,%al; jle +0x173`），Blood 体 fall-through；旧的 `a && b && c`
    // 单条件式使 4.4 把整链物化成 bool（`mov $1/%eax … mov $0/%eax; test; je`
    // = 多 5 条）。按 P225 推广拆嵌套 if 复现逐条直跳。
    if (myParty != 0)
    {
        // [K6-DpcProcess 2026-09-11 第2轮读数修正] 第1轮把赋值留在条件里
        // `if ((myDungeon = …) != 0)` 使 4.4 对赋值表达式的值再做 bool 归一化 ⇒
        // `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`（4 条，
        // OURS +0x128 段实测）；ORIG 0x081c2b52 是 `cmpl $0x0,-0xc(%ebp); je`（2 条）
        // ⇒ 赋值必须拆成**独立语句**（P0：ORIG 有 -0xc 值槽 = 原码本就两条语句）。
        myDungeon = myParty->m_battleField.m_pDungeon;
        if (myDungeon != 0)
        {
            // ORIG 0x081c2b5e `test %al,%al` + 0x081c2b60 `jle +0x173`：
            // 有符号字节 >= 1（等价于 >0）单向跳转直证。
            if (static_cast<signed char>(myDungeon->m_bloodDungeonType) >= 1)
            {
                myParty->BloodHandleDieUser(user);
                out.m_errNo = 0x7fffffff;
                // ORIG 0x081c2b6c `mov $0x0,%eax` / 0x081c2b71 `jmp +0x190`：
                // Blood 分支尾是**独立 return**（不是共享尾）。
                return 0;
            }
        }
    }
    if (partyByIdx != 0)
    {
        partyByIdx->die_user(user);
    }
    return 0;
}
