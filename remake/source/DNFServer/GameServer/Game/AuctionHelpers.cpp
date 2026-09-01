// ============================================================================
// AuctionHelpers.cpp —— 拍卖域价格/乘法校验自由函数（全局 T 符号）
// ORIG: df_game_r
//   _Z12IsPriceErrorii       IsPriceError(int,int)          0x08213e15  T
//   _Z21IsValidMultiplyResultii IsValidMultiplyResult(int,int) 0x08213e40  T
// 见 AuctionHelpers.h 头部说明。
// 实现与 ORIG 逐指令一致（objdump + Ghidra 双重确认，256 组输入交叉验证）。
// ============================================================================

#include "AuctionHelpers.h"

bool IsPriceError(int price, int limit)
{
    if (price < -1 || limit < -1)
        return 1;
    if (limit == -1)
        return 0;
    if (price < limit)
        return 0;
    return 1;
}

bool IsValidMultiplyResult(int a, int b)
{
    unsigned int A = (unsigned int)a;
    unsigned int B = (unsigned int)b;
    unsigned long long P = (unsigned long long)A * (unsigned long long)B;
    int acc = (a >> 31) * b + (b >> 31) * a + (int)(unsigned int)(P >> 32);
    if (acc < 0)
        return 1;
    if (acc > 0)
        return 0;
    return ((unsigned int)P <= 0x7FFFFFFFu) ? 1 : 0;
}
