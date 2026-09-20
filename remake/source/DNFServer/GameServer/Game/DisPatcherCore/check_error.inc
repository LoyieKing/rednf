// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 225（ORIG 267 / ours 284），残差=P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [M8BigTURest/M8DPB 2026-09-11 签名轴] ORIG .rodata 直证
// `int DisPatcher_SelectCharac::check_error(CUser*, MSG_BASE&)`（原写 unsigned int）。
int DisPatcher_SelectCharac::check_error(CUser* user, MSG_BASE& msg)

{
    if (user->get_state() != 2)
    {
        return 0x7fffffff;
    }
    if (user->isLockedCreateCharac() != false)
    {
        return 0x13;
    }
    if (user->isHumanCertified() != true)
    {
        return 0x7fffffff;
    }
    MSG_CHARAC_SELECT& sel = reinterpret_cast<MSG_CHARAC_SELECT&>(msg);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        // [G5-MiscC] P0/P52：ORIG 0x081c68ed `call getOnlinePreliminaryCharacNo;
        // mov %eax,0x4(%esp); mov 0xc(%ebp),%eax; mov %eax,(%esp); call get_charac_idx`
        // —— 预选角色号作实参直传，无 uVar6 具名槽（我方 fresh 多 -0x1c 槽）⇒ 内联。
        sel.m_slot = static_cast<signed char>(
            user->get_charac_idx(user->getOnlinePreliminaryCharacNo()));
        // [b28e-DispCore] P225/P295：ORIG 0x081c68f? 为 `cmp $0xff,%al; sete %al;
        // test %al,%al; je <继续>`（4.1 bool 四段物化）而非 4.4 直跳 `jne`
        // ⇒ 试 while+break/return 形复现物化（P225 实证该形出 setne/test/je）。
        while (static_cast<unsigned char>(sel.m_slot) == 0xff)
        {
            return 0xffffffff;
        }
    }
    if (checkPvpEnterCharacter(user, sel) != true)
    {
        return 0xffffffff;
    }
    if (('#' < static_cast<char>(sel.m_slot)) || (static_cast<char>(sel.m_slot) < '\0'))
    {
        // [G5-MiscC] P0/P52：ORIG 0x081c6963-0x081c696a 仅 `movzbl 0xd(%eax),%eax;
        // movsbl %al,%ebx`（值在 callee-saved %ebx 跨 cMyTrace ctor 保活），
        // 无 char 具名槽（我方 fresh 多 -0x15）⇒ 内联回 varargs 实参位。
        // [b28e-DispCore] P227：ORIG 的 `movsbl %al,%ebx` 预载在 ctor **之前**
        // ⇒ 原码是全表达式临时链（4.4 亦把 op() 实参在 ctor 前求值入 %ebx）；
        // 两语句形把 slot 求值推到 ctor 之后（实测改后该段行序与 ORIG 一致）。
        cMyTrace(__PRETTY_FUNCTION__, 0x16c4, 5)(
            "pUser->SelectCharac() fail, %d out of range", static_cast<int>(sel.m_slot));
        return 4;
    }
    if (user->m_characList.size() <= static_cast<unsigned char>(sel.m_slot))
    {
        return 0xfffffffe;
    }
    std::string local_38(user->m_characList[static_cast<int>(sel.m_slot)].m_name);
    // [FL4] P0/P198②：ORIG 0x081c6a5d-0x081c6a87 该 bool 无栈槽（`mov $0x1,%eax; jmp;
    // mov $0x0,%eax; test %al,%al; je` 寄存器物化后直测）⇒ 原码是**单条 if（内联短路表达式）**，非
    // 「bool 具名 + 二次 if」；旧形多发 `movb $0/$1,-0x21; cmpb $0,-0x21` 四条槽往返。
    // [DPC_CheckErr] P222 极性轴：ORIG 0x081c6a4d `call HasSlangName; test %al,%al;
    // jne 0x081c6a79`（真岛 `mov $0x1,%eax`，HasSlang 真亦直落该岛）+ 0x081c6a72
    // `je 0x081c6a80`（假岛 `mov $0x0,%eax`）⇒ 原码是**正极性** `A || B` 包一层 `!`
    // （我方原 `!A && !B` 德摩根形多发两条 `xor $1`，ORIG 无 xor）。
    // [H5 核账 2026-09-11] ours_insns 270→270（verdict 未变）：4.4 对 `!(A||B)` 与
    // `!A && !B` 恒产同一序列（短路后统一 `xor $1` 取反）⇒ 中性。语义等价（德摩根），保留。
    // ORIG 0x081c6a85 test al; je continue; ebx=0x9f — 有 slang 才拒
    if (CSyncSlangFilter::GetInstance()->HasSlangName(local_38) ||
        CSyncSlangFilter::GetInstance()->HasSlang(local_38))
    {
        return 0x9f;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c6a99-0x081c6abf 为
    // `call get_acc_id; mov %eax,0x4(%esp); ... call isGM; movzbl %al,%ebx;
    //  call G_CDataManager; mov %ebx,0x4(%esp); call setCharacNameFilterPass`
    // —— isGM 结果在 %ebx 跨 G_CDataManager() 保活，accId/bVar4b 均无具名槽
    // （我方 fresh 多 -0x1c/-0x15 两槽）⇒ 单语句链式（P52）。
    G_CDataManager()->setCharacNameFilterPass(
        GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()));
    // [DPC_CheckErr] P222/极性轴：ORIG 0x081c6b30 `call hasPreventPrefixCharacName;
    // test %al,%al; je 0x081c6b37`（裸 bool 直判，只测 al）；我方原 `!= false`
    // 多发 `test %eax,%eax; setne %al` 两条 int 提升归一。
    // [H5 核账 2026-09-11] ours_insns 270→270（verdict 未变）：`CDataManager::
    // hasPreventPrefixCharacName` 声明为 int，4.4 在 bool 上下文（if 条件）恒插
    // `test %eax,%eax; setne %al; test %al,%al` ⇒ 去 `!= false` 无法消除 setne
    // （须 .h 改返回 bool，已列建议）。语义等价，保留。
    if (G_CDataManager()->hasPreventPrefixCharacName(
            user->m_characList[static_cast<int>(sel.m_slot)].m_name,
            static_cast<int>(
                strlen(user->m_characList[static_cast<int>(sel.m_slot)].m_name))))
    {
        return 8;
    }
    // [G5-MiscC] P292 档①：ORIG 0x081c6b50 `call checkSelectCharac; xor $1,%eax;
    // test %al,%al; je <继续>` ⇒ 原码是取反 `!`（`== true` 正向形不符）⇒ 反条件早退。
    if (!user->checkSelectCharac(static_cast<int>(sel.m_slot)))
    {
        return 0xfffffffd;
    }
    // [b28e-DispCore] 本件 20→4 后残余 3 处（全 N）：①~②`xor $1` 两处 = 4.4 对
    // `== false` / `!(A||B)` 恒折取反（P168a/P292 档①，需 callee bool 化属他泳道）；
    // ③`mov (%eax),%eax; mov %eax,%ebx` = N18a 单段直落不可造（ORIG `mov (%eax),%ebx`）。
    // [DPC_CheckErr] P222/极性轴：ORIG 0x081c6bcc `call isLockedDeleteCharac;
    // test %al,%al; je <lock4DataLoad 块>`（无 `xor $1`）⇒ 原码是**正极性**
    // `!= false` 显式块（cMyTrace 0x171c 段）+ false 路 fall-through 到
    // lock4DataLoad 块。我方旧形 `== false` 把两块物理序写反（4.4 折 xor $1）。
    // [b28e-DispCore] 翻转后块序 + 消 xor（ORIG 238-248 的 lock4DataLoad 块物理在后）。
    if (user->isLockedDeleteCharac() != false)
    {
        // [G5-MiscC] P0/P52：ORIG 0x081c6b8a `call characList[]; mov (%eax),%ebx`
        // （m_characNo 直读 %ebx，跨 cMyTrace ctor 保活，无 uVar9 具名槽）⇒ 实参位内联。
        // [b28e-DispCore] P227：ORIG 0x081c6bf5 段 `mov (%eax),%ebx`（m_characNo 入
        // %ebx）在 ctor **之前**、且 ORIG 无具名 trace 对象槽（ctor/op() 全表达式链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x171c, 0)(
            "select_charac error during delete charac_no(%d)",
            static_cast<int>(user->m_characList[static_cast<int>(sel.m_slot)].m_characNo));
        return 0x7fffffff;
    }
    // [G5-MiscC] P292 档①：ORIG 0x081c6bdd `call lock4DataLoad; xor $1,%eax;
    // test %al,%al; je <+0x37d>` 后 0x081c6be4 `mov $0xfffffffc,%ebx` / 0x081c6beb
    // `mov $0x0,%ebx` ⇒ `!` 形 + 常量直返。
    if (!user->lock4DataLoad())
    {
        return 0xfffffffc;
    }
    return 0;
}
