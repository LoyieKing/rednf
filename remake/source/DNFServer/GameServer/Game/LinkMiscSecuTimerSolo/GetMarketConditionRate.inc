// ===========================================================================
// GetMarketConditionRate  ORIG T 0x082f843f
// 找市场条件档位：outIdx = 首个 arr[outIdx]*rate > amount 的下标（<=4 扫描；
// 全不满足为 5），钳到 nCount+1；非 0 时 outRate = arr[outIdx+4]。
// P0 判据（ORIG 切片逐条）：无独立 i 局部——循环索引直接走 outIdx 引用槽
// （0x20(%ebp) 每次重读），源形 for(outIdx=0;...;outIdx=outIdx+1)；自增为
// 双读引用 5 条 lea/store 形；钳位比较 RHS 先物化（`lea 0x1(nc);cmp;jge`），
// 即源码写 `nCount + 1 < outIdx`；尾索引 outIdx+4 经 scratch 槽（-0x4）
// store/load 往返。余 fucompp/fnstsw/sahf vs fucomip 为 N（x87 跨版本）。
// [b28c-L23 N85 → 实测可路由翻正] 本件在默认旗标下的 8 行残差中，4 行是
// `fucompp/fnstsw %ax/sahf` vs 4.4 的 `fucomip %st(1),%st/fstp %st(0)`（N85 x87
// 比较两向）。**fresh 实测**：本 TU 加 `-march=i486 -mtune=generic` 后该段完全消失、
// ndiff 8→0（orig=71 ours=71 指令全等，见探针记录）——4.4 默认 i686 目标才发
// fcomi 系。按 Main 路由纪律本波不改 dnf_game_cxx.sh，故现路由下仍记 DIFF(N85)，
// 已把该 TU 列入「4.4.6/i486 路由变更后优先复测」清单。
// ===========================================================================
void GetMarketConditionRate(double rate, double amount, double* arr,
                            int nCount, int& outIdx, double& outRate)
{
    outIdx = 0;
    outRate = 1.0;
    for (outIdx = 0; outIdx <= 4; outIdx = outIdx + 1)
    {
        if (arr[outIdx] * rate > amount)
            break;
    }
    if (nCount + 1 < outIdx)
    {
        outIdx = nCount + 1;
    }
    if (outIdx != 0)
    {
        int idx = outIdx + 4;
        outRate = arr[idx];
    }
}