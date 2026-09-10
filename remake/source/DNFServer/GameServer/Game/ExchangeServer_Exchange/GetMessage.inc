Stream* CExchangeServer::GetMessage()
{
    return GlobalData::s_msgq_mgr->get(QIdx(5));
