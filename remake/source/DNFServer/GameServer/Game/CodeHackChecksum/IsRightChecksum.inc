bool CodeHackChecksumData::IsRightChecksum(const CodeHackChecksum* p) // T @0x0808a662
{
    // [b28d-lane16] ORIG 0x0808a668 `lea -0x14(%ebp); call __normal_iterator::C1Ev`
    // = 默认构造；0x0808a680 begin() sret -0x1c 后 `mov -0x1c,%eax; mov %eax,-0x14`
    // = 先声明后赋值（P50 复活形，缺默认 ctor 弱实例即 +2 missing）；循环体
    // 0x0808a69b `mov %eax,-0xc(%ebp)` = 解引用结果具名指针局部（P0 槽往返）。
    checksum_iterator it;
    it = m_checksums.begin();
    for (; it != m_checksums.end(); ++it)
    {
        const CodeHackChecksum* e = &(*it);
        // [b28d-solo-CodeHackChecksum] ORIG 0x0808a69e 先载 p→%edx / e→%eax 再压常量；
        // 4.4 对裸指针实参恒即时右到左压参（常量先）⇒ 显式 `(const char*)` cast
        // 复现 ORIG「先求值后压栈」调度（P268 cast 旋钮族；探针 /tmp/probe_chc2.cpp
        // g1 裸形 vs g2 cast 形）。
        if (std::memcmp((const char*)e, (const char*)p, 0x20) == 0)
            return true;
    }
    return false;
