void CUser::checkLogOutCorrectly()
{
    m_guildAgitDB.m_pad2[0] = 1;   // +0x8d0df（ORIG 单指令 movb 写；组件视图折叠）
