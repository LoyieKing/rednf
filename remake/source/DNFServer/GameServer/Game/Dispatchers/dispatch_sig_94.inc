int Dispatcher_Antibot_DPCallBack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    typedef int (*CheckFn)(void*, int, char*, unsigned short, char*, short*);  // vtable +0x2c
    // [round5 A01 · 轴 B 反向（Main 判据：以 ORIG 符号性为准）] ORIG 0x0820b188/0x0820b196
    //   均为 `movzbl 0x8d252(%eax),%eax` + `cmp $0x4/%0x5,%al; **ja**`（零扩展读 + **无符号**序）
    //   ⇒ 该字节按 unsigned char 访问；旧码 `char&` 有符号 ⇒ 我方产 setl/jl（统计 u/s=1/3
    //   vs ORIG 4/0）。语义等价（`<5` ⇔ `<=4`、无符号/有符号在 0..5 值域同解）。
    unsigned char& retryCnt =
        reinterpret_cast<unsigned char&>(user->m_field5);          // +0x8d252（movzbl 直证）
    if (retryCnt < 5)
    {
        retryCnt = static_cast<unsigned char>(retryCnt + 1);
    }
    unsigned int value = 0;                                                    // -0x14
    if (!packet.get_int(value))
    {
        if (retryCnt < 6)
        {
            user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)1,
                                            0x25d, retryCnt, 0);
        }
        return 0xa76a;
    }
    const int dummySeq = 0xd;                                                  // -0x10
    if (value > 0x567)
    {
        if (retryCnt < 6)
        {
            user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)2,
                                            0x25d, retryCnt,
                                            static_cast<int>(value));
        }
        return LineFunc(
            0xa780,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    short hashLen = 0xaf0;                                                     // -0x16
    const int index = packet.get_index();
    char* bufPtr = packet.get_buf_ptr(index);                                  // -0xc
    char outBuf[2800];                                                         // -0xb06（ORIG 栈缓冲）
    void* checker = GlobalData::s_antibotChecker_;
    CheckFn fn = *(CheckFn*)((*(void***)checker)[0x2c / 4]);
    const int checkRet =
        fn(checker, user->getAntibotKey(), bufPtr,
           static_cast<unsigned short>(value), outBuf, &hashLen);
    if (checkRet != 0)
    {
        return 0;
    }
    return 0;
