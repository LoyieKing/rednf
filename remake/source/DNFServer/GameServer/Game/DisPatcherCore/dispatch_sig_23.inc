// ORIG 0x081ece64 T _ZN17DisPatcher_DiePvP12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82：帧 0x6c vs 0x60 + 槽平移（ushort 数组 -0x36 vs -0x26），调用点全等  (ORIG 0x081ece64 insn 146->147)
// 报告：docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md（汇编+反编译 C 全读）
// [Lane5] P227 已落（gap 146→146=0）：getter 内联进 CSwitchLog 构造即调用单表达式，
// trace ctor 提到 getter 之前的问题消除。残余=整帧槽深 +4（-0x36 vs -0x3a 等）+
// 帧 0x6c vs 0x5c（N82/N115 族），ndiff ~60。
int DisPatcher_DiePvP::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 7)
    {
        return 0;
    }
    // [FL4 负结论] P232 撑帧法对本件无效：补 `char pad[0x10]` 后 `sub` 立即数虽对齐
    // 0x6c，但全部局部槽相对 ORIG 由「深 4」扩为「深 0x14」（ORIG PacketGuard -0x44 /
    // 我方 -0x58），ndiff 30→28 仅来自帧行本身 ⇒ 判 N82（4.1 按生命周期穿插 vs 4.4
    // 按首次使用分区的槽分配差），保持原帧。
    unsigned short targetIdx = 0xffff;
    if (!packet.get_short(targetIdx))
    {
        return LineFunc(0x6f12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CUser* target = G_GameWorld()->find_from_world(targetIdx);
    if (target == 0)
    {
        targetIdx = 0xffff;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x6f1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard guard;
    int result = room->die_charac(user, target);
    // [SigB] P0/P183：ORIG 的 -0x4c **不是具名槽**——证据链：
    //  ① ORIG 0x081ecf69 调 get_pvp_battle_mode 在 0x081ecf77 get_index /
    //     0x081ecf84 getCurCharacName / 0x081ecf91 get_acc_name **之前**，
    //     与 varargs 右起求值序（0x3a,result,battleMode,index,name,acc_name）一致；
    //  ② 0x081ecf6e `mov %eax,-0x4c(%ebp)` 是该实参**跨 0x081ecfbe ctor 调用的 spill 槽**
    //     ——ebx/esi/edi 已被另三个 vararg 占满（0x081ecf7c/+0x125/+0x132），O0 无寄存器可保活。
    //  ③ 我方原写具名 `int battleMode` 使该槽落在标量组 -0x1c（全体槽随之深 4）。
    // ⇒ 原码是 log 实参**内联** `room->get_pvp_battle_mode()`。见 [Lane5] P227 链式求值形。
    CSwitchLog(__PRETTY_FUNCTION__, 0x6f22, 0, 0)(
        "pvp@log %s,%s,room(%d),%d,%d,%d",
        user->get_acc_name(), user->getCurCharacName(), room->get_index(),
        room->get_pvp_battle_mode(), result, 0x3a);
    if (0 < result)
    {
        guard.put_header(1, 0x3a);
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
