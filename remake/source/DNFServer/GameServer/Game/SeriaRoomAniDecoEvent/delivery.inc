void OneADayItemShopEvent::delivery(DeliveryMsg& msg)   // 0x081ba116
{
    // P17/ORIG：线性 cmp/je 链（单读双判、body 后置）= 两 case switch 形。
    switch (msg.m_type)
    {
        case 1:
            questClearLog(msg);
            break;
        case 2:
            buyItemLog(msg);
            break;
    }
