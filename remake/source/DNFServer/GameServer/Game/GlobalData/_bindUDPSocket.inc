int UdpThread::_bindUDPSocket(unsigned short port)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    // ORIG 0857c3be：栈槽 -0x20 sockaddr_in（memset 0x10）；写序 movw $2 @+0（family）
    // → htonl(0) 结果 @-0x1c（s_addr）→ htons 结果 @-0x1e（sin_port），见 _bindUDPSocket.md。
    struct sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(port);
    if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), 0x10) < 0)
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x84,
                              "[ERROR]UDP Thread Socket Binding ERROR");
        int err = *__errno_location();
        // ORIG 0x0857c4a2..：err 先 `mov %eax,-0xc(%ebp)` 再整段提升为
        // `mov -0xc(%ebp),%eax; cmp $0x62,%eax,je / cmp $0x63,%eax,je /
        // test %eax,%eax,je / jmp` 的判定链，三个 case 体依次后置 ⇒ 整数
        // switch 降形态（非 if/else-if 交错），case 0 为空实现。
        switch (err)
        {
        case 0x62:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x9e,
                                  "Port %d for receiving UDP is in use\n",
                                  (int)(unsigned short)addr.sin_port);
            break;
        case 0x63:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa2,
                                  "Cannot assign requested address\n");
            break;
        case 0:
            break;
        default:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa6,
                                  "Could not bind UDP receive port. Error= %d , strerror = %s\n",
                                  err, strerror(err));
            break;
        }
        exit(-1);
    }
    if (!CNetwork<4096, 450000>::SetNonBlock(fd))
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xb0,
                              "Fail to SetNonBlock UdpThread.");
        exit(-1);
    }
    return fd;
