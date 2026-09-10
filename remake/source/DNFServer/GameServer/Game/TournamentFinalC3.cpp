// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TournamentFinalC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h

// ---------------------------------------------------------------------------
// CBattle_Field::InsertTournamentInfo @ 0x08303e02
// 报告要点：并行迭代 infos/indexes —— 循环条件先判 infoIt!=infos.end() 再判
// idxIt!=indexes.end()（0x08303f0c..0x08303f5b），任一到尾即退出；循环体把
// *idxIt 截断为 short 写入元素首字段 m_roundOrder(+0x00)（0x08303e50 mov %ax,
// (%ebx)），随后以该字段自身作键 make_pair(m_roundOrder, *infoIt)（ORIG 两实参
// 均指向同一元素：operator->/operator* 结果一致）经 pair<short,TournamentInfo>
// → pair<const short,TournamentInfo> 转换构造后 map::insert（结果对 -0x48，
// 丢弃），最后 infos.erase/infoIt、indexes.erase/idxIt 双向推进 —— 迭代器仅靠
// erase 返回值前进（0x08303eb0..0x08303ef4），无 ++it、无空表防护 —— 忠实保留。
#include "TournamentFinalC3/InsertTournamentInfo.inc"
    std::vector<TournamentInfo>& infos,
    std::map<short, TournamentInfo>& roundMap)
{
    std::vector<TournamentInfo>::iterator infoIt = infos.begin();
    std::vector<int>::iterator idxIt = indexes.begin();

    while (infoIt != infos.end() && idxIt != indexes.end())
    {
        infoIt->m_roundOrder = (short)*idxIt;
        roundMap.insert(std::make_pair(infoIt->m_roundOrder, *infoIt));

        infoIt = infos.erase(infoIt);
        idxIt = indexes.erase(idxIt);
    }
}

// ---------------------------------------------------------------------------
// CBattle_Field::MakeRoundInfo @ 0x08303f66
// 报告要点：round 从 1 计到 4（0x083040d7 cmp $0x4 / setle，含 4）。每轮：
// map::begin 后构造空 snapshot vector（0x08303f7a..0x08303f95），内层 while
// （每轮重取 map::end，0x08303fd6）将全部 second（pair+0x04）push_back；
// 迭代器用后置 it++（ORIG 0x083113d8 三参 operator++(int)，含丢弃的临时拷贝）；
// 插入 roundInfoMap（返回值丢弃），~pair×2 后调 MakeNextRoundInfo(round,
// roundMap) 推进对阵（0x0830410c），最后 ~vector；循环外调
// MakeTournamentMonsterList(roundInfoMap, mobList)（0x08304594）。
// roundMap 不预先 clear —— 忠实保留。
#include "TournamentFinalC3/MakeRoundInfo.inc"
    std::map<short, std::vector<TournamentInfo> >& roundInfoMap,
    std::list<_mapMonster>& mobList)
{
    for (int round = 1; round <= 4; ++round)
    {
        std::map<short, TournamentInfo>::iterator it = roundMap.begin();
        std::vector<TournamentInfo> snapshot;
        while (it != roundMap.end())
        {
            snapshot.push_back(it->second);
            it++;
        }

        roundInfoMap.insert(std::make_pair(round, snapshot));
        MakeNextRoundInfo(round, roundMap);
    }

    MakeTournamentMonsterList(roundInfoMap, mobList);
}

// ---------------------------------------------------------------------------
// CBattle_Field::MakeNextRoundInfo @ 0x0830410c
// 报告要点：函数顶部构造局部 nextMap（0x0831116e）；循环 i=0 起、步进 +2，
// 条件每轮重算 0x10/memberCount-1（0x08304528 idivl 有符号除法，memberCount
// 为 0 时 ORIG 直接除零 —— 无防护，忠实保留）。每对 a=i/b=i+1：先取
// roundMap[(short)a].m_group(+0x14)、roundMap[(short)b].m_group；若
// roundMap[(short)a].m_monster[0].m_mobId(+0x04)==0 则 a 晋级，否则判 b；
// 都不空时 rndA=0;rndB=0 初始化后 get_rand_int(groupA)/get_rand_int(groupB)，
// rndA>rndB 时 a 晋级否则 b。晋级写 roundMap[key].m_roundOrder=(short)(key/2)
// 后以三次 operator[] 形态 insert 进 nextMap：
// insert(make_pair(roundMap[key].m_roundOrder, roundMap[key]))（返回值丢弃）。
// 循环外 roundMap = nextMap（copy-assign 0x083115be）→ ~nextMap。
#include "TournamentFinalC3/MakeNextRoundInfo.inc"
    std::map<short, TournamentInfo>& roundMap)
{
    std::map<short, TournamentInfo> nextMap;
    for (int i = 0; i < 0x10 / memberCount - 1; i += 2)
    {
        int a = i;
        int b = i + 1;
        int groupA = roundMap[(short)a].m_group;
        int groupB = roundMap[(short)b].m_group;

        if (roundMap[(short)a].m_monster[0].m_mobId == 0)
        {
            roundMap[(short)a].m_roundOrder = (short)(a / 2);
            nextMap.insert(std::make_pair(
                roundMap[(short)a].m_roundOrder, roundMap[(short)a]));
        }
        else if (roundMap[(short)b].m_monster[0].m_mobId == 0)
        {
            roundMap[(short)b].m_roundOrder = (short)(b / 2);
            nextMap.insert(std::make_pair(
                roundMap[(short)b].m_roundOrder, roundMap[(short)b]));
        }
        else
        {
            int rndA = 0;
            int rndB = 0;
            rndA = get_rand_int(groupA);
            rndB = get_rand_int(groupB);
            if (rndA > rndB)
            {
                roundMap[(short)a].m_roundOrder = (short)(a / 2);
                nextMap.insert(std::make_pair(
                    roundMap[(short)a].m_roundOrder, roundMap[(short)a]));
            }
            else
            {
                roundMap[(short)b].m_roundOrder = (short)(b / 2);
                nextMap.insert(std::make_pair(
                    roundMap[(short)b].m_roundOrder, roundMap[(short)b]));
            }
        }
    }
    roundMap = nextMap;
}

// ---------------------------------------------------------------------------
// CBattle_Field::AddTournamentMonster @ 0x08304718
// 报告要点：i<=1 循环（cmpl $1/setle）；每次迭代先构造 _mapMonster（ctor
// 0830e4fc）再判 info.m_monster[i].m_mobId!=0（param+i*8+4），命中按 ORIG
// 写入顺序填字段：flag04(字节)=1 → field08=0 → levelAdd=1 → field14=1 →
// spawnType=0 → roleType 以 if/else 双分支直接存储（isUltimate?3:0）→ champion(字节)=0 → posX=0 →
// orderIdx=0 → hpPct=100 → flag30(字节)=0（注意：ORIG 不写 m_level(+0x24)，
// 保持 ctor 默认），push_back 后析构局部 —— 空槽同样走 ctor/dtor 对。
#include "TournamentFinalC3/AddTournamentMonster.inc"
    std::list<_mapMonster>& mobList, bool isUltimate)
{
    for (int i = 0; i <= 1; ++i)
    {
        _mapMonster mob;
        if (info.m_monster[i].m_mobId != 0)
        {
            mob.m_mobId = info.m_monster[i].m_mobId;
            mob.m_flag04 = 1;
            mob.m_field08 = 0;
            mob.m_levelAdd = 1;
            mob.m_field14 = 1;
            mob.m_spawnType = 0;
            if (isUltimate)
                mob.m_roleType = 3;
            else
                mob.m_roleType = 0;
            mob.m_champion = 0;
            mob.m_posX = 0;
            mob.m_orderIdx = 0;
            mob.m_hpPct = 100;
            mob.m_flag30 = 0;
            mobList.push_back(mob);
        }
    }
}

// ---------------------------------------------------------------------------
// CBattle_Field::MakeTournamentMonsterList @ 0x08304594
// 报告要点：round=1..4（cmpl $4/setle）；每轮 vec=roundMap[(short)round]
// （operator[] 083115dc）；peer 初值 -1，内层 i=0..15 找首个
// m_monster[0].m_mobId==0 的下标，偶数 peer=i+1 否则 peer=i-1 后 break；
// 未命中保持 peer=-1 仍无条件 AddTournamentMonster(vec[peer],...)（负下标
// UB —— ORIG 无防护，忠实保留）；round==4 传 true 否则 false。
// 循环后遍历 mobList 重编号 it->m_orderIdx=order++（+0x2c，list 迭代器
// 后置 it++ 推进）。
#include "TournamentFinalC3/MakeTournamentMonsterList.inc"
    std::map<short, std::vector<TournamentInfo> >& roundInfoMap,
    std::list<_mapMonster>& mobList)
{
    for (int round = 1; round <= 4; ++round)
    {
        const std::vector<TournamentInfo>& vec = roundInfoMap[(short)round];   // ORIG const operator[]
        int peer = -1;
        for (int i = 0; i < 0x10; ++i)
        {
            if (vec[i].m_monster[0].m_mobId == 0)
            {
                if ((i & 1) == 0)
                    peer = i + 1;
                else
                    peer = i - 1;
                break;
            }
        }

        if (round == 4)
            AddTournamentMonster(vec[peer], mobList, true);
        else
            AddTournamentMonster(vec[peer], mobList, false);
    }

    std::list<_mapMonster>::iterator it = mobList.begin();
    int order = 0;
    while (it != mobList.end())
    {
        it->m_orderIdx = order++;
        it++;
    }
}
