// ---- handlePremiumTimeout —— ORIG T 0x086afa7c (handlePremiumTimeout.md) ----
// CheckPremiumTimeout 返回 0xc（PC 房类）→ setPremiumState(0xc,3)+通知；
// 其它非 0 → handleSetUserPremium(type,0,0,0,1) + RemoveRestrictedGoods +
// 角色在线时 ProcPremiumFatigue/SendFatigue + g_SPremiumInfo[type] 分支
// （kind==1 且 field64>0 → SetNeedCheckOverEquipItem+SendNotiPacket 组；
//  0x53/0x57 → 表情重算；0x21 → Lethe 标志）+ 通知。
void CHandlePremium::handlePremiumTimeout(CUser* user)
{
    // [b28d-56 判词·N → L6S5 复修] ORIG 0x086afa7c 118 条 vs 我们 114 条（ORIG `push %ebx;
    // sub $0x44` vs 我们 `sub $0x48`，SUserPremiumInfo 返回槽 -0x1c vs -0x20）。根因 =
    // 旧码 `int timeoutType = info.m_type;` 具名槽（-0xc，+1 store）把 sret 槽顶深 4 并让
    // 4 处比较折成 `cmpl $imm,-0xc(%ebp)` 内存直比；ORIG 0x086afaa0/0x086afaeb/0x086afaf6/
    // 0x086afacc/0x086afb68/0x086afbc5/0x086afc13 **七处**均 `mov -0x1c(%ebp),%eax` fresh
    // 读再 `cmp/test %eax` ⇒ 删局部、逐处写 info.m_type（P0）。setPremiumState/
    // handleNotifyPremiumInfo 的 type 实参 ORIG 亦取 info.m_type（0x086afaa8/0x086afacc），
    // 非字面量 0xc。探针 /tmp/l6s5_probe2.cpp 实证 4.4.7 对「局部 sret 结构成员 == imm」
    // 恒产 `mov slot,%eax; cmp $imm,%eax`，与 ORIG 同形。
    // [L6S5 实测] 旧 113 条/ndiff 55 → **122 条/ndiff 4**（ORIG 118）。残余 4 行 = 4.1 跨三次
    // 比较复用同一 %eax 的 CSE（ORIG 一次 load，4.4 每处重载 ⇒ 多 2 条 mov）+ 1 条块尾 jmp，
    // 属跨版本 N 族（switch 形实测 ndiff 10 更劣，已回滚，见下方 P272 注释）。
    SUserPremiumInfo info = user->GetPremiumInfo()->CheckPremiumTimeout();
    // [P0/P183] ORIG 0x086afaa0/0x086afaeb/0x086afaf6/0x086afacc/0x086afb68/0x086afbc5/
    // 0x086afc13 七处均 `mov -0x1c(%ebp),%eax` fresh 读 info.m_type —— **无 timeoutType
    // 具名槽**（旧码置 -0xc：+1 store +N 内存直比，且把 sret 槽顶到 -0x20）。语义等价，
    // 逐处用 info.m_type 表达式即复现寄存器比较形。
    // setPremiumState/handleNotifyPremiumInfo 的 type 实参 ORIG 亦取 info.m_type
    // （0x086afaa8 `mov -0x1c(%ebp),%eax; mov %eax,%ebx` / 0x086afacc），非字面量 0xc。
    if (info.m_type == 0xc)
    {
        user->GetPremiumInfoW()->setPremiumState((ENUM_PREMIUM_TYPE)info.m_type,
                                                 (ENUM_PREMIUM_STATE)3);
        handleNotifyPremiumInfo(user, info.m_type, 0);
        return;
    }
    if (info.m_type == 0)
        return;

    handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)info.m_type, 0, 0, 0, true);
    // ORIG 0x086afb4c：mov -0x10(%ebp) = info.m_flag(+0xc)（非 m_type）。
    user->RemoveRestrictedGoods(info.m_flag);
    if (user->getCurCharacR() != 0)
    {
        user->ProcPremiumFatigue();
        user->SendFatigue();
        if (g_SPremiumInfo[info.m_type].m_kind == 1)
        {
            if (g_SPremiumInfo[info.m_type].m_field64 > 0)
                user->SetNeedCheckOverEquipItem();
            user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
        }
        // [P272 两跳同落 then ⇒ 德摩根块序交换] ORIG 0x086afbc5..0x086afbfb：
        // `cmp $0x53; je L_emoticon` + `cmp $0x57; je L_emoticon`（两跳**同落**后置
        // emoticon 块）+ `cmp $0x21; jne L_end` + lethe 块 `jmp L_end` ⇒ 原码是
        // 「否定合取包裹」形（探针 /tmp/l6s5_probe4.cpp t_nested 实测 4.4.7 逐条同构；
        // 原 `if (a==0x53||a==0x57){emoticon} else if (a==0x21){lethe}` 反产
        // `jne` 极性 + emoticon 直落，即我方旧码 DIFF 的唯一来源）。
        // [L6S5 试形记录] switch 形（case 0x53/0x57 同体 + case 0x21）实测 ndiff 10
        // （`je/jmp` 双跳失配 + lethe 块 movzbl 段位置错），劣于本 nested 形（ndiff 4）
        // ⇒ 已回滚。残余 2 条 `mov -0x1c(%ebp),%eax` = 4.1 跨比较复用 %eax 的 CSE
        // （ORIG 三次比较一次 load；4.4 逐次重载），属跨版本 N 族封顶。
        if (info.m_type != 0x53 && info.m_type != 0x57)
        {
            if (info.m_type == 0x21)
            {
                user->SetOneDayLetheFlag(
                    CPremiumLetheManager::NOT_USE_LETHE_STATE);
                user->SetIsEndLetheAtLogin();
            }
        }
        else
        {
            user->ReCalcChattingEmoticon();
            user->SendChattingEmoticon();
        }
    }
    handleNotifyPremiumInfo(user, info.m_type, 0);
}