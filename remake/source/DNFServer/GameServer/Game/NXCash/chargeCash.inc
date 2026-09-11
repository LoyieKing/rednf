// ORIG 0x081ade34：无 reconnect 前置；组"%s|%i|%i|%i|%s|%s|%s|%s|%i" 明文 →
// mbstowcs → Adler32(宽串字节) → EncodeDressStringWide(key=0x1b1bde64) →
// wcstombs 回 ASCII 密文入包 + 协议键 qword + adler qword。
// [b28d-solo-nexon_cash] 本波修真 bug 3 处：①4 个函数级 static 缓冲（ORIG 符号
// _ZZ...E4data/E9wide_data/E8enc_data/E9send_data，尺寸 0x3e8/0xfa0/0xfa0/0x3e8）
// 替代 struct+getter（旧形发明符号 + memset 长度 0x1f40 错）；②格式串 9 token
// （iv/code 为 %i，旧形漏传 pD/code 且 iv 当 char*）；③adler 为 unsigned long long
// （8 字节槽 0x081adfdd）。帧/槽已全对齐；N 封顶残余 = 4.1 局部物化/EH 段差
// （P0/N82 族，our 200 vs orig 196，多数为 .o 重定位伪差）。
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

    ByteBuffer msg = set_data_header(0x02, 0x01);

    // [b28d-solo-nexon_cash] ORIG 0x081ade75..0x081ade83：set_data_header 后
    // 立即 3 条 movl（-0x2c=0xe8c3f80b 未消费槽 + -0x28/-0x24=qword key
    // 0x00000000F3D8266F）⇒ 原码为具名局部（P232 dead 标量照样占槽）。
    int deadField = static_cast<int>(0xe8c3f80bu);
    unsigned long long frameKey = 0x00000000F3D8266FULL;
    (void)deadField;

    // [P331 语句序] ORIG 0x081ade8a..0x081adec1 的 4 次 memset 落在
    // set_data_header(0x081ade6d) 与 frameKey 3 条 movl 之后；旧形把 4 个 memset
    // 写在最前 ⇒ 整块前移（16 条指令位移）。
    memset(data, 0, sizeof(data));
    memset(wide_data, 0, sizeof(wide_data));
    memset(enc_data, 0, sizeof(enc_data));
    memset(send_data, 0, sizeof(send_data));

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

    // [P232 dead 标量占槽] ORIG 0x081ae004..0x081ae015：`call wcslen` 后
    // `add $0x1,%eax; add %eax,%eax; mov %eax,-0x10(%ebp)` 把 (wcslen+1)*2
    // 物化进 -0x10 槽（该值后续**未被消费**）；0x081ae01f 再取一次
    // wcslen(enc_data) 直接作 wcstombs 第 3 参。旧形单次 wcslen + 具名复用
    // ⇒ 少 1 次 call + 2 条 add（正 gap 段之一）。
    int encSizeChars = static_cast<int>((wcslen(enc_data) + 1) * 2);
    (void)encSizeChars;
    wcstombs(send_data, enc_data, wcslen(enc_data));

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
    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081ae0cf 的 `lea -0x80(%ebp)` 即
    // get_data_header 的 sret 对象，随后 0x081ae15a/0x081ae16f 两条
    // `ByteBuffer::operator>>` 继续用同一对象（sret 与 in 同槽）⇒ 原码是
    // `ByteBuffer in = get_data_header(...)` 续读；旧形丢弃返回值后
    // 重新 `ByteBuffer::wrap(m_inMsg.body(), body_length())` 从头读入包 body
    // ⇒ 字段错位（读到 ack/err 而非 db_id/tid）+ 多 5 调用/16B 栈。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);

    if (errCode != 2)
    {
        // [b28d-solo-nexon_cash] §4.1/P154：删发明符号
        // _ZN5nexon4cash12_GLOBAL__N_114traceRecvErrorEiPKc（ORIG nm 无此符号）。
        // ORIG 0x081ae105..0x081ae145 为就地具名 cMyTrace 两句式（ctor + op()）。
        // [P231 保活] ORIG 0x081ae101 `movzbl -0x69(%ebp),%eax; movzbl %al,%ebx`
        // 把 (int)errCode 在 ctor **之前**物化进 ebx（跨 ctor 保活，ctor 后
        // 直接 `mov %ebx,0x8(%esp)`），代价是 prologue `push %ebx`/epilogue
        // `mov -0x4(%ebp),%ebx` 各 1 条；4.4 对具名 errCode 在 ctor 后重载
        // （2 条 movzbl）⇒ 少 2 条。用 register 中间量复现 4.1 的提前物化。
        // [P221 全表达式临时链] ORIG ctor 的 this 落 -0x3c（临时槽区，浅于
        // in@-0x80）；4.4 把我方**具名** trace 分到固定槽 -0x80，挤掉 in
        // （in 上移 0x10、fmt/msg/CMyTrace 全套平移，130 行残差）⇒ 改链式
        // 表达式让 4.4 走临时槽区（语义同：dtor 在 full-expression 末，ORIG
        // 亦无 dtor 代码）。
        register int pid = static_cast<int>(errCode);
        cMyTrace(__FUNCTION__, 0x303, 0)(
            "NCashImpl::chargeCash Recv Error!! packet_id:%d, data:%s",
            pid, data);
        return static_cast<int>(errCode);
    }

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] CHARGE_CASH OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type), trOut.m_id);
    return 0;
}