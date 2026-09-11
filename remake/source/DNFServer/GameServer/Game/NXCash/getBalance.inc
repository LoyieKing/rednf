// ORIG 0x081ad49e：packet_id=0 应答 err==0 后读余额。
// [b28d-D43] N82/N95 帧/槽平移（81/104）（ORIG 0x081ad49e）。
int NCashImpl::getBalance(const char* userId, int& balanceOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x00);
    msg << userId;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081ad4de 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081ad58c 直接复用
    // get_data_header 的 sret 对象（-0x34）继续 `in >> balanceOut`（读 body
    // 第 4 个字段）；旧形重新 wrap 入包 body 从头读（字段错位 + 多 3 调用）。
    // P171/P212：三元组无预清；具名 in 承载 sret（与 msg@-0x1c 同布局）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 0)
        return handle_cash_error(errCode);

    in >> balanceOut;
    printf("[%s] Remain cash: %d\n", __FUNCTION__, balanceOut);
    return 0;
}