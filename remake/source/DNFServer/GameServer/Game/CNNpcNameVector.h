#ifndef DNF_GAME_CNNPCNAMEVECTOR_H_
#define DNF_GAME_CNNPCNAMEVECTOR_H_

#include <string>
#include <vector>

// ORIG: _ZN9CNNpcNameD1Ev 仅转发 _ZNSsD1Ev（std::string 析构），无 vtable/typeinfo
// 符号 ⇒ 非多态，唯一成员为 std::string。
class CNNpcName
{
public:
    std::string m_name;
};

// ORIG（df_game_r @0x89c3654/0x89c3668/0x89dc4ba，全部 W 弱符号 ⇒ 头内联）：
//   C1: vector<CNNpcName>::C1 + clear()；D1: vector<CNNpcName>::D1；
//   clear: vector<CNNpcName>::clear。无 _ZTV/_ZTI ⇒ 非多态（去掉 virtual）。
class CNNpcNameVector
{
public:
    CNNpcNameVector() : m_vec()
    {
        clear();
    }

    ~CNNpcNameVector()
    {
    }

    void clear()
    {
        m_vec.clear();
    }

private:
    std::vector<CNNpcName> m_vec;
};

#endif // DNF_GAME_CNNPCNAMEVECTOR_H_
