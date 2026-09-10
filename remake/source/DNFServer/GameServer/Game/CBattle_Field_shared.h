#ifndef DNF_GAME_CBATTLE_FIELD_SHARED_H_
#define DNF_GAME_CBATTLE_FIELD_SHARED_H_

// df_game_r 还原 —— CBattle_Field 域共享支撑类型（原 CBattle_Field_deps.h 权威继承者）。
// 本头只持有 CBattle_Field 专属且无其它权威归属的类型：
//   ENUM_DUNGEON_DIFFICULTY / ENUM_BLOOD_STATE（战斗域枚举）
//   STKillMonsterData（kill_monster 输入载荷）
//   WongWork::stGenerateRefData_t / stGenerateResult_t（掉落生成引用；ctor/dtor 见
//     CBattle_Field_helpers.cpp / stGenerateRefData_t.cpp）
//   addUserHackCount149/150（enumPartyMember hack 计数回调）
// 其余类型（map_*/MapInfo/CDungeon/CMap/CParty/CGameManager/CWorldMap/CMonster/
// STMonsterScript/QuickParty/HellPartyGroup/CVillageMonsterMgr 等）均分发至各自权威头。

#include <vector>

#include "CItemGenerator.h"   // WongWork::stGenerateRefData_t/stGenerateResult_t
                              // 权威声明（唯一声明点，布局/ctor 证据见该头）

// village_attacked::CVillageMonsterMgr 完整类型只在 CBattle_Field_missing.cpp 等
// 直接消费者处 include CVillageMonsterMgr.h（避免与 GameWorld_deps.h 的 MapArea
// ODR 冲突）。
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
    // ORIG C1/C2 W 0x085bf03a：m_field0c=0 → m_field08=m_field0c →
    // m_dieReason=(char)m_field08 → m_mapIndex=(short)m_dieReason →
    // m_monsterUid=m_mapIndex → m_flag11=0 → m_flag10=m_flag11 → m_field14=1.0f
    // （链式逐字段，O0 生成同形 mov 序列）。
    STKillMonsterData()
    {
        m_field0c = 0;
        m_field08 = m_field0c;
        m_dieReason = static_cast<char>(m_field08);
        m_mapIndex = static_cast<short>(m_dieReason);
        m_monsterUid = m_mapIndex;
        m_flag11 = 0;
        m_flag10 = m_flag11;
        m_field14 = 1.0f;
    }

    unsigned short m_monsterUid;  // +0x00
    unsigned short m_mapIndex;    // +0x02
    char m_dieReason;             // +0x04
    char m_pad5[3];               // +0x05
    int m_field08;                // +0x08
    int m_field0c;                // +0x0c
    // [N5A 2026-09-11 · P248/q_probe] ORIG kill_monster 0x0830bccc/0x0830bcd9 的 store 为
    // `movzbl 0x10(%eax),%eax; mov %al,-0x96(%ebp)` 单条字节直存（无 test/setne），
    // 消费者 0x0830c8d5 `movzbl -0x96; xor $0x1; test; je` 为 bool 语义 ⇒ 字段是 bool。
    // 1 字节、sizeof(0x18)/偏移/mangled 零影响；写入点仅本结构 ctor 与 CPartyLink2.cpp:191-192
    // （写 0/1）。bool 字段 + bool 局部 ⇒ store 与消费者**两侧**同时复现 ORIG
    // （探针 q_probe.cpp Q1；char 字段 Q2 多 test+setne、uchar 局部 Q3 消费者退化）。
    bool m_flag10;                // +0x10
    bool m_flag11;                // +0x11
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

#endif  // DNF_GAME_CBATTLE_FIELD_SHARED_H_