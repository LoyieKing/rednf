// ORIG W 0x086978ec：四参顺序写 +0x00..+0x0c。
void DeliveryMsg::setBaseData(int type, unsigned int accId, int characNo,
                              int serverGroup)
{
    m_type = type;
    m_accId = accId;
    m_characNo = characNo;
    m_serverGroup = serverGroup;
}