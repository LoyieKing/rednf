// df_game_r Game/ expert_job 核心类型实现（地址证据见 CExpertJob.h 头注释）。
#include "CExpertJob.h"

#include "CDataManager.h"
#include "CExpertJobList.h"
#include "CInventory.h"
#include "CItem.h"
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM
#include "CStreamGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include <cstring>

#include "GameRand.h"
#include "SigTypes.h"
#include "SkillSlot.h"

namespace collision_detection
{

Rectangle::Rectangle() : m_pos(0, 0), m_width(0), m_height(0) {}

// ORIG W 0x85cb492：四段包含判定（边界相等视为命中），任一命中返回 true。
bool Rectangle::include(Rectangle const& other) const
{
    // 块A：other 左上点落在本矩形内
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x <= m_pos.m_x + m_width &&
        other.m_pos.m_y >= m_pos.m_y &&
        other.m_pos.m_y <= m_pos.m_y + m_height)
        return true;

    // 块B：other 整体包含于本矩形
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x + other.m_width <= m_pos.m_x + m_width &&
        other.m_pos.m_y >= m_pos.m_y &&
        other.m_pos.m_y + other.m_height <= m_pos.m_y + m_height)
        return true;

    // 块C：other 左边在 x 区间且下边在 y 区间
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x <= m_pos.m_x + m_width &&
        other.m_pos.m_y + other.m_height >= m_pos.m_y &&
        other.m_pos.m_y + other.m_height <= m_pos.m_y + m_height)
        return true;

    // 块D：other 右边在 x 区间且下边在 y 区间
    if (other.m_pos.m_x + other.m_width >= m_pos.m_x &&
        other.m_pos.m_x + other.m_width <= m_pos.m_x + m_width &&
        other.m_pos.m_y + other.m_height >= m_pos.m_y &&
        other.m_pos.m_y + other.m_height <= m_pos.m_y + m_height)
        return true;

    return false;
}

}  // namespace collision_detection

namespace expert_job
{

ExpertJobMaterial::ExpertJobMaterial() { Reset(); }

// ORIG 0x849f252：清零 +0x00/+0x04/+0x08 与 +0x0c/+0x0e（short）。
void ExpertJobMaterial::Reset()
{
    m_field00 = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
    m_field0e = 0;
}

// ORIG 0x849f2b4：仅置 vptr。
CExpertJob::CExpertJob() {}

CExpertJob::~CExpertJob() {}

// ORIG 0x8234796：return *(int*)(this+0x04)。
int CExpertJob::GetType() { return m_type; }

// ORIG 0x849f148：*(int*)(this+0x04) = type。
void CExpertJob::SetType(ENUM_EXPERT_JOB_TYPE type) { m_type = (int)type; }

// ORIG 0x85d3d7e：IObject 基类构造 -> Rectangle(+0x04) -> CUser*+0x14 清零 ->
// std::string(+0x18) 默认构造。
CDisjointMachine::CDisjointMachine()
    : m_rect(), m_user(0), m_name(), m_field1c(0), m_field20(0),
      m_field24(0), m_field28(0)
{
}

CDisjointMachine::~CDisjointMachine() {}

// ORIG 0x85d3e1e：+0x14 置空、m_name.clear()、+0x1c 清零、+0x20 清零。
void CDisjointMachine::reset()
{
    m_user = 0;
    m_name.clear();
    m_field1c = 0;
    m_field20 = 0;
}

// ORIG 0x85d1cc2：m_user 为空时走 cMyTrace 断言并返回 0；
// 否则 put_byte(0) + put_disjoint_machine_info 后返回 1。
bool CDisjointMachine::put_object_info(PacketGuard& p)
{
    if (m_user == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x33, 0);
        trace("disjoint machine user is NULL");
        return false;
    }
    p.put_byte(0);
    put_disjoint_machine_info(p);
    return true;
}

// ORIG 0x85d1bc0：short(unique_id 低16位)、int(name.length)、str(name)、
// byte(Vill)、byte(get_area(false))、short(+0x24)、short(+0x28)、int(+0x1c)。
void CDisjointMachine::put_disjoint_machine_info(PacketGuard& p)
{
    p.put_short(static_cast<int>(m_user->get_unique_id() & 0xffff));
    p.put_int(static_cast<int>(m_name.length()));
    p.put_str(m_name.c_str(), static_cast<int>(m_name.length()));
    p.put_byte(static_cast<int>(m_user->getCurCharacVill()));
    p.put_byte(m_user->get_area(false));
    p.put_short(m_field24);
    p.put_short(m_field28);
    p.put_int(m_field1c);
}

// ORIG 0x85d1d3c：基类 CExpertJob -> CDisjointMachine@+0x8。
CDisjointer::CDisjointer() : m_machine() {}

CDisjointer::~CDisjointer() {}

// ORIG 0x849f2c2：基类 CExpertJob -> ExpertJobMaterial[2]@+0x8。
CEnchanter::CEnchanter() : m_materials() {}

CEnchanter::~CEnchanter() {}

// ============================================================================
// CExpertJob 静态工具（ORIG，见 expert_job__CExpertJob/*.md）
// ============================================================================

// GetSuccessRate @ 0x849b2e8：查 m_recipeLevelMap（itemIndex→所需等级），
// 所需等级 - 当前等级 = 等级差 → GetCompoundRate(差) 取率表项；variation =
// ROUND(CInventory::GetExpertJobCompoundRateVariation(type, level))。
stCompoundRate* CExpertJob::GetSuccessRate(CUser* user, CStackableItem const* item,
                                           int& variation)
{
    variation = 0;
    user->GetCurCharacExpertJobType();
    CDataManager* cm = G_CDataManager();
    STExpertJobScript* script =
        (STExpertJobScript*)cm->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return 0;
    int itemIndex = ((CItem*)item)->get_index();
    std::map<int, int>::iterator it = script->m_recipeLevelMap.find(itemIndex);
    if (it == script->m_recipeLevelMap.end())
        return 0;
    int requiredLevel = it->second;
    int levelDiff = requiredLevel -
                    user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    stCompoundRate* rate = script->GetCompoundRate(levelDiff);
    if (rate == 0)
        return 0;
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    int level = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    float f = inven->GetExpertJobCompoundRateVariation(
        (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(), level);
    // ORIG fistpl 四舍五入（round-to-nearest）
    variation = (int)(f < 0 ? f - 0.5f : f + 0.5f);
    return rate;
}

// IsSuccess @ 0x849b462：base 成功率 = rate->m_successRate + variation；
// 随机 100 < base 则成功：resultExp ∈ [expMin, expMax]，再加
// GetExpertJobAddtionalExp 的额外经验（概率 m_field0，量 m_field4）。
bool CExpertJob::IsSuccess(CUser* user, CStackableItem const* item, int& resultExp)
{
    int variation = 0;
    stCompoundRate* rate = GetSuccessRate(user, item, variation);
    if (rate == 0)
        return false;
    int base = rate->m_successRate + variation;
    if (get_rand_int(100) < base)
    {
        resultExp = get_rand_int(rate->m_expMax - rate->m_expMin + 1) + rate->m_expMin;
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        STItemScript::SEXPERTJOB::stExpertJobAdditionalExp ae;
        int level = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
        inven->GetExpertJobAddtionalExp(
            (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(), level, ae);
        if ((float)get_rand_int(100) < ae.m_field0)
            resultExp += ae.m_field4;
        return true;
    }
    return false;
}

// IsLearnRecipe @ 0x849b590：GetRecipeInfoIndex(itemIndex) 找到 recipe 且
// 已学习（get_recipe）或 GM → true；否则 cMyTrace + false。
bool CExpertJob::IsLearnRecipe(CUser* user, CStackableItem const* item)
{
    user->GetCurCharacExpertJobType();
    CDataManager* cm = G_CDataManager();
    STExpertJobScript* script =
        (STExpertJobScript*)cm->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return false;
    int idx = script->GetRecipeInfoIndex(((CItem*)item)->get_index());
    if (idx < 0)
        return false;
    CCharacExpertJob* expand =
        (CCharacExpertJob*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0);
    if (expand->get_recipe(idx) != 1)
    {
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
            return false;
    }
    return true;
}

// IncreaseExpertJobExp @ 0x849ae40：clamp exp 至 m_expVec 末项上限；跨级/边界
// 处理 isBoundaryExpValue + isExpertJobLimitLevel；SetCurCharacExpertJobExp +
// make_basic_info 包；非边界本地 Send，边界则 LogHistory + send_all + 升级技能
// m_growSkillVec 循环 + send_skill_info + quest 包。
void CExpertJob::IncreaseExpertJobExp(CUser* user, int exp)
{
    if (user->getCurCharacR() == 0)
        return;
    user->GetCurCharacExpertJobType();
    CDataManager* cm = G_CDataManager();
    STExpertJobScript* script =
        (STExpertJobScript*)cm->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return;
    int maxExp = script->m_expVec[script->m_expVec.size() - 1].m_exp;
    if (maxExp < exp)
        exp = maxExp;

    bool isBoundary = false;
    int curLevel = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    int newLevel = user->GetCurExpertJobLevel(exp);
    STExpertJobEtcScript* etc = (STExpertJobEtcScript*)cm->GetExpertJobEtcScript();
    if (curLevel != newLevel)
    {
        std::map<unsigned char, short>* lvlMap = &etc->m_levelMap;
        std::map<unsigned char, short>::iterator it =
            lvlMap->find((unsigned char)newLevel);
        if (it != lvlMap->end() && (int)it->second <= user->get_charac_level())
            isBoundary = true;
    }
    else
    {
        bool b = false;
        if (script->isBoundaryExpValue(exp))
        {
            if (etc->isExpertJobLimitLevel(user->get_charac_level()))
                b = true;
        }
        if (b && curLevel != 0xb)
            isBoundary = true;
    }

    int prevExp = script->m_expVec[curLevel - 1].m_exp;
    if (!isBoundary && prevExp < exp)
        exp = prevExp;
    if (isBoundary || user->GetCurCharacExpertJobExp() != exp)
    {
        user->SetCurCharacExpertJobExp(exp);
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->clear();
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        user->make_basic_info((char*)&packet, 0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        if (!isBoundary)
        {
            user->Send(packet);
        }
        else
        {
            int lv = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            user->LogHistory("ExpertJobLv+,%d,%d,%d", user->get_charac_level(),
                             user->GetCurCharacExpertJobType(), lv);
            GameWorld* gw = G_GameWorld();
            gw->send_all(packet, user);
            for (size_t i = 0; i < script->m_growSkillVec.size(); ++i)
            {
                int sk = script->m_growSkillVec[i].first;
                int lv2 = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), sk, lv2, (ENUM_SKILL_TREE_KIND)0);
                sk = script->m_growSkillVec[i].first;
                lv2 = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), sk, lv2, (ENUM_SKILL_TREE_KIND)1);
            }
            user->send_skill_info();
            PacketGuard questPacket;
            user->getCurCharacQuestR()->get_quest_info((char*)&questPacket);
            user->Send(questPacket);
        }
    }
}

// ============================================================================
// CCharacExpertJob（ORIG，见 expert_job__CCharacExpertJob/*.md）
// ============================================================================

// ctor 0x0832af52：仅基类 CData 构造（vptr），成员未初始化（loadData 填充）。
CCharacExpertJob::CCharacExpertJob()
{
}

CCharacExpertJob::~CCharacExpertJob()
{
}

// InitScript 0x0832b36c：m_recipeInfo = script+0x274；m_field0c = 0；清 30 字节
// recipe flag；alter()。
bool CCharacExpertJob::InitScript(STExpertJobScript* script)
{
    m_recipeInfo = *(int*)((char*)script + 0x274);
    m_field0c = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
    alter();
    return 1;
}

// get_recipe 0x0832b306：idx = index>>3，越界(<0 || >0x1d)返回 0；
// 返回 bitmap 第 (index%8) 位。
int CCharacExpertJob::get_recipe(int index) const
{
    int idx = index >> 3;
    if (idx < 0 || idx > 0x1d)
        return 0;
    int bit = index % 8;
    return ((int)(char)m_recipeFlag[idx] >> (bit & 0x1f)) & 1;
}

// set_recipe 0x0832b264：未置位则置位并返回 1；已置位返回 0。
int CCharacExpertJob::set_recipe(int index)
{
    int idx = index >> 3;
    if (idx < 0 || idx > 0x1d)
        return 0;
    int bit = index % 8;
    if ((((int)(char)m_recipeFlag[idx] >> (bit & 0x1f)) & 1) == 0)
    {
        m_recipeFlag[idx] |= (unsigned char)(1 << (bit & 0x1f));
        return 1;
    }
    return 0;
}

// learnRecipe 0x0832b3c0：GetRecipeInfoIndex(recipeIndex)；index<0 → 0；
// GetRecipeInfo(index) 为空 → 0；recipeInfo->m_field8 <= param+2 才 set_recipe + alter。
int CCharacExpertJob::learnRecipe(unsigned long recipeIndex, STExpertJobScript* script,
                                  int param)
{
    int idx = script->GetRecipeInfoIndex(recipeIndex);
    if (idx < 0)
        return 0;
    stRecipeInfo* info = script->GetRecipeInfo(idx);
    if (info == 0)
        return 0;
    if (param + 2 < (unsigned int)info->m_field8)
        return 0;
    set_recipe(idx);
    alter();
    return 1;
}

// loadData 0x0832b1fe：从 pData 拷贝 +0x06/+0x08/+0x0c 与 30 字节 recipe flag。
bool CCharacExpertJob::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    m_giveupCount = *(unsigned short*)(pData + 4);
    m_recipeInfo = *(int*)(pData + 8);
    m_field0c = *(int*)(pData + 0xc);
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = (unsigned char)pData[i + 0x10];
    return 1;
}

// _saveData 0x0832b0ac：StreamGuard 写 0x181 头 + UID + SIG_EXPERT_JOB 载荷，
// MsgQueueMgr::put(DB_Q)。
bool CCharacExpertJob::_saveData(CUser* pUser)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("CharacExpertJob.cpp", 0x16);
    CStreamGuard guard(s, true);
    guard << 0x181;
    guard << (int)pUser->GetUID();
    SIG_EXPERT_JOB* buf = guard->GetInBuffer<SIG_EXPERT_JOB>();
    memset(buf, 0, 0x30);
    *(int*)buf = (int)pUser->getCurCharacNo();
    *(unsigned short*)((char*)buf + 4) = m_giveupCount;
    *(int*)((char*)buf + 8) = m_recipeInfo;
    *(int*)((char*)buf + 0xc) = m_field0c;
    memcpy((char*)buf + 0x10, m_recipeFlag, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// _reset 0x0832b064：清 +0x06/+0x08/+0x0c 与 30 字节 recipe flag。
void CCharacExpertJob::_reset()
{
    m_giveupCount = 0;
    m_recipeInfo = 0;
    m_field0c = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
}

unsigned short CCharacExpertJob::get_giveup_count() const { return m_giveupCount; }

void CCharacExpertJob::set_giveup_count(unsigned short c) { m_giveupCount = c; }

}  // namespace expert_job
