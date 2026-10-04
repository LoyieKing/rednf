// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d14aa orig/ours=81/80 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: jne    081d14d7 <+0x2d> | mov    $0xffffffff,%eax | jmp    081d15ab <+0x101>
unsigned int Dispatcher_GuildCargoPopItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    // [G1Frest 2026-09-10] ORIG 0x081d14aa 全形重写（逐条对照 0x081d14aa..0x081d15b1）：
    //   ① 提前返回结构 + 各条件按 ORIG 极性/bool 物化（diamond `mov $1/%mov $0`、
    //      `sete`、`xor $1`），旧码为嵌套 if + guildkey 具名局部（多 -0x18 槽、帧
    //      0x28 vs ORIG `push %ebx; sub $0x24`）；
    //   ② 0x081d154e `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— itemIdx 视图
    //      取自 **msg**（第 2 参；0xc=user/0x10=msg/0x14=param），旧码误用 param
    //      ⇒ **参数错位真 bug**。
    (void)param;

    if ((bool)(user->get_state() != 3 || user->getCurCharacR() == 0))   // 0x081d14b7 起 diamond
        return -1;
    if ((bool)(user->get_charac_guildkey() == 0))                       // 0x081d14f0 `sete` 岛
        return 100;
    if (!user->IsExistGuildAgit())                                      // 0x081d150e `xor $1`
        return 0xb5;

    int secu = GlobalData::s_pSecuProtectionField->Check(               // ORIG -0x14 具名
        user, (SECURITY_PROTCTION)9);                                   // 0x08288a02
    if (secu != 0)
        return secu;

    // msg +0x0f（packed 具名视图，ORIG 单级 disp `mov 0xf(%eax),%ebx`）
    MSG_GUILD_CARGO_POP_ITEM* pv = reinterpret_cast<MSG_GUILD_CARGO_POP_ITEM*>(&msg);
    // [L7DispG4 2026-09-11 · P52 链式 receiver] ORIG 0x081d155a/0x081d1569：
    //   `mov 0xc(%ebp),%eax; mov %eax,(%esp); call getCurCharacInvenR;
    //    mov %ebx,0x4(%esp); mov %eax,(%esp); call possible_to_insert`
    //   —— receiver **无具名槽**（我方原 `inven` 落 -0xc 并多 2 条存取）。
    //   ORIG 0x081d1571 `xor $0x1,%eax; test %al,%al; je` ⇒ bool callee 取反形。
    if (!user->getCurCharacInvenR()->possible_to_insert(pv->m_itemIdx))
        return 0xcb;
    // [L7DispG4 2026-09-11 · P0 槽律] ORIG 0x081d1591 段：
    //   `call find_item; mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp); jne` ⇒
    //   **proto 具名指针槽 -0xc**（我方原 `== 0` 直判产 test/sete 物化，无槽）。
    CItem* proto = G_CDataManager()->find_item(pv->m_itemIdx);           // ORIG 0x0835fa32
    if (proto == 0)
        return 0xc5;
    return 0;
}