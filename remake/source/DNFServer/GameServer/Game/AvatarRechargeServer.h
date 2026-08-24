#ifndef DNF_GAME_AVATARRECHARGESERVER_H_
#define DNF_GAME_AVATARRECHARGESERVER_H_

// df_game_r Game/ AvatarRechargeServer（ARAD 脚本服务，独立 TU）。
//
// ORIG（df_game_r, elf32-i386）：
//   class AvatarRechargeServer          C1/D1 @0x08190200/0x08190246
//     vtable _ZTV20AvatarRechargeServer @0x8b9b950（16 字节 = 槽0 Load 0x0819026c、槽1 Destroy 0x081902de）
//   class ARAD::SCRIPT::AvatarRecharge  size = 0x50
//     （报告目录 docs/class_func_reports/ARAD__SCRIPT__AvatarRecharge/；C1 @0x088b0dd0）
//
// Server 布局（C1 0x08190200 反汇编）：
//   +0x00 ScriptInterface vptr（真实基类：ARAD::Singleton<Self> + ARAD::ScriptInterface）
//   +0x04 ARAD::SCRIPT::AvatarRecharge（0x50）
//   +0x54 int    m_enduranceReduceRate（ctor 清零；GmSetEnduranceReduceRate 写入）
//   +0x58 bool   m_bUseEnduranceReduceRate（ctor 清零；GmSetEnduranceReduceRate 写入）
//
// SCRIPT::AvatarRecharge 布局（C1 0x088b0dd0 / clearScript 0x088b12d8 实证）：
//   +0x00 int             充值上限（[avatar recharge limit]；ctor 不初始化，ORIG 同）
//   +0x04 map<int,int>    [emblem recharge stone info]（item_index→?；IsEmblemRechargeStone）
//   +0x1c map<int,int>    [emblem recharge]（grade→数值；getEmblemRecharge）
//   +0x34 map<int,int>    [recharge use emblem info]（grade→rate；GetRateFromEmblemGrade 读 this+0x38）
//   +0x4c bool            已装载标志（ctor/clearScript 清零，loadScript 成功置 1）

#include <map>

#include "Arad_DataManager.h"   // ARAD::ScriptInterface / Singleton

namespace arad { struct SigAradAvatarEnduranceSave; }
class CUser;  // 前置声明（CalcDurabilityChangeEquip 形参；完整定义见 CUser.h）

namespace ARAD {
namespace SCRIPT {

class AvatarRecharge
{
public:
    AvatarRecharge();    // ORIG C1 0x088b0dd0：三个 map 构造 + 清已装载标志
    ~AvatarRecharge();   // ORIG D1 0x088b0e46

    // ORIG 0x088b0eba：装载 "Etc/EmblemRenewal/"+"avatar_recharge_jpn.etc"。
    // 已装载直接返回 0；打开失败 -1；重复键 printf 后分别返回 -2/-3/-4；
    // 成功 unloadRDARScriptFile + 置已装载并返回 0。
    int loadScript();

    // ORIG 0x088b12d8：上限清零 + 三个 map clear。
    void clearScript();

    int getAvatarRecharageLimit();              // ORIG 0x088b1314：return *(int*)this
    bool IsEmblemRechargeStone(int itemIndex);  // ORIG 0x088b13a6：+0x04 map 命中即 true
    std::map<int, int>* getEmblemRechargeStoneInfo();  // ORIG 0x088b131e：return this+4
    int getEmblemRecharge(int grade);           // ORIG 0x088b132a：+0x1c map 值或 0

    int m_rechargeLimit;                        // +0x00（ctor 不初始化，忠实 ORIG）
    std::map<int, int> m_emblemRechargeStoneMap;  // +0x04
    std::map<int, int> m_emblemRechargeMap;       // +0x1c
    std::map<int, int> m_useEmblemGradeRateMap;   // +0x34
    bool m_loaded;                                // +0x4c
};

}  // namespace SCRIPT
}  // namespace ARAD

class AvatarRechargeServer : public ARAD::Singleton<AvatarRechargeServer>,
                             public ARAD::ScriptInterface
{
public:
    AvatarRechargeServer();
    ~AvatarRechargeServer();

    virtual bool Load();      // vtable 槽0（ORIG 0x0819026c）
    virtual void Destroy();   // vtable 槽1（ORIG 0x081902de 返 1；接口 void 化同 Manager 先例）

    void Test();              // ORIG 0x08190266：纯 ret

    // ORIG T 0x081905b4 _ZN20AvatarRechargeServer25CalcDurabilityChangeEquipEP5CUseri：
    // 换装（CUser::change_equip 头像分支 ChangeEquip 成功路径）后按 dstSlot 计算
    // 装备耐久变化；定义由 AvatarRechargeServer TU 批次提供。
    void CalcDurabilityChangeEquip(CUser* user, int equipSlot);
    // ORIG 0x0819114c：非静态成员（asm 读 this+0x38 → m_script.m_useEmblemGradeRateMap），
    // 命中返回 rate，未命中返回 0。调用方经 Singleton<AvatarRechargeServer>::Get() 访问。
    int GetRateFromEmblemGrade(int grade);

    // ORIG 0x08190dea / 0x08190eae：不触碰 this（静态），经 DB 队列发耐久保存流。
    static void SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save);
    static void SaveAvatarEnduranceUnit(int key, int value);

    ARAD::SCRIPT::AvatarRecharge m_script;  // +0x04
    int m_enduranceReduceRate;              // +0x54
    bool m_bUseEnduranceReduceRate;         // +0x58
};

#endif  // DNF_GAME_AVATARRECHARGESERVER_H_
