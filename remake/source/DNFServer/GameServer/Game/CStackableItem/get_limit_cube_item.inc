// [b28d-solo-CStackableItem] NEAR 118=118。已修：`sum` 局部 unsigned 化（新范式
// P218/P310 扩展）⇒ 比较翻正为 ORIG `mov rand,%eax; cmp sum,%eax; ja`（此前 int sum
// 恒产 `mov sum,%eax; cmp rand,%eax; jb`）。残余 N95 封顶：range 槽 ORIG -0x38
// （在 begin/end/pp 临时槽之后），4.4 恒 -0x44（临时槽之前）；把 range 声明移到循环后
// 实测反向（range -0x48/it -0x44）；本波补测「移到 total 之后」= 零效（range 仍 -0x44，
// 4.4 按首次使用序把取地址具名标量排到匿名临时区之后）⇒ 槽分配方向不可择，判 N95/N93。
// ORIG 0x850fbe4 两循环复用同一 const_iterator 槽 -0x48（第二循环 `it = begin()`
// 重新赋值），循环体先 operator*（deEv）取引用地址入槽（-0x10/-0xc），增量段 ppEi。
// [S1Charac 2026-09-11 复核] 118=118、无指令增删：diff 43+/43- 全为 begin/ctor/end/ppEi
// 调用块的**位置重排**（ORIG 把 end() 提到循环前存 -0x40、range 槽 -0x38；我方 range
// -0x44 且 end() 后置）⇒ 4.4 循环调度 + 槽分配差（N95/N93）；§6.5 无串引用 ⇒ CLEAR。
// [W13AStackableItem 2026-09-11 探针负结论·含 ORIG 指令形] range/it 槽交换
// （-0x38↔-0x3c）**不可翻**，两形均实测劣化：
//   ORIG 0x0850fc45 `lea -0x3c(%ebp),%eax`（第一循环 begin() 的 sret 目标 ⇒ it 槽 -0x3c）、
//        0x0850fc96 `mov %eax,-0x38(%ebp)`（`range = total` ⇒ range 槽 -0x38）、
//        0x0850fca2 `lea -0x38(%ebp),%edx`（randInt(range) 实参）⇒ ORIG: it -0x3c(1)/range -0x38(2)；
//   我方基线（slotmap 影子实测）：it -0x38(1)/range -0x3c(2)。
//   探针 v1（`range` 移至 it 之后 + 内联初始化 `unsigned long range = total;`）／
//        v2（同位置、保持 `range = total;`）⇒ **两形均新增劣化**（-0x44/-0x48 计数互换；
//        v2 ndiff 7→8），已回滚。判：4.1 按声明序而 4.4 按首次使用序分配取地址标量，
//        方向差不可由源码声明顺序反转（N95/N93 族封顶）。
void CStackableItem::get_limit_cube_item(std::pair<int, int>& out)
{
    unsigned long range;
    int itemIdx = 0;
    int count = 0;
    int total = 0;
    std::vector<stLimitCubeResult>::const_iterator it =
        m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        const stLimitCubeResult& r = *it;
        total += r.m_prob;
        it++;
    }
    range = total;
    unsigned int rand = m_pRand->randInt(range);
    // b28d-solo 修法：sum unsigned（裸比较翻操作数序，P218/P310 扩展；见函数头判词）。
    unsigned int sum = 0;
    it = m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        const stLimitCubeResult& r = *it;
        sum += r.m_prob;
        if (rand <= sum)
        {
            itemIdx = r.m_itemIdx;
            count = r.m_count;
            break;
        }
        it++;
    }
    out.first = itemIdx;
    out.second = count;
}