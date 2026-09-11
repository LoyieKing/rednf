// ORIG 0x081ad7ae：subCmd∈{60,62} 时 streamType 替换为 subCmd，其余同上。
int NCashImpl::purchase(unsigned long long productSeq, int cashType,
                        int cashAmount, const char* param1, const char* param2,
                        NXTransaction& trOut, short subCmd)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x01);
    // [b23S3C] 双臂各直推一次（ORIG +0x68/+0x7d 两块内各 call lsEs、jmp 共享
    // 后继）——具名 streamType 合流形少一条 lsEs（P167 实证 MISSING lsEs）。
    if (subCmd == 0x3c || subCmd == 0x3e)
        msg << subCmd;
    else
        msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    // [b23S3C] P197③：ORIG rc 槽两段式（length 前先 movl $0,-0xc）。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b23S3C] 三元组无预清（ORIG sret call 前无 movb/movl $0，槽仅由
    // get_data_header 填写）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b23S3C] P192：sret 返回值续读 trOut（消 EXTRA C1/wrap/body/body_length）。
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