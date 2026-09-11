// ORIG 0x081add04：与 confirm 同构，packet_id=4、__FUNCTION__="cancel"。
// [b28d-D43] N82/N95 帧/槽平移：set_data_header 返回槽 ORIG -0x1c vs 4.4 -0x2c（ORIG 0x081add04）。
int NCashImpl::cancel(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x04);
    msg << trFlag;
    msg << trId;

    // [b28d-solo-nexon_cash] P198①：同 confirm（ORIG 0x081add86 预清）。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b28d-solo-nexon_cash] P212：具名 in 承载 get_data_header 的 sret，令 4.4
    // 槽位与 ORIG 全等（msg@-0x1c / sret@-0x34）⇒ AE。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    (void)in;
    // [b28d-solo-nexon_cash] 真语义 bug：同 confirm（ORIG 0x081ade11 后
    // 0x081ade16 jmp <leave>，errCode!=3 时不打印）。
    if (errCode != 3)
        return handle_cash_error(errCode);
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);  // 共用格式串
    return 0;
}