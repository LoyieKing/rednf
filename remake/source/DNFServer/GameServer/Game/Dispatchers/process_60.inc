int Dispatcher_ChangeEmotion::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);                     // ORIG 0x081d1cec（二参版）
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    // 声明顺序决定落槽：ORIG 0x081d1cex 先物化 msg→-0x14、后物化 param→-0x10
    // （技巧21 先声明占远槽），我方原顺序相反导致两槽互换 + 后续 `mov -0x10/%-0x14` 全错。
    MSG_CHANGE_EMOTION& req = reinterpret_cast<MSG_CHANGE_EMOTION&>(msg);
    stChangeEmotionParam& out = static_cast<stChangeEmotionParam&>(param);
    out.m_4 = req.m_emotionIdx;                                 // msg+0x0d short → param+0x04

    try
    {
        if (user->get_state() <= 2)                             // cmp $0x2,setle
        {
            throw static_cast<ENUM_ERROR>(3);
        }
        // 同上配方：ORIG 0x081d1ebe 为 `xor $1,%eax; test %al,%al` 单段。
        unsigned char emBad =
            (unsigned char)isVaildEmotionValue((short)out.m_4);
        emBad ^= 1;
        if (emBad)
        {
            throw static_cast<ENUM_ERROR>(1);
        }
        user->SetCurCharacEmotion((short)out.m_4);              // CUserCharacInfo::SetCurCharacEmotion
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)3, 0, (ENUM_PACKETCLASS)0);  // ORIG 0x08122390
        return 0;
    }
    catch (ENUM_ERROR e)                                        // catch handler @0x081d1c56
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x102, (unsigned char)e);
        return -1;
    }
