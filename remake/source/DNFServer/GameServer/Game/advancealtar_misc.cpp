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
// 其行命中 ft 时返回 &FieldData；否则记 line 0x27b 错误日志后再以 key=1 做一次
// 相同两段查询并同式返回（继承 ORIG 尾部指令序 [objdump 0x88a2d5a..2e41]）。
// [b28d-solo-advancealtar_misc 判词] NEAR（134:134，助记符序列全等、仅槽号差）
//   已修：①返回类型 int→FieldData*（ORIG 两路 `call operator-> ; add $0x4,%eax`）；
//   ②第二段 find key 常量 1（P171 照抄）；③typeName 独立块（析构先于第二段 find）
//   使返回路径回归 ORIG `add $0x4,%eax`（块外声明时为 lea+ebx 中转）。
//   残余（N 封顶，P212/N93/N82附）：4.1 给 typeName 浅槽 -0x10、lit 深 -0x1c；
//   4.4 相反（typeName -0x24、lit -0x18），其余槽随动 4B（end2 -0x10↔-0x14、
//   end1 -0x14↔-0x18、fit -0x20↔-0x24、fit2 -0x1c↔-0x20；key -0xc / sret -0x2c
//   两侧相同）。P212 全头部声明法对非平凡类型不可用（提前声明 string 会引入
//   C1Ev + operator= 两处 call，实测 missing +2、更劣）；嵌套块/块内先声明 fit2/
//   迭代器先声明后赋值（P50）三形实测均不闭合。语义/调用点/指令数全等。
FieldData* BuyUpgradeData::getFieldData(int level, FieldType::T ft)
{
    // b28d-solo-advancealtar_misc：ORIG @0x088a2c62 两处返回路径均为
    //   `call _Rb_tree_iterator<pair<const FieldType::T,FieldData>>::operator->`
    //   （0x088a4498 W）+ `add $0x4,%eax` ⇒ 返回 &pair.second（FieldData*），
    //   非取值 ⇒ 返回类型由 int 改 FieldData*（见 CDataManager.h 声明处注释）。
    //   第二段查询 key 为常量 1（ORIG 0x088a2de0 `movl $0x1,-0xc(%ebp)` +
    //   `lea -0xc(%ebp),%edx` 传 find 的 key 引用；P171 原码行为照抄）。
    LogManager::logFormat(
        0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
        0x26e, "BuyUpgradeData::GetFieldData level = %d", level);   // [objdump 0x88a2c98]

    LevelFieldDataMap::iterator lit = m_listMap.find(level);
    if (lit != m_listMap.end())
    {
        FieldDataMap::iterator fit = lit->second.find(ft);
        if (fit != lit->second.end())
        {
            return &fit->second;                                    // [objdump 0x88a2d52 add $4]
        }
    }

    // ORIG 0x088a2dd5 在第二段 find 之前析构 typeName ⇒ 失败日志段为独立块
    //   （P212 块作用域；ORIG typeName 槽 -0x10 浅于 lit -0x1c、fit2 -0x20）。
    {
        std::string typeName(getStatStringIndex(ft));               // [objdump 0x88a2d6e]
        LogManager::logFormat(
            0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
            0x27b,
            "AdvanceAltar BuyUpgradeData::GetFieldData Error level = %d, type = %s",
            level, typeName.c_str());                               // [objdump 0x88a2db3]
    }

    lit = m_listMap.find(1);                                        // [objdump 0x88a2de0/0x88a2dfe]
    FieldDataMap::iterator fit2 = lit->second.find(ft);             // [objdump 0x88a2e29]
    return &fit2->second;                                           // [objdump 0x88a2e3e add $4]
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
    // b28d-solo-advancealtar_misc：IDENTICAL/AE 复验通过（fresh .o compare_tu clean，
    //   49=49 逐条全等，仅 .o 未解析重定位伪差）。ORIG @0x088a3322。
    // b28d-35: IDENTICAL_AE（49/49 clean 全等）。ORIG @0x088a3322；ppEi 后置
    //   三槽形 + advancealtar.h enum 化后重编复验通过。
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
    // b28d-solo-advancealtar_misc：IDENTICAL/AE 复验通过（fresh .o compare_tu clean，
    //   49=49 逐条全等）。ORIG @0x088a33c0。
    // b28d-35: IDENTICAL_AE（49/49 clean 全等）。ORIG @0x088a33c0；同 getMaxLevel。
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
    // b28d-35: DIFF→N 封顶（P197④ 镜像族：ORIG @0x088a2f7a 对 call 结果算术
    //   比较出直跳 `ja`，4.4 恒物化 `setbe;test;je`）。七形实测（/tmp/d35_sfp*.cpp
    //   h1-h21）：裸指针解引用直跳、迭代器 operator* 结果恒物化；< 0xfU 与 <= 0xeU
    //   同折；goto/取反/双比较（双比较需重复调用 operator*，违 P48）均不闭合。
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
        // b28d-solo-advancealtar_misc：ORIG 判定段 `sub $0xa,%eax / cmp $0xe,%eax / ja`
        //   ——**直跳**（无 bool 物化）。4.4 对 if 形（`< 0xfU`/`<= 0xeU`/`> 0xeU`
        //   加 continue/空 then 共七形实测）恒物化 `setbe;test;je`（+2 条，P197④ 镜像族）；
        //   改 15 个连续 case 汇聚到同一 `return pos` 的 switch 后，4.4 折叠为范围检查
        //   `sub/cmp/ja`，与 ORIG 逐条全等（探针 /tmp/sfp_p2.cpp fH vs fI）。
        //   语义 = *it ∈ [FT_HP(0xa), FT_ETC2(0x18)] 闭区间，与 if 形等价。
        switch ((unsigned int)*it - 10U)
        {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6:
        case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14:
            return pos;
        default:
            break;
        }
        ++pos;
    }
    return -1;
}

// ORIG T 0x088a3056：ft∈[10,24] → 序号 0..14，缺省 -1。
int BuyUpgradeData::getStatIconIndex(FieldType::T ft) const
{
    // b28d-35：ORIG 为「具名 result 局部（-0x4，初值 -1）+ 每 case
    //   `movl $N,-0x4; jmp` 汇聚尾 `mov -0x4,%eax`」；逐 case return 形使 4.4
    //   直接 `mov $N,%eax`（无 result 槽）⇒ 改 result 赋值 + break 形。
    int result = -1;
    switch (ft)
    {
    case FieldType::FT_HP:   result = 0;   break;   // 0x0a
    case FieldType::FT_DMG:  result = 1;   break;   // 0x0b
    case FieldType::FT_PIC:  result = 2;   break;   // 0x0c
    case FieldType::FT_RCV:  result = 3;   break;   // 0x0d
    case FieldType::FT_SUM:  result = 4;   break;   // 0x0e
    case FieldType::FT_MHT:  result = 5;   break;   // 0x0f
    case FieldType::FT_RGN:  result = 6;   break;   // 0x10
    case FieldType::FT_MSD:  result = 7;   break;   // 0x11
    case FieldType::FT_ASD:  result = 8;   break;   // 0x12
    case FieldType::FT_SDT:  result = 9;   break;   // 0x13
    case FieldType::FT_FDT:  result = 10;  break;   // 0x14
    case FieldType::FT_ETC:  result = 0xb; break;   // 0x15
    case FieldType::FT_LRG:  result = 0xc; break;   // 0x16
    case FieldType::FT_SPL:  result = 0xd; break;   // 0x17
    case FieldType::FT_ETC2: result = 0xe; break;   // 0x18
    default:                 break;
    }
    return result;
}

// ORIG T 0x088a3106（sret 返回 std::string）：FieldType→字符串表条目
// CStringManager::getStr(id)；区间外的 ft 返回空串。
std::string BuyUpgradeData::getStatStringIndex(FieldType::T ft) const
{
    // b28d-solo-advancealtar_misc：ORIG 0x088a3106 = 140 条，每 case 独立
    //   `movl $id,(%esp); call getStr; mov %ebx,%edx; mov %eax,0x4(%esp);
    //   mov %edx,(%esp); call string::operator=(char const*); jmp 共享 sret 尾`
    //   ⇒ 原码是**每 case 独立赋值** `result = CStringManager::getStr(id); break;`
    //   （共享 sret result，非逐臂 return——后者 4.4 展开独立 sret 构造/析构 397 条）。
    //   原「具名 strId + 统一尾调用」形 4.4 只发一次 getStr/op=（73 条）。
    std::string result;
    switch (ft)
    {
    case FieldType::FT_HP:   result = CStringManager::getStr(0x11570); break;  // [objdump 0x88a3134]
    case FieldType::FT_DMG:  result = CStringManager::getStr(0x11571); break;  // [objdump 0x88a3153]
    case FieldType::FT_PIC:  result = CStringManager::getStr(0x11573); break;  // [objdump 0x88a3172]
    case FieldType::FT_RCV:  result = CStringManager::getStr(0x11574); break;  // [objdump 0x88a3191]
    case FieldType::FT_SUM:  result = CStringManager::getStr(0x11575); break;  // [objdump 0x88a31b0]
    case FieldType::FT_MHT:  result = CStringManager::getStr(0x11576); break;  // [objdump 0x88a31cf]
    case FieldType::FT_RGN:  result = CStringManager::getStr(0x1158f); break;  // [objdump 0x88a31ee]
    case FieldType::FT_MSD:  result = CStringManager::getStr(0x11590); break;  // [objdump 0x88a320d]
    case FieldType::FT_ASD:  result = CStringManager::getStr(0x11591); break;  // [objdump 0x88a322c]
    case FieldType::FT_SDT:  result = CStringManager::getStr(0x11592); break;  // [objdump 0x88a324b]
    case FieldType::FT_FDT:  result = CStringManager::getStr(0x11593); break;  // [objdump 0x88a326a]
    case FieldType::FT_ETC:  result = CStringManager::getStr(0x11596); break;  // [objdump 0x88a3286]
    case FieldType::FT_LRG:  result = CStringManager::getStr(0x11572); break;  // [objdump 0x88a32a2]
    case FieldType::FT_SPL:  result = CStringManager::getStr(0x11595); break;  // [objdump 0x88a32be]
    case FieldType::FT_ETC2: result = CStringManager::getStr(0x11596); break;  // [objdump 0x88a32da]
    default:                 return result;            // 跳转表范围外 → 空 string
    }
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
         it != m_rewardMap.end(); it++)
    {
        // b28d-35：ORIG 0x0889b5a8 每字段重新计算 `it->second` 基址
        //   （`call operator->` 后直接 `mov 0x8(%eax)` / 再调一次后 `mov 0xc(%eax)`）
        //   ⇒ P183 禁引用缓存：删除 `const AchievementRewardData& data` 具名缓存。
        // b28d-solo-advancealtar_misc：ORIG 0x0889b5d0 首比较为
        //   `mov 0x8(%eax),%eax; mov %eax,%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`
        //   —— 两侧显式 `(unsigned int)` cast 使 4.4 保留 eax→edx 中转 + 形参入
        //   %eax 的寄存器比较（探针 /tmp/enp4.cpp k1；`(int)` cast 或裸比较恒折
        //   单段 `mov 0x8(%eax),%edx` + 内存比较）。第二比较（int vs int）保持
        //   裸形，与 ORIG `mov 0xc(%eax),%eax; cmp 0x10(%ebp),%eax` 一致。
        if ((unsigned int)it->second.m_difficulty == (unsigned int)diff &&
            it->second.m_stageClearCount == count)
        {
            return const_cast<AchievementRewardData*>(&it->second);
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
    // b28d-solo-advancealtar_misc：ORIG @0x088a2bc0 用**非 const** `_Rb_tree_iterator`
    //   （Ghidra C 实证 begin/end/ne/operator-> 均为非 K 版本）⇒ 原 const_iterator
    //   触发 iterator→const_iterator 转换构造/析构，帧 0x38、end 临时 -0x1c（P96/P270）。
    //   改 iterator 后帧/槽与 ORIG 一致（maxKey -0x10、it -0x14、end 临时 -0xc）。
    //   增量段 ORIG 为 `ppEv`（**前置** ++，无 sret 临时）⇒ 源保持 `++it`。
    int maxKey = 0;
    for (std::map<int, _StarRestFeeByGold>::iterator it = m_starRestMap.begin();
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
    // P208/P163 + P46：switch 复现 ORIG 4.1 决策树（值缓存 %eax + test/cmp 链）。
    // 枚举底层为 unsigned 会吞 -1 case（「case 标号低于类型最小值」告警），
    // switch(int) 保留。b28d-35：case 必须按数值升序书写（P46 叶块排放序），
    // 原 0,1,-1 序使 4.4 产 `je` 反向树 + 叶序 [1,2,0]（ORIG 为 `jne` + 叶序 [0,1,2]）。
    switch (static_cast<int>(diff))
    {
    case -1:                             // -1 → 0
        return DIFF_EASY;
    case DIFF_EASY:                      // 0 → 1
        return DIFF_MEDIUM;
    case DIFF_MEDIUM:                    // 1 → 2
        return DIFF_HARD;
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
// [b28d-solo-advancealtar_misc 判词] N 封顶（N18a 单段 regmove）：
//   fresh .o 350:349（gap 1），p167 调用点差集 CLEAR（57=57，0 missing/extra）；
//   唯一残差 = 首条 put_binary 的 `&data` 实参装载：ORIG 两段 `mov 0xc(%ebp),%eax;
//   mov %eax,%edx`，4.4.6 恒折单段 `mov 0xc(%ebp),%edx`。P248 三档旋钮已试：
//   `const_cast<char*>(reinterpret_cast<const char*>(&data))` 与
//   `reinterpret_cast<char*>(const_cast<AdvanceAltarData*>(&data))` 两 cast 形
//   产物相同（均单段）；形参类型受 mangled 约束（RKNS_16AdvanceAltarDataE）不可异型化；
//   ORIG 的 `mov mem,%eax; mov %eax,%reg` 属 4.1 冗余 regmove（N18a）不可造。
// ============================================================================
void putRidableInfoToPacket(PacketGuard& packet, const AdvanceAltarData& data)
{
    // b28d-35: DIFF→N82（350/349 差 1 条）。ORIG @0x08141781 首条载 0xc(%ebp)，
    //   我方装载序差 1 条（b28d 已 288→1，P183 去缓存后残余为实参装载序）。
    // ORIG 每处直接 `mov 0x8(%ebp)` 传参（无 buf 引用槽）⇒ 不用具名别名。
    // 段 2..6 容器指针 = ORIG 二段初始化槽（`movl $0,-0x10/-0xc` + 赋值，
    // 0x0814184e/0x0814185b 等）；段 1/7 直取（0x081417a3/0x08141bb7）。
    packet.put_binary(reinterpret_cast<char*>(const_cast<AdvanceAltarData*>(&data)), 0x21);   // [objdump 0x8141798]

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
    // [b28d-solo-advancealtar_misc 判词] DIFF→N 封顶（N82 帧/槽族）：
    //   已修：四段循环的 8 个 memcpy 局部 + 3 个 _Slot/_BuyItem/_Reward 默认 ctor
    //   改为 **const 引用视图**直读线格式缓冲（P156 允许），使 p167 差集从
    //   MISSING make_pair<const&,const&>×4 / EXTRA memcpy×8+ctor×3+make_pair<&,&>×4
    //   收敛为 **CLEAR（ORIG 17 = OUR 17，0 missing / 0 extra）**；stages 段删
    //   `const _Stage&` 缓存（P183，ORIG 每处重算 `db.m_stages[i]`）。
    //   残余：帧 0x108 vs ORIG 0xd8（-0x30）——ORIG 对 `db+0x1b0+i*10` 每处重算
    //   （0x088993cc/d1/ee/f3/40b/422/49d/4d5 八处 `add $0x1b0,%eax`，无 rec 缓存）
    //   且 pair/make_pair/insert 的临时槽更少；4.1/4.4 槽复用策略差属 N82 族
    //   （判据：p167 CLEAR + 控制流/调用序全等，仅帧与槽号平移）。
    m_update = 0;                                                  // [objdump 0x88992e6]
    std::memcpy(this, &db, 0x21);                                  // [objdump 0x8899302]

    // stages：i<=0x31，条件/键均读 db.m_stages[i].m_field0（pack 后 db+0x21+i*8）。
    // b28d-solo-advancealtar_misc：ORIG 每处重算 `db.m_stages[i]` 两级寻址
    //   （0x0889939f `mov -0x1c,%edx; add $4; mov 0x1(%eax,%edx,8),%eax` 与
    //   make_pair 两实参各自 `shl $3; add` 重算）⇒ 原码无具名 `_Stage&` 缓存（P183）；
    //   原引用形使 4.4 把指针落槽（-0x48）并多一对 load/store。
    for (int i = 0; i <= 0x31; ++i)                                // [objdump 0x889939f setle]
    {
        if ((int)db.m_stages[i].m_field0 > 0)                      // [objdump 0x889931c jle]
        {
            m_stages.insert(std::make_pair(
                db.m_stages[i].m_field0, db.m_stages[i]));         // [0x8899353 key/value 同址]
        }
    }

    // slots：i<=0xb；记录基 = db+0x1b0+i*10（元素实际起 0x1b1）：
    //   key = 记录.m_byte0（rec+1）、cond int = m_int1（rec+2）、
    //   value = 从 rec+6 起的 10 字节（跨到下一记录头部，忠实 ORIG）
    // b28d-solo-advancealtar_misc：p167 差集（ORIG 17 calls / 我方 28）显示 ORIG 的
    //   make_pair 实例全为 **const 引用**形（`make_pair<unsigned char const&,_Slot const&>`
    //   等 4 个）且无 _Slot/_BuyItem/_Reward 默认 ctor、无 8 个 memcpy ⇒ 原码用
    //   **const 引用视图**直读线格式缓冲（P156「线格式序列化缓冲」允许偏移视图），
    //   原 memcpy 到非 const 局部形多 ctor+memcpy 且实例化非 const make_pair。
    for (int i = 0; i <= 0xb; ++i)                                 // [objdump 0x889952c]
    {
        const char* rec = reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10;
        const int& slotType = *reinterpret_cast<const int*>(rec + 2);   // [objdump 0x88993d1 +2]
        if (slotType == 0)
        {
            continue;                                              // [objdump 0x88993d6 je]
        }
        const unsigned char& slotKey =
            *reinterpret_cast<const unsigned char*>(rec + 1);      // [objdump 0x8899413 key=rec+1]
        const _Slot& slotValue =
            *reinterpret_cast<const _Slot*>(rec + 6);              // [objdump 0x889942a +6]
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
        const int& count = *reinterpret_cast<const int*>(rec + 4); // [objdump 0x889955e +0xd→abs 0x22d]
        if (count <= 0)
        {
            continue;                                              // [objdump 0x8899563 jle]
        }
        const int& itemKey = *reinterpret_cast<const int*>(rec);   // [objdump 0x8899580 +0x9→abs 0x229]
        std::map<int, _BuyItem>* target =
            getBuyItemMap((AdvanceAltarShopType::T)itemKey);       // [objdump 0x889958d]
        if (target == 0)
        {
            continue;                                              // [objdump 0x8899599 je]
        }
        const _BuyItem& itemValue =
            *reinterpret_cast<const _BuyItem*>(rec + 4);           // [objdump 0x88995c6 value=+0xd]
        target->insert(std::make_pair(itemKey, itemValue));
    }

    // rewards：i<=0x1d（数组 29 项 + 第 30 次读取落入尾部 pad，忠实 ORIG 上限）；
    // key/value 同址取 m_rewards[i].m_field0。
    for (int i = 0; i <= 0x1d; ++i)                                // [objdump 0x88996c6]
    {
        const char* rec = reinterpret_cast<const char*>(&db) + 0x480 + i * 5;
        const int& rewardKey = *reinterpret_cast<const int*>(rec + 1);  // [objdump 0x8899644 +1→abs 0x481]
        if (rewardKey <= 0)
        {
            continue;                                              // [objdump 0x8899649 jle]
        }
        const _Reward& rewardValue =
            *reinterpret_cast<const _Reward*>(rec + 1);
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
    // b28d-solo-advancealtar_misc：ORIG @0x0889a930 关键源形证据——
    //   ①`bool ok` 为**函数级单槽**（0x0889a938 `movb $0x0,-0x9(%ebp)` 只在函数头
    //   初始化一次，其余全是 ScanInt(&ok) 传址 + movzbl 回读；原多分支各自
    //   `bool ok = false;` 生成 -0x29/-0x41 多个独立槽）；
    //   ②内层循环**无 phaseDone 标志**（全函数仅一条 movb 写 -0x9）⇒ 原码用
    //   `for(;;){... if (tag=="[/phase]") break; ...}`，原 while+标志多一个 bool 槽。
    bool ok = false;                                               // [objdump 0x0889a938 -0x9]
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
            for (;;)
            {
                std::string tag;
                if (!ScanType(tag, true))
                {
                    return 0x2be;                                  // [objdump 0x889a9ee 路径]
                }
                if (tag == "[/phase]")
                {
                    break;
                }
                else if (tag == "[duration]")
                {
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
// [b28d-solo-advancealtar_misc 判词] DIFF→N 封顶（语义/调用点/控制流全等）：
//   已修①**static 成员**（ORIG 首参 timeLine 读 0x8(%ebp)、无 this；原非 static 声明
//   使 timeLine/defines 落在 0xc/0x10、整帧 +4 错位，见 CDataManager.h 声明处）；
//   ②_PhaseSummary::m_pad 字段化 + 逐字段直写（原 fields[3]+memcpy 多一次 memcpy 调用）；
//   ③`vector<_ActionAssign>& assigns = pit->m_assigns;` 复现 ORIG -0xc 地址缓存；
//   ④两循环后置 `++`（ORIG ppEi 0x0889bede/0x0889bf48 带 sret 临时）与**体内
//   `pit++` + 体末 `++phaseIndex`**（ORIG 0x0889a1d0 位于 add 之后、addl 之前）。
//   残余（N82附/跨版本）：dit(-0x34) 与 summary(-0x40) 槽互换——4.4 聚合对象优先占
//   低槽（summary 最深、dit 次深），4.1 按生命周期穿插（dit -0x40、summary -0x3c）；
//   其余槽（pit -0x2c/end -0x28/m_assigns.end -0x24/defines.end -0x20/ait ppEi -0x1c/
//   pit ppEi -0x18/totalDuration -0x14/phaseIndex -0x10/assigns -0xc/ait -0x30）全等；
//   4.4 多 push esi（后置 ++ sret 跨调用保活）。全头部声明法对非平凡 summary 不可用。
//   p167：MISSING 0；EXTRA = `_PhaseSummary::~_PhaseSummary()`×2 + `_Unwind_Resume`×1
//   —— ORIG(4.1) 对可见空 dtor 不发射调用、4.4 恒 call（P71 跨版本；ORIG 该 dtor
//   符号 0x0889ebc0 存在，按 P12 不可删）。
int StageTimeLineParameter::checkAndSetActionDefine(
    TimeLine& timeLine, const std::map<int, ActionDefine>& defines)
{
    // b28d-solo-advancealtar_misc：ORIG @0x0889a032 全函数**不复用** Phase&
    //   （每处 `pit->m_*` 重新 ptEv：0x0889a061/a185/a1ab），原具名 `Phase& phase`
    //   使 4.4 缓存 phase 指针于 -0xc；改逐点 `pit->m_*` 后 -0xc 复现 ORIG 的
    //   `&pit->m_assigns` 缓存形（`call ptEv; add $8,%eax; mov %eax,-0xc`）。
    //   summary 由 fields[3]+memcpy 改具名字段直写（_PhaseSummary::m_pad 已字段化）。
    //   ORIG 两循环增量段均为 **后置** `operator++(int)`（Phase ppEi 0x0889bede /
    //   _ActionAssign ppEi 0x0889bf48，各带独立 sret 临时）⇒ 源写 `pit++`/`ait++`；
    //   ORIG -0xc 缓存 `&pit->m_assigns`（`call ptEv; add $8,%eax; mov %eax,-0xc`）
    //   ⇒ 原码有 `std::vector<_ActionAssign>& assigns = pit->m_assigns;`（P0 槽往返）。
    // ORIG 外层为「体内 `pit++` + 体末 `++phaseIndex`」（0x0889a1d0 ppEi 位于
    //   `add %eax,-0x14` 之后、`addl $1,-0x10` 之前）⇒ 原码非 for 增量位。
    int totalDuration = 0;                                         // local_18
    int phaseIndex = 0;                                            // local_14
    for (std::vector<Phase>::iterator pit = timeLine.m_phases.begin();
         pit != timeLine.m_phases.end(); )
    {
        std::vector<_ActionAssign>& assigns = pit->m_assigns;
        for (std::vector<_ActionAssign>::iterator ait = assigns.begin();
             ait != assigns.end(); ait++)
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

        {
            _PhaseSummary summary;                                 // [objdump ctor -0x3c]
            summary.m_startDuration = totalDuration;               // +0x04（ORIG 先写）
            summary.m_phaseIndex = phaseIndex;                     // +0x00
            summary.m_phaseType = pit->m_field4;                   // +0x08（ptEv 形）
            timeLine.m_summary.m_phaseSummarys.push_back(summary); // push param_1+0x10
        }

        totalDuration += (int)pit->m_field0;                       // [objdump 本轮时长累加]
        pit++;
        ++phaseIndex;                                              // local_14
    }
    timeLine.m_summary.m_field0 = totalDuration;                   // [objdump *(int*)(param_1+0xc)]
    return 1;
}

// ---- 布局断言（本批次相关）-------------------------------------------------
typedef char assert_slotfee_0x4[(sizeof(_StarRestFeeByGold) == 4) ? 1 : -1];

}  // namespace advancealtar
