bool CmmChannelProxy::ConnectBridgeServer()
{
    sockaddr_in addr;
    std::memset(&addr, 0, 0x10);
    // ORIG 0x08106287：socket 返回值直写成员 m_sock（this+0x18），后续每处成员直读。
    m_sock = socket(AF_INET, SOCK_STREAM, 0);          // socket(2,1,0)
    if (m_sock < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't create socket");
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_ip.c_str());    // inet_addr(c_str(this))
    addr.sin_port = htons((unsigned short)G_CEnvironment()->get_tcp_port_channel());
    // ORIG 0x08106336 区装载序 = addrlen→%ecx、&addr→%edx、m_sock→%eax
    // （右起第三实参最先落寄存器）。4.4 恒把 %ecx 装载推迟到 m_sock 之后
    // ⇒ 唯一能翻转装载序的手段是把 addrlen 做成 volatile 局部（探针 f2 证），
    // 但 4.4 的 volatile 特判把它塞进更远槽、并连带把 addr/nonblock 等**全部**
    // 后续槽号推偏：实测 volatile + volatile 垫块回推的写法让本函数
    // ndiff 2 → 20（非统一平移，无法补偿）⇒ 两难坐实，记 N82 族封顶。
    int addrlen = 16;
    if (connect(m_sock, reinterpret_cast<sockaddr*>(&addr), (socklen_t)addrlen) < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't connect channel bridge server");
        return false;
    }
    int nonblock = 1;
    ioctl(m_sock, FIONBIO, &nonblock);                 // 0x5421
    return true;
