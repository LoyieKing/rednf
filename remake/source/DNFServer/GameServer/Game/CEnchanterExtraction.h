// expert_extraction::CEnchanterExtraction —— ORIG W 声明头（实现 ExpertExtraction.cpp）。
// ORIG：C1/C2 W 0x084a4658、_check_error T 0x084a3b92、check_error T 0x084a3cb6、
//       extraction T 0x084a37c6、get_extraction_count T 0x084a3b34、
//       D1 W 0x084a472c/D0 W 0x084a475c。
// 佐证：ctor 仅写 vptr → sizeof 同基类 CExtraction【推断】。
#ifndef DNF_GAME_CENCHANTEREXTRACTION_H_
#define DNF_GAME_CENCHANTEREXTRACTION_H_

#include "CExtraction.h"

class CUser;
struct Inven_Item;

namespace expert_extraction
{

class CEnchanterExtraction : public CExtraction
{
public:
    // [b28d-solo-GlobalData] P299/P318：ORIG C1/C2 为 W（0x084a4658）⇒ 删用户空 ctor。
    virtual ~CEnchanterExtraction();

    virtual bool extraction(CUser* user, short param,
                            IntPairVector& needItems);
    virtual int check_error(CUser* user, char a, short b, short c);
    virtual int get_extraction_count(Inven_Item item);
    virtual int _check_error(CUser* user, short a, short b);
};

}  // namespace expert_extraction

#endif  // DNF_GAME_CENCHANTEREXTRACTION_H_
