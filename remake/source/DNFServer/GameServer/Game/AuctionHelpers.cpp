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
    // ORIG 单布尔形态（Ghidra 等）：`(a<-1||b<-1) || (b!=-1 && b<=a)`。
    if ((price < -1 || limit < -1) || (limit != -1 && price >= limit))
        return 1;
    return 0;
}

bool IsValidMultiplyResult(int a, int b)
{
    // ORIG 0x08213e40 的指令序列即 4.1 对 `(long long)a * b > 0x7fffffff`
    // 的 64 位乘法内联展开（sar $0x1f 符号修正 + mul 高位）与 64 位常量比较
    // （hi<0→假、hi>0→真、hi==0→无符号比较 lo）。按 P0 全部内联为表达式，
    // 不设具名局部（ORIG 无栈槽往返）。
    return ((long long)a * (long long)b > 0x7fffffff) ? 0 : 1;
}
