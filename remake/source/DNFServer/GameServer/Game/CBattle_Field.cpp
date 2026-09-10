// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBattle_Field/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CBattle_Field（G2-3 第一批）。
// 逐函数对照 docs/class_func_reports/CBattle_Field.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径，分支/调用目标→<T>、
// 大绝对地址→<A>、去@plt、剥尾部 nop、C2/D2 同址别名良性）。
// 已并入原 CBattle_Field_map.cpp（地图/移动小簇）；草稿文件已删除。
#include <cstring>

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"
#include "PacketGuard.h" // 权威 PacketGuard(0xc)
#include "CQuickPartyRewardManager.h"
#include "GameRand.h"  // get_rand_int

// ============================================================================

// ============================================================================
// 外部方法（权威头已提供声明：MazeScript::getGridR 见 CDungeon.h，
// CSystemTime::getCurTickCount 见 CSystemTime.h，CRidable::CheckAppearRidableObject
// 见 CRidable.h；GlobalData::s_systemTime_ 见 CUserCharacInfo.h）
// ============================================================================
CDataManager* G_CDataManager();

// CDungeon 局部布局代理（真实 CDungeon.h 已含 +0x8a0 字段布局的完整类型）：
// 本代理仅用于 +0x8a0 字段直接位移访问，保持 ORIG 直接位移加载形态。
struct CDungeon_8a0_proxy
{
    char m_pad[0x8a0];
    int m_8a0;
};

// G_CDataManager() +0x6170 处的 int 表视图（incUltimateRoundClearPoint 08307096
// 实证 add $0x185c,%edx + (%eax,%edx,4) 寻址 = 元素索引 v+0x185c 的成员数组
// 折算形态；视图仅本 TU 使用）
struct CDataManager_UltimateArr_proxy
{
    char m_pad[0x6170];
    int m_arr[1];
};

// ============================================================================
// 小访问器（ORIG 弱符号，直接读反汇编实现）
// ============================================================================

#include "CBattle_Field/SetTournamentCurRound.inc"
{
    m_tournamentCurRound = round;
}

#include "CBattle_Field/SetParty.inc"
{
    m_party = party;
}

#include "CBattle_Field/SetCurSeatNo.inc"
{
    m_curSeatNo = seatNo;
}

#include "CBattle_Field/GetMapInfoKeyValue.inc"
{
    return x * 100 + y;
}

#include "CBattle_Field/GetCurrentMapInfo.inc"
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    return GetMapInfoFromPos(x, y, false);
}

// [b28d-solo-CBattle_Field] 已修 AE（178=178；旧判「N95 槽基差族」作废）：
// ORIG 第二次 key 落**独立槽 -0x204**（首算 key 在 -0x1fc）⇒ P0 槽-局部
// 双向对应律：原码是新的具名局部，不是复用 `key`。改成 `int insertKey = ...`
// 后 4.4 逐条复现（info 槽 -0x2f0、key -0x204 同时归位；帧 sub $0x310 不变）。
// m_layeredMap bool 化后 0x0830cd42 段 setne 伪差已消（DIFF→NEAR→AE）。
#include "CBattle_Field/GetMapInfoFromPos.inc"
{
    int key = GetMapInfoKeyValue(x, y);
    std::map<int, MapInfo>::iterator it = m_mapInfo.find(key);
    if (it != m_mapInfo.end())
    {
        if (create)
        {
            return &it->second;
        }
        if (m_gridMaze != 0)
        {
            const GridScript* grid = &m_gridMaze->getGridR(x, y);
            if (grid != 0 && grid->m_layeredMapIndexes.size() != 0)
            {
                if (getCurrentLayeredMapInfo()->m_layeredMap)
                {
                    return getCurrentLayeredMapInfo();
                }
            }
        }
        return &it->second;
    }

    MapInfo info;
    // [b28d-sweep-CBattle_Field] P0：ORIG 第二次 key 落独立槽 -0x204（首算
    // 的 key 在 -0x1fc）⇒ 原码是新的具名局部（复用 key 时 4.4 恒复用 -0x1fc）。
    int insertKey = GetMapInfoKeyValue(x, y);
    m_mapInfo.insert(std::make_pair(insertKey, info));
    it = m_mapInfo.find(insertKey);
    return &it->second;
}

#include "CBattle_Field/getMapWidth.inc"
{
    return m_mapWidth;
}

#include "CBattle_Field/getMapHeight.inc"
{
    return m_mapHeight;
}

#include "CBattle_Field/check_start_point.inc"
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    if (m_startX == x && m_startY == y)
    {
        return true;
    }
    return false;
}

#include "CBattle_Field/check_end_point.inc"
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    if (m_endX == x && m_endY == y)
    {
        return true;
    }
    return false;
}

#include "CBattle_Field/get_cur_map.inc"
{
    return GetCurrentMapInfo()->m_mapIndex;
}

#include "CBattle_Field/GetMapInfo.inc"
{
    return GetCurrentMapInfo();
}

#include "CBattle_Field/getMaze.inc"
{
    return m_gridMaze;
}

#include "CBattle_Field/GetCurrentMapIndex.inc"
{
    if (m_pMap != 0)
    {
        return m_pMap->get_index();
    }
    return 0;
}

#include "CBattle_Field/IsClearDungeon.inc"
{
    if (!m_clearCondition.Empty())
    {
        if (m_clearCondition.IsClearCondition())
        {
            return true;
        }
        return false;
    }
    return check_end_point();
}

#include "CBattle_Field/get_dungeon_index.inc"
{
    return m_dungeonIndex;
}

#include "CBattle_Field/get_dungeon_diff.inc"
{
    return m_dungeonDiff;
}

#include "CBattle_Field/GetDungeon.inc"
{
    return m_pDungeon;
}

#include "CBattle_Field/GetMonsterMap.inc"
{
    return &GetCurrentMapInfo()->m_monsterMap;
}

#include "CBattle_Field/get_map_index.inc"
{
    return GetMapInfoFromPos(x, y, false)->m_mapIndex;
}

#include "CBattle_Field/GetCurrentMap.inc"
{
    return m_pMap;
}

#include "CBattle_Field/ClearCondition.inc"
{
    if (!m_clearCondition.Empty())
    {
        if (m_clearCondition.ClearCondition(type, idx))
        {
            return true;
        }
    }
    return false;
}

#include "CBattle_Field/IsVisitedCurrentMap.inc"
{
    return GetCurrentMapInfo()->m_visited;
}

#include "CBattle_Field/setWorldMapTowerArea.inc"
{
    m_worldMapTowerArea = flag;
}

#include "CBattle_Field/isWorldMapTowerArea.inc"
{
    return m_worldMapTowerArea;
}

// [b28d-solo-CBattle_Field] N4/P30 封顶（28=28，唯 add-vs-lea + 索引寄存器落点）：
// ORIG 0x085bf364 `mov 0x114(%eax),%edx; add $0x2,%edx` vs 4.4 `mov mem,%eax;
// lea 0x2(%eax),%edx`。复核：D43 六形 + 本波字段类型全变体探针（/tmp/s18/p5.cpp
// int/uint/short/ushort/long/ulong 六型恒 lea）⇒ 4.4 ISel 恒 lea，语义等价。
#include "CBattle_Field/GetCurPosX.inc"
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        // ORIG 0x085bf364-0x373 索引段 5 条：`mov 0x114(%eax),%edx`（值直落
        // edx）+ `add $0x2,%edx` 原地自增。4.4 恒 `mov mem,%eax` 落点 +
        // `lea` 合成（实测 P168 值绑定 register asm("edx") 反产 mov%eax,%edx
        // +回读 6 条，负结论已回报），合法源码无法复现直落形——记 N81 附族
        // （4.1 索引 regmove/load 落点），行集仍 5 条。
        return m_curPos[m_curSeatNo].first;
    }
    else
    {
        return m_curX;
    }
}

// [b28d-solo-CBattle_Field] N4/P30 同 GetCurPosX（28=28，+2 索引段两处；ORIG 0x085bf3ba 区）。
#include "CBattle_Field/GetCurPosY.inc"
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        // 同上（ORIG 0x085bf3ba 区同构，N81 附族标注）。
        return m_curPos[m_curSeatNo].second;
    }
    else
    {
        return m_curY;
    }
}

// N81 附族：m_lastPos[m_curSeatNo] 索引段同上（add vs lea 直落，不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 封顶（49=49，+6 索引段两处）：ORIG
// `mov 0x114(%eax),%edx; add $0x6,%edx` vs 4.4 `mov mem,%eax; lea 0x6(%eax),%edx`
// （ORIG 0x0830eac8 段）。
#include "CBattle_Field/GetLastMapInfo.inc"
{
    int x = -1;
    int y = -1;
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        x = m_lastPos[m_curSeatNo].first;
        y = m_lastPos[m_curSeatNo].second;
    }
    else
    {
        x = m_lastX;
        y = m_lastY;
    }
    return GetMapInfoFromPos(x, y, false);
}

// N81 附族：m_curPos[m_curSeatNo] 索引段 `mov mem,%eax`+`lea` 合成 vs ORIG
// `mov mem,%edx`+`add` 直落（同 GetCurPosX/Y，4.1 索引 regmove 落点，合法源码
// 不可复现——负面结论同列）。
// [b28d-solo-CBattle_Field] N4/P30 同 GetCurPosX（51=51，+2 索引段两处；ORIG 0x08300da6）。
#include "CBattle_Field/getCurPosXY.inc"
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        x = m_curPos[m_curSeatNo].first;
        y = m_curPos[m_curSeatNo].second;
    }
    else
    {
        x = m_curX;
        y = m_curY;
    }
}

// N81 附族：m_lastPos[m_curSeatNo] 索引段同上（add vs lea 直落，不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 同族（47=47，+6 索引段两处；ORIG 0x08300e44）。
#include "CBattle_Field/getLastPosXY.inc"
{
    if (m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        x = m_lastPos[m_curSeatNo].first;
        y = m_lastPos[m_curSeatNo].second;
    }
    else
    {
        // ORIG 0x8300e44 原版笔误：两次均写 &x（mov 0xc(%ebp)），y 形参未写。
        x = m_lastX;
        x = m_lastY;
    }
}

// N81 附族：m_lastPos[...]/m_curPos[...] 索引段 + make_pair 传参同理（add vs lea 直落，
// 不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 同族（76=76，+6/+2 索引段 + make_pair<int&,int&> 段；ORIG 0x0830878e）。
#include "CBattle_Field/swap_curpos_xy.inc"
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        m_lastPos[m_curSeatNo] = m_curPos[m_curSeatNo];
        m_curPos[m_curSeatNo] = std::make_pair<int&, int&>(x, y);
    }
    else
    {
        m_lastX = m_curX;
        m_lastY = m_curY;
        m_curX = x;
        m_curY = y;
    }
}

// ============================================================================
// 小型访问器（读 ORIG 反汇编逐个实现）
// ============================================================================

#include "CBattle_Field/getBloodState.inc"
{
    return m_bloodState2;
}

#include "CBattle_Field/SetEnableHellDungeon.inc"
{
    bool* p = GetEnableHellDungeon();
    // p 槽取址使 4.4 store 前发纯装载 `mov -0xc(%ebp),%eax`（LHS 先，ORIG
    // 0x0822cf44 形）；无取址时 4.4 恒 RHS 值先（probe /tmp/l0p8.cpp SetH）。
    (void)&p;
    *p = flag;
}

#include "CBattle_Field/setUltimateDifficulty.inc"
{
    m_ultimateDifficulty = diff;
}

#include "CBattle_Field/getUltimateDifficylty.inc"
{
    return m_ultimateDifficulty;
}

#include "CBattle_Field/IsAssignNPCByPotion.inc"
{
    return GetCurrentMapInfo()->m_assignNPCByPotion;
}

#include "CBattle_Field/IsEnableHellDungeon.inc"
{
    bool* p = GetEnableHellDungeon();
    return *p;
}

// [b28d-solo-CBattle_Field] N97/P230 封顶（12/14）：4.4 char 形参恒 spill（`mov
// %al,-0x4`，ORIG 4.1 直读 `movzbl 0xc(%ebp),%edx`）+ if-conversion `cmova`
// （-march=i486 才消，本波不改路由表）+ N7 store 寄存器对调（ORIG 0x0830ed1e）。
#include "CBattle_Field/set_dungeon_diff.inc"
{
    // [b28d-D43] ORIG 0x0830ed21：`movzbl 0xc(%ebp),%edx; cmp $0x4,%edx; jbe`
    // + clamp 臂 `push $0x4; pop %edx`（值恒驻寄存器、无形参槽写回）⇒ 原码是
    // 三目 clamp 表达式（P35/P77：ORIG 无 spill 槽即表达式右值直用）；改写
    // `if(..) diff=4;` 会引入 movb $4,-0x4 写回槽（12→15）。
    // [b28d-solo-CBattle_Field] 实测三形（当前 14 条为最短）：
    // ①外层 cast 三目=14 条（cmova 形）；②无外层 cast=16 条（双 movzbl+ja 分支，
    // 探针 /tmp/s18/p2.cpp a/d 与 TU 实测一致）；③if 写回形=15 条。
    // 残余不可消：4.4 对 char 形参恒 spill（`mov %al,-0x4`，ORIG 4.1 直读
    // `movzbl 0xc(%ebp),%edx`）+ cmov（P230：需 -march=i486，本波不改路由表）
    // + N7 store 寄存器对调 ⇒ N97/P230 封顶。
    // [M3FreeNext15B.M3S6AucAvBattle 2026-09-11] 本波再证（ORIG 0x0830ed1e，
    // 兜底反汇编 12 条：movzbl 0xc(%ebp),%edx / cmp $4,%edx / jbe / push $4;pop %edx /
    // nop / mov %edx,0x1a0(%eax)）：探针 /tmp/p_setdd/{probe,probe2}.cpp 共 26 形
    // （含新形 ②`(diff & 0xff)` 32 位比较域=14 条、`*(unsigned char*)&diff`=18 条、
    // int 域比较 16 条、<= 反向 16 条）实测最短仍 14 条 ⇒ 14 为 4.4 形下限，
    // 封顶维持。
    m_dungeonDiff =
        (unsigned char)((unsigned char)diff > 4 ? 4 : (unsigned char)diff);
}

#include "CBattle_Field/setBloodState.inc"
{
    if (state <= m_bloodState2)
        return false;
    m_bloodState2 = state;
    return true;
}

#include "CBattle_Field/GetTournamentCurRound.inc"
{
    return m_tournamentCurRound;
}

// [E05 R2] P321 T3 消费端判据：bool（ORIG 0x08307656 `movzbl; ret` + 调用点直判）。
#include "CBattle_Field/IsFirstEnterDungeon.inc"
{
    return m_firstEnterDungeon;
}

#include "CBattle_Field/ResetFirstEnterDungeon.inc"
{
    m_firstEnterDungeon = 0;
}

#include "CBattle_Field/getLayeredMapIndex.inc"
{
    return m_layeredMapIndex;
}

#include "CBattle_Field/setLayeredMapIndex.inc"
{
    m_layeredMapIndex = m_layeredMapIndex + 1;
}

#include "CBattle_Field/getLayeredMapKeyValue.inc"
{
    return idx * 100 + 10000;
}

#include "CBattle_Field/getLayeredMapInfoIndex.inc"
{
    int key = getLayeredMapKeyValue(idx);
    std::map<int, MapInfo>::iterator it = m_mapInfo.find(key);
    if (it != m_mapInfo.end())
    {
        return &it->second;
    }

    MapInfo info;
    int key2 = getLayeredMapKeyValue(idx);
    m_mapInfo.insert(std::make_pair(key2, info));
    it = m_mapInfo.find(key2);
    return &it->second;
}

#include "CBattle_Field/getCurrentLayeredMapInfo.inc"
{
    return getLayeredMapInfoIndex(getLayeredMapIndex());
}

#include "CBattle_Field/IsKilledAllHellGruoups.inc"
{
    return m_killedAllHellGroups;
}

#include "CBattle_Field/SetRidableObjectState.inc"
{
    return m_ridable.SetRidableObjectState(idx, state);
}

#include "CBattle_Field/CheckAppearRidableObject.inc"
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    m_ridable.CheckAppearRidableObject(m_party, x, y, packet);
}

// CBattle_Field::GetFieldMob —— ORIG 0x0830a708（0x10a 字节）
//   报告：docs/class_func_reports/CBattle_Field/GetFieldMob.md
//   迭代器先默认构造（P28：it = find(...) 经临时中转拷贝）；两次独立
//   GetCurrentMapInfo()（P48 无 CSE）；找到则经块作用域临时 map_monster
//   拷贝给 out（tmp 析构先于 return true，对齐 ORIG 的 mov $1 在 dtor 后）。
//   【b28 封顶】指令数 72=72、帧 sub $0x380 全等；残差为局部槽基深差：4.1 把
//   迭代器/临时对象顶到帧底（-0x35c/-0x36c/-0x358…），4.4 恒自 ebp-8 起
//   （-0x10/-0xc/-0x18…）⇒ 全帧槽号平移 0x34c（N95/N82 族，P182 不强凑；
//   double framePad 仅调 ±4 无法位移 0x34c）。语义闭合，N 封顶勿再耗轮。
// [b28d-solo-CBattle_Field] NEAR/N82附 槽分区封顶（72=72，帧 sub $0x380 全等）：
// 4.4 聚合对象优先低槽（map_monster 临时 -0x35c）+标量分区（it/end -0x10/-0xc）
// vs 4.1 生命周期穿插（-0x354/-0x35c/-0x358）；P190 double framePad 仅 ±4
// 无法位移 0x34c（ORIG 0x0830a708）。
// [M3FreeNext15B.M3S6AucAvBattle 2026-09-11] 新钥匙复验（逐条）：
// ①本波实测槽表（我方 /tmp/tu_CBattle_Field.o fresh）：it=-0x10、end=-0xc、
//   tmp=[-0x35c,-0x10)（0x34c 字节）、find sret 临时=-0x36c；ORIG：it=-0x35c、
//   end=-0x358、tmp=[-0x354,-0x8)、find=-0x36c ⇒ **find 临时双方恒等**，差额是
//   「具名 it/end 与 tmp 的浅/深端归属相反」（非整体平移；旧「平移 0x34c」表述
//   由此精确化）。
// ②P339（全头部声明+深→浅序）对 tmp **不可用**：map_monster 为非 POD 类对象，
//   P331/P339 明文禁提前（ctor 发射点必须不动）。
// ③实测「tmp 去掉内层块作用域改函数级」（零指令代价形，析构点仍早于 return true）
//   ⇒ our 仍 72 但 verdict 由 NEAR 变 DIFF（TU 四数 near 1→0 / diff 3→4，劣化）
//   ⇒ 已 apply_patch 回滚。P326 不适用（本件无迭代器对/循环）。
// ④唯一能让 4.4 把 it/end 顶到深端的形是「tmp 先分配」（即函数头默认构造 + 块内
//   赋值），代价为多一条默认 ctor call（72→73+）并破坏与 ORIG 的 ctor 序列一致，
//   净负 ⇒ 不试。结论：N95/N82附 仍成立，N 封顶。
#include "CBattle_Field/GetFieldMob.inc"
{
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
        return false;
    {
        map_monster tmp(it->second);
        out = tmp;
    }
    return true;
}

#include "CBattle_Field/getBloodMaxRound.inc"
{
    if (m_pDungeon != 0)
    {
        return ((CDungeon_8a0_proxy*)m_pDungeon)->m_8a0;
    }
    return 0;
}

// CBattle_Field::BloodRoundMaxTimePacket —— ORIG 0x08305dc4（0xd8 字节）
//   报告：docs/class_func_reports/CBattle_Field/BloodRoundMaxTimePacket.md
//   header 0xd6 = 大轮+1、最大时间、秒数；经 m_party 广播。
#include "CBattle_Field/BloodRoundMaxTimePacket.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xd6);
    packet.put_byte(0);
    packet.put_byte((int)round + 1);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// CBattle_Field::getNextRoundRandomDifficulty —— ORIG 0x08306156（0x5f 字节）
//   报告：docs/class_func_reports/CBattle_Field/getNextRoundRandomDifficulty.md
//   大轮序==最大轮-1 → 1；否则掷 100，<=49 → 2，否则 1。
#include "CBattle_Field/getNextRoundRandomDifficulty.inc"
{
    if ((short)m_bloodRound.getBloodRoundOrder() == getBloodMaxRound() - 1)
    {
        return 1;
    }
    else
    {
        int r = get_rand_int(100);
        if (r > 49)
            return 1;
        return 2;
    }
}

// ============================================================================
// 嵌套类型方法（ORIG 弱符号）
// ============================================================================

#include "CBattle_Field/TournamentInfo.inc"
{
    Clear();
}

#include "CBattle_Field/Clear.inc"
{
    m_roundOrder = -1;
    for (int i = 0; i <= 1; ++i)
    {
        m_monster[i].Clear();
    }
    m_group = 0;
}

bool CBattle_Field::TournamentInfo::operator<(const TournamentInfo& other) const
{
    if (m_group < other.m_group)
        return true;
    return false;
}

#include "CBattle_Field/setBloodRoundOrder.inc"
{
    m_roundOrder = m_preMapRoundOrder;
}

#include "CBattle_Field/setPreMapBloodRoundOrder.inc"
{
    m_preMapRoundOrder = m_roundOrder;
}

#include "CBattle_Field/incBloodRoundOrder.inc"
{
    ++m_roundOrder;
}

#include "CBattle_Field/getBloodRoundOrder.inc"
{
    return m_roundOrder;
}

#include "CBattle_Field/setBloodSmallRoundOrder.inc"
{
    m_smallRoundOrder = v;
}

#include "CBattle_Field/incBloodSmallRoundOrder.inc"
{
    ++m_smallRoundOrder;
}

#include "CBattle_Field/getBloodSmallRoundOrder.inc"
{
    return m_smallRoundOrder;
}

#include "CBattle_Field/setBloodLastSmallRoundOrder.inc"
{
    m_lastSmallRoundOrder = v;
}

#include "CBattle_Field/getBloodLastSmallRoundOrder.inc"
{
    return m_lastSmallRoundOrder;
}

#include "CBattle_Field/reset_blood_values.inc"
{
    m_lastSmallRoundOrder = 0;
    m_roundOrder = 0;
    m_smallRoundOrder = 0;
}

// CBattle_Field::reset_blood_values @ 0x08306d86
// 清空血条分配向量、重生计数/时间数组并复位血条状态字段。
#include "CBattle_Field/reset_blood_values_1.inc"
{
    m_bloodSystems.clear();
    m_bloodPhaseTimes.clear();
    m_bloodPhaseOrder.clear();
    m_bloodPhaseClear.clear();
    memset(m_bloodSpawnCnt, 0, sizeof(m_bloodSpawnCnt));
    memset(m_bloodSpawnTime, 0, sizeof(m_bloodSpawnTime));
    m_bloodCrazyRound = -1;
    m_bloodSpawnCntCur = 0;
    m_bloodCrazyType = 0;
}

#include "CBattle_Field/stRewardItem_t.inc"
{
}

#include "CBattle_Field/reset.inc"
{
    m_count = 0;
    for (int i = 0; i <= 9; ++i)
    {
        m_item[i].reset();
        m_item[i].m_addInfo = -1;
    }
}

#include "CBattle_Field/CBloodClearRewardData.inc"
{
}

#include "CBattle_Field/reset_1.inc"
{
    m_bloodRound = 0;
    m_playTime = 0;
    m_prevPlayTime = 0;
    m_isBloodClear = 0;
    for (int i = 0; i <= 3; ++i)
    {
        m_rewardExp[i] = 0;
        m_reward[i].reset();
    }
    m_ultimateClearPoint = 0;
}

#include "CBattle_Field/onStartBloodRound.inc"
{
    m_startTick = GlobalData::s_systemTime_.getCurTickCount();
}

#include "CBattle_Field/addPlayTime.inc"
{
    m_prevPlayTime = m_playTime;
    m_playTime = m_playTime + t;
}

#include "CBattle_Field/getPlayTime.inc"
{
    return m_playTime;
}

#include "CBattle_Field/getBloodRoundClearTime.inc"
{
    return m_endTick - m_startTick;
}

#include "CBattle_Field/getCurrentBloodRound.inc"
{
    return m_bloodRound;
}

#include "CBattle_Field/setCurrentBloodRound.inc"
{
    m_bloodRound = v;
}

#include "CBattle_Field/getRewardItemCount.inc"
{
    return m_reward[idx].m_count;
}

#include "CBattle_Field/getRewardItem.inc"
{
    return &m_reward[idx].m_item[0];
}

#include "CBattle_Field/getRewardExp.inc"
{
    return m_rewardExp[idx];
}

// b28 P211 翻案（DIFF 6→IDENTICAL）：字段 int→unsigned int 与 exp 形参同型化
// 后 store 展开翻为 LHS-first（this 于 idx 之后即载，`lea 0x4(%edx),%ecx` 并入
// scaled 寻址），与 ORIG 0x085bf144 逐条全等；旧「add vs lea」判词系字段异型
// 所致的装载方向差，非 ISel 伪差。
#include "CBattle_Field/setRewardExp.inc"
{
    m_rewardExp[idx] = exp;
}

#include "CBattle_Field/setRewardItem.inc"
{
    m_reward[idx].m_count = itemIdx;
}

#include "CBattle_Field/setBloodClear.inc"
{
    m_isBloodClear = v;
}

#include "CBattle_Field/isBloodClear.inc"
{
    return m_isBloodClear;
}

#include "CBattle_Field/getUltimateRoundClearPoint.inc"
{
    return m_ultimateClearPoint;
}

#include "CBattle_Field/incUltimateRoundClearPoint.inc"
{
    // ORIG 08307096：m_9c8 += dm_intTable[v+0x185c]；视图 m_arr[v] 折算同址。
    m_ultimateClearPoint =
        m_ultimateClearPoint
        + ((CDataManager_UltimateArr_proxy*)G_CDataManager())->m_arr[v];
}

#include "CBattle_Field/CTowerOfDespairRewardData.inc"
{
}

#include "CBattle_Field/dtor_CTowerOfDespairRewardData.inc"
{
}

#include "CBattle_Field/reset_tod.inc"
{
    m_todMap.clear();
}

#include "CBattle_Field/stConditionEnterBossRoomStatus_t.inc"
{
}

#include "CBattle_Field/stConditionEnterBossRoomStatus_t_1.inc"
    const stConditionEnterBossRoomStatus_t& other)
    : m_monsterIdx(other.m_monsterIdx),
      m_pos(other.m_pos),
      m_curKillCount(other.m_curKillCount),
      m_maxKillCount(other.m_maxKillCount)
{
}

#include "CBattle_Field/dtor_stConditionEnterBossRoomStatus_t.inc"
{
}

CBattle_Field::stConditionEnterBossRoomStatus_t&
CBattle_Field::stConditionEnterBossRoomStatus_t::operator=(
    const stConditionEnterBossRoomStatus_t& other)
{
    m_monsterIdx = other.m_monsterIdx;
    m_pos = other.m_pos;
    m_curKillCount = other.m_curKillCount;
    m_maxKillCount = other.m_maxKillCount;
    return *this;
}

// [b28d-Main 代落·S324 P299/P317] 用户 ctor/dtor 定义删除（声明端同步）：
// 隐式 ctor 由使用点发射 W（含 m_pos 的 pair 默认构造），隐式 trivial dtor 的
// D1/D2 由 new_allocator::destroy 发射 W——与 ORIG 0830ee4c / 08316adc 同形。

// ============================================================================
// 外部依赖（CGameManager.h 与本头组合存在重定义冲突，按 ORIG 符号做最小声明，
// 链接解析到真实定义 —— 与 QuickParty.cpp 同一模式）
// ============================================================================
class CGameManager
{
public:
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
};
CGameManager* G_CGameManager();

// WongWork::CDeathTower 最小完成类型（CUser.h 仅前置声明；非虚方法，仅用 this）。
namespace WongWork
{
class CDeathTower
{
public:
    int genTimerKey(TIMER_MESSAGE msg);   // ORIG 0x8142c46 域内（见 CGameManager.h 声明）
    int getIdx();
};
}

// ============================================================================
// CBattle_Field::getRandomBuffDungeonType —— ORIG 0x0830aaa4（0x1d5 字节）
//   报告：docs/class_func_reports/CBattle_Field/getRandomBuffDungeonType.md
// ============================================================================
// [b28d-solo-CBattle_Field] 已修 AE（138=138，call 目标/槽号/分支全等）：
// ①P48/P13 两次独立 G_CGameManager 链调用 + end 具名缓存；②const 引用视图
// 取 const begin/end（P96/P270，消转换构造）；③P0/P48 两次独立
// get_blood_dungeon_type（去具名 int blood）；④(signed char) cast 复现
// `setg`（ORIG 0x0830aaa4）。
#include "CBattle_Field/getRandomBuffDungeonType.inc"
{
    if (dungeonIdx == 0)
    {
        if (m_pDungeon == 0)
            return 7;
        dungeonIdx = m_pDungeon->get_index();
    }

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return 7;

    // quick party 奖励副本表命中 → 类型 7
    // [b28d-solo-CBattle_Field] P48/P13：ORIG 0x0830ab02/0x0830ab29 两次独立
    // G_CGameManager 链调用（无 CSE），end 一次求值后具名缓存（循环条件只调
    // operator!=）；旧引用缓存 idxList 多一条槽 store 并使帧 0x38→0x28。
    // P96/P270：ORIG 调 `_ZNKSt6vectorIiSaIiEE5beginEv/endEv`（const 版）且
    // sret 直落 it/end 槽 ⇒ 对 const 引用视图取 begin/end（const_iterator 同型，
    // 无 __normal_iterator 转换 C1；非 const 版 begin 会多 2 次转换构造）。
    std::vector<int>::const_iterator it =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list()).begin();
    std::vector<int>::const_iterator end =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list()).end();
    for (; it != end; ++it)
    {
        if (*it == dungeonIdx)
            return 7;
    }

    // [b28d-solo-CBattle_Field] ORIG 0x0830ab9a `test %al,%al; setg %al`（有符号
    // 判 >0）⇒ 源码须写 (signed char) cast（探针 /tmp/s18/p3.cpp t4 逐条命中；
    // 裸 unsigned char > 0 在 4.4 恒折 setne）。
    if ((signed char)dungeon->get_dimension_possible() > 0)
        return 2;
    if (dungeon->m_89c != 0)                       // +0x89c 直读（ORIG movzbl 0x89c）
        return 1;

    // [b28d-solo-CBattle_Field] P0/P48：ORIG 0x0830abd0/0x0830abee 两次独立调用
    // get_blood_dungeon_type（无具名 int 槽，直接 cmp $1/$2,%al）。
    if (dungeon->get_blood_dungeon_type() == 1)
        return 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        return 5;

    // kind==1 且塔内堆叠物品限制非 0 → 3；kind==1 且限制==0 → 6；否则 0。
    // （ORIG 分支顺序如此：!=0 → 3 先判，再判 !1 → 6。）
    if (dungeon->getDungeonKind() == 1 && dungeon->limitOfStackableItemInTower() != 0)
        return 3;
    if (dungeon->getDungeonKind() == 1 && dungeon->limitOfStackableItemInTower() != 1)
        return 6;
    return 0;
}

// ============================================================================
// CBattle_Field::setEplpWaitingTimer —— ORIG 0x0830ac7a（0x17b 字节）
//   报告：docs/class_func_reports/CBattle_Field/setEplpWaitingTimer.md
//   type 3/6 → 绝望之塔 timer(0x4d)；4/5 → party timer(0x25)；其余 → party
//   timer(0x15)。秒数均 0x1e。
//   （N 封顶：ndiff 93 残差为分支判定调度（cmp $4,jge vs $3,jle）+ 三段
//   InsertTimer 链 esi/ebx 跨调用保存与帧差异，源码已按 ORIG 比较边界写 >=4，
//   GCC 仍规范化，无法逐字节复现——见报告）
// ============================================================================
// [b28d-solo-CBattle_Field] ORIG 0x0830ac7a：帧 sub $0x30（push esi/ebx）已对齐；
// key 直落 %esi（无 ebx 中转）⇒ 去 static_cast（P41 变体）与形参 register（P248）。
#include "CBattle_Field/setEplpWaitingTimer.inc"
{
    if (m_party == 0)
        return;

    // [b28d-solo-CBattle_Field] P46/P261：ORIG 0x0830ac92 决策链为
    // `cmp $5;jg / cmp $4;jge / cmp $3;je / jmp / cmp $6;jne`（无状态槽的
    // 寄存器 cmp 链）⇒ 原码是 switch(type)，if-else 链在 4.4 恒产
    // `cmp $3;jle` + 内存直比（探针 /tmp/s18/p4.cpp f3 逐条命中决策链）。
    // 注：RandomBuffDungeon 占位枚举仅 NONE=0，4.4 -fstrict-enums 会把
    // case 3..6 判为不可达并整体删除 ⇒ 以 (int) 视图 switch（不改共享头）。
    switch ((int)type)
    {
    case 3:
    case 6:
    {
        WongWork::CDeathTower* tower = m_party->getManager()->getDeathTower();
        if (tower == 0)
            return;
        register int key = tower->genTimerKey((TIMER_MESSAGE)0x4d);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)6, tower->getIdx(),
                                    (TIMER_MESSAGE)0x4d, 0x1e, key, 0x4d);
        return;
    }
    case 4:
    case 5:
    {
        // P0/P24：ORIG 0x0830ad31 段 idx 直落 %ebx（无具名槽）
        // ⇒ GetPartyIndex() 内联进 InsertTimer 实参。
        register int key = m_party->gen_timer_key((TIMER_MESSAGE)0x25);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    m_party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x25, 0x1e, key, 0x25);
        return;
    }
    default:
        break;
    }
    register int key = m_party->gen_timer_key((TIMER_MESSAGE)0x15);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x15, 0x1e, key, 0x15);
}
