// expert_extraction::CExtraction —— ORIG T 声明头（虚基类；实现 ExpertExtraction.cpp）。
// ORIG 符号：C1 T 0x084a2938、_check_need_slot T 0x084a29f4、check_error T 0x084a2a5e、
//       inven_process T 0x084a2cec、D1 T 0x084a29a2/D0 T 0x084a29d6。
// 布局：vptr@+0x00 + CMTRand*@+0x04（ctor new(0x9c8)+seed(time)）→ sizeof 0x8。
// vtable @0x8c7ddc0（vptr 指向 vtable+8，槽序经 ctor vptr 赋值与 extraction 族
// 虚调用偏移实测）：+0x08 extraction / +0x0c check_error / +0x10 inven_process /
//   +0x14 _check_need_slot / +0x18 get_extraction_count / +0x1c _check_error
//   （基类三纯虚槽为 __cxa_pure_virtual）。
#ifndef DNF_GAME_CEXTRACTION_H_
#define DNF_GAME_CEXTRACTION_H_
#include <cstddef>
#include <utility>
#include <vector>

#include "Inven_Item.h"
#include "GameTypes.h"   // IntPairVector（P7 类型轴：extraction 第 3 参拼写；三个派生类头经本头传递可见）

class CUser;
class CMTRand;

namespace expert_extraction
{

class CExtraction
{
public:
    CExtraction();
    virtual ~CExtraction();

    // ---- 虚函数（声明序 = ORIG vtable 槽序） ----
    // [P7HeaderAxis 2026-09-11] 第 3 参拼写 IntPairVector（ORIG prett 直证 ×3：
    // `virtual bool expert_extraction::CAlchemistExtraction::extraction(CUser*, short int, IntPairVector&)`、
    // CEnchanter/CDollController 同形，size 0x64/0x64/0x69；mangled `…EP5CUsersRSt6vectorISt4pairIiiESaIS5_EE`
    // 展开为 St6vectorISt4pairIiiESaIS5_EE ⇒ 同底层类型、零 ABI）。定义侧见 ExpertExtraction TU（P6AuditOldN）。
    virtual bool extraction(CUser* user, short param,
                            IntPairVector& needItems) = 0;
    virtual int check_error(CUser* user, char a, short b, short c);
    virtual bool inven_process(CUser* user, short param,
                               std::vector<std::pair<int, int> >& needItems,
                               std::vector<int>& outSlots);
    virtual bool _check_need_slot(CUser* user);
    virtual int get_extraction_count(Inven_Item item) = 0;
    virtual int _check_error(CUser* user, short a, short b) = 0;

protected:
    CMTRand* m_rand;  // +0x04 指向 new(0x9c8) MT19937（seed(time(0))）【推断】
};

}  // namespace expert_extraction

#endif  // DNF_GAME_CEXTRACTION_H_
