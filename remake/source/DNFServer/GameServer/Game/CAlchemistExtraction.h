// expert_extraction::CAlchemistExtraction —— ORIG W 声明头（实现 ExpertExtraction.cpp）。
// ORIG：C1/C2 W 0x084a463c、_check_error T 0x084a35f2、check_error T 0x084a3716、
//       extraction T 0x084a3182、get_alchemist_extraction_type T 0x084a355c、
//       get_extraction_count T 0x084a3596、D1 W 0x084a477a/D0 W 0x084a47aa。
// 佐证：ctor 仅写 vptr（基类 CExtraction 无成员追加）→ sizeof 同基类【推断】。
#ifndef DNF_GAME_CALCHEMISTEXTRACTION_H_
#define DNF_GAME_CALCHEMISTEXTRACTION_H_

#include "CExtraction.h"

class CUser;
class CEquipItem;
struct Inven_Item;

namespace expert_extraction
{

class CAlchemistExtraction : public CExtraction
{
public:
    // [b28d-solo-GlobalData] P299/P318：ORIG C1/C2 为 W（0x084a463c 隐式合成）
    // ⇒ 删用户空 ctor 声明，恢复 `new T()` 的 value-init 前置清零（CExtractionMgr::C1 实证）。
    virtual ~CAlchemistExtraction();

    virtual bool extraction(CUser* user, short param,
                            std::vector<std::pair<int, int> >& needItems);
    virtual int check_error(CUser* user, char a, short b, short c);
    virtual int get_extraction_count(Inven_Item item);
    virtual int _check_error(CUser* user, short a, short b);

    int get_alchemist_extraction_type(CEquipItem const* equip);
};

}  // namespace expert_extraction

#endif  // DNF_GAME_CALCHEMISTEXTRACTION_H_
