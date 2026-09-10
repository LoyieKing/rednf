int NCashImpl::chargeCash(char* sessionKey, unsigned char h1, unsigned char h2,
                          int iv, char* pA, char* pB, char* pC, char* pD,
                          int code, NXTransaction& trOut)
{
    // [b28d-solo-nexon_cash] ORIG 4 个函数级 static 缓冲（nm 实证）：
    // _ZZ...chargeCash...E4data@0x09405640(0x3e8) / E9wide_data@0x9405a40(0xfa0)
    // / E8enc_data@0x94069e0(0xfa0) / E9send_data@0x9407980(0x3e8)，均 b 段。
    // 旧 struct+ccBufs() getter 造发明符号且访问经局部引用缓存；ORIG 每次直压
    // 静态地址立即数 ⇒ 必须函数内 static 声明。
    // [b28d-solo-nexon_cash] nm 尺寸实证：data/send_data 0x3e8、wide_data/enc_data
    // 0xfa0（wchar_t=4B ⇒ wchar_t[1000]；旧 [2000] 使 memset 长度 0x1f40 与 ORIG
    // 的 0xfa0 不符且符号尺寸翻倍）。
    static char data[1000];
    static wchar_t wide_data[1000];
    static wchar_t enc_data[1000];
    static char send_data[1000];

    memset(data, 0, sizeof(data));
    memset(wide_data, 0, sizeof(wide_data));
    memset(enc_data, 0, sizeof(enc_data));
    memset(send_data, 0, sizeof(send_data));

    ByteBuffer msg = set_data_header(0x02, 0x01);

    // [b28d-solo-nexon_cash] ORIG 0x081ade75..0x081ade83：set_data_header 后
    // 立即 3 条 movl（-0x2c=0xe8c3f80b 未消费槽 + -0x28/-0x24=qword key
    // 0x00000000F3D8266F）⇒ 原码为具名局部（P232 dead 标量照样占槽）。
    int deadField = static_cast<int>(0xe8c3f80bu);
    unsigned long long frameKey = 0x00000000F3D8266FULL;
    (void)deadField;

    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081adefa..0x081adf2a 在栈上
    // 构造 27 字节 fmt = "%s|%i|%i|%i|%s|%s|%s|%s|%i"（9 token），并在
    // 0x081adf3c..0x081adf6a 压 9 个变参 sessionKey,h1,h2,iv,pA,pB,pC,pD,code
    // （0x18(%ebp)=iv 与 0x2c(%ebp)=code 均参与）⇒ 旧形把 iv 当 char* cast、
    // 少传 pD/code 且 fmt 仅 8 token（输出串错位）。
    char fmt[] = "%s|%i|%i|%i|%s|%s|%s|%s|%i";
    snprintf(data, sizeof(data), fmt,
             sessionKey,
             static_cast<int>(h1),
             static_cast<int>(h2),
             iv,
             pA, pB, pC, pD,
             code);

    size_t len = strlen(data) + 1;
    mbstowcs(wide_data, data, len);

    // [b28d-solo-nexon_cash] P0/P211：ORIG 0x081adfc4 的 wcslen 结果直接
    // `lea (%eax,%eax,1),%edx`（无 wlen 具名槽）；Adler32 返回值零扩展后存
    // 8 字节槽（0x081adfdd `mov $0x0,%edx` + -0x18/-0x14）⇒ 原码是
    // unsigned long long adler。
    unsigned long long adler = Encryption::Adler32(
        reinterpret_cast<char*>(wide_data),
        static_cast<unsigned int>(wcslen(wide_data) * 2));

    Encryption::EncodeDressStringWide(wide_data, enc_data,
                                      0x1b1bde64u);

    size_t enclenChars = wcslen(enc_data);
    wcstombs(send_data, enc_data, enclenChars);

    msg << send_data;
    msg << frameKey;                      // [-0x28/-0x24] 帧 key
    msg << adler;

    // [b28d-solo-nexon_cash] P198①：ORIG 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P171：三元组无预清。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    get_data_header(ackPid, errCode, reqId);

    if (errCode != 2)
    {
        // [b28d-solo-nexon_cash] §4.1/P154：删发明符号
        // _ZN5nexon4cash12_GLOBAL__N_114traceRecvErrorEiPKc（ORIG nm 无此符号）。
        // ORIG 0x081ae105..0x081ae145 为就地具名 cMyTrace 两句式（ctor + op()）。
        cMyTrace trace(__FUNCTION__, 0x303, 0);
        trace("NCashImpl::chargeCash Recv Error!! packet_id:%d, data:%s",
              static_cast<int>(errCode), data);
        return static_cast<int>(errCode);
    }

    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] CHARGE_CASH OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type), trOut.m_id);
    return 0;
