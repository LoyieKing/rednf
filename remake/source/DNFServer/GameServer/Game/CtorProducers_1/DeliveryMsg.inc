// ORIG W 0x086978c0：四字段清零（server group 位为 0xffffffff）。
// 原 CUserDepsB.cpp（BuyItemDeliveryMsgC1 调用点 outgoing 帧 0x18 依赖此迁移）。
DeliveryMsg::DeliveryMsg()
{
    m_type = 0;
    m_accId = 0;
    m_characNo = 0;
    m_serverGroup = -1;
}