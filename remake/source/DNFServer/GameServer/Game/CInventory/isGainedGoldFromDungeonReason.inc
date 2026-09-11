// ============================================================================
// 三个自由辅助函数（ORIG 全局符号，Money 簇自由函数）
//   isGainedGoldFromDungeonReason  @0x850d32a (W)  : reason==4 || reason==0xf
//   write_log_gain_money           @0x84febd9 (T)  : AddGold + 流通统计跳表
//   write_log_use_money            @0x84fed9b (T)  : SubGold + 流通/Value统计跳表
// 语义依据 CInventory.md 逐条对照；amount<0 日志分支忠实 ORIG logFormat 串。
// 注：ORIG 各 case 额外调用 CUser::AddMoneyPlus/AddMoneyMinus(@0x868f266/
//   @0x868f310)，该符号尚未在源实现，此处省略以避免引入新未定义。
// ============================================================================
bool isGainedGoldFromDungeonReason(eMoneyAddReason reason)
{
    // ORIG 0x867d9a2：switch 形态（先 mov 参数到寄存器再 cmp/je 链）。
    switch ((int)reason)
    {
    case 4:
    case 0xf:
        return true;
    }
    return false;
}