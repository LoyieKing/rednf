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
// ============================================================================

#include "CUser.h"

// RestrictCategory：ORIG 枚举/字符串（enumStringRestrict 表 0x93700a0 仅 ORIG 数据）。
class RestrictCategory
{
public:
    enum Enum
    {
        ENUM_0 = 0,
        Enum_3 = 3
    };
};

// ------------------------------------------------ ServiceRestrictManager ----
class ServiceRestrictManager
{
public:
    bool isRestricted(CUser* user, RestrictCategory::Enum category, int v);
    bool isRestricted(RestrictCategory::Enum category, int v);
};

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