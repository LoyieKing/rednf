// ORIG W 0x089858da：基类 → string+0x8 → mail+0xc → str.clear → mail.reset。
AccountFirstLoginScript::AccountFirstLoginScript()
{
    m_dbTableName.clear();
    m_mail.reset();
}