int CBingo::initUserData(BingoData& data)
{
    // ORIG 0x080cadd4：实参 `mov (%eax),%eax`（m_eventId 4B）；call 结果 `mov %eax,%edx;
    // mov 0xc(%ebp),%eax; mov %dl,0x4(%eax)` 重载 &data 字节直落 m_pad4；返回 1
    data.m_pad4 = static_cast<char>(calBoradMatchLine(data.m_eventId));
    return 1;
