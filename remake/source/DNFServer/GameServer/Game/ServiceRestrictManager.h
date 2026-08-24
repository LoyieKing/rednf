#ifndef GAME_SERVICERESTRICTMANAGER_H_
#define GAME_SERVICERESTRICTMANAGER_H_

// ============================================================================
// df_game_r ServiceRestrictManager —— 唯一声明点
// ORIG 符号（nm -S df_game_r 核实）：
//   ctor 0x816e272 / dtor 0x816e29e / initialize 0x816e2b2 / savePolicy 0x816e8ce /
//   loadPolicy 0x816e7d6 / isRestricted(CUser*,Enum,int) 0x816e6b8 /
//   isRestricted(Enum,int) 0x816e75e / ARAD::Singleton<..>::Get 0x81625e6。
// 类布局（ctor 反编译）：map<RestrictCategory::Enum, InterfaceServicePolicy*> +
// +0x18/+0x19 两个标志字节。
// ============================================================================

#include <map>

class CUser;
class InterfaceServicePolicy;

namespace RestrictCategory
{
enum Enum
{
    ENUM_0 = 0,
    Enum_1 = 1,   // ORIG isRestricted(user,1,0xf/0x10)：随机选项重置/再铸限制类别
    Enum_3 = 3
};
}

class ServiceRestrictManager
{
public:
    ServiceRestrictManager();
    ~ServiceRestrictManager();
    bool initialize();    // channel_no==1 置 +0x19；注册 4 个策略
    void savePolicy();    // +0x19==1 时遍历序列化入 DB 队列
    void loadPolicy();    // SigLoadRestrictPolicy 请求入 DB 队列
    bool isRestricted(CUser* user, RestrictCategory::Enum category, int v);
    bool isRestricted(RestrictCategory::Enum category, int v);

    // ORIG 布局：map + 2 标志字节。
    std::map<int, InterfaceServicePolicy*> m_policyMap;
    char m_flag18;   // +0x18（ORIG ctor 置 0）
    char m_flag19;   // +0x19（channel_no==1 时置 1）
};

#endif  // GAME_SERVICERESTRICTMANAGER_H_
