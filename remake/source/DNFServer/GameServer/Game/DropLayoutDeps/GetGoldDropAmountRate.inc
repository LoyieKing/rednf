int CAutoMarketConditionsControl::GetGoldDropAmountRate(int level, int gold)
{
    // [b28d-solo-DropLayoutDeps] N 封顶（ORIG 0x082f89bc，98/99；本函数级
    // `__attribute__((target("arch=i486")))` 消 x87 比较 lowering，N85 突破口）。
    // 已修：①真语义 bug——比较改 Ghidra C 实证的 `rate < 0.0`（原 `!(rate>0.0)`
    // 在 rate==0/NaN 时错误回退 base；ORIG 0x82f8a20 `test $0x45,%ah; sete` 实证）；
    // ②logCount/trace 段移入 m_field00!=0 分支（ORIG `je +0x11c`）；③P243 乘序
    // (1-base)*goldPerLevel；④register bool logIt（P231 免槽，槽组 -0x14/-0x10/-0x24；
    // 删 register 帧 0x54→0x64 全槽漂）。残差 N18a/N7：静态 logCount 自增 ORIG
    // `lea 0x1(%ecx),%edx; mov %edx,addr` vs 4.4 `mov/add/mov`（+1 条，`+=1`/
    // `1+x`/`x+1` 三形实测同）+ bool 局部 %al vs %bl（跨版本寄存器分配）。
    int result = 0;                         // ORIG prolog 后立即清零（-0x14）
    double rate;                            // ORIG 不预初始化（仅 else 路径赋值）
    if (m_field00 != 0)
    {
        // rate = (1-base)*cond[level]/(-1000) + 1；出 (0,1] 则回退 base。
        // ORIG fldl base→fsubp 先于 fildl cond[level]（P243 书写序）。
        rate = ((1.0 - m_goldBaseRate) * (double)m_goldPerLevel[level]) / -1000.0 + 1.0;
        if (rate < 0.0 || rate > 1.0)   // Ghidra C 实证：严格小于（非 <=）
            rate = m_goldBaseRate;
        result = (int)((double)gold * rate);         // ORIG fistpl 直接截断（无 +0.5）
        static int logCount = 0;            // ORIG bss _ZZ...GetGoldDropAmountRateEiiE8logCount
        register bool logIt = (logCount % 1000 == 0);
        logCount += 1;   // [L27-r2] 实测 `++logCount` 与 `+=1` 同形（4.4 对 static 恒
                         //   mov/add/mov 3 条，ORIG `lea 0x1(%ecx),%edx; mov %edx,<A>`
                         //   2 条 lea 形）⇒ 按 P211 负例（static 复合赋值序不可翻）判 N
        if (logIt)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__, 0xce, 0);
            trace("goldDropAmountRate(%lf) gold(%d), baseGold(%d)", rate, result, gold);
        }
    }
    else
    {
        result = gold;                      // 未启用折算：原样返回（ORIG 尾部块）
    }
    return result;
