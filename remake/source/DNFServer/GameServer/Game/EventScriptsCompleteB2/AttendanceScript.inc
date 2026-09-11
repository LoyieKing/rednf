// ORIG W 0x0898597a：全成员构造 → str(+8).clear → vec(+0x10).clear →
// mail.reset → int+0xc=0 → int+0x24=0。
AttendanceScript::AttendanceScript()
{
    m_dbTableName.clear();
    m_attends.clear();
    m_mail.reset();
    m_field0c = 0;
    m_field24 = 0;
}