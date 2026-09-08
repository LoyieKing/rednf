// expert_extraction::CDollControllerExtraction —— ORIG W 声明头（实现 ExpertExtraction.cpp）。
// ORIG：C1/C2 W 0x084a4674、_check_error T 0x084a4286、check_error T 0x084a43aa、
//       extraction T 0x084a3dce、get_doll_controller_extraction_type T 0x084a3d66、
//       get_extraction_count T 0x084a41a8、D1 W 0x084a46de/D0 W 0x084a470e。
// 佐证：ctor 仅写 vptr → sizeof 同基类 CExtraction【推断】。
#ifndef DNF_GAME_CDOLLCONTROLLEREXTRACTION_H_
#define DNF_GAME_CDOLLCONTROLLEREXTRACTION_H_

#include "CExtraction.h"

class CUser;
class CEquipItem;
struct Inven_Item;

namespace expert_extraction
{

class CDollControllerExtraction : public CExtraction
{
public:
    // [b28d-solo-GlobalData] P299/P318：ORIG C1/C2 为 W（0x084a4674）⇒ 删用户空 ctor。
    virtual ~CDollControllerExtraction();

    virtual bool extraction(CUser* user, short param,
                            std::vector<std::pair<int, int> >& needItems);
    virtual int check_error(CUser* user, char a, short b, short c);
    virtual int get_extraction_count(Inven_Item item);
    virtual int _check_error(CUser* user, short a, short b);

    int get_doll_controller_extraction_type(CEquipItem const* equip);
};

}  // namespace expert_extraction

#endif  // DNF_GAME_CDOLLCONTROLLEREXTRACTION_H_
