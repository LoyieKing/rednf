// ORIG 0x08233280 W _ZN18DisPatcher_Lottery7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部抽奖逻辑（CheckInTrade 拒绝包 0x1d/0xda → lottery_item → 应答包 0x1d →
// 失败时限 3 秒内 addServerHackCnt(0x334) → setLastLotteryTime）内嵌于
// dispatch_sig(0x081f8380，报告 docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md)。
int DisPatcher_Lottery::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
