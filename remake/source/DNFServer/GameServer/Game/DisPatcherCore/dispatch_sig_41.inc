int Dispatcher_ChangeCharacName::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 2)
        return LineFunc(0x9eb3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    unsigned char slot = 0;                              // ORIG -0x65
    int newLen = 0;                                      // ORIG -0x6c
    int srcLen = 0;                                      // ORIG -0x70
    char srcName[0x1e] = { '\0' };                       // ORIG -0x8e（P4 零初始化展开）
    char newName[0x1e] = { '\0' };                       // ORIG -0xac
    if (!packet.get_byte(slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    bool srcOk = packet.get_int(srcLen);                 // ORIG local_25
    if (!srcOk)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x9ec7, 0);
        trace("ting : m_id(%s) src char name length(%d), ret(%d)",
              NumberToString(user->get_acc_id(), 0), srcLen, (int)srcOk);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (!packet.get_str(srcName, 0x1e, srcLen))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    bool newOk = packet.get_int(newLen);                 // ORIG local_25（第二次）
    if (!newOk)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x9ed4, 0);
        trace("ting : m_id(%s) char name length(%d), ret(%d)",
              NumberToString(user->get_acc_id(), 0), newLen, (int)newOk);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (!packet.get_str(newName, 0x1e, newLen))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (newLen < 4 || 0x12 < newLen)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 1);
        return 0;
    }

    const char* serverName = user->get_charac_name((int)slot);
    if (serverName == 0 || strncmp(srcName, serverName, 0x1d) != 0)
    {
        if (serverName == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x9ef6, 0);
            trace("ting : m_id(%s) srcCharacName2 is null",
                  NumberToString(user->get_acc_id(), 0));
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x9ef8, 0);
            trace("ting : src char name(%s), src server char name(%s)",
                  srcName, serverName);
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }

    unsigned char typeFlag = 0;                          // ORIG -0xad
    if (!packet.get_byte(typeFlag))
        return LineFunc(0x9eff,
                        __PRETTY_FUNCTION__,
                        0, 0);

    int nameState;                                       // ORIG -0x1c
    if (typeFlag == 2)
    {
        // ORIG hasPreventPrefixCharacName(src, srcLen)（Ghidra 误抄参数，asm 实证）。
        if (!G_CDataManager()->hasPreventPrefixCharacName(srcName, srcLen))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
            return 0;
        }
        nameState = 2;
    }
    else
    {
        if (G_CDataManager()->hasSpecialCharacter(serverName))
            nameState = 0;
        else if (G_CDataManager()->hasPreventString(serverName) ||
                 G_CDataManager()->hasPreventStringName(serverName))
            nameState = 1;
        else
        {
            if (!user->CheckPossibleRequestRenameState((int)slot))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
                return 0;
            }
            if (!checkValidCharacName(newName))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
                return 0;
            }
            user->SetCharacViewState((int)slot, 1);
            nameState = 3;
        }
    }

    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(newName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
        return 0;
    }
    if (!checkValidCharacName(newName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
        return 0;
    }
    DB_ReqChangeCharacName::makeRequest((int)user->GetUID(),
                                        user->get_charac_no((int)slot),
                                        newName, nameState);
    return 0;
