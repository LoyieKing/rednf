// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864809c | _ZN5CUser11AntibotSendER9PacketBuf
// [b28d-GrpD 判词·结构化 N] ORIG 0x0864809c：残差为 ① checker/全局访问的
//   `mov 0x0,%R; mov (%R),%R`（未解析重定位 + 指针解引用形）② VLA 段槽号整体位移
//   （-0x30.. vs -0x34..）③ 未解析 call 标注。语义关键点逐条对齐：VLA 步进
//   `-(len+0x212)&~0xf`、虚槽 +0x34(verify)/+0x28(encrypt) 直调、encReq 单次短路物化、
//   encrypt 非 0=失败→trace(0x807)、`(!encReq&&isEncryptRequired())||encFailed` 链、
//   send_packet<0 → trace(0x834)+packetId!=199 DisConnSig ⇒ 无语义缺口（第三方 checker
//   槽建模已在下 §4.5 例外块终判）。
bool CUser::AntibotSend(PacketBuf& packet)   // b28c-L5 SIGFIX：ORIG rodata 串
                                              // "bool CUser::AntibotSend(PacketBuf&)"（P174）
{
    // ORIG T 0x0864809c 全文转写（报告 AntibotSend.md Ghidra C+asm 双段读完）：
    // ①verify（vtable+0x34）先行，成功后才进加密缓冲段；②加密缓冲=**VLA +
    // guard.clear + memset 500+len**（0x864822b/245：orig memset@plt=1、无
    // vector——alloca 步进 `-(len+0x212)&0xfffffff0`）；③encrypt 返回 **非0=失败**
    // → trace(0x807)+encFailed，**返回0=成功** → put_binary/Send 后 return
    // （0x86483b0 早退，非我旧版 ==0 续跑）；④encrypt 调用实参求值序 get_len→
    // get_packet→getAntibotKey（右→左）；⑤trace1 的 key/len 在 C1 **之前**求值；
    // ⑥isEncryptRequired 缓存（encReq 真则二段不再调，0x864842b else 路）。
    unsigned short packetId =
        *(unsigned short*)(packet.get_packet(0) + 1);
    bool encFailed = false;   // ORIG 0x086480c5 movb $0,-0x2e 预清（在 encReq 赋值前）
    // ORIG 0x086480cf..0x08648117：`isEncryptRequired() && verify(...)!=0` **单一
    //   短路物化**到 -0x2d（`mov $1/$0 + mov %al,-0x2d`），verify 仅在需求为真时调用。
    typedef int (*VerifyFn)(void*, int, unsigned short);   // vtable +0x34
    bool encReq = packet.isEncryptRequired() &&
        ((VerifyFn)(*(VerifyFn**)GlobalData::s_antibotChecker_)[0x34 / 4])(
            GlobalData::s_antibotChecker_, getAntibotKey(), packetId) != 0;
#if !remake_verify
    cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
        "[DNF-DIAG ENC] id=%u encReq=%d need=%d len=%d",
        (unsigned)packetId, (int)encReq, (int)packet.isEncryptRequired(),
        packet.get_len());
#endif
    // 0x165 TITLE: client parses inner 0x165 as plaintext after 561 unwrap.
    // G_Cipher XTEA of 0x165 crashes Squirrel. ORIG 561-wraps when verify!=0;
    // SDK verify returns 0 for 0x165 here, so use the same 561 envelope as 0x15.
    // Restored from 3badc73 (last known good enter-game).
    if (encReq || packetId == 0x165)
    {
        // §4.5 例外：s_antibotChecker_（GlobalData.h `void*`）承自外部
        // 【b22 终判·第三方】CreateAntiServerObj(ORIG 0x082a6539 W) 反汇编实证
        // = strlen/strncat/memcpy 拼路径 → dlopen → dlsym → call *%eax：checker
        // 对象来自运行时加载的外部 .so（反外挂 SDK），其 C++ 类/构造/vtable 均
        // 不在 df_game_r 编译单元内 ⇒ 本仓建模该类无从取证（未知槽的签名不可
        // 声明、§4.1 禁造），typedef 槽指针直调即 ORIG 行为的忠实表达——建模
        // 收益评估否决，此案终结。全族七槽用法同源：VerifyFn+0x34/EncryptFn+
        // 0x28/Fn3+0x08/Fn1+0x24/ReportFn+0x38/AddClient+0x04/P2PAddClient+
        // 0x20（本文件 :7746 与 PacketDispatcher/InterDispatcher 各点同判）。
        const int bufLen = packet.get_len() + 500;   // local_6c
        char buf[bufLen];                            // ORIG VLA（alloca 步进）
        int outLen = bufLen;                         // local_24
        PacketGuard guard;                           // local_68（块作用域，
                                                       // dtor 于早退前 0x86483b5）
        ((InterfacePacketBuf*)&guard)->clear();
        memset(buf, 0, bufLen);
        ((InterfacePacketBuf*)&guard)->put_header(0, 0x231);
        typedef int (*EncryptFn)(void*, int, char*, int, char*, int*);  // +0x28
        // 实参右→左求值：get_len→get_packet→getAntibotKey（P24/実参序）
        int r = ((EncryptFn)(*(EncryptFn**)GlobalData::s_antibotChecker_)[0x28 / 4])(
            GlobalData::s_antibotChecker_, getAntibotKey(),
            packet.get_packet(0), packet.get_len(), buf, &outLen);
        if (r != 0)
        {
            // ORIG 0x08648323..0x0864838a：全表达式临时链，get_len/getAntibotKey
            //   在 cMyTrace C1 **之前**求值并跨构造保活（P87/P227；具名局部会槽化）。
            cMyTrace(__PRETTY_FUNCTION__, 0x807, 0)(
                "[RecvBufferForEncrypt] result : %d, PacketID : %d, "
                "Key : %d, Length : %d", r, (int)packetId,
                getAntibotKey(), packet.get_len());
            encFailed = true;
        }
        else
        {
#if !remake_verify
            cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
                "[DNF-DIAG 561] id=%u in=%d outLen=%d",
                (unsigned)packetId, packet.get_len(), outLen);
#endif
            // ORIG 0x86482d3: put_binary(buf, outLen) 用 Encrypt 回写长度，
            // 不是 VLA bufLen(=get_len()+500)。多 500B 垃圾会让 561 解出来 INT3。
            ((InterfacePacketBuf*)&guard)->put_binary(buf, outLen);
            ((InterfacePacketBuf*)&guard)->finalize(false);
            return Send(guard);   // 0x8648313 起 return = Send 返回值透传
        }
    }
    PacketBuf sendBuf;
    char* data = packet.get_packet(0);
    int len = packet.get_len();
    // ORIG 0x08648379..0x08648441：`(!encReq && isEncryptRequired()) || encFailed`
    //   短路链（`movzbl encReq; xor $1; test; je 查 encFailed` + `mov $1/$0`）。
    // [R20a11] §26.3 多余物化：ORIG 0x08648441 `test %al,%al; je 0x86484a0` ——
    //   短路链物化岛（0x08648435/0x0864843c `mov $1/$0,%eax`）后**直测寄存器**，
    //   无具名 bool 槽。具名 `bool doEnc` 使 4.4 多发 `mov %al,-0x25(%ebp)` +
    //   `cmpb $0x0,-0x25(%ebp)`（+1 指令）且 VLA 帧 0x1739c→0x173a0（+4B）。
    //   修法 = P226 语句表达式内联条件（内不含具名变量 ⇒ 零槽短路物化）。
    // ORIG 0x0864841c..0x08648441: (!encReq && isEncryptRequired()) || encFailed
    // no packetId filter (AntibotSend.md)
    // DIAG_SKIP_GCI_CHARLIST: temporary bisect. Plain CHARLIST body matches ORIG
    // byte-for-byte but UI still empty after GCI (67->71). Skip GCI for cmd==2
    // only; if slots appear, Cipher Encrypt for key=2 is the breaker.
#define DIAG_SKIP_GCI_CHARLIST 0
    if (({ (!encReq && packet.isEncryptRequired()) || encFailed; }))
    {
#if !remake_verify && DIAG_SKIP_GCI_CHARLIST
        if (packetId == 2)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
                "[DNF-DIAG GCI] SKIP id=2 len=%d (plain zlib CHARLIST bisect)",
                packet.get_len());
        }
        else
#endif
        {
            sendBuf.copy(packet);
            sendBuf.encrypt_packet();
            data = sendBuf.get_packet(0);
            len = sendBuf.get_len();
            sendBuf.encFinalize();
#if !remake_verify
            cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
                "[DNF-DIAG GCI] id=%u afterEnc=%d", (unsigned)packetId, len);
#endif
        }
    }
    _NS_PI_2ND_SendHash_SC(data, (unsigned short)len, this);
    int sret = m_network.send_packet(data, len);
    if (sret < 0)
    {
        // ORIG 0x086484ec..0x08648539：getCurCharacName 在 C1 之前求值（%ebx 保活）
        cMyTrace(__PRETTY_FUNCTION__, 0x834, 0)(
            "User %s - send_packet() fail length(ID:%d,LEN:%d),",
            getCurCharacName(), (int)packetId, len);
        if (packetId != 199)
            DisConnSig((DISCONN_SIG)6, false, 0);
        return 0;
    }
    return 1;
}