bool CmmChannelProxy::SendCheckChannelScript()
{
    ConnectBridgeServer();

    char name[0x14];
    std::memset(name, 0, sizeof(name));
    std::strncpy(name, "channel_info", 0x14);
    struct stat st;
    if (stat(name, &st) < 0)
    {
        if (errno != ENOENT)
        {
            return false;
        }
        DNFFLib::Make_Dir(const_cast<char*>(name));
    }

    char line[0x400];
    FILE* fp = std::fopen("channel_info/version", "wb+");
    // ORIG 0x081067e4：`cmpl $0,-0x20; sete %al; test %al,%al; je` ⇒ 原码是
    // **while** 形（P295/IsGateArea 实证；探针 /tmp/gdprobe/p3.cpp f2/f3：
    // `while(p==0){…return false;}` 4.4 产同形，`if` 形恒折 `jne`）。
    while (fp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x143, 9,
            true, true);
        trace("\t- [ERROR] : cannt open channel_script_version file\n");
        return false;
    }
    // ORIG（0x08106870 段）：行缓冲为函数作用域 static（bss -0x4d4），循环带
    // 行首空白剥离 + '\n'/'#' 注释行跳行（linePos 跨行累计【实证：不随行重置】）。
    // [b28d-solo-GlobalData] p167 EXTRA fgets×1：ORIG 仅 1 次 fgets（0x08106888
    // 尾测 `test;setne;test;jne <body>` 旋转形）⇒ 单条件 while，删 init 弃用读。
    while (std::fgets(s_version, 0x400, fp) != 0)
    {
        char* src = s_version;
        char* dst = src;
        do
        {
            while (*src == ' ' || *src == '\t')
            {
                ++src;
            }
            *dst++ = *src++;
        } while (*(src - 1) != '\0');
        if (s_version[s_linePos] == '\n')
        {
            s_linePos = 0;
            continue;
        }
        if (s_version[s_linePos] != '#')
            break;
        s_linePos = 0;
    }
    std::fclose(fp);

    tagCS_CHECK_SCRIPT_VERSION reqVer;
    std::strncpy(reqVer.m_version, s_version, 0x10);
    if (SendPacket(reinterpret_cast<char*>(&reqVer),
                   reqVer.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    // ORIG 08106922：m_buf 保存为局部头指针槽（mov this; add $0x20; mov -0x1c(%ebp)），
    // 循环条件经 tagPacketHeader::getSize（call 08106bcc）读 +0x02 dword size。
    tagPacketHeader* pVerResp = reinterpret_cast<tagPacketHeader*>(m_buf);
    while (m_field1c != pVerResp->getSize())
    {
        if (RecvPacket() == 0)
        {
            return false;
        }
    }

    fp = std::fopen("channel_info/version", "rb+");
    // ORIG 0x081069d7：同 P295 while 形（sete/test/je）。
    while (fp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x188, 9,
            true, true);
        trace("\t- [ERROR] : cannt open channel_script_version file\n");
        return false;
    }
    std::fputs(reinterpret_cast<char*>(pVerResp) + 0xf, fp); // ORIG lea 0xf(%eax) on 同一局部槽：头 0xb + first 4
    std::fclose(fp);

    m_field1c = 0;
    std::memset(m_buf, 0, 0x80000);

    tagCS_GET_SCRIPT reqGet;
    if (SendPacket(reinterpret_cast<char*>(&reqGet),
                   reqGet.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    // ORIG 08106a7c：第二段独立局部槽 -0xc(%ebp)，同 getSize 形态。
    tagPacketHeader* pScriptResp = reinterpret_cast<tagPacketHeader*>(m_buf);
    while (m_field1c != pScriptResp->getSize())
    {
        if (RecvPacket() == 0)
        {
            return false;
        }
    }
    fp = std::fopen("channel_info/channel_info.etc", "w+");
    // ORIG 0x08106ae8：`cmpl $0,-0x20; sete; test; je; mov $0,%eax; jmp` ⇒
    // while 形（探针 p3.cpp f3 逐条同形）。
    while (fp == 0)
    {
        return false;
    }
    int total = pScriptResp->getSize();
    std::fwrite(reinterpret_cast<char*>(pScriptResp) + 0xb, 1, total - 0xb, fp);
    std::fclose(fp);
    close(m_sock);
    return true;
