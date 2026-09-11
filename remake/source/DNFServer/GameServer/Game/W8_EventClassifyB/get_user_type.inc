// ============================================================================
// CUsedFatigueGiveItem —— get_user_type（ORIG 0x08110eec；报告缺失，
// objdump 直接转写：movzbl 0xc(script)=UsedFatigueGiveItemScript::m_userType）
// ============================================================================
char CUsedFatigueGiveItem::get_user_type() const
{
    return static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_userType;
}