// ORIG 0x0864a956 T _ZN5CUser31GetGuildIdBeforeDeleteCharacterEi：
// 返回 m_characList[slot].m_guildId（_Charac_info +0xdbe）
int CUser::GetGuildIdBeforeDeleteCharacter(int slot)

{
    return m_characList[static_cast<unsigned int>(slot)].m_guildId;
}
