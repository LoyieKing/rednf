// ============================================================================
// df_game_r 还原 —— CRewardUserList（奖励用户列表基础容器，见 CRewardUserList.md）
//   RewardUserData::RewardUserData()  0x081bbf82：a=b=0，mark=0
//   CRewardUserList::CRewardUserList() 0x081bbfb6：构造 map 于 +0x00、deque 于 +0x18
//   insert     0x081bb690：is_exist 不命中才插入
//   is_exist   0x081bb73c：find != end
//   delete_mark 0x081bb7bc：find 命中置 mark=1，返回 true
// ============================================================================

#include "RewardUserList.h"

RewardUserData::RewardUserData()
{
    a = 0;
    b = 0;
    mark = false;
}

CRewardUserList::CRewardUserList()
{
    // map(+0x00) 与 deque(+0x18) 由成员默认构造（ORIG ctor 0x081bbfb6 显式构造二者）
}

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
}

bool CRewardUserList::is_exist(unsigned int key)
{
    return m_map.find(key) != m_map.end();
}

bool CRewardUserList::delete_mark(unsigned int key)
{
    std::map<unsigned int, RewardUserData>::iterator it = m_map.find(key);
    if (it != m_map.end()) {
        it->second.mark = true;
        return true;
    }
    return false;
}