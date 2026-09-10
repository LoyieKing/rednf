bool CTitleBook::loadData(CUser* pUser, char* pData)
{
    // [b28c-6] N7：ORIG 虚调用 `mov (%eax),%edx; mov this,%eax; mov %eax,(%esp);
    // call *%edx` vs 4.4 恒目标 %eax + this 走 %edx（N7 寄存器互换，临时/引用/
    // 限定名均无效）。ndiff 8 封顶。
    _reset();
    m_user = pUser;
    std::memcpy(&m_book, pData, 0x6b3a);
    return 1;
