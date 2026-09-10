bool CRewardUserList::insert(unsigned int key, int a, int b)
{
    if (!is_exist(key)) {
        RewardUserData d;   // 默认构造：a=b=0，mark=0
        d.a = a;
        d.b = b;
        d.mark = false;     // ORIG 在 ctor 后再次显式清零 mark（offset 0x8）
        m_map.insert(std::make_pair(key, d));
        m_index.push_back(key);
        return true;
    }
    return false;
