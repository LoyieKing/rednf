// ORIG T 0x088a3322：初值 0，取 m_listMap 最大 key。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
int BuyUpgradeData::getMaxLevel() const
{
    // b28d-solo-advancealtar_misc：IDENTICAL/AE 复验通过（fresh .o compare_tu clean，
    //   49=49 逐条全等，仅 .o 未解析重定位伪差）。ORIG @0x088a3322。
    // b28d-35: IDENTICAL_AE（49/49 clean 全等）。ORIG @0x088a3322；ppEi 后置
    //   三槽形 + advancealtar.h enum 化后重编复验通过。
    // b28c-L5：ORIG 0x088a3322 循环增量段调
    //   `_ZNSt23_Rb_tree_const_iterator...ppEi`（**后置** ++，带独立 sret 临时槽
    //   -0x10，it=-0x18 / end=-0x14 三槽并存）。4.4 对 `++it` 走 ppEv（返回引用、
    //   免临时槽）⇒ 迭代器槽阶梯少一格、cond 段整体错位（原 ndiff 26）。
    //   改后置形即复现 ORIG 三槽 + ppEi 调用（P28 声明序即栈槽序的增量侧推论）。
    // [W13C 2026-09-11 落地] 符号轴：ORIG 0x088a3322 两处 key 读取的 call 目标是
    //   `_ZNKSt23_Rb_tree_const_iterator<...>::deEv`（**operator\***），我方原 `it->first`
    //   发射 `...ptEv`（**operator->**）⇒ 我方多一份 ORIG 全 ELF 无的弱实例、少一份
    //   ORIG 有的 deEv 实例。改 `(*it).first` 后 routed 口径实测：
    //   `deEv` → IDENTICAL、`ptEv` 消失（identical 273→274、missing 2→1，其余三项不变）。
    //   语义等价（`(*it).first` ≡ `it->first`，同为 const int 左值）。
    int maxLevel = 0;
    for (LevelFieldDataMap::const_iterator it = m_listMap.begin();
         it != m_listMap.end(); it++)
    {
        if (maxLevel < (*it).first)
        {
            maxLevel = (*it).first;
        }
    }
    return maxLevel;
}