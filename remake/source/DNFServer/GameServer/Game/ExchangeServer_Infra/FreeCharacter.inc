// FreeCharacter ORIG 0x8490bf4：池 Free；(1000-remain)!=map.size() 时 cMyTrace 告警
// （line=0x4e）。
// [b28d-lane15] 已修：Size() 常量 1000（P119 头内 inline 4.4 恒发 call）、trace
// 全表达式临时链（P37/P227）、格式串按 ORIG .rodata 0x08c78398 实测
// "FreeCharacter() error (%d,%d,%d)" 且实参序 (1000,remain,size)。
// [b28d-solo-ExchangeServer_Infra 判词·AE] 原 N 判词（N7/N18a 四段中转不可造）作废：
// ORIG 0x8490c31 链 `mov $0x3e8,%edx; mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax;
// mov %eax,%ebx` 的 ecx→eax 中转来自 **unsigned 域**（P248 档③ cast 旋钮）——条件写
// `static_cast<unsigned int>(1000 - m_pool.Remain()) != m_map.size()` 即逐条全等。
void CCacheCharacterMgr::FreeCharacter(CACHE_CHARACTER_TYPE* mem)
{
    Guard<Mutex> g(&m_mutex);
    m_pool.Free(mem);
    // [b28d-lane15] ORIG 0x08490c31 `mov $0x3e8,%edx; mov %edx,%ecx; sub %eax,%ecx`
    // —— 4.1 把头内 `int Size() { return N; }` 折成常量 1000（**无 Size call**）；
    // 4.4-O0 对同一头内定义恒发 out-of-line call（P119）。按 ORIG 形直写 1000
    // （N=1000 模板实参，语义等价；同时消掉 ORIG 不存在的 Size 调用点）。
    // 且 ORIG 的 used 无栈槽（`mov %eax,%ebx` 寄存器保活）⇒ 不设具名局部（P0）。
    if (static_cast<unsigned int>(1000 - m_pool.Remain()) != m_map.size())
    {
        // [b28d-lane15] ORIG .rodata 0x08c78398 实测 = "FreeCharacter() error (%d,%d,%d)"，
        // 实参装载 0x8/0xc/0x10 = 1000 / remain / map.size()（非我方旧推断的
        // "map size/pool remain/max" 且顺序相反）。
        // 且 ORIG 先算完两个实参再构造 tr（P37/P227 全表达式临时链）。
        // [P6AuditOldN 2026-09-11 §6.5 真修] 旧手写串只到函数名（缺参数表），ORIG prett 实读
        // （0x08c78398 邻域符号 `_ZZN15exchange_server18CCacheCharacterMgr13FreeCharacterEP20CACHE_CHARACTER_TYPEE19__PRETTY_FUNCTION__`
        // size 0x4f）= "void exchange_server::CCacheCharacterMgr::FreeCharacter(CACHE_CHARACTER_TYPE*)"。
        // 本仓类/命名空间与参数拼写一致 ⇒ `__PRETTY_FUNCTION__` 展开逐字节相同，同时补 prett 符号。
        cMyTrace(__PRETTY_FUNCTION__, 0x4e, 0)(
            "FreeCharacter() error (%d,%d,%d)",
            1000, m_pool.Remain(), static_cast<int>(m_map.size()));
    }
}