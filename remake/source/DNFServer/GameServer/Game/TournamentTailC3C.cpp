// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TournamentTailC3C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <list>
#include <map>
#include <vector>

#include "CDataManager.h"
#include "DataManagerInitDepends.h"  // stHellParty_t/stHellPartyMonsterGroupList（须在 CDataManager.h 之后）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "GameRand.h"                // get_rand_int
#include "HellPartyGroupOrder.h"

// CDataManager.cpp 单例（ORIG 0x089097c4），返回静态缓冲区指针。
extern int* Get_instance_stHellParty_t();

// ---------------------------------------------------------------------------
// CBattle_Field::MakeBasicRoundInfo @ 0x08303896
// 报告要点：random_shuffle(monsters)；begin()==end() 直接返回 false；
// while(round<=14 && it!=end)：每轮构造 TournamentInfo，内层 for(j<memberCount)
// 复制 *it 到 m_monster[j] 并 m_group += it->m_rate(+4)、++it，push_back 后 ++round；
// 退出时返回 round==15。ORIG 内层循环不做 end 检查、无 memberCount 边界检查，
// roundInfo 不预先 clear —— 全部忠实保留。
#include "TournamentTailC3C/MakeBasicRoundInfo.inc"
    std::vector<TournamentMonster>& monsters,
    std::vector<TournamentInfo>& roundInfo)
{
    std::random_shuffle(monsters.begin(), monsters.end());
    std::vector<TournamentMonster>::iterator it = monsters.begin();
    if (it == monsters.end())
        return false;

    int round = 0;
    for (round = 0; round <= 14 && it != monsters.end();)
    {
        TournamentInfo info;
        for (int j = 0; j < memberCount; ++j)
        {
            info.m_monster[j] = *it;
            info.m_group += it->m_rate;
            it++;
        }
        roundInfo.push_back(info);
        ++round;
    }
    // ORIG 尾块：cmpl $0xf; je →mov$1 岛；false 臂原地 fall-through（P47 镜像）。
    if (round != 15)
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// CBattle_Field::ChoiceFirstGroup @ 0x08303a1c
// 报告要点：先取 src 首元素 m_group 作基准（无条件解引用 begin()）；随后
// while(it!=end && it->m_group==group)：dst.push_back(*it)、it=src.erase(it)。
// ORIG 不清空 dst、无空表防护 —— 忠实保留。
#include "TournamentTailC3C/ChoiceFirstGroup.inc"
    std::vector<TournamentInfo>& dst)
{
    std::vector<TournamentInfo>::iterator it = src.begin();
    const int group = it->m_group;
    // ORIG 条件序：先 it==end 测试（sete+test/je 退），后 m_group==group
    // 直分支 —— 拆嵌套消 4.4 对 && 右臂的 choose 物化。
    while (it != src.end())
    {
        if (it->m_group == group)
        {
            dst.push_back(*it);
            it = src.erase(it);
        }
        else
            break;
    }
}

// ---------------------------------------------------------------------------
// CBattle_Field::ConsistTournamentRound @ 0x08303ace
// 报告要点：randIdx=get_rand_int(0x10)；先插入键 randIdx 的空 TournamentInfo；
// 偶数 peer=randIdx+1 否则 randIdx-1；shuffle(first) 后无条件改首元素
// m_roundOrder=peer 并插入副本再 erase(begin())；indexes 收集 [0,0x10) 中除
// randIdx/peer 外的下标并 shuffle；InsertTournamentInfo 先 first 后 second；
// 最后 MakeRoundInfo(tournament, roundMap, mobList)。ORIG 对空 first 无防护 —— 忠实保留。
#include "TournamentTailC3C/ConsistTournamentRound.inc"
    std::vector<TournamentInfo>& second,
    std::map<short, std::vector<TournamentInfo> >& roundMap,
    std::list<_mapMonster>& mobList)
{
    int randIdx = -1;
    randIdx = get_rand_int(0x10);

    std::map<short, TournamentInfo> tournament;

    TournamentInfo empty;
    empty.m_roundOrder = (short)randIdx;
    tournament.insert(std::make_pair(empty.m_roundOrder, empty));


    int peer = -1;
    if ((randIdx & 1) == 0)
        peer = randIdx + 1;
    else
        peer = randIdx - 1;

    std::random_shuffle(first.begin(), first.end());
    // [b28d-20] ORIG 0x08303bd2..0x08303c7c：begin() 仅调用一次（迭代器落 -0xd8 槽），
    // 后续 ptEv/deEv/ptEv/erase 全部复用该具名迭代器 —— 写 first.begin() 三次会多 3 组
    // begin()+sret 且槽号漂移（P0 槽往返 ⇒ 原码有具名 iterator it）。
    std::vector<TournamentInfo>::iterator it = first.begin();
    it->m_roundOrder = (short)peer;
    tournament.insert(std::make_pair(it->m_roundOrder, *it));
    first.erase(it);

    std::vector<int> indexes;
    for (int i = 0; i < 0x10; ++i)
    {
        // [b28d-20] ORIG 0x08303ca7/0x08303cb2 两个 je 落点不同（+0x200 / +0x203 两独立
        // nop 岛）⇒ P223 多岛细则形：两条正条件 goto + 尾标签串（标签序=守卫序）；
        // 合并 && 会被 4.4 jump-thread 成单落点。
        if (i == randIdx) goto L_skip1;
        if (i == peer) goto L_skip2;
        indexes.push_back(i);
    L_skip1:;
    L_skip2:;
    }
    std::random_shuffle(indexes.begin(), indexes.end());

    InsertTournamentInfo(indexes, first, tournament);
    InsertTournamentInfo(indexes, second, tournament);
    MakeRoundInfo(tournament, roundMap, mobList);
}

// ---------------------------------------------------------------------------
// Get_instance_groupOrder_map_pool @ 0x089e813f
// 报告要点：__cxa_guard 保护的函数局部静态 map<int,map<unsigned short,
// STHellPartyGroupOrder>>（hellPartygroupOrderMapPool_）+ __cxa_atexit 析构注册，
// 返回其地址 —— 与 C++ 函数局部静态语义一一对应。
HellPartyGroupOrderMapPool* Get_instance_groupOrder_map_pool()
{
    static HellPartyGroupOrderMapPool hellPartygroupOrderMapPool_;
    return &hellPartygroupOrderMapPool_;
}

// ---------------------------------------------------------------------------
// CDataManager::find_hellparty_group @ 0x08363716
// 报告要点：key=(unsigned short)idx；在 Get_instance_stHellParty_t() 单例
// +0x54 的 map<unsigned short,stHellPartyMonsterGroupList> 上 find；
// 未命中返回 NULL，命中返回 &it->second（pair 内偏移 +4，与 ORIG
// operator->+0x4 一致）。调用方（BattleMonsterTailC3A.cpp）将该指针视图化为
// stHellPartyMonsterGroupList* 使用。
#include "TournamentTailC3C/find_hellparty_group.inc"
{
    const unsigned short key = (unsigned short)idx;
    std::map<unsigned short, stHellPartyMonsterGroupList>::iterator it =
        reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())
            ->m_groupMap.find(key);
    if (it != reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())
                  ->m_groupMap.end())
        return reinterpret_cast<int*>(&it->second);
    return NULL;
}
