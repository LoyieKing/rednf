int Dispatcher_SecurityCardRetransfer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return LineFunc(0xa6ac,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned int type = 0;              // -0x1c
    char phoneBuf[0xc] = {0};           // -0x28 起清零（-0x28/-0x24/-0x20 三个 dword）
    if (!packet.get_int(type))
    {
        return LineFunc(0xa6b1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(phoneBuf, 0xc, (int)type))
    {
        return LineFunc(0xa6b2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!GlobalData::s_securityCardCenter->checkPhoneNumber(phoneBuf))
    {
        return LineFunc(0xa6b5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char phoneHex[0x29];                // -0x9f
    memset(phoneHex, 0, 0x29);
    CSHA1 sha1;
    sha1.Update((unsigned char const*)phoneBuf, (unsigned int)type);
    sha1.Final();
    sha1.GetHexHash(phoneHex);

    if (user->getSecurityCard()->isActivate())
    {
        if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x7a);
            return 0;
        }
        // ORIG 0x0820ab66：card 未激活直接返回 0；pad 检查与密钥生成全在 card 块内
        //（0820ab68 `je 0820ad85` → 尾 return 0）。
        if (user->getPad()->isActivate())
        {
            if (user->getPad()->getFailCnt() > 2)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x88);
                return 0;
            }
            if (!user->getPad()->isCertified())
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x86);
                return 0;
            }
        }

        int certKeyLen = 0;                 // -0x2e（int + word 清零）
        short certKeyPad = 0;               // -0x2a
        // ORIG 0x0820ac59/0x0820acb1：两数组以聚合零初始化内联展开（非 memset call）。
        char serverKey[0x21] = {0};         // -0x4f
        char cardData[0x27] = {0};          // -0x76
        char* pCardData = GlobalData::s_securityCardCenter->generateCardData(cardData);
        char* pServerKey = GlobalData::s_securityCardCenter->generateServerKey(serverKey);
        char* pCertKey = GlobalData::s_securityCardCenter->generateCertKey((char*)&certKeyLen);
        int accId = user->get_acc_id();
        int uid = user->GetUID();
        DB_SecurityCardRetransfer::makeRequest(
            uid, phoneBuf, phoneHex, (unsigned int)accId,
            pCertKey, pServerKey, pCardData);
    }
    return 0;
