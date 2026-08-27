#ifndef GAME_HEROMISSIONCONDITION_H_
#define GAME_HEROMISSIONCONDITION_H_

// ============================================================================
// df_game_r 还原 —— HeroMissionCondition / BaseHeroMissionCondition 权威头。
// 证据：docs/class_func_reports/HeroMissionCondition/{clear,operator=}.md；
//       docs/class_func_reports/BaseHeroMissionCondition/（全部报告）；
//       缺报告生命周期按 objdump 转写并标地址（见 W9HeroCondition.cpp 头注）。
// 布局结论：
//   HeroMissionCondition（值类型，内嵌于 BaseHeroMissionCondition +0x08，
//   sizeof = 0x2c）：
//     +0x00 int        m_missionType      （Base::getMissionType 返回）
//     +0x04 int        m_field04          （clear/operator= 按 dword 拷贝；未证实语义）
//     +0x08 char       m_isRepeatMission  （Base::isRepeatMission movzbl +0x10）
//     +0x09 char       m_isResetState     （Base::isResetState movzbl +0x11）
//     +0x0c string     m_mailTitle        （Base::getMailTitle 拷贝源 this+0x14）
//     +0x10 string     m_mailContent      （getMailContent 拷贝源 this+0x18）
//     +0x14 vector<RewardData> m_rewards  （rewardProcess 遍历）
//     +0x20 vector<unsigned short> m_extra（operator= 尾部 back_insert copy）
//   BaseHeroMissionCondition：vptr +0x00（vtable 0x08b7f0d8 核实：
//     D1 0x08164f3a / D0 0x08164f6e / __cxa_pure_virtual ×2）、
//     +0x04 unsigned int m_missionCode（ctor 参数；getMissionCode）、
//     +0x08 m_condition。
// 下游注意：HeroMissionEvent.h 现 include 本头；勿再局部重复定义该域类型。
// ============================================================================

#include <string>
#include <vector>

class CUser;
class HeroMissionValue;

class HeroMissionCondition
{
public:
    // ORIG mangled N20HeroMissionCondition9MissionNo1TE：
    // MissionNo 为类内嵌套类型，成员 enum T 即 Base ctor/rewardMission 的按值参数。
    // 已证实取值：T_4=4（CUserDepsB 注释 MissionNo=4）、T_6（HeroMissionLink4）；
    // 其余编号调用点以整型 cast 使用，一并给出连续枚举避免隐式截断风险。
    struct MissionNo
    {
        enum T
        {
            T_0 = 0,
            T_1 = 1,
            T_2 = 2,
            T_3 = 3,
            T_4 = 4,
            T_5 = 5,
            T_6 = 6,
        };
    };

    // ---- RewardData：奖励物品描述 ----
    // rewardProcess 用法：m_itemIdx → G_CDataManager()->find_item()；
    // m_addInfo → Inven_Item::set_add_info（ushort，reward+0x04）。
    // ORIG D1/D2 0x08966634 为空体用户析构（out-of-line 保持 non-trivial 链路）。
    struct RewardData
    {
        ~RewardData();              // ORIG W 0x08966634（空）
        int m_itemIdx;              // +0x00（find_item 目标）
        unsigned short m_addInfo;   // +0x04（set_add_info）
    };

    HeroMissionCondition();                                       // W 0x081681c8
    HeroMissionCondition(const HeroMissionCondition& other);      // W 0x08964f5c
    ~HeroMissionCondition();                                      // W 0x08168260
    HeroMissionCondition& operator=(const HeroMissionCondition&); // T 0x08961234
    void clear();                                                 // T 0x08961376

    int m_missionType;         // +0x00
    int m_field04;             // +0x04（ORIG ctor 不初始化；仅按 clear/operator= 操作）
    char m_isRepeatMission;    // +0x08
    char m_isResetState;       // +0x09
    std::string m_mailTitle;                    // +0x0c
    std::string m_mailContent;                  // +0x10
    std::vector<RewardData> m_rewards;          // +0x14
    std::vector<unsigned short> m_extra;        // +0x20
};

static_assert(sizeof(std::string) == 4,
              "GCC C++98 COW basic_string expected (4 bytes, i386)");
static_assert(sizeof(HeroMissionCondition) == 0x2c,
              "HeroMissionCondition layout: 0xa raw + strings@0xc/0x10 + vecs@0x14/0x20");

// ============================================================================
// BaseHeroMissionCondition —— 各英雄任务条件派生类基类。
// vtable 0x08b7f0d8 内容核实（objdump -s）：[offset/typeinfo]、D1 0x08164f3a、
// D0 0x08164f6e、__cxa_pure_virtual 0x08724fe0 ×2 —— 仅虚析构 + 两个纯虚槽
// （process / process2）；getter 与 setConditionData/rewardProcess 均非虚。
// ============================================================================
class BaseHeroMissionCondition
{
public:
    explicit BaseHeroMissionCondition(
        HeroMissionCondition::MissionNo::T no);      // T 0x08164f12 (C1/C2)
    virtual ~BaseHeroMissionCondition();             // D1/D2 0x08164f3a / D0 0x08164f6e

    virtual void process(CUser* user, HeroMissionValue* value,
                         unsigned int param) = 0;    // vptr+0x08（派生类实现）
    virtual void process2(CUser* user, HeroMissionValue* value,
                          unsigned int param) = 0;   // vptr+0x0c（派生类实现）

    void setConditionData(HeroMissionCondition& data);   // T 0x08164f8c
    unsigned int getMissionCode();                       // T 0x08164faa（this+4）
    unsigned int getMissionType();                       // T 0x08164fb6（condition+0）
    char isRepeatMission();                              // T 0x08164fc2（this+0x10）
    char isResetState();                                 // T 0x08164fce（this+0x11）
    std::string getMailTitle();                          // T 0x08164fda（condition+0xc sret）
    std::string getMailContent();                        // T 0x08165004（condition+0x10 sret）
    bool rewardProcess(CUser* user);                     // T 0x0816502e

    unsigned int m_missionCode;        // +0x04（ctor 写入）
    HeroMissionCondition m_condition;  // +0x08
};

#endif  // GAME_HEROMISSIONCONDITION_H_
