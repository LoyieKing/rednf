// ORIG 0x081affd0：具名 ret 槽（-0xc cmpl/jns）+ 两条 dead 数组写
// （0x081affe4 movl $0,-0x10 / 0x081affeB movl $4,-0x14，技巧31 dead 槽撑帧）。
bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    int dead_vals[2];
    dead_vals[1] = 0;   // ORIG 0x081affe4
    dead_vals[0] = 4;   // ORIG 0x081affeb
    int ret = ::setsockopt(m_socket, 1, 7 /*SO_SNDBUF*/, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
}