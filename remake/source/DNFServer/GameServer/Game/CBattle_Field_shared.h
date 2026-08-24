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

#include "CItemGenerator.h"   // WongWork::stGenerateRefData_t/stGenerateResult_t
                              // 权威声明（唯一声明点，布局/ctor 证据见该头）

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

// ---- CMap 前向声明 ----
// CMap 唯一权威定义在 CMap.h（sizeof 0x130，含 getAICharacters/getAPCRandomPoint/
// get_index 等完整访问器）。历史上此处曾放置最小 CMap mirror（标量访问器视图），
// 与 CMap.h 完整定义构成 ODR 冲突/可见性分裂（同一 TU 内 include 顺序不同会得到
// 缺少 getAICharacters/getAPCRandomPoint 的错误 CMap）。现改为前向声明，保证
// CMap 永远由 CMap.h 唯一权威定义；需要完整 CMap 的 TU（CBattle_Field.h 已 include
// CMap.h）直接使用完整类型。
class CMap;


// ---- hack 计数回调（enumPartyMember 使用；实现见 CBattle_Field_helpers.cpp） ----
bool addUserHackCount149(CUser* user);  // ORIG 0x830dc9d
bool addUserHackCount150(CUser* user);  // ORIG 0x830dce4

// ---- GlobalData 静态成员（asm label 直引） ----
extern village_attacked::CVillageMonsterMgr* GlobalData_s_villageMonsterMgr
    asm("_ZN10GlobalData19s_villageMonsterMgrE");

#endif  // DNF_GAME_CBATTLE_FIELD_SHARED_H_