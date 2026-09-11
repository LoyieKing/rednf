// ORIG T 0x085a2246（RestartDungeon.md）：复位战场后以原 idx/diff 重开局。
void CParty::RestartDungeon()
{
    int dungeonIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                         ->get_dungeon_index();
    // ORIG：mov %eax,-0xc（int 槽），dungeon_start 前 movsbl %al 扩符号。
    int diff = reinterpret_cast<CBattle_Field*>(&m_battleField)
                   ->get_dungeon_diff();
    reinterpret_cast<CBattle_Field*>(&m_battleField)->reset_field();
    dungeon_start(dungeonIdx, (char)diff, static_cast<ENUM_DUNGEON_TYPE>(0));
}