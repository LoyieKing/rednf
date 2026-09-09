// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）；
// 逐函数对照 docs/class_func_reports/ServerParameterScript.md。
// ============================================================================

#include "ServerParameterScript.h"

#include <string.h>
#include <algorithm>

#include "DNFLexWrapper.h"

#include <stdio.h>

#include "LogManager.h"
#include "CItemAmplifier.h"   // ENUM_RARITY（[drop upgrade rarity] 段 ORIG
                              // make_pairIR11ENUM_RARITYR15ItemFarmingData 实证）

// SetLightServerFlag（ORIG 0x8a8cba5，_Z18SetLightServerFlagb）定义于
// CDataManagerScripts.cpp；此处仅声明供本 TU 真实调用。
void SetLightServerFlag(bool flag);

// ORIG W 0x0836fc5a：按声明序构造 10 个 stLevelCond4PremiumGoldCard，
// 其余标量/填充区保持未初始化。
ServerParameterScript::stPremiumGoldCardEvent::stPremiumGoldCardEvent()
{
}

ServerParameterScript::ServerParameterScript()
{
    clear();
}

ServerParameterScript::~ServerParameterScript()
{
}

void ServerParameterScript::clear()
{
    // b28d-solo-ServerParameterScript: DIFF→修（P191 循环变量双初始化 + 复用）
    // ORIG 0x0836f38c 帧 0x28：`movl $0x0,-0x18` 出现两次（函数头 `int i=0;` +
    // 首个 `for (i=0;…)`），此后 6 个一维循环与 0x1a 循环**复用同一 -0x18**；
    // emblem 循环独占 -0x14、二维循环 -0x10/-0xc。旧代码每循环各声明新 i（帧 0x48、
    // 槽 -0x30..-0x1c）。
    m_358 = 0;
    int i = 0;   // ORIG 在 m_358=0 之后（双 movl $0,-0x18 的第二条为 for-init）
    for (i = 0; i < 4; ++i)
    {
        m_versionBytes[i] = 0;
    }
    m_expRate360[0] = 1.0f;
    m_expRate360[1] = 1.0f;
    for (i = 0; i < 4; ++i)
    {
        m_expGroup368[i] = 1.0f;
    }
    for (i = 0; i < 4; ++i)
    {
        m_expGroup378[i] = 1.0f;
    }
    for (i = 0; i < 4; ++i)
    {
        m_expGroup388[i] = 1.0f;
    }
    // ORIG clear 循环 lea 0xe4 disp 8 只复位前 4 个（第 5 个由 import 写入）。
    for (i = 0; i < 4; ++i)
    {
        m_monsterExpTable398[i] = 1.0f;
    }
    for (i = 0; i < 5; ++i)
    {
        m_expBonusWeight3ac[i] = 1.0f;
    }
    m_vec57c.clear();
    m_vec588.clear();
    m_598 = 0;
    m_59c = 0;
    m_assaultLevelPenalty = 0.0f;
    m_float5a4.clear();
    m_508[0] = 0;
    m_508[1] = 0;
    m_508[2] = 0;
    m_508[3] = 0;
    m_508[4] = 0;
    m_508[5] = 0;
    m_508[6] = 0;
    m_508[7] = 0;
    m_508[8] = 0;
    m_508[9] = 0;
    m_508[10] = 0;
    m_508[11] = 0;
    m_508[12] = 0;
    m_508[13] = 0;
    m_508[14] = 0;
    m_508[15] = 0;
    m_508[16] = 0;
    m_508[17] = 0;
    m_508[18] = 0;
    m_508[19] = 0;
    m_558 = -1;
    for (i = 0; i < 0x1a; ++i)
    {
        m_55c[i] = 0;
    }
    m_578 = 0;
    m_burning48c.clear();
    m_luck48.clear();
    m_luckDed54.clear();
    m_luckDed60.clear();
    m_vec6c.clear();
    m_78 = 0;
    m_80 = 0;
    m_7c = 0;
    m_84 = 0;
    m_49e8 = 0;
    m_76c = 0;
    m_map88.clear();
    m_veca0.clear();
    m_ac = 0;
    m_mapb0.clear();
    m_unusableEC.clear();
    m_f8 = 0;
    m_unusableItemFC.clear();
    m_map114.clear();
    m_dungeonInoutc8.clear();
    m_dungeonOpenE0.clear();
    m_bloodReward794.m_vecc.clear();
    m_bloodReward794.m_vec0.clear();
    m_bloodReward794.m_vec18.clear();
    m_7b8 = 0;
    m_7bc = 0;
    for (int k = 0; k < 7; ++k)
    {
        m_emblemCompound[k].m_cost = -1;   // ORIG 0x836f7c0：cost=0xffffffff（EMBLEM 无此等级）
    }
    for (int i = 0; i < 32; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            m_arr868[i][j] = 0;
            m_arr1868[i][j] = 0;
        }
    }
    m_49ec = 0;
    m_49f0 = 0;
    m_488c = 0;
    m_4890 = 0;
    m_vec4954.clear();
    m_4960 = 0;
    m_4964 = 1;
    m_4968 = 0;
    m_496c = 0;
    // ORIG 0x836f8f4：0x4970..0x4977 八个独立 movb（非循环）
    m_schoolMatchSeal[0] = 0;
    m_schoolMatchSeal[1] = 0;
    m_schoolMatchSeal[2] = 0;
    m_schoolMatchSeal[3] = 0;
    m_schoolMatchSeal[4] = 0;
    m_schoolMatchSeal[5] = 0;
    m_schoolMatchSeal[6] = 0;
    m_schoolMatchSeal[7] = 0;
    m_schoolMatchDailyPlayLimit = 0;
    m_map497c.clear();
    m_map4994.clear();
    m_pvp49ac.clear();
    m_map49c4.clear();
    m_vec49dc.clear();
    m_chaosWeight4e4.clear();
    m_ghostLevel4f0.clear();
    m_ghostChaos4fc.clear();
    // ORIG 0x836f9cd：clear 末尾 memset(this+0x4bc, 0, 4)
    // （+0x3c0/+0x400/+0x468/+0x480/+0x498/+0x4a8/+0x4c0/+0x4d8/
    //  +0x3c/+0x18/+0x24 在 ORIG clear 中不处理，故不调用其 clear）
    memset(m_weekendBouns4b4.m_dropRoutineByParty, 0, 4);
}

float ServerParameterScript::GetExpBonusWeight(eRankBonusIndex idx) const
{
    if (idx >= 0 && idx <= 4)
    {
        return m_expBonusWeight3ac[idx];
    }
    return 0.0f;
}

bool ServerParameterScript::CheckCollectDungeonStatisticServer(int serverGroup,
                                                               int channelNo)
{
    bool result = false;
    stServerIndex key;
    key.m_field0 = serverGroup;
    key.m_field4 = channelNo;
    std::map<stServerIndex, bool>::iterator it = m_map0.find(key);
    if (it != m_map0.end())
    {
        result = it->second;
    }
    return result;
}

// ORIG stFatigueEventParam_t::stFatigueEventParam_t() 0x08a8ccc4
stFatigueEventParam_t::stFatigueEventParam_t()
{
    m_field0 = 0;
    m_field4 = 0;
    memset(&m_field8, 0, 4);
}

// ORIG stWeekendBouns::stWeekendBouns() 0x0836f374
stWeekendBouns::stWeekendBouns()
{
    m_bonusFatiguePercent = 0;
    m_bonusExpPercent = 0;
}

// ORIG stLevelCond4PremiumGoldCard::stLevelCond4PremiumGoldCard() 0x0836f348
stLevelCond4PremiumGoldCard::stLevelCond4PremiumGoldCard()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0)
{
}

bool ServerParameterScript::CheckLightServer(int serverIdx)
{
    bool result = false;
    std::map<int, bool>::iterator it = m_map24.find(serverIdx);
    if (it != m_map24.end())
    {
        result = it->second;
        SetLightServerFlag(result);
    }
    return result;
}

std::vector<std::pair<int, int> >* ServerParameterScript::GetEscaladeTutorialReward(
    unsigned int rewardIdx)
{
    std::map<int, std::vector<std::pair<int, int> > >::iterator it =
        m_map88.find(rewardIdx);
    if (it != m_map88.end())
    {
        return &it->second;
    }
    return 0;
}

bool ServerParameterScript::checkAssaultServer(int serverGroup, int channelNo)
{
    // b28d-solo-ServerParameterScript: [AE] — ORIG 0x08a8c3ee 49 insn；fresh 复验
    // 49/50，唯一差 = 4.4 在 `mov $1,%eax; jmp` 之后多一条尾对齐 nop（§6.1 剥离后
    // 逐条全等）；p167 调用点差集 0/0，源码未改。
    std::pair<int, int> p;
    std::vector<std::pair<int, int> >::iterator it = m_vec18.begin();
    std::vector<std::pair<int, int> >::iterator e = m_vec18.end();
    for (; it != e; ++it)
    {
        p = *it;
        if (p.first != serverGroup)
        {
            continue;
        }
        else if (p.second == channelNo)
        {
            return true;
        }
    }
    return false;
}

unsigned int ServerParameterScript::getAssaultGhostTime(int level,
                                                        int chaosPoint)
{
    // b28d-solo-ServerParameterScript: DIFF→修（真语义 bug + P243 + P50 复活）
    // ORIG 0x08a8c804 逐段：① time clamp 为 `cmp t0,time; jbe`/`cmp t1,time; jae`
    // 回写形（与源码同）；② chaos 段 `iterator cit;` **默认构造 C1Ev 0x08a8dd24 +
    // `cit = begin()` 赋值**（ORIG 0x08a8c9a5/0x8a8c9cb）⇒ 先声明后赋值（P50 复活）；
    // ③ rate 的 x87 序列 `flds r0; flds r1; flds r0; fsubrp … faddp`（faddp 前无
    // 新 flds）⇒ 原码 `r0 + (r1-r0)/(max-min)*(chaos-min)`（P243 左端写 r0）；
    // ④ **rate clamp 真 bug**：ORIG 首分支 `flds r0; flds rate; fxch; fucompp;
    // seta` ⇒ `rate < r0 → rate = r0`（下限 r0），次分支 `test $0x45; sete` ⇒
    // `rate > r1 → rate = r1`（上限 r1）。旧源码写成 `rate > r0 → rate = r0` 与
    // `rate < r1 → rate = r1`，区间反置（r0<r1 时恒被拉到 r1）⇒ 真语义 bug。
    // ⑤ x87 比较 fucompp/fnstsw/sahf vs 4.4 fucomip/fstp 属 N85（P230 TU 旗标）。
    if (m_ghostLevel4f0.empty())
    {
        return 0xb4;
    }
    std::vector<stAssaultLevelGhostPanalty>::iterator it =
        m_ghostLevel4f0.begin();
    stAssaultLevelGhostPanalty g;
    for (; it != m_ghostLevel4f0.end(); it++)
    {
        if (level >= it->m_min && level <= it->m_max)
        {
            g = *it;
            break;
        }
    }
    unsigned int time = 0;
    time = g.m_t0 + (level - g.m_min) *
                        ((g.m_t1 - g.m_t0) / (unsigned int)(g.m_max - g.m_min));
    if (time < g.m_t0)
    {
        time = g.m_t0;
    }
    else if (time > g.m_t1)
    {
        time = g.m_t1;
    }
    if (m_ghostChaos4fc.empty())
    {
        return time / 1000;
    }
    if (chaosPoint > 1000)
    {
        chaosPoint = 1000;
    }
    float rate = 1.0f;
    std::vector<stAssaultChaosPointGhostPanalty>::iterator cit;
    cit = m_ghostChaos4fc.begin();
    stAssaultChaosPointGhostPanalty cp;
    for (; cit != m_ghostChaos4fc.end(); cit++)
    {
        if (chaosPoint >= cit->m_min && chaosPoint <= cit->m_max)
        {
            cp = *cit;
            break;
        }
    }
    rate = cp.m_r0 + ((cp.m_r1 - cp.m_r0) / (float)(cp.m_max - cp.m_min)) *
                         (float)(chaosPoint - cp.m_min);
    if (rate < cp.m_r0)
    {
        rate = cp.m_r0;
    }
    if (rate > cp.m_r1)
    {
        rate = cp.m_r1;
    }
    time = (unsigned int)((long double)time * rate);
    return time / 1000;
}

float ServerParameterScript::getAssaultPriceChaospointWeight(int chaosPoint)
{
    // b28d-solo-ServerParameterScript: DIFF→修（P243 flds 序 + early-return 汇聚）
    // ORIG 0x08a8c668 逐段：① `flds w0; flds w1; flds w0; fsubrp` ⇒ 原码把 `+w0`
    // 写在**最左**（`w0 + (w1-w0)/(max-min)*(chaos-min)`），我方旧式 `... + w0` 使 4.4
    // 末尾重复 flds w0（多 1 条）；② 两处 clamp 在 ORIG 为 `mov 字段,%eax; jmp 尾中转`
    // （直接返回字段，不回写 weight 槽）⇒ 原码是 early-return 而非回写；③ x87 比较
    // fucompp/fnstsw/sahf vs 4.4 fucomip/fstp 属 N85 跨版本（P230 需 TU 旗标 -march=i486）。
    if (m_chaosWeight4e4.empty())
    {
        return 1.0f;
    }
    if (chaosPoint > 1000)
    {
        chaosPoint = 1000;
    }
    std::vector<stAssaultChaosPointWeight>::iterator it =
        m_chaosWeight4e4.begin();
    stAssaultChaosPointWeight w;
    for (; it != m_chaosWeight4e4.end(); it++)
    {
        if (chaosPoint >= it->m_min && chaosPoint <= it->m_max)
        {
            w = *it;
            break;
        }
    }
    if (w.m_w0 == 0.0f)
    {
        return 1.0f;
    }
    float weight = w.m_w0 + ((w.m_w1 - w.m_w0) /
                             (float)(w.m_max - w.m_min)) *
                                (float)(chaosPoint - w.m_min);
    if (weight < w.m_w0)
    {
        return w.m_w0;
    }
    if (weight > w.m_w1)
    {
        return w.m_w1;
    }
    return weight;
}

int ServerParameterScript::getLuckPoint(int idx, int& out)
{
    unsigned int i = 0;
    for (; i < m_luck48.size(); ++i)
    {
        if (idx <= m_luck48[i].m_field0)
        {
            out = m_luck48[i].m_field1;
            return 1;
        }
    }
    return 0;
}

// getLuckPointDeductionRate —— ORIG T 0x08a8c4fe 逐指令转写：
//   外层遍历 m_luckDed54(+0x54)，跳过 m_field0 == level 的行；
//   内层 j=0..2 在行内 m_levels(+0x04) 找 == grade 项，命中返回该项；
//   未命中返回该行 m_levels[3]（ORIG 无条件 ix(3)）；行耗尽返回 0。
int ServerParameterScript::getLuckPointDeductionRate(int level, int grade)
{
    unsigned int i = 0;
    while (i < m_luckDed54.size())
    {
        if (level <= m_luckDed54[i].m_field0)
        {
            int j = 0;
            while (j <= 2)
            {
                if (m_vec6c[j] <= grade)
                {
                    return m_luckDed54[i].m_levels[j];
                }
                ++j;
            }
            return m_luckDed54[i].m_levels[3];
        }
        ++i;
    }
    return 0;
}

bool ServerParameterScript::LoadScript(const char* path)
{
    return importServerParameterScript(this, path);
}

// ===================== 简单 getter =====================

int ServerParameterScript::GetLottePoint(int idx)
{
    // b28c-L8 [AE 6→0]：`it->first`/`it->second` → `(*it).first`/`(*it).second`。
    // p167 差集一击：ORIG 0x085bec76 三处解引用均为 __normal_iterator::operator*()
    // const（MISSING deEv ×3 / EXTRA ptEv ×3），非 operator->。同法连带
    // GetPriceAverage(0x85beaf6)/GetAdmissionIncrease(0x85bebb6)/
    // GetCleanChattingCount(0x86d17ec) 共 4 件归零。
    if (m_bloodReward794.m_vec18.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vec18.begin();
    std::vector<std::pair<int, int> >::iterator e =
        m_bloodReward794.m_vec18.end();
    for (; it != e; ++it)
    {
        if ((*it).first > idx)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
}

int ServerParameterScript::GetPriceAverage(int idx)
{
    if (m_bloodReward794.m_vec0.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vec0.begin();
    std::vector<std::pair<int, int> >::iterator e =
        m_bloodReward794.m_vec0.end();
    for (; it != e; ++it)
    {
        if ((*it).first >= idx)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
}

// ORIG 0x082f03bc：idx∈[1,200]，取 m_vec588(+0x588)[idx-1]。
int ServerParameterScript::getAssaultPrice(int idx)
{
    if (idx > 200 || idx <= 0)
    {
        return 0;
    }
    return m_vec588[idx - 1];
}

// ORIG 0x082f0410：idx<=0 → 0；idx>size 时 clamp 到 size；返回 v[idx-1]。
float ServerParameterScript::getChaosPointWeight(int idx)
{
    if (idx <= 0)
    {
        return 0.0f;
    }
    int size = m_float5a4.size();
    if (idx <= size)
    {
    }
    else
    {
        idx = size;
    }
    return m_float5a4[idx - 1];
}

int ServerParameterScript::GetAdmissionIncrease(int idx)
{
    if (m_bloodReward794.m_vecc.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vecc.begin();
    std::vector<std::pair<int, int> >::iterator e =
        m_bloodReward794.m_vecc.end();
    for (; it != e; ++it)
    {
        if ((*it).first > idx)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
}

float ServerParameterScript::GetPvPPenaltyRevision(int grade)
{
    std::map<int, int>::iterator it = m_mapb0.find(grade);
    // ORIG 0x08693918：命中正分支 → int 具名槽 fildl; flds 100.0f; fdivrp；
    // fld1 冷岛在尾。
    if (it != m_mapb0.end())
    {
        return (float)it->second / 100.0f;
    }
    return 1.0f;
}

int ServerParameterScript::getRecoverStaminaPrice(int stamina, int price)
{
    int size = m_vec57c.size();
    int v = m_vec57c[std::min(size, stamina) - 1];
    // ORIG 0x0869375a：magic 0xb60b60b7 + sar $0x6 = 除以 90（非 100）
    return ((100 - price) * v) / 90;
}

int ServerParameterScript::GetPvPChannelGradePenalty(int grade) const
{
    if (m_map497c.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map497c.find(grade);
    if (it != m_map497c.end())
    {
        return it->second;
    }
    // ORIG 0x08693xx 实证：size>2(seta) 时 it **复用** = end() 临时+拷贝赋值，
    // --it 后取 second；否则落尾部 return 0 岛。
    if (m_map497c.size() > 2)
    {
        it = m_map497c.end();
        --it;
        return it->second;
    }
    return 0;
}

bool ServerParameterScript::CheckUnusableEquipmentInfo(ENUM_UNUSABLE_EQUIPMENT_TYPE type, int idx)
{
    for (std::vector<stUnusableEquipmentInfo>::iterator it =
             m_unusableEC.begin();
         it != m_unusableEC.end(); ++it)
    {
        stUnusableEquipmentInfo* p = &*it;
        // b28d-L3：ORIG 0x08568ac0 段 `mov -0xc,%eax; mov (%eax),%eax;
        // cmp 0xc(%ebp),%eax`（field0 入 eax、type 内存直比）⇒ 比较方向交换。
        // b28d-solo-ServerParameterScript: 去冗余 cast 复现 mem 直比；守卫改
        // 正极 `field4!=0 || field8!=0`（ORIG `jne CHECK` + `je FALSE` 混合形，
        // 德摩根 `&&` 形在 4.4 恒双 jne）。
        // 残差（N 封顶，59 vs 64）：① type 比较 4.4 恒先载形参（`mov 0xc(%ebp),%edx;
        // cmp %eax,%edx`）vs ORIG mem 直比（`cmp 0xc(%ebp),%eax`）——P209 unsigned
        // 形参族；② field4>idx 与 field8>=idx 两处 return 的共享尾块（ORIG `jg TRUE`
        // 与 fall-through 共用 `mov $1`）4.4 恒就地发射——N93附 尾部双 return 块序。
        if (type == p->m_field0)
        {
            // b28c-L8 语义修正（ORIG 0x08568af6 逐条）：field4!=0 时 ORIG 经
            // `jne +0x4f` 落入**同一段** field4/field8 判定（非跳过），旧源码把
            // 该段误置于 field4==0 分支内 ⇒ field4>idx 且 field8>=idx 时分叉。
            // CFG：b!=0→CHECK；b==0&&c==0→false；CHECK: b>idx→true；
            //     c>=idx→false；否则 true。
            if (p->m_field4 != 0 || p->m_field8 != 0)
            {
                if (p->m_field4 > idx)
                {
                    return true;
                }
                if (p->m_field8 >= idx)
                {
                    return false;
                }
                return true;
            }
            return false;
        }
    }
    return true;
}

bool ServerParameterScript::CheckUnusableItem(ENUM_EQUIPMENTTYPE equipType,
                                              unsigned long itemIdx) const
{
    // b28d-solo-ServerParameterScript: 真语义 bug 修正 + 形态修（P293 具名迭代器）
    // ORIG 0x08568bbe 逐段实证：① m_unusableItemFC 命中 → return false；
    // ② m_map114（[usable items] **白名单**）未列出该 equipType → 尾块 return true；
    // ③ 列出且集合**命中** itemIdx → 跳尾块 return true（ORIG `call eq; test; je
    // 8568cd8`：eq 假=sit!=end 才跳 true）；集合未命中 → mov $0 return false。
    // 旧源码 `if (it!=end && set.find!=set.end()) return false;` 把白名单判成黑名单，
    // 命中时返回 false 与 ORIG 相反 ⇒ 真语义 bug。单表达式 `find()!=end()` 在 4.4 恒
    // end 先/find 后（本次实测），故三处均改具名迭代器；尾块只留一处 return true。
    std::set<unsigned long>::const_iterator unusableIt =
        m_unusableItemFC.find(itemIdx);
    if (unusableIt != m_unusableItemFC.end())
    {
        return false;
    }
    std::map<ENUM_EQUIPMENTTYPE, std::set<unsigned long> >::const_iterator it =
        m_map114.find(equipType);
    if (it != m_map114.end())
    {
        std::set<unsigned long>::const_iterator sit = it->second.find(itemIdx);
        if (sit == it->second.end())
        {
            return false;
        }
    }
    return true;
}

int ServerParameterScript::get_pvp_item_give(int idx)
{
    int ret = -1;
    for (std::vector<int>::const_iterator it = m_vec49dc.begin();
         it != m_vec49dc.end(); ++it)
    {
        if (*it > idx)
        {
            return ret;
        }
        ret = *it;
    }
    return ret;
}

int ServerParameterScript::GetCleanChattingCount(int level)
{
    if (m_vec4954.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it = m_vec4954.begin();
    std::vector<std::pair<int, int> >::iterator e = m_vec4954.end();
    for (; it != e; ++it)
    {
        if ((*it).first >= level)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
}

int ServerParameterScript::GetTraningFirstGive(int a, int b, int c,
                                               std::vector<std::pair<int, int> >& out)
{
    // b28d-solo-ServerParameterScript: DIFF→修（去 end 缓存）
    // ORIG 0x084a61c4 逐段：① begin 后 `jmp COND`，COND 在循环体之后**每轮重发
    // end()+C1ERK 转换**（0x84a622d 段），循环后 `it==end()` 亦重新 end()（0x84a6270）
    // ⇒ 原码无 end 缓存局部；旧源码 `const_iterator e=...end(); it!=e` 少两组
    // end/转换槽（帧 0x48 vs ORIG 0x58）且循环次数形态不同。② PvPPlayerKey 查表、
    // ③ insert 段与 ORIG 逐条同形（仅槽号随帧）。
    std::map<int, int>::const_iterator it = m_map49c4.begin();
    for (; it != m_map49c4.end(); ++it)
    {
        if (it->first >= c)
        {
            c = it->first;
            break;
        }
    }
    if (it == m_map49c4.end())
    {
        --it;
        c = it->first;
    }
    PvPPlayerKey key((unsigned char)a, (unsigned char)b, (unsigned char)c);
    std::map<PvPPlayerKey, std::vector<std::pair<int, int> > >::const_iterator
        found = m_pvp49ac.find(key);
    if (found == m_pvp49ac.end())
    {
        return 0;
    }
    out.insert(out.end(), found->second.begin(), found->second.end());
    return 1;
}

int ServerParameterScript::GetTraningSealGive(int idx) const
{
    if (m_map4994.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map4994.find(idx);
    if (it != m_map4994.end())
    {
        return it->second;
    }
    // ORIG 实证：!empty() 正形(xor$1)，it 复用 = end() 临时+拷贝赋值
    if (!m_map4994.empty())
    {
        it = m_map4994.end();
        --it;
        return it->second;
    }
    return 0;
}

bool ServerParameterScript::CheckTraningFirstGive() const
{
    if (m_map49c4.empty())
        return false;

    bool first = true;
    int expected = 0;
    for (std::map<int, int>::const_iterator it = m_map49c4.begin();
         it != m_map49c4.end(); ++it)
    {
        if (first)
        {
            expected = it->second;
            first = false;
        }
        else if (it->second != expected)
        {
            return false;
        }
    }
    return true;
}

bool ServerParameterScript::isDungeonOpen(int dungeonIdx)
{
    std::vector<int>::iterator it =
        std::find(m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
    // ORIG 0x085bf0xx 尾部实证 test/je + 两分支均 mov$1：原源码即
    // 「found 与否都 return true」的死分支形（忠实还原）。
    if (it != m_dungeonOpenE0.end())
    {
        return true;
    }
    return true;
}

void ServerParameterScript::setDungeonOpen(int dungeonIdx, bool open)
{
    // b28d-solo-ServerParameterScript: 真语义 bug 修正 — ORIG 0x08268698
    // `mov 0x10(%ebp),%eax; mov %al,-0x3c; cmpb $0x1,-0x3c; je 0x826874d`
    // ⇒ **open==1 走 erase 路径**（je 目标段：ne(find,end) 真则 erase），
    // open==0 落 fall-through 段：eq(find,end) 真则 push_back。旧源码
    // `if (!open)` 把两段整段互换（erase↔push_back），行为与原码相反。
    // 分支形态照 ORIG：erase 段 `if (it != end) erase`（ne）；
    // push_back 段 `if (it == end) push_back`（eq，ORIG 0x82688da 实例名）。
    // 残差（N 封顶）：ORIG `cmpb $0x1,-0x3c` vs 4.4 `cmpb $0x0,-0x3c`（bool 条件码
    // 规范化方向跨版本）+ erase/push_back 两段 fall-through↔je 目标互换（N93附 块序；
    // 两形互试实测 101 vs 103 不闭合）。
    if (open)
    {
        std::vector<int>::iterator it = std::find(
            m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
        if (it != m_dungeonOpenE0.end())
        {
            m_dungeonOpenE0.erase(it);
        }
        return;
    }
    std::vector<int>::iterator it2 = std::find(
        m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
    if (it2 == m_dungeonOpenE0.end())
    {
        m_dungeonOpenE0.push_back(dungeonIdx);
    }
}

// ===================== getAssaultUserRunawayNotice（ORIG W 0x082f03f8） =====================
const char* ServerParameterScript::getAssaultUserRunawayNotice()
{
    return m_str594.c_str();
}

// 内联 getter 取址锚定（ORIG W：0x085bed36/0x085bed44/0x08568bb0/0x086d18ba/
//   0x082694fa/0x086d18ac/0x08269508）。文件作用域常量数组使取址成为 odr-use，
// 强制编译器发出各 W 符号；数组仅存于 .rodata，无动态初始化。
typedef int (ServerParameterScript::*SpsIntGetter)();
static const SpsIntGetter g_spsIntGetters[] = {
    &ServerParameterScript::GetLottoCost,
    &ServerParameterScript::GetJackpot,
    &ServerParameterScript::GetDailyMatchCount,
    &ServerParameterScript::GetPaneltyDupMessage,
    &ServerParameterScript::GetMaxCleanChattingCount,
    &ServerParameterScript::GetPvPLiveEventPlayCount,   // ORIG W 0x08269508
};
typedef std::vector<std::pair<int, int> >& (ServerParameterScript::*SpsVecGetter)();
static const SpsVecGetter g_spsVecGetter =
    &ServerParameterScript::GetPvPLiveEventReward;

void anchorServerParameterScriptWMembers()
{
    // stLightServerNotice_t：C1 / C1ERKS / aSERKS（ORIG W 0x08a8ce10/0x08a8e764/0x08a8e81e）
    stLightServerNotice_t noticeA;
    stLightServerNotice_t noticeB(noticeA);
    noticeA = noticeB;

    // stLuckPointDeduction_t：C1 / C1ERKS / aSERKS（ORIG W 0x08a8ce26/0x08a8ee74/0x08a8ef22）
    stLuckPointDeduction_t luckA;
    stLuckPointDeduction_t luckB(luckA);
    luckA = luckB;

    // stBurningFatigueParam_t：C1（ORIG W 0x08a8cc64）
    stBurningFatigueParam_t burn;

    // PvPPlayerKey：C1Ehhh / operator<（ORIG W 0x084a60aa / 0x084a6034；
    //   ORIG 无默认 ctor，不锚定 C1Ev）
    ServerParameterScript::PvPPlayerKey keyA(1, 2, 3);
    ServerParameterScript::PvPPlayerKey keyB(4, 5, 6);
    if (keyA < keyB)
    {
        keyB.m_field0 = keyA.m_field0;
    }

    // stAssault* 系列：C1（析构由作用域结束触发 D1/D2）
    stAssaultChaosPointWeight chaosW;
    stAssaultLevelGhostPanalty ghostL;
    stAssaultChaosPointGhostPanalty ghostC;

    // STDungeonInoutTime / stAuctionLimitItemParam_t / stUpgradeItemTicketParam_t：C1
    STDungeonInoutTime dungeonTime;
    stAuctionLimitItemParam_t auctionLimit;
    stUpgradeItemTicketParam_t ticket;

    // stLuckPoint_t / stUnusableEquipmentInfo：构造（D1/D2 由作用域析构触发）
    stLuckPoint_t luckPt;
    stUnusableEquipmentInfo unusable;
    luckPt.m_field0 = 1;
    luckPt.m_field1 = 2;
    unusable.m_field0 = 3;

    // EventRewardSection / EventRewardItemInfo：C1 / C1ERKS / aSERKS / D1
    EventRewardSection erA;
    EventRewardSection erB(erA);
    erA = erB;
    EventRewardItemInfo eiA;
    EventRewardItemInfo eiB(eiA);
    eiA = eiB;
    eiA.clear();

    // 锚定函数体保持非空（引用上述常量避免未使用告警）
    (void)g_spsIntGetters;
    (void)g_spsVecGetter;
}
// ============================================================================
// ============================================================================
// importServerParameterScript —— ORIG T 0x08a870b7（0x51d4 字节，4616 insn）。
// 按 docs/class_func_reports/_lt_global_gt_/importServerParameterScript.md
// （Ghidra 伪 C + 汇编）逐节 [xxx] 分派完整转写。分支名与写入偏移以 ORIG
// 汇编为准（Ghidra 伪 C 在 [training seal give]/[pvp channel grade penalty]/
// [training first give] 三节错位，已按汇编 0x8a8af11/0x8a8b004/0x8a8b115 修正：
// seal give→m_map4994 2-int 循环、grade penalty→m_map497c 2-int 循环、
// first give→PvPPlayerKey 复杂结构 + m_map49c4 计数）。
// 具名布局字段依据 ORIG 汇编的绝对成员偏移；所有普通成员访问均经脚本字段。
// 仍保留的裸偏移仅存在于局部 stUpgradeItemTicketParam_t 字节视图。
// ============================================================================
unsigned int g_new_scriptVersion;   // ORIG B 0x0949b298（仅本函数写入）

int GetEquipmentTypeStringToEnum(std::string& type);   // CStackableItem.cpp

bool importServerParameterScript(ServerParameterScript* script,
                                 const char* path)
{
    // b28d-solo-ServerParameterScript: DIFF（N82+N89+N91 封顶；p167 差集已大幅收敛）
    // 本波 p167 定位并修正：① [escalade tutorial reward] operator[] → find/end/ne/
    // ptEv + 两分支 push_back + insert；② [training first give] m_map49c4[a]=1 →
    // insert；③ int,int make_pair → std::pair<int,int>(a,b)（9 点：m_vecMapped/
    // m_vecShareKind/m_vec0/m_vecc/m_vec18(blood)/ontime/vec/escalade×2）；④ [ontime
    // event reward] break→return 0、去 unsigned short 中转、pair 直构；⑤ [drop upgrade
    // rarity] rarity int→ENUM_RARITY（含 CItemAmplifier.h include）；⑥ [drop bonus of
    // dungeon level increase] 具名 pair 左值 push_back；⑦ [bring up event] `line !=`
    // → 正向 `line ==`；⑧ m_levels(×2)/m_vec6c push_back 实参内联（push_backEOi）；
    // ⑨ m_pvp49ac make_pair 第一参改匿名右值；⑩ [full level reward] 去 (unsigned) cast。
    // ⑪ [ontime event reward] `int a=0; short b=0;` 预清两句式 + pair 默认构造
    // + 逐字段写 + 左值 push_back；⑫ [pvp live event reward] 同形。
    // **p167 终态：MISSING=0**（全部语义调用点已还原）；EXTRA 仅 ①空析构 D1 ×2/类型
    // 共 16（4.4 对局部空析构发射 D1 调用，ORIG 4.1 内联省略；D1 W 符号必须保留
    // ⇒ N91 封顶）②ScanInt +1（1 处多余读取，逐段槽序不可分辨，N82 封顶）。
    // ndiff 主体为 N82（帧/槽复用）与 N89（STL out-of-line 实例）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    script->clear();
    std::string line;
    bool ok = false;
    bool missed = false;
    while (ScanType(line, true))
    {
        missed = false;
        if (line == "[nation code]")
        {
            ScanInt((bool*)0);
        }
        else if (line == "[script version]")
        {
            script->m_358 = (unsigned int)ScanInt((bool*)0);
            g_new_scriptVersion = script->m_358;
        }
        else if (line == "[version]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_versionBytes[i] = (unsigned char)ScanInt((bool*)0);
            }
        }
        else if (line == "[clear exp bonusrate]")
        {
            script->m_expRate360[0] = ScanFloat((bool*)0);
        }
        else if (line == "[monster exp bonusrate]")
        {
            script->m_expRate360[1] = ScanFloat((bool*)0);
        }
        else if (line == "[party user number exp bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup368[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number exp bonusrate starter server]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup378[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[quest item drop bonus starter server]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup388[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[dungeon difficulty exp bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_monsterExpTable398[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[clear rank exp bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_expBonusWeight3ac[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[drop prob number]")
        {
            sItemGenRef zero = {0, 0, 0, 0, 0};
            script->m_genRefs0.resize(
                (unsigned int)ScanInt((bool*)0), zero);
        }
        else if (line == "[drop prob]")
        {
            for (int i = 0; i < (int)script->m_genRefs0.size(); ++i)
            {
                sItemGenRef& r = script->m_genRefs0[i];
                r.m_field0 = ScanInt((bool*)0);
                r.m_field4 = ScanInt((bool*)0);
                r.m_field8 = ScanInt((bool*)0);
                // ORIG 0x08a875b7 实证：第 4 个 ScanInt 写入 +0x10（m_dropRate），
                // +0x0c 固定 1000（SetItemDropRate 缩放 +0x10）。
                r.m_dropRate = ScanInt((bool*)0);
                r.m_fieldc = 1000;
            }
        }
        else if (line == "[dungeon difficulty drop bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_floatPool3cc[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number drop bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_floatPool3e0[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[drop bonusrate of monster kind]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_flagExpRate3f0[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[result reward prob number]")
        {
            sItemGenRef zero = {0, 0, 0, 0, 0};
            script->m_genRefs1.resize(
                (unsigned int)ScanInt((bool*)0), zero);
        }
        else if (line == "[result reward prob]")
        {
            for (int i = 0; i < (int)script->m_genRefs1.size(); ++i)
            {
                sItemGenRef& r = script->m_genRefs1[i];
                r.m_field0 = ScanInt((bool*)0);
                r.m_field4 = ScanInt((bool*)0);
                r.m_field8 = 0;
                // ORIG 0x08a8786a 实证：第 4 个 ScanInt 写入 +0x10（m_dropRate），
                // +0x0c 固定 1000（与 [drop prob] 节一致）。
                r.m_dropRate = ScanInt((bool*)0);
                r.m_fieldc = 1000;
            }
        }
        else if (line == "[dungeon difficulty reward bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_floatPool40c[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number reward bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_floatPool420[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[basis of item dicision]")
        {
            script->m_430 = ScanInt((bool*)0);
        }
        else if (line == "[basis of equipment rarity dicision]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_ints434[i] = ScanInt((bool*)0);
            }
        }
        else if (line == "[basis of stackable rarity dicision]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_ints444[i] = ScanInt((bool*)0);
            }
        }
        else if (line == "[stamina recovery cost]")
        {
            script->m_vec57c.clear();
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec57c.push_back(v);
            }
        }
        else if (line == "[increase abuse point]")
        {
            script->m_increaseAbusePoint = ScanInt((bool*)0);
        }
        else if (line == "[decrease abuse point]")
        {
            script->m_decreaseAbusePoint = ScanInt((bool*)0);
        }
        else if (line == "[abuse point penalty]")
        {
            script->m_abusePointPenalty = ScanInt((bool*)0);
        }
        else if (line == "[decrease experience point]")
        {
            script->m_decreaseExperiencePoint = ScanInt((bool*)0);
        }
        else if (line == "[upgrade with legacy]")
        {
            script->m_508[8] = ScanInt((bool*)0);
            script->m_508[9] = ScanInt((bool*)0);
            script->m_508[10] = ScanInt((bool*)0);
            script->m_508[11] = (script->m_508[9] - script->m_508[8]) * 1000 + 999;
            script->m_508[12] = ScanInt((bool*)0);
            script->m_508[13] = ScanInt((bool*)0);
            script->m_508[14] = ScanInt((bool*)0);
            script->m_508[15] = (script->m_508[13] - script->m_508[12]) * 1000 + 999;
            script->m_508[16] = ScanInt((bool*)0);
            script->m_508[17] = ScanInt((bool*)0);
            script->m_508[18] = ScanInt((bool*)0);
            script->m_508[19] = (script->m_508[17] - script->m_508[16]) * 1000 + 999;
        }
        else if (line == "[notice legacy with rarity]")
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptServerParameter.cpp",
                                      "importServerParameterScript", 400,
                                      "importUpgradeTable - invalid notice legacy with rarity tag");
            }
            script->m_558 = v;
        }
        else if (line == "[notice legacy with type]")
        {
            memset(script->m_55c, 0, sizeof(script->m_55c));
            std::string s;
            while (ScanStr(&s))
            {
                int idx = GetEquipmentTypeStringToEnum(s);
                script->m_55c[idx] = 1;
            }
        }
        else if (line == "[notice legacy with upgrade]")
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptServerParameter.cpp",
                                      "importServerParameterScript", 0x1a7,
                                      "importUpgradeTable - invalid notice legacy with upgrade tag");
            }
            script->m_578 = v;
        }
        else if (line == "[collect dungeon statistic server]")
        {
            while (true)
            {
                stServerIndex key;
                key.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                key.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map0.insert(std::make_pair(key, true));
            }
        }
        else if (line == "[light server no]")
        {
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_map24.insert(std::make_pair(v, true));
            }
        }
        else if (line == "[burning fatigue event param]")
        {
            while (true)
            {
                stBurningFatigueParam_t p;
                p.m_level = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_bonusExpRate = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_refillFatigue = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field0c = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field10 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field14 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[0] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[3] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_burning48c.push_back(p);
            }
        }
        else if (line == "[fatigue event param]")
        {
            while (true)
            {
                stFatigueEventParam_t p;
                p.m_field0 = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field8 = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field9 = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_fielda = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_fieldb = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_fatigue4a8.push_back(p);
            }
        }
        else if (line == "[light server notice]")
        {
            while (true)
            {
                stLightServerNotice_t n;
                n.m_hour = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                n.m_min = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                if (!ScanStr(&n.m_message))
                {
                    return 0;
                }
                script->m_light3c.push_back(n);
            }
        }
        else if (line == "[assault cost]")
        {
            script->m_vec588.clear();
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec588.push_back(v);
            }
        }
        else if (line == "[assault channel]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: ORIG 此段仍为 make_pairIRiS0_
                // （pair 直构仅命中 escalade/m_vecMapped/m_vecShareKind/m_vec0/
                // m_vecc/m_vec18(blood)/m_vec4880/vec 共 9 点），回退。
                script->m_vec18.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[assault user runaway notice]")
        {
            if (!ScanStr(&script->m_str594))
            {
                return 0;
            }
        }
        else if (line == "[assault term panalty]")
        {
            script->m_598 = ScanInt((bool*)0);
        }
        else if (line == "[assault term panalty keep up time]")
        {
            script->m_59c = ScanInt((bool*)0);
        }
        else if (line == "[assault level panalty]")
        {
            script->m_assaultLevelPenalty = ScanFloat((bool*)0);
        }
        else if (line == "[assault chaos point weight]")
        {
            script->m_float5a4.clear();
            while (true)
            {
                float v = ScanFloat(&ok);
                if (!ok)
                {
                    break;
                }
                if (script->m_float5a4.size() == 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        script->m_float5a4.push_back(v);
                    }
                }
                else
                {
                    for (int i = 0; i < 5; ++i)
                    {
                        script->m_float5a4.push_back(v);
                        if (script->m_float5a4.size() > 99)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if (line == "[luck point]")
        {
            while (true)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                stLuckPoint_t p;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field1 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_luck48.push_back(p);
            }
        }
        else if (line == "[luck point deduction rare]")
        {
            while (true)
            {
                stLuckPointDeduction_t p;
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_levels.reserve(4);
                for (int i = 0; i < 4; ++i)
                {
                    // b28d-solo-ServerParameterScript: ORIG 0x08a88ad0 段
                    // `call ScanInt; mov %eax,-0x2fc; lea -0x2fc; call push_backEOi`
                    // ⇒ 实参内联（右值）而非具名 v（p167 MISSING push_backEOi ×3 /
                    // EXTRA push_backERKi ×3）。
                    p.m_levels.push_back(ScanInt(&ok));
                    if (!ok)
                    {
                        return 0;
                    }
                }
                (void)level;
                script->m_luckDed54.push_back(p);
            }
        }
        else if (line == "[luck point deduction unique]")
        {
            while (true)
            {
                stLuckPointDeduction_t p;
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_levels.reserve(4);
                for (int i = 0; i < 4; ++i)
                {
                    // b28d-solo-ServerParameterScript: 同 rare 段（push_backEOi）。
                    p.m_levels.push_back(ScanInt(&ok));
                    if (!ok)
                    {
                        return 0;
                    }
                }
                (void)level;
                script->m_luckDed60.push_back(p);
            }
        }
        else if (line == "[luck point deduction gap]")
        {
            script->m_vec6c.reserve(4);
            for (int i = 0; i < 3; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                // b28d-solo-ServerParameterScript: ORIG 0x08a88d80 段
                // `call ScanInt; mov %eax,-0x2f4; lea -0x2f4; call push_backEOi`
                // ⇒ 实参内联（push_backEOi）。
                script->m_vec6c.push_back(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
            }
            int unused2 = ScanInt(&ok);
            (void)unused2;
            if (!ok)
            {
                return 0;
            }
            int unused3 = ScanInt(&ok);
            (void)unused3;
            if (!ok)
            {
                return 0;
            }
        }
        else if (line == "[point per fatigue]")
        {
            script->m_78 = ScanInt((bool*)0);
        }
        else if (line == "[event point per fatigue]")
        {
            script->m_7c = ScanInt((bool*)0);
        }
        else if (line == "[point per play time]")
        {
            script->m_80 = ScanInt((bool*)0);
            script->m_84 = ScanInt((bool*)0);
        }
        else if (line == "[black list help parameter]")
        {
            script->m_blackListHelpParameter0 = ScanInt((bool*)0);
            script->m_blackListHelpParameter1 = ScanInt((bool*)0);
        }
        else if (line == "[unify server prefix]")
        {
            script->m_preventPrefixCount = ScanInt((bool*)0);
            for (int i = 0; i < script->m_preventPrefixCount; ++i)
            {
                ScanStr(script->m_preventPrefixEntries + i * 0x1e, 0x1e);
            }
        }
        else if (line == "[damaged equipment ratio]")
        {
            script->m_49e8 = ScanInt((bool*)0);
        }
        else if (line == "[limit fatigue battery charging amount]")
        {
            script->m_fatigueTable5c0.m_capLimit =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[end level fatigue battery weight]")
        {
            script->m_fatigueTable5c0.m_chargeRate4 = ScanFloat((bool*)0);
        }
        else if (line == "[character level fatigue battery constance]")
        {
            script->m_fatigueTable5c0.m_chargeDivisor8 =
                (unsigned char)ScanInt((bool*)0);
        }
        else if (line == "[limit fatigue battery level]")
        {
            script->m_fatigueTable5c0.m_minChargeLevel9 =
                (unsigned char)ScanInt((bool*)0);
        }
        else if (line == "[fatigue battery money amount]")
        {
            script->m_fatigueTable5c0.m_fieldc = ScanInt((bool*)0);
        }
        else if (line == "[fatigue battery level for money active]")
        {
            script->m_fatigueTable5c0.m_endLevel10 =
                (unsigned char)ScanInt((bool*)0);
        }
        else if (line == "[min fatigue battery per buff active]")
        {
            script->m_fatigueTable5c0.m_batteryThreshold1a8 =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[limit grownup buff cnt fatigue battery]")
        {
            int i = 0;
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                *reinterpret_cast<short*>(
                    script->m_fatigueTable5c0.m_data11 + 1 + i * 2) =
                    (short)v;
                ++i;
            }
        }
        else if (line == "[fatigue battery per grownup buff active]")
        {
            script->m_fatigueTable5c0.m_activeSwitch1a4 =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[grownup buff per fatigue battery active]")
        {
            script->m_fatigueTable5c0.m_bonusMultiplier1a6 =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[pc room party exp bonus rate]")
        {
            float v = ScanFloat(&ok);
            if (!ok)
            {
                return 0;
            }
            script->m_76c = v;
        }
        else if (line == "[escalade tutorial reward]")
        {
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: P123 查找形式修正 — ORIG 0x08a8d1xx 段
                // 为 find/end/ne/ptEv + 两分支 push_backEOS1_ + else 内 vec C1Ev +
                // make_pair(IRi,RSt6vector) + pair<const int,vector> 转换 + insertERKS8_；
                // 旧 `m_map88[key]` 走 operator[]（p167 EXTRA ixERS7_ +1 / MISSING
                // find/end/ne/ptEv/insert 各 1）。
                std::map<int, std::vector<std::pair<int, int> > >::iterator it =
                    script->m_map88.find(key);
                if (it != script->m_map88.end())
                {
                    it->second.push_back(std::pair<int, int>(a, b));
                }
                else
                {
                    std::vector<std::pair<int, int> > vec;
                    vec.push_back(std::pair<int, int>(a, b));
                    script->m_map88.insert(std::make_pair(key, vec));
                }
            }
        }
        else if (line == "[server message index onoff]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (b == 1)
                {
                    script->m_shared770.m_vecIndex.push_back(a);
                }
            }
        }
        else if (line == "[server message index kind]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_shared770.m_vecMapped.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[server message event kind]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_shared770.m_vecShareKind.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[price average]")
        {
            while (true)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vec0.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[lotto cost]")
        {
            script->m_7b8 = ScanInt((bool*)0);
        }
        else if (line == "[jackpot]")
        {
            script->m_7bc = ScanInt((bool*)0);
        }
        else if (line == "[admission increase]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vecc.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[lotto point]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vec18.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[emblem compound info]")
        {
            while (true)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_emblemCompound[idx].m_cost = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                for (int j = 0; j < 5; ++j)
                {
                    script->m_emblemCompound[idx].m_result[j] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                }
            }
        }
        else if (line == "[recipe upgrade table]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe amplify table]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr1868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe upgrade table 2nd]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr2868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe amplify table 2nd]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr3868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[upper bind cube rate bonus]")
        {
            script->m_49ec = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[lower bind cube rate bonus]")
        {
            script->m_49f0 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[full level reward]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: ORIG pair C1 模板参为 <int,int>
                // （_ZNSt4pairIKijEC1IiiEEOS_IT_T0_），旧 (unsigned int) cast 使实例
                // 变 <int,unsigned>（p167 EXTRA C1IijE 1 / MISSING C1IiiE 1）。
                script->m_map4868.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[ontime event reward]")
        {
            while (true)
            {
                // b28d-solo-ServerParameterScript: ORIG 0x08a89f60 段逐条
                // `movl $0,-0x48` + `movw $0,-0x42`（P198① 声明预清两句式）→
                // a=ScanInt（int 槽）/b=(short)ScanInt（mov %ax 16 位槽）→
                // `pair C1Ev` 默认构造 + p.first/p.second 逐字段写 → push_backERKS1_。
                int a = 0;
                short b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                b = (short)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                std::pair<int, int> p;
                p.first = a;
                p.second = b;
                script->m_vec4880.push_back(p);
            }
        }
        else if (line == "[pvp live event play count]")
        {
            script->m_ac = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[pvp live event reward]")
        {
            while (true)
            {
                // b28d-solo-ServerParameterScript: ORIG 0x08a8a0c0 段
                // `pair C1Ev`（默认构造）→ p.first=ScanInt / p.second=ScanInt →
                // `push_backERKS1_`（左值）；p167 MISSING C1Ev 2 + ERKS1_ 2。
                std::pair<int, int> p;
                p.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.second = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_veca0.push_back(p);
            }
        }
        else if (line == "[pvp exp penalty revision]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_mapb0.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[unusable equipment info]")
        {
            while (true)
            {
                std::string s;
                if (!ScanStr(&s))
                {
                    break;
                }
                int type = 0;
                if (s == "relic")
                {
                    type = 2;
                }
                else if (s == "unique")
                {
                    type = 3;
                }
                else if (s == "choronicle")
                {
                    type = 4;
                }
                else if (s == "epic")
                {
                    type = 5;
                }
                else if (s == "rare_avatar")
                {
                    type = 6;
                }
                else if (s == "common")
                {
                    type = 7;
                }
                else if (s == "uncommon")
                {
                    type = 8;
                }
                else if (s == "rare")
                {
                    type = 9;
                }
                else if (s == "rare_creature")
                {
                    type = 10;
                }
                else if (s == "real_rare")
                {
                    type = 1;
                }
                else
                {
                    return 0;
                }
                stUnusableEquipmentInfo info;
                info.m_field0 = type;
                info.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                info.m_field8 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_unusableEC.push_back(info);
            }
        }
        else if (line == "[daily match count]")
        {
            script->m_f8 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[unusable items]")
        {
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_unusableItemFC.insert((unsigned long)v);
            }
        }
        else if (line == "[usable items]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map114[(ENUM_EQUIPMENTTYPE)a].insert((unsigned long)b);
            }
        }
        else if (line == "[dungeon inout time]")
        {
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                STDungeonInoutTime t;
                t.m_startHour = ScanInt((bool*)0);
                t.m_endHour = ScanInt((bool*)0);
                t.m_startMin = ScanInt((bool*)0);
                t.m_endMin = ScanInt((bool*)0);
                if (t.m_endHour < t.m_startHour || t.m_endMin < t.m_startMin ||
                    t.m_endHour == 0x18 || t.m_endMin == 0x3c)
                {
                    return 0;
                }
                script->m_dungeonInoutc8.insert(std::make_pair(key, t));
            }
        }
        else if (line == "[member bonus fatigue limit]")
        {
            script->m_488c = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_4890 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[premiun card cost]")
        {
            script->m_premiumGoldCard4894.m_cost = ScanFloat((bool*)0);
        }
        else if (line == "[premium card drop]")
        {
            int i = 0;
            while (true)
            {
                script->m_premiumGoldCard4894.m_conds[i].m_field0 =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_premiumGoldCard4894.m_conds[i].m_field4 =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_premiumGoldCard4894.m_conds[i].m_field8 =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_premiumGoldCard4894.m_conds[i].m_fieldc =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ++i;
                if (9 < i)
                {
                    break;
                }
            }
            script->m_premiumGoldCard4894.m_conditionCount = (short)i;
        }
        else if (line == "[premium rarity dicision]")
        {
            script->m_premiumGoldCard4894.m_rarityDecision[0] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[1] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[2] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[3] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[4] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[5] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[max clean chatting count]")
        {
            script->m_4960 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[clean chatting count]")
        {
            while (true)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_vec4954.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[clean chatting duplication]")
        {
            script->m_4964 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[max monster drop per map]")
        {
            script->m_4968 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[max monster count per map]")
        {
            script->m_496c = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[private training seal gain count]")
        {
            script->m_schoolMatchSeal[0] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[1] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[2] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[3] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[relay battle training seal gain count]")
        {
            script->m_schoolMatchSeal[4] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[5] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[6] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[7] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[training seal bonus count]")
        {
            script->m_schoolMatchDailyPlayLimit =
                (unsigned short)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[training seal give]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map4994.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[pvp channel grade penalty]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map497c.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[training first give]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int c = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int n = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                std::vector<std::pair<int, int> > vec;
                for (int i = 0; i < n; ++i)
                {
                    int x = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int y = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    vec.push_back(std::pair<int, int>(x, y));
                }
                // b28d-solo-ServerParameterScript: ORIG 0x08a8d5bc
                // `_ZSt9make_pairIN21…PvPPlayerKeyE…`（第一参**按值**）⇒ 实参为匿名
                // 右值临时（p167 EXTRA make_pairIRN… 1 / MISSING make_pairIN… 1）。
                script->m_pvp49ac.insert(std::make_pair(
                    ServerParameterScript::PvPPlayerKey((unsigned char)a,
                                                        (unsigned char)b,
                                                        (unsigned char)c),
                    vec));
                std::map<int, int>::iterator it = script->m_map49c4.find(a);
                // b28d-solo-ServerParameterScript: P123 — ORIG 用 ne(find,end) 正极性 +
                // insertERKS4_（0x08a8b3ec/0x8a8b468 段），旧 `m_map49c4[a]=1` 走
                // operator[]（p167 EXTRA ixERS3_ +1 / MISSING insertERKS4_ 1）。
                if (it != script->m_map49c4.end())
                {
                    ++it->second;
                }
                else
                {
                    script->m_map49c4.insert(std::make_pair(a, 1));
                }
            }
        }
        else if (line == "[pvp item give]")
        {
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec49dc.push_back(v);
            }
        }
        else if (line == "[bring up event]")
        {
            if (!ScanType(line, true))
            {
                goto lab_finish;
            }
            if (line == "[get eventitem fatigue]")
            {
                while (true)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    script->m_eventIdolBringUp454.m_vec8.push_back(v);
                }
            }
            if (!ScanType(line, true))
            {
                goto lab_finish;
            }
            // b28d-solo-ServerParameterScript: ORIG 全函数只用 eq（_ZSteqIcSt11char…）
            // ⇒ 原码为正向 `if (line == …)`（p167 EXTRA _ZStneIc… 1 / MISSING eq 1）。
            if (line == "[get reward count]")
            {
                script->m_eventIdolBringUp454.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    goto lab_finish;
                }
                script->m_eventIdolBringUp454.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    goto lab_finish;
                }
            }
        }
        else if (line == "[protect of upgrade down jpn]")
        {
            while (true)
            {
                unsigned int key = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stUpgradeItemTicketParam_t p;
                char* pb = reinterpret_cast<char*>(&p);
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                pb[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                pb[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int f4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (0 < f4)
                {
                    pb[3] = 1;
                }
                script->m_ticket468.insert(std::make_pair(key, p));
            }
        }
        else if (line == "[protect of upgrade down]")
        {
            while (true)
            {
                unsigned int key = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stUpgradeItemTicketParam_t p;
                char* pb = reinterpret_cast<char*>(&p);
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                pb[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                pb[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int f4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (0 < f4)
                {
                    pb[3] = 1;
                }
                script->m_ticket468.insert(std::make_pair(key, p));
            }
        }
        else if (line == "[weekend bonus fatigue]")
        {
            script->m_weekendBouns4b4.m_bonusFatiguePercent =
                (unsigned short)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[weekend bonus exp]")
        {
            script->m_weekendBouns4b4.m_bonusExpPercent = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[drop routin by party]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_weekendBouns4b4.m_dropRoutineByParty[i] =
                    (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
            }
        }
        else if (line == "[drop upgrade rarity]")
        {
            while (true)
            {
                std::string s;
                if (!ScanStr(&s))
                {
                    break;
                }
                // b28d-solo-ServerParameterScript: ORIG 0x08a8d70e 段
                // `make_pairIR11ENUM_RARITYR15ItemFarmingData` ⇒ 第一参类型为
                // ENUM_RARITY 左值（p167 MISSING 该实例 1 + pair C1I11ENUM_RARITYS1_
                // 1 + pair D1 2 / EXTRA make_pairIRiR15ItemFarmingData 1）。
                ENUM_RARITY rarity = (ENUM_RARITY)6;
                if (s == "[common]")
                {
                    rarity = (ENUM_RARITY)0;
                }
                else if (s == "[uncommon]")
                {
                    rarity = (ENUM_RARITY)1;
                }
                else if (s == "[rare]")
                {
                    rarity = (ENUM_RARITY)2;
                }
                else if (s == "[unique]")
                {
                    rarity = (ENUM_RARITY)3;
                }
                else if (s == "[epic]")
                {
                    rarity = (ENUM_RARITY)4;
                }
                else if (s == "[chronicle]")
                {
                    rarity = (ENUM_RARITY)5;
                }
                ItemFarmingData data;
                while (true)
                {
                    int itemId = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    int weight = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    data.m_active += weight;
                    data.m_vecWeight.push_back(std::pair<int, int>(itemId, weight));
                }
                script->m_farming4c0.insert(std::make_pair(rarity, data));
            }
        }
        else if (line == "[drop bonus of dungeon level increase]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                float b = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: ORIG 用 push_backERKS1_（左值实参）
                // ⇒ 具名 pair 局部再传（p167 MISSING push_backERKS1_ 1 / EXTRA
                // push_backEOS1_ 1）。
                std::pair<int, float> entry = std::make_pair(a, b);
                script->m_vec4d8.push_back(entry);
            }
        }
        // [N-类·跨版本 EH] 本函数 4 处局部对象段(auction480/chaosWeight4e4/
        // ghostLevel4f0/ghostChaos4fc，ORIG tag $0xd67/$0xd8b/$0xda3/$0xdbb)我们
        // 各多 1 个 8 行 landing pad(mov%edx,%ebx;mov%eax,%esi;lea obj;call D1;
        // 还原;jmp)。ORIG=gcc4.1 SJLJ EH：清理走函数级 personality，无 per-call
        // landing pad(ORIG @0x08a87f16 后 4 段实测 0 条 cleanup)；4.4.7 DW2 每
        // potential-throw 区生成。正常路径 D1 调用两侧一致，仅冷路径多。豁免。
        else if (line == "[auction register item limit]")
        {
            while (true)
            {
                stAuctionLimitItemParam_t p;
                p.m_itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_startSec = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_endSec = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_auction480.push_back(p);
            }
        }
        else if (line == "[my assault chaos point weight]")
        {
            while (true)
            {
                stAssaultChaosPointWeight w;
                w.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                w.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                w.m_w0 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                w.m_w1 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_chaosWeight4e4.push_back(w);
            }
        }
        else if (line == "[assault level ghost panalty]")
        {
            while (true)
            {
                stAssaultLevelGhostPanalty g;
                g.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                g.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_t0 = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_t1 = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_ghostLevel4f0.push_back(g);
            }
        }
        else if (line == "[assault chaos point ghost panalty]")
        {
            while (true)
            {
                stAssaultChaosPointGhostPanalty g;
                g.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                g.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_r0 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_r1 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_ghostChaos4fc.push_back(g);
            }
        }
        else
        {
            missed = true;
        }
        if (missed)
        {
            printf("MISSED SCRIPT TAG [%s] %s\n", path, line.c_str());
        }
    }
lab_finish:
    return script->m_558 != -1;
}

