bool CUser::isLocked4DataLoad()
{
    return m_locked4DataLoad;   // ORIG 0x864aa0a movzbl 直返（bool 字段兼容）
}