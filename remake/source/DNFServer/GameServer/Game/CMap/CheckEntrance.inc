bool CMap::CheckEntrance(int flag) const   // [b28d-Lane27] 头旋钮 int→bool 连带同步（同 ABI）
{
    std::pair<int,int> entrance;   // ORIG 循环外默认构造 pair（C1Ev），体内拷贝
    for (unsigned int i = 0; i < m_entranceVec.size(); ++i)
    {
        entrance = m_entranceVec[i];
        if ((entrance.second & flag) != 0)
            return 1;
    }
    return 0;
}