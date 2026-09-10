EventClassify::CEventEntity::CEventEntity()
    : m_isEventing(0), m_dbTableName(), m_startTime(0),
      m_endTime(0), m_script(0), m_recordType(0)
{
    // ORIG 0x810af8e：构造后调 m_dbTableName.clear()（空串冗余但存在，
    // 产生异常清理路径含 _ZNSsD1Ev——调用集合对齐）。
    m_dbTableName.clear();
