void Action_SendMail::reset()
{
    m_jobMap.clear();
    m_field18 = 0;
    m_field1c = 0;
    m_addInfo = 0;
    m_itemIdx = 0;
    m_gold = 0;
    m_coin = 0;          // ORIG 仅清 +0x2c 低字节（movb），字段即按字节建模
    m_field30.clear();
    m_field34.clear();
