void ncl_make_mac(char* out)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::memcpy(ifr.ifr_name, "eth0", 5);
    ioctl(sock, SIOCGIFHWADDR, &ifr);
    close(sock);
    int off = 0;
    // [b28d-D43] 循环变量复用 sock 槽（ORIG 0x0857247c/0x08572483 同一 -0x10
    // 槽：4.1 对已死 sock 做 lifetime 复用；4.4 不复用，独立 i 会多占一槽并使
    // ifr 整体浅移 4 ⇒ 源码改用同一变量，槽序即与 ORIG 全等）。
    for (sock = 0; sock <= 5; ++sock)
    {
        std::sprintf(out + off, "%03d",
                     static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[sock]));
        off += 3;
        out[off] = '\0';
    }
