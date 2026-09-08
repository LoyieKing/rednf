// df_game_r BattleData 还原（G2-5 批次）。
// CParty +0x32c，总尺寸 0x7f8。符号（nm -C，W/T 全实现）：
//   C1 0x082a4cea / D1 0x082a54d6 / Reset 0x082a4e8a /
//   GetHellPartyValueTotal 0x0822d7c4 / SetHellPartyValueTotal 0x0822d7b2 /
//   GetTotalKilledMonsterCount 0x085bf456 /
//   IncEventDungeonDestoryObjectPoint 0x085be6e2 /
//   ResetEventDungeonClearPoint 0x085be6ba /
//   ClearEventDungeonRoom 0x085be7aa / IsClearEventDungeonRoom 0x085be862
// 布局（由 Reset/构造/析构反汇编推导）：
//   +0x00 int[10] 字段区、+0x28 char[4]、+0x2c int[9]（含 3 个击杀计数）、
//   +0x50 char[4]x5、+0x64 int[4]、
//   +0x74 Inven_Item[4][2]（0x3d 每项）、+0x25c Inven_Item[4][2]、
//   +0x444 Inven_Item[4][2]、+0x62c stEventClearData[12]（0x8 每项）、
//   +0x68c int、+0x690 GameResultType[4]（0x50 每项）、+0x7d0 int[4]、
//   +0x7e0 float m_hellPartyValueTotal、+0x7e4 char[4]、+0x7e8 int[4]。
// Inven_Item/GameResultType 跨类方法以 extern asm 标签引用（ORIG 真实符号）。

#include <string.h>

#include "BattleData.h"
#include "Inven_Item.h"
#include "GameResultType.h"

// ============================================================================
// 构造 / 析构 / Reset
// ============================================================================

// ORIG 0x082a4cea：ctor 体内只有 Reset()；三组 Inven_Item[4][2] 与
// GameResultType[4] 的构造全部由编译器隐式生成（含 EH 反向析构 landing pad），
// 不存在 placement-new（ORIG 无 _ZnwjPv 调用点）。
BattleData::BattleData()
{
    Reset();
}

// ORIG 0x082a54d6：空体，仅隐式析构 GameResultType[4]（反向 0x140 → 0）。
BattleData::~BattleData()
{
}

// ORIG 0x82a4e90 头部清零为 4.1 dest 缓存形：每对字段一次 this 载入+双 store
//（0x0..0x24 五对、0x2c..0x48 四对、0x4c 单、0x28..0x2b 字节逐条、0x68c 单）。
// 4.4 函数体逐条重载 this 不可造（探针 /tmp/l0_probe2.cpp 逐句/链式/逗号三形
// 实测全不复现）——经 8 字节清零视图折叠复现（探针 /tmp/l0_probe4.cpp ResetG
// 与 /tmp/l0_probe3.cpp ResetF 同形实证）。视图仅表达布局，字段语义仍以头内
// m_fieldN/m_killedMonsterN 为权威。
namespace {
struct BattleDataZeroView {
    long long a[5];   // +0x00 m_field0/4 .. m_field20/24
    char pad28[4];    // +0x28 m_flag28[0..3]（字节域另逐字节清）
    long long b[4];   // +0x2c m_field2c/30 .. m_killedMonster2/m_field48
    int c;            // +0x4c m_field4c
};
} // namespace

void BattleData::Reset()
{
    ((BattleDataZeroView*)this)->a[0] = 0;
    ((BattleDataZeroView*)this)->a[1] = 0;
    ((BattleDataZeroView*)this)->a[2] = 0;
    ((BattleDataZeroView*)this)->a[3] = 0;
    ((BattleDataZeroView*)this)->a[4] = 0;
    ((BattleDataZeroView*)this)->b[0] = 0;   // 0x2c/0x30 成对
    ((BattleDataZeroView*)this)->b[1] = 0;   // 0x34/0x38 成对
    m_killedMonster0 = 0;                    // ORIG 0x3c..0x4c 各独立单 store
    m_killedMonster1 = 0;
    m_killedMonster2 = 0;
    m_field48 = 0;
    m_field4c = 0;
    m_flag28[0] = 0;
    m_flag28[1] = 0;
    m_flag28[2] = 0;
    m_flag28[3] = 0;
    m_field68c = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_arr50[i] = 0;
        m_arr54[i] = 0;
        m_arr58[i] = 0;
        m_arr5c[i] = 0;
        m_arr60[i] = 0;
        m_gameResults[i].Clear();
        m_arr7e8[i] = 0x7fffffff;
        m_arr64[i] = 0;
        reinterpret_cast<Inven_Item*>(&m_itemGroups0[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups0[i][1])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups1[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups1[i][1])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups2[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups2[i][1])->reset();
        m_arr7d0[i] = 0;
        m_arr7e4[i] = 0;
    }
}

int BattleData::GetHellPartyValueTotal()
{
    return (int)m_hellPartyValueTotal;
}

void BattleData::SetHellPartyValueTotal(int value)
{
    m_hellPartyValueTotal = (float)value;
}

int BattleData::GetTotalKilledMonsterCount() const
{
    return m_killedMonster0 + m_killedMonster1 + m_killedMonster2;
}

// b28d-40: 真 bug 已修（比较方向 >=）+ P183 去缓存 + goto fail 共享出口；
// 残差 = ORIG ebx 缓存 roomIndex + 单累加器 add 链 vs 4.4 lea（N4/P30 封顶）。
// ORIG 0x085be7aa。
void BattleData::IncEventDungeonDestoryObjectPoint(
    int param_1, const CMDPacketStruct::_STReqEventDungeonDestoryObject& st)
{
    if (st.m_roomIndex >= 3)
    {
        return;
    }
    // b28d-40: P183 去 idx 缓存（ORIG 0x085be711/0x085be761 两臂各一次全量重算索引）。
    if (st.m_destroyType == 1)
    {
        m_eventClear[param_1 * 3 + st.m_roomIndex].m_monsterIdx += st.m_point;
    }
    else
    {
        m_eventClear[param_1 * 3 + st.m_roomIndex].m_roomIdx += st.m_point;
    }
}

void BattleData::ResetEventDungeonClearPoint()
{
    memset(m_eventClear, 0, sizeof(m_eventClear));
}

// b28d-40: 真 bug 已修（st.m_monsterIndex >= param_2，原写 <= 反向；ORIG 0x085be82b
// `cmp 0x10(%ebp),%eax; jl fail`）+ P183 去 idx 缓存 + 共享 fail 出口复现；
// 残差 = 索引 add-vs-lea + ebx callee-saved（N4/P30 封顶）。ORIG 0x085be7aa。
int BattleData::ClearEventDungeonRoom(
    int param_1, int param_2, const CMDPacketStruct::_STReqEventDungeonClearRoom& st)
{
    // b28d-40: 真语义 bug 修正——ORIG 0x085be82b `cmp 0x10(%ebp),%eax; jl fail`
    // ⇒ 成功条件是 st.m_monsterIndex >= param_2（原码写成 <= 反向）。
    // 同时按 P183 去 idx 缓存（ORIG 三次独立全量重算索引）、P133 拆独立守卫。
    if (st.m_roomIndex <= 2)
    {
        if (m_eventClear[param_1 * 3 + st.m_roomIndex].m_monsterIdx != st.m_monsterIndex)
            goto fail;
        if (m_eventClear[param_1 * 3 + st.m_roomIndex].m_roomIdx != st.m_monsterCount)
            goto fail;
        if (st.m_monsterIndex < param_2)
            goto fail;
        m_eventClear[param_1 * 3 + st.m_roomIndex].m_clearFlag = 1;
        return 1;
    }
    return 0;
fail:
    return 0;
}

char BattleData::IsClearEventDungeonRoom(int param_1, unsigned short roomIdx) const
{
    // b28d-40: N4/P30 封顶——ORIG 0x085be87e 索引链用 %ebx 缓存 roomIdx（push %ebx、
    // 槽 -0x8）且单累加器 add；4.4 用 %eax + `lea (%eax,%edx,1),%edx`（下标序交换
    // 实测无效）。add-vs-lea + callee-saved 选择语义全等。ORIG 0x085be862。
    if (roomIdx >= 3)
    {
        return 0;
    }
    return m_eventClear[param_1 * 3 + roomIdx].m_clearFlag;
}
