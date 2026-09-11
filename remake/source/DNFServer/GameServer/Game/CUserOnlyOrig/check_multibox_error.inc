// ---------------------------------------------------------------------------
// 31) CUser::check_multibox_error(int requiredEmptySlots)
//     报告：docs/class_func_reports/CUser/check_multibox_error.md。ORIG 0x08675edc。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08675edc：P168a 裸短路链（callee bool）一次归零
// （具名 uchar r + 逗号绑定形令 4.4 落 -0x9 槽 + cmpb；见函数内注释）。
int CUser::check_multibox_error(int requiredEmptySlots)
{
    // ORIG 0x08675ee2-0x08675fb1：getCurCharacInvenW 每段重调（免 inv 槽）+
    // 各段 `xor $0x1,%eax; test %al,%al; jne BAD岛`——(check^1) || 短路链 +
    // 具名 bool 两级物化（[mov$1][mov$0]; test;je → 0x13 岛，08675fbf-08675fc8）
    // 4.4 把 `(x^1)||` 折成 `cmp $1`——register %eax 具名 + 逗号 expr 阻断
    // fold-const，复现 ORIG 0x08675f04 每段 `xor $0x1,%eax; test %al,%al; jne
    // 共享BAD岛(mov$1)`、末段 `je GOOD(mov$0)` 两级物化（probe t19 实证）
    // uchar 绑 %eax + 逗号短路 + **hit 值物化**（ORIG 0x8675fb3-0x8675fca：
    // 真岛 mov$1 / 假岛 mov$0 / 合并 test %al; je → 0x13/0 双段——`if(a||b…)`
    // 直判形不产 hit 值物化，必须 `bool hit = (…)||…;` 显式取值）。
    // 另：本函数 ORIG 止于五段空位检查（0x8675fcf leave;ret）——前批臆造的
    // IsCurCharacGhost/CheckInTrade/GetAssaultPlace/IsPVPChannel 四段已删
    // （ORIG 全 68 insn 内无对应 call，2026-09-05 CUserC8 objdump 全文核对）。
    // [b28d-solo-CUserOnlyOrig P168a/P226] ORIG 每段 `call; xor $0x1,%eax;
    // test %al,%al; jne <共享BAD岛>` 无栈槽往返 ⇒ 原码无具名 r（P0）；
    // callee 返 bool 时裸短路链直产该形，具名 uchar r 令 4.4 落 -0x9 槽 + cmpb。
    if (! getCurCharacInvenW()->check_empty_count((Inven_Item::ITEM_TYPE)1, requiredEmptySlots) ||
        ! getCurCharacInvenW()->check_empty_count((Inven_Item::ITEM_TYPE)2, requiredEmptySlots) ||
        ! getCurCharacInvenW()->check_empty_count((Inven_Item::ITEM_TYPE)3, requiredEmptySlots) ||
        ! getCurCharacInvenW()->check_empty_count((Inven_Item::ITEM_TYPE)10, requiredEmptySlots) ||
        ! getCurCharacInvenW()->check_empty_count((Inven_Item::ITEM_TYPE)5, requiredEmptySlots))
        return 0x13;
    return 0;
}