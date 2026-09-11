// ORIG 0x081af532：
//   m_socket = socket(AF_INET=2, SOCK_STREAM=1, IPPROTO_TCP=6);
//   if (m_socket == -1) { printf("Could not create a TCP socket : %d\n",
//        errno); return false; }  // 字符串 @0x08bb4aac
//   setOptLinger(false); setOptNagle(false); return true;
bool TCPSocket::open()
{
    m_socket = ::socket(2, 1, 6);
    if (m_socket == INVALID_SOCKET)
    {
        int err = errno;
        // [P6AuditOldN.P6S2 2026-09-11 · §6.5 串审计] ORIG .rodata @0x08b6caac
        //   逐字节 = "Could not create a TDP socket : %d\n"（TDP 为 ORIG 原拼写）。
        printf("Could not create a TDP socket : %d\n", err);
        return false;
    }
    setOptLinger(false);
    setOptNagle(false);
    return true;
}