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

// ORIG W 0x85cb492：两矩形相交四情形（任一命中返回 true）：
//   左上角点包含 / 右边界x带×上边y带 / 上边x带×下边y带 / 右边界x带×下边y带。
bool Rectangle::include(Rectangle const& other) const
{
    // 块A：other 左上点落在本矩形内
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x <= m_pos.m_x + m_width &&
        other.m_pos.m_y >= m_pos.m_y &&
        other.m_pos.m_y <= m_pos.m_y + m_height)
        return true;

    // 块B：other 右边界 x 落在本矩形内且上边 y 在 y 区间
    if (other.m_pos.m_x + other.m_width >= m_pos.m_x &&
        other.m_pos.m_x + other.m_width <= m_pos.m_x + m_width &&
        other.m_pos.m_y >= m_pos.m_y &&
        other.m_pos.m_y <= m_pos.m_y + m_height)
        return true;

    // 块C：other 左边 x 在 x 区间且下边 y 落在本矩形内
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x <= m_pos.m_x + m_width &&
        other.m_pos.m_y + other.m_height >= m_pos.m_y &&
        other.m_pos.m_y + other.m_height <= m_pos.m_y + m_height)
        return true;

    // 块D：other 右边界 x 与下边 y 均落在本矩形内
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

// ORIG 0x849f252：先清 +0x0c/+0x0e（short），再清 +0x00/+0x04/+0x08。
void ExpertJobMaterial::Reset()
{
    m_slot = 0;
    m_field0e = 0;
    m_user = 0;
    m_item = 0;
    m_itemIndex = 0;
}

// ORIG 0x849f2b4：仅置 vptr。
CExpertJob::CExpertJob() {}

CExpertJob::~CExpertJob() {}

// ORIG 0x8234796：return *(int*)(this+0x04)。
int CExpertJob::GetType() { return m_type; }

// ORIG 0x849f148：*(int*)(this+0x04) = type。
// b28：字段枚举化（CExpertJob.h m_type）后 4.4 store 序翻为 LHS-first，
// 与 ORIG mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx 同形。
void CExpertJob::SetType(ENUM_EXPERT_JOB_TYPE type) { m_type = type; }

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
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
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
    // ORIG 0x849b436：fistpl 前 fldcw 置截断模式（mov $0xc,%ah，RC=11），
    // 直接向零截断，无 ±0.5 四舍五入分支。
    variation = (int)f;
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
// 已学习（get_recipe）或 GM → true；否则对应 cMyTrace + false。
bool CExpertJob::IsLearnRecipe(CUser* user, CStackableItem const* item)
{
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x13f, 0);
        trace("EXPERT_JOB_ERROR : GetExpertJobScript() charac_no(%d)",
              user->getCurCharacNo());
        return false;
    }
    int idx = script->GetRecipeInfoIndex(((CItem*)item)->get_index());
    if (idx < 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x146, 0);
        trace("EXPERT_JOB_ERROR : GetRecipeInfoIndex() charac_no(%d)",
              user->getCurCharacNo());
        return false;
    }
    CCharacExpertJob* expand =
        (CCharacExpertJob*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0);
    if (expand->get_recipe(idx) != 1)
    {
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14e, 0);
            trace("EXPERT_JOB_ERROR : get_recipe() charac_no(%d)",
                  user->getCurCharacNo());
            return false;
        }
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
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return;
    int maxExp = script->m_expVec[script->m_expVec.size() - 1].m_exp;
    if (maxExp < exp)
        exp = maxExp;

    bool isBoundary = false;
    int curLevel = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    int newLevel = user->GetCurExpertJobLevel(exp);
    STExpertJobEtcScript* etc =
        (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
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

// InitScript 0x0832b36c：m_endurance = script->m_enduranceInit（+0x274，
// [endurance initial value]）；m_enchantCount = 0；清 30 字节 recipe flag；
// alter()。
bool CCharacExpertJob::InitScript(STExpertJobScript* script)
{
    m_endurance = script->m_enduranceInit;
    m_enchantCount = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
    alter();
    return 1;
}

// get_recipe 0x0832b306：idx = index/8（带符号除），越界(<0 || >0x1d)返回 0；
// 返回 bitmap 第 (index%8) 位。
// b28c-1 N97: 手写除形后余 mov/sar/shr/lea vs 4.4 sar/shr/add 寄存器选择差。
int CCharacExpertJob::get_recipe(int index) const
{
    // b28c-1：ORIG 0x0832b306 `mov;sar $0x1f;shr $0x1d;lea;sar $3` 四步
    //  ⇒ 手写 (index + ((unsigned)(index>>31) >> 29)) >> 3 复现（N97 翻形）。
    int idx = (index + (int)(((unsigned int)(index >> 31)) >> 29)) >> 3;
    int bit = index % 8;
    if (idx < 0 || idx > 0x1d)
        return 0;
    return ((int)(char)m_recipeFlag[idx] >> bit) & 1;
}

// set_recipe 0x0832b264：未置位则置位并返回 1；已置位返回 0。
// b28c-1 N97: 同上（手写 (index+((uint)(index>>31)>>29))>>3 已消 cmovs）。
int CCharacExpertJob::set_recipe(int index)
{
    // b28：4.4 对 index/8 恒出 lea+cmovs（4.1 为 sar/shr/lea 三步、无 cmov）；
    // 改 (index + ((index>>31)&7)) >> 3 恒等复现 ORIG 三步形（负数经边界检查返 0，
    // 越界读不可达）。
    int idx = (index + (int)(((unsigned int)(index >> 31)) >> 29)) >> 3;
    int bit = index % 8;   // ORIG 保留完整除余链（mov/sar/shr/add/and/sub）
    if (idx < 0 || idx > 0x1d)
        return 0;
    // b28 P222 两形互试：负极早返（ORIG je→set 岛形，al=1 即失败直落 mov$0）。
    if ((((int)(char)m_recipeFlag[idx] >> bit) & 1) != 0)
        return 0;
    m_recipeFlag[idx] |= (unsigned char)(1 << bit);
    return 1;
}


// learnRecipe 0x0832b3c0：GetRecipeInfoIndex(recipeIndex)；index<0 → 0；
// GetRecipeInfo(index) 为空 → 0；recipeInfo->m_field8 <= param+2 才 set_recipe + alter。
// b28c-1 N: 符号位测试 4.4 恒折 cmpl/jns vs ORIG mov/shr/test（跨版本 lowering）。
bool CCharacExpertJob::learnRecipe(unsigned long recipeIndex, STExpertJobScript* script,
                                  int param)
{
    int idx = -1;                                   // ORIG：-1 初始化
    idx = script->GetRecipeInfoIndex(recipeIndex);
    // b28：ORIG 判负形为 `(unsigned)idx >> 31` 提取符号位 + test/je 岛（P222 互试）。
    if ((int)((unsigned int)idx >> 31) != 0)
        return 0;
    stRecipeInfo* info = script->GetRecipeInfo(idx);
    if (info == 0)
        return 0;
    // b28 P222 互试：正极性 `mf8 > param+2 → 0`（ORIG cmp %edx,%eax; jle continue）。
    if ((int)(unsigned int)info->m_field8 > param + 2)
        return 0;
    set_recipe(idx);
    alter();
    return 1;
}

// loadData 0x0832b1fe：pData 即 DB 侧 SIG_EXPERT_JOB 载荷（wire 布局见
// SigTypes.h），按具名字段拷入。
bool CCharacExpertJob::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    const SIG_EXPERT_JOB* sig = (const SIG_EXPERT_JOB*)pData;
    m_giveupCount = sig->m_giveupCount;
    m_endurance = sig->m_endurance;
    m_enchantCount = sig->m_enchantCount;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = sig->m_recipeFlag[i];
    return 1;
}

// _saveData 0x0832b0ac：StreamGuard 写 0x181 头 + UID + SIG_EXPERT_JOB 载荷
// （DB wire 格式，结构定义见 SigTypes.h），MsgQueueMgr::put(DB_Q)。
bool CCharacExpertJob::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacExpertJob.cpp", 0x16),
        true);
    **guard << 0x181;
    **guard << (int)pUser->GetUID();
    SIG_EXPERT_JOB* buf = guard->GetInBuffer<SIG_EXPERT_JOB>();
    memset(buf, 0, sizeof(SIG_EXPERT_JOB));   // wire 载荷整体清零（含 pad）
    buf->m_characNo = (int)pUser->getCurCharacNo();
    buf->m_giveupCount = m_giveupCount;
    buf->m_endurance = m_endurance;
    buf->m_enchantCount = m_enchantCount;
    memcpy(buf->m_recipeFlag, m_recipeFlag, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// _reset 0x0832b064：清 +0x06(giveup)/+0x08(endurance)/+0x0c(enchant) 与 30 字节 recipe flag。
void CCharacExpertJob::_reset()
{
    m_giveupCount = 0;
    m_endurance = 0;
    m_enchantCount = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
}

// ORIG 0x849f5ac：先 CData::alter() 再写 +0x06。
void CCharacExpertJob::set_giveup_count(unsigned short c)
{
    alter();
    m_giveupCount = c;
}

unsigned short CCharacExpertJob::get_giveup_count() const { return m_giveupCount; }

}  // namespace expert_job
