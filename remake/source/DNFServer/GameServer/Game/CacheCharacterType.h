#ifndef GAME_CACHECHARACTERTYPE_H_
#define GAME_CACHECHARACTERTYPE_H_

// ============================================================================
// df_game_r CACHE_CHARACTER_TYPE 权威布局（exchange_server 角色缓存单元）。
// 完整类型证据：ORIG C1 0x8492262 W（七次子对象构造）与 GetMemberPosition
// 0x848fd7c（区域偏移/尺寸表）、AcquireCharacter memset 0x3f85c。
// ============================================================================

#include "SigTypes.h"          // SIG_LOAD_CHARAC/INVENTORY/QUEST/ETC/KILL_MONSTER_INFO
#include "APSystemTypes.h"     // APSystem::_SIG_LOAD_ACTION_POINT（sizeof 0xcf1）
#include "advancealtar.h"      // advancealtar::_CharacAdvanceAltarDbData（0x51c）

// （ENUM_INTERNALPACKET 见下方守卫块；ORIG mangled
//   _ZN20CACHE_CHARACTER_TYPE17GetMemberPositionE19ENUM_INTERNALPACKETPiRb）
#ifndef DNF_GAME_ENUM_INTERNALPACKET_DEFINED
#define DNF_GAME_ENUM_INTERNALPACKET_DEFINED
// ORIG 全局枚举（mangled 无嵌套名 ⇒ 全局作用域）。权威值域 {0x89,0x8a}。
enum ENUM_INTERNALPACKET
{
    ENUM_INTERNALPACKET_0x89 = 0x89,
    ENUM_INTERNALPACKET_RECV_MAIL = 0x8a   // 推断命名
};
#endif
class CACHE_CHARACTER_TYPE
{
public:
    unsigned int getCacheTime() const          // +0x34880
    { return m_cacheTime; }
    void setCacheTime(unsigned int t)          // +0x34880
    { m_cacheTime = t; }

    CACHE_CHARACTER_TYPE();              // ORIG C1 0x8492262 W
    ~CACHE_CHARACTER_TYPE();             // ORIG D1/D2 0x84922d6 W
    // ORIG 0x848fd7c W：按 ENUM 返回成员区 (offset,size)；bool& 为命中标志。
    int GetMemberPosition(ENUM_INTERNALPACKET type, int* size, bool& found);

    // ORIG 成员布局（偏移=GetMemberPosition 返回值逐一对应；C1@0x8492262 对
    // charac/inventory/quest/actionPoint/altar/etc/expand 七区调用子对象构造；
    // 其余区域无 ctor/dtor ⇒ ORIG 内为 trivial POD，char 数组承载）。
    SIG_LOAD_CHARAC m_charac;                       // +0x00000  ENUM 0x07
    SIG_LOAD_INVENTORY m_inventory;                 // +0x00703  ENUM 0x08
    SIG_LOAD_QUEST m_quest;                         // +0x15b4f  ENUM 0x0a
    char m_ticket[0x19c];                           // +0x1d157  ENUM 0x0b（trivial）
    char m_dungeonClear[0x1810];                    // +0x1d2f3  ENUM 0x0c（trivial）
    char m_blackList[0x10];                         // +0x1eb03  ENUM 0xb3（trivial）
    APSystem::_SIG_LOAD_ACTION_POINT m_actionPoint; // +0x1eb13  ENUM 0x11
    advancealtar::_CharacAdvanceAltarDbData m_altar;// +0x1f804  ENUM 0x19
    SIG_LOAD_ETC m_etc;                             // +0x1fd20  ENUM 0x20
    long m_cacheTime;                               // +0x34880   缓存时间戳（AcquireCharacter 写 time(0)；
                                                    //   P217：time_t 同型直存消 4.4 mov%eax,%edx 中转，
                                                    //   sizeof/偏移不变，b28-L11 探针 l11_n92 实证）
    SIG_LOAD_KILL_MONSTER_INFO m_expand;            // +0x34884  ENUM 0x282
    char m_memberFlag[0xc];                         // +0x3f850   成员脏标记
};
static_assert(sizeof(CACHE_CHARACTER_TYPE) == 0x3f85c,
              "CACHE_CHARACTER_TYPE ABI drift (ORIG 0x3f85c)");

#endif  // GAME_CACHECHARACTERTYPE_H_
