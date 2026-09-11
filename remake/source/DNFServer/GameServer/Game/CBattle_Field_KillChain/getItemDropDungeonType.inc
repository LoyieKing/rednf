// ============================================================================
// getItemDropDungeonType —— ORIG T 0x0830aa46。
// 无副本=3；维度可入=2；脚本 boss 图(dungeon+0x89c)=1；普通=0。
// ============================================================================
int CBattle_Field::getItemDropDungeonType()
{
    if (m_pDungeon == 0)
        return 3;
    if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
        return 2;
    if (m_pDungeon->m_89c != 0)   // +0x89c 脚本 boss 图标记
        return 1;
    return 0;
}