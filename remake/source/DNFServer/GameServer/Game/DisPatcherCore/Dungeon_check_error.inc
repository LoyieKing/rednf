int DisPatcher_UseCoin::Dungeon_check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b28d-solo-DisPatcherCore] P28/P212：ORIG +0x06 `mov 0x10(%ebp),%eax;
    // mov %eax,-0x1c(%ebp)` = msg 视图引用**首个声明**（-0x1c），其后 party0/-0x18、
    // party/-0x14、dungeon/-0xc 逐槽对齐 ⇒ 把 req 提到函数头。
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    (void)param;   // ORIG 第 3 参未消费（coin 读自 msg+0xd）
    // ORIG（report Ghidra C）调 GetParty **两次**：this_00 用于早期 ==0 return -1
    // 且传给 usedCoinDungeonCheck；this_01 用于 dungeon 分支。源不得合并为一次缓存
    // （合并会少一次 call，P196 反例拓扑）。
    CParty* party0 = static_cast<CParty*>(user->GetParty());
    if (party0 == 0)
    {
        return -1;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // [b28d-solo-DisPatcherCore] P157/P183：ORIG `cmpl $0,-0x14; je` 之后才发
    // `mov 0xcac(%eax),%eax`（dungeon 延迟到嵌套 if 内），且 battleField 地址每次
    // 重算 `add $0xb24`（无具名局部槽）⇒ 改嵌套 if + 视图内联表达式。
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;   // +0xcac
        if (dungeon != 0)
        {
            // ORIG：report Ghidra C `(char)this[0x89f] < 1` —— 对 +0x89f 字节做**有符号**
            // 比较（movzbl 载入后 test %al; jle）。字段声明 unsigned char，若直写
            // `m_bloodDungeonType >= 1` 4.4 产 `je`（==0 语义，对 ≥0x80 值分叉），故
            // 按 ORIG 有符号语义 `(int)(signed char)field > 0`（P196 同宽字节符号序）。
            if ((int)(signed char)dungeon->m_bloodDungeonType > 0 &&
                reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.getBloodState() >= 3)
            {
                return 0x13;
            }
            if (dungeon->isTournamentDungeon() && party->GetTournamentDungeonClearState() >= 1)
            {
                return 0x13;
            }
        }
    }
    int err = usedCoinDungeonCheck(user, party0);
    // [b28d-solo-DisPatcherCore] P16 早返形：ORIG `cmpl $0,-0x10; je L_body;
    // mov -0x10,%eax; jmp L_ret`（err!=0 直返）+ 内层 `jle L_skip; mov -0x10,%eax;
    // jmp; L_skip: mov $0,%eax`。
    if (err != 0)
    {
        return err;
    }
    // ORIG 0x081ca73c `mov -0x18(%ebp),%eax` ⇒ ReviveUser 的 this 是**第一次**
    // GetParty 的返回值（party0，与 usedCoinDungeonCheck 同源；Ghidra C 的 this_01
    // 映射有误，以 ORIG 汇编为准）。
    err = party0->ReviveUser(user, req.m_0d);   // ORIG movzwl msg+0xd
    if (err > 0)
    {
        return err;
    }
    return 0;
