// ============================================================================
// Game/Arad_Verify.cpp —— OrigArad20 独占 TU（Arad/脚本域缺口）。
//
// 覆盖 ORIG 符号（df_game_r nm -S 实证）：
//   _ZN4ARAD21arad_equipment_verifyEP10Inven_ItemPK5CItemN9ARAD_TYPE14INVENTORY_TYPEE
//                                                                 T 0x0819eaee（0xa 字节）
//   _ZN4ARAD21arad_stackable_verifyEP10Inven_ItemPK5CItem        T 0x0819eae4（0xa 字节）
//   _ZN4ARAD19FN_DeleteGameScriptclERSt4pairIKSsPNS_24Arad_InterfaceGameScriptEE
//                                                                 W 0x08188c56
//
// 证据来源：
//   docs/class_func_reports/ARAD/{arad_equipment_verify,arad_stackable_verify}.md
//   （FN_DeleteGameScript 无独立报告，按 objdump 0x08188c56 逐指令转写。）
//
// 语义说明：
//   - arad_equipment_verify / arad_stackable_verify 为 ORIG 常量返回函数
//     （10 字节：push ebp; mov ebp,esp; mov $imm,%eax; pop ebp; ret），
//     参数全部未使用，返回值 0 / 0xb0 为 ORIG 硬编码【报告实证】。
//   - FN_DeleteGameScript::operator() 为 Arad_DataManager::destroyAradData
//     for_each 删除函子：脚本对象非空 → 先调 vtable 槽3 clearScriptData()，
//     再测非空 → delete（vtable 槽1 D0 deleting dtor），最后 pair.second 置 0。
//     ORIG 两次独立重读 p.second（P48 无 CSE：两处调用各自读成员）。
// ============================================================================
#include "Arad_DataManager.h"    // ARAD::Arad_InterfaceGameScript 完整类型（虚槽2/3）
#include "ARAD_FN_DeleteGameScript.h"
#include "CItem.h"
#include "Inven_Item.h"

#include <utility>

// ORIG mangled 内嵌枚举：N9ARAD_TYPE14INVENTORY_TYPEE = ARAD_TYPE::INVENTORY_TYPE。
// 值域 0..3 与 ORIG 全局 INVENTORY_TYPE（LinkClosureD.cpp 建模）一致【推断】；
// arad_equipment_verify 不读取该参数，值不影响函数行为。
namespace ARAD_TYPE
{
    enum INVENTORY_TYPE
    {
        INVENTORY_TYPE_0 = 0,
        INVENTORY_TYPE_1 = 1,
        INVENTORY_TYPE_2 = 2,
        INVENTORY_TYPE_3 = 3
    };
}

namespace ARAD
{

// ORIG T 0x0819eaee（10 字节）：mov $0x0,%eax; ret。
int arad_equipment_verify(Inven_Item*, CItem const*, ARAD_TYPE::INVENTORY_TYPE)
{
    return 0;
}

// ORIG T 0x0819eae4（10 字节）：mov $0xb0,%eax; ret。
int arad_stackable_verify(Inven_Item*, CItem const*)
{
    return 0xb0;
}

// ORIG W 0x08188c56：destroyAradData 的 for_each 删除函子。
// 汇编形态（objdump）：
//   mov 0xc(%ebp),%eax; mov 0x4(%eax),%eax; test; je +0x54     ← p.second 判空
//   取 vptr; add $0xc; call *(槽3)                              ← clearScriptData()
//   重读 p.second; test; je +0x4a                               ← 二次判空
//   取 vptr; add $0x4; call *(槽1)                              ← D0 deleting dtor
//   movl $0,0x4(pair)                                          ← p.second = 0
void FN_DeleteGameScript::operator()(
    std::pair<const std::string, Arad_InterfaceGameScript*>& p)
{
    if (p.second != 0)
    {
        p.second->clearScriptData();
        delete p.second;
        p.second = 0;
    }
}

}  // namespace ARAD
