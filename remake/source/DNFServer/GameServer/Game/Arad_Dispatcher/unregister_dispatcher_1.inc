bool Arad_Dispatcher_Manager::unregister_dispatcher(int code)
{
    // ORIG 0x0818fed6：inter[code] 判空→虚析构调用块（Ghidra unreachable）→置空返回 0；
    // 否则 db[code] 同样处理；两处均非空返回 1。按报告反编译 C 转写：
    // [b28d-23 语义补齐/P184 死块照抄] ORIG 0x0818fee8 有具名局部
    // `d = inter[code]`（-0x10 槽往返，P0）且块内为 `delete d` 的 4.1 展开
    // （内建判空死块 0x0818feee + 虚 D0 调用 0x0818fef4..f04，Ghidra 标
    // unreachable）。原实现漏 delete ⇒ 补回；db 组同（-0xc 槽 + 0x354 折叠）。
    Arad_Dispatcher* d = m_interDispatchers[code];
    if (d == 0)
    {
        delete d;
        m_interDispatchers[code] = 0;
        return false;
    }
    Arad_Dispatcher* d2 = m_dbDispatchers[code];
    if (d2 == 0)
    {
        delete d2;
        m_dbDispatchers[code] = 0;
        return false;
    }
    return true;
