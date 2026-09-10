int NCashImpl::confirm(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x03);
    msg << trFlag;
    msg << trId;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081adc1d `movl $0x0,-0xc` 预清
    // rc 后才 call send_msg ⇒ 必须两句式；合并初始化式丢预清行。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P171：ORIG get_data_header 前无 movb/movl $0
    // 预清（槽仅由被调填写）⇒ 三元组声明不得带初始化。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b28d-solo-nexon_cash] P212：具名 in 承载 get_data_header 的 sret，
    // 令 4.4 槽位与 ORIG 全等（msg@-0x1c / sret@-0x34）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    (void)in;
    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081adce3..0x081adcf0 为
    // errCode!=3 → call handle_cash_error; jmp <leave>（跳过 printf 并返回
    // 其错误码）；旧形忽略返回值后继续打印成功行 ⇒ 改为 return。
    if (errCode != 3)
        return handle_cash_error(errCode);
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);
    return 0;
