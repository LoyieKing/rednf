// ORIG W 0x0869795a：基类构造后清零两扩展字段。
BuyItemDeliveryMsg::BuyItemDeliveryMsg() : DeliveryMsg()
{
    m_deliveryType = 0;
    m_item = 0;
}