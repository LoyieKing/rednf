int Dispatcher_Antibot_DP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820b322：读嵌套包长度→checker vtbl+0x2c 解密→doDispatch 内层包。
    unsigned int binarySize = 0;
    if (!packet.get_int(binarySize))
    {
        return LineFunc(0xa795, __PRETTY_FUNCTION__, 0, 0);
    }
    char* nestedPacket = packet.get_buf_ptr(packet.get_index());
    int local_28 = 0xd;  // ORIG 局部（写后未用，保持帧形态）
    (void)local_28;
    if (binarySize > 0x567)
    {
        return LineFunc(0xa7a3, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 段直证：`sub $0xb5c,%esp` + `lea -0xb2e(%ebp)`（outBuf 基址）⇒ 解密输出缓冲
    // 容量 = 0xaf0（与 outLen 初值一致），非 4 字节视图（原 4B 声明令帧 0xb5c→0x64，
    // 且 Decrypt 出参写 0xaf0 上限即越界）。消费侧仍按 +0x00 class / +0x01 short 视图。
    unsigned char outBuf[0xaf0];
    unsigned short outLen = 0xaf0;
    typedef int (*DecryptFn)(void*, int, void*, unsigned short, unsigned char*, unsigned short*);
    void* checker = GlobalData::s_antibotChecker_;
    DecryptFn fn = *reinterpret_cast<DecryptFn*>(*reinterpret_cast<void***>(checker) + (0x2c / 4));
    int ret = fn(checker, user->getAntibotKey(), nestedPacket,
                 (unsigned short)(binarySize & 0xffff), outBuf, &outLen);
    if (ret != 0)
    {
        int key = user->getAntibotKey();
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa7af, 0);
        trace("[Dispatcher_Antibot_DP] Key : %d, nestedPacket : %x, binarySize : %d, return : %d",
              key, nestedPacket, binarySize, ret);
        return 0;
    }
    PacketDispatcher* dispatcher = G_PacketDispatcher();
    dispatcher->doDispatch(user,
                           (ENUM_PACKETCLASS)outBuf[0],
                           (unsigned short)*reinterpret_cast<unsigned short*>(outBuf + 1),
                           reinterpret_cast<char*>(outBuf),
                           static_cast<int>(outLen),
                           (ENUM_PACKETCLASS)1,
                           (unsigned short)0x25c);
    return 0;
