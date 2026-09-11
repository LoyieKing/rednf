// ORIG 0x089fb508 T：外层 i 0..≤4（stride 0x90）、内层 j 0..≤2（stride 0x30），
// 逐槽 m_slots[i][j].clear()。
void stFavorableRelationShip_t::stRewardItem_t::clear()
{
    for (int i = 0; i <= 4; ++i)
    {
        for (int j = 0; j <= 2; ++j)
        {
            m_slots[i][j].clear();
        }
    }
}