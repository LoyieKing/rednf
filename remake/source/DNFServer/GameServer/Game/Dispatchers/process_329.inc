int Dispatcher_RequestIngameAdvertisement::process(CUser* user, MSG_BASE& msg,
                                                   ParamBase& param)
{
    stRequestIngameAdvertisement* R =
        reinterpret_cast<stRequestIngameAdvertisement*>(&param);
    MSG_REQUEST_INGAME_ADVERTISEMENT& M =
        reinterpret_cast<MSG_REQUEST_INGAME_ADVERTISEMENT&>(msg);

    R->m_errorCode = check_error(user, msg);

    if (R->m_errorCode >= 1)
    {
        return 0;
    }
    if (R->m_errorCode < 0)
    {
        LineFunc(
            0x5c20,
            __PRETTY_FUNCTION__,
            R->m_errorCode, 0);
        return 0;
    }

    // err == 0 主流程
    R->m_adType = M.m_field0d;

    if (InGameADManagerInstance()->CheckIngameAdvertisementCondition(
            user, *reinterpret_cast<stNotifyIngameADInfo*>(&R->m_code),
            (int)M.m_field0d))
    {
        if (R->m_adShape == 4 && user->get_multiboxLotteryItemFailCnt() > 2)
        {
            if (R->m_prizeShape != 0 && R->m_itemID != 0)
            {
                sendRewardItemSystemMail(user, R->m_itemID, 1);
                if (R->m_logFlag != 0)
                {
                    InGameADManagerInstance()->WriteLog(
                        user, '\x03', M.m_field0d,
                        (char)R->m_adShape, R->m_code);
                }
            }
            else if (R->m_prizeShape == 0 && R->m_itemID2 != 0)
            {
                sendRewardItemSystemMail(user, R->m_itemID2, 1);
                if (R->m_logFlag != 0)
                {
                    InGameADManagerInstance()->WriteLog(
                        user, '\x03', M.m_field0d,
                        (char)R->m_adShape, R->m_code);
                }
            }

            user->set_multiboxLotteryItemFailCnt(0);        // 0x080fdf44
        }

        if (R->m_logFlag != 0)
        {
            InGameADManagerInstance()->WriteLog(
                user, '\0', M.m_field0d,
                (char)R->m_adShape, R->m_code);
        }
    }
    else
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(R) + 0x640) = 4;             // 条件不满足固定错误码
    }

    return 0;
