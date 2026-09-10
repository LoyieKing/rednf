int CDataManager::get_creature_level_exp(int level) const
{
    if (level < 2 || level > 99)
    {
        return 0;
    }
    // ORIG 消费 CreatureScript::ExpTable_E（0x94fefa0，initCreatureScript 填充）。
    return CreatureScript::ExpTable_[level - 2];
