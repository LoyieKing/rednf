void CExchangeServer::SetServerType(EXCHANGE_SERVER_TYPE type)
{
    // 枚举字段=枚举形参同型直赋（无转换节点）：4.4 LHS 先载=ORIG 实证。
    m_serverType68 = type;
