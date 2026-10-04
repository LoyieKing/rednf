// ORIG 0x084c8d10 T dispatch_sig —— 游戏内广告条件重载。
// 报告 docs/class_func_reports/Inter_ReloadIngameAdvertisement/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=5（ORIG 0x084c8d10）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=10（ORIG 0x84c8d10）：N101 imul/lea 折叠+P30/N4 add-vs-lea。
int Inter_ReloadIngameAdvertisement::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG：InitADExposeInfo 前 local_14=param_3（-0x10）；循环体每迭代重调 Instance
    // （不缓存局部，asm +0x4c call）；-1 检查在循环体顶部（break）、计数器 setle 在底部。
    // [b28c-34] ORIG 0x084c8d17 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` = 具名
    // recv 局部（P0 槽往返）；循环内两处经该槽装载（lea/add 形）。
    stInGameADExposeCondition* cond =
        reinterpret_cast<stInGameADExposeCondition*>(data);
    InGameADManagerInstance()->InitADExposeInfo();
    int count;
    for (count = 0; count < 200; ++count)
    {
        // [b28d-PoolInterDisp] int 加法链形（probe_g 实证，10→4）：POINTER_PLUS 形被
        // 4.4 fold 成 base+(prod+const)（恒残 10）；纯 int PLUS 不折 ⇒ 复现
        // imul/lea/add $0x210 三段。字段访问形 cond[count].m_sentinel 实测折成
        // `add MEM+mov 0x210(,)`（gap+2、ndiff 6，劣于本形）。残 4 = imul 目标寄存器
        // 与 base 装载先后对调（LRA 形，N18a 族）。
        // [N 判词·2026-09-11 复核] 残 1 块 = `imul $0x84c,%eax,%eax`（ORIG 就地 + base 后装载）
        //   vs 4.4 `imul $0x84c,%eax,%edx` + `mov base,%eax`（N18a/N101 寄存器选择）。
        //   试形：交换加法操作数（`count*0x84c + (int)cond`）实测 **零变化**（仍 51/51、ndiff 1）⇒ 封 N。
        if (cond[count].m_sentinel == -1)
            break;                                       // ORIG +0x3c je（顶部门槛）
        InGameADManagerInstance()->InsertADExposeInfo(
            cond[count]);
    }
    cMyTrace(__PRETTY_FUNCTION__,
        0x2290, 0)("In Game Advertisement condition Reload OK, count(%d)", count);
    return 0;
}