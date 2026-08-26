#ifndef DNF_GAME_AVATARFIXEDHIDDENOPTIONSERVER_H_
#define DNF_GAME_AVATARFIXEDHIDDENOPTIONSERVER_H_

// df_game_r Game/ AvatarFixedHiddenOptionServer（ARAD 脚本服务，ORIG 独立 TU 域）。
//
// ORIG（df_game_r, elf32-i386）：
//   class AvatarFixedHiddenOptionServer          size = 0x98
//     C1 _ZN29AvatarFixedHiddenOptionServerC1Ev @0x0817e02c
//     D1 _ZN29AvatarFixedHiddenOptionServerD1Ev @0x0817e0e6
//     vtable _ZTV29AvatarFixedHiddenOptionServer @0x8b8ac58（16 字节 = 槽0 Load 0x0817e154、槽1 Destroy 0x0817e1e2）
//   class ARAD::SCRIPT::AvatarFixedHiddenOption  size = 0x4c
//     （报告目录 docs/class_func_reports/ARAD__SCRIPT__AvatarFixedHiddenOption/）
//   struct STAvatarHiddenAbilityVariation        size = 0x14
//     ctor(T,int,int) W 0x088b01e8 / ctor(T,float,int) W 0x088b021a / dtor W 0x088b0aae
//
// Server 布局（C1 0x0817e02c 反汇编）：
//   +0x00 ScriptInterface vptr（真实基类，vtable 槽0=Load 槽1=Destroy）
//   +0x04 ARAD::SCRIPT::AvatarFixedHiddenOption（0x4c：6×vector@+0x00..+0x47、int 材料索引@+0x48）
//   +0x50 std::vector<int> m_cumulativeRate[6]（Init 把各桶 m_nRate 累加成前缀和，+0x50..+0x97）

namespace arad { struct SigAvatarHiddenOptionChange; }

#include <vector>
class CUser;

#include <string>

#include "Arad_DataManager.h"   // ARAD::ScriptInterface / Singleton
#include "CItemAmplifier.h"     // ENUM_RARITY（DNF_ENUM_RARITY_DEFINED 守卫）

// ----------------------------------------------------------------------------
// AVATAR_HIDDEN_ABILITY_TYPE —— ORIG 枚举嵌套于同名 struct（mangled
// N26AVATAR_HIDDEN_ABILITY_TYPE1TE）。成员名按 parseAbility 标签语义命名【推断】，
// 数值由 parseAbility 构造实参（1..21）实证。
// ----------------------------------------------------------------------------
struct AVATAR_HIDDEN_ABILITY_TYPE
{
    enum T
    {
        NONE                          = 0,
        PHYSICAL_ATTACK               = 1,   // [physical attack]
        MAGICAL_ATTACK                = 2,   // [magical attack]
        PHYSICAL_DEFENSE              = 3,   // [physical defense]
        MAGICAL_DEFENSE               = 4,   // [magical defense]
        HP_MAX                        = 5,   // [HP MAX]
        MP_MAX                        = 6,   // [MP MAX]
        HP_REGEN_SPEED                = 7,   // [HP regen speed]
        MP_REGEN_SPEED                = 8,   // [MP Regen speed]
        ATTACK_SPEED                  = 9,   // [attack speed]
        MOVE_SPEED                    = 10,  // [move speed]
        CAST_SPEED                    = 11,  // [cast speed]
        INVENTORY_LIMIT               = 12,  // [inventory limit]
        STUCK                         = 13,  // [stuck]（浮点值）
        STUCK_RESISTANCE              = 14,  // [stuck resistance]
        ALL_ACTIVESTATUS_RESISTANCE   = 15,  // [all activestatus resistance]
        HIT_RECOVERY                  = 16,  // [hit recovery]
        EQUIPMENT_MAGICAL_DEFENCE     = 17,  // [equipment magical defence]
        EQUIPMENT_PHYSICAL_DEFENCE    = 18,  // [equipment physical defence]
        JUMP_POWER                    = 19,  // [jump power]
        PHYSICAL_CRITICAL_HIT         = 20,  // [physical critical hit]（浮点值）
        MAGICAL_CRITICAL_HIT          = 21,  // [magical critical hit]（浮点值）
    };
};

// ----------------------------------------------------------------------------
// STAvatarHiddenAbilityVariation —— 字段布局由两个 W ctor 反汇编实证：
//   int 重载：type=arg2 →+0x00、+0x04=0（整型标志）、+0x08=0、value=arg3→+0x0c、rate=arg4→+0x10
//   float 重载：type→+0x00、+0x04=1、float value=arg3→+0x08、+0x0c=0、rate=arg4→+0x10
// +0x10 为随机权重（Server::Init 累加成前缀和表）。
// ----------------------------------------------------------------------------
struct STAvatarHiddenAbilityVariation
{
    STAvatarHiddenAbilityVariation(AVATAR_HIDDEN_ABILITY_TYPE::T type, int value, int rate);
    STAvatarHiddenAbilityVariation(AVATAR_HIDDEN_ABILITY_TYPE::T type, float value, int rate);
    ~STAvatarHiddenAbilityVariation();

    AVATAR_HIDDEN_ABILITY_TYPE::T m_type;   // +0x00
    bool m_bFloatValue;                     // +0x04
    float m_fValue;                         // +0x08（仅 float 重载写入）
    int m_nValue;                           // +0x0c（仅 int 重载写入）
    int m_nRate;                            // +0x10（概率权重，Init 累加）
};

class AvatarFixedHiddenOptionServer;

namespace ARAD {
namespace SCRIPT {

// ----------------------------------------------------------------------------
// AvatarFixedHiddenOption —— 脚本加载器子对象（Server @+0x04，size 0x4c）。
// ----------------------------------------------------------------------------
class AvatarFixedHiddenOption
{
public:
    AvatarFixedHiddenOption();    // ORIG C1 0x088aecac：构造 6 个 vector 后 clearScript()
    ~AvatarFixedHiddenOption();   // ORIG D1 0x088aed5e：clearScript + 逆序析构 6 个 vector

    // ORIG 0x088aedce：装载 "Etc/Avatar_Roulette/"+"AvatarFixedHiddenOptionList.etc"；
    // "[upper]" 段元素入 m_abilities[2]、"[rare]" 入 m_abilities[3]（ORIG this+0x18/+0x24）、
    // "[mystic circle]" 读材料物品索引。返回 0=成功 / -1=打开脚本失败。
    // 注：忠实转写 ORIG 控制流——段切换时首个标签会被内层 ScanType 消费并因
    // parseAbility 未命中而回落外层（ORIG 同样如此）。
    int loadScript();

    // ORIG 0x088b00e0：清空 6 个桶并各回填一个默认元素（type=NONE/value/rate 全 0）。
    void clearScript();

    int getMaterialItemIndex();                 // ORIG 0x088b0164：return *(int*)(this+0x48)
    bool isMaterialItemIndex(int itemIndex);    // ORIG 0x088b0170：return *(int*)(this+0x48)==itemIndex

public:
    // ORIG 0x088aef52：函数级静态映射（21 标签→内部 id 0x46..0xac）首次调用时构建；
    // 按 id 读 2 个数值构造元素 push 到 variations。stuck/物理/魔法暴击第一个数值为
    // ScanFloat；equipment defence 首个 ScanInt 弃读；未知标签返回 false。
    bool parseAbility(std::vector<STAvatarHiddenAbilityVariation>& variations,
                      std::string& tag);

    std::vector<STAvatarHiddenAbilityVariation> m_abilities[6];  // +0x00..+0x47
    int m_materialItemIndex;                                     // +0x48
};

}  // namespace SCRIPT
}  // namespace ARAD

// ----------------------------------------------------------------------------
// AvatarFixedHiddenOptionServer —— 服务包装（Singleton + ScriptInterface 双基，
// SCRIPT 子对象@+0x04，前缀和桶@+0x50）。ORIG vtable 仅 [Load, Destroy] 两槽。
// ----------------------------------------------------------------------------
class AvatarFixedHiddenOptionServer : public ARAD::Singleton<AvatarFixedHiddenOptionServer>,
                                      public ARAD::ScriptInterface
{
public:
    AvatarFixedHiddenOptionServer();
    ~AvatarFixedHiddenOptionServer();

    virtual bool Load();      // vtable 槽0（ORIG 0x0817e154）
    virtual void Destroy();   // vtable 槽1（ORIG 0x0817e1e2 返 1；接口 void 化同 Manager 先例）

    // ORIG 0x0817e3e8：对 6 个桶跳过下标 0 的种子元素，把 m_nRate 累加写入
    // m_cumulativeRate[i]（reserve(count-1)，与 ORIG 一致依赖 clearScript 的种子保证非空）。
    // 恒返回 1。
    int Init();

    // ORIG 0x0817e4c6 CheckCondition / 0x0817e8d6 ChangeHiddenOption
    // （Arad_INTER_kAvatarHiddenOptionChange 派发链调用）。仅声明，定义由
    // AvatarFixedHiddenOptionServer TU 批次提供。
    bool CheckCondition(CUser* user, arad::SigAvatarHiddenOptionChange* sig);
    bool ChangeHiddenOption(CUser* user, arad::SigAvatarHiddenOptionChange* sig);
    // ---- ORIG T（ChangeHiddenOption 依赖；LinkClosureB.AradServers 定义归
    //      AvatarFixedHiddenOptionServer.cpp）----
    // ORIG _ZN29AvatarFixedHiddenOptionServer13GetSkillIndexE11ENUM_RARITY @0x0817e1ec
    int GetSkillIndex(ENUM_RARITY rarity);
    // ORIG _ZN29AvatarFixedHiddenOptionServer13GetSkillIndexE11ENUM_RARITYi @0x0817e23e
    int GetSkillIndex(ENUM_RARITY rarity, int randIdx);
    // ORIG _ZN29AvatarFixedHiddenOptionServer14GetRandomInputE11ENUM_RARITY @0x0817e334
    int GetRandomInput(ENUM_RARITY rarity);
    // ORIG _ZN29AvatarFixedHiddenOptionServer17IsVaildSkillIndexE11ENUM_RARITYi @0x0817e392
    bool IsVaildSkillIndex(ENUM_RARITY rarity, int idx);

    ARAD::SCRIPT::AvatarFixedHiddenOption m_script;  // +0x04
    std::vector<int> m_cumulativeRate[6];            // +0x50..+0x97
};

#endif  // DNF_GAME_AVATARFIXEDHIDDENOPTIONSERVER_H_
