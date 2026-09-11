// ORIG execute 0x084a8d02。
void CMoveCharacCmd::execute()
{
    // P48：ORIG 每用点 fresh G_GameWorld()（find/move_area/send_to_area 各调）
    CUser* target =
        G_GameWorld()->find_user_by_charac_name(m_str0.c_str());
    if (target == 0)
    {
        return;
    }
    // [N2-S5 负结论] ORIG 两处早退 `je 0x084a8f62` 同目标（单一落点），但合并为
    // `target == 0 || getCurCharacR() == 0` 实测 4.4 反劣 170→171（`||` 求值序
    // 使 getCurCharacR 结果多一次布尔归一）⇒ 保持两条独立 if。
    // [P6AuditOldN.P6S2 2026-09-11] P59 守卫包裹试验：ORIG 0x084a8d17..0x084a8d1d
    //   = `test %eax,%eax; setne %al; test %al,%al; je 0x084a8f62`（正条件物化 +
    //   零跳尾），我方早退形恒 `sete/jne`。探针 /tmp/p6/probe/p1.cpp g3 实证
    //   「if (f()!=0) { 正文 } else { return; }」产 setne/je ⇒ 以此形替换早退。
    if (target->getCurCharacR() != 0)   // ORIG CUserCharacInfo::getCurCharacR 判 0
    {
    int vill = static_cast<int>(target->getCurCharacVill());
    int area = target->get_area(false);
    // [N2-S5] ORIG 槽 -0x10/-0xc 由 `movzwl %ax,%eax` 提升后直存 int 槽
    // （get_posX/Y 返回 unsigned short）；试 int 具名（原 unsigned + `& 0xffff`
    // 使 move_area 实参预取到 %edi/%esi 并多 push/pop %edi）。
    int x = target->get_posX();
    int y = target->get_posY();

    // ORIG(0x084a8dd4)：同 vill && 同 area → set_position，否则 move_area。
    // [G5 2026-09-10 P167/P183 真缺口修复] 旧码 `CUser* self = GetUser();` 把 5 个用点
    //   合并成 1 次调用；p167 实证 ORIG 调 `CCommand::GetUser()` **5 次**
    //   （MISSING 4×，OUR calls 27 vs ORIG 31）⇒ 删具名缓存，逐用点 fresh 重发
    //   （P48/P183：ORIG 在实参位直读、无 self 槽）。
    // [N2-S5] ORIG 0x084a8daf `jne 0x084a8dd4` + 0x084a8dd2 `je 0x084a8ddb`
    // ⇒ 条件为 `vill != curVill || area != curArea`（不等=真），真支落
    // fall-through 0x084a8de4 move_area，假支 0x084a8e48 set_position。
    // 旧码写相等 `&&` 使两分支布局镜像（move_area 被挤到尾部，多 1 条 jmp
    // 与 %edi 保活 push/pop）。
    if (static_cast<int>(GetUser()->getCurCharacVill()) != vill ||
        GetUser()->get_area(false) != area)
    {
        G_GameWorld()->move_area(GetUser(), vill, area,
                                 static_cast<int>(x), static_cast<int>(y),
                                 0, false, 0, 0, 0);
    }
    else
    {
        GetUser()->set_position(static_cast<unsigned short>(x),
                                static_cast<unsigned short>(y), 0);
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(static_cast<int>(GetUser()->get_unique_id() & 0xffff));
    packet.put_short(static_cast<int>(x));
    packet.put_short(static_cast<int>(y));
    packet.put_byte(0);
    packet.finalize(true);
    if (vill != 0 || area != 0)
    {
        G_GameWorld()->send_to_area(vill, area, packet);
    }
    // [P6AuditOldN.P6S2 2026-09-11 · 负结论] 残余 = 尾部 PacketGuard 析构布局
    //   （-5 insns）：ORIG 0x084a8eef..0x084a8f19 在 send_to_area 分支**内联**一份
    //   `lea -0x28(%ebp),%eax; mov %eax,(%esp); call 0x858de80<~PacketGuard>; jmp 0x84a8f62`，
    //   0x084a8f3a 另一份供 else（area==0）路径共用；我方 4.4 把真支折成
    //   `jmp <共享析构块>`（该块位于 EH cleanup 之后）⇒ 少 4 条、多 1 条 jmp，净 -5。
    //   已试钥匙（均无效/回滚）：① send_to_area 分支显式 `return;`（4.4 仍共享，
    //   读数 171 不变）；② 括号包裹/goto 尾标签同效。判 4.1/4.4 尾块布局 lowering 差，
    //   源码不可控。本件本轮落地 = P59 守卫包裹（setne/je 块已消）。
    }
    else
    {
        return;
    }
}