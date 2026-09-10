int CUser::GetGuildIdBeforeDeleteCharacter(int slot)
{
    return m_characList[static_cast<unsigned int>(slot)].m_guildId;
