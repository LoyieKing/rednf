#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_RankMailDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CDeathTowerRanking.h"
#include "CMailBoxHelper.h"

#include <string.h>
#include <new>

#include "Inven_Item.h"          // CMail 成员 / placement ctor
#include "CDungeon.h"            // get_standard_level/getObjectDropItems/getSpecialPassiveObjectItems
#include "CDeathTower.h"         // WongWork::CDeathTower::CDungeonMgr

// ============================================================================
// A) WongWork::CDeathTowerRanking
// ============================================================================

namespace {

// （原 rankSlotBase 无类型字节寻址 helper 已删除——SecuLink 批次 2026-09-05
//  裸偏移清理：塔块 6×0x98 连续 stBestRecord_t 槽证据见 CDeathTowerRanking.h
//  stTowerBlock 注释；registRanker 内改用具类型 stride 寻址 + 具名字段。）

} // namespace

#include "W6_RankMailDeps/registRanker.inc"
    unsigned int towerIdx, unsigned int slotIdx,
    const struct stDeathTowerRecordMemberInfo_t* members,
    unsigned int stageVal, unsigned int playTime)
{
    // asm +0x00c..+0x03c: cmp $5/ja —— slotIdx>5 时跳过槽操作（落到 +0x21b）。
    if (slotIdx <= 5)
    {
        // 具类型 stride 寻址（asm +0x021..+0x032：imul $0x390(tower) +
        // imul $0x98(slot) 两步，塔块 = 6×stBestRecord_t 连续槽）。
        // [b28d-solo-W6_RankMailDeps] P183 去缓存：ORIG 每处访问重发完整
        //   `mov towerIdx; mov slotIdx; imul $0x98; imul $0x390; lea;
        //    add this` 链（无 slot 具名指针槽）；旧形缓存 slot 使帧 0xdc→0xec
        //   且整段寻址退化为 add+lea 两步。真二维下标（头 m_slots[6]，
        //   Main 代落后）直接产 ORIG 形（探针 /tmp/b28d_w6_probe3.cpp f_2d）。
#define W6_RANKER_SLOT(idx) (m_towers[towerIdx][idx])   // [b28d-Main·检查点14] 二维化
        // asm +0x064..+0x0a3: 槽已占位 → 先按旧 stage 行、旧时间值 erase 排名
        // 记录（读取发生在改写之前；ORIG 直传 &slot->m_playTime 引用，
        // 无 oldStage/oldTime 局部槽）。
        if (W6_RANKER_SLOT(slotIdx).m_flag90 != 0)
        {
            m_rankMap[towerIdx][W6_RANKER_SLOT(slotIdx).m_stage]
                .erase(W6_RANKER_SLOT(slotIdx).m_playTime);
        }
        // asm +0x0a8..+0x1a9: for i < m_tableType: strncpy(名字,0x1e) +
        // 三 byte lane ← members[i*0x17 + 0x14/15/16]。
        // members 按 ORIG 字节读（stDeathTowerRecordMemberInfo_t 未建模字段，
        // +0x14/15/16 三 byte 无具名证据，保留 stride+偏移形式）。
        for (unsigned int i = 0; i < m_tableType; ++i)
        {
            strncpy(W6_RANKER_SLOT(slotIdx).m_memberData[i],
                    members[i].m_name, 0x1e);
            // [b28d-solo-W6_RankMailDeps] 数组引用形（`uchar(*)[0x17]` 二维
            //   下标）复现 ORIG `movzbl 0x14(%eax)` disp 读形：探针
            //   /tmp/b28d_w6_probe4.cpp l2 实测 5 条 vs 指针下标 l1 的 7 条
            //   （P30/N4 族，残余 1 条 members 载入差）。
            W6_RANKER_SLOT(slotIdx).m_pad7c[0][i] =
                members[i].m_b14;
            W6_RANKER_SLOT(slotIdx).m_pad7c[1][i] =
                members[i].m_b15;
            W6_RANKER_SLOT(slotIdx).m_pad7c[2][i] =
                members[i].m_b16;
        }
        // asm +0x1af..+0x21b: 写新 stageVal/playTime 并置“有纪录”位=1。
        W6_RANKER_SLOT(slotIdx).m_stage = stageVal;
        W6_RANKER_SLOT(slotIdx).m_playTime = playTime;
        W6_RANKER_SLOT(slotIdx).m_flag90 = 1;
#undef W6_RANKER_SLOT
    }
    // asm +0x21b..+0x2ae: 无条件——m_rankMap[towerIdx][stageVal].insert(
    //   pair(playTime→slotIdx))；键已存在则改写值为 slotIdx（ORIG fresh 插入与
    //   dup 改写均收敛为同一语义对）。
    std::pair<RankMap::iterator, bool> r =
        // b28d-55（P178/P86）：ORIG 双层临时链 make_pair → value_type 右值构造
        // （P167 差集：make_pair<unsigned&,unsigned&> + pair<const unsigned,unsigned>
        //  ::C1IjjE）；4.4 对 value_type(a,b) 直构折叠掉 make_pair。
        // [S2W6Deps] P183 去具名引用局部：ORIG 0x084686bc..0x084686f8 在实参位置**内联重算**
        // 秩表地址（`mov towerIdx; mov stageVal; mov %eax,%eax...` 直接进
        // insert 的 0x4(%esp)），无 `RankMap& row` 缓存槽（我方原形多 1 个
        // 槽 + 存/取 2 条，帧 0x14 差）；改内联表达式。
        m_rankMap[towerIdx][stageVal].insert(
            RankMap::value_type(std::make_pair(playTime, slotIdx)));
    if (!r.second)
        r.first->second = slotIdx;
    // asm +0x2b4..+0x2f5: 表级最优纪录比较，更优则整块拷贝（rep movsl 0x26*4）。
    stBestRecord_t cur;
    cur.m_stage = stageVal;
    cur.m_playTime = playTime;
    if (m_tableBest > cur)
        m_tableBest = cur;   // P83：ORIG rep movsl 0x26 dword = 结构整体赋值
}

#include "W6_RankMailDeps/resetRanking.inc"


#include "W6_RankMailDeps/registRanking.inc"


#include "W6_RankMailDeps/updateRanking.inc"


#include "W6_RankMailDeps/registBestRecord.inc"
    unsigned int towerIdx, unsigned int key,
    const struct stDeathTowerRecordMemberInfo_t* members,
    unsigned int stageVal, unsigned int playTime)
{
    // 栈上纪录（ctor 先清零；asm +0x01a: record+0 存 key）。
    stBestRecord_t rec;
    rec.m_key = key;         // asm +0x01d `mov %eax,-0x220(%ebp)`（4B）
    // asm +0x02f..+0x0c5: 名字/三条 byte lane 填充（同 registRanker 视图）。
    // [b28d-solo-W6_RankMailDeps] P183 去缓存：ORIG 每 lane 重发完整
    //   `mov i,%edx; imul $0x17,%edx,%edx; add 0x14(%ebp),%edx;
    //    movzbl 0x14(%edx),%edx` 链，无 src 具名指针槽（旧形多 -0x20 槽
    //   + 每 lane 2 条装载，帧 0x22c 下 rec 槽深 4）。
    for (unsigned int i = 0; i < m_tableType; ++i)
    {
        strncpy(rec.m_memberData[i],
                members[i].m_name, 0x1e);
        rec.m_pad7c[0][i] =
            members[i].m_b14;
        rec.m_pad7c[1][i] =
            members[i].m_b15;
        rec.m_pad7c[2][i] =
            members[i].m_b16;
    }
    rec.m_stage = stageVal;      // asm -0x198 = record+0x88
    rec.m_playTime = playTime;   // asm -0x194 = record+0x8c
    // asm +0x0d8..+0x187: ranking.find(key) → 纪录存故名次 field94；未命中保持
    // ctor 初值 -1。
    {
        RankTable& rt = *_getRankingTable(towerIdx);
        RankTable::const_iterator ri(rt.find(key));
        if (ri != RankTable::const_iterator(rt.end()))
            rec.m_field94 = static_cast<int>(ri->second);
    }
    // asm +0x199..+0x238: best 子表 insert(pair(key,record))，重复键整块拷贝
    // 覆盖为本次纪录（hash_map operator[] + rep movsl 0x26*4）。
    BestMap& bt = *_getBestRecordTable(towerIdx);
    std::pair<BestMap::iterator, bool> br =
        // b28d-55（P178）：ORIG make_pair<unsigned&,stBestRecord_t&> → value_type
        // 右值构造（P167 差集实证）。
        bt.insert(BestMap::value_type(std::make_pair(key, rec)));
    if (!br.second)
        bt[key] = rec;   // P83：ORIG operator[] + rep movsl 0x26 = 结构整体赋值
}

// ORIG W 0x08469d90 (size 0x1e) —— objdump 补证（无独立 md）：
// return this + 0x11d0 + idx*0x14 == &m_rankingTables[idx]（stride 已验证）。
WongWork::CDeathTowerRanking::RankTable*
#include "W6_RankMailDeps/_getRankingTable.inc"


// ORIG W 0x08469dae (size 0x1e) —— 同构，基址 0x1234 == m_bestTables。
WongWork::CDeathTowerRanking::BestMap*
#include "W6_RankMailDeps/_getBestRecordTable.inc"


// ============================================================================
// B) WongWork::CDeathTower::CDungeonMgr — 三 getter（转发 CDungeon）
// ============================================================================

#include "W6_RankMailDeps/getStandardLevel.inc"


std::vector<std::pair<int, int> >*
#include "W6_RankMailDeps/getObjectDropItems.inc"


std::vector<stDungeonAssignItem_t>*
#include "W6_RankMailDeps/getSpecialPassiveObjectItems.inc"


// ============================================================================
// C) WongWork::CMailBox / CMail
// ============================================================================

#include "W6_RankMailDeps/stAddNewMailInput.inc"


#include "W6_RankMailDeps/DecLoadedLetterCount.inc"


#include "W6_RankMailDeps/DeleteLetterKeepCount.inc"


#include "W6_RankMailDeps/IncNotLoadedMailCount.inc"


#include "W6_RankMailDeps/getMailLoadCount.inc"


#include "W6_RankMailDeps/Init.inc"


#include "W6_RankMailDeps/getMailIdx.inc"


// CMail::._398 子对象 ctor 现由 CMailBoxHelper.h 的匿名 m_package 子对象声明确认，
// 编译器自动生成（原先的 asm label 桥违反 §4.5，已移除）。
