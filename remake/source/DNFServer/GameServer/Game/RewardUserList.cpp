// ============================================================================
// df_game_r 还原 —— CRewardUserList（奖励用户列表基础容器，见 CRewardUserList.md）
//   RewardUserData::RewardUserData()  0x081bbf82：a=b=0，mark=0
//   CRewardUserList::CRewardUserList() 0x081bbfb6：构造 map 于 +0x00、deque 于 +0x18
//   insert     0x081bb690：is_exist 不命中才插入
//   is_exist   0x081bb73c：find != end
//   delete_mark 0x081bb7bc：find 命中置 mark=1，返回 true
// ============================================================================

#include "RewardUserList.h"

CRewardUserList::CRewardUserList()
{
    // map(+0x00) 与 deque(+0x18) 由成员默认构造（ORIG ctor 0x081bbfb6 显式构造二者）
}

RewardUserData::RewardUserData()
{
    a = 0;
    b = 0;
    mark = false;
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

// ORIG 0x081bb73c：find→iterator 临时→const_iterator 拷贝→end→拷贝→
// operator== 取反汇聚（test/je/mov $0/jmp/mov $1）。
bool CRewardUserList::is_exist(unsigned int key)
{
    std::map<unsigned int, RewardUserData>::const_iterator it =
        m_map.find(key);
    return it == m_map.end() ? false : true;
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

CRewardUserList::~CRewardUserList()
{
    // 成员按声明反序自动析构：deque(+0x18) → map(+0x00)（ORIG D1/D2 0x081bbffc 同序）
}

void CRewardUserList::clear()
{
    m_map.clear();     // ORIG 先 map（0x081bc04e）
    m_index.clear();   // 后 deque（0x081bc05c）
}

unsigned int CRewardUserList::get_index_size()
{
    return m_index.size();   // ORIG 0x081bc064
}