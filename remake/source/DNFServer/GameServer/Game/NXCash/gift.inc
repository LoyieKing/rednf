// ORIG 0x081ad9c4：两段附加串位于 m_paramB/m_fieldC 之后。
// [b28d-D43] N82 帧：ORIG sub $0x68 vs 4.4 push %ebx+sub $0x74（ORIG 0x081ad9c4）。
int NCashImpl::gift(unsigned long long productSeq, int cashType,
                    int cashAmount, const char* targetUserId,
                    const char* message, const char* extra1,
                    const char* extra2, NXTransaction& trOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x07);
    msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << targetUserId;
    msg << message;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);
    msg << extra1;
    msg << extra2;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081adae2 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] 真语义 bug（同 getBalance）：ORIG 0x081adb81 复用
    // get_data_header 的 sret 对象（-0x34）继续读 trOut.m_type/m_id；旧形重新
    // wrap 入包 body 从头读（字段错位 + 多 C1/body/body_length/wrap 4 调用）。
    // 三元组无预清（P171），具名 in 承载 sret（msg@-0x1c 同布局）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}