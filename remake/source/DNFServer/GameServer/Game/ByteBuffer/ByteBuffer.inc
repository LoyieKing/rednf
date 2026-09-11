// ORIG 0x081ae9e2：start=write=read=buffer; capacity=len。
ByteBuffer::ByteBuffer(char* buffer, unsigned int len)
    : m_start(buffer), m_writePtr(buffer), m_readPtr(buffer), m_capacity(len)
{
}

// ORIG 0x081acd1e：len=wcslen；字节数=len*2；超 0xffff 抛 invalid_argument；
// 空间足够则写 ushort 长度前缀（大端）+ wcsncpy + 游标前进 len*2。
// [b28d-D43] N82/N89 大函数（124/124）：STL 实例化 + 槽平移（ORIG 0x081acd1e）。
ByteBuffer& ByteBuffer::operator<<(const wchar_t* value)
{
    // [b28d-solo-nexon_cash] P0/P48：ORIG 0x081acd2d 的 wcslen 结果直接
    // `add %eax,%eax`（无具名 len 局部槽），且 0x081ace63 在 wcsncpy 前二次
    // 调 wcslen ⇒ 原码无 len 局部，字节数就地计算。
    size_t bytes = wcslen(value) * 2;
    if (bytes > (size_t)std::numeric_limits<unsigned short>::max())
        // [P6AuditOldN.P6S2 2026-09-11 · §6.5 串审计] ORIG .rodata @0x08b6bc00
        //   逐字节 = "string length too long: must shorter than max(unsigned short)"
        //   （我方旧串 "string too long" 为其前缀截断）⇒ 运行期可见文本，按 ORIG 还原。
        throw std::invalid_argument(
            "string length too long: must shorter than max(unsigned short)");
    if (static_cast<unsigned int>(length() + bytes) <= m_capacity)
    {
        *this << static_cast<unsigned short>(bytes);
        wcsncpy(reinterpret_cast<wchar_t*>(m_writePtr), value, wcslen(value));
        m_writePtr += bytes;
    }
    return *this;
}