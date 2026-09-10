int Dispatcher_SecurityCardAuthRpy::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    SecuCardRpyParam& param = static_cast<SecuCardRpyParam&>(paramBase);

    if (check_error(user, msg) > 0)
        return -1;

    param.m_4 = 0;
    param.m_5 = 0;

    // [Lane1Dispatchers 2026-09-10] N78/P214③：ORIG 内联 rep stos（rep-stos 计 0/1），
    //   `memset` 在 4.4 恒退化为 call ⇒ 聚合初始化。
    unsigned char decrypted[0x400] = {0};
    int decryptedLen = 0x1000;

    // ORIG 0x0826283f..0x082629ee：嵌套 if（card 先判，else-if 才判 pad），
    // 两分支各自 getPad()->GetSelectedCipherIdx() 选 cipher（P183 逐点重取）。
    if (user->getSecurityCard()->isActivate() && m.m_secuType == 0)
    {
        // card 分支：ORIG 不检查 Decrypt 返回值。
        Cipher* cipher = GlobalData::s_secu_cipher_keystring_manager_
            [user->getPad()->GetSelectedCipherIdx() & 0xffff].cipher;
        cipher->Decrypt((int)m.m_cryptoType,
                        reinterpret_cast<const unsigned char*>(m.m_data),
                        m.m_dataLen, decrypted, &decryptedLen);
    }
    else if (user->getPad()->isActivate() && m.m_secuType == 1)
    {
        // pad 分支：解密失败即 trace + 错误出参后返回。
        Cipher* cipher = GlobalData::s_secu_cipher_keystring_manager_
            [user->getPad()->GetSelectedCipherIdx() & 0xffff].cipher;
        if (cipher->Decrypt((int)m.m_cryptoType,
                            reinterpret_cast<const unsigned char*>(m.m_data),
                            m.m_dataLen, decrypted, &decryptedLen) != 1)
        {
            param.m_5 = 1;
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x71, 0);
            trace("ERROR [TRACE AUTH RPY] : ENUM_CMDPACKET_SECURITY_CARD_AUTH_RPY,ENUM_ERROR_ETC");
            return 0;
        }
    }
    else
    {
        return LineFunc(0x76,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // 解密明文前 6 字节（安卡用前两字节作答案；pad 用全部 6 字节组 7 字节串）。
    const unsigned char answer0 = decrypted[0];
    const unsigned char answer1 = decrypted[1];
    const unsigned char answer2 = decrypted[2];
    const unsigned char answer3 = decrypted[3];
    const unsigned char answer4 = decrypted[4];
    const unsigned char answer5 = decrypted[5];

    // ORIG 0x08262a50：第二段处理链重复同一条件（card 再判 isActivate，每处重取）。
    if (user->getSecurityCard()->isActivate() && m.m_secuType == 0)
    {
        // ORIG：cardActive 分支先 setRequestState(0)（ORIG 0x0822ef64），
        // 再预检 failCnt>2 → param.m_5=0x7a 直接拒绝（ORIG 0x08262a44 段）。
        user->getSecurityCard()->setRequestState(WongWork::CSecurityCard::eRequestStateNone);
        if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)
        {
            param.m_5 = 0x7a;
            return 0;
        }
        // ORIG checkAnswer(answer0, answer1)：非 1 走失败链。
        if (user->getSecurityCard()->checkAnswer(answer0, answer1) != 1)
        {
            // ORIG SecuService(1, 0) → incFailCnt → 新值 >2 走 0x7a/0x17/hack，
            // 否则 0x79 + ChangeSecurity(1, failCnt==1 ? 0x15 : 0x16) → DB 上报后返回。
            user->m_historyLog.SecuService(
                (ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)0);
            if ((unsigned int)user->getSecurityCard()->incFailCnt() > 2)
            {
                param.m_5 = 0x7a;
                user->getHades()->ChangeSecurity(
                    (XNuclear::ENUM_HADES_SECURITY)1, 0x17);
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x343, 1, 0, 0);
            }
            else
            {
                param.m_5 = 0x79;
                if (user->getSecurityCard()->getFailCnt() == 1)
                {
                    user->getHades()->ChangeSecurity(
                        (XNuclear::ENUM_HADES_SECURITY)1, 0x15);
                }
                else
                {
                    user->getHades()->ChangeSecurity(
                        (XNuclear::ENUM_HADES_SECURITY)1, 0x16);
                }
            }
            DB_SecurityCardUpdateFailCnt::makeRequest(
                user->get_acc_id(),
                (unsigned int)user->getSecurityCard()->getFailCnt());
            return 0;
        }

        // 认证成功：SecuService(1, 1) → ChangeSecurity(1, 0x19) →
        // 发包（0xa7, byte1, certCnt>0x3b?1:0）→ DB 清零 fail/cancel → setCertified/resetFailCnt。
        user->m_historyLog.SecuService(
            (ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)1);
        user->getHades()->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1, 0x19);

        PacketGuard packet;
        packet.put_header(1, 0xa7);
        packet.put_byte(1);
        int certCnt = user->getSecurityCard()->getCertCnt();
        packet.put_byte(certCnt > 0x3b ? 1 : 0);
        packet.finalize(true);
        user->Send(packet);

        DB_SecurityCardUpdateFailCnt::makeRequest(user->get_acc_id(), 0);
        if (user->getSecurityCard()->getCancelCnt() != 0)
        {
            DB_SecurityCardUpdateCancelCnt::makeRequest(user->get_acc_id(), 0);
        }
        DB_SecurityCardAddCertCnt::makeRequest(user->get_acc_id());
        user->getSecurityCard()->setCertified(true);
        user->getSecurityCard()->resetFailCnt();
    }
    else if (user->getPad()->isActivate() && m.m_secuType == 1)
    {
        user->getPad()->setRequestState(Sanicova::CPad::eRequestStateNone);
        if (user->getPad()->getFailCnt() > 2)
        {
            param.m_5 = 0x88;
            return 0;
        }

        char passAnswer[8] = { 0, };
        passAnswer[0] = (char)answer0;
        passAnswer[1] = (char)answer1;
        passAnswer[2] = (char)answer2;
        passAnswer[3] = (char)answer3;
        passAnswer[4] = (char)answer4;
        passAnswer[5] = (char)answer5;
        passAnswer[6] = '\0';

        // ORIG doPassPadReplay(user, passAnswer, answerType, ok, errCode)
        // （Dispatcher_MouseRegister，0x082633c6）。
        Dispatcher_MouseRegister::doPassPadReplay(
            user, passAnswer, (char)m.m_answerType,
            *reinterpret_cast<bool*>(&param.m_4), param.m_5);
        if (param.m_4 == 0)
            return 0;
        param.m_5 = 1;
    }
    else
    {
        // ORIG 0x08262edb：两条件皆不满足（None Secu Service）。
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x10b, 0);
        trace("ERROR [TRACE AUTH RPY] : None Secu Service");
        return LineFunc(0x10c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    param.m_4 = 1;
    return 0;
