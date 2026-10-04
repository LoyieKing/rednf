// ===================== initRandomOptionItem =====================
// [b28d-solo-CDataManagerMethods] DIFF 封 N107/N18a：P167 CLEAR（14/14）。
// 唯一残差 = `&innerIt->second.m_field0`：ORIG `lea 0x4(%eax),%ecx` 单条，
// 4.4 恒 `add $0x4,%eax; mov %eax,%edx` 两段；探针（引用/解引用/指针算术/
// 字段 unsigned long 化）全产 add ⇒ 源形不可择。
// ORIG 0x083580aa：遍历 +0x4e70 的 map<int,map<int,BaseItemValue>>，
// 对每个内层元素以 make_pair(值, 递增序号) 插入 +0x4e88 反查表；
// 任一插入冲突立即返回 0，全部成功返回 1。（计数器每个外层键重置。）
bool CDataManager::initRandomOptionItem()
{
    //   b28 BM4 判词（N18a/N4 在册）：fresh 实残 = make_pair 首 ref 实参 ORIG
    //   `lea 0x4(%eax),%ecx` 单条折叠 vs 4.4 `add $0x4,%eax; mov %eax,%edx`
    //   两段+中转 hop（+1 行）；P30 穷举 add/lea 与寄存器名不可齐。其余
    //   迭代器/插入流逐条同形（含 ++count 逗号序）。语义等价封顶。
    //   [l5f 2026-09-11 · 探针复核] /tmp/probe_mp.cpp m1（C cast 取址）/m2（显式
    //   引用模板实参 make_pair<unsigned long&,int&> + *(unsigned long*)& 解引用）
    //   两形产物**逐条全同**（add $0x4,%eax; mov %eax,%edx；call 符号同为
    //   make_pairIRmRiE）⇒ 取址 lowering 系 4.1↔4.4 代差（N107 同族），源形不可择；
    //   ORIG 0x083580e9 单条 lea 为 4.1 独有形 ⇒ 封 N18a。
    int count = 0;
    for (std::map<int, std::map<int, BaseItemValue> >::iterator outerIt =
             m_randomOption.m_baseItems60.begin();
         outerIt !=
             m_randomOption.m_baseItems60.end();
         ++outerIt)
    {
        count = 0;
        // ORIG：++innerIt 先于 ++count 发射 ⇒ ++count 位于 for-third（逗号序）。
        for (std::map<int, BaseItemValue>::iterator innerIt =
                 outerIt->second.begin();
             innerIt != outerIt->second.end(); ++innerIt, ++count)
        {
            if (!m_randomOption.m_map78.insert(
                         std::make_pair(
                             (unsigned long&)innerIt->second.m_field0,
                             count)).second)
            {
                return 0;
            }
        }
    }
    return 1;
}