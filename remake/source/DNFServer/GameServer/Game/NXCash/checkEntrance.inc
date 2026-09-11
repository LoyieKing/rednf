// ORIG 0x081ad316：手组包头（不走 set_data_header！仅自增 requestID 不写入）。
// [b28d-solo-nexon_cash] N 封顶：rc 预清/三元组无预清/声明序已归位后，残余
// 仅为 ORIG 0x081ad430 wrap 实参装载寄存器差（ORIG `lea in,%edx; mov body,%eax`
// vs 4.4 `mov body,%edx; lea in,%eax`，P75/N7 跨版本装载调度，our 107 vs
// orig 106），语义等价。
int NCashImpl::checkEntrance(const char* userName, char& outStatus)
{
    if (!reconnect())
        return 2;

    unsigned char typeByte = static_cast<unsigned char>(m_fieldD);

    ByteBuffer out(reinterpret_cast<char*>(m_outMsg.body()), 0x200);
    out << static_cast<unsigned char>(0x03);
    out << static_cast<unsigned char>(0x01);
    out << typeByte;
    out << userName;
    _increaseRequestID();  // ORIG: 自增但不入包

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081ad3a9 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(out.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P212/P171：ackPid/statusByte 先声明占浅槽
    // （ORIG -0x21/-0x22）、in 后声明占深槽 -0x34；ORIG 无 movb $0 预清。
    unsigned char ackPid;
    unsigned char statusByte;
    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> ackPid;
    in >> statusByte;
    outStatus = static_cast<char>(statusByte);
    printf("[%s] CheckEntrace: %d\n", __FUNCTION__,
           static_cast<int>(outStatus));  // ORIG 格式串 typo 忠实保留
    return 0;
}