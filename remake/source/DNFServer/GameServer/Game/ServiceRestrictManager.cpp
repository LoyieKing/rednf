// ============================================================================
// df_game_r 还原 —— RestrictCategory + ServiceRestrictManager（从 GameStubs 迁移，G2）
// ORIG 符号（nm -C df_game_r）：
//   _ZN16RestrictCategory12enumToStringENS_4EnumE               RestrictCategory::enumToString(Enum)
//   _ZN22ServiceRestrictManager12isRestrictedEP5CUserN16RestrictCategory4EnumEi
//                                                              isRestricted(CUser*, Enum, int)  0x816e6b8
//   _ZN22ServiceRestrictManager12isRestrictedEN16RestrictCategory4EnumEi
//                                                              isRestricted(Enum, int)          0x816e75e
// 依赖：InterfaceServicePolicy / Taiwan::GarenaAuthData / CUser::getGarenaAuthData。
// 当前为占位实现（符号 T，不引入未实现的完整策略语义）；后续按
//   docs/class_func_reports/ServiceRestrictManager.md 精修。
// 类型声明统一于 ServiceRestrictManager.h（唯一声明点）。
// ============================================================================

#include "CUser.h"
#include "ServiceRestrictManager.h"

bool ServiceRestrictManager::isRestricted(CUser* user,
                                          RestrictCategory::Enum category,
                                          int v)
{
    // ORIG 0x816e6b8：无 Garena 移动认证时按 map<Enum,InterfaceServicePolicy*>
    // 查找并调用策略。占位实现（返回 false）。
    // TODO(G2): InterfaceServicePolicy / GarenaAuthData 语义。
    (void)user;
    (void)category;
    (void)v;
    return false;
}

bool ServiceRestrictManager::isRestricted(RestrictCategory::Enum category, int v)
{
    // ORIG 0x816e75e（WarField 用）。占位实现。
    // TODO(G2)
    (void)category;
    (void)v;
    return false;
}
