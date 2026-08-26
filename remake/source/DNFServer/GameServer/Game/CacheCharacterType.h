#ifndef GAME_CACHECHARACTERTYPE_H_
#define GAME_CACHECHARACTERTYPE_H_

// ============================================================================
// df_game_r CACHE_CHARACTER_TYPE 权威布局（exchange_server 角色缓存单元）。
// 2026-08-27 自 CGameManager.cpp 提取为独立头（OnlyOrig_Exchange 批：
//   exchange_server::CCacheCharacterMgr/StaticPool 显式实例化跨 TU 需要完整类型）。
// 定义证据与字段语义见原注释（GetMemberPosition@0x848fd7c / C1@0x8492262 /
// AcquireCharacter memset 0x3f85c）。
// ============================================================================

// （ENUM_INTERNALPACKET 见下方守卫块；ORIG mangled
//   _ZN20CACHE_CHARACTER_TYPE17GetMemberPositionE19ENUM_INTERNALPACKETPiRb）
#ifndef DNF_GAME_ENUM_INTERNALPACKET_DEFINED
#define DNF_GAME_ENUM_INTERNALPACKET_DEFINED
// ORIG 全局枚举（mangled _ZN20CACHE_CHARACTER_TYPE17GetMemberPositionE19ENUM_
// INTERNALPACKETPiRb 无嵌套名 ⇒ 全局作用域）。权威值域 {0x89,0x8a}；
// 与 CGameManager.h 同守卫副本（keeper 范式），零跨头耦合。
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

    // ORIG 成员布局（offset 与 GetMemberPosition 返回值逐一对应；size 为该 ENUM 区尺寸）
    // 全部用 char 数组承载 ORIG 精确尺寸：不依赖 SIG_LOAD_* 类型（并发改型中），
    // 亦规避 offsetof 在含用户构造函数的类上的不可靠性（仅 sizeof 断言把关）。
    char m_charac[0x703];        // +0x00000   ENUM 0x07, size 0x703
    char m_inventory[0x1544c];   // +0x00703   ENUM 0x08, size 0x1544c
    char m_quest[0x7608];        // +0x15b4f   ENUM 0x0a, size 0x7608
    char m_ticket[0x19c];        // +0x1d157   ENUM 0x0b, size 0x19c
    char m_dungeonClear[0x1810]; // +0x1d2f3   ENUM 0x0c, size 0x1810
    char m_blackList[0x10];      // +0x1eb03   ENUM 0xb3, size 0x10
    char m_actionPoint[0xcf1];   // +0x1eb13   ENUM 0x11, size 0xcf1
    char m_altar[0x51c];         // +0x1f804   ENUM 0x19, size 0x51c
    char m_etc[0x14b60];         // +0x1fd20   ENUM 0x20, size 0x14b60
    unsigned int m_cacheTime;    // +0x34880   缓存时间戳（AcquireCharacter 写 time(0)）
    char m_expand[0xafcc];       // +0x34884   ENUM 0x282, size 0xafcc
    char m_memberFlag[0xc];      // +0x3f850   成员脏标记（GetMemberPosition 按 +0x3f850+i 读）
};
static_assert(sizeof(CACHE_CHARACTER_TYPE) == 0x3f85c,
              "CACHE_CHARACTER_TYPE ABI drift (ORIG 0x3f85c)");

#endif  // GAME_CACHECHARACTERTYPE_H_
