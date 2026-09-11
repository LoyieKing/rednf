// ORIG 0x081bb73c：find→iterator 临时→const_iterator 拷贝→end→拷贝→
// operator== 取反汇聚（test/je/mov $0/jmp/mov $1）。
bool CRewardUserList::is_exist(unsigned int key)
{
    std::map<unsigned int, RewardUserData>::const_iterator it =
        m_map.find(key);
    return it == m_map.end() ? false : true;
}