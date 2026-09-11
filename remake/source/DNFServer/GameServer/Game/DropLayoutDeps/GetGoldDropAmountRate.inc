// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
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
    // [L1FreeTop12.S2 2026-09-11] 已修⑤：静态 logCount 自增改**后置自增形**
    // `if (logCount++ % 1000 == 0)`（旧 `bool logIt=…; logCount+=1; if(logIt)` 恒 3 条
    // mov/add/mov）——旧值留 %ecx、写回复用 `lea 0x1(%ecx),%edx`（ORIG 0x082f8a7e/
    // 0x082f8a81 实证），bool 具名局部随之消失（sete %al 直 test）⇒ fresh .o 98/98
    // 逐条全等 AE（原 99/98 gap −1）⇒ verdict DIFF→AE。
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
        // [L1FreeTop12.S2 2026-09-11] 后置自增形：旧值留在 %ecx，写回复用
        // `lea 0x1(%ecx),%edx`（ORIG 0x082f8a7e/0x82f8a81 实证 2 条），替代
        // `mov/add/mov` 3 条形（`+=1`/`1+x`/`x+1` 均 3 条）。同 P187/参
        // LinkClosureB_Misc.cpp:483。
        if (logCount++ % 1000 == 0)
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
}