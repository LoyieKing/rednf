bool CBossStage::randomMapIndex(int& index)
{
    int acc = 0;
    int roll = 0;
    int stageCur;
    std::map<int, std::vector<RandomList> >::const_iterator it;
    std::vector<RandomList> vec;
    stageCur = m_stageCur;                            // +0xae0
    // P0：ORIG 无 receiver 指针槽往返 ⇒ getRandomTowerMapIndexes() 内联两次；
    // ORIG 调 const 版 find（_ZNKSt3map…4findERKi）⇒ receiver 按 const 视图内联。
    it = static_cast<const std::map<int, std::vector<RandomList> >*>(
             m_dungeon->getRandomTowerMapIndexes())->find(stageCur);
    if (it == static_cast<const std::map<int, std::vector<RandomList> >*>(
                  m_dungeon->getRandomTowerMapIndexes())->end())
        return false;
    vec = it->second;
    roll = get_rand_int(10000);
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        acc += vec[i].m_weight;
        if (roll < acc)
        {
            index = vec[i].m_mapIndex;
            break;
        }
    }
    return true;
