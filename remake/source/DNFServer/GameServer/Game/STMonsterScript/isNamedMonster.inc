// ===========================================================================
// isNamedMonster —— ORIG W 0x088ba18c（全量转写）
// ===========================================================================
bool isNamedMonster(int monsterIndex)
{
    if (monsterIndex > 0xc34f && monsterIndex <= 0xc738)
        return true;
    if (monsterIndex > 0xdabf && monsterIndex <= 0xea60)
        return true;
    return false;
}