// ---------------------------------------------------------------------------
// CUser::EnableCharacInfo(CACHE_SECTION section) W 0x081253f4 sz0x13
// 报告缺失（nm 0x081253f4 定址后 objdump 忠实转写）：
//   movb $1, 0x8d0fd(%edx,%eax,1) → this + section + 0x8d0fd
//   即 m_pad8d0fd[section] = 1（缓存段使能标志数组）。
// ---------------------------------------------------------------------------
void CUser::EnableCharacInfo(CACHE_SECTION section)
{
    m_pad8d0fd[static_cast<int>(section)] = 1;
}