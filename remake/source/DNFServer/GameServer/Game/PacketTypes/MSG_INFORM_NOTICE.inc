MSG_INFORM_NOTICE::MSG_INFORM_NOTICE()
{
    m_noticeFlag = 0x80;  // ORIG C1：notice 类型哨兵（字段化改名，值语义不变）
