// ORIG 0x086940fc (sz 0x64):
//   if (m_offset + count >= m_size) return 0;
//   for (i=0; i<count; ++i) { m_buffer[m_offset++] = src[i]; }
//   return 1;
bool CPacketHelper::padding(unsigned char* src, int count)
{
    if (static_cast<int>(static_cast<unsigned>(m_offset)) + count >= m_size)
        return false;
    // ORIG 0x08694126：`m_buffer[m_offset++] = src[i]` 后缀自增形——m_buffer
    // 落 %ecx、src+i 落 %edx、m_offset 留 %eax 跨 store（拆两句使 4.4 把
    // src+i 占 %eax 并重载 m_offset，+1 条；b28c-lane25 探针 /tmp/l25probe/pad.cpp fB）。
    for (int i = 0; i < count; ++i)
    {
        m_buffer[m_offset++] = static_cast<char>(src[i]);
    }
    return true;
}