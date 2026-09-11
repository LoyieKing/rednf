// ORIG W 0x081348ce _ZN5CUser17DisableCharacInfoE13CACHE_SECTION：
// movb $0,(this+0x8d0fd+section) —— m_pad8d0fd[section] 清零。
void CUser::DisableCharacInfo(CACHE_SECTION section)
{
    m_pad8d0fd[static_cast<int>(section)] = 0;
}