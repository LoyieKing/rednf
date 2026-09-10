bool DbmwClient::SendHeader(char type, int bodyLen)
{
    // ORIG @0x08120654：18 字节头 {0x15,type,len+0x12,12B zero} 后 send。
    // ORIG 逐字段直 disp store（total 槽即 header+2）→ 具名视图结构。
    struct __attribute__((packed)) {
        unsigned char b0;
        unsigned char b1;
        int total;
        int z2;
        int z3;
        int z4;
    } header;
    header.b0 = 0x15;
    header.b1 = (unsigned char)type;
    header.total = bodyLen + 0x12;
    header.z2 = 0;
    header.z3 = 0;
    header.z4 = 0;

    ssize_t sent = ::send(m_sock, &header, 0x12, 0);
    if (sent <= 0)
        return false;
    return true;
