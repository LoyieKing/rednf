DisPatcher_BuyItem::~DisPatcher_BuyItem() {}
// ORIG 0x081be46a T _ZN18DisPatcher_BuyItem11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_BuyItem/check_error.md 汇编+反编译 C 全文转写）：
// checkValidShopItem(0x15,(ulong)0x0d) 失败 → OneADayShop 判定（是→0x11，否→
// addServerHackCnt 0x3c9+(-1)）；随后 CheckInTrade/getCurCharacR/isHackUser → 0x13、
// isRestricted(1,3) → 0xd1、state!=8 时 secu 0x22 非0返码、IsBusyPrivateStore → 0x3c、
// [b28d-29] setcc-fold + N82：ORIG 直分支常量岛（mov $0x11; jmp），4.4 折 sete/setne；槽号平移，语义集合全等  (ORIG 0x081be46a insn 136->149)
// OneADayShop 时 GetRepeatEvent(0x6d)->IsEventing(0) 为假 → 0x11；否则 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 39（ORIG 136 / ours 149），残差=setcc-fold bool 物化岛；沿用上方 b28d-29 归因，封顶。
// [b28d-TakeDispatchCore] 真缺口修复（双调用）：原「get_state()==8 || Check(0x22)==0 进主体 +
// 末尾重调 Check 返回」= Check 调 2 次（p167 EXTRA +1）；ORIG 0x081be5a8..0x081be5d0 是
// `if (get_state() != 8) { int r = Check(user,0x22); if (r != 0) return r; }` 单次调用
// （结果存 -0xc(%ebp)，cmpl $0 后回读 jmp 尾）。改写后 CALLSITE CLEAR 18/18、
// ndiff 39→10（ORIG 136 / ours 140）、our 149→140（gap −13→−4）。
