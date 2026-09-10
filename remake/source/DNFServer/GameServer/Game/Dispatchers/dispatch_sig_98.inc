int Dispatcher_CharacterStatistic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82051d0：get_state < 3 → 0；依次 get_int 6 个字段（任一失败 →
    // LineFunc(0x9d1f+k, __PRETTY_FUNCTION__, 0, 0) 并返回其返回值），
    // 成功后写 CUser+0x8e03c..0x8e050（6 个 int，Character 统计区），并置
    // CUser+0x8e074 = 1。零外部调用（自包含）。
    if (user->get_state() < 3)
    {
        return 0;
    }

    int v0, v1, v2, v3, v4, v5;
    if (packet.get_int(v0) != 1) return LineFunc(0x9d1f, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v1) != 1) return LineFunc(0x9d20, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v2) != 1) return LineFunc(0x9d21, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v3) != 1) return LineFunc(0x9d22, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v4) != 1) return LineFunc(0x9d23, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v5) != 1) return LineFunc(0x9d24, __PRETTY_FUNCTION__, 0, 0);

    char* base = reinterpret_cast<char*>(user);
    *(int*)(base + 0x8e03c) = v0;  // ORIG *(int*)(param_1+0x8e03c) = local_24
    *(int*)(base + 0x8e044) = v1;  // local_1c
    *(int*)(base + 0x8e048) = v2;  // local_18
    *(int*)(base + 0x8e040) = v3;  // local_20
    *(int*)(base + 0x8e04c) = v4;  // local_14
    *(int*)(base + 0x8e050) = v5;  // local_10[0]
    *(base + 0x8e074) = 1;
    return 0;
