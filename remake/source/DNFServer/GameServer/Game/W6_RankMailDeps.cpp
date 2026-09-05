// ============================================================================
// W6_RankMailDeps.cpp — Wave6 清零 only_orig（WongWork 排行榜 / 邮箱叶子）
//
// 覆盖（ORIG nm -S 权威地址；报告 docs/class_func_reports/ 相应目录）：
//   A) WongWork::CDeathTowerRanking：
//        registRanker / resetRanking / registRanking / updateRanking /
//        stBestRecord_t::operator> / operator==（头内 inline 去 const 后由
//        实例化点产生）/ _getRankingTable / registBestRecord /
//        _getBestRecordTable
//   B) WongWork::CDeathTower::CDungeonMgr：getStandardLevel /
//        getObjectDropItems / getSpecialPassiveObjectItems（三 getter 均转发
//        CDungeon 同名函数；报告目录无独立 md，objdump 0x08469ac0/0x08469ad6/
//        0x08469aec 补证）
//   C) WongWork::CMailBox：stAddNewMailInput C1/C2、DecLoadedLetterCount、
//        DeleteLetterKeepCount、IncNotLoadedMailCount、CMail::Init、
//        CMailBox::getMailLoadCount(const)、CMail::getMailIdx const、
//        CMail::_398 C1/C2（GCC 局部别名类构造：this+0x4 处 Inven_Item 构造）
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

// ORIG T 0x08468614 (size 0x300) —— registRanker.md
void WongWork::CDeathTowerRanking::registRanker(
    unsigned int towerIdx, unsigned int slotIdx,
    const struct stDeathTowerRecordMemberInfo_t* members,
    unsigned int stageVal, unsigned int playTime)
{
    // asm +0x00c..+0x03c: cmp $5/ja —— slotIdx>5 时跳过槽操作（落到 +0x21b）。
    if (slotIdx <= 5)
    {
        // 具类型 stride 寻址（asm +0x021..+0x032：imul $0x390(tower) +
        // imul $0x98(slot) 两步，塔块 = 6×stBestRecord_t 连续槽）。
        stBestRecord_t* slot =
            reinterpret_cast<stBestRecord_t*>(&m_towers[towerIdx]) + slotIdx;
        // asm +0x064..+0x0a3: 槽已占位 → 先按旧 stage 行、旧时间值 erase 排名
        // 记录（读取发生在改写之前）。
        if (slot->m_flag90 != 0)
        {
            unsigned int oldStage = slot->m_stage;      // +0x88
            unsigned int oldTime  = slot->m_playTime;   // +0x8c
            m_rankMap[towerIdx][oldStage].erase(oldTime);
        }
        // asm +0x0a8..+0x1a9: for i < m_tableType: strncpy(名字,0x1e) +
        // 三 byte lane ← members[i*0x17 + 0x14/15/16]。
        // members 按 ORIG 字节读（stDeathTowerRecordMemberInfo_t 未建模字段，
        // +0x14/15/16 三 byte 无具名证据，保留 stride+偏移形式）。
        for (unsigned int i = 0; i < m_tableType; ++i)
        {
            strncpy(slot->m_memberData[i],
                    reinterpret_cast<const char*>(members) + i * 0x17, 0x1e);
            const unsigned char* src =
                reinterpret_cast<const unsigned char*>(members) + i * 0x17;
            slot->m_pad7c[0][i] = src[0x14];
            slot->m_pad7c[1][i] = src[0x15];
            slot->m_pad7c[2][i] = src[0x16];
        }
        // asm +0x1af..+0x21b: 写新 stageVal/playTime 并置“有纪录”位=1。
        slot->m_stage = stageVal;
        slot->m_playTime = playTime;
        slot->m_flag90 = 1;
    }
    // asm +0x21b..+0x2ae: 无条件——m_rankMap[towerIdx][stageVal].insert(
    //   pair(playTime→slotIdx))；键已存在则改写值为 slotIdx（ORIG fresh 插入与
    //   dup 改写均收敛为同一语义对）。
    RankMap& row = m_rankMap[towerIdx][stageVal];
    std::pair<RankMap::iterator, bool> r =
        row.insert(RankMap::value_type(playTime, slotIdx));
    if (!r.second)
        r.first->second = slotIdx;
    // asm +0x2b4..+0x2f5: 表级最优纪录比较，更优则整块拷贝（rep movsl 0x26*4）。
    stBestRecord_t cur;
    cur.m_stage = stageVal;
    cur.m_playTime = playTime;
    if (m_tableBest > cur)
        m_tableBest = cur;   // P83：ORIG rep movsl 0x26 dword = 结构整体赋值
}

// ORIG T 0x08468594 (size 0x80) —— resetRanking.md
void WongWork::CDeathTowerRanking::resetRanking()
{
    // asm: 外层 cmp $0x4/setle ⇒ i<=4；内层 cmp $0xc7/setbe ⇒ s<=199。
    for (unsigned int i = 0; i < 5; ++i)
    {
        _getRankingTable(i)->clear();
        for (unsigned int s = 0; s < 200; ++s)
            m_rankMap[i][s].clear();
    }
}

// ORIG T 0x084683da (size 0xf3) —— registRanking.md
void WongWork::CDeathTowerRanking::registRanking(unsigned int tableIdx,
                                                 unsigned int key,
                                                 unsigned int rank)
{
    // asm +0x18..+0x81: ranking 子表 insert(key→rank)，重复键改写。
    RankTable& rt = *_getRankingTable(tableIdx);
    std::pair<RankTable::iterator, bool> rr =
        rt.insert(RankTable::value_type(key, rank));
    if (!rr.second)
        rr.first->second = rank;
    // asm +0x93..+0xf2: best 子表 find(key)；命中则其记录存故名次(+0x94)=rank。
    BestMap& bt = *_getBestRecordTable(tableIdx);
    BestMap::iterator bi = bt.find(key);
    if (bi != bt.end())
        bi->second.m_field94 = static_cast<int>(rank);
}

// ORIG T 0x084684ce (size 0x83) —— updateRanking.md
void WongWork::CDeathTowerRanking::updateRanking(unsigned int tableIdx,
                                                 unsigned int key,
                                                 unsigned int rank)
{
    RankTable& rt = *_getRankingTable(tableIdx);
    std::pair<RankTable::iterator, bool> rr =
        rt.insert(RankTable::value_type(key, rank));
    if (!rr.second)
        rr.first->second = rank;
}

// ORIG T 0x08468914 (size 0x238) —— registBestRecord.md
void WongWork::CDeathTowerRanking::registBestRecord(
    unsigned int towerIdx, unsigned int key,
    const struct stDeathTowerRecordMemberInfo_t* members,
    unsigned int stageVal, unsigned int playTime)
{
    // 栈上纪录（ctor 先清零；asm +0x01a: record+0 存 key）。
    stBestRecord_t rec;
    rec.m_key = key;         // asm +0x01d `mov %eax,-0x220(%ebp)`（4B）
    // asm +0x02f..+0x0c5: 名字/三条 byte lane 填充（同 registRanker 视图）。
    for (unsigned int i = 0; i < m_tableType; ++i)
    {
        strncpy(rec.m_memberData[i],
                reinterpret_cast<const char*>(members) + i * 0x17, 0x1e);
        const unsigned char* src =
            reinterpret_cast<const unsigned char*>(members) + i * 0x17;
        rec.m_pad7c[0][i] = src[0x14];
        rec.m_pad7c[1][i] = src[0x15];
        rec.m_pad7c[2][i] = src[0x16];
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
        bt.insert(BestMap::value_type(key, rec));
    if (!br.second)
        bt[key] = rec;   // P83：ORIG operator[] + rep movsl 0x26 = 结构整体赋值
}

// ORIG W 0x08469d90 (size 0x1e) —— objdump 补证（无独立 md）：
// return this + 0x11d0 + idx*0x14 == &m_rankingTables[idx]（stride 已验证）。
WongWork::CDeathTowerRanking::RankTable*
WongWork::CDeathTowerRanking::_getRankingTable(unsigned int idx)
{
    return &m_rankingTables[idx];
}

// ORIG W 0x08469dae (size 0x1e) —— 同构，基址 0x1234 == m_bestTables。
WongWork::CDeathTowerRanking::BestMap*
WongWork::CDeathTowerRanking::_getBestRecordTable(unsigned int idx)
{
    return &m_bestTables[idx];
}

// ============================================================================
// B) WongWork::CDeathTower::CDungeonMgr — 三 getter（转发 CDungeon）
// ============================================================================

int WongWork::CDeathTower::CDungeonMgr::getStandardLevel() const
{
    return m_pDungeon->get_standard_level();  // ORIG call _ZNK8CDungeon18get_standard_levelEv
}

std::vector<std::pair<int, int> >*
WongWork::CDeathTower::CDungeonMgr::getObjectDropItems() const
{
    return m_pDungeon->getObjectDropItems();  // ORIG call _ZNK8CDungeon18getObjectDropItemsEv
}

std::vector<stDungeonAssignItem_t>*
WongWork::CDeathTower::CDungeonMgr::getSpecialPassiveObjectItems() const
{
    return m_pDungeon->getSpecialPassiveObjectItems();  // ORIG call _ZNK8CDungeon28getSpecialPassiveObjectItemsEv
}

// ============================================================================
// C) WongWork::CMailBox / CMail
// ============================================================================

// ORIG W 0x085578ae (size 0x65)：前 0x28 字节清零（7 dword + byte@0x1c +
// dword@0x20/0x24）。语义映射（AddNewMail.md 装载证据）：+0x00 mailIndex、
// +0x04 senderCharacNo、+0x08 packageMoney/senderNamePtr（CMailBox.cpp 与
// CMailBoxHelper.cpp 两 TU 对该槽/后段视图分歧，见 NOTES 上报）、+0x0c letterId、
// +0x10 remainSec、+0x14 packageType、+0x18 period、+0x1c unlimit(byte)、
// +0x20/+0x24 指针字段。
WongWork::CMailBox::stAddNewMailInput::stAddNewMailInput()
{
    // 具名字段清零（SecuLink 批次裸偏移清理；字段布局/宽度证据见
    // CMailBoxHelper.h stAddNewMailInput 注释——AddNewMail ORIG 0x08551f5a）。
    // ORIG 清零序：+0x00..+0x18 七 dword、+0x1c 一 byte、+0x20/+0x24 两 dword。
    m_mailIndex = 0;
    m_senderCharacNo = 0;
    m_packageMoney = 0;
    m_letterId = 0;
    m_remainSec = 0;
    m_packageType = 0;
    m_period = 0;
    m_unlimit = 0;
    m_senderName = 0;
    m_packageItem = 0;
}

// ORIG W 0x084ed2fc (size 0x34)：m_loadedLetterCount-1；负数归零。
void WongWork::CMailBox::DecLoadedLetterCount()
{
    --m_loadedLetterCount;
    if (m_loadedLetterCount < 0)
        m_loadedLetterCount = 0;
}

// ORIG W 0x084ed280 (size 0x7c)：letterKeepCount erase；命中返回 1 否则 0。
int WongWork::CMailBox::DeleteLetterKeepCount(unsigned int key)
{
    // ORIG +0x006..+0x07a: find(key); it != end() → erase(it) 返回 1，否则 0。
    std::map<int, bool>::iterator it =
        m_letterKeepCount.find(static_cast<int>(key));
    if (it != m_letterKeepCount.end())
    {
        m_letterKeepCount.erase(it);
        return 1;
    }
    return 0;
}

// ORIG W 0x084ed330 (size 0x14)：m_notLoadedMailCount+1。
void WongWork::CMailBox::IncNotLoadedMailCount()
{
    ++m_notLoadedMailCount;
}

// ORIG W 0x084ed216 (size 0xb)：return this+0xc 的 int（现头具名字段
// m_mailLoadCount，原 m_pad0c 区）。
int WongWork::CMailBox::getMailLoadCount() const
{
    return m_mailLoadCount;
}

// ORIG T 0x085519e2 (size 0x1c)：m_mailIndex=0 后调 ResetPackage()。
void WongWork::CMailBox::CMail::Init()
{
    m_mailIndex = 0;
    ResetPackage();
}

// ORIG W 0x0869782e (size 0xa)：return m_mailIndex。
unsigned int WongWork::CMailBox::CMail::getMailIdx() const
{
    return m_mailIndex;
}

// CMail::._398 子对象 ctor 现由 CMailBoxHelper.h 的匿名 m_package 子对象声明确认，
// 编译器自动生成（原先的 asm label 桥违反 §4.5，已移除）。
