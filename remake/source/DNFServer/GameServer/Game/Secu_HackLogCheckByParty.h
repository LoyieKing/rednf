// df_game_r Secu_HackLogCheckByParty —— 唯一声明点（Secu_HackLogCheckByParty.cpp 实现；CParty 子对象 +0x1abc，总尺寸 0x14）。
// 布局：+0x00 Secu_DungeonAverageCheck 基类（0x10：CParty* + vector 占位）
//   +0x10 CParty* m_party2
#ifndef GAME_SECU_HACKLOGCHECKBYPARTY_H_
#define GAME_SECU_HACKLOGCHECKBYPARTY_H_

class CParty;
class CUser;
class CMap;
class map_monster;
#include <vector>
#include "PacketDispatcher.h"

// MSG_MONSTER_DIE（网络填充结构，ORIG 访问至少到 +0xa21；
// setKillMonsterInfo 0x82794d6 使用 +0xa1e/+0xa1a 两个计数值——语义推断：
// +0xa1e 为耗时类、+0xa1a 为经验类累计）。
struct __attribute__((packed)) MSG_MONSTER_DIE : MSG_BASE
{
    // ORIG wire 布局按 DisPatcher_DieMob::dispatch_sig（0x081eb0c4）读取序
    // 全量展开；数组容量/偏移由循环寻址精确复现（256 项、无重叠）。
    unsigned short m_mobSeq;            // +0x0d（击杀的 mob 序号）
    short m_fieldF;                     // +0x0f（==0xffff 标记；其余为 killer uid）
    int m_field11;                      // +0x11
    int m_field15;                      // +0x15
    unsigned char m_mobCount;           // +0x19（四数组有效项数）
    unsigned short m_skillList[256];    // +0x1a（per-mob 技能 id，命中 0x111 过伤害检查）【语义推断】
    unsigned short m_attackerList[256]; // +0x21a（per-mob 造成伤害者 uniqueId）【语义推断】
    unsigned int m_damageList[256];     // +0x41a（per-mob 累计伤害值）【语义推断】
    unsigned short m_hitCountList[256]; // +0x81a（per-mob 攻击次数；0 时报 0x6e 并置 1）
    unsigned int m_killValue2;          // +0xa1a（经验类累计【推断】，%10 联检）
    unsigned int m_killValue1;          // +0xa1e（耗时类累计【推断】，>0x5dc 检查）
    unsigned char m_fieldA22;           // +0xa22
    unsigned char m_fieldA23;           // +0xa23（原 m_pad23）
    unsigned short m_killPairA24;       // +0xa24（ValidateFixMobDiePos x）
    unsigned short m_killPairA26;       // +0xa26（ValidateFixMobDiePos y）
    char m_fieldA28;                    // +0xa28（原 m_pad28）
    short m_fieldA29;                   // +0xa29
    short m_fieldA2b;                   // +0xa2b
    int m_fieldA2d;                     // +0xa2d
    int m_fieldA31;                     // +0xa31
    short m_fieldA35;                   // +0xa35
    short m_fieldA37;                   // +0xa37
    short m_fieldA39;                   // +0xa39
    short m_fieldA3b;                   // +0xa3b
    short m_fieldA3d;                   // +0xa3d
};

// SecKillMonsterInfo（0x18；ctor ORIG 随 push_back 内联，字段偏移实证）。
struct SecKillMonsterInfo
{
    SecKillMonsterInfo() : m_monsterId(0), m_count(0), m_sumValue1(0),
                           m_sumValue2(0), m_hackCntE6(0), m_hackCntE7(0) {}
    // [P248 档③ 重试 · 依据 ORIG 0x08279d30] ORIG 为 LHS-first
    //   （`mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,(%eax)` ×4）；
    //   旧判词把「字段 int / 形参 unsigned 异型」当封顶，但 P248 要求在**字段侧**
    //   同型化（int→unsigned 与实参同型）后 4.4 才翻 LHS-first ⇒ 本波同型 + 删 cast。
    //   4×4B 同宽 ⇒ sizeof 0x18 / 全偏移 / mangled 零变化（static_assert 锁）。
    SecKillMonsterInfo(unsigned int monsterId, unsigned int count,
                       unsigned int v1, unsigned int v2)
        : m_monsterId(monsterId), m_count(count), m_sumValue1(v1),
          m_sumValue2(v2), m_hackCntE6(0), m_hackCntE7(0) {}

    unsigned int m_monsterId;     // +0x00（P248 档③：与 ctor 形参同型）
    unsigned int m_count;         // +0x04 击杀次数
    unsigned int m_sumValue1;     // +0x08 耗时类累计
    unsigned int m_sumValue2;     // +0x0c 经验类累计
    int m_hackCntE6;     // +0x10 已上报 0xe6 次数
    int m_hackCntE7;     // +0x14 已上报 0xe7 次数
};
static_assert(sizeof(SecKillMonsterInfo) == 0x18, "SecKillMonsterInfo ABI drift");
// ---- Secu_DungeonAverageCheck（0x10；ctor/dtor/方法由 ORIG/桩提供）----
class Secu_DungeonAverageCheck
{
public:
    Secu_DungeonAverageCheck();
    ~Secu_DungeonAverageCheck();

    void SetPartyPtr(CParty* party);
    void reset();
    void startDungeon();
    void dungeonClear();
    void dungeonFail();
    void CheckHackTypeEndDungeon();   // ORIG T 0x08279a12（平均等级 hack 上报）
    std::vector<SecKillMonsterInfo>::iterator
    findKillMonsterInfo(unsigned int monsterId);   // ORIG 0x827993c
    void setKillMonsterInfo(const MSG_MONSTER_DIE* msg, map_monster* mob);

    CParty* m_party;    // +0x00
    std::vector<SecKillMonsterInfo> m_killInfos;   // +0x04..+0x0f（原 pad 占位）


};
// ---- Secu_HackLogCheckByParty（sizeof 0x14）----
class Secu_HackLogCheckByParty : public Secu_DungeonAverageCheck
{
public:
    Secu_HackLogCheckByParty();
    ~Secu_HackLogCheckByParty();

    void Init(CParty* party);
    void DieMob(const MSG_MONSTER_DIE* msg, map_monster* mob);
    void CheckMoveMapFitInPathHackCnt(CUser* user, CParty* party,
                                      unsigned int a, unsigned int b,
                                      int mapIdx);
    void startDungeon();
    void dungeonClear();
    void dungeonFail();

    CParty* m_party2;   // +0x10
};

#endif  // GAME_SECU_HACKLOGCHECKBYPARTY_H_