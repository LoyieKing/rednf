void ItemVendingMachine::ProcessIPG_ResultQuery(CUser* user,
                                                WongWork::IPG::SIPGData* reqData,
                                                WongWork::IPG::SIPGData* resData,
                                                Stream* stream)
{
    if (user == 0)
        return;

    PacketGuard guard;
    int retCode = atoi(resData->m_cmd.m_query.m_retCode);
    // ORIG 0x08178530：`cmpl $0x0,-0x14; je <sum 支>` ⇒ 非零支为 fall-through
    // 主体（SendCashData(true)+0x4dd 日志），求和支在跳转目标处 ⇒ 源码写
    // `if (retCode != 0) {…} else {…}`（N57 主体位置范式）。
    if (retCode != 0)
    {
        user->SendCashData(true);
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultQuery,
                              0x4dd, "IPG_QUERY %d %d", user->get_acc_id(), retCode);
    }
    else
    {
        int total = atoi(resData->m_cmd.m_query.m_ceraValues[4]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[5]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[2]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[1]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[3]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[0]);
        user->SetCera((unsigned int)total);
        unsigned int point = (unsigned int)atoi(resData->m_cmd.m_query.m_ceraPoint);
        user->SetCeraPoint(point);
        // ORIG cUserHistoryLog@user+0x79700 ::CeraPoint(ENUM 0, total)
                user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)0,
                                     (unsigned int)total);
        user->SendCashData(false);
    }
