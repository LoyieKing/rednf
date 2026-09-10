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
        cMyTrace("exchange_server::CCacheCharacterMgr::FreeCharacter", 0x4e, 0)(
            "FreeCharacter() error (%d,%d,%d)",
            1000, m_pool.Remain(), static_cast<int>(m_map.size()));
    }
