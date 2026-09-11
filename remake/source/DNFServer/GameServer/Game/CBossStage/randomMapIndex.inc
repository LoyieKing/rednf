// ---- randomMapIndex（ORIG 0x0814f4fc）----
// acc/roll 先置零；栈上 vector 拷贝后遍历；getRandomTowerMapIndexes 在
// find 与 end 处各调用一次（ORIG 两次外呼）。
// [b28d-62] N82：p167 CLEAR，109/112。ORIG 0x0814f4fc
// [b28d-solo-CBossStage] AE：P0 去 receiver 指针局部 + const 视图内联调 const
//   find（消转换构造 call）；stageCur 声明/赋值分离复现槽序。ndiff 58→0。
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
}