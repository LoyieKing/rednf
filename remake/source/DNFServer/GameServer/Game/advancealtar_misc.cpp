// df_game_r Game/ advancealtar 杂项批次（Wave5 AltarMisc）。覆盖 ORIG 符号：
//   BuyUpgradeData::{operator= 0x88a5784 W, getFieldData 0x88a2c62, getFieldType
//     0x88a3012, getMaxLevel 0x88a3322, getMinLevel 0x88a33c0,
//     getMinLevelFieldDataPoint 0x88a2f1a, getStatFirstPos 0x88a2f7a,
//     getStatIconIndex 0x88a3056, getStatStringIndex 0x88a3106,
//     isValidLevel 0x88a345e}
//   BuyShopData::reset                                  0x088a1090 T
//   AchievementRewardData::reset                        0x0889ae56 T
//   SurvivalRewardData::getRoundRewardData              0x0889aebe T(const)
//   RewardParameter::getAchievementRewardData           0x0889b544 / 0x0889b5a8
//   AdvanceAltarShopParameter::getMaxStarResetGoldFee   0x088a2bc0 T
//   StageDifficulty::getNextDifficulty                  0x0889900a T
//   isOnEvent                                           0x0812cfa6 T
//   isOpenAdvanceAltar                                  0x0812cfea T
//   putRidableInfoToPacket                              0x08141781 T
//   AdvanceAltarData::{set 0x88992da, getBuyItemMap 0x88999d8,
//                      getSlotMap 0x8899a0c}
//   StageTimeLineParameter::{importPhase 0x889a930,
//                            checkAndSetActionDefine 0x889a032}
// 以及使用点发射的生命周期弱符号：
//   _StarRestFeeByGold C1/C2 0x88a3760（inline ctor，头内定义）
//   _ActionAssign C1/C2      0x889b99c（inline ctor，头内定义）
//   _PhaseSummary C1/C2/D1/D2 0x889b954 / 0x0889ebc0（inline ctor/dtor，头内定义）
//   _BuyItem D1/D2           0x0813b4cc（inline 空析构，随使用点发射）
// 逐函数对照 docs/class_func_reports/ 对应报告；报告缺处以 objdump 转写并标
// [objdump ADDR]。需要 CMake 登记。
#include "CDataManager.h"            // advancealtar 共享类型权威段（含 AdvanceAltarField.h）
#include "advancealtar.h"            // AdvanceAltarData/_AdvanceAltarDbData/_Slot/_Stage 等
#include "advancealtar_StageDifficulty.h"
#include "CDungeon.h"                // SurvivalRewardData/RoundRewardData
#include "GlobalData.h"              // s_event_manager / s_DailyScheduleManager
#include "CGuildServerProxy.h"       // CEventManager::GetRepeatEvent / CEventBase::IsEventing
#include "PacketGuard.h"             // PacketGuard : InterfacePacketBuf
#include "DNFLexWrapper.h"           // ScanType/ScanInt(bool*)/ScanStr
#include "StringResource.h"          // CStringManager（全局域类，LinkFinal 链接收口批）

#include <cstdio>
#include <cstring>

namespace advancealtar
{



// ============================================================================
// BuyUpgradeData 查询组（docs/class_func_reports/advancealtar__BuyUpgradeData/）
// ============================================================================

// ORIG W 0x88a5784（objdump 全文）：+0/+4 直拷、+8 仅低 1 字节（movzbl）、
// +0xc string 赋值、+0x10 vector<FieldType::T> 赋值、+0x1c LevelFieldDataMap
// 赋值；返回 *this。
BuyUpgradeData& BuyUpgradeData::operator=(const BuyUpgradeData& other)
{
    m_nItemId = other.m_nItemId;                                    // [objdump 0x88a578a]
    m_field4 = other.m_field4;                                      // [objdump 0x88a5794]
    m_field8 = (int)(unsigned char)other.m_field8;                  // [objdump 0x88a57a3 movzbl]
    m_strDesc = other.m_strDesc;                                    // [objdump 0x88a57c0 Ss::operator=]
    m_fieldTypes = other.m_fieldTypes;                              // [objdump 0x88a57d8 vector::operator=]
    m_listMap = other.m_listMap;                                    // [objdump 0x88a57f0 map::operator=]
    return *this;
}

// ORIG T 0x088a2c62：入口恒记 logFormat(line 0x26e)；m_listMap 命中 level 且
// 其行命中 ft 时返回 FieldData::m_value；否则记 line 0x27b 错误日志后再做一次
// 相同两段查询并以同式返回（继承 ORIG 尾部指令序 [objdump 0x88a2d5a..2e41]）。
int BuyUpgradeData::getFieldData(int level, FieldType::T ft)
{
    LogManager::logFormat(
        0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
        0x26e, "BuyUpgradeData::GetFieldData level = %d", level);   // [objdump 0x88a2c98]

    LevelFieldDataMap::iterator lit = m_listMap.find(level);
    if (lit != m_listMap.end())
    {
        FieldDataMap::iterator fit = lit->second.find(ft);
        if (fit != lit->second.end())
        {
            return fit->second.m_value;                             // [objdump 0x88a2d52 add $4]
        }
    }

    std::string typeName(getStatStringIndex(ft));                   // [objdump 0x88a2d6e]
    LogManager::logFormat(
        0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
        0x27b,
        "AdvanceAltar BuyUpgradeData::GetFieldData Error level = %d, type = %s",
        level, typeName.c_str());                                   // [objdump 0x88a2db3]

    LevelFieldDataMap::iterator lit2 = m_listMap.find(level);       // [objdump 0x88a2dfe]
    FieldDataMap::iterator fit2 = lit2->second.find(ft);            // [objdump 0x88a2e29]
    return fit2->second.m_value;                                    // [objdump 0x88a2e3e add $4]
}

// ORIG T 0x088a3012：idx 越界 → -1；否则 m_fieldTypes[idx]。
int BuyUpgradeData::getFieldType(int idx) const
{
    int result = -1;
    if (idx < (int)m_fieldTypes.size())
    {
        result = (int)m_fieldTypes[idx];
    }
    return result;
}

// ORIG T 0x088a3322：初值 0，取 m_listMap 最大 key。
int BuyUpgradeData::getMaxLevel() const
{
    // b28c-L5：ORIG 0x088a3322 循环增量段调
    //   `_ZNSt23_Rb_tree_const_iterator...ppEi`（**后置** ++，带独立 sret 临时槽
    //   -0x10，it=-0x18 / end=-0x14 三槽并存）。4.4 对 `++it` 走 ppEv（返回引用、
    //   免临时槽）⇒ 迭代器槽阶梯少一格、cond 段整体错位（原 ndiff 26）。
    //   改后置形即复现 ORIG 三槽 + ppEi 调用（P28 声明序即栈槽序的增量侧推论）。
    int maxLevel = 0;
    for (LevelFieldDataMap::const_iterator it = m_listMap.begin();
         it != m_listMap.end(); it++)
    {
        if (maxLevel < it->first)
        {
            maxLevel = it->first;
        }
    }
    return maxLevel;
}

// ORIG T 0x088a33c0：初值 0，取 m_listMap 最小 key。
int BuyUpgradeData::getMinLevel() const
{
    // b28c-L5：同 getMaxLevel —— ORIG ppEi 后置 ++ 三槽形（0x088a33c0）。
    int minLevel = 0;
    for (LevelFieldDataMap::const_iterator it = m_listMap.begin();
         it != m_listMap.end(); it++)
    {
        if (it->first < minLevel)
        {
            minLevel = it->first;
        }
    }
    return minLevel;
}

// ORIG T 0x088a2f1a：out=0 起、失败再以 out=1 试一次，返回末次
// getFieldDataPoint 结果。
FieldData* BuyUpgradeData::getMinLevelFieldDataPoint(FieldType::T ft, int& outLevel)
{
    outLevel = 0;
    FieldData* point = getFieldDataPoint(outLevel, ft);
    if (point == 0)
    {
        outLevel = 1;
        point = getFieldDataPoint(outLevel, ft);
    }
    return point;
}

// ORIG T 0x088a2f7a：首个落在 [FT_HP(0xa), FT_ETC2(0x18)] 的字段下标，否则 -1。
int BuyUpgradeData::getStatFirstPos() const
{
    // b28c-L5：ORIG 0x088a2f7a 两处形状约束——
    //   判词（N197④ 反向·setcc 物化不可免）：ORIG 边界判定段为
    //     `mov (%eax),%eax / sub $0xa,%eax / cmp $0xe,%eax / ja <continue>`（4 条直跳）；
    //   4.4-O0 对「**寄存器内算术结果** vs 立即数」的比较且该 bool 有分支消费者时，
    //   恒物化 `setbe %al; test %al,%al; je`（5 条）。`< 0xfU` 与 `<= 0xeU` 两写法
    //   实测产物完全相同（同一 fold 路径）⇒ 非源形可控，属 P197④ 的镜像案例
    //   （该条记的是「4.4 恒直跳、ORIG 恒 sete」，此处相反）。语义与边界值全等
    //   （[FT_HP=0xa, FT_ETC2=0x18] 闭区间），判 setcc-fold 负例封顶。
    //   ① 取值段 `call de; mov (%eax),%eax; sub $0xa,%eax; cmp $0xe,%eax; ja`
    //      无槽往返 ⇒ *it 是表达式直用（P0），原实现具名 `const unsigned int ft`
    //      多出 `mov %eax,-0xc / mov -0xc,%eax` 一对并把 pos 槽顶到 -0x10；
    //   ② 增量段 `addl $1,-0xc(pos)` 后 `ppEi`（后置 ++，独立 sret -0x10）⇒
    //      pos 先自增 + it 后置。两处同改后 pos 回落 -0xc、三槽阶梯复原。
    int pos = 0;
    for (std::vector<FieldType::T>::const_iterator it = m_fieldTypes.begin();
         it != m_fieldTypes.end(); it++)
    {
        // b28c-L5：ORIG 判定段为 `sub $0xa,%eax / cmp $0xe,%eax / ja <continue>`
        //   ——**直跳**（3 条）。4.4 对 `< 0xfU` 恒物化 bool（`setbe;test;je`，5 条）；
        //   改等价 `<= 0xeU` 后 4.4 出直跳形（同一无符号边界，仅比较算符反转）。
        if ((unsigned int)*it - 10U <= 0xeU)
        {
            return pos;
        }
        ++pos;
    }
    return -1;
}

// ORIG T 0x088a3056：ft∈[10,24] → 序号 0..14，缺省 -1。
int BuyUpgradeData::getStatIconIndex(FieldType::T ft) const
{
    switch (ft)
    {
    case FieldType::FT_HP:   return 0;      // 0x0a
    case FieldType::FT_DMG:  return 1;      // 0x0b
    case FieldType::FT_PIC:  return 2;      // 0x0c
    case FieldType::FT_RCV:  return 3;      // 0x0d
    case FieldType::FT_SUM:  return 4;      // 0x0e
    case FieldType::FT_MHT:  return 5;      // 0x0f
    case FieldType::FT_RGN:  return 6;      // 0x10
    case FieldType::FT_MSD:  return 7;      // 0x11
    case FieldType::FT_ASD:  return 8;      // 0x12
    case FieldType::FT_SDT:  return 9;      // 0x13
    case FieldType::FT_FDT:  return 10;     // 0x14
    case FieldType::FT_ETC:  return 0xb;    // 0x15
    case FieldType::FT_LRG:  return 0xc;    // 0x16
    case FieldType::FT_SPL:  return 0xd;    // 0x17
    case FieldType::FT_ETC2: return 0xe;    // 0x18
    default:                 return -1;
    }
}

// ORIG T 0x088a3106（sret 返回 std::string）：FieldType→字符串表条目
// CStringManager::getStr(id)；区间外的 ft 返回空串。
std::string BuyUpgradeData::getStatStringIndex(FieldType::T ft) const
{
    std::string result;
    int strId = -1;
    switch (ft)
    {
    case FieldType::FT_HP:   strId = 0x11570; break;   // [objdump 0x88a3134]
    case FieldType::FT_DMG:  strId = 0x11571; break;   // [objdump 0x88a3153]
    case FieldType::FT_PIC:  strId = 0x11573; break;   // [objdump 0x88a3172]
    case FieldType::FT_RCV:  strId = 0x11574; break;   // [objdump 0x88a3191]
    case FieldType::FT_SUM:  strId = 0x11575; break;   // [objdump 0x88a31b0]
    case FieldType::FT_MHT:  strId = 0x11576; break;   // [objdump 0x88a31cf]
    case FieldType::FT_RGN:  strId = 0x1158f; break;   // [objdump 0x88a31ee]
    case FieldType::FT_MSD:  strId = 0x11590; break;   // [objdump 0x88a320d]
    case FieldType::FT_ASD:  strId = 0x11591; break;   // [objdump 0x88a322c]
    case FieldType::FT_SDT:  strId = 0x11592; break;   // [objdump 0x88a324b]
    case FieldType::FT_FDT:  strId = 0x11593; break;   // [objdump 0x88a326a]
    case FieldType::FT_ETC:  strId = 0x11596; break;   // [objdump 0x88a3286]
    case FieldType::FT_LRG:  strId = 0x11572; break;   // [objdump 0x88a32a5 区段]
    case FieldType::FT_SPL:  strId = 0x11595; break;
    case FieldType::FT_ETC2: strId = 0x11596; break;
    default:                 return result;            // 跳转表范围外 → 空 string
    }
    result = CStringManager::getStr(strId);
    return result;
}

// ORIG T 0x088a345e：!(level<min || max<level)。
bool BuyUpgradeData::isValidLevel(int level)
{
    bool result = true;
    if (level < getMinLevel() || getMaxLevel() < level)
        result = false;
    return result;
}

// ============================================================================
// BuyShopData::reset（ORIG T 0x088a1090：+0=0 + 三 vector clear）
// ============================================================================
void BuyShopData::reset()
{
    m_nField0 = 0;
    m_vecT0.clear();
    m_vecT1.clear();
    m_vecT2.clear();
}

// ============================================================================
// AchievementRewardData::reset（ORIG T 0x0889ae56 报告全文）
// ============================================================================
void AchievementRewardData::reset()
{
    m_type = 0;                    // +0x00
    m_difficulty = -1;             // +0x04
    m_stageClearCount = 0;         // +0x08
    m_iconImage = -1;              // +0x0c
    m_rewardItems.clear();         // +0x10
    m_rewardTitle.clear();         // +0x1c
    m_postalTitle.clear();         // +0x20
    m_postalLetter.clear();        // +0x24
}

// ============================================================================
// SurvivalRewardData::getRoundRewardData（ORIG T const 0x0889aebe 报告全文）：
// 首个满足 m_0<=round<=m_4 的 RoundRewardData → &元素.m_vec8，未命中 → 0。
// ============================================================================
std::vector<_RewardItem>* SurvivalRewardData::getRoundRewardData(int round) const
{
    // b28c-L5：ORIG 0x0889aebe 增量段 `ppEi`（后置 ++，sret 临时 -0xc；
    //   it=-0x14 / end=-0x10）⇒ 改后置形复原三槽阶梯（同 getMaxLevel 判据）。
    for (std::vector<RoundRewardData>::const_iterator it = m_vec.begin();
         it != m_vec.end(); it++)
    {
        if (!((round < it->m_0) || (it->m_4 < round)))
        {
            return const_cast<std::vector<_RewardItem>*>(&it->m_vec8);
        }
    }
    return 0;
}

// ============================================================================
// RewardParameter::getAchievementRewardData 两个重载
// ============================================================================

// ORIG T 0x0889b544：m_rewardMap.find(type) 命中 → 元素指针，否则 0。
advancealtar::AchievementRewardData*
RewardParameter::getAchievementRewardData(int type) const
{
    // b28c-L5：ORIG 0x0889b544 块排布 = `operator== 命中(==end) → je 落「命中分支」`，
    //   即原码为**否定早返** `if (it == end) return 0;`（return 0 岛在 ptEv 之前，
    //   尾 `jmp` 汇合）。4.4 对 `if (it != end) {...} return 0;` 产镜像排布
    //   （ptEv 在前、mov $0 在尾）→ 6 行块序差。P222 双向：本例负极性早返即正解。
    std::map<int, AchievementRewardData>::const_iterator it = m_rewardMap.find(type);
    if (it == m_rewardMap.end())
    {
        return 0;
    }
    return const_cast<AchievementRewardData*>(&it->second);
}

// ORIG T 0x0889b5a8：线性找 m_difficulty==diff 且 m_stageClearCount==count 的
// 条目（命中判断 [objdump 0x889b61c +0x8/+0xc]）；未命中 → 0。
advancealtar::AchievementRewardData*
RewardParameter::getAchievementRewardData(StageDifficulty::T diff, int count) const
{
    for (std::map<int, AchievementRewardData>::const_iterator it = m_rewardMap.begin();
         it != m_rewardMap.end(); ++it)
    {
        const AchievementRewardData& data = it->second;
        if (data.m_difficulty == (int)diff && data.m_stageClearCount == count)
        {
            return const_cast<AchievementRewardData*>(&data);
        }
    }
    return 0;
}

// ============================================================================
// AdvanceAltarShopParameter::getMaxStarResetGoldFee（ORIG T 0x088a2bc0 报告全
// 文）：maxKey 初值 0 取 m_starRestMap 最大 key，返回 operator[](maxKey) 元素
// 值（空表时会插入默认元素，行为保留）。
// ============================================================================
int AdvanceAltarShopParameter::getMaxStarResetGoldFee()
{
    int maxKey = 0;
    for (std::map<int, _StarRestFeeByGold>::const_iterator it = m_starRestMap.begin();
         it != m_starRestMap.end(); ++it)
    {
        if (maxKey < it->first)
        {
            maxKey = it->first;
        }
    }
    return m_starRestMap[maxKey].m_field0;
}

// ============================================================================
// StageDifficulty::getNextDifficulty（ORIG T 0x0889900a 报告全文）
// ============================================================================
StageDifficulty::T StageDifficulty::getNextDifficulty(StageDifficulty::T diff)
{
    // P208/P163：switch 复现 ORIG 4.1 决策树（值缓存 %eax + test/cmp 链）。
    // 枚举底层为 unsigned 会吞 -1 case（「case 标号低于类型最小值」告警），
    // switch(int) 保留（g2 探针全等 ORIG 树）。
    switch (static_cast<int>(diff))
    {
    case DIFF_EASY:                     // 0 → 1
        return DIFF_MEDIUM;
    case DIFF_MEDIUM:                   // 1 → 2
        return DIFF_HARD;
    case -1:                             // -1 → 0
        return DIFF_EASY;
    default:
        return diff;
    }
}

// ============================================================================
// isOnEvent（ORIG T 0x0812cfa6，objdump 全文）：
//   eax=s_event_manager(0x0941f730)；GetRepeatEvent(0x77)；虚槽 vptr+0x34
//   （IsEventing(CUser*)）以参数 0 调用；返回 IsEventing!=false。
bool isOnEvent()
{
    // P183 禁指针缓存：ORIG 每步重物化，无具名局部（0x812cfac/cfbc 实证）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x77)->IsEventing((CUser*)0))
        return false;
    return true;
}
// ============================================================================
bool isOpenAdvanceAltar()
{
    const int state =
        GlobalData::s_DailyScheduleManager->GetScheduleState((_DailyScheduleType)4, 0xa);
    if (state == 1)
        return true;
    return false;
}

// ============================================================================
// putRidableInfoToPacket（ORIG T 0x08141781，报告全文）：前 0x21 字节直拷 +
// 七个 map 各 put_int(size) 后逐元素 put_binary(&value, 元素尺寸)。
// 去发明模板：ORIG nm 无 putMapEntries 实例（§4.1），七段逐循环内联，
// 循环体形态按 ORIG 汇编（begin 前置 0x0851417d2 区、end 每轮尾构、
// postfix it++ 带 dest 临时 0x08135f38、operator-> 后 +4/+1 直传 put_binary）。
// 残余 DIFF 归因（终态 insns 350 vs 349，槽图 -0x10/-0xc/-0x14..-0x64 与
// begin/end/++temp 三件套逐段对齐）：唯一真实指令差 = 首条 put_binary 的
// data 装载 ORIG 两段 `mov 0xc(%ebp),%eax; mov %eax,%edx`（0x08141787/8a），
// 4.4.7 恒折单段（N18a 负例，probe 不可翻）；其余 diff 行为该 1 行错位引发
// 的对齐伪差与 ORIG nearest-symbol call 显示行（put_binary 等公共 W 符号两侧
// 同名，relink 归一后消）。
// ============================================================================
void putRidableInfoToPacket(PacketGuard& packet, const AdvanceAltarData& data)
{
    // ORIG 每处直接 `mov 0x8(%ebp)` 传参（无 buf 引用槽）⇒ 不用具名别名。
    // 段 2..6 容器指针 = ORIG 二段初始化槽（`movl $0,-0x10/-0xc` + 赋值，
    // 0x0814184e/0x0814185b 等）；段 1/7 直取（0x081417a3/0x08141bb7）。
    packet.put_binary(const_cast<char*>(reinterpret_cast<const char*>(&data)), 0x21);   // [objdump 0x8141798]

    // +0x24 stages（put_binary 8 字节，0x081417fe）
    packet.put_int((int)data.m_stages.size());                           // [0x081417ac/0x081417bd]
    for (std::map<int, _Stage>::const_iterator it = data.m_stages.begin();
         it != data.m_stages.end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Stage*>(&it->second)), sizeof(_Stage));

    // +0x3c / +0x54 / +0x6c 三张 buyItem 表（各 0xc，0x081418b3/0x0814195e/
    // 0x08141a09），局部引用形：ORIG 每轮 `lea 0xXX(%eax)` 重取容器地址。
    std::map<int, _BuyItem> const* pBuy = 0;
    pBuy = &data.m_buyItems1;
    packet.put_int((int)pBuy->size());                                   // [0x08141864]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    pBuy = &data.m_buyItems2;
    packet.put_int((int)pBuy->size());                                   // [0x0814190f]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    pBuy = &data.m_buyItems3;
    packet.put_int((int)pBuy->size());                                   // [0x081419ba]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    // +0x84 / +0x9c slot 表（key uchar，value 偏移 +1，len 10，
    // 0x08141abd/0x08141b6a）
    std::map<unsigned char, _Slot> const* pSlot = 0;
    pSlot = &data.m_slots1;
    packet.put_int((int)pSlot->size());                                  // [0x08141875]
    for (std::map<unsigned char, _Slot>::const_iterator it = pSlot->begin();
         it != pSlot->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Slot*>(&it->second)), sizeof(_Slot));

    pSlot = &data.m_slots2;
    packet.put_int((int)pSlot->size());                                  // [0x08141920]
    for (std::map<unsigned char, _Slot>::const_iterator it = pSlot->begin();
         it != pSlot->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Slot*>(&it->second)), sizeof(_Slot));

    // +0xb4 rewards（5 字节，0x08141c17）
    packet.put_int((int)data.m_rewards.size());                          // [0x08141bc2]
    for (std::map<int, _Reward>::const_iterator it = data.m_rewards.begin();
         it != data.m_rewards.end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Reward*>(&it->second)), sizeof(_Reward));
}

// ============================================================================
// AdvanceAltarData::{getBuyItemMap,getSlotMap,set}
// ============================================================================

// ORIG T 0x088999d8（非 const，nm 实证 _ZN 无 K）：0/1/2 →
// +0x3c/+0x54/+0x6c，其它 → 0。
std::map<int, _BuyItem>* AdvanceAltarData::getBuyItemMap(AdvanceAltarShopType::T type)
{
    switch ((int)type)
    {
    case AdvanceAltarShopType::T_0: /* 仅覆盖整型 0 */
        return &m_buyItems1;
    case AdvanceAltarShopType::T_1:
        return &m_buyItems2;
    case AdvanceAltarShopType::T_2:
        return &m_buyItems3;
    default:
        return 0;
    }
}

// ORIG T 0x08899a0c（非 const，nm 实证 _ZN 无 K）：1/2 → +0x84/+0x9c，
// 其它（含 0）→ 0。
std::map<unsigned char, _Slot>* AdvanceAltarData::getSlotMap(
    AdvanceAltarSlotType::T type)
{
    switch (static_cast<int>(type))
    {
    case 1:
        return &m_slots1;
    case 2:
        return &m_slots2;
    default:
        return 0;
    }
}

// ORIG T 0x088992da（objdump 全文，四段循环逐条转写）。序列化侧 get()
// （advancealtar_DB.cpp）仅落 value 域，因此本逆序列化中各记录的 map key 均
// 取自记录自身的 m_field0/m_byte0/m_int1 字段（与 make_pair 的同址取参一致，
// 见函数体注释地址）。
void AdvanceAltarData::set(const _AdvanceAltarDbData& db)
{
    m_update = 0;                                                  // [objdump 0x88992e6]
    std::memcpy(this, &db, 0x21);                                  // [objdump 0x8899302]

    // stages：i<=0x31，条件/键均读 db.m_stages[i].m_field0（pack 后 db+0x21+i*8）
    for (int i = 0; i <= 0x31; ++i)                                // [objdump 0x889939f setle]
    {
        const _Stage& stage = db.m_stages[i];
        if ((int)stage.m_field0 > 0)                               // [objdump 0x889931c jle]
        {
            m_stages.insert(std::make_pair(stage.m_field0, stage));  // [0x8899353 key/value 同址]
        }
    }

    // slots：i<=0xb；记录基 = db+0x1b0+i*10（元素实际起 0x1b1）：
    //   key = 记录.m_byte0（rec+1）、cond int = m_int1（rec+2）、
    //   value = 从 rec+6 起的 10 字节（跨到下一记录头部，忠实 ORIG）
    for (int i = 0; i <= 0xb; ++i)                                 // [objdump 0x889952c]
    {
        const char* rec = reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10;
        int slotType = 0;
        std::memcpy(&slotType, rec + 2, sizeof(slotType));         // [objdump 0x88993d1 +2]
        if (slotType == 0)
        {
            continue;                                              // [objdump 0x88993d6 je]
        }
        unsigned char slotKey = 0;
        std::memcpy(&slotKey, rec + 1, sizeof(slotKey));           // [objdump 0x8899413 key=rec+1]
        _Slot slotValue;
        std::memcpy(&slotValue, rec + 6, sizeof(slotValue));       // [objdump 0x889942a +6]
        if (slotType == 1)                                         // [objdump 0x88993f6]
        {
            m_slots1.insert(std::make_pair(slotKey, slotValue));   // [objdump 0x8899461 target +0x84]
        }
        else if (slotType == 2)                                    // [objdump 0x88994a5]
        {
            m_slots2.insert(std::make_pair(slotKey, slotValue));   // [objdump 0x8899503 target +0x9c]
        }
    }

    // buyItems：i<=0x31；key/shopType 读 m_buyItems[i].m_field0（db+0x229+i*12），
    // 计数条件读 m_field4（db+0x22d+i*12），value 取自 m_field4 起 12 字节。
    for (int i = 0; i <= 0x31; ++i)                                // [objdump 0x8899614]
    {
        const char* rec = reinterpret_cast<const char*>(&db.m_buyItems) + i * 0xc;
        int count = 0;
        std::memcpy(&count, rec + 4, sizeof(count));               // [objdump 0x889955e +0xd→abs 0x22d]
        if (count <= 0)
        {
            continue;                                              // [objdump 0x8899563 jle]
        }
        AdvanceAltarShopType::T shopType = (AdvanceAltarShopType::T)0;
        int itemKey = 0;
        std::memcpy(&itemKey, rec, sizeof(itemKey));               // [objdump 0x8899580 +0x9→abs 0x229]
        shopType = (AdvanceAltarShopType::T)itemKey;
        std::map<int, _BuyItem>* target = getBuyItemMap(shopType); // [objdump 0x889958d]
        if (target == 0)
        {
            continue;                                              // [objdump 0x8899599 je]
        }
        _BuyItem itemValue;
        std::memcpy(&itemValue, rec + 4, sizeof(itemValue));       // [objdump 0x88995c6 value=+0xd]
        target->insert(std::make_pair(itemKey, itemValue));
    }

    // rewards：i<=0x1d（数组 29 项 + 第 30 次读取落入尾部 pad，忠实 ORIG 上限）；
    // key/value 同址取 m_rewards[i].m_field0。
    for (int i = 0; i <= 0x1d; ++i)                                // [objdump 0x88996c6]
    {
        const char* rec = reinterpret_cast<const char*>(&db) + 0x480 + i * 5;
        int rewardKey = 0;
        std::memcpy(&rewardKey, rec + 1, sizeof(rewardKey));       // [objdump 0x8899644 +1→abs 0x481]
        if (rewardKey <= 0)
        {
            continue;                                              // [objdump 0x8899649 jle]
        }
        _Reward rewardValue;
        std::memcpy(&rewardValue, rec + 1, sizeof(rewardValue));
        m_rewards.insert(std::make_pair(rewardKey, rewardValue));  // [objdump 0x889683]
    }
}

// ============================================================================
// StageTimeLineParameter::{importPhase,checkAndSetActionDefine}
// ============================================================================

// ORIG T 0x889a930（报告反编译 + 关键偏移 objdump 复核：Phase 局部对象 -0x24，
// [duration]→Phase+0、[type]→Phase+4、_ActionAssign 局部 -0x3c 基址，
// 3 次 ScanInt 分别写 +0 int/-8→short/-6→short，push 到 Phase+0x08）。
int StageTimeLineParameter::importPhase(TimeLine& timeLine)
{
    for (;;)
    {
        std::string token;
        if (!ScanType(token, true))
        {
            return 0x2b1;                                          // token 流耗尽
        }
        if (token == "[/time line]")
        {
            return 0;
        }
        if (token == "[phase]")
        {
            Phase phase;
            bool phaseDone = false;
            while (!phaseDone)
            {
                std::string tag;
                if (!ScanType(tag, true))
                {
                    return 0x2be;                                  // [objdump 0x889a9ee 路径]
                }
                if (tag == "[/phase]")
                {
                    phaseDone = true;
                }
                else if (tag == "[duration]")
                {
                    bool ok = false;
                    phase.m_field0 = ScanInt(&ok);                 // [objdump 0x889aa36 -0x24]
                    if (!ok)
                    {
                        return 0x2c6;                              // [objdump 0x889aa48]
                    }
                }
                else if (tag == "[type]")
                {
                    std::string name;
                    if (!ScanStr(&name))
                    {
                        return 0x2cc;                              // [objdump 0x889aa8f]
                    }
                    if (name == "normal")
                    {
                        phase.m_field4 = 0;                        // [objdump 0x889aac? -0x20]
                    }
                    else if (name == "named")
                    {
                        phase.m_field4 = 1;
                    }
                    else if (name == "boss")
                    {
                        phase.m_field4 = 2;                        // [objdump 0x889aaf5]
                    }
                    else
                    {
                        return 0x2dc;                              // [objdump 0x889aafe]
                    }
                }
                else if (tag == "[action assign]")
                {
                    for (;;)
                    {
                        _ActionAssign assign;                      // [objdump 0x889ab61 ctor 每次]
                        bool ok = false;
                        assign.m_field0 = ScanInt(&ok);            // [objdump 0x889ab77 -0x3c]
                        if (!ok)
                        {
                            break;                                 // [objdump 0x889ab83 jne→esi=2]
                        }
                        assign.m_minAliveCount = (short)ScanInt(&ok);   // [objdump 0x889ab90 -0x34]
                        if (!ok)
                        {
                            return 0x2ed;                          // [objdump 0x889ab9f]
                        }
                        assign.m_maxAliveCount = (short)ScanInt(&ok);   // [objdump 0x889abb6 -0x32]
                        if (!ok)
                        {
                            return 0x2f0;                          // [objdump 0x889abc5]
                        }
                        phase.m_assigns.push_back(assign);         // [objdump 0x889abe1]
                    }
                }
            }
            timeLine.m_phases.push_back(phase);                    // [objdump 0x889ac27 区段]
        }
    }
}

// ORIG T 0x889a032（报告反编译全文）：回填 m_define；找不到 → printf + 0；
// 全部成功则写 summary（(phaseIdx, 累计时长起点, phase type)）并把累计时长存
// TimeLineSummary::m_field0（TimeLine+0xc），返回 1。
int StageTimeLineParameter::checkAndSetActionDefine(
    TimeLine& timeLine, const std::map<int, ActionDefine>& defines)
{
    int totalDuration = 0;                                         // local_18
    int phaseIndex = 0;                                            // local_14
    for (std::vector<Phase>::iterator pit = timeLine.m_phases.begin();
         pit != timeLine.m_phases.end(); ++pit)
    {
        Phase& phase = *pit;
        for (std::vector<_ActionAssign>::iterator ait = phase.m_assigns.begin();
             ait != phase.m_assigns.end(); ++ait)
        {
            std::map<int, ActionDefine>::const_iterator dit = defines.find(ait->m_field0);
            if (dit == defines.end())
            {
                std::printf("advancealtar : not found ActionDefine Id = %d\n",
                            ait->m_field0);                        // [objdump printf 格式串]
                return 0;
            }
            ait->m_define = const_cast<ActionDefine*>(&dit->second);   // +4 回填
        }

        _PhaseSummary summary;                                     // [objdump ctor -0x40]
        {
            // m_pad 仅字节视图：+0=phaseIdx、+4=累计时长起点、+8=phase 类型
            // （[objdump 0x889a?...]：local_40/local_3c/local_38 三连写）。
            int fields[3];
            fields[0] = phaseIndex;
            fields[1] = totalDuration;
            fields[2] = phase.m_field4;
            std::memcpy(summary.m_pad, fields, sizeof(fields));
        }
        timeLine.m_summary.m_phaseSummarys.push_back(summary);     // push param_1+0x10

        totalDuration += (int)phase.m_field0;                      // [objdump 本轮时长累加]
        ++phaseIndex;                                              // local_14
    }
    timeLine.m_summary.m_field0 = totalDuration;                   // [objdump *(int*)(param_1+0xc)]
    return 1;
}

// ---- 布局断言（本批次相关）-------------------------------------------------
typedef char assert_slotfee_0x4[(sizeof(_StarRestFeeByGold) == 4) ? 1 : -1];

}  // namespace advancealtar
