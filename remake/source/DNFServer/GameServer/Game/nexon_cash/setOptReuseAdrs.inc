// ORIG 0x081af8bc：setsockopt(SOL_SOCKET, SO_REUSEADDR=2, &flag(0/1), 4)。
// flag 两语句式：死预置 0（0x081af8c8 movl $0,-0xc）+ 显式 if/else 各支内存存储
// （0x081af8d5 movl $1 / 0x081af8de movl $0），三元写法会被折成寄存器选择形。
bool TCPSocket::setOptReuseAdrs(bool on)
{
    int flag = 0;
    if (on)
        flag = 1;
    else
        flag = 0;
    if (::setsockopt(m_socket, 1, 2, &flag, sizeof(flag)) < 0)
        return false;
    return true;
}