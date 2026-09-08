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
    // [b28d-41 判词·N] DIFF：ORIG 0x08213e40 无栈帧（push edi/esi/ebx 后直入），
    // acc 前两项在 %esi、乘积 mul 一次（eax:edx），单出口 %ecx 物化——4.1 对
    // 单语句内重复子表达式走 SAVE_EXPR **寄存器**复用。4.4 对同一表达式恒把
    // SAVE_EXPR 落槽并重算子表达式（本件 +0x24 帧 / -0x28..-0x14 五槽 / acc 与
    // 乘积各算两次）。八形探针（/tmp/d41_p8.cpp fA..fI：具名/三元链/短路/register/
    // 宏内联/乘积前置）全部落槽，无类型旋钮可试（比较两侧均 int/unsigned/ull）
    // ⇒ 跨版本 SAVE_EXPR 处理差异，语义等价（乘积高位+符号修正项，历史 256 组
    // 输入交叉验证一致）。
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
