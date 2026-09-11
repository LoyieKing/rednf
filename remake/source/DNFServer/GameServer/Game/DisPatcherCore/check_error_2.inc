// ORIG 0x081c14f0 T _ZN23DisPatcher_DeleteCharac11check_errorEP5CUserR8MSG_BASE
// [b28d-29] N82/P0：msg 形参副本落 -0x18（ORIG -0x14 直读形），帧 0x44 vs 0x40，调用点全等  (ORIG 0x081c14f0 insn 199->198)
// 报告：docs/class_func_reports/DisPatcher_DeleteCharac/check_error.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 76（ORIG 199 / ours 197），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [DPCB 2026-09-11] 签名轴（§6.5 串盲区直证）：ORIG .rodata 串为
// `int DisPatcher_DeleteCharac::check_error(CUser*, MSG_BASE&)`（strings df_game_r 命中）。
// 我方原 `unsigned int` ⇒ __PRETTY_FUNCTION__ 契约不一致。返回类型不入 mangled。
int DisPatcher_DeleteCharac::check_error(CUser* user, MSG_BASE& msg)

{
    if (user->get_state() != 2)
    {
        return -1;
    }
    if (user->isLockedCreateCharac())
    {
        return 0x13;
    }
    // [G5-MiscC] P28/声明序：ORIG 0x081c14f8/0x081c1517 两次 state 检查（`call get_state`
    // 与 `call isLockedCreateCharac`）**之后**才在 0x081c1530 落 msg 视图具名槽
    // （`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`）⇒ msg 视图声明必须后置于前两条检查
    // （我方原把 req 声明放函数首，槽写出现在 0x081c150x 之前的 -0x18）。
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
    if (!((signed char)req.m_slot < user->get_charac_count() && req.m_slot >= 0))
    {
        return -2;
    }
    if (!user->CheckDeleteCharacTime(req.m_slot))
    {
        return 0x16;
    }
    if (user->getCurCharacNo() != 0 &&
        user->getCurCharacNo() == user->get_charac_no(req.m_slot))
    {
        // [b28e-DispCore] P227 分点复核：ORIG 0x081c19f? 段为
        // `mov %eax,%ebx`（GetCurCharacNo 结果）+ `movzbl 0xd; movsbl %al,%esi`
        // （req.m_slot）**在 ctor 之前**求值（入 callee-saved）、ctor 后才压 op()
        // 实参 ⇒ 本点（0xaa4）原码是全表达式临时链。旧「链式劣化」结论系两点同改
        // （0xab4 点是 ctor 后求值）所致 ⇒ 本轮只翻本点、0xab4 保持两语句。
        cMyTrace(__PRETTY_FUNCTION__, 0xaa4, 5)(
            "pUser->getCurCharacNo() == recv_pk_del_charac.charac_index %d %d",
            req.m_slot, user->get_charac_no(req.m_slot));
        return 0x16;
    }
    if (user->isLocked4DataLoad())
    {
        return 0x13;
    }
    if (user->getCurCharacR() != 0)
    {
        return -3;
    }
    // [b28e-L5] P196/轴B 真语义分叉修正：ORIG 0x081c16aa `movzbl 0x1246(%eax),%eax;
    // cmp $0x1,%al; setg %al`（**有符号** setg），我方原写无符号比较产 `seta`
    // ⇒ 值域 ≥0x80 时两侧分叉（0x80 起 ORIG 视为负而跳过异常链）⇒ 按 ORIG 加
    // (signed char) 读法（字段本型不动）。
    if ((signed char)user->m_characList[(int)req.m_slot].m_characLinkType > 1)
    {
        // [b28e-DispCore] P221 两点同链式：ORIG 两点槽 -0x34(0xaa4)/-0x24(0xab4)
        // 与我方互反；两点统一全表达式临时链后 4.4 槽序与 ORIG 同向（P221 判决）。
        cMyTrace(__PRETTY_FUNCTION__, 0xab4, 5)("LINK_LOG : ABNOMAL CHARAC DELETEING!!");
        return -4;
    }
    // [DPC_CheckErr] P0/P183：ORIG 0x081c16d6 `call get_charac_name; mov %eax,-0x10(%ebp)`
    // （name 具名槽 -0x10 ✓）；而 +0x12 角色名字区**无具名槽** —— ORIG 0x081c16ef
    // `mov -0x14(%ebp),%eax; add $0x12,%eax; movzbl (%eax),%eax` 与 0x081c1741
    // `mov -0x14(%ebp),%eax; add $0x12,%eax`（strncmp 第二实参）都是**每次从 msg 视图
    // 重算**（msg 视图槽 -0x14）。我方原有 `const char* reqName = req.m_padE + 4;` 具名
    // 多占一槽（-0x10）⇒ msg 视图被挤到 -0x18、帧 0x44（ORIG 0x40）、且多发一条槽写。
    // 按 ORIG 内联（+0x12 = m_padE+4，m_padE 内 +0xe..+0x11 为填充）。
    const char* name = user->get_charac_name(req.m_slot);
    // [g05 代落 · G16 实测 · P365 整型算术形] ORIG 0x081c16ef `mov -0x14(%ebp),%eax;
    //   add $0x12,%eax; movzbl (%eax),%eax`（base 先入 eax + 整型加法）⇒ 用
    //   `reinterpret_cast<unsigned long>(&req) + 0x12` 复现两步 add（G16 实测本件 AE 199/199）。
    if (name == 0 ||
        *reinterpret_cast<unsigned char*>(
            reinterpret_cast<unsigned long>(&req) + 0x12) == 0)
    {
        return -5;
    }
    if (strncmp(name,
                reinterpret_cast<const char*>(
                    reinterpret_cast<unsigned long>(&req) + 0x12),
                0x1e) != 0)
    {
        return -6;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x17))
    {
        return 0xd1;
    }
    int chk = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2e);
    // [b28e-DispCore] P222 两形互试：ORIG 0x081c1b? 为 `cmpl $0x0,-0xc; je <mov $0,%eax>`
    // ＋尾 `mov -0xc(%ebp),%eax; jmp` ⇒ 反写形（`!= 0` 返回 chk、`mov $0` 落尾）。
    // 残余唯一 1 处 = `add $0x12,%eax; movzbl (%eax),%eax`（ORIG）vs 4.4 disp 折叠
    // `movzbl 0x12(%eax),%eax` ⇒ N102（结构首成员数组退化+常量偏移，七源形全折 disp）。
    if (chk != 0)
    {
        return chk;
    }
    return 0;
}
