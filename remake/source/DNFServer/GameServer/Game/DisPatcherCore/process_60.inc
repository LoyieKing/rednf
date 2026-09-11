// ORIG 0x081cb2e2 T _ZN20DisPatcher_AddFriend7processEP5CUserR8MSG_BASER9ParamBase：
// 薄包装，尾调 process_packet_jpn 并透传返回值（汇编 leave/ret 直传 eax）。
int DisPatcher_AddFriend::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // P31 帧差：ORIG sub $0x58 → dead 局部 0x40（薄包装仍保留深帧）。
    volatile char dead_40[0x40]; (void)dead_40;
    // [P5-DPC 2026-09-11 P139] ORIG 发射了 `_ZZN20DisPatcher_AddFriend7processE…E19__PRETTY_FUNCTION__`
    // （0x08bd5080，size 0x49，串 `virtual int DisPatcher_AddFriend::process(CUser*, MSG_BASE&, ParamBase&)`），
    // 但全库反汇编对该地址**零引用**（`objdump -d df_game_r | grep -c 8bd5080` = 0）
    // ⇒ 是 ORIG 遗留的死字符串；GCC 只在源码「使用」__PRETTY_FUNCTION__ 时发射该局部
    // 静态串。探针 /tmp/P5/probe_prett2.cpp（同形包装函数）直证
    // `if (0) { (void)__PRETTY_FUNCTION__; }` **发射符号且零指令影响**（-O0 亦成立）
    // ⇒ 用它补齐符号形态（verdict 不变，无运行期行为）。
    if (0) { (void)__PRETTY_FUNCTION__; }
    return process_packet_jpn(user, msg, param);
}
