bool WindowsExeFile::GenerateChecksum(CodeHackChecksum* ck, long sectionIdx,
                                      unsigned long offset, unsigned long len) // 0x085743d4
{
    // [b28d-58] P0：ORIG 无 total 槽（`cmp 0x10(%ebp),%eax; setle %al; test;
    // je`），条件内联 size()；失败返回 0、成功返回 1（见 .h 返回类型判词）。
    if (static_cast<long>(m_commonSections.size()) <= sectionIdx)
        return false;
    {
        // [b28d-58] P157/P183：ORIG 每字段重取 operator[]（两次 call），不缓存
        // 引用（ORIG 0x085743d4 段两次 `call vector::operator[]`）；求值序为
        // 先 `(offset - start)`（第一次取 start）再 fileOffset（第二次）——
        // 中间值各占一槽（ORIG -0x10/-0xc/-0x14）⇒ 具名局部形。
        unsigned char* addr;
        unsigned long d;
        unsigned long sum;
        d = offset - m_commonSections[sectionIdx]->start;
        sum = d + m_commonSections[sectionIdx]->fileOffset;
        addr = m_image + sum;
        CalculateMemoryChecksum(ck, len, addr);
    }
    return true;
