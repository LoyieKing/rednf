// ORIG 0x081af7fe (sz 0xd)：mov 0x8(%ebp),%eax; mov (%eax),%eax;
// cmp $0xffffffff,%eax; pop; ret —— 尾置 return 的 if 被折叠成裸 cmp（技巧6变体）。
void TCPSocket::shutdown(int how)
{
    // 忠实转写：ORIG 本函数未发起 shutdown 系统调用（内联裁剪后的残体）。
    if (m_socket == INVALID_SOCKET)
        return;
}