int importImageCommunicationInfo(const char* path, ImageCommunicationData* out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3233,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xe8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    // [b28d-solo-RDARScriptEtcParameter] ORIG 0x08913dbc：循环体内每轮重置
    // isType(-0xd)/ok(-0xe)+构造 token(-0x14)、尾析构；ScanTypeOrInt 返回值落
    // -0xc 死槽（照抄 P171）。ORIG 真形 = 三态共享尾（state 0=返回 result /
    // 1=退出循环返回 1 / 2=继续；result/state 全程 esi/ebx 无槽往返 ⇒ 原码用
    // break+循环外 return 1，而非循环内 return 1，P283/P63 族）。
    for (;;)
    {
        bool isType = false;
        bool ok = false;
        std::string token;
        int ret = ScanTypeOrInt(isType, token, ok, true);
        (void)ret;
        if (!ok)
            return 0;
        if (isType && token == "[charge]")
        {
            out->m_charge = ScanInt(&ok);
            if (!ok)
                return 0;
            break;
        }
        if (isType && token == "[/image communication]")
        {
            break;
        }
    }
    return 1;
