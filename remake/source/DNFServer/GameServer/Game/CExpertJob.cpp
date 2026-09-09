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
// [b28d-solo-CExpertJob] AE（110=110）：P0 去 requiredLevel/level 具名局部（ORIG
//   0x0849b3a6/0x0849b416 均 %ebx 免槽跨 call 保活）+ P198① 函数头预清 +
//   P309 levelDiff 先于 script/variation 声明（复现 -0x1c/-0x18 槽序）；p167 CLEAR。
//   ORIG 0x0849b2e8
stCompoundRate* CExpertJob::GetSuccessRate(CUser* user, CStackableItem const* item,
                                           int& variation)
{
    // [b28d-solo-CExpertJob P198①/P309] ORIG 函数头第一条即 `movl $0x0,-0x1c(%ebp)`
    // （levelDiff 预清）且其槽(-0x1c)深于 script(-0x18) ⇒ levelDiff 必须在 script
    // 之前声明（4.4 头部声明先声明深）、且先于 `variation = 0` 的写出参 store，
    // 否则预清落在中段/顺序反。
    int levelDiff = 0;
    variation = 0;
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return 0;
    int itemIndex = ((CItem*)item)->get_index();
    std::map<int, int>::iterator it = script->m_recipeLevelMap.find(itemIndex);
    if (it == script->m_recipeLevelMap.end())
        return 0;
    // [b28d-solo-CExpertJob P0] ORIG 0x0849b3a6 `mov 0x4(%eax),%ebx` 把 it->second
    // 直传 %ebx 跨 GetCurCharacExpertJobExp/GetCurExpertJobLevel 保活（无槽）⇒ 禁
    // 具名 requiredLevel 局部（P198① 预清见函数头声明）。
    levelDiff = it->second -
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    stCompoundRate* rate = script->GetCompoundRate(levelDiff);
    if (rate == 0)
        return 0;
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    // [b28d-solo-CExpertJob P0] ORIG 0x0849b416 `mov %eax,%ebx`：level 结果同样免槽
    // 跨 getCurCharacExpertJobType 保活（实参 R2L：level 先求值、type 后求值）。
    float f = inven->GetExpertJobCompoundRateVariation(
        (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(),
        user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()));
    // ORIG 0x849b436：fistpl 前 fldcw 置截断模式（mov $0xc,%ah，RC=11），
    // 直接向零截断，无 ±0.5 四舍五入分支。
    variation = (int)f;
    return rate;
}

// IsSuccess @ 0x849b462：base 成功率 = rate->m_successRate + variation；
// 随机 100 < base 则成功：resultExp ∈ [expMin, expMax]，再加
// GetExpertJobAddtionalExp 的额外经验（概率 m_field0，量 m_field4）。
// [b28d-solo-CExpertJob] N85 封顶（93/92）：书写序旋钮
//   `base > get_rand_int(100)` 复现 ORIG 0x0849b49c 的 %ebx 免槽跨 call 保活
//   （lea/cmp/setg 段逐条全等）；剩余 = x87 比较 lowering（4.1
//   fucompp/fnstsw/test/sete vs 4.4 fucomip/fstp/seta，语义等价，N85 判例）。
//   p167 CLEAR。ORIG 0x0849b462
bool CExpertJob::IsSuccess(CUser* user, CStackableItem const* item, int& resultExp)
{
    int variation = 0;
    stCompoundRate* rate = GetSuccessRate(user, item, variation);
    if (rate == 0)
        return false;
    // [b28d-62] P0：ORIG 0x0849b49c 把 `rate->m_successRate + variation` 放 %ebx
    //   跨 get_rand_int 保活（无栈槽）⇒ 内联实参，禁具名 base 局部。
    // [b28d-solo-CExpertJob] 书写序旋钮：ORIG `lea (%edx,%eax,1),%ebx; movl
    //   $0x64,(%esp); call; cmp %eax,%ebx; setg` 证明源码左操作数是 base（先物化
    //   %ebx 跨 call 保活）；写成 `get_rand_int(100) < base` 时 4.4 把 call 提到
    //   前面重排（P243/P52 同族：操作数书写序决定求值/装载序）。
    if (rate->m_successRate + variation > get_rand_int(100))
    {
        resultExp = get_rand_int(rate->m_expMax - rate->m_expMin + 1) + rate->m_expMin;
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        STItemScript::SEXPERTJOB::stExpertJobAdditionalExp ae;
        // [b28d-62] P0：ORIG 0x0849b51f level 落 %ebx（无槽）⇒ 内联实参。
        inven->GetExpertJobAddtionalExp(
            (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(),
            user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()), ae);
        if ((float)get_rand_int(100) < ae.m_field0)
            resultExp += ae.m_field4;
        return true;
    }
    return false;
}

// IsLearnRecipe @ 0x849b590：GetRecipeInfoIndex(itemIndex) 找到 recipe 且
// 已学习（get_recipe）或 GM → true；否则对应 cMyTrace + false。
// [b28d-62] AE：P227 trace 全表达式链 + P0/P134 expand 免槽内联 + P266 字节
//   收窄 + P198① 预清 + get_recipe bool 化；105=105，p167 CLEAR。ORIG 0x0849b590
bool CExpertJob::IsLearnRecipe(CUser* user, CStackableItem const* item)
{
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
    {
        // [b28d-62] P227 全表达式临时链：ORIG 0x0849b5c9 先把 getCurCharacNo 结果
        //   入 %ebx 跨 cMyTrace::C1 保活，再调 op()；具名 trace 对象会槽化并错序。
        cMyTrace(__PRETTY_FUNCTION__, 0x13f, 0)(
            "EXPERT_JOB_ERROR : GetExpertJobScript() charac_no(%d)",
            user->getCurCharacNo());
        return false;
    }
    // [b28d-62] P198① 预清必写：ORIG 0x0849b60f `movl $0xffffffff,-0xc` 后独立赋值；
    //   符号测试用 P266 字节收窄形（4.4 对 `idx < 0` 恒折 cmpl/jns）。
    int idx = -1;
    idx = script->GetRecipeInfoIndex(((CItem*)item)->get_index());
    if ((unsigned char)((unsigned int)idx >> 31) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x146, 0)(
            "EXPERT_JOB_ERROR : GetRecipeInfoIndex() charac_no(%d)",
            user->getCurCharacNo());
        return false;
    }
    // [b28d-62] ORIG 0x0849b6b0 `call get_recipe; xor $0x1,%eax; test %al,%al; je`
    //   ⇒ 取反物化形（`!call()`）；expand 结果**免槽直用**（P0/P134：ORIG 无
    //   expand 槽，%eax 直接作 get_recipe 的 this）⇒ 内联表达式。
    if (!((CCharacExpertJob*)user->GetCharacExpandData(
              (ENUM_CHARAC_EXPAND_TYPE)0))
             ->get_recipe(idx))
    {
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x14e, 0)(
                "EXPERT_JOB_ERROR : get_recipe() charac_no(%d)",
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
// [b28d-solo-CExpertJob] N 封顶（语义完成、p167 CLEAR 60/60）：本件 341 条。
//   已复现：P0 去 lvlMap/gw/sk/lv2 具名局部（ORIG 0x0849af10/0x0849b127/0x0849b14e
//   均寄存器免槽）、P225 推广嵌套 if 复现 setle 物化（0x0849af76 逐条全等）、
//   P226 扩展语句表达式复现 b 短路物化岛零槽（0x0849af90 段）、P196 比较方向
//   `exp > prevExp`（0x0849aff8 jle 段全等）、块序 `if (isBoundary){长}else{Send}`
//   复现 0x0849b0c6 `cmpb;je 尾部` 布局。剩余差异（结构性、语义等价）：
//   ① 开头 getCurCharacR()==0 的 4.1 取反物化 `setne;je` vs 4.4 `sete;jne`
//      （setcc-fold 跨版本极性，两形实测均折）；② 尾部 4.4 epilogue-threading
//      `jmp;nop;jmp;nop` 岛（N121 三形不可消）。ORIG 0x0849ae40
void CExpertJob::IncreaseExpertJobExp(CUser* user, int exp)
{
    // [b28d-solo-CExpertJob N-setcc-fold] ORIG 0x0849ae54 `test %eax,%eax;
    //   setne %al; test %al,%al; je <尾>`（4.1 取反物化）；`p == 0`/`!(p!=0)` 两形
    //   4.4 均产 `sete %al; jne`（极性反、语义等价）⇒ 跨版本 setcc-fold 封顶。
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
        // [b28d-solo-CExpertJob P0] ORIG 0x0849af10 直接用 etc 重读 `mov -0x28(%ebp),
        // %edx; lea 0xc(%eax),%edx`（find 与 end 各重取一次），无 lvlMap 具名局部槽。
        std::map<unsigned char, short>::iterator it =
            etc->m_levelMap.find((unsigned char)newLevel);
        // [b28d-solo-CExpertJob P225 推广] ORIG 0x0849af76 `cmp %eax,%ebx;
        //   setle %al; test %al,%al; je`（4.1 对 <= 的 setcc 物化）。`&&` 合并形
        //   被 4.4 折成 jg/mov$1/mov$0 直跳；拆嵌套 if 即逐条复现（P225 推广实证）。
        if (it != etc->m_levelMap.end())
        {
            if ((int)it->second <= user->get_charac_level())
                isBoundary = true;
        }
    }
    else
    {
        // [b28d-solo-CExpertJob P226 扩展] ORIG 0x0849af90 短路物化岛零槽：
        //   `call isBoundaryExpValue; test; je L0; …call isExpertJobLimitLevel;
        //    test; je L0; mov $1,%eax; jmp L1; L0: mov $0,%eax; L1: test %al,%al;
        //    je skip; cmp $0xb,curLevel; je skip`。具名 bool b 会落栈槽
        //   （cmpb $0,-N），裸语句表达式复现零槽物化。
        if (({ script->isBoundaryExpValue(exp) &&
               etc->isExpertJobLimitLevel(user->get_charac_level()); }))
        {
            if (curLevel != 0xb)
                isBoundary = true;
        }
    }

    int prevExp = script->m_expVec[curLevel - 1].m_exp;
    // [b28d-solo-CExpertJob P196] ORIG 0x0849aff8 `mov 0xc(%ebp),%eax; cmp
    //   -0x24(%ebp),%eax; jle` ⇒ 比较方向 exp <= prevExp（源码写 exp > prevExp）。
    if (!isBoundary && exp > prevExp)
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
        // [b28d-solo-CExpertJob 块序] ORIG 0x0849b0c6 `cmpb $0x0,-0x31(%ebp);
        //   je 0849b2a2`（isBoundary==0 跳尾部 Send 块）⇒ 原码正极性
        //   `if (isBoundary) {边界块} else {Send}`，4.4 才把 Send 短块放到
        //   边界块（含 questPacket 析构）之后，与 ORIG 物理序一致。
        if (isBoundary)
        {
            int lv = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            user->LogHistory("ExpertJobLv+,%d,%d,%d", user->get_charac_level(),
                             user->GetCurCharacExpertJobType(), lv);
            // [b28d-solo-CExpertJob P0] ORIG 0x0849b127 `call G_GameWorld; mov %eax,
            // (%esp)` 直传 receiver，无 gw 具名局部槽。
            G_GameWorld()->send_all(packet, user);
            for (size_t i = 0; i < script->m_growSkillVec.size(); ++i)
            {
                // [b28d-solo-CExpertJob P0] ORIG 0x0849b14e body：lv2 落 %esi、sk 落
                //   %edi（均无槽）⇒ 禁具名 sk/lv2 局部，实参 R2L 内联（P157：每段重新
                //   operator[]）。
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), script->m_growSkillVec[i].first,
                    user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()),
                    (ENUM_SKILL_TREE_KIND)0);
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), script->m_growSkillVec[i].first,
                    user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()),
                    (ENUM_SKILL_TREE_KIND)1);
            }
            user->send_skill_info();
            PacketGuard questPacket;
            user->getCurCharacQuestR()->get_quest_info((char*)&questPacket);
            user->Send(questPacket);
        }
        else
        {
            user->Send(packet);
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
// [b28d-62] N97：有符号 /8 lowering（4.1 mov/sar/shr/lea vs 4.4 覆盖 %eax 回读
//   形参）；返回类型改 bool 后调用点 xor $1 形归零。ORIG 0x0832b306
// [b28d-solo-CExpertJob] N97 复核（fresh .o 逐条对齐）：剩余仅 idx 段 5 vs 6 条
//   （4.1 index 留 %eax/sign 走 %edx/lea vs 4.4 覆盖 %eax/add mem 回读）+ 尾 nop。
//   本轮再试 `(unsigned)index>>31>>29`(折 35 条更劣)、`(index>>31)&7`(sar/and/add
//   更劣)、去 (int) 版(shr 逻辑右移，语义错) ⇒ 维持封顶。
bool CCharacExpertJob::get_recipe(int index) const
{
    // [b28d-62] N97 封顶（三形实测）：ORIG 0x0832b30d `mov %eax,%edx; sar $0x1f,%edx;
    //   shr $0x1d,%edx; lea (%edx,%eax,1),%eax; sar $3`（4.1 有符号 /8 lowering，
    //   index 留 %eax、sign 走 %edx）；本形 4.4 恒覆盖 %eax 再回读形参
    //   (`sar/shr %eax; add 0xc(%ebp),%eax`，38 条，差 1 条 + 寄存器名)；
    //   `index/8` 生 cmovs 版（N97 原文）、`(unsigned)index>>31>>29` 被折成 35 条
    //   更劣、裸 register 使 idx 免槽且比较改寄存器版（39 vs 38 但槽位全错）
    //   ⇒ 源形不可造，语义等价封顶（ORIG 0x0832b306）。
    int idx = (index + (int)(((unsigned int)(index >> 31)) >> 29)) >> 3;
    int bit = index % 8;
    if (idx < 0 || idx > 0x1d)
        return 0;
    return (((int)(char)m_recipeFlag[idx] >> bit) & 1);
}

// set_recipe 0x0832b264：未置位则置位并返回 1；已置位返回 0。
// b28c-1 N97: 同上（手写 (index+((uint)(index>>31)>>29))>>3 已消 cmovs）。
// [b28d-62] N97：同 get_recipe（三形实测）。ORIG 0x0832b264
// [b28d-solo-CExpertJob] N97 复核（fresh .o 逐条对齐）：剩余仅 idx 段 5 vs 6 条
//   （同 get_recipe：4.1 mov/sar/shr/lea vs 4.4 sar/shr/add mem），其余 61 条全等
//   （含 P222 负极早返 set 岛形）；三形复测维持封顶。
int CCharacExpertJob::set_recipe(int index)
{
    // [b28d-62] N97 封顶（同 get_recipe 三形实测）：ORIG 0x0832b26b 为 4.1 有符号
    // /8 lowering `mov %eax,%edx; sar $0x1f,%edx; shr $0x1d,%edx; lea (%edx,%eax,1),
    // %eax; sar $3`；本手写 sign 形 4.4 恒覆盖 %eax 后回读形参（差 1 条 + 寄存器名），
    // `index/8` 生 cmovs、裸 register 使 idx 免槽 ⇒ 语义等价封顶（ORIG 0x0832b264）。
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
// [b28d-62] AE：P266 字节收窄符号测试（P198① 预清）后 46=46，p167 CLEAR。
//   ORIG 0x0832b3c0
bool CCharacExpertJob::learnRecipe(unsigned long recipeIndex, STExpertJobScript* script,
                                  int param)
{
    int idx = -1;                                   // ORIG：-1 初始化
    idx = script->GetRecipeInfoIndex(recipeIndex);
    // b28：ORIG 判负形为 `(unsigned)idx >> 31` 提取符号位 + test/je 岛（P222 互试）。
    // [b28d-62] 加 (unsigned char) 收窄：4.4 对 int 版恒折 `cmpl $0,-0x10; jns`；
    //   ORIG 是 `mov idx,%eax; shr $0x1f,%eax; test %al,%al; je`（P266 字节比较形）。
    if ((unsigned char)((unsigned int)idx >> 31) != 0)
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
// [b28d-solo-CExpertJob] AE（92=92，DIFF→AE）：P211 档③/P235 匿名 union 双视角
//   （SigTypes_B.h SIG_EXPERT_JOB::m_characNoU，与 getCurCharacNo() 同型）复现
//   ORIG 0x0832b163 `mov -0xc(%ebp),%edx; mov %eax,(%edx)` LHS-first 直存；
//   消费点 W8LT_DBTask.cpp 读 int 视角零回归（属主 S340 fresh 复验确认）。ORIG 0x0832b0ac
bool CCharacExpertJob::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacExpertJob.cpp", 0x16),
        true);
    **guard << 0x181;
    **guard << (int)pUser->GetUID();
    SIG_EXPERT_JOB* buf = guard->GetInBuffer<SIG_EXPERT_JOB>();
    memset(buf, 0, sizeof(SIG_EXPERT_JOB));   // wire 载荷整体清零（含 pad）
    // [b28d-solo-CExpertJob P211档③] getCurCharacNo() 返回 unsigned int；写 int 字段
    // 异型 ⇒ 4.4 恒 RHS-first+`mov %eax,%edx` 中转（多 1 条）。改走同址 unsigned
    // 视角（SigTypes_B.h union）后与 ORIG 0x0832b163 的 LHS-first 直存逐条全等。
    buf->m_characNoU = pUser->getCurCharacNo();
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
