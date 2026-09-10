bool CodeHackChecksumEx::IsRightChecksum(const CodeHackChecksum* p) // T @0x0808a8c8
{
    // [b28d-lane16] 与 CodeHackChecksumData::IsRightChecksum 同形：ORIG 0x0808a8ce
    // `call __normal_iterator::C1Ev` 默认构造 + begin() 临时槽 -0x1c + 解引用结果
    // 具名指针局部 -0xc（P50 复活 / P0 槽往返）。
    checksum_iterator it;
    it = m_checksums.begin();
    for (; it != m_checksums.end(); ++it)
    {
        const CodeHackChecksum* e = &(*it);
        // [b28d-solo-CodeHackChecksum] memcmp 实参须显式 `(const char*)` 形：
        // ORIG 0x0808a8ce 先载 p→%edx / e→%eax 再压常量，4.4 对裸指针实参
        // 恒即时右到左压参（常量先）；显式 cast 使 4.4 复现「先求值后压栈」
        // 调度（P268 cast 旋钮族，Data 版同形 0x0808a69e）。
        if (std::memcmp((const char*)e, (const char*)p, 0x20) == 0)
            return true;
    }
    return false;
