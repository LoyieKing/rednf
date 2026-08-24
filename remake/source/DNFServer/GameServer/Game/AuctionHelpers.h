#ifndef DNF_GAME_AUCTION_HELPERS_H_
#define DNF_GAME_AUCTION_HELPERS_H_

// ============================================================================
// AuctionHelpers.h —— 拍卖域价格/乘法校验自由函数（全局 T 符号）
// ORIG: df_game_r
//   _Z12IsPriceErrorii       IsPriceError(int,int)          0x08213e15  T
//   _Z21IsValidMultiplyResultii IsValidMultiplyResult(int,int) 0x08213e40  T
// 语义严格还原自 docs/class_func_reports/_lt_global_gt_/IsPriceError.md 与
// IsValidMultiplyResult.md（objdump 逐指令 + Ghidra 反编译交叉确认）。
// 初始调用方：Dispatcher_AuctionRegistItem::dispatch_sig。
// 纯函数，无内部状态，无外部依赖。
// ============================================================================

// 检查 (price, limit) 是否构成“非法价格”。
// 语义（C = Ghidra）：
//   return (param_1 < -1 || param_2 < -1 || (param_2 != -1 && param_2 <= param_1)) ? 1 : 0
// 即：
//   - 任一操作数 < -1          => 错误（1）
//   - b == -1                  => 不带上限的合法价      => 非错误（0）
//   - a >= b                   => 价格达到/超过上限      => 错误（1）
//   - 其余                     => 非错误（0）
int IsPriceError(int price, int limit);

// 检查两个 int 的有符号乘积是否仍在“合法 int 量级”（ORIG 特有的不对称边界）。
// 语义（严格按寄存器轨迹还原）：
//   A=(unsigned)a; B=(unsigned)b;
//   signA = a>>31 ? 0xFFFFFFFF : 0;
//   signB = b>>31 ? 0xFFFFFFFF : 0;
//   acc   = signA*B + signB*A + (unsigned)(((u64)A*(u64)B)>>32);
//   if ((int)acc < 0) return 1;
//   if ((int)acc > 0) return 0;
//   return (unsigned)A*(unsigned)B <= 0x7FFFFFFF ? 1 : 0;
// 注意：对“极大负溢出”（如 -0x80000000 * 3）返回 1（合法），
//       而普通 __builtin_mul_overflow 判为溢出；此为 ORIG 忠实行为，勿用
//       “乘积落在 int32 区间”的朴素判定替代。
int IsValidMultiplyResult(int a, int b);

#endif // DNF_GAME_AUCTION_HELPERS_H_
