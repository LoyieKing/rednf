// b28d-solo-CHackAnalyzer: N 封顶（N82+ 槽分区，本轮五形复测）——ORIG 0x080f7cca。
//   槽图（ORIG）: ratio@-0xc, elapsed@-0x10, X(8B 死区)@-0x18..-0x11,
//   numMs@-0x20..-0x19, CW@-0x22, CW2@-0x24；帧 0x38。
//   4.4 x87 工作槽恒**紧贴最深槽对象**，故两态不可两全：
//   ①无死局部（V0）：numMs@-0x18/CW@-0x1a/-0x1c（帧 0x38 ✓，numMs 浅 8）；
//   ②`long long deadPad`/`char[8]`/2×int/`long long[2]` 置于 numMs 后：
//     numMs@-0x20 ✓ 但死局部恒落 numMs **深侧**（-0x28）⇒ CW 被推至 -0x2a/-0x2c、
//     帧 0x48（4 形实测同结论；V3 置于 numMs 前反使 numMs 回 -0x18）。
//   4.4 未使用标量「按最深可用处分配」，源码不可控其浅侧落位 ⇒ N 封顶（帧/助记符全等）。
//   [round5 复核] fresh compare_tu 口径 55/55 NEAR（.o 报告 DIFF 系 N124 假报）；
//   槽图 ORIG numMs@-0x20 / CW@-0x22,CW2@-0x24 vs 我方 numMs@-0x18 / CW@-0x1a,-0x1c
//   —— 4.4 把 x87 工作槽紧贴「最深槽对象」的分配策略不可源级翻转 ⇒ 维持 N82。
// [P8S1 2026-09-11 本波复核] 维持封顶：N82/N126 x87 工作槽紧贴最深槽对象(ORIG numMs@-0x20)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
void CHackAnalyzer::_analyzeSpeedHack(unsigned int ms)
{
    // ORIG 0x080f7cca（_analyzeSpeedHack.md）：ratio=(int)(100.0f*ms/elapsed)；
    // ratio<=0x45 或 >0x82 时记 0x3c1。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_SPEED))
        return;

    if (m_field24 == 0)
    {
        _reportIncurrectReport();
        return;
    }

    long long numMs;                                     // ORIG 声明在前（槽最深），赋值在后

    int elapsed = m_field24 - m_field20;                 // ORIG fildl（先物化 elapsed）
    numMs = ms;                                          // ORIG fildll（有符号直转，无 2^64 修正段）
    // [L6S4 实测负结论] 声明序旋钮（P331 混合尺寸区）已试：numMs 声明移至 elapsed
    // 之后 ⇒ elapsed 槽 -0x10→-0x1c、x87 组 -0x1a/-0x1c→-0x2a/-0x2c（反向劣化 0x10），
    // 回滚。ORIG numMs@-0x20/CW@-0x22/CW2@-0x24 与 4.4 的 -0x18/-0x1a/-0x1c 差 8 属
    // 4.1/4.4 槽分配策略差（N82/N126），声明序不可翻 ⇒ 封顶。
    // ORIG flds 在 fdivrp 之后 ⇒ 左操作数为内存 float 变量（乘法左序）。
    int ratio = (int)(((double)numMs / (double)elapsed) *
                      _S_HACK_RATIO_COEF_100F);

    if (ratio <= 0x45 || ratio > 0x82)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_SPEED, (ENUM_HACKTYPE)0x3c1, ratio);
    }
}