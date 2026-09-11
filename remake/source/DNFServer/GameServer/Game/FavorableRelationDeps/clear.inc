// ORIG 0x089fb4c0 T：j 0..≤2 —— vector.clear(j*0xc) 后 weight[j]=0（写位于槽尾
// 0x24+4j，即本结构的权重计数字段）。
void stFavorableRelationShip_t::stRewardItemInfo_t::clear()
{
    for (int j = 0; j <= 2; ++j)
    {
        m_byDir[j].clear();
        m_weight[j] = 0;
    }
}