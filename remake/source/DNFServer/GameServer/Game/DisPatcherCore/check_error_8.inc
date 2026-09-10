unsigned int DisPatcher_StartGame::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // [DPCB 实验 2026-09-11] 条件极性轴：ORIG 0x081c9f7e/0x081c9f81 `cmp $0x3,%eax;
    // je 0x081c9f9a`（=false 块）+ 0x081c9f8e `cmp $0xb,%eax; je 0x081c9f9a` 两跳同指
    // **false** 岛、fall 出 `mov $0x1,%eax`（0x081c9f93）⇒ 原码是德摩根形
    // `state != 3 && state != 0xb` 的物化，而非 `== 3 || == 0xb`（我方旧形给真岛，
    // 两处 jcc 极性反转）。
    if (user->get_state() != 3 && user->get_state() != 0xb)
    {
        return 0x7fffffff;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c9fad 只有**单**具名槽 -0x18
    // （`mov 0x14(%ebp),%eax; mov %eax,-0x18(%ebp)` 直存 param 本身，访问形
    //  `mov -0x18(%ebp),%eax; movl $0xffffffff,0x8(%eax)` = param+0x8）。
    // 我方原 `stStartGameParam& paramRef` + `int* local_18 = &paramRef.m_8` 是
    // 引用+指针两个具名（fresh 多 -0x38/-0x34 两条 mov）⇒ 合并为单个指针具名。
    stStartGameParam* local_18 = static_cast<stStartGameParam*>(&param);
    if (G_GameWorld()->IsPVPChannel() != false)
    {
        local_18->m_8 = 0xffffffff;
        return 8;
    }
    if (user->IsBusyDisjointStore() != false)
    {
        local_18->m_8 = 0xffffffff;
        return 0x13;
    }
    if (user->isCompetitionMercenary() != false)
    {
        local_18->m_8 = 0xffffffff;
        return 0xea;
    }
    CParty* pCVar5 = static_cast<CParty*>(user->GetParty());
    if (pCVar5 != 0)
    {
        // [G5-MiscC] P0/P183：ORIG 0x081ca032 与 0x081ca049 **各自重发**全链
        // `mov -0x14(%ebp),%eax; mov %eax,(%esp); call GetPartyTelePort;
        //  mov %eax,(%esp); call get_teleport_state`，teleport/state 均无具名槽
        // （我方 fresh 多 -0x28/-0x22/-0x21 三槽）⇒ 内联回链式右值。
        // 判据取 ORIG 极性：0x081ca047 `test %al,%al; js`（<0 为假）+
        // 0x081ca05c `cmp $0x3,%al; jg`（>3 为假）⇒ `>= 0 && <= 3`。
        if (static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                    ->get_teleport_state() >= '\0' &&
            static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                    ->get_teleport_state() <= '\x03')
        {
            // ORIG 0x081ca070 / 0x081ca083 同样各自重发全链（P183）。
            static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                ->reset_teleport_data();
            static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                ->send_teleport_status('\0');
        }
    }
    // [G5-MiscC] P0/P52：ORIG 0x081ca0a4 / 0x081ca0c0 / 0x081ca0fb 三处均为
    // `mov 0xc(%ebp),%eax; mov %eax,(%esp); call getCurCharacName; mov %eax,%ebx;
    //  call G_GameWorld; ...` —— pcVar7 无槽（我方 fresh 多 -0x2c），
    // GetLeftTimeOfForbiddenMove 结果亦无槽（0x081ca0de 直接
    // `test %eax,%eax; setne %al`，我方 fresh 多 -0x20）⇒ 全部内联回实参位
    // （P52：callee-saved 寄存器跨调用保活）。
    if (G_GameWorld()->IsForbiddenMove(user->getCurCharacName()) != false)
    {
        // [b28e-L5] C05④ 步骤2 实测**不命中**（已回退）：ORIG 0x081ca0de 为
        // `test %eax,%eax; setne %al; test %al,%al; je`（body 以 `return 0x44` 终止、
        // 表面符合「含 return」的 P225 命中前件），但整块换 `while (cond != 0)
        // { body; break; }` + 去具名 iVar3 后 our_insns **246→249**（ORIG 228）劣化
        // ⇒ 本件残差主体是 N82 帧差（0x34 vs 0x58）+ 条件极性折痕，保持原形。
        if (G_GameWorld()->GetLeftTimeOfForbiddenMove(user->getCurCharacName()) != 0)
        {
            local_18->m_8 = 0;
            return 0x44;
        }
        G_GameWorld()->EnableMove(user->getCurCharacName());
    }
    // [P5-DPC 2026-09-11] ORIG 0x081ca119 `movl $0x0,-0x14(%ebp)`（紧随 EnableMove 的
    // call、且在 CheckInParty 调用序列之前）⇒ 原码此处把 party 指针槽重新置 0（P183
    // 赋值轴）。旧形缺该 store（diff `delete orig[120:121] ours[120:120]`）。
    pCVar5 = 0;
    // [DPC_CheckErr·H5核账 2026-09-11] 试形（裸调用替换 `!= false`）实测
    // ours_insns 230→231 劣化，按 codegen 轴纪律回滚，留 N。
    // [DPC-SigSmall 2026-09-11] 短路轴（SetPVPMapIndex 同族手法）：ORIG
    // 0x081ca14x `call CheckInParty; ...; call get_state; cmp $0x3,%eax; sete %al;
    // test %al,%al; je <tail>` —— 第二条件以 **sete 值物化**（非 0/1 岛）；
    // probe /tmp/dpc_probe2.cpp v4 直证 4.4.6 只在**嵌套 if 内层**出 sete
    // ⇒ 原码为嵌套 if，旧形 `A && B` 产岛（我方 OURS-only mov/jmp/jne）。
    if (user->CheckInParty() != false)
    {
        if (user->get_state() == 3)
        {
            pCVar5 = static_cast<CParty*>(user->GetParty());
            if (pCVar5 != 0)
            {
                // [G5-MiscC] P292 档①：ORIG 0x081ca16f 与 0x081ca181 均为
                // `call; xor $1,%eax; test %al,%al; je <假岛 0x081ca18f: mov $0,%eax>`
                // ⇒ 源码是 bool 取反 `!`（原 `== '\x01'` 变体不符，且我原先的
                // if/else 正条件形亦不符）。两跳同落假岛 ⇒ 判据 = `!A && !B`，
                // 0x081ca194 `test %al,%al; je 0x240` 即假（A 真或 B 真）跳过 0x15。
                if (!pCVar5->CheckMemberArea() && !pCVar5->is_quick_party())
                {
                    local_18->m_8 = 0xffffffff;
                    return 0x15;
                }
                // [G5-MiscC] P0/P52：ORIG 0x081ca1c2 的 getHackAnalyzer 结果只在
                // %eax 上跨实参布局（0x081ca1cd-0x081ca1f4 先直填 0x14/0x10/0xc/0x8(%esp)，
                // 最后才 `mov %eax,(%esp)` = this 最后求值），无具名槽 ⇒ 内联链式。
                // [K6-DpcSigSmallCheckErr] 改前形 `excludeAddHackCount(user) != false`
                // → 改后形裸调用。依据：K6 父泳道已把返回类型 int→bool（ORIG 调用点
                // 0x081ca1be `call; test %al,%al; je` P292 档①），保留 `!= false` 会继续
                // 生成 `test %eax,%eax; setne %al; test %al,%al` 三步归一。
                if (excludeAddHackCount(user))
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, static_cast<WongWork::ENUM_HACKTYPE>(0x1f6), 1, 0, 0);
                }
                // [DPC-SigSmall 2026-09-11 二轮·负结论] ORIG 0x081ca1f9 的
                // `movl $0x0,-0x14(%ebp)` 预清虽可复现（ORIG-only 的 movl 项消失），
                // 但本件实测 our 226→227、ndiff 10→11 **劣化**（新预清使槽序列错位）
                // ⇒ 按纪律回滚为单句式（ORIG-only 保留 movl 项）。
                // [DPCB 2026-09-11 重试·负结论] 头部极性轴已修（ndiff 10→6）后在新
                // 基线上重试 ORIG-only `movl $0x0,-0x14(%ebp)`：拆两语句后 our 226→228
                // 且我方在 -0x10 多出同形 store、原 -0x14 项仍缺，ndiff 6→7 **劣化**
                // ⇒ 回滚为单句式。ORIG 该项属 4.1 声明槽物化序（N 类）。
                int iVar3b = G_GameWorld()->GetWorldMapIndex(user);
                if (0 < iVar3b)
                {
                    CWorldMap* this_00 = G_CDataManager()->m_worldMapList.find_world_map(iVar3b);
                    if (this_00 == 0)
                    {
                        local_18->m_8 = 0xffffffff;
                        return 0x15;
                    }
                    // [G5-MiscC] P292 档①：ORIG 0x081ca25a `call; xor $1,%eax;
                    // test %al,%al; je 0x296` ⇒ bool 取反 ⇒ `!IsOpenRevengeDungeon()`
                    // （原 `!= '\x01'` 形不符）。
                    if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
                    {
                        // [G5-MiscC] P0/P52：ORIG 0x081ca261 `call getDungeonMinLevel;
                        // mov %eax,%ebx` + 0x081ca26e `call getMemberMinLevel;
                        // add $0x6,%eax; cmp %eax,%ebx; setge %al` —— 两结果均无槽
                        // ⇒ 内联回比较两侧（右值 getDungeonMinLevel 先求值，与 ORIG 序同）。
                        // [DPC_CheckErr·H5核账 2026-09-11] 试形（翻转比较方向对齐 ORIG
                        // 先求值序 + setge）并入本件试形，实测 230→231 劣化 ⇒ 回滚。
                        // [DPC-SigSmall 2026-09-11 二轮] 在嵌套 if 已落地（225 条）的
                        // 基线上**单独**重试：ORIG 0x081ca261 `call getDungeonMinLevel;
                        // mov %eax,%ebx` + 0x081ca26e `call getMemberMinLevel; add $0x6,%eax;
                        // cmp %eax,%ebx; setge %al` ⇒ LHS=层级（先求值、落 %ebx）。
                        if (this_00->getDungeonMinLevel() >= pCVar5->getMemberMinLevel() + 6)
                        {
                            local_18->m_8 = 0xffffffff;
                            return 0xe;
                        }
                    }
                }
                return 0;
            }
        }
    }
    return 0;
}
