#ifndef GAME_SERVICERESTRICTMANAGER_H_
#define GAME_SERVICERESTRICTMANAGER_H_

// ============================================================================
// df_game_r ServiceRestrictManager —— 唯一声明点
// 实现见 ServiceRestrictManager.cpp（符号：isRestricted(CUser*,Enum,int) 0x816e6b8、
// isRestricted(Enum,int) 0x816e75e；ARAD::Singleton<ServiceRestrictManager>::Get
// 0x81625e6）。
// 注意：GlobalData.cpp / WarField.cpp 各有本地子集声明，未在本头覆盖其私有
// 成员（initialize/savePolicy/loadPolicy 等见 GlobalData.cpp 本地类）。
// ============================================================================

class CUser;

// RestrictCategory：ORIG 枚举/字符串（enumStringRestrict 表 0x93700a0 仅 ORIG 数据）。
namespace RestrictCategory
{
enum Enum
{
    ENUM_0 = 0,
    Enum_3 = 3
};
}

class ServiceRestrictManager
{
public:
    bool isRestricted(CUser* user, RestrictCategory::Enum category, int v);
    bool isRestricted(RestrictCategory::Enum category, int v);
};

#endif  // GAME_SERVICERESTRICTMANAGER_H_
