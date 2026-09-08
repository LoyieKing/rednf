// random_option::CRandomOptionItemHandleTest —— ORIG W 声明头（仅声明，无实现）。
// ORIG：C1 W 0x0822cb70（size 0x15，写 +0x00/+0x04）、_get_auction_id、
//       _regist_auction(Inven_Item&,long long,int,int,char const*)、
//       test_change/test_drop/test_regist_auction(CUser*)/test_reset/test_unseal。
#ifndef DNF_GAME_CRANDOMOPTIONITEMHANDLETEST_H_
#define DNF_GAME_CRANDOMOPTIONITEMHANDLETEST_H_
#include <cstddef>

class CUser;
struct Inven_Item;

namespace random_option
{

class CRandomOptionItemHandle;  // 完整定义见 random_option.h（若已有权威头）

class CRandomOptionItemHandleTest
{
public:
    CRandomOptionItemHandleTest();
    void test_change();
    void test_drop();
    void test_regist_auction(CUser* user);
    void test_reset();
    void test_unseal();
    // ORIG T 0x085f3cc4：返回 64 位（CONCAT44(uVar3,uVar2)，m_slot0/m_slot4 组合）。
    long long _get_auction_id();
    // ORIG T 0x085f3dba：返回 1（mov $0x1,%eax 实证）。
    int _regist_auction(Inven_Item& item, long long id, int a, int b,
                        char const* label);

private:
    unsigned int m_slot0;  // +0x00【推断】ORIG 0x085f3cc4 双字段 or（P254 同型）
    unsigned int m_slot4;  // +0x04【推断】
};

}  // namespace random_option

#endif  // DNF_GAME_CRANDOMOPTIONITEMHANDLETEST_H_