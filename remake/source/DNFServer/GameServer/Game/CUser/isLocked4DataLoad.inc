bool CUser::isLocked4DataLoad()
{
    return m_field8d0f0;   // ORIG 0x864aa0a movzbl 直返（bool 字段兼容）
}