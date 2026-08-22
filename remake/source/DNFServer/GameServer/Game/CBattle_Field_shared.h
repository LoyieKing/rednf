#ifndef DNF_GAME_CBATTLE_FIELD_SHARED_H_
#define DNF_GAME_CBATTLE_FIELD_SHARED_H_

// df_game_r 还原 —— CBattle_Field 域共享支撑类型（原 CBattle_Field_deps.h 权威继承者）。
// 本头只持有 CBattle_Field 专属且无其它权威归属的类型：
//   ENUM_DUNGEON_DIFFICULTY / ENUM_BLOOD_STATE（战斗域枚举）
//   STKillMonsterData（kill_monster 输入载荷）
//   WongWork::stGenerateRefData_t / stGenerateResult_t（掉落生成引用；ctor/dtor 见
//     CBattle_Field_helpers.cpp / stGenerateRefData_t.cpp）
//   addUserHackCount149/150（enumPartyMember hack 计数回调）
//   GlobalData_s_villageMonsterMgr（GlobalData 静态成员，asm label 直引，
//     避免整头 include GlobalData.h 与 CUser.h/CUserCharacInfo.h 冲突）
// 其余类型（map_*/MapInfo/CDungeon/CMap/CParty/CGameManager/CWorldMap/CMonster/
// STMonsterScript/QuickParty/HellPartyGroup/CVillageMonsterMgr 等）均分发至各自权威头。

#include <vector>

// village_attacked::CVillageMonsterMgr 完整类型只在 CBattle_Field_missing.cpp 等
// 直接消费者处 include CVillageMonsterMgr.h（避免与 GameWorld_deps.h 的 MapArea
// ODR 冲突；此处仅前向声明供 extern 引用）。
namespace village_attacked
{
class CVillageMonsterMgr;
}

class CUser;

// ---- 枚举（CBattle_Field 域；无其它权威头承载） ----
#ifndef DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED
enum ENUM_DUNGEON_DIFFICULTY { ENUM_DUNGEON_DIFFICULTY_0 = 0 };
#endif
#ifndef DNF_ENUM_ENUM_BLOOD_STATE_DEFINED
#define DNF_ENUM_ENUM_BLOOD_STATE_DEFINED
enum ENUM_BLOOD_STATE { ENUM_BLOOD_STATE_0 = 0 };
#endif

// ---- kill_monster 输入（ORIG 布局：+0 ushort uid、+2 ushort、+4 char、+8 int、
//      +0xc int、+0x10/+0x11 char、+0x14 float） ----
struct STKillMonsterData
{
    unsigned short m_monsterUid;  // +0x00
    unsigned short m_mapIndex;    // +0x02
    char m_dieReason;             // +0x04
    char m_pad5[3];               // +0x05
    int m_field08;                // +0x08
    int m_field0c;                // +0x0c
    char m_flag10;                // +0x10
    char m_flag11;                // +0x11
    char m_pad12[2];              // +0x12
    float m_field14;              // +0x14
};

// ---- CMap 最小访问视图 ----
// CMap.h 与 CBattle_Field.h 因 _mapMonster/_mapItem 等结构体同名（布局一致但类型
// 各异）互斥，无法在同一 TU 内 include CMap.h；此处提供 CMap 标量访问器最小视图
// （与 CMap.h 布局一致）。若真实 CMap.h 先被 include（定义了 CMAP_CLASS_DEFINED），
// 本声明自动跳过，避免重复定义。完整 CMap 整合（_mapMonster 去重）为独立 G2 任务。
#ifndef CMAP_CLASS_DEFINED
class CMap
{
public:
    int GetBossCount() const { return m_bossCount; }   // +0x00（CMap.h inline）
    int get_index() const   { return m_4; }            // +0x04
    int get_map_type() const { return m_8; }           // +0x08
    int m_bossCount;                 // +0x00
    int m_4;                         // +0x04
    int m_8;                         // +0x08
};
#endif  // CMAP_CLASS_DEFINED

namespace WongWork
{

// stGenerateRefData_t（掉落生成引用；ctor ORIG 0814a706 / dtor 0814aaf0）。
// 必要字段视图（setPremiumDropRate 写 m_premiumDropRate；setPremiumDropRate 读 m_flag40）；
// 完整布局与 ctor/dtor 实现见 CBattle_Field_helpers.cpp。
struct stGenerateRefData_t
{
    char m_pad0[0x20];
    int m_premiumDropRate;      // +0x20
    char m_pad24[0x1c];         // +0x24
    char m_flag40;              // +0x40

    stGenerateRefData_t();
    ~stGenerateRefData_t();
};

struct stGenerateResult_t
{
    char m_pad[0xc];
    stGenerateResult_t();
    ~stGenerateResult_t();
};

}  // namespace WongWork

// ---- hack 计数回调（enumPartyMember 使用；实现见 CBattle_Field_helpers.cpp） ----
bool addUserHackCount149(CUser* user);  // ORIG 0x830dc9d
bool addUserHackCount150(CUser* user);  // ORIG 0x830dce4

// ---- GlobalData 静态成员（asm label 直引） ----
extern village_attacked::CVillageMonsterMgr* GlobalData_s_villageMonsterMgr
    asm("_ZN10GlobalData19s_villageMonsterMgrE");

#endif  // DNF_GAME_CBATTLE_FIELD_SHARED_H_