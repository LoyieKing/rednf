void OneADayItemShopEvent::buyItemLog(DeliveryMsg& msg)     // 0x081ba5e8
{
    // ORIG 汇编：shopId 实参取自 msg+0x10（m_logType），
    // CDataManager::m_itemShop(+0x8cbc)->IsOneADayShop(shopId) 为 0 则直接返回。
    EventStageLogDeliveryMsg& m = reinterpret_cast<EventStageLogDeliveryMsg&>(msg);  // ORIG：无 stage_view 调用
    if (G_CDataManager()->m_itemShop->IsOneADayShop(m.m_logType) != 1)
        return;

    SIG_EVENT_STAGE_LOG sig;    // 本函数 ORIG 槽基 -0x98 已 4 对齐，无需 aligned 属性
    // ORIG：经 m 视图（-0xc 槽）读三字段，写入顺序 [1][2][3][0][4]。
    sig.m_field0[1] = m.m_accId;
    sig.m_field0[2] = m.m_characNo;
    sig.m_field0[3] = m.m_serverGroup;
    sig.m_field0[0] = 0x6d;
    sig.m_field0[4] = 9;

    // ORIG：text 默认构造 + sep=string("/") 在 payloadPtr 检查前构造；赋值走
    // operator=（非 +=）；lexical_cast 实参直接传 payload+2 地址（const& 形参，
    // 无具名 value 局部，P0）；memcpy 无补 NUL（P171 照抄不写）。
    std::string text;
    std::string sep("/");
    if (m.m_payloadPtr != 0)
    {
        text = boost::lexical_cast<std::string>(
                   *reinterpret_cast<const unsigned long*>(m.m_payloadPtr + 2)) +
               sep;
        if (text.size() < 100)
        {
            memcpy(sig.m_data, text.c_str(), text.size());
        }
    }

    DB_InsertEventStageLog::makeRequest(sig);
}
