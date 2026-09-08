// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CBoosterGage.h"  // BoosterGageData 权威布局（须先于 CDataManager.h）
#include "CDataManager.h"
#include "cMyTraceNoop.h"  // add_lottery_needMoney 命中 trace（ORIG 0x83650f5）
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include "NPCScript.h"   // CNPCScript 完整类型（_destroy 中 delete）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（唯一声明点）
#include "DNFLexWrapper.h"   // ScanType/ScanInt/ScanStr/ScanFloat/loadRDARScriptFile/
                             // unloadRDARScriptFile（RDAR 脚本扫描接口）
#include "Statuepredicate.h"  // pieceQuestRewardStatus 合并谓词（getRewardStatus）
#include "CDungeon.h"        // advancealtar::{StageDifficulty,RewardItemType}
                             // （importAchievementRewardParameter 解析用）
#include <cstring>

// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 0x088bbd77 T（定义 STScriptFileList.cpp；DNFLexWrapper.h 未含此声明）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);
// 频道类型 ∈ {4,6,5,7,8,9,10,13,14,15,16} 返回真，否则假。
// 调用侧（GetPvPChannelNumber）按 bool 语义 test %al 使用；定义者按 ORIG
// cmp/je 链 + mov $1/mov $0 分支形态实现即可（int/bool 返回符号相同）。
bool IS_PVP_CHANNEL(int channelType);
#include <algorithm>     // std::find（CWorldMap::hasDungeon）

// ============================================================================
// 脚本 POD 生命周期（ORIG 语义，见 docs/class_func_reports 与 ORIG 反汇编）。
// 各 ctor 构造 STL 容器成员（必要时调 clear()），dtor 逆序析构。
// ============================================================================

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}

QuestParameterScript::QuestParameterScript()
{
    clear();
}
QuestParameterScript::~QuestParameterScript() {}

void QuestParameterScript::clear()
{
    m_map0.clear();
    m_vec24.clear();
    m_vec30.clear();
    m_map3c.clear();
    m_map54.clear();
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
}

// ============================================================================
// QuestParameterScript 奖励段查询（ORIG T 0x08a6bdc6 / 0x08a6be66）。
// getRewardLevel：遍历 m_map3c（map<int,int>），取首个 second<=level 的
// first（初始 0）；getRewardStatus：getRewardLevel(level) 后遍历 m_map54 中
// key<=level 的各 vector，按 Statuepredicate（+0x0/+0x4 两 int）合并：
// 命中段累加 m_value（+0x8 float），否则 push_back（sret 返回）。
// ============================================================================
int QuestParameterScript::getRewardLevel(int level)
{
    int result = 0;
    std::map<int, int>::iterator it = m_map3c.begin();
    // ORIG 0x8a6be2a 调 ppEi（后缀 it++，0x8(%esp)=$0，返回值落废弃临时
    // -0x10），非前缀 ++it。
    for (; it != m_map3c.end(); it++)
    {
        if (it->second <= level)
        {
            result = it->first;
        }
    }
    return result;
}

// [b28d-solo-CDataManagerScripts] DIFF→N：N86/N93 —— 帧 0x7c vs 0x8c + 迭代器/临时槽序（已按 P96/P28 非 const + 后置 ++；4.1 两段拷贝 vs 4.4 直构）；ORIG 0x08a6be66。
std::vector<pieceQuestRewardStatus> QuestParameterScript::getRewardStatus(
    int level)
{
    std::vector<pieceQuestRewardStatus> result;
    int rewardLevel = getRewardLevel(level);
    if (rewardLevel == -1)
        return result;

    for (std::map<int, std::vector<pieceQuestRewardStatus> >::iterator it =
             m_map54.begin(); it != m_map54.end(); it++)
    {
        if (rewardLevel < it->first)
            break;

        std::vector<pieceQuestRewardStatus>& section = it->second;
        for (std::vector<pieceQuestRewardStatus>::iterator sit =
                 section.begin(); sit != section.end(); sit++)
        {
            Statuepredicate pred(*sit);
            std::vector<pieceQuestRewardStatus>::iterator found =
                std::find_if(result.begin(), result.end(), pred);
            if (found != result.end())
            {
                found->m_value += sit->m_value;
            }
            else
            {
                result.push_back(*sit);
            }
        }
    }
    return result;
}

// ---- GuildParameterScript（ORIG C1 0x837058a / clear 0x83704ae / D1 0x837d542）----
GuildParameterScript::GuildParameterScript()
{
    clear();
}
GuildParameterScript::~GuildParameterScript() {}
void GuildParameterScript::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_map10.clear();
    m_field28 = 0;
    m_vec2c.clear();
    m_vec38.clear();
    m_vecE4.clear();
    m_vecF0.clear();
    m_fieldFC = 0.0;
    m_field104 = 0;
    m_field108 = 0;
    m_field10c = 0;
    m_field110 = 0.0;
    memset(m_levelUpCost, 0, sizeof(m_levelUpCost));  // ORIG：+0x5c memset 0x88
}

// ORIG C1 0x8372480：仅构造 +0x08 map 后调 clear()（+0x1c call；+0/+4 由 clear 内清零）。
AccountCargoScript::AccountCargoScript()
    : m_upgradeMap()
{
    clear();
}
AccountCargoScript::~AccountCargoScript() {}

// ---- PvPSkillTreeParameterScript（ORIG C1 0x8371eee / clear 0x8371e5a / D1 0x837da02）----
PvPSkillTreeParameterScript::PvPSkillTreeParameterScript()
{
    clear();
}
PvPSkillTreeParameterScript::~PvPSkillTreeParameterScript() {}

void PvPSkillTreeParameterScript::clear()
{
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_field48 = 0;
    m_field4c = 0;
    m_field50 = 0;
    m_map54.clear();
}
// ============================================================================
// getPvPSkillPointNext（ORIG T 0x08a5df0e，const）：
// m_map18[grade] 与 m_map18[grade-1] 中 (job*5+firstGrow) 的投入点数差，
// 再减 m_map30[(job,firstGrow)] 的 m_pointCost。secondGrow 不参与
// （ORIG 写入 0 后仅用于 key 第三字节，恒 0）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：带参 `PvPSkillTreeKey key(job,firstGrow,secondGrow)` + `secondGrow=0` 写回形参槽已落（ORIG C1Ehhh 0x08a5e768）；残余全帧槽序平移 4 + key 槽 -0x3f vs ORIG -0x1b（N93/P112 声明序不可控）；ORIG 0x08a5df0e。
int PvPSkillTreeParameterScript::getPvPSkillPointNext(
    int job, int firstGrow, int secondGrow, int grade) const
{
    int points = 0;
    std::map<int, std::map<int, int> >::const_iterator gradeIt =
        m_map18.find(grade);
    if (gradeIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            gradeIt->second.find(job * 5 + firstGrow);
        if (slotIt != gradeIt->second.end())
            points = slotIt->second;
    }

    int prevPoints = 0;
    std::map<int, std::map<int, int> >::const_iterator prevIt =
        m_map18.find(grade - 1);
    if (prevIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            prevIt->second.find(job * 5 + firstGrow);
        if (slotIt != prevIt->second.end())
            prevPoints = slotIt->second;
    }

    // ORIG 0x08a5e166：`movl $0x0,0x14(%ebp)`（secondGrow 写回形参槽）后
    // 三字节实参调 PvPSkillTreeKey::C1Ehhh（0x08a5e768）⇒ 带参构造形。
    secondGrow = 0;
    int cost = 0;
    PvPSkillTreeKey key(job, firstGrow, secondGrow);
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator costIt =
        m_map30.find(key);
    if (costIt != m_map30.end())
        cost = costIt->second.m_pointCost;

    return (points - prevPoints) - cost;
}

// ---- IndependentDropParameterScript（ORIG C1 0x8372d46 / clear 0x8372dfa / D1 0x837daac）----
IndependentDropParameterScript::IndependentDropParameterScript()
{
    clear();
}
IndependentDropParameterScript::~IndependentDropParameterScript() {}
void IndependentDropParameterScript::clear()
{
    m_vecC.clear();  // ORIG clear 仅清 +0x0c vector
}

// ---- stMercenarySystemInfo（ORIG C1 0x8373328 / D1 0x837db42）----
stMercenarySystemInfo::stMercenarySystemInfo()
    : m_field0(0xe10), m_field4(0xc8), m_vec8(), m_vec14(), m_vec20(),
      m_str2c(), m_vec30(), m_field3c(0), m_vec40()
{
}
stMercenarySystemInfo::~stMercenarySystemInfo() {}

// ---- channel_script_t ----
channel_script_t::channel_script_t() : m_channelDungeonMap(), m_channelMap() {}
channel_script_t::~channel_script_t() {}

// ORIG C1 0x8371694：构造 vector +0x0/map +0xc 后各自 clear()（+0x27/+0x35 call）。
stBroadCastItemScript::stBroadCastItemScript() : m_vec0(), m_mapC()
{
    m_vec0.clear();
    m_mapC.clear();
}
stBroadCastItemScript::~stBroadCastItemScript() {}

// ORIG isUnCommonItem 0x8918d9c：m_mapC.find(itemIdx) != end()
bool stBroadCastItemScript::isUnCommonItem(int itemIdx) const
{
    // ORIG 0x8918da8：find sret→具名 it(-0x10，声明即构造)；end 临时 -0xc；
    // 0x8918de6 ne(it,end) 后 je→mov$0/落空→mov$1 ⇒ 显式 if-return 形。
    std::map<int, std::vector<int> >::const_iterator it = m_mapC.find(itemIdx);
    if (it != m_mapC.end())
        return true;
    return false;
}

// ORIG checkCommonItem 0x8918cdc：遍历 m_vec0 找 itemIdx
// [N类·跨版本豁免] 语义与 ORIG 一致（it!=end 循环内 *it==itemIdx 命中返 1）。ORIG
// 59 vs 我方 39 指令差为 gcc4.1.2 的 dead-bool XOR 哨兵（local_11^=1 + 嵌套 while +
// int* 物化重载，见 ## 汇编 0x08918ce2 xorb/0x08918d7a）——非源码可表达；范式库
// 「负例 N 类·跨版本」。证据 ORIG 0x08918cdc。
// [b28d-solo-CDataManagerScripts] DIFF→N：跨版本 dead-bool XOR 哨兵（ORIG 0x8918d8b `xorb $0x1,-0xd(%ebp); cmpb $0,-0xd` 栈槽物化 + 入口 `movb $0,-0xd; cmpb; jne`），P168a④ 判据：栈槽物化点位非源码可表达；ORIG 0x08918cdc。
bool stBroadCastItemScript::checkCommonItem(int itemIdx) const
{
    for (std::vector<int>::const_iterator it = m_vec0.begin(); it != m_vec0.end(); it++)
    {
        if (*it == itemIdx)
        {
            return true;
        }
    }
    return false;
}

// ORIG checkUnCommonNestedItem 0x8918dfe：m_mapC.find(itemIdx) 命中后遍历其子 vector 找 subItemIdx
// [N类·跨版本豁免] 语义与 ORIG 一致（find!=end → 子 vector 迭代 *vit==subItemIdx 返 1）。
// ORIG 87 vs 我方 69 差同 checkCommonItem：dead-bool XOR 哨兵（0x08918f03 xorb + 嵌套
// while + operator!=/operator* 双缓存续算），非源码可表达；范式库「跨版本」。证据 0x08918dfe。
// [b28d-solo-CDataManagerScripts] DIFF→N：同 checkCommonItem —— 跨版本 dead-bool XOR 哨兵（栈槽物化）+ operator->/operator!= 双缓存形差，非源码可表达；ORIG 0x08918dfe。
bool stBroadCastItemScript::checkUnCommonNestedItem(int itemIdx, int subItemIdx) const
{
    std::map<int, std::vector<int> >::const_iterator it = m_mapC.find(itemIdx);
    if (it != m_mapC.end())
    {
        const std::vector<int>& v = it->second;
        for (std::vector<int>::const_iterator vit = v.begin(); vit != v.end(); vit++)
        {
            if (*vit == subItemIdx)
            {
                return true;
            }
        }
    }
    return false;
}

// [b28d-solo-CDataManagerScripts] AE：map C1 + CSlotBoundChecker C1（+0x18 真实内嵌成员）+ clear；fresh check_tu_routed clean（39=39）；ORIG 0x083738a2。
eventReward::eventReward() : m_map0()
{
    // ORIG C1 0x83738a2：构造 map + CSlotBoundChecker 后 clear map。
    // CSlotBoundChecker 重建为静态数组实现（无 per-instance 成员），+0x18 保持 pad。
    m_map0.clear();
}
// [b28d-solo-CDataManagerScripts] AE：map D1 + CSlotBoundChecker D1（+0x18 真实内嵌成员，ORIG 0x0837dfb2 调 _ZN17CSlotBoundCheckerD1Ev）；fresh check_tu_routed clean（27=27）；ORIG 0x0837dfb2。
eventReward::~eventReward()
{
    // ORIG D1 0x837dfb2：析构 map + CSlotBoundChecker（后者静态实现无成员，跳过）
}

// ---- ConditionLevelChkDungeon / FairPvPStatScript / RegenerationROI ----
// ORIG C1 0x837c9ac / D1 0x837c9c0（本 TU 唯一 owner）：仅构造/析构 +0x00 的
// vector<int>；objdump 直转，标量成员无动作。
ConditionLevelChkDungeon::ConditionLevelChkDungeon()
    : m_unableDungeons()
{
}
ConditionLevelChkDungeon::~ConditionLevelChkDungeon() {}
// ORIG C1 0x83724f0：map 构造后 clear()；D1 仅析构 map；clear 0x83724c8 清空 map。
FairPvPStatScript::FairPvPStatScript()
{
    clear();   // ORIG C1 0x83724f0 调本类 clear()（W 0x83724c8）
}
FairPvPStatScript::~FairPvPStatScript() {}
void FairPvPStatScript::clear() { m_map0.clear(); }
// ORIG _ZN28RandomOptionItem_RegeneratorC1Ev 0x85f971a T：四容器构造后 +0x48=0。
RandomOptionItem_Regenerator::RandomOptionItem_Regenerator()
    : m_map0(), m_map18(), m_vec30(), m_vec3c()
{
    m_field48 = 0;
}

RegenerationROI::RegenerationROI()
    : m_regenerator()
{
    // ORIG C1 0x85f8486：构造 +0x04 regenerator 后 new CMTRand(0x9c8) 存 +0x00；
    // 不初始化 +0x50。
    m_regeneratorPtr = new CMTRand();
}
RegenerationROI::~RegenerationROI()
{
    // ORIG D1 0x85f8506：+0x00 非空则 delete，置 0，析构 +0x04 regenerator
    if (m_regeneratorPtr != 0)
    {
        ::operator delete(m_regeneratorPtr);
    }
    m_regeneratorPtr = 0;
}
// ORIG C1/D1：map<int,Value> 默认构造/析构（原 pad 占位时的 ZERO_POD 已随真实布局移除）。
stSeriaBlessingScript::stSeriaBlessingScript() {}
stSeriaBlessingScript::~stSeriaBlessingScript() {}

// ORIG W 0x822af22 is_possible_lv：FindOneByLv(0x822aebe 存 lv) 谓词
// （0x822aecc）：min==max → lv==min；否则 min<=lv<max。
bool stSeriaBlessingScript::is_possible_lv(int lv)
{
    // ORIG 0x822af22：pred 入口构造(-0x18)；find_if 内联实参（右到左求值：
    // 0x822af47 end()→临时 -0x14 先、0x822af5c begin()→临时 -0x10 后）；
    // 外层 ne 0x822afa9 的 this=第二 end() 临时(-0xc)、rhs=find_if 结果(-0x1c)
    // ⇒ 源码方向 end() != find_if(...)。
    std::map<int, Value>::iterator found =
        std::find_if(m_map0.begin(), m_map0.end(), FindOneByLv(lv));
    return m_map0.end() != found;
}

// ORIG C1 0x83724f0 / FindCharacStat 0x8693c66：map.find(CharacterKind(job,grow))，
// 命中返回 &it->second，未命中返回 0（迭代器 != end 判定）。
CharacterStatus* FairPvPStatScript::FindCharacStat(int job, int growType)
{
    std::map<CharacterKind, CharacterStatus>::iterator it =
        m_map0.find(CharacterKind((unsigned char)job, (unsigned char)growType));
    if (it != m_map0.end())
        return &it->second;
    return 0;
}

// ORIG W 0x822b07e get_break_away_section_data(level)：sret；
// 返回最后一个 elem.m_field1 <= level 的段拷贝（memmove 3 字节；
// 一旦 m_field1>level 即退出，未命中返回清零段）。
STBreakAwaySection STGrowthPowerData::get_break_away_section_data(int level)
{
    STBreakAwaySection out;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it)
    {
        if ((*it).m_field1 > level)   // ORIG 用 operator*（de）
            break;
        memmove(&out, &*it, sizeof(STBreakAwaySection));
    }
    return out;
}

// ORIG W 0x822afd6 get_break_away_section_low_level(lv)：返回最后一个
// m_field1<=lv 段的 m_field1；首个 m_field1>lv 即返回 -1（循环内直返）。
int STGrowthPowerData::get_break_away_section_low_level(int lv)
{
    int result = -1;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it)
    {
        if ((*it).m_field1 > lv)   // ORIG 用 operator*（de）
            return result;
        result = (*it).m_field1;
    }
    return result;
}

// ORIG C1 0x8371194 / 0x83714c6：map 构造后 clear()。
stLevelUpRewardItemScript::stLevelUpRewardItemScript()
{
    clear();   // ORIG C1 0x8371194 调本类 clear()（W 0x083711d6）
}
stLevelUpRewardItemScript::~stLevelUpRewardItemScript() {}
stStepRewardStackableItemScript::stStepRewardStackableItemScript()
{
    clear();   // ORIG C1 0x83714c6 调本类 clear()（W 0x08371508），而非内联 map::clear
}
stStepRewardStackableItemScript::~stStepRewardStackableItemScript() {}
stGrowthCapsuleScript::stGrowthCapsuleScript()
{
    // ORIG C1 0x0837114a：五个 int 成员逐个清零（非 memset）。
    m_maxGageExp = 0;
    m_gagePercent = 0;
    m_capsuleId = 0;
    m_capsuleExp = 0;
    m_capsuleLimitLv = 0;
}
CLottery_NeedMoney::CLottery_NeedMoney() : m_needMoneyMap() {}
CLottery_NeedMoney::~CLottery_NeedMoney() {}

int CLottery_NeedMoney::isNeedMoney(unsigned int itemIdx) const
{
    static std::map<unsigned int, unsigned int>::const_iterator _itr;  // ORIG _ZZNK…isNeedMoneyEjE4_itr
    _itr = m_needMoneyMap.find(itemIdx);
    // ORIG 0x83651d2/0x83651e8：end() 临时(-0xc)为 ne 的 this、&静态 _itr 为
    // rhs ⇒ 源码方向 end() != _itr；命中 0x83651f1 ptEv 后 `mov 0x4(%eax),%eax`
    // 值直返（int 透传，无 bool 收窄），未命中 je→0x8365202 mov$0 尾岛。
    if (m_needMoneyMap.end() != _itr)
        return _itr->second;
    return 0;
}

void CLottery_NeedMoney::add_lottery_needMoney(unsigned int itemIdx, unsigned int cost)
{
    static std::map<unsigned int, unsigned int>::iterator _itr;  // ORIG _ZZN…add_lottery_needMoneyEjjE4_itr
    // ORIG 0x83650b2：end() 先于 find 调用（sret 槽 -0x28）；find 结果临时
    // -0x2c 落静态 _itr（0x83650d6-0xd9）；0x83650ec ne(this=_itr, rhs=&end)。
    // 命中 fall-through trace（cMyTraceNoop() 临时 -0x21 @0x83650f5-0x836510e，
    // 串 "already exist item index!!"@0x8c33253），je→0x8365115 insert。
    std::map<unsigned int, unsigned int>::iterator e = m_needMoneyMap.end();
    _itr = m_needMoneyMap.find(itemIdx);
    if (_itr != e)
    {
        cMyTraceNoop()("already exist item index!!");
    }
    else
        m_needMoneyMap.insert(std::make_pair(itemIdx, cost));
}

channel_info_t* channel_script_t::getChannelInfo(unsigned char channel, unsigned int idx) const
{
    // ORIG 0x88dcd44/0x88dcd4f：入口按"内层(-0x1c)先、外层(-0x20)后"顺序默认
    // 构造两个迭代器；find 结果落临时 -0x30 再单字赋值（0x88dcd80-0x88dcd83、
    // 0x88dcde8-0x88dcdeb）⇒ 声明+赋值两段式，inner 声明在前。
    std::map<int, channel_info_t>::const_iterator itInner;
    std::map<int, std::map<int, channel_info_t> >::const_iterator itOuter;
    itOuter = m_channelMap.find((int)channel);
    if (itOuter == m_channelMap.end())
        return 0;
    itInner = itOuter->second.find((int)idx);
    if (itInner == itOuter->second.end())
        return 0;
    return const_cast<channel_info_t*>(&itInner->second);
}

int channel_script_t::GetPvPChannelNumber(unsigned char channel, unsigned int channelNo) const
{
    // ORIG 0x088dcc28：外层 map(+0x18) 以 channel(serverNo) 查，未命中返回 -1；
    // 内层按 channelNo 升序遍历，每遇 PVP 频道（IS_PVP_CHANNEL(m_channelType)）
    // 计数 +1（-1 起步），命中 channelNo 即返回当前计数；遍历完未命中返回 -1。
    std::map<int, std::map<int, channel_info_t> >::const_iterator itOuter =
        m_channelMap.find((int)channel);
    if (itOuter == m_channelMap.end())
        return -1;
    const std::map<int, channel_info_t>* pInner = &itOuter->second;
    int pvpIndex = -1;
    for (std::map<int, channel_info_t>::const_iterator it = pInner->begin();
         it != pInner->end(); ++it)
    {
        if (IS_PVP_CHANNEL(it->second.m_channelType))
            ++pvpIndex;
        // ORIG 0x88dccea：mov (%eax),%eax; cmp 0x10(%ebp),%eax（参数槽直折入 cmp，
        // 无寄存器中转 ⇒ 不加 (int) cast）
        if (it->first == channelNo)
            return pvpIndex;
    }
    return -1;
}

// [b28d-solo-CDataManagerScripts] AE：①拆两个独立 null 守卫（P133，删 || 合并短路）；②删 vector 引用缓存（P183 逐点重读 info->m_pBonusRates）；③size_t i 头部前置 ⇒ info/-0xc、i/-0x10 与 ORIG 对齐；ORIG 0x088dce3c。
float channel_script_t::getBonusRate(unsigned char channel, unsigned int channelNo,
                                     unsigned int dungeonIndex) const
{
    // ORIG 0x088dce3c：getChannelInfo → 遍历 m_pBonusRates（vector<int>），命中则
    // 返回 (float)m_bonusRate / 1000.0f；否则 0.0f。
    size_t i;
    channel_info_t* info = getChannelInfo(channel, channelNo);
    if (info == 0)
        return 0.0f;
    if (info->m_pBonusRates == 0)
        return 0.0f;
    for (i = 0; i < info->m_pBonusRates->size(); ++i)
    {
        if (info->m_pBonusRates->at(i) == dungeonIndex)
            return (float)info->m_bonusRate / 1000.0f;
    }
    return 0.0f;
}

// ORIG T 0x08581680：hash_map 命中返回 CNPCScript*，未命中返回 0。
CNPCScript* CNPCScriptList::find(unsigned int idx) const
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator it =
        m_npcMap.find(idx);
    if (it == m_npcMap.end())
        return 0;
    return it->second;
}

// ORIG _ZNK14CNPCScriptList16getMaxFavorValueEv W 0x8582364：
// movzwl 0x14(%eax)（m_npcCommon.m_maxFavorExp）+ movzwl %ax 零扩展，int 返回。
// ORIG _ZNK14CNPCScriptList13getFavorLevelEt 0x85816e4：favorExp 低于上限时按
// m_favorExp[1..3] 阈值表（ushort 无符号比较）返回 1..3，否则 4。
// 【P210】定义前置：调用者须在 getMaxFavorValue 定义之前，否则 4.4 按
// callee 帧精确继承 outgoing，0x34 保守大帧塌缩。
int CNPCScriptList::getFavorLevel(unsigned short favorExp) const
{
    // ORIG 0x8581701-0x8581703：cmp %eax,%ebx; setge ⇒ `favorExp >= max` 正向
    // 早退 4（0x858170a 尾岛），循环体内 0x8581728 直接 cmp 参数槽 -0x1c。
    if (favorExp >= getMaxFavorValue())
        return 4;
    for (int i = 1; i <= 3; ++i)
    {
        if (m_npcCommon.m_favorExp[i] >= favorExp)
            return i;
    }
    return 4;
}

// ORIG _ZNK14CNPCScriptList18getFavorLevelValueEj 0x858174e：level 0..3 返
// m_favorExp[level]（level 0 返 0），越界（>3）返 getMaxFavorValue()。
// [b28d-solo-CDataManagerScripts] AE：①返回类型 unsigned short（ORIG 0x0858177d
// 单条 `movzwl` 直返；int 返回 4.4 恒多一条 `movzwl %ax,%eax`，P9/探针
// /tmp/b28d_s160_p1.cpp 实证）；②本定义置于 getMaxFavorValue 之前（P210：callee
// 定义在后 ⇒ 4.4 走保守 `sub $0x18`，与 ORIG 0x08581751 一致）；ORIG 0x0858174e。
unsigned short CNPCScriptList::getFavorLevelValue(unsigned int level) const
{
    if (level > 3)
        return getMaxFavorValue();
    if (level == 0)
        return 0;
    return m_npcCommon.m_favorExp[level];
}

int CNPCScriptList::getMaxFavorValue() const
{
    return m_npcCommon.m_maxFavorExp;
}

// ORIG _destroy 0x8581784：遍历 hash_map delete 各 CNPCScript*（调 D1 + operator delete），
// 再 clear()。
// [b28d-solo-CDataManagerScripts] AE：①缓存 end 转换构造 `const_iterator e(m_npcMap.end())`（P13）；②无条件 `delete it->second`（P141 删显式判空，消 -0xc 槽）⇒ 帧/槽与 ORIG 对齐；ORIG 0x08581784。
void CNPCScriptList::_destroy()
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        it(m_npcMap.begin());
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        e(m_npcMap.end());
    while (it != e)
    {
        delete it->second;
        ++it;
    }
    m_npcMap.clear();
}

CNPCScriptList::~CNPCScriptList()
{
    // ORIG D1 0x83762a8：_destroy() + stNPCCommonData_t D1 + hash_map D1
    _destroy();
}

namespace WongWork
{
CItemGeneratorMgr::~CItemGeneratorMgr() {}
void CItemGeneratorMgr::setDropRatio(float ratio)
{
    // ORIG 0x8534278：遍历 i=0..7，虚槽 +0x00（setDropRatio，权威头已落地）。
    // ORIG 0x8534287-0x85342b3：m_generators[i] 被读两遍（0x8534293 取 vtable、
    // 0x85342a6 再取作 this），无具名 gen 局部 ⇒ i 独占 -0xc。
    for (int i = 0; i < 8; ++i)
        m_generators[i]->setDropRatio(ratio);
}
// getGenerator 已由 CItemGeneratorMgr.cpp 真实定义，不重复。
}

WongWork::CItemGeneratorMgr* CDataManager::getItemGenerator()
{
    return m_itemGeneratorMgr;
}
// get_event_script_mng 已由 CDataManager.cpp 定义，不重复。
#undef ZERO_POD

// ============================================================================
// AccountCargoScript 查询（ORIG 0x88c8056 / 0x88c80ba / 0x88c8124）。
// 全部基于 +0x8 std::map<int, UpgradeInfo>：
//   GetCreateInfo：begin() 的 value（首个升级档），空 map 返回 0；
//   GetCurrUpgradeInfo(idx)：find(idx) 的 value，未找到返回 0；
//   GetNextUpgradeInfo(idx)：find(idx) 的下一项 value，无下一项返回 0。
// 返回 void* 以匹配 CAccountCargo.cpp 的 asm 别名调用。
// ============================================================================
void* AccountCargoScript::GetCreateInfo()
{
    // ORIG 0x88c806c begin()→临时 -0x10；0x88c8084 end()→临时 -0xc；
    // 0x88c8099 ne(&it,&end)：无 empty() 调用。命中 0x88c80ad add $4 直返；
    // je→0x88c80b2 mov$0 尾岛。
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.begin();
    if (it != m_upgradeMap.end())
        return &it->second;
    return 0;
}

void* AccountCargoScript::GetCurrUpgradeInfo(int idx)
{
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.find(idx);
    // ORIG 0x88c80f1 ne + 命中 fall-through；je→0x88c811d mov$0 尾岛
    if (it != m_upgradeMap.end())
        return &it->second;
    return 0;
}

void* AccountCargoScript::GetNextUpgradeInfo(int idx)
{
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.find(idx);
    // ORIG 0x88c8149-0x88c814c：find 后立即整字复制 nxt(-0x14→-0x18)，
    // ++nxt 在首个 it != end(0x88c8174 ne) 命中块内（0x88c8183 ppEv(-0x18)）；
    // 两级均 ne+命中 fall-through，je→mov$0 尾岛 0x88c81c6/0x88c81cd。
    std::map<int, UpgradeInfo>::iterator nxt = it;
    if (it != m_upgradeMap.end())
    {
        ++nxt;
        if (nxt != m_upgradeMap.end())
            return &nxt->second;
        return 0;
    }
    return 0;
}

// ============================================================================
// SecretShopScript::GetNpcIdx / GetNpcByDungeonIdx / GetNpcByDungeonLev
// （ORIG 0x8a82998 / 0x8a828f8 / 0x8a8293c）。
// GetNpcIdx：遍历 equal_range 区间，累加 stDungeonNpc::m_rate（+0x8），
// 首次累计 > target 时输出 m_npcIdx（+0x4）并返回 1，遍历完返回 0。
// ============================================================================
bool SecretShopScript::GetNpcIdx(
    int& out, int target,
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator>& range)
{
    int sum = 0;
    std::multimap<int, stDungeonNpc>::iterator it = range.first;
    while (it != range.second)
    {
        stDungeonNpc& npc = it->second;
        sum += npc.m_rate;
        if (sum > target)
        {
            out = npc.m_npcIdx;
            return 1;
        }
        ++it;
    }
    return 0;
}

bool SecretShopScript::GetNpcByDungeonIdx(int& out, int a, int b)
{
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator> range =
        m_npcMap.equal_range(b);
    return GetNpcIdx(out, a, range);
}

bool SecretShopScript::GetNpcByDungeonLev(int& out, int a, int b)
{
    // ORIG 0x8a8293c：GetLevelIdx(b) → +0x18（m_levelNpcMap）equal_range →
    // GetNpcIdx（返回其 char 结果）。
    int levelIdx = GetLevelIdx(b);
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator> range =
        m_levelNpcMap.equal_range(levelIdx);
    return GetNpcIdx(out, a, range);
}

// ============================================================================
// DeleteInvalidItemScript 生命周期与查询。
// C1/D1（0x8ac1d40/0x8ac1dce）：构造 string +0x0/string +0x4/map +0x8 后
// clearScript()；IsInvalidItem2Delete（0x8ac2096）：map.find(idx) != end。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemScript()
{
    // ORIG C1 0x8ac1d40：构造 string/map 后调 clearScript()（+0x35 call）
    clearScript();
}
// ORIG D1 0x8ac1dce W：clearScript() 后 string +0x0/+0x4 与 map +0x8 由编译器按成员序析构
// （P12 真实析构，非空桩——成员析构仍生成）。
DeleteInvalidItemScript::~DeleteInvalidItemScript()
{
    // ORIG D1 0x8ac1dce：先调 clearScript()（+0x0e call）再析构成员
    clearScript();
}

// ORIG 0x08ac2066 T：清 +0x00/+0x04 两 string 与 +0x8 map（loadScript 前置）。
void DeleteInvalidItemScript::clearScript()
{
    m_field0.clear();
    m_field4.clear();
    m_deleteMap.clear();
}

bool DeleteInvalidItemScript::IsInvalidItem2Delete(int idx)
{
    // ORIG 0x08ac20e0：call operator== ; test %al,%al; je <mov $1> / mov $0;jmp —
    // 0/1 物化 je 岛（test 有 je 消费者，判据③→②直分支 if 形，非 !(x) sete 归一）。
    std::map<int, DeleteInvalidItemData>::iterator it = m_deleteMap.find(idx);
    if (it == m_deleteMap.end())
        return false;
    return true;
}

// ORIG 0x8ac20f8：map.find(idx)；命中 → rewardIdx=data.m_field0、rewardCnt=data.m_field4。
bool DeleteInvalidItemScript::GetRewardItemInfo(int idx, int& rewardIdx,
                                                int& rewardCnt)
{
    // ORIG 用非 const _Rb_tree_iterator（find/operator==/operator-> 全非 const 重载，
    // 0x08ac238a/23dc/23f0）；const_iterator 会换 callee 并加宽 we-larger 9。
    std::map<int, DeleteInvalidItemData>::iterator it =
        m_deleteMap.find(idx);
    if (it == m_deleteMap.end())
        return false;
    // ORIG 0x8ac2152：ptEv 读 second+0x4 / second+0x8 —— rewardIdx=m_field4、rewardCnt=m_field8
    //（m_field0 为脚本 idx，与 map key 重复；loadScript 填充 data[0]=idx,data[4]=rewardIdx,data[8]=rewardCnt）
    rewardIdx = it->second.m_field4;
    rewardCnt = it->second.m_field8;
    return true;
}

// ORIG 0x8ac2180：返回脚本 +0x00/+0x04 两字符串（发件人名/正文模板）。
void DeleteInvalidItemScript::GetMailInfo(std::string& title,
                                          std::string& body) const
{
    title = m_field0;
    body = m_field4;
}
// ============================================================================
// createStackableLotteryInfo（ORIG 0x89bacc1）：与 createLotteryInfo 同型，
// 但前两元素写入 m_defaultIdx/m_defaultCount，之后每三元素为一组
// (item, count, weight) 写入 m_items/m_counts。
// ============================================================================
// [b28d-19] AE（P28 后置 it++ 修复：p167 MISSING operator++(int) 归零，130=130）；ORIG 0x089bacc1。
void createStackableLotteryInfo(LotteryInfo* info, std::vector<int>* source)
{
    info->clear();
    std::vector<int>::iterator it = source->begin();
    info->m_defaultIdx = *it;
    it++;
    info->m_defaultCount = *it;
    it++;
    while (it != source->end())
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        if (it == source->end())
            break;
        p.second = *it;
        it++;
        if (it == source->end())
            break;
        info->m_counts.push_back(*it);
        it++;
        info->m_items.push_back(p);
    }
}

// ============================================================================
// createGlobalEffectInfo（ORIG 0x89bae94）：源为 int 三元组
// (field0, field4, rate*10000)；size 非 3 的倍数返回 0。修正了原桩错误的
// vector<char>& 形参（ORIG mangled 为 vector<stGlobalEffect_t>&）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①CStackableItem.h stGlobalEffect_t m_field4→float / m_field8→int（R2 广播，sizeof/偏移/mangled 不变）；②赋值序 field0→field4(fstps)→field8 + 后置 `*it++` ⇒ 逐条对齐；ORIG 0x089bae94。
int createGlobalEffectInfo(std::vector<stGlobalEffect_t>& out,
                           std::vector<int>* source)
{
    out.clear();
    if (source->size() % 3 != 0)
        return 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        stGlobalEffect_t effect;
        effect.m_field0 = *it++;
        effect.m_field4 = static_cast<float>(*it++) / 10000.0f;
        effect.m_field8 = *it++;
        out.push_back(effect);
    }
    return 1;
}

// ============================================================================
// SetLightServerFlag / GetLightServerFlag（ORIG 0x8a8cba5 / 0x822ad3a）。
// ORIG 全局标志位于 0x940b558（.bss）；SetLightServerFlag 经
// GetLightServerFlag() 取指针后写入 bool。
// ============================================================================

// probe 实证（/tmp/wk/probe12.cpp）：调用点若见 GetLightServerFlag 的"定义"，
// 4.4 将 caller 帧收紧为 0x14/spill -0x14；只见原型则 pad[12] 生效得
// ORIG 帧 0x28/spill -0x1c/p -0xc（0x8a8cba8-0x8a8cbbe）⇒ 定义置于调用点之后。
bool* GetLightServerFlag();
void SetLightServerFlag(bool flag)
{
    bool* lightFlag = GetLightServerFlag();
    // ORIG 0x8a8cba8 帧 0x28、arg-spill -0x1c、指针槽 -0xc；4.4 需 12B dead pad
    // 撑帧使 arg spill 恰落 -0x1c（probe 实证：(void)pad 须在末次使用之后，
    // 否则 4.4 把死数组槽折叠）。尾读 movzbl -0x1c,%edx @0x8a8cbbc
    char pad[12];
    *lightFlag = flag;
    (void)pad;
}

bool* GetLightServerFlag()
{
    static bool light_server_flag = false;  // ORIG _ZZ18GetLightServerFlagvE17light_server_flag V @0x940b558
	return &light_server_flag;
}
// stLimitItemUsageInfoEx 权威定义在 CDataManager.h（含 ctor/dtor 声明），
// 此处不再重复定义。
// ============================================================================
// _checkTimeoutItem（ORIG 0x867cf8d T，G3 任务域辅助）
// CTitleBook 通过 _checkTimeoutItem 直接调用：检查物品是否过期。
// ORIG 逻辑（逐步核对 0x867cf8d 反汇编）：
//   虚表 +0x10（IsAvatarItem，@4）非 0 → 0；
//   虚表 +0x48（PeriodAfterUnsealing，@18）非 0 且 item.m_field0 != 0 → 1；
//   虚表 +0xc（GetItemType，@3）== 0xa 时：expirationDate <= now → 1；
//   否则（其他类型）：item.m_fieldb * 86400 + 0x44a54a80 <= now → 1；
//   其余 → 0。
//   注：ORIG vtable（0x8c96da8）实证 @4=IsAvatarItem、@18=PeriodAfterUnsealing、
//   @19=IsRandomOption（NEW 原实现误用 IsCreatureItem/IsRandomOption，已纠正）。
// [b28d-solo-CDataManagerScripts] AE：①P52 链式 find_item（实参先物化 %ebx）；②P226 条件向语句表达式复现短路物化岛（`mov $1/$0 + test/je`，零槽）；③真语义 bug 修正：`PeriodAfterUnsealing() && m_field0!=0` 路径 ORIG 0x0867d03c 走 +0xb1 `mov $0,%eax` = 返回 **0**（原实现误写 return 1）；fresh clean 101=101；ORIG 0x0867cf8d。
bool _checkTimeoutItem(const Inven_Item& item)   // b28d-Main(P292/P248档①)：ORIG 0x086412ba 调用点 `call; test %al,%al; jne` ⇒ bool
{
    // ORIG 0x0867cf8f：实参 item.m_addInfo 先物化 %ebx 再 call G_CDataManager()
    // ⇒ 单条链式表达式（P52），勿拆具名 dm 局部（会生 -0x18 槽 + 换序）。
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    // [b28d-solo-CDataManagerScripts] P226 条件向：ORIG 0x0867cfb4/0x0867cfd4/0x0867cfdb
    // 短路链物化为 `mov $1/%eax … mov $0/%eax; test %al; je`（无槽）⇒ 语句表达式
    // 承载整条条件（内不含具名变量）。
    if (({ ci == 0 || (ci->getUsablePeriod() == 0 && ci->getExpirationDate() == 0); }))
    {
        return 0;
    }
    if (ci->IsAvatarItem())
    {
        return 0;
    }
    // [b28d-solo-CDataManagerScripts] 真语义 bug 修正：ORIG 0x0867d03c `test %al; je +0xb8`
    // ⇒ 该条件为真时走 +0xb1 `mov $0,%eax; jmp 尾` = **return 0**（原实现写 return 1）。
    if (ci->PeriodAfterUnsealing() && item.m_field0 != 0)
    {
        return 0;
    }
    int now = GlobalData::s_systemTime_.getCurSec();
    if (ci->GetItemType() == 0xa)
    {
        if (ci->getExpirationDate() <= now)
        {
            return 1;
        }
    }
    else
    {
        int expire = item.m_fieldb * 0x15180 + 0x44a54a80;
        if (expire <= now)
        {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// _getIntegratedPvPItemAttr（ORIG T 0x84fc541，G3 PvP 物品属性）
// ORIG 反汇编（84fc541）：
//   it = G_CDataManager()->find_item(item.m_addInfo)；it==0 → 0；
//   虚表 +0xc（GetItemType）== 0xa → item.GetReSealCount() << 5；
//   0xb < GetItemType() <= 0x15 → item.GetReSealCount() << 5；
//   其余 → item.GetItemAttr()。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：P9 —— GetReSealCount/GetItemAttr 返回 uchar（ORIG body 0x822b456 `movzbl 0x6(%eax),%eax; shr $0x5,%al` 实证），4.4 调用点恒插 `movzbl %al,%eax`（ORIG 无）；改共享头返回类型会动他 TU 调用点，保留；ORIG 0x084fc541。
int _getIntegratedPvPItemAttr(const Inven_Item& item)
{
    CItem* it = G_CDataManager()->find_item(item.m_addInfo);
    if (it == 0)
    {
        return 0;
    }
    if (it->GetItemType() == 0xa)
    {
        return (int)item.GetReSealCount() << 5;
    }
    if (it->GetItemType() > 0xb && it->GetItemType() <= 0x15)
    {
        return (int)item.GetReSealCount() << 5;
    }
    return item.GetItemAttr();
}

// ============================================================================
// GetIntegratedPvPItemAttr（ORIG T 0x84fc5ff）
// ORIG 反汇编（84fc5ff）：
//   G_GameWorld()->IsIntegratedPvPBaseChannel() 且
//   G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0 → _getIntegratedPvPItemAttr(item)
//   否则 → item.GetItemAttr()
// ============================================================================
// ORIG 0x84fc5ff：返回类型为 byte（函数体只经最后 call 的 %eax/%al 透传，
//   else 分支 `call GetItemAttr; leave; ret` 无 movzbl 扩宽）。声明 unsigned char
//   ⇒ 两分支均不透传扩展（true 支 int 直落、else 支 byte 无扩），byte 全等 ORIG。
//   调用方（CAccountCargo/CCargo/CTitleBook）均 `(int)Get...(item)&0xff`，低字节一致。
unsigned char GetIntegratedPvPItemAttr(const Inven_Item& item)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        return _getIntegratedPvPItemAttr(item);
    }
    return item.GetItemAttr();
}

// ============================================================================
// GameStubs.cpp 迁移：纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// 权威声明见 CDataManager.h（seria/growth/create/use/rank/death/blood/village/
// questShop/WorldDrop/Slang/Invalid/GrowthPower/Booster/AvatarColor 等）、
// CStackableItem.h（RecipeInfo/MultiBoxLotteryInfo）、CEquipItem.h（RandomItemTable）、
// CItemList.h（BaseItemKey/BaseItemValue）。
// ============================================================================

// ---- RecipeInfo（CStackableItem.h；ORIG C1 0x8513c6a / clear 0x8513bf2 / D1 0x85144f4）----
RecipeInfo::RecipeInfo()
{
    clear();
}
RecipeInfo::~RecipeInfo() {}
void RecipeInfo::clear()
{
    m_vec0.clear();
    m_vecC.clear();
    m_vec18.clear();
    m_field24 = 0;
    m_field25 = 0;
    m_vec28.clear();
    m_field34 = 0;
    m_field38 = 0;
    m_field3c = 0;
    m_vec40.clear();
}

void RecipeInfo::changeOutput(std::vector<std::pair<int, int> >& out)
{   // ORIG W 0x08693a78：out.clear(); out = m_vec40（this+0x40 向量赋值）
    out.clear();
    out = m_vec40;
}

// ---- RandomItemTable（CEquipItem.h；ORIG W 0x8515468 / 0x8514b46）----
RandomItemTable::RandomItemTable() {}
RandomItemTable::~RandomItemTable() {}

// ---- BaseItemKey / BaseItemValue（CItemList.h）----
BaseItemKey::BaseItemKey(unsigned char rarity, unsigned char usableLevel,
                         unsigned short groupName)
    : m_rarity(rarity), m_level(usableLevel), m_partType(groupName)
{
}
BaseItemValue::BaseItemValue() : m_field0(0) {}
BaseItemValue::BaseItemValue(int value) : m_field0(value) {}

// ---- seriaRoomDecoEventScript（ORIG C1 0x8372ef4：构造 vector@0、+0xc=0、
//      map@+0x10、vector@+0x28、+0x34=0、+0x38=0 后三容器各自 clear）----
seriaRoomDecoEventScript::seriaRoomDecoEventScript()
    : m_eventItemIdx(), m_maxEventItemCount(0), m_matchEventMap(),
      m_soundList(), m_fatigueCondition(0), m_rewardMailExpireDate(0)
{
    m_matchEventMap.clear();   // ORIG：先 clear +0x10 map
    m_eventItemIdx.clear();    // 再 clear +0x00 vector
    m_soundList.clear();       // 最后 clear +0x28 vector
}
seriaRoomDecoEventScript::~seriaRoomDecoEventScript() {}

// ---- growthEquipmentScript（ORIG C1 0x8372fbc）----
growthEquipmentScript::growthEquipmentScript()
    : m_rewardMap(), m_giftBox(-1), m_materialItem(-1),
      m_materialMax(0), m_eventBubbleMax(0), m_eventSandMax(0)
{
    m_rewardMap.clear();  // ORIG C1 构造 map 后 clear
}
growthEquipmentScript::~growthEquipmentScript() {}

// ---- createChracScript（ORIG C1 0x8373042：map 构造后 clear）----
createChracScript::createChracScript() { m_map0.clear(); }
// ORIG D1 0x837c3d4 (W)：析构成员 map<int,rewardInfoList>。
createChracScript::~createChracScript() {}
// ORIG rewardInfoList C1 0x84e91fa：vector<rewardInfo> ctor + clear()。
createChracScript::rewardInfoList::rewardInfoList()
{
    m_list.clear();
}
// ORIG C1 0x8373098：map 构造后 clear()。
useCharacterJobFatigue::useCharacterJobFatigue()
{
    m_giftMap.clear();
}
useCharacterJobFatigue::~useCharacterJobFatigue() {}

// ---- CharacterCreateCreatureGiftScript（ORIG C1 0x83730ee：构造 map@0/
//      +0x18=0/map@+0x1c/map@+0x34 后三容器各自 clear）----
CharacterCreateCreatureGiftScript::CharacterCreateCreatureGiftScript()
    : m_eventJobMap(), m_changeAbleCreatureLev(0), m_clientChangeMap(),
      m_infinityChangeMap()
{
    m_eventJobMap.clear();
    m_clientChangeMap.clear();
    m_infinityChangeMap.clear();
}
CharacterCreateCreatureGiftScript::~CharacterCreateCreatureGiftScript() {}

// ---- seriaRoomDecoAniEventScript（ORIG C1 0x83731ca：五容器构造后 clear 0x83732a4）----
seriaRoomDecoAniEventScript::seriaRoomDecoAniEventScript() { clear(); }
seriaRoomDecoAniEventScript::~seriaRoomDecoAniEventScript() {}

stRankSystemInfo::stDungeonRankSystem_t::stDungeonRankSystem_t() : m_map0(), m_vec18()
{
    clear();
}
stRankSystemInfo::stDungeonRankSystem_t::~stDungeonRankSystem_t() {}
void stRankSystemInfo::stDungeonRankSystem_t::clear()
{
    // ORIG 0x8370e34：清 map/vector + memset +0x24 0x12 字节
    m_map0.clear();
    m_vec18.clear();
    // ORIG 0x8370e34：+0x24 起 memset 0x12 字节 = m_rankLevel[9] 全体
    memset(m_rankLevel, 0, sizeof(m_rankLevel));
}

// ORIG C1 0x8370eba：构造子对象 + 4 容器后调 clear()（clear 内写 +0x74）。
stRankSystemInfo::stRankSystemInfo() : m_dungeonRank(), m_vec38(), m_map44(),
                                       m_vec5c(), m_vec68()
{
    clear();
}
stRankSystemInfo::~stRankSystemInfo() {}
void stRankSystemInfo::clear()
{
    m_vec38.clear();
    m_map44.clear();
    m_vec5c.clear();
    m_vec68.clear();
    m_field74 = 0;
}

// ---- stRankSystemInfo 嵌套子类（ORIG W 符号）----
// stDungeonClearTimeData：C1 0x0891b6f2（调 clear）/ clear 0x0891b706
// （memset(this,0,0x1c)）/ D1 0x0893fbc6（空体）。
stRankSystemInfo::stDungeonClearTimeData::stDungeonClearTimeData()
{
    clear();
}
void stRankSystemInfo::stDungeonClearTimeData::clear()
{
    memset(this, 0, sizeof(*this));   // ORIG memset 0x1c
}
stRankSystemInfo::stDungeonClearTimeData::~stDungeonClearTimeData() {}

// stComboIntervalBoundary：C1 0x0891b72a / clear 0x0891b73e
// （memset(this,0,0x10)）/ D1 0x08940176（空体）。
stRankSystemInfo::stComboIntervalBoundary::stComboIntervalBoundary()
{
    clear();
}
void stRankSystemInfo::stComboIntervalBoundary::clear()
{
    memset(this, 0, sizeof(*this));   // ORIG memset 0x10
}
stRankSystemInfo::stComboIntervalBoundary::~stComboIntervalBoundary() {}

// ---- stBloodDungeon_t（ORIG C1 0x8370a3a / clear 0x8370b02 / D1 0x8370ab0）----
stBloodDungeon_t::stBloodDungeon_t() : m_vec334(), m_map358()
{
    clear();
}
stBloodDungeon_t::~stBloodDungeon_t() {}
// [b28d-solo-CDataManagerScripts] AE：P310 —— `for (int i = 0; (unsigned int)i <= 199; ++i)` 复现 ORIG 两段寄存器尾测（`mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe`）；fresh clean 42=42；ORIG 0x08370b02。
void stBloodDungeon_t::clear()
{
    // [b28c-27] N：ORIG 循环条件 `mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe`
    // （局部先载寄存器）；4.4 O0 恒 `cmpl $0xc7,-0xc(%ebp)` mem 直比。unsigned/
    // while 两形实测同折，属局部比较 codegen 跨版本差（N-86b 族），语义全等。
    // ORIG 0x8370b02：循环(unsigned≤199)置 1.0f → +0x334 clear → +0x320=1 →
    // +0x330=1.0f → memset(+0x324,0,0xc) → memset(+0x340,0,0x18) → +0x358 clear。
    //   【N】循环条件 ORIG `mov -0xc,%eax; cmp $0xc7,%eax`（先载入寄存器），
    //   4.4 O0 恒折 `cmpl $0xc7,-0xc(%ebp)`（mem 直比），语义全等，register 形差。
    // [b28d-solo-CDataManagerScripts] P310：局部改 int + 显式 (unsigned int) 比较
    // 复现 ORIG 两段寄存器尾测（mov -0xc,%eax; cmp $0xc7,%eax; setbe）。
    for (int i = 0; (unsigned int)i <= 199; ++i)
        m_rate[i] = 1.0f;
    m_vec334.clear();
    m_field320 = 1;
    m_field330 = 1.0f;
    memset(m_field324, 0, sizeof(m_field324));
    memset(m_pad340, 0, 0x18);  // 连续覆盖 +0x340..+0x357（含 344/348 两 int）
    m_map358.clear();
}
// stVillageAttacked_t 现为纯 vector 包装（CDataManager.h），ctor/dtor 隐式生成
// （ORIG W 0x837cf1e/0x837cf32 即隐式符号），无显式定义。

// ---- stDeathTower_t（ORIG C1 0x08370970）----

// ORIG C1 0x8370970：四个 200 元素表初始化 + 尾部字段，非全量 memset。
// [b28d-solo-CDataManagerScripts] AE：P310 —— `for (int i = 0; (unsigned int)i < 200; ++i)` 复现 ORIG 两段寄存器尾测；fresh clean 45=45；ORIG 0x08370970。
stDeathTower_t::stDeathTower_t()
{
    m_apcExpWeight = 0.0f;        // ORIG +0x00 位清零（float 语义：import [apc exp weight] fstps 实证）
    m_monsterExpWeight = 0.0f;    // float 语义（[monster exp weight] fstps）
    m_rewardGoldWeight = 1.0f;
    //   【N】循环条件 ORIG `mov -0xc,%eax; cmp $0xc7,%eax`（先载入寄存器），
    //   4.4 O0 恒折 `cmpl $0xc7,-0xc(%ebp)`（mem 直比），语义全等，register 形差。
    // [b28d-solo-CDataManagerScripts] P310：int 局部 + 显式 (unsigned int) 比较。
    for (int i = 0; (unsigned int)i < 200; ++i)
    {
        m_rewardExpWeight[i] = 1.0f;          // this+0x04+i*4
        m_rewardCardNum[i] = 0;               // this+0x324+i*4
        m_illusionRewardExpWeight[i] = 1.0f;  // this+0x644+i*4
        m_illusionRewardCardNum[i] = 0;       // this+0x964+i*4
    }
    memset(m_rewardItemProb, 0, sizeof(m_rewardItemProb));  // ORIG memset(+0xc88,0,12)
}

// live_server_info_t ctor ORIG 0x0837dd64：仅 map 成员构造（自动）。
live_server_info_t::live_server_info_t() : m_pvpChannelMap() {}
live_server_info_t::~live_server_info_t() {}

stQuestShop::stQuestShop()
{
    // ORIG C1 0x83710d4：[map C1][initCost C1][map clear] 后 -1、memset(+0x28,0,0x4d)。
    m_map0.clear();
    m_field18 = -1;
    memset(m_pad28, 0, sizeof(m_pad28));
}
stQuestShop::~stQuestShop() {}
InvalidCharData::InvalidCharData()
    : m_charList(), m_charRange(), m_shortList(), m_shortRange()
{
}
InvalidCharData::~InvalidCharData() {}

// ---- SlangNameData（ORIG C1 0x837d08e / D1 0x837d0fa，本 TU 唯一 owner）----
// C1 依次构造 string/unsigned-char/unsigned-short 三个 vector；D1 反向析构。
SlangNameData::SlangNameData()
    : m_slangList(), m_charList(), m_shortList()
{
}
SlangNameData::~SlangNameData() {}

// ---- STGrowthPowerData（ORIG C1 0x83711fe / D1 0x837d290）----
STGrowthPowerData::STGrowthPowerData() : m_vec0(), m_vecC(), m_map18()
{
    // ORIG C1 构造后 clear 各容器
    m_vec0.clear();
    m_vecC.clear();
    m_map18.clear();
}
STGrowthPowerData::~STGrowthPowerData() {}

// ---- AvatarColorInfo（ORIG D1 0x837d358）----
AvatarVariation::AvatarColorInfo::AvatarColorInfo() : m_colorMap(), m_nameMap()
{
}
AvatarVariation::AvatarColorInfo::~AvatarColorInfo() {}
BoosterGageData::BoosterGageData()
{
    // ORIG C1 0x83850cc：[vector C1] 后 clear + 三字节清零。
    m_BoosterPoint.clear();
    m_cMaxBoosterGage = 0;
    m_cFieldD = 0;
    m_cMultiboxCount = 0;
}
BoosterGageData::~BoosterGageData() {}

// ============================================================================
// GameStubs.cpp 迁移（第二批）：其余纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// ============================================================================

// ---- reseal_table_t（含 vector + 2×int[6]；ORIG 0x8370742）----
reseal_table_t::reseal_table_t()
{
    for (int i = 0; i <= 5; ++i)
        m_arr1[i] = 0;
    for (int i = 0; i <= 5; ++i)
        m_arr2[i] = 0;
}
reseal_table_t::~reseal_table_t() {}

// ORIG get_reseal_cost @0x08900b02（T）。见 CDataManager.h 声明注释。
// [b28d-solo-CDataManagerScripts] DIFF→N：有符号除 10 已翻正（`int idx=(param_2-1)/10` ⇒ ORIG magic 0x66666667）；残余 ①idx 槽 -0x10 vs ORIG -0xc 的 mem 直比（4.4 恒插 mov 中转）②%ebx/result 寄存器分配（P248 三档试尽）；ORIG 0x08900b02。
int reseal_table_t::get_reseal_cost(unsigned char param_1,
                                    unsigned char param_2,
                                    unsigned char param_3)
{
    // param_1 强化等级索引 0..5；param_2 等级 1..99；param_3 选项 0..6
    if (param_2 == 0)              // -0x20(%ebp)==0
        return -1;
    if (param_1 > 5)              // -0x1c(%ebp)>5 → jbe 未命中时
        return -2;
    if (param_2 > 99)             // -0x20(%ebp)>99
        return -3;
    if (param_3 > 6)              // -0x24(%ebp)>6
        return -4;

    // (param_2-1)/10（0x66666667 乘法魔数 → 无符号除 10 的 32 位无符号截断）
    int idx = (param_2 - 1) / 10;
    if (m_vec.size() <= idx)      // setbe → vector::size() <= idx（ORIG 0x08900b86 mem 直比 dst=eax）
        idx = (int)m_vec.size() - 1;   // 取末元素
    // return m_vec[idx]*m_arr1[param_1] + param_3*m_arr2[param_1]
    // （ORIG imul 为 32 位模 2^32，此处用无符号累加再一次性转 int 保持位模式）
    return (int)(m_vec[idx] * (unsigned int)m_arr1[param_1]
                 + (unsigned int)param_3 * (unsigned int)m_arr2[param_1]);
}

// ORIG get_oneshot_reseal_cost @0x08900be0（T）。见头声明注释。
int reseal_table_t::get_oneshot_reseal_cost(unsigned char param_1,
                                            unsigned char param_2,
                                            unsigned char param_3)
{
    // 汇编仅校验 param_1<=5 与 param_2 范围，忽略 param_3（无 -0xc(%ebp) 读取）
    (void)param_3;
    if (param_2 == 0)
        return -1;
    if (param_1 > 5)
        return -2;
    if (param_2 > 99)
        return -3;
    return 1;
}

// ---- InGameAdvertisementScript（本 TU 唯一 owner；InitScriptProducers_7 重复已删）----
// ORIG C1 0x83732f0：仅调 clear()。
InGameAdvertisementScript::InGameAdvertisementScript()
{
    clear();
}

// ORIG clear 0x8373304：memset(this, 1, 0x97)——前 0x97 字节置 1，尾字节保持原值。
void InGameAdvertisementScript::clear()
{
    // P68 正向：ORIG 0x837330a 实参序「this 先物化→0x97→0x1」＝ 带 cast 形。
    memset((char*)this, 1, 0x97);
}

// ---- PcBangItemRentarData（ORIG D1 0x837c2c0）----
PcBangItemRentarData::PcBangItemRentarData() : m_vec0(), m_mapC()
{
}
PcBangItemRentarData::~PcBangItemRentarData() {}

// ---- DimensionActivationData（ORIG C1 0x8371422 / D1 0x837c306）----
DimensionActivationData::DimensionActivationData()
    : m_field0(0), m_coinMap()
{
	// ORIG C1：+0x00 = 0，构造 map/rewardData 后清 coinMap（m_rewardData 默认初始化，无值初始化 memset）
	m_coinMap.clear();
}
DimensionActivationData::~DimensionActivationData() {}

// ORIG C1 0x837c53c：仅构造 +0x4/+0x10/+0x1c/+0x28/+0x34 五容器，+0x00 不初始化。
stUnlimitChallengeInfo::stUnlimitChallengeInfo()
    : m_vec4(), m_vec10(), m_vec1c(), m_vec28(), m_multiMap34()
{
}
stUnlimitChallengeInfo::~stUnlimitChallengeInfo() {}

// ---- stLimitItemUsageInfoEx（ORIG D1 0x837c7a2）----
stLimitItemUsageInfoEx::stLimitItemUsageInfoEx()
    : m_info0(), m_resetC(), m_refill18()
{
}
stLimitItemUsageInfoEx::~stLimitItemUsageInfoEx() {}

// ---- CNPCRelationEventManager（ORIG D1 0x890ce34：遍历 map 删除节点后析构 map）----
CNPCRelationEventManager::CNPCRelationEventManager()
    : m_npcEventMap()
{
}
// ORIG 0x890ce34：遍历 delete 节点后直接析构 map（无显式 clear()）。
CNPCRelationEventManager::~CNPCRelationEventManager()
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.begin();
    while (it != m_npcEventMap.end())
    {
        stNPCRelationEventInfo* node = it->second;
        if (node != 0)
            delete node;
        ++it;
    }
}

// ---- CNPCRelationEventManager::AddNPCInfo（ORIG 0x890ceea：
//      make_pair(idx, info) 后 map insert）----
void CNPCRelationEventManager::AddNPCInfo(const unsigned short idx,
                                          stNPCRelationEventInfo* info)
{
    m_npcEventMap.insert(std::make_pair(idx, info));
}

// ---- CNPCRelationEventManager::AddFavorItemInfo（ORIG 0x890cf44：
//      find(idx)，非 end 时将 {itemId,count} push_back 到 node+kind*0x14
//      即 m_favor[kind].m_items；步长 0x14 为 stNPCFavorAndRewardInfo）----
void CNPCRelationEventManager::AddFavorItemInfo(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
    unsigned int itemId, unsigned int count)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    // ORIG 0x890cf7x：node 物化局部槽后重读（operator-> 一次），item 栈构 push。
    stNPCRelationEventInfo* node = it->second;
    stNPCEventItemInfo item;
    item.m_itemId = static_cast<int>(itemId);
    item.m_count = static_cast<int>(count);
    node->m_favor[kind].m_items.push_back(item);
}

// ---- CNPCRelationEventManager::AddRewardItemInfo（ORIG 0x890cfda：
//      find(idx)，非 end 时写 node+kind*0x14+0xc / +0x10 两奖励值）----
// [b28d-solo-CDataManagerScripts] DIFF→N：N107/N7 —— ORIG 两步 lea（`lea (%ecx,%eax,1); lea 0xc(%eax),%edx`）vs 4.4 `lea+add $0xc`（数组名算术 P214① 本轮实测不翻），另加实参装载序 + %ebx 分配；ORIG 0x0890cfda。
void CNPCRelationEventManager::AddRewardItemInfo(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
    unsigned int rewardA, unsigned int rewardB)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    // ORIG 0x890cfda：每写各发一次 operator->/寻址（47<55 引用绑定会过度折叠，
    // node 两写形 57 最近）。
    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d052：`lea (%ecx,%eax,1),%eax; lea 0xc(%eax),%edx`（两步 lea）
    // ⇒ 数组名参与算术（P214①）而非下标形（4.4 下标形折 add $0xc）。
    (node->m_favor + kind)->m_rewardA = static_cast<int>(rewardA);
    (node->m_favor + kind)->m_rewardB = static_cast<int>(rewardB);
}

// ---- CNPCRelationEventManager::AddDislikeItemInfo（ORIG 0x890d07a：
//      find(idx)，非 end 时向 node+0x3c 的 vector<int> push_back）----
// [b28d-solo-CDataManagerScripts] AE：删具名 `int item`（P0 右值直传 push_back(itemId) 的 const& 临时由编译器生成）⇒ it/end 槽 -0x14/-0x10 与 ORIG 对齐；ORIG 0x0890d07a。
void CNPCRelationEventManager::AddDislikeItemInfo(unsigned short idx,
                                                  unsigned int itemId)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    // ORIG 0x890d0c5 eq + 0x890d0cc jne 跳过 ⇒ 源码 == end 早退形；
    // 0x890d0d9-0x890d0f5：node 物化槽 -0xc 后重读、(int)itemId 物化 -0x10，
    // push_back(EOi) 传 &temp（4.4 将 node 折进 %eax 不复现 ⇒ 余差属 N）。
    if (it == m_npcEventMap.end())
        return;
    stNPCRelationEventInfo* node = it->second;
    node->m_dislikeItems.push_back(itemId);
}

// ---- CNPCRelationEventManager::GetFavorIndex（ORIG 0x890d3d0：
//      strncmp 前 10 字节分派 [good]/[soso]/[bad]→0/1/2，其余 3）----
ENUM_NPC_RELATION_EVENT_FAVOR_KIND CNPCRelationEventManager::GetFavorIndex(
    const char* name)
{
    if (std::strncmp("[good]", name, 10) == 0)
        return ENUM_NPC_RELATION_FAVOR_GOOD;
    if (std::strncmp("[soso]", name, 10) == 0)
        return ENUM_NPC_RELATION_FAVOR_SOSO;
    if (std::strncmp("[bad]", name, 10) == 0)
        return static_cast<ENUM_NPC_RELATION_EVENT_FAVOR_KIND>(2);
    return static_cast<ENUM_NPC_RELATION_EVENT_FAVOR_KIND>(3);
}

// ============================================================================
// IsDislikeItem（ORIG T 0x0890d100）：m_npcEventMap.find(idx) 未命中 → 0；
// node->m_dislikeItems（+0x3c vector<int>）任一元素 == itemId → 1。
// ============================================================================
bool CNPCRelationEventManager::IsDislikeItem(unsigned short idx,
                                             unsigned int itemId)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return false;

    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d100：索引循环 .size()/.at()（非迭代器），每元素一次 at()
    // 返回指针后 (*puVar3==itemId) 用 sete 直判命中即返 1。
    for (unsigned int col = 0; col < node->m_dislikeItems.size(); ++col)
    {
        // [b28c-L15] ORIG 0x0890d15x：`mov (%eax),%eax; cmp 0x10(%ebp),%eax; sete %al`
        //   —— at() 结果留在 %eax、形参槽作内存源操作数（无 (int) cast 中转）。
        if (node->m_dislikeItems.at(col) == itemId)
            return true;
    }
    return false;
}

// ============================================================================
// GetItemKind（ORIG T 0x0890d1c0，sret stTempRewardInfo*）：
// m_npcEventMap.find(idx) 未命中 → 默认值；遍历 node->m_favor[kind].m_items
// （kind=0..2，步长 0x14）：item.m_itemId==itemId 且 item.m_count<=count：
//   count==0 → 默认值（不写）；
//   否则 → out.m_kind=kind、out.m_count=ROUND(count/item.m_count)。
// [N类·跨版本豁免] 已还原 ORIG 真形（our 113 vs 112）：残 1 指令差为 gcc4.1/4.4 的
// outgoing-frame / 死栈槽产物（范式库『跨版本』），助记符序列已对齐；语义完全一致。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：值拷贝 `stNPCEventItemInfo item = at(col)` 已落（消引用指针槽）；残余整体槽平移 8（-0x1c.. vs ORIG -0x14..）+ ROUND x87 段槽序（N93/N86）；ORIG 0x0890d1c0。
stTempRewardInfo CNPCRelationEventManager::GetItemKind(
    unsigned short idx, unsigned int itemId, unsigned int count)
{
    stTempRewardInfo result;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;   // ORIG 0x0890d1d2 `mov %ebx,(%esp); call C1Ev` ⇒ NRVO 直构 sret

    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d1c0：不绑定 vector 引用/迭代器，每轮 size()/at() 现取
    // &node->m_favor[kind].m_items（m_items 在 favor[kind]+0，步长 0x14），
    // at() 结果缓存一次后读 m_itemId(+0)/m_count(+4)；ROUND=(int)(long double)
    // count/(long double)m_count（x87 fildll/fdivrp/fldcw-PC64/fistpl 最近舍入）。
    for (int kind = 0; kind < 3; ++kind)
    {
        for (unsigned int col = 0;
             col < node->m_favor[kind].m_items.size(); ++col)
        {
            stNPCEventItemInfo item = node->m_favor[kind].m_items.at(col);
            if (item.m_itemId == (int)itemId
                && (unsigned int)item.m_count <= count)
            {
                if (item.m_count == 0)
                    return result;
                result.m_kind = kind;
                result.m_count =
                    (int)((long double)count / (long double)item.m_count);
                return result;
            }
        }
    }
    return result;
}

// ORIG C1 0x837cf46：仅构造 +0x4/+0x10/+0x1c/+0x28 四 vector，+0x00 不初始化。
stCharacLinkSystem_t::stCharacLinkSystem_t()
    : m_vec4(), m_vec10(), m_vec1c(), m_vec28()
{
}
stCharacLinkSystem_t::~stCharacLinkSystem_t() {}
// ---- CNPCRelationEventManager::GetRewardItem（ORIG T 0x0890d326，sret
//      stNPCRewardItemInfo）：find(idx) 未命中 → {0,0}；命中 →
//      node->m_favor[kind].m_rewardA / m_rewardB（+0xc/+0x10）。----
// [b28d-solo-CDataManagerScripts] 部分翻正：`return stNPCRewardItemInfo(result);` 破 4.4 NRVO ⇒ result 落 -0x18/-0x14 + 尾部拷贝 sret，帧/槽与 ORIG 对齐；残余 = N107 成员数组寻址折叠（ORIG `mov 0xc(%eax,%ecx,1)` vs 4.4 lea+add）⇒ N107/N86 封顶；ORIG 0x0890d326。
stNPCRewardItemInfo CNPCRelationEventManager::GetRewardItem(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind)
{
    stNPCRewardItemInfo result;
    result.m_rewardA = 0;
    result.m_rewardB = 0;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return stNPCRewardItemInfo(result);

    stNPCRelationEventInfo* node = it->second;
    result.m_rewardA = node->m_favor[kind].m_rewardA;
    result.m_rewardB = node->m_favor[kind].m_rewardB;
    return stNPCRewardItemInfo(result);
}

// ---- advancealtar（StageTimeLineParameter D1 0x8899e88 /
//      AdvanceAltarShopParameter D1 0x88a0fa2：析构 map 成员；
//      RewardParameter 清 pad；ConfigParameter 按 ORIG 0x889b662）----
advancealtar::StageTimeLineParameter::StageTimeLineParameter()
    : m_summonMap(), m_actionMap(), m_timeLine()
{
}
advancealtar::StageTimeLineParameter::~StageTimeLineParameter() {}
advancealtar::AdvanceAltarShopParameter::AdvanceAltarShopParameter()
    : m_buyShopMap(), m_starRestMap()
{
}
advancealtar::AdvanceAltarShopParameter::~AdvanceAltarShopParameter() {}
// ORIG _ZN12advancealtar14BuyUpgradeDataC1Ev 0x088a3570 T：构造 string +0xc/
// vector +0x10/map +0x1c 后：+0x00=0、+0x04=0、+0x08=1（ORIG movb 低字节，
// bool 语义）、vector.clear()、map.clear()。
advancealtar::BuyUpgradeData::BuyUpgradeData()
    : m_strDesc(), m_fieldTypes(), m_listMap()
{
    m_nItemId = 0;
    m_field4 = 0;
    m_field8 = 1;
    m_fieldTypes.clear();
    m_listMap.clear();
}
advancealtar::RewardParameter::RewardParameter()
{
    // ORIG C1 0x889af74 W：构造 this+0 的 map<int,AchievementRewardData>；
    // m_rewardMap 默认构造即空 map，与 ORIG 一致。
}
advancealtar::RewardParameter::~RewardParameter() {}
// ORIG _ZN12advancealtar11BuyShopDataC1Ev 0x88a3630：三 vector 构造后 +0=0、clear×3。
advancealtar::BuyShopData::BuyShopData()
{
    m_nField0 = 0;
    m_vecT0.clear();
    m_vecT1.clear();
    m_vecT2.clear();
}
advancealtar::ConfigParameter::ConfigParameter()
{
    // ORIG 0x889b662：+0/+4 清零、char*+0x18=&(+8)、两段 0x10 memset。
    m_0 = 0;
    m_4 = 0.0f;
    m_strPtr = reinterpret_cast<char*>(this) + 8;
    std::memset(m_gageUpRate, 0, sizeof(m_gageUpRate));
    std::memset(m_gageUpAux, 0, sizeof(m_gageUpAux));
}
advancealtar::ConfigParameter::~ConfigParameter() {}

// ORIG reset 0x0889af9c T：m_rewardMap.clear()。
void advancealtar::RewardParameter::reset()
{
    m_rewardMap.clear();
}

// ============================================================================
// eventReward::importRewardTable（ORIG 0x080ef29c T）
// 列表装载失败返 0x77；逐文件 ImportRewardScript 失败返 0x71；成功返 0。
// ORIG STScriptFileList 即 +0 的 map<int,string>（NpcScript.h 占位版同尺寸），
// 故直接以本地 map 重解释传入，避免与本 TU 已含头中的占位定义冲突。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①P214① 正极性包裹；②真实 STScriptFileList 局部（ORIG 调 _ZN16STScriptFileListC1Ev；原 map+reinterpret_cast 调错 C1 符号且槽深 4）；③P13 缓存 end（ORIG 循环前调一次 end 存 -0x28）；④`!ImportRewardScript(...)` 复现 ORIG `test %eax,%eax; je` 直分支；fresh clean 82=82；ORIG 0x080ef29c。
int eventReward::importRewardTable(const char* dir, const char* file)
{
    // [b28d-solo-CDataManagerScripts] 真实 STScriptFileList（ORIG 0x080ef2a7 调
    // _ZN16STScriptFileListC1Ev；此前用 map + reinterpret_cast 会调 map C1 且多
    // 一条取址 lea、槽整体深 4）。
    STScriptFileList fileList;
    // ORIG 0x080ef2a7：test %al; je 失败块（末尾 mov $0x77）⇒ 正极性包裹
    // （P214①），勿写 if(!f()) 早返（产 xor$1 + 前置换位）。
    if (loadRDARScriptListFile(&fileList, dir, file, 0))
    {
        // fileList 为非 const 局部 ⇒ ORIG 用非 const iterator（const_iterator 换
        // begin/end/operator++ callee 集致 we-larger 主因）。ORIG 0x080ef2ce 在
        // 循环前调一次 end() 存具名 e（槽 -0x28）⇒ P13 缓存 end。
        std::map<int, std::string>::iterator it = fileList.m_fileMap.begin();
        std::map<int, std::string>::iterator e = fileList.m_fileMap.end();
        for (; it != e; ++it)
        {
            // ORIG 0x080ef2f7 `test %eax,%eax; je` 直接分支（4.4 对 ==0 物化 sete）。
            if (!ImportRewardScript(dir, it->second.c_str()))
                return 0x71;
        }
        return 0;
    }
    return 0x77;
}

// ============================================================================
// DeleteInvalidItemScript::loadScript（ORIG 0x08ac1e60 T）
// tw_DeleteInvalidItem.etc：[delete item] 三元组 (idx, rewardIdx, count)
// 以 idx 为 key、{idx,rewardIdx,count} 为值 insert；[send mail title]/
// [send mail content] 分别 ScanStr 进 +0/+4 字符串，ScanStr 失败直接结束；
// ScanType 耗尽正常结束返回 1。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemData::DeleteInvalidItemData()
{
    // ORIG C1 0x08ac220c W：转调 init()（三 int 清零）。
    init();
}
void DeleteInvalidItemScript::DeleteInvalidItemData::init()
{
    // ORIG 0x08ac2220 W：三 int 清零。
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// [b28d-solo-CDataManagerScripts] AE：函数头声明序 `token, ok, idx, data`（P212/P309：4.4 头部声明按先声明浅 ⇒ data 落最深 -0x50、idx -0x44，与 ORIG 一致）；fresh clean 149=149；ORIG 0x08ac1e60。
bool DeleteInvalidItemScript::loadScript()
{
    // [X1 形] ORIG 7 检查点（load 0x08ac1e7c / ScanType 0x08ac1ec3 / ScanInt-ok×3
    // 0x08ac1efb.. / ScanStr→ok 回读×2 0x08ac1fdd/0x08ac201a）均 xor$1+test 带分支
    // 消费者形。[b28c-18-unbind] P0 硬约束：register asm("eax") 绑定已全域清除，
    // 落 N96+N18a+P168a④ 判词——无绑定即不可复现 movzbl;xor$1;test 寄存器物化形，
    // 封顶自然形（bool callee 裸形直产 xor 段者除外，P168a①）。
    if (!(loadRDARScriptFile("Etc/", "tw_DeleteInvalidItem.etc")))
        return false;

    std::string token;
    bool ok = false;
    int idx;
    DeleteInvalidItemData data;
    for (;;)
    {
        if (!(ScanType(token, true)))
            break;
        if (token == "[delete item]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!(ok))
                    break;
                int rewardIdx = ScanInt(&ok);
                if (!(ok))
                    break;
                int count = ScanInt(&ok);
                if (!(ok))
                    break;
                data.init();
                data.m_field0 = idx;        // ORIG：data.f0=首个 int（与 key 同值）
                data.m_field4 = rewardIdx;
                data.m_field8 = count;
                m_deleteMap.insert(std::make_pair(idx, data));
            }
        }
        else if (token == "[send mail title]")
        {
            ok = (bool)ScanStr(&m_field0);  // ORIG：先物化入 ok 槽再回读
            if (!(ok))
                break;
        }
        else if (token == "[send mail content]")
        {
            ok = (bool)ScanStr(&m_field4);
            if (!(ok))
                break;
        }
    }
    unloadRDARScriptFile();
    return true;
}

// ============================================================================
// advancealtar::ConfigParameter::importConfigParameter（ORIG 0x0889b6cc T）
// 逐段还原 ORIG ## 汇编 + ## 反编译 C：
//  · [gage up rate] 行 (idx, P, Q)：0<=idx<=3 时 this+8(idx*4)=Q(m_gageUpRate)、
//    this+0x1c(idx*4)=P(m_gageUpAux)（ORIG 0x0889b742/0x0889b757：+8 存第 3 个
//    ScanInt=local_18，+0x1c 存第 2 个=local_1c，与直觉相反，原实现写反已修）；
//    循环带一个 ORIG dead 计数 local_24（0..每行 ++，从不读）。P 失败→0x3e9、
//    Q 失败→0x3ec。
//  · [gage upgrade cost percent]：(float)(long double)ScanFloat 存 m_4(+4)，失败 0x3f8；
//    [daily refill ticket]：ScanInt 存 m_0(+0)，失败 0x3ff。
//  · 三个开标签均不匹配时（ORIG 最内 else 0x0889b84f..）短路比对
//    [/gage up rate]、[/gage upgrade cost percent]、[/daily refill ticket]（结果丢弃）。
//    原实现缺这三条比较 → we-smaller 主因。成功/文件打开失败均返 0（非零=Fail）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：N93 —— 帧 sub $0x30 vs 0x40 + 块级对象槽序（语义完成：三段解析/闭标签短路比对齐）；ORIG 0x0889b6cc。
int advancealtar::ConfigParameter::importConfigParameter(const char* path)
{
    std::string token;
    bool ok = false;
    // [X2 形·技巧22精化（DispC8 probe 实证，Main 批准）] ORIG 对 bool callee 返回一律
    // `xor $0x1,%eax; test %al,%al; j<cc>`（0x0889b6fd/0x0889b725/0x0889b764/…），4.4
    // 直写 !call()/!ok 折成 test/sete 两段。[b28c-18-unbind] 绑定已全部清除
    // （N96+N18a+P168a④，判词见 DeleteInvalidItemScript::loadScript 头）。
    if (!(loadRDARScriptFile("", path)))
        return 0;

    for (;;)
    {
        if (!(ScanType(token, true)))
            break;                               // ORIG 0x0889b72a jne 出口 → 返 0
        if (token == "[gage up rate]")
        {
            int lineNo = 0;                      // ORIG local_24：dead 行计数
            for (;;)                              // ORIG 头：idx=ScanInt 于循环顶
            {
                int idx = ScanInt(&ok);
                if (!(ok))
                    break;                        // ORIG：行首失败仅结束本段(0x…76c)
                int p = ScanInt(&ok);            // ORIG local_1c（第 2 个）
                if (!(ok))
                    return 0x3e9;
                int q = ScanInt(&ok);            // ORIG local_18（第 3 个）
                if (!(ok))
                    return 0x3ec;
                if (idx >= 0 && idx <= 3)        // ORIG cmpl $0/js + cmpl $3/jg
                {
                    m_gageUpRate[idx] = q;       // this+8   = 第 3 值
                    m_gageUpAux[idx] = p;        // this+0x1c = 第 2 值
                }
                ++lineNo;                        // ORIG local_24++
            }
        }
        else if (token == "[gage upgrade cost percent]")
        {
            float fv = ScanFloat(&ok);           // ORIG：fstps 暂存，校验后才写 this+4
            if (!(ok))
                return 0x3f8;
            m_4 = fv;
        }
        else if (token == "[daily refill ticket]")
        {
            int tv = ScanInt(&ok);               // ORIG：暂存 -0xc，校验后才写 this+0
            if (!(ok))
                return 0x3ff;
            m_0 = tv;
        }
        else
        {
            // ORIG 0x0889b872..：三段 `call operator== ; test %al,%al ; jne <loop>`，
            // 命中闭标签直接回循环头（true→jne，非 != 取反物化）。
            if (token == "[/gage up rate]")
                continue;
            if (token == "[/gage upgrade cost percent]")
                continue;
            (void)(token == "[/daily refill ticket]");
        }
    }
    return 0;
}

// ============================================================================
// advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter
// （ORIG 0x088a10cc T，恒返 0）
// 语义（ORIG 汇编逐段还原，见 importAdvanceAltarShopParameter.md ## 汇编/## 反编译 C）：
//  · [star reset fee gold] 段：每轮先 ctor _StarRestFeeByGold(-0x238)，key=ScanInt，
//    fee=ScanInt，任一失败仅结束本段回外层扫描头；成功 insert(key,fee)。
//  · 每个 [ridable id] 块：ridableId=ScanInt；!ok 或 !isValidRidableId ⇒ bVar4=假
//    ⇒ 不插入直接返 0（原实现误无条件插入，已修正）。valid 时 shop.m_nField0=id，
//    进 LAB_088a12ab 段循环，[unit]/[skill]/[tower] 各自内联展开三份独立解析块。
//  · item [id] 子块字段序：unit/tower=[unlock stage index],[name],[field name],[list];
//    skill 额外在 unlock 与 name 间插 [is settable slot]。
//  · 关键：push_back(item) 绑定在 [list] 处理尾（非 [/id]），故 item 收集入
//    shop.m_vecT0/T1/T2 由 [list] 触发；[list] 逐行读入 item.m_listMap(key=FieldType-0
//    行 m_value)，直到某行 ScanStr/ScanInt 失败。[/ridable id] 或 EOF ⇒ 插入 shop。
// [b28c-18-unbind] 32 处 bool-callee 检查点的 register 绑定已全域清除（P0 硬约束），
// 统一落 N96+N18a+P168a④ 判词；xor 物化形为跨版本寄存器分配产物，源码不可复现。
// 语义已全量还原为 ORIG 三段内联真形（our_insns
// 708→1403→1431 vs ORIG 1441，gap 727→38→10）。残留 10 指令差为 gcc 4.1.2 相对
// 4.4.7 的 EH landing-pad 与 BuyShopData/BuyUpgradeData 双份 dtor epilogue vs merge、
// deferred-write 寄存器轮换（技巧24 落点岛/技巧31 dead 撑帧无法从源码复现）
// （范式库「负例 N 类·跨版本：delete[]+成员 dtor EH cleanup / EH 双份 dtor epilogue vs
// merge」），非源码可表达；证据 ORIG 0x088a18de/0x088a2731/0x088a2704 CatchHandler 区段。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：N82 —— 超大件帧槽（§6.2 口径：调用点差集已核、语义完成、byte 层封顶）；ORIG 0x088a10cc。
int advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter(
    const char* path)
{
    // ORIG 0x088a10cc：三种段 [unit]/[skill]/[tower] 各自内联展开（各 ~190 指令），
    // [list] 处理结尾 push_back 当前 item（非 [/id] 绑定），且仅当 isValidRidableId
    // 通过并入 m_buyShopMap 后外层 do-while(bVar4) 才继续，否则返 0。下面按 ORIG
    // 汇编 LAB 布局逐段还原为三块独立代码。
    std::string token;
    bool ok = false;
    // [X2/X1 形·Main 批准配方] ORIG 全部 bool callee 检查为 `xor $0x1,%eax;
    // test %al,%al; j…`（0x088a1101/0x088a112c/0x088a117c/… 共 24 处），4.4 直写
    // !call/!ok 折成 test 单段缺 xor。[b28c-18-unbind] 绑定已全域清除
    // （N96+N18a+P168a④，判词见 DeleteInvalidItemScript::loadScript 头）。
    if (!(loadRDARScriptFile("", path)))
        return 0;

    for (;;)
    {
        // 外层跳过：处理 [star reset fee gold]，直到 [ridable id]（isRidable）或 EOF。
        bool isRidable = false;
        for (;;)
        {
            if (!(ScanType(token, true)))
                break;                          // ORIG 0x088a1136 jne → EOF 返 0
            if (token == "[star reset fee gold]")
            {
                for (;;)
                {
                    _StarRestFeeByGold fee;                 // ORIG 每轮 ctor(-0x238)
                    int key = ScanInt(&ok);                 // ORIG -0x23c
                    if (!(ok))
                        break;                              // 行首失败：结束本段回外层扫描头
                    int feeVal = ScanInt(&ok);              // ORIG -0x60
                    if (!(ok))
                        break;
                    fee.m_field0 = feeVal;                  // ORIG 校验后才 -0x60→-0x238
                    m_starRestMap.insert(std::make_pair(key, fee));
                }
                continue;
            }
            if (token == "[ridable id]")
            {
                isRidable = true;
                break;
            }
        }
        if (!isRidable)
            break;                                          // 文件耗尽 → 返 0

        BuyShopData shop;
        int ridableId = ScanInt(&ok);
        if (!(ok))
            break;                                          // ORIG !ok：bVar4=假 → 不插入返回
        if (!isValidRidableId(static_cast<advancealtar::RidableId::T>(ridableId)))
            break;                                          // ORIG 非法：bVar4=假 → 不插入返回
        shop.m_nField0 = ridableId;

        // LAB_088a12ab 段循环。[/ridable id] 或 EOF 跳出 → 插入 shop。
        for (;;)
        {
            if (!(ScanType(token, true)))
                break;                                      // EOF → 插入
            if (token == "[unit]")
            {
                for (;;)
                {
                    if (!(ScanType(token, true)))
                        break;                              // ORIG fail → 回段循环
                    if (token == "[/unit]")
                        break;
                    if (token != "[id]")
                        continue;
                    BuyUpgradeData item;
                    item.m_nItemId = ScanInt(&ok);
                    if (!(ok))
                        continue;                           // id 扫描失败 → 回段循环
                    for (;;)
                    {
                        if (!(ScanType(token, true)))
                            break;
                        if (token == "[unlock stage index]")
                        {
                            item.m_field4 = ScanInt(&ok);
                            if (!(ok))
                                break;
                            continue;
                        }
                        if (token == "[name]")
                        {
                            if (!(ScanStr(&item.m_strDesc)))
                                break;
                            continue;
                        }
                        if (token == "[field name]")
                        {
                            std::string fieldName;
                            for (;;)
                            {
                                if (!(ScanStr(&fieldName)))
                                    break;
                                int ft = getFieldType(fieldName);
                                if (ft != -1)
                                    item.m_fieldTypes.push_back(
                                        static_cast<FieldType::T>(ft));
                            }
                            continue;
                        }
                        if (token == "[list]")
                        {
                            FieldData fd;
                            FieldDataMap rowMap;
                            bool rowOk = true;
                            while (rowOk)
                            {
                                rowMap.clear();
                                for (size_t fi = 0;
                                     fi < item.m_fieldTypes.size(); ++fi)
                                {
                                    FieldType::T ft = item.m_fieldTypes[fi];
                                    fd.m_field0 = ft;
                                    if (getdataType(ft) == 0)
                                    {
                                        std::string s;
                                        if (!ScanStr(&s))
                                                fd.m_str = s;
                                        else
                                            rowOk = false;
                                        if (!rowOk)
                                            break;
                                    }
                                    else
                                    {
                                        fd.m_value = ScanInt(&ok);
                                        if (!(ok))
                                        {
                                            rowOk = false;
                                            break;
                                        }
                                    }
                                    if (rowOk)
                                        rowMap.insert(std::make_pair(ft, fd));
                                }
                                if (rowOk)
                                {
                                    int key = rowMap[FieldType::FT_LEVEL].m_value;
                                    item.m_listMap.insert(
                                        std::make_pair(key, rowMap));
                                }
                            }
                            shop.m_vecT0.push_back(item);   // ORIG push_back 绑定 [list] 尾
                            continue;
                        }
                        if (token == "[/id]")
                            break;
                    }
                }
                continue;
            }
            if (token == "[skill]")
            {
                for (;;)
                {
                    if (!(ScanType(token, true)))
                        break;
                    if (token == "[/skill]")
                        break;
                    if (token != "[id]")
                        continue;
                    BuyUpgradeData item;
                    item.m_nItemId = ScanInt(&ok);
                    if (!(ok))
                        continue;
                    for (;;)
                    {
                        if (!(ScanType(token, true)))
                            break;
                        if (token == "[unlock stage index]")
                        {
                            item.m_field4 = ScanInt(&ok);
                            if (!(ok))
                                break;
                            continue;
                        }
                        if (token == "[is settable slot]")
                        {
                            int v = ScanInt(&ok);
                            if (!(ok))
                                break;
                            item.m_field8 = (v != 0);
                            continue;
                        }
                        if (token == "[name]")
                        {
                            if (!(ScanStr(&item.m_strDesc)))
                                break;
                            continue;
                        }
                        if (token == "[field name]")
                        {
                            std::string fieldName;
                            for (;;)
                            {
                                if (!(ScanStr(&fieldName)))
                                    break;
                                int ft = getFieldType(fieldName);
                                if (ft != -1)
                                    item.m_fieldTypes.push_back(
                                        static_cast<FieldType::T>(ft));
                            }
                            continue;
                        }
                        if (token == "[list]")
                        {
                            FieldData fd;
                            FieldDataMap rowMap;
                            bool rowOk = true;
                            while (rowOk)
                            {
                                rowMap.clear();
                                for (size_t fi = 0;
                                     fi < item.m_fieldTypes.size(); ++fi)
                                {
                                    FieldType::T ft = item.m_fieldTypes[fi];
                                    fd.m_field0 = ft;
                                    if (getdataType(ft) == 0)
                                    {
                                        std::string s;
                                        if (!ScanStr(&s))
                                                fd.m_str = s;
                                        else
                                            rowOk = false;
                                        if (!rowOk)
                                            break;
                                    }
                                    else
                                    {
                                        fd.m_value = ScanInt(&ok);
                                        if (!(ok))
                                        {
                                            rowOk = false;
                                            break;
                                        }
                                    }
                                    if (rowOk)
                                        rowMap.insert(std::make_pair(ft, fd));
                                }
                                if (rowOk)
                                {
                                    int key = rowMap[FieldType::FT_LEVEL].m_value;
                                    item.m_listMap.insert(
                                        std::make_pair(key, rowMap));
                                }
                            }
                            shop.m_vecT1.push_back(item);
                            continue;
                        }
                        if (token == "[/id]")
                            break;
                    }
                }
                continue;
            }
            if (token == "[tower]")
            {
                for (;;)
                {
                    if (!(ScanType(token, true)))
                        break;
                    if (token == "[/tower]")
                        break;
                    if (token != "[id]")
                        continue;
                    BuyUpgradeData item;
                    item.m_nItemId = ScanInt(&ok);
                    if (!(ok))
                        continue;
                    for (;;)
                    {
                        if (!(ScanType(token, true)))
                            break;
                        if (token == "[unlock stage index]")
                        {
                            item.m_field4 = ScanInt(&ok);
                            if (!(ok))
                                break;
                            continue;
                        }
                        if (token == "[name]")
                        {
                            if (!(ScanStr(&item.m_strDesc)))
                                break;
                            continue;
                        }
                        if (token == "[field name]")
                        {
                            std::string fieldName;
                            for (;;)
                            {
                                if (!(ScanStr(&fieldName)))
                                    break;
                                int ft = getFieldType(fieldName);
                                if (ft != -1)
                                    item.m_fieldTypes.push_back(
                                        static_cast<FieldType::T>(ft));
                            }
                            continue;
                        }
                        if (token == "[list]")
                        {
                            FieldData fd;
                            FieldDataMap rowMap;
                            bool rowOk = true;
                            while (rowOk)
                            {
                                rowMap.clear();
                                for (size_t fi = 0;
                                     fi < item.m_fieldTypes.size(); ++fi)
                                {
                                    FieldType::T ft = item.m_fieldTypes[fi];
                                    fd.m_field0 = ft;
                                    if (getdataType(ft) == 0)
                                    {
                                        std::string s;
                                        if (!ScanStr(&s))
                                                fd.m_str = s;
                                        else
                                            rowOk = false;
                                        if (!rowOk)
                                            break;
                                    }
                                    else
                                    {
                                        fd.m_value = ScanInt(&ok);
                                        if (!(ok))
                                        {
                                            rowOk = false;
                                            break;
                                        }
                                    }
                                    if (rowOk)
                                        rowMap.insert(std::make_pair(ft, fd));
                                }
                                if (rowOk)
                                {
                                    int key = rowMap[FieldType::FT_LEVEL].m_value;
                                    item.m_listMap.insert(
                                        std::make_pair(key, rowMap));
                                }
                            }
                            shop.m_vecT2.push_back(item);
                            continue;
                        }
                        if (token == "[/id]")
                            break;
                    }
                }
                continue;
            }
            if (token == "[/ridable id]")
                break;
        }
        // ORIG LAB_088a267d：valid 路径下 [/ridable id] 或 EOF 都把 shop 入 map。
        m_buyShopMap.insert(std::make_pair(shop.m_nField0, shop));
    }
    return 0;
}

// ---- advancealtar::BuyUpgradeData::getFieldDataPoint（ORIG T 0x088a2e4c）----
// m_listMap.find(level) 未命中→0；it->second.find(ft) 未命中→0；命中返回
// &fit->second（FieldData*）。报告 advancealtar__BuyUpgradeData/getFieldDataPoint.md。
advancealtar::FieldData* advancealtar::BuyUpgradeData::getFieldDataPoint(
    int level, FieldType::T ft)
{
    std::map<int, FieldDataMap>::iterator it = m_listMap.find(level);
    // ORIG 0x88a2e9d 外层 ne+je→0x88a2f13 mov$0（外层尾岛）；
    // 内层 0x88a2efa je→0x88a2f0c mov$0;jmp（内层岛），命中 0x88a2f07 add $4;jmp merge
    if (it != m_listMap.end())
    {
        std::map<FieldType::T, FieldData>::iterator fit = it->second.find(ft);
        if (fit != it->second.end())
            return &fit->second;
        return 0;
    }
    return 0;
}

// ---- advancealtar::BuyShopData::getBuyUpgradeDataVec（ORIG T 0x088a105c）----
// type 0/1/2 → &m_vecT0/T1/T2；其它 → 0。报告 advancealtar__BuyShopData/。
std::vector<advancealtar::BuyUpgradeData>*
advancealtar::BuyShopData::getBuyUpgradeDataVec(AdvanceAltarShopType::T type)
{
    switch (type)
    {
    case AdvanceAltarShopType::T_0: return &m_vecT0;
    case AdvanceAltarShopType::T_1: return &m_vecT1;
    case AdvanceAltarShopType::T_2: return &m_vecT2;
    default: return 0;
    }
}

// ---- advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec
// （ORIG T 0x088a0fe8）：m_buyShopMap.find(key) 未命中→0；命中转调
// BuyShopData::getBuyUpgradeDataVec(type)。报告同名 md。
std::vector<advancealtar::BuyUpgradeData>*
advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec(
    int key, AdvanceAltarShopType::T type)
{
    std::map<int, BuyShopData>::iterator it = m_buyShopMap.find(key);
    // ORIG 0x88a102c 调 ne、命中块 fall-through（0x88a1035），je→0x88a1054 mov$0 尾岛
    if (it != m_buyShopMap.end())
        return it->second.getBuyUpgradeDataVec(type);
    return 0;
}

// ============================================================================
// advancealtar::RewardParameter::importAchievementRewardParameter
// （ORIG 0x0889afb0 T）[reward data]…[/reward data] 块解析进
// AchievementRewardData 后按 m_type 插入 m_rewardMap。错误码：
// type 扫描 0x362 / type<=0 0x364 / 难度串 0x36a / 难度枚举 0x36d /
// 清空次数 0x373 / 图标 0x37a / 奖励计数 0x391 / 标题 0x399 /
// 邮政标题 0x39e / 正文 0x3a3。[reward item] 三元组仅 {type,_,itemId}
// 进入向量（第三值写入越界临时不入拷贝，见报告注记）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：N82/N93 —— 大件槽序平移（语义完成；调用点/字段访问已核，4.1/4.4 槽复用策略差）；ORIG 0x0889afb0。
int advancealtar::RewardParameter::importAchievementRewardParameter(
    const char* path)
{
    std::string token;
    bool ok = false;
    reset();
    if (!loadRDARScriptFile("", path))
        return 0;

    while (ScanType(token, true))
    {
        if (token != "[reward data]")
            continue;

        AchievementRewardData data;
        while (ScanType(token, true))
        {
            if (token == "[/reward data]")
            {
                m_rewardMap.insert(std::make_pair(data.m_type, data));
                break;
            }
            if (token == "[type]")
            {
                data.m_type = ScanInt(&ok);
                if (!ok)
                    return 0x362;
                if (!(data.m_type > 0))
                    return 0x364;
                continue;
            }
            if (token == "[stage difficulty]")
            {
                std::string diffName;
                if (!ScanStr(&diffName))
                    return 0x36a;
                StageDifficulty::T diff;
                if (!StageDifficulty::getDifficulty(diffName, diff))
                    return 0x36d;
                data.m_difficulty = static_cast<int>(diff);
                continue;
            }
            if (token == "[stage clear count]")
            {
                data.m_stageClearCount = ScanInt(&ok);
                if (!ok)
                    return 0x373;
                continue;
            }
            if (token == "[icon image]")
            {
                data.m_iconImage = ScanInt(&ok);
                if (!ok)
                    return 0x37a;
                continue;
            }
            if (token == "[reward item]")
            {
                for (;;)
                {
                    _RewardItem item;
                    std::string typeName;
                    RewardItemType::T itemType;
                    if (!ScanStr(&typeName))
                        break;                       // 静默退出本段
                    if (!RewardItemType::getRewardItemType(typeName, itemType))
                        break;
                    item.m_type = itemType;
                    item.m_value2 = ScanInt(&ok);    // ORIG 写 scratch+8
                    if (!ok)
                        break;
                    // ORIG 第二次 ScanInt 写入 12 字节临时对象之外，
                    // push_back 仅拷贝前 12 字节，计数值不入向量；
                    // 此处保留解析推进语义但不存储。
                    ScanInt(&ok);
                    if (!ok)
                        return 0x391;
                    data.m_rewardItems.push_back(item);
                }
                continue;
            }
            if (token == "[reward title]")
            {
                if (!ScanStr(&data.m_rewardTitle))
                    return 0x399;
                continue;
            }
            if (token == "[postal title]")
            {
                if (!ScanStr(&data.m_postalTitle))
                    return 0x39e;
                continue;
            }
            if (token == "[postal letter]")
            {
                if (!ScanStr(&data.m_postalLetter))
                    return 0x3a3;
                continue;
            }
        }
    }
    return 0;
}

// ORIG C1 0x08371a26：构造 7 容器成员后调 clear()（+0x84 call clear）。
PowerParameterScript::PowerParameterScript()
{
    clear();
}
PowerParameterScript::~PowerParameterScript()
{
    // ORIG D1：逆序析构 7 个容器成员（+0xc8/+0xbc/+0xac/+0x9c/+0x70/+0x38/+0x20）
}

// ---- PowerParameterScript 查询（ORIG T 0x08a59c6e / 0x08a59d58）----
// GetPenalty：遍历 m_vecBC（+0xbc），取首个 m_field4 >= victory 的条目按值返回；
// 无匹配时返回最后一条（ORIG memmove(&vec[size-1], 0x10)）；空表未定义。
// [b28d-solo-CDataManagerScripts] AE：P13 反向 —— ORIG 0x08a59ce3 在循环条件处每轮调 end()（end 临时 -0xc，不缓存），改 for-init 内 begin + 条件 end() 后逐条对齐；fresh clean 71=71；ORIG 0x08a59c6e。
stVictoryPenalty PowerParameterScript::GetPenalty(int victory)
{
    // [b28d-solo-CDataManagerScripts] P13 反向：ORIG 0x08a59ce3 循环条件处每轮
    // 调 end()（end 临时槽 -0xc），不缓存；it 槽 -0x10。
    for (std::vector<stVictoryPenalty>::iterator it = m_vecBC.begin();
         it != m_vecBC.end(); ++it)
    {
        stVictoryPenalty entry = *it;
        if (entry.m_field4 >= victory)
            return entry;
    }
    // ORIG：无条件整体赋值（memmove 16B，无 empty 保护；size==0 时为
    // 与 ORIG 相同的越界读，勿加保护）。
    return m_vecBC[m_vecBC.size() - 1];
}

// GetBreakBonus（ORIG T 0x08a59d58）：m_vecC8（+0xc8 连破奖励表）；
// winCount < size → v[winCount]，否则 v[size-1]（越界取末元素）。
int PowerParameterScript::GetBreakBonus(int winCount)
{
    // ORIG 0x8a59d5e：m_vecC8 不物化引用（每处 call 现场 `add $0xc8,%edx`）；
    // size→-0xc；0x8a59d77 `cmp -0xc,%eax; jl 0x8a59d98` ⇒ winCount>=size 为
    // fall-through 首分支（ix(size-1) @0x8a59d8f），jl 目标=ix(winCount)。
    int size = m_vecC8.size();
    if (winCount >= size)
        return m_vecC8[size - 1];
    return m_vecC8[winCount];
}

// ============================================================================
// checkPowerWarServer（ORIG T 0x08a59db4）：遍历 m_powerWarServerList
// （+0x9c，vector<pair<int,int>>），pair.first==a && pair.second==b → 1，
// 否则 0。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①`std::pair<int,int> p;` 默认构造于 begin 前（P50 复活；ORIG 0x81b5c8e=C1Ev）；②end 缓存（P13）；③`p = *it` 结构赋值替代两次 ptEv；ORIG 0x08a59db4。
bool PowerParameterScript::checkPowerWarServer(int a, int b)
{
    std::pair<int, int> p;
    std::vector<std::pair<int, int> >::iterator it = m_powerWarServerList.begin();
    std::vector<std::pair<int, int> >::iterator e = m_powerWarServerList.end();
    for (; it != e; ++it)
    {
        p = *it;
        if (p.first == a && p.second == b)
            return true;
    }
    return false;
}

// ---- SecretShopScript（成员默认构造）----

// ---- SecretShopScript（成员默认构造）----
SecretShopScript::SecretShopScript()
{
    clear();   // ORIG C1 0x83720f4：构造 5 容器成员后调 clear()（写 +0x80/+0x84）
}
SecretShopScript::~SecretShopScript() {}

// ---- stTitleBookInfo（ORIG C1 0x837d828 / D1 0x837d906）----
stTitleBookInfo::stTitleBookInfo()
    : m_vec0(), m_vecC(), m_map18(), m_vec30(), m_vec3c(), m_vec48()
{
}
stTitleBookInfo::~stTitleBookInfo() {}

// ---- stItemMakingSkill（ORIG C1 0x8370ff2 / clear 0x837105a / D1 0x837dce2）----
stItemMakingSkill::stItemMakingSkill() : m_map0(), m_vec18()
{
    clear();   // ORIG C1：构造 map/vector 后调 clear()（清三 int）
}
stItemMakingSkill::~stItemMakingSkill() {}
// ORIG clear 0x837105a：map.clear()、vector.clear()、+0x24/+0x28/+0x2c 三 int 清零。
void stItemMakingSkill::clear()
{
    m_map0.clear();
    m_vec18.clear();
    m_exp = 0;
    m_bonusExp = 0;
    m_levelLimit = 0;
}

// ---- ChoiceItemInfo / ImageCommunicationData（无析构声明）----
ChoiceItemInfo::ChoiceItemInfo()
{
    clear();
}

void ChoiceItemInfo::clear()
{
    m_checked = 0;
    m_choice = static_cast<unsigned int>(-1);
}
ImageCommunicationData::ImageCommunicationData()
{
    m_charge = 0;
}

// ---- ARAD::SCRIPT::AradJumping_Script----
// W7_BuildHarmonize 去重：生命周期唯一 owner 为 Arad_Script.cpp
// （同文件含 getRDARScriptData/importScript 等同类方法）。

// ---- EventEtcScript（ORIG C1 0x8372e66 / clear 0x8372ece / D1 0x837dda0）----
EventEtcScript::EventEtcScript() : m_vec0(), m_mapC()
{
    clear();
}
EventEtcScript::~EventEtcScript() {}
void EventEtcScript::clear()
{
    m_vec0.clear();
    m_vec0.reserve(7);
}

// ---- stReturnUserRewardScript / stBingoScript / stNewAccountLevelUpToJobScript----
// ORIG C1 0x8371544：两个 int 置 0 后构造 + clear 内嵌 stLevelUpRewardItemScript。
stReturnUserRewardScript::stReturnUserRewardScript()
    : m_field0(0), m_field4(0)
{
    m_levelUp.clear();   // ORIG C1 0x8371544 调 stLevelUpRewardItemScript::clear()
}
stReturnUserRewardScript::~stReturnUserRewardScript() {}
stBingoScript::stBingoScript() : m_map0(), m_map18(), m_map30(), m_vec48()
{
    // ORIG C1：构造 4 容器后各自 clear()
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_vec48.clear();
}
stBingoScript::~stBingoScript() {}
stNewAccountLevelUpToJobScript::stNewAccountLevelUpToJobScript()
    : m_stepReward(), m_levelUp18(), m_levelUp30()
{
}
stNewAccountLevelUpToJobScript::~stNewAccountLevelUpToJobScript() {}
// ---- CWorldMapList / StageMapList / WarAreaCounter----
// ORIG CWorldMapList（0x837480a ctor / 0x837481e dtor / 0x8374752 destroy）即一个
// map<int,CWorldMap*>，成员 m_map 位于偏移 0。
CWorldMapList::CWorldMapList() : m_map() {}
CWorldMapList::~CWorldMapList()
{
    destroy();  // ORIG D1 0x837481e：destroy() 后析构 map
}

// ORIG 0x8374752：遍历 delete 各 CWorldMap*（D1 + operator delete），最后清空 map。
// [b28d-solo-CDataManagerScripts] AE：①`iterator it;` 默认构造 + `it = m_map.begin()`（P50 复活）+ `iterator e = m_map.end()` 缓存；②无条件 `delete worldMap`（P141）⇒ 51=51 全等；ORIG 0x08374752。
void CWorldMapList::destroy()
{
    // ORIG 0x8374752：局部变量暂存 second、delete、清零局部（不写回 map 值）。
    std::map<int, CWorldMap*>::iterator it;
    std::map<int, CWorldMap*>::iterator e = m_map.end();
    it = m_map.begin();
    for (; it != e; ++it)
    {
        CWorldMap* worldMap = it->second;
        delete worldMap;
        worldMap = 0;
    }
    m_map.clear();
}

// ORIG 0x83647a2：map<int,CWorldMap*>::find(idx)，命中返回 second，否则 0。
CWorldMap* CWorldMapList::find_world_map(int idx) const
{
    // ORIG 0x83647a8：it 先默认构造（C1Ev @-0x10）；find 结果落临时 -0x1c 后
    // 0x83647cf-0x83647d2 单字赋值拷贝 → 复现"声明+赋值"两段式。
    std::map<int, CWorldMap*>::const_iterator it;
    it = m_map.find(idx);
    return (it == m_map.end()) ? 0 : it->second;
}

// ---- CWorldMap 访问器（ORIG 无独立报告；按 objdump 直读字节字段实现） ----
bool CWorldMap::IsInHellDungeon()   // ORIG W 0x830e6ba：movzbl 0x4(%eax)（非 const，直读 bool）
{
    return m_isHellDungeon;
}

bool CWorldMap::hasDeathTower() const     // ORIG W 0x822b520：movzbl 0xc(%eax)（直读 bool）
{
    return m_hasDeathTower;
}

// ORIG W 0x0822b514 _ZNK9CWorldMap18getDungeonMinLevelEv（const）：返回
// m_dungeonIndex（this+0x8）；StartGame::check_error 消费。
int CWorldMap::getDungeonMinLevel() const
{
    return m_dungeonIndex;
}

// ORIG W 0x822b52c _ZNK9CWorldMap10hasDungeonEj（0x85 字节）：
// std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) != end()。
// 实参为 unsigned int（find 模板 _ZSt4findI...jE...）；调用方
// DisPatcher_SelectDungeon::process 传 (int)(short)MSG_CHARAC_SELECT+0xd。
bool CWorldMap::hasDungeon(unsigned int dungeonIdx) const
{
    return std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) !=
           m_dungeonList.end();
}

StageMapList::StageMapList() : m_firstStageId(0) {}
StageMapList::~StageMapList() {}
// ORIG C1 0x083708c6：构造 +0x120 的 map 后调用 clear()。
WarAreaCounter::WarAreaCounter()
{
    clear();
}

// ORIG W 0x083707a4：memset +0x000/0x60/0xc0 三段数组、清 +0xe8..+0xf4 四个
// int、memset +0x0f8 段、map(+0x120).clear()、清 +0x140；其余（wpPenalty/
// monsterExpConst/warRoomCount/pad_2d4）不动。
void WarAreaCounter::clear()
{
    memset(m_counterTable, 0, sizeof(m_counterTable));
    memset(m_timeHourList, 0, sizeof(m_timeHourList));
    memset(m_doorFee, 0, sizeof(m_doorFee));
    m_curHourIdx = 0;
    m_banInterval = 0;
    m_kickPoint = 0;
    m_readyCount = 0;
    memset(m_warAreaDungeon, 0, sizeof(m_warAreaDungeon));
    m_wpPerMonsterHp.clear();
    m_num = 0;
    // ORIG 尾段：+0x140=num 后 memset(+0x144,0,0x3c0)、清 +0x138、+0x13c。
    memset(m_warRoomCount, 0, sizeof(m_warRoomCount));
    m_wpPenalty = 0;
    m_monsterExpConst = 0;
}
WarAreaCounter::~WarAreaCounter() {}

// ---- stEquipmentGradeSample（ORIG C1 0x837c810 / D1 0x837c884：
//      5 个 vector<unsigned int> 依 +0x00→+0x30 构造、逆序析构）----
// ORIG C1 0x837c810：体为空，5 个 vector 由编译器隐式数组构造（含 EH 逆序析构）。
stEquipmentGradeSample::stEquipmentGradeSample()
{
}
stEquipmentGradeSample::~stEquipmentGradeSample() {}

// ---- stOneEmotionData / stEmotionTypeTagsInfo（自 GameStubs.cpp 迁移）----
// ORIG 0x891b560：构造 strings + vector<string>，再 clear
stOneEmotionData::stOneEmotionData() : m_8(), m_c(), m_14(), m_18()
{
    clear();
}
// ORIG 0x891b640（stOneEmotionData::operator=）：逐成员复制
// 0x00/0x04 int、0x08/0x0c/0x14 string、0x10/0x11/0x12 char、0x18 vector。
// 注意：+0x13 为 padding（m_pad13），ORIG 不复制；镜像布局须显式定义以
// 覆盖编译器隐式逐成员赋值（隐式版会复制 m_pad13）。
stOneEmotionData& stOneEmotionData::operator=(const stOneEmotionData& rhs)
{
    m_0 = rhs.m_0;
    m_4 = rhs.m_4;
    m_8 = rhs.m_8;
    m_c = rhs.m_c;
    m_10 = rhs.m_10;
    m_11 = rhs.m_11;
    m_12 = rhs.m_12;
    m_14 = rhs.m_14;
    m_18 = rhs.m_18;
    return *this;
}
// ORIG W 0x8370bb0（0x9b 字节）：成员自动析构，标量无操作。
stOneEmotionData::~stOneEmotionData() {}
// ORIG W 0x891b61a：clear —— 清零 +0x00/+0x04、string clear(0x8/0xc)、
// 0x891b652/659/667 逐字节清 0x10/0x11/0x12（无 0x13 存储 ⇒ pad 不清）、
// string clear(0x14)、vector clear(0x18)。
void stOneEmotionData::clear()
{
    m_0 = 0;
    m_4 = 0;
    m_8.clear();
    m_c.clear();
    m_10 = 0;
    m_11 = 0;
    m_12 = 0;
    m_14.clear();
    m_18.clear();
}
// ORIG 0x8370c4c：构造两个 vector 后 clear
stEmotionTypeTagsInfo::stEmotionTypeTagsInfo() : m_tags(), m_data()
{
    clear();
}
stEmotionTypeTagsInfo::~stEmotionTypeTagsInfo() {}
void stEmotionTypeTagsInfo::clear()
{
    // ORIG 0x8370cfa
    m_tags.clear();
    m_data.clear();
}

// ORIG C1 0x837c8c6：仅构造 +0x8/+0x20/+0x38 三容器，+0x0/+0x4 不初始化。
stConditionEventInfo::stConditionEventInfo()
    : m_map8(), m_multiMap20(), m_multiMap38()
{
}
stConditionEventInfo::~stConditionEventInfo() {}

// ---- pvp_channel_info_t（ORIG C1 _ZN18pvp_channel_info_tC1Ev 0x8370912：
//      将 +0x00..+0x20 共 9 个 dword 清零；+0x24 m_pvpMissionSystem 不初始化）----
pvp_channel_info_t::pvp_channel_info_t()
{
    // ORIG 0x8370912：+0x00..+0x20 共 9 个 dword 清零；+0x24 不初始化。
    m_field0 = 0;
    m_minGrade = 0;
    m_maxGrade = 0;
    m_upgradeRevision = 0;    // +0x0c（ORIG 先清 +0x0c 再清 +0x10）
    m_channelType = 0;        // +0x10
    m_minLevel = 0;
    m_maxLevel = 0;
    m_field1c = 0;
    m_field20 = 0;
}

// ============================================================================
// STExpertJobScript::isBoundaryExpValue（TSV 修复，ORIG 0x0849f420 T）。
// ORIG：遍历 vector<ExpertJobExp>（元素尺寸 12，见 _ZNSt6vectorI12ExpertJobExpSaIS0_EEixEj
// 乘法 i*12），任一元素首整数字段 == exp 返回 true，否则 false。
// 权威声明见 STExpertJobScript.h（CUser::GetCurExpertJobLevel 真实调用）。
// ============================================================================

// [b28d-solo-CDataManagerScripts] NEAR(1)：N22 —— 4.4 恒 `cmp %ebx,%eax`（dst=call 结果/字段侧），ORIG 4.1 `cmp %eax,%ebx`（dst=局部 e）；交换源码操作数两形均不翻（P248 三档试尽）；语义等价；ORIG 0x0849f420。
bool STExpertJobScript::isBoundaryExpValue(int exp)
{
    // [b28c-L18] NEAR（fresh 32=32，助记符序列全等，仅 cmp 操作数序差 2 行）：
    // ORIG 把 int 形参 exp 每轮 `mov 0xc(%ebp),%ebx` 提升 %ebx 跨 operator[]
    // call 保活（push ebx/sub $0x24 + `cmp %eax,%ebx`）。修法=裸 `register int e`
    // **置于循环体内**（声明在体外则 %ebx 装载位置前移，d 4；无裸 register 则
    // 4.4 折 `cmp 0xc(%ebp),%eax` 内存直比，d 10）。残余 N18b（4.4 恒
    // `cmp MEM/reg,reg` 目标寄存器为字段侧，方向翻转两形均不可改）。
    // 三条件齐备（跨 call 保活 / ORIG 目标 callee-saved / fresh 非劣化）故保留
    // 裸 register（C28L26 分场景口径）。
    size_t i = 0;
    for (; i < m_expVec.size(); ++i)
    {
        register int e = exp;
        if (m_expVec[i].m_exp == e)
            return true;
    }
    return false;
}

// STExpertJobScript::GetLevel（TSV 修复发明符号：_ZN17STExpertJobScript8GetLevelEj，ORIG 0x08693ba0）。
// ORIG：遍历 vector<ExpertJobExp>，返回首个 v[i].m_exp > exp 的 i+1；无则返回 size()。
int STExpertJobScript::GetLevel(unsigned int exp)
{
    // ORIG 0x8693ba6：i 于函数级声明（-0xc）；0x8693bee 退出 fall-through
    // 直接 mov -0xc(%ebp),%eax 返回 i（循环尾 i==size，不再 call size）。
    size_t i;
    for (i = 0; i < m_expVec.size(); ++i)
    {
        if ((unsigned int)m_expVec[i].m_exp > exp)
            return (int)i + 1;
    }
    return (int)i;
}

// STExpertJobScript::GetCompoundRate（ORIG 0x0849f366 W）。
// 遍历 m_compoundRateVec（+0x1f4）：返回首个 stCompoundRate.m_level >= level 的元素；
// 全表都 < level 时返回末元素；空表返回 0。调用方（GetSuccessRate）读 +4/+8/+0xc。
// [b28d-solo-CDataManagerScripts] AE：①P13 缓存 end（`iterator e = m_compoundRateVec.end();` 具名初始化）⇒ begin/end 槽 -0xc/-0x10 对齐；②P167 —— ORIG 三处调 `operator*`(deEv)，`it->m_level` 改 `(*it).m_level` 消 ptEv 差集；fresh clean 56=56；ORIG 0x0849f366。
stCompoundRate* STExpertJobScript::GetCompoundRate(int level)
{
    if (m_compoundRateVec.empty())
        return 0;
    std::vector<stCompoundRate>::iterator it = m_compoundRateVec.begin();
    std::vector<stCompoundRate>::iterator e = m_compoundRateVec.end();
    for (; it != e; ++it)
    {
        if ((*it).m_level >= level)
            return &(*it);
    }
    --it;  // 全部 < level：取末元素
    return &(*it);
}

// STExpertJobScript::GetRecipeInfoIndex（ORIG 0x0832b726 W）。
// 在 m_recipeIndexMap（+0x218，map<unsigned long,int>）按 itemIndex 查；
// 找到返回 mapped int（recipe 信息索引），否则 -1。
int STExpertJobScript::GetRecipeInfoIndex(unsigned long itemIndex)
{
    std::map<unsigned long, int>::iterator it = m_recipeIndexMap.find(itemIndex);
    // ORIG 0x832b765 ne + 命中 fall-through；未命中 je→0x832b78f mov $-1 尾岛
    if (it != m_recipeIndexMap.end())
        return it->second;
    return -1;
}

// STExpertJobScript::GetRecipeInfo（ORIG 0x0832b6b6 W）。
// 在 m_recipeInfoMap（+0x200，map<int,stRecipeInfo>）按 index 查；
// 找到返回 &it->second，否则 0。
stRecipeInfo* STExpertJobScript::GetRecipeInfo(int index)
{
    std::map<int, stRecipeInfo>::iterator it = m_recipeInfoMap.find(index);
    // ORIG 0x832b6f9 ne + 命中 fall-through；je→0x832b71f mov$0 尾岛
    if (it != m_recipeInfoMap.end())
        return &it->second;
    return 0;
}
// ORIG D1 0x837c3d4 (W)：析构成员 map<int,rewardInfoList>。
// ---- GuildParameterScript::getGuildExpWeight（ORIG T 0x089795de）----
// m_map44.find(level) 命中返回 second，否则 0（CUser::FatigueUp 公会经验权重，
// 与 0x08979864 getExpWeight 的 m_vecE4 扫表路径不同；ORIG 返回 int）。
int GuildParameterScript::getGuildExpWeight(int level)
{
    std::map<int, int>::iterator it = m_map44.find(level);
    if (it != m_map44.end()) return it->second;
    return 0;
}

// ---- GuildParameterScript::getGuildAgitUpgradeInfo（ORIG T 0x089796dc）----
// m_vec2c（STAgitUpgradeInfo[agit upgrade info]）取下标 level-1；
// level-1 >= size 返回 NULL（ORIG cmp/setl 分支一致）。
STAgitUpgradeInfo* GuildParameterScript::getGuildAgitUpgradeInfo(int level)
{
    if (level - 1 < (int)m_vec2c.size())
        return &m_vec2c.at(level - 1);   // ORIG 用 at（含界检），非 operator[]
    return 0;
}

// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----

// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----
// 自高向低扫 m_vecE4（vector<pair<int,double>>，等级阈值→经验权重），
// 首个 first<=guildLevel 的 second；全不命中取 [0].second（ORIG 无判空，照抄）。
double GuildParameterScript::getExpWeight(int guildLevel)
{
    for (int i = (int)m_vecE4.size() - 1; i >= 0; --i)
    {
        if (m_vecE4[i].first <= guildLevel)
            return m_vecE4[i].second;
    }
    return m_vecE4[0].second;
}

// ---- GuildParameterScript::getGuildLevelUpParam（ORIG T 0x08979648）----
// level∈[1,16] 返回 m_levelUpCost+(level-1)*8 的地址（按 int 口径），否则 0。
int GuildParameterScript::getGuildLevelUpParam(int level)
{
    // [b28c-L29] ORIG 0x08979648：负极性短路 `level<=0||level>0x10` 的 4.4 复现
    // 形=正极性包裹（P214①）：&& 两失败项各自 jle/jg 跳同一 return 0 尾块；
    // 返回 m_levelUpCost+(level-1) 指针算术形生 lea 0x5c(%eax),%edx 链
    // （探针 l29_probe.cpp f5 逐条复现）。
    if (level > 0 && level <= 0x10)
        return reinterpret_cast<int>(m_levelUpCost + level - 1);
    return 0;
}

// ---- GuildParameterScript::getLevelupPrice（ORIG T 0x089797a4）----
// 自高向低扫 m_vecF0，首个 first<=memberCnt（members 阈值）的
// second[level]；全不命中取 [0].second[level]；结果 ×10000。
// [b28d-solo-CDataManagerScripts] AE：删 tbl 引用缓存与 priceVecIdx（P183 每点全表达式 m_vecF0[i] + 命中直接 return）⇒ 循环变量 -0xc 复用、无 -0x14/-0x10 缓存槽；ORIG 0x089797a4。
int GuildParameterScript::getLevelupPrice(int level, int memberCnt)
{
    for (int i = (int)m_vecF0.size() - 1; i >= 0; --i)
    {
        if (m_vecF0[i].first <= memberCnt)
            return m_vecF0[i].second[level] * 10000;
    }
    return m_vecF0[0].second[level] * 10000;
}

// ---- GuildParameterScript::getGuildExpBook（ORIG T 0x08979672）----
// m_map10.find(key) 命中返回 second，否则 0（W7-BigThree increase_status case 0x10 依赖）。
int GuildParameterScript::getGuildExpBook(int key)
{
    std::map<int, int>::iterator it = m_map10.find(key);
    if (it != m_map10.end()) return it->second;
    return 0;
}
// ORIG _Z14IS_PVP_CHANNELi 0x088dd255（W）：线性 cmpl/je 链，命中集合
// {4,6,5,7,8,9,10,13,14,15,16} 返回 1，否则 0（P45 短路线性判定同构）。
// 定义落本 TU：声明同文件（CDataManagerScripts.cpp 顶部），调用点在
// GetPvPChannelNumber 0x088dcc28（按 bool 语义消费）。
bool IS_PVP_CHANNEL(int channelType)
{
    return (channelType == 4) || (channelType == 6) ||
           (channelType == 5) || (channelType == 7) ||
           (channelType == 8) || (channelType == 9) ||
           (channelType == 10) || (channelType == 13) ||
           (channelType == 14) || (channelType == 15) ||
           (channelType == 16);
}
