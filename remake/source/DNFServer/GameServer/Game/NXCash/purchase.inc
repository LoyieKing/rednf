int NCashImpl::purchase(unsigned long long productSeq, int cashType,
                        int cashAmount, const char* param1, const char* param2,
                        NXTransaction& trOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x01);
    msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    // [b27L8] ORIG send_msg call 前 movl $0x0,-0xc 预清 ⇒ P198① 两句式。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b27L8] ORIG 无 ackPid/errCode/reqId 预清（movb/movl 缺失）⇒ 未初始化声明。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b23S3C] P192：以 sret 返回值续读 trOut（消本函数 C1/wrap/body/
    // body_length EXTRA×4）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
