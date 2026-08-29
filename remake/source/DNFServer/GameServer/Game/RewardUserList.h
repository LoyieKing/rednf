#ifndef GAME_REWARDUSERLIST_H_
#define GAME_REWARDUSERLIST_H_

// ============================================================================
// df_game_r 还原 —— CRewardUserList / RewardUserData（奖励用户列表基础容器）
// 布局依据 ORIG objdump（CRewardUserList.md）：
//
//   RewardUserData（sizeof 0x0c，insert 栈帧确认字段偏移）:
//     +0x00 int   a     （insert: mov param_2 -> +0）
//     +0x04 int   b     （insert: mov param_3 -> +4）
//     +0x08 bool  mark  （insert/ctor: movb 0 -> +8；delete_mark: movb 1 -> +8）
//   （bool 后对齐填充到 0x0c，故 pair<const uint, RewardUserData> 为 0x10）
//
//   CRewardUserList:
//     +0x00 std::map<unsigned int, RewardUserData>   （ctor 0x081bbfb6 构造）
//     +0x18 std::deque<unsigned int>                 （ctor 0x081bbfb6 构造）
//
//   函数（符号自 ORIG df_game_r）:
//     RewardUserData::RewardUserData()  0x081bbf82：a=b=0，mark=0
//     CRewardUserList::CRewardUserList() 0x081bbfb6：构造 map 于 +0x00、deque 于 +0x18
//     insert     0x081bb690：is_exist 不命中才插入；make_pair(key,d) 入 map；key 入 deque
//     is_exist   0x081bb73c：m_map.find(key) != m_map.end()
//     delete_mark 0x081bb7bc：find 命中则置 value.mark=1，返回 true；否则 false
//     ~CRewardUserList()  0x081bbffc（D1/D2）：成员反序析构 deque→map
//     clear      0x081bc042：先 m_map.clear() 再 m_index.clear()
//     get_index_size 0x081bc064：m_index.size()
//   友邻 TU（COnTimeEvent 等）直接访问公有成员 m_map/m_index（ORIG 无 getter 符号）。
// ============================================================================

#include <map>
#include <deque>

// 奖励用户数据（单条记录，sizeof 0x0c；见 CRewardUserList.md insert 栈帧字段偏移）
struct RewardUserData
{
    int   a;    // +0x00
    int   b;    // +0x04
    bool  mark; // +0x08

    RewardUserData();  // a=b=0，mark=0 （ORIG 0x081bbf82）
};

class CRewardUserList
{
public:
    CRewardUserList();  // 构造 map(+0x00) 与 deque(+0x18)（ORIG 0x081bbfb6）

    bool insert(unsigned int key, int a, int b);  // ORIG 0x081bb690
    bool is_exist(unsigned int key);              // ORIG 0x081bb73c
    bool delete_mark(unsigned int key);           // ORIG 0x081bb7bc
    ~CRewardUserList();  // 析构（ORIG D1/D2 0x081bbffc：成员反序析构 deque→map）

    void clear();                          // ORIG 0x081bc042：先 map.clear 再 deque.clear
    unsigned int get_index_size();         // ORIG 0x081bc064：deque.size()（ORIG 无 const，符号 _ZN...14get_index_sizeEv）

    // m_map/m_index 公有：ORIG 无 get_map/get_index 符号，COnTimeEvent 直接
    // lea this+0x2c/this+0x14 成员访问（process_send_reward_to_user @0x081bbd46 实证）。
    std::map<unsigned int, RewardUserData> m_map;  // +0x00
    std::deque<unsigned int>               m_index; // +0x18（key 的插入顺序索引）
};

#endif  // GAME_REWARDUSERLIST_H_