#ifndef DNF_GAME_CNNPCNAMEVECTOR_H_
#define DNF_GAME_CNNPCNAMEVECTOR_H_

#include <string>
#include <vector>

// ORIG: _ZN9CNNpcNameD1Ev 仅转发 _ZNSsD1Ev（std::string 析构），无 vtable/typeinfo
// 符号 ⇒ 非多态，唯一成员为 std::string。
class CNNpcName
{
public:
    std::string m_name;   // +0x0
    // [Q6HeaderTypeAxis 2026-09-11 布局轴·Main 批准 A 方案] 补第二成员占位（4B）：
    //   直证：ORIG `_ZNSt12_Destroy_auxILb0EE9__destroyIP9CNNpcNameEEvT_S4_` @0x089d51b7
    //   用 `addl $0x8,0x8(%ebp)`（元素步长 8）；我方原为 `addl $0x4`（步长 4）⇒
    //   `vector<CNNpcName>` 非空时析构/erase 推进量错 ⇒ 错地址调 `~CNNpcName`。
    //   32 位 `std::string` = 4B ⇒ 差额 4B 必来自第二成员。B 步反查（Q7S6 + 本泳道）：
    //   ORIG `~CNNpcName` @0x89d7d16 仅 1 条 `call _ZNSsD1Ev`（第二成员**不需析构**）且
    //   无 `_ZTV9CNNpcName`/`_ZTI`（**非多态**）⇒ 只能是 4B POD、**类型不可定** ⇒ 占位。
    //   全 ELF 无任何对该第二成员的访问点，且 ORIG 无 `vector<CNNpcName>` 的
    //   `push_back`/`_M_insert_aux` 实例 ⇒ 该 vector 构造后恒空 ⇒ 本差异为**潜伏**（半径 0）。
    char m_pad4[4];       // +0x4（ORIG 侧第二 4B 成员，用途未定）
};
// 布局锁（R2 头改动纪律）：ORIG sizeof == 8（由上述步长直证唯一确定）。
static_assert(sizeof(CNNpcName) == 8, "CNNpcName ABI: ORIG sizeof == 8");

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
