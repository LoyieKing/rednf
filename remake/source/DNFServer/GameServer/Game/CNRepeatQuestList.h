#ifndef DNF_GAME_CNREPEATQUESTLIST_H_
#define DNF_GAME_CNREPEATQUESTLIST_H_

#include <vector>

// ORIG（df_game_r @0x89c35fe/0x89c3612/0x89dc4ce，全部 W 弱符号 ⇒ 头内联）：
//   C1: vector<int>::C1 + clear()；D1: vector<int>::D1；
//   clear: vector<int>::clear。无 _ZTV/_ZTI ⇒ 非多态（去掉 virtual）。
class CNRepeatQuestList
{
public:
    CNRepeatQuestList() : m_vec()
    {
        clear();
    }

    ~CNRepeatQuestList()
    {
    }

    void clear()
    {
        m_vec.clear();
    }

private:
    std::vector<int> m_vec;
};

#endif // DNF_GAME_CNREPEATQUESTLIST_H_
