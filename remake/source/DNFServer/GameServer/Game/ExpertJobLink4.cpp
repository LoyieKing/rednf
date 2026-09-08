// ============================================================================
// ExpertJobLink4.cpp — expert_job 域 LinkMisc4 批次
//
// 主符号（ORIG nm -S 核对）：
//   _ZN10expert_job10CAlchemist19OnTeleportCharacterEP5CUseriiiib
//       expert_job::CAlchemist::OnTeleportCharacter(CUser*, int, int, int, int, bool)
//       T 0x085d1424 sz 0x26f
//   _ZN10expert_job11CDisjointer21OnCreateDisjointStoreEP5CUserPKciii
//       expert_job::CDisjointer::OnCreateDisjointStore(CUser*, char const*, int, int, int)
//       T 0x085d2d08 sz 0x391
//   _ZN10expert_job11CDisjointer20get_disjoint_result_EP5CItemP10Inven_ItemiR14DisJointResultbP5CUser
//       expert_job::CDisjointer::get_disjoint_result_(CItem*, Inven_Item*, int,
//                                                     DisJointResult&, bool, CUser*)
//       T 0x085d234a sz 0x6cc
//   _ZN10expert_job13CExpertJobMgr16AcquireExpertJobE20ENUM_EXPERT_JOB_TYPE
//       expert_job::CExpertJobMgr::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE)
//       T 0x0849e1dc sz 0x1c
//
// 直接依赖符号（被上述主函数调用，报告/objdump 在案，一并忠实转写以闭合链接）：
//   CDisjointer::GetScript / GetEtcScript      T 0x085d1e0a / 0x085d1e3a
//   expert_job::get_additional_item            T 0x0896d6ab
//   CDisjointMachine 六参 ctor / operator=     T 0x085d1ae8 / W 0x085d3ed6
//   CCharacExpertJob::get_disjoint_machine_endurance/grade  W 0x85d3e66/0x85d3e8e
//   stItemSelectRate::stItemSelectRate         W 0x085d3bd6
//   DisjointMachineGrade 三参 ctor             W 0x085d3b9e
//   CAlchemist ctor / D1 / D0                  T 0x085d11b4 / 0x085d11d0 / 0x085d1200
// ============================================================================

// getItemSellPrice 定义于 Store.cpp（ORIG T 0x0898c86e）；仓库暂无共享头声明。
int getItemSellPrice(int price, int curEndurance, int maxEndurance,
                     bool isRandomOption);

#include "CExpertJob.h"

#include <math.h>

#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript(int) /
                               // GetExpertJobEtcScript
#include "CExpertJobList.h"    // STExpertJobEtcScript
#include "CItem.h"             // GetSellPrice/get_rarity/get_index/get_grade/
                               // IsRandomOption
#include "CItemAmplifier.h"    // stAmplifyOption_t::isIdentified
#include "CPrivateStoreMgr.h"  // private_store::{GetInstancePrivateStoreMgr,
                               // CPrivateStoreMgr::IsBusyPrivateStore}
#include "CUser.h"
#include "DisJoint.h"          // DisJointResult
#include "CStackableItem.h"       // m_int324(+0x324) 拆解券等级门槛
#include "DisPatcherCore.h"       // DisPatcher_DisJointItem（静态成员定义）
#include "GameRand.h"          // get_rand_int
#include "GameWorld.h"         // G_GameWorld 及各查询/移动接口
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // GlobalData::s_villageObjectMgr
#include "Inven_Item.h"        // Inven_Item::m_amp
#include "PacketGuard.h"


// ---- stItemSelectRate::stItemSelectRate（ORIG W 0x085d3bd6：5×int 清零）----
stItemSelectRate::stItemSelectRate()
{
    m_itemIndex = 0;
    m_rate      = 0;
    m_float8    = 0;
    m_rateMin   = 0;
    m_rateMax   = 0;
}

// ---- DisjointMachineGrade 三参 ctor（ORIG W 0x085d3b9e）----
// 参数装载实证：+0=m_grade=第2参、+1=m_field1=第1参、+2=m_field2=第3参。
DisjointMachineGrade::DisjointMachineGrade(unsigned char a, unsigned char b,
                                           unsigned char c)
{
    m_grade  = b;
    m_field1 = a;
    m_field2 = c;
}

// ---- DisjointMachineGrade 默认 ctor（ORIG W 0x0896dbcc：三字节清零）----
DisjointMachineGrade::DisjointMachineGrade()
{
    m_grade  = 0;
    m_field1 = 0;
    m_field2 = 0;
}

namespace expert_job
{

// ---------------------------------------------------------------------------
// 辅助：ORIG 浮点常量（.rodata 实测）
// ORIG 所有浮点→整型转换均显式置 FPU 控制字 RC=最近偶数（fldcw 0xc<<8，
// 0x85d246f flds+fnstcw+fistpl 实证），即 x87 就近取整（round-to-nearest-even，
// x.5 边界取偶）；以 (int)value 直接转换由编译器生成同一 fistpl 序列。
// 注意不可用 +0.5f 截断：2.5 → RC=even 得 2，+0.5 截断得 3。
// ---------------------------------------------------------------------------
namespace
{

inline int round_nearest(float value)
{
    return (int)value;
}

}  // namespace


// ---- expert_job::get_additional_item（报告 expert_job/get_additional_item.md，
//      ORIG T 0x0896d6ab）：顺序遍历表项写入 out；命中 [min,max] 区间后若剩余
//      随机值 >= 累加概率则返回，否则扣减继续。 ----
void get_additional_item(std::vector<stItemSelectRate> const& table, int randValue,
                         stItemSelectRate& out, int grade)
{
    for (std::vector<stItemSelectRate>::const_iterator it = table.begin();
         it != table.end(); ++it)
    {
        out = *it;
        if (out.m_rateMin <= grade && grade <= out.m_rateMax)
        {
            if (randValue >= out.m_rate)
                return;
            randValue -= out.m_rate;
        }
    }
}

// ---- CCharacExpertJob 分解机 getter（ORIG W 0x85d3e66 / 0x85d3e8e）----
int CCharacExpertJob::get_disjoint_machine_endurance() const
{
    return m_endurance;   // +0x08
}

int CCharacExpertJob::get_disjoint_machine_grade() const
{
    return m_enchantCount;   // +0x0c【字段名沿用既有推断名；getter 实证即分解机等级】
}

// ORIG W 0x085d3e72：alter() 后写 +0x08。
void CCharacExpertJob::set_disjoint_machine_endurance(int value)
{
    alter();
    m_endurance = value;
}

// ---- CAlchemist 生命周期（ctor 报告 expert_job__CAlchemist/CAlchemist.md；
//      dtor D1/D0 0x085d11d0/0x085d1200）----
CAlchemist::CAlchemist()
{
    // ORIG：基类构造后仅置 vptr（由编译器生成），无额外成员。
}

CAlchemist::~CAlchemist()
{
}

// ---- CAlchemist::OnTeleportCharacter（报告 OnTeleportCharacter.md，
//      ORIG T 0x085d1424 sz 0x26f）----
int CAlchemist::OnTeleportCharacter(CUser* user, int vill, int area, int x,
                                    int y, bool useGate)
{
    if (useGate)
    {
        area = G_GameWorld()->get_gate_area(vill);
        // ORIG 0x085d1453：shr $0x1f; test; je（4.1 符号位形；4.4 恒折 cmpl/jns，N86b）。
        if (area < 0)
            return 0;

        if (!G_GameWorld()->get_area_center_point(vill, area, x, y))
        {
            cMyTrace trace(
                "static bool expert_job::CAlchemist::OnTeleportCharacter("
                "CUser*, int, int, int, int, bool)", 0x8b, 5);
            trace("get_area_center_point() call failed");
            return 0;
        }
    }

    // ORIG 0x085d14d7：直接 if 判断（无 needMove 中间变量）。
    if ((char)user->getCurCharacVill() != vill ||
        user->get_area(false) != area)
    {
        if (!G_GameWorld()->move_area(user, vill, area, x, y, 0, false, 0, 0, 0))
            return 0;
    }
    else
    {
        user->set_position((unsigned short)x, (unsigned short)y, '\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(x);
    packet.put_short(y);
    packet.put_byte(0);
    packet.finalize(true);

    if (vill != 0 || area != 0)
        G_GameWorld()->send_to_area(vill, area, packet);

    return 1;
}

// ---- CDisjointer::GetScript / GetEtcScript（ORIG T 0x085d1e0a / 0x085d1e3a：
//      函数级静态缓存单例；ORIG 静态变量 b 0x09472f98 / 0x09472f94）----
STExpertJobScript* CDisjointer::GetScript()
{
    static STExpertJobScript* script = 0;   // ORIG GetScript()::script
    if (script == 0)
        script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(3);
    return script;
}

STExpertJobEtcScript* CDisjointer::GetEtcScript()
{
    static STExpertJobEtcScript* script = 0;   // ORIG GetEtcScript()::script
    if (script == 0)
        script = (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    return script;
}

// ---- CDisjointMachine 六参 ctor（objdump 0x085d1ae8 忠实转写）：
//      IObject 基类 → vptr → Rectangle 默认构造 → name 默认构造 → 逐字段赋值 →
//      碰撞盒 {x-0x50, y-0x32, 0xa0, 0x64}。 ----
CDisjointMachine::CDisjointMachine(CUser* user, char const* name, int level,
                                   char grade, int x, int y)
{
    m_user              = user;                 // +0x14
    m_name              = name;                 // +0x18
    m_field1c           = level;                // +0x1c
    m_field20           = grade;                // +0x20（char）
    m_field24           = x;                    // +0x24（ORIG 全宽 mov，无 ushort 截断）
    m_field28           = y;                    // +0x28

    m_rect.m_pos.m_x    = m_field24 - 0x50;     // Rectangle +0x00（ORIG 成员回读）
    m_rect.m_width      = 0xa0;                 // +0x08
    m_rect.m_pos.m_y    = m_field28 - 0x32;     // +0x04（同上）
    m_rect.m_height     = 0x64;                 // +0x0c
}

// ---- CDisjointMachine::operator=（ORIG W 0x085d3ed6：基类 IObject::operator=
//      空操作 + 拷 +0x04..+0x14 与 string/+0x1c/+0x20/+0x24/+0x28，不拷 vptr）----
CDisjointMachine& CDisjointMachine::operator=(CDisjointMachine const& other)
{
    village_object::IObject::operator=(other);   // ORIG W 0x85ccadc 同款空操作
    // b28：结构整体赋值（探针 /tmp/l3/p6.cpp 实证 4.4 出 ORIG 的 dest/src 双缓存
    // 4-dword 连发形：mov 0x4(%edx),%ecx…末条 %edx 自复用，逐字段形则 16 条异形）。
    m_rect = other.m_rect;
    m_user           = other.m_user;
    m_name           = other.m_name;
    m_field1c        = other.m_field1c;
    m_field20        = other.m_field20;
    m_field24        = other.m_field24;
    m_field28        = other.m_field28;
    return *this;
}

// ---- CDisjointer::OnCreateDisjointStore（报告 OnCreateDisjointStore.md，
//      ORIG T 0x085d2d08 sz 0x391）----
int CDisjointer::OnCreateDisjointStore(CUser* user, char const* name, int level,
                                       int x, int y)
{
    if (level < 0 || GetScript()->m_limitDisjointGold < level)   // +0x28c
        return 10;

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0xbe;

    if ((char)user->getCurCharacVill() == 7)
        return 0xbe;

    int area = user->get_area(false);
    char vill = (char)user->getCurCharacVill();

    if (!G_GameWorld()->is_available_point(vill, area, x, y))
        return 0xbe;

    int restrictErr =
        G_GameWorld()->check_restrictive_commercial_transaction_zone(vill, area, x, y);
    if (restrictErr != 0)
        return restrictErr;

    // 耐久检查（两次独立取扩展数据，与 ORIG 调用序一致）。
    if (((expert_job::CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))->get_disjoint_machine_endurance() <= 0)
        return 0x16;

    char machineGrade = (char)((expert_job::CCharacExpertJob*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0))->get_disjoint_machine_grade();

    // 局部构造后赋给 m_machine（ORIG 经栈上临时 + operator= + 临时析构）。
    CDisjointMachine local(user, name, level, machineGrade, x, y);
    m_machine = local;

    user->set_position((unsigned short)x, (unsigned short)y, '\0');

    village_object::Zone zone(user->getCurCharacVill(), user->get_area(false));
    if (!GlobalData::s_villageObjectMgr->register_object(zone, &m_machine))
        return 0x14;

    PacketGuard packet;
    packet.put_header(0, 0xce);
    m_machine.put_disjoint_machine_info(packet);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet);

    packet.clear();
    packet.put_header(1, 0xee);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);

    return 0;
}

// ---- CDisjointer::get_disjoint_result_（报告 get_disjoint_result_.md，
//      ORIG T 0x085d234a sz 0x6cc）。 ----
int CDisjointer::get_disjoint_result_(CItem* item, Inven_Item* invenItem,
                                      int level, DisJointResult& result,
                                      bool selfService, CUser* user)
{
    int itemIndex  = item->get_index();     // local_54（ORIG 计算后未再使用）
    int sellPrice  = item->GetSellPrice();  // local_50（×1.1 后作基价）
    int rarity     = item->get_rarity();    // local_4c
    int grade      = item->get_grade();     // local_48
    (void)itemIndex;

    bool unidentified = false;
    if (invenItem != 0 && !invenItem->m_amp.isIdentified())
        unidentified = true;

    // map<DisjointMachineGrade,stDisjointResult> 键：(a=rarity, b=level,
    // c=unidentified) → {m_grade=level, m_field1=rarity, m_field2=unidentified}。
    STExpertJobScript* script = GetScript();
    std::map<DisjointMachineGrade, stDisjointResult>::iterator it =
        script->m_disjointResultMap.find(
            DisjointMachineGrade((unsigned char)rarity, (unsigned char)level,
                                 unidentified));
    if (it == script->m_disjointResultMap.end())
        return unidentified ? 0xd3 : 0x11;

    stDisjointResult& res = it->second;

    sellPrice = round_nearest(1.1f * sellPrice);          // ROUND(_DAT_08cc080c × price)
    result.m_f[5] = res.m_field0;                         // +0x14 cube 索引

    int money = getItemSellPrice(sellPrice, 0, 0, item->IsRandomOption());
    money = (money * 1000) / GetScript()->m_baseConst;    // +0x1d4 [base const]
    result.m_f[0] = round_nearest(money * res.m_field4 / 1000.0f);   // _DAT_08cc0810
    if (result.m_f[0] < 1)
        result.m_f[0] = 1;
    result.m_f[6] = 0;                                    // +0x18 附加物 idx
    result.m_f[1] = 0;                                    // +0x04 附加物数量

    // 大奖基础率：res+0x20 ×100（导入侧第 6 列，当前导入 TU 未持久化——见
    // STExpertJobScript.h 注释）。
    int bigwinRate = res.m_field20 * 100;

    if (selfService)
    {
        unsigned char key = (unsigned char)rarity + 1;
        unsigned int rate = 0;
        std::map<unsigned char, unsigned char>::iterator etcIt =
            GetEtcScript()->m_disjointerBigWinRate.find(key);
        if (etcIt != GetEtcScript()->m_disjointerBigWinRate.end())
            rate = etcIt->second;
        bigwinRate += (int)(rate * 100);

        bool active = (user != 0 && user->get_state() >= 3 &&
                       user->getCurCharacR() != 0);
        if (active)
        {
            const CInventory* inv = user->getCurCharacInvenR();
            int jobLevel =
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            float bonus = 100.0f * inv->GetExpertJobSelfDisjointBigWinRate(
                                     (ENUM_EXPERT_JOB_TYPE)3, jobLevel);
            bigwinRate = round_nearest(bigwinRate + bonus);
        }
    }

    if (get_rand_int(10000) < bigwinRate)
    {
        stItemSelectRate select;
        select.m_itemIndex = -1;
        get_additional_item(res.m_bigwinItems, get_rand_int(10000), select, grade);
        if (select.m_itemIndex != -1)
        {
            result.m_f[6] = select.m_itemIndex;
            result.m_f[1] = round_nearest((float)grade / select.m_float8);
            if (result.m_f[1] < 1)
                result.m_f[1] = 1;
        }
    }
    else
    {
        stItemSelectRate select;
        select.m_itemIndex = -1;
        get_additional_item(res.m_additionalItems, get_rand_int(10000), select,
                            grade);
        if (select.m_itemIndex != -1)
        {
            result.m_f[6] = select.m_itemIndex;
            result.m_f[1] = round_nearest((float)grade / select.m_float8);
            if (result.m_f[1] < 1)
                result.m_f[1] = 1;
        }
    }

    if (selfService)
    {
        // script+0x30 [disjoint self service]：{概率, 奖励A, 奖励B}
        // （stSelfDisjoinMaterialInfo m_field0/m_field4/m_field8）。
        int* svc = &GetScript()->m_selfDisjoin.m_field0;
        if (rarity >= 2 && svc[0] > get_rand_int(100))
        {
            result.m_f[2] = svc[2];   // +0x08
            result.m_f[7] = svc[1];   // +0x1c
        }

        bool active = (user != 0 && user->get_state() >= 3 &&
                       user->getCurCharacR() != 0);
        if (active)
        {
            const CInventory* inv = user->getCurCharacInvenR();
            int jobLevel =
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            float variation =
                inv->GetExpertJobSelfDisjointResultVariation(
                    (ENUM_EXPERT_JOB_TYPE)3, jobLevel) / 100.0f;   // _DAT_08cc0814

            int tmp[5];
            for (int i = 0; i <= 4; ++i)
            {
                tmp[i] = round_nearest(result.m_f[i] * variation);
                result.m_f[i] += tmp[i];
            }
        }
    }

    return 0;
}

// getItemSellPrice 定义于 Store.cpp（ORIG T 0x0898c86e）；仓库暂无共享头声明。
int getItemSellPrice(int price, int curEndurance, int maxEndurance,
                     bool isRandomOption);

// ---- CExpertJobMgr::AcquireExpertJob（ORIG T 0x0849e1dc：纯转发工厂）----
CExpertJob* CExpertJobMgr::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type)
{
    return m_factory->AcquireExpertJob(type);
}

// ---- CExpertJobFactory::AcquireExpertJob（ORIG W 0x0849f5d2）：
//      type==1 → Enchanter 池（this+0x00）；type==3 → Disjointer 池（this+0x30）；
//      取到后先虚调用 Reset()（vtable+0xc 槽）再非虚 SetType(type)。 ----
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
CExpertJob* CExpertJobFactory::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type)
{
    CExpertJob* acquired = 0;
    switch (type)
    {
    case (ENUM_EXPERT_JOB_TYPE)1:
        acquired = m_enchanterPool.Acquire();
        break;
    case (ENUM_EXPERT_JOB_TYPE)3:
        acquired = m_disjointerPool.Acquire();
        break;
    }

    if (acquired != 0)
    {
        acquired->Reset();
        acquired->SetType(type);
    }
    return acquired;
}

// ---- CExpertJobMgr::FreeExpertJob（ORIG T 0x0849e1f8：转发工厂）----
void CExpertJobMgr::FreeExpertJob(CExpertJob* job)
{
    m_factory->FreeExpertJob(job);
}

// ---- CExpertJobFactory::FreeExpertJob（ORIG W 0x0849f63e）：
//      job 非空时按 GetType() 分派到对应池。 ----
void CExpertJobFactory::FreeExpertJob(CExpertJob* job)
{
    // b28：正极性包 if（P214①族/探针 /tmp/l3/p4.cpp fs2）——负极性 return
    // 早退使 4.4 尾块前多插 jmp+nop 中继；正包形末 case fall-through leave。
    if (job != 0)
    {
        switch (job->GetType())
        {
        case 1:
            m_enchanterPool.Free((CEnchanter*)job);
            break;
        case 3:
            m_disjointerPool.Free((CDisjointer*)job);
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// CDisjointMachine getter（ORIG W 0x85d3e06 / 0x082347e0 / 0x85d3e12）
// P210：以下 CDisjointer 包装（grade/isMine）必须定义于 get_user/get_grade
// 之前，否则 4.4 继承 callee outgoing 帧塌缩（sub $0x4 vs ORIG $0x18）。
// ---------------------------------------------------------------------------
// get_disjoint_machine_grade（ORIG T 0x85d1f0a：movsbl m_machine.get_grade()）
int CDisjointer::get_disjoint_machine_grade()
{
    return (int)m_machine.get_grade();
}

// isMine（ORIG W 0x08234854）：m_machine.get_user() == user
bool CDisjointer::isMine(CUser* user)
{
    return m_machine.get_user() == user;
}

// get_disjoint_cost（ORIG W 0x085d3e50）
int CDisjointer::get_disjoint_cost() const
{
    return m_machine.get_cost();
}

int CDisjointMachine::get_cost() const { return m_field1c; }       // +0x1c
CUser* CDisjointMachine::get_user() const { return m_user; }       // +0x14
char CDisjointMachine::get_grade() const { return m_field20; }     // +0x20 movzbl

// ---------------------------------------------------------------------------
// CDisjointer —— LinkMisc5 收口（vtable 重排后按虚槽落地）
// ---------------------------------------------------------------------------

// send_error（报告 send_error.md，ORIG T 0x085d1e62）：
// header(0,0xcb)/byte(0)/byte(err)/finalize/Send。
void CDisjointer::send_error(CUser* user, int err)
{
    PacketGuard packet;
    packet.put_header(0, 0xcb);
    packet.put_byte(0);
    packet.put_byte(err);
    packet.finalize(true);
    user->Send(packet);
}

// check_disjoint_endurance（报告 check_disjoint_endurance.md，ORIG T 0x085d1f24）
bool CDisjointer::check_disjoint_endurance()
{
    if (((CCharacExpertJob*)m_machine.get_user()->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance() <= 0)
        return false;
    else
        return true;
}

// IsAvailableJob（ORIG W 0x849f596：恒返 1）
bool CDisjointer::IsAvailableJob() { return true; }

// Reset（报告 Reset.md，ORIG T 0x085d2cf2）
void CDisjointer::Reset() { m_machine.reset(); }

// OnStartMission（报告 OnStartMission.md，ORIG T 0x085d2a16 sz 0x15e）。
// targetUser=分解机主人；requestUser=发起请求者。
void CDisjointer::OnStartMission(CUser* targetUser, CUser* requestUser)
{
    // b28：清 register asm 绑定（N96/P0 全域禁），ORIG 物化岛形为 4.1 CFG 产物，
    // 先按最短语义（&& 短路 + 直接 throw）复测。
    if (targetUser != requestUser &&
        requestUser->getCurCharacMoney() < m_machine.get_cost())
        throw static_cast<ENUM_ERROR>(0x15);

    if (!targetUser->CheckMoney(m_machine.get_cost()))   // ORIG CheckMoney 返 1 为足额
    {
        send_error(targetUser, 10);
        // ORIG vptr+0x4 虚槽 = slot1 OnCloseMission(机主, 0)
        OnCloseMission(m_machine.get_user(), ENUM_CLOSE_MISSION_TYPE_0);
        throw static_cast<ENUM_ERROR>(10);
    }

    if (!check_disjoint_endurance())
    {
        send_error(targetUser, 0xbd);
        throw static_cast<ENUM_ERROR>(0xbd);
    }
}

// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x085d2b74 sz 0x17e）：
// 广播 0xd9 下线包 → 注销村庄对象 → 清专家职业 → 归还池。
void CDisjointer::OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type)
{
    (void)type;
    PacketGuard packet;
    packet.put_header(0, 0xd9);
    packet.put_short(static_cast<int>(user->get_unique_id() & 0xffff));
    packet.finalize(true);

    int area = user->get_area(false);
    // b28d-61 [P200 短整直传实参，ORIG 0x085d30xx `call getCurCharacVill;
    // movsbl %al,%esi`]：旧形具名 char 局部多 `mov %al,-0x9; movsbl -0x9,%ebx`
    // 槽往返；去局部后隐式提升直产 2 操作数 movsbl。
    G_GameWorld()->send_to_area((int)user->getCurCharacVill(), area, packet);

    village_object::Zone zone(user->getCurCharacVill(), user->get_area(false));
    if (!GlobalData::s_villageObjectMgr->unregister_object(zone, &m_machine))
    {
        LogManager::logFormat(
            1, "ProfessionDisjointer.cpp",
            "virtual void expert_job::CDisjointer::OnCloseMission(CUser*, "
            "expert_job::ENUM_CLOSE_MISSION_TYPE)",
            0x15b, "unregister_object error");
    }

    user->SetCurCharacExpertJob(0);
    GlobalData::s_ExpertJobMgr->FreeExpertJob(this);
}

// complete_disjoint（报告 complete_disjoint.md，ORIG T 0x085d31a0 sz 0x42d）。
// 耐久/经验结算 + 请求者付费、机主收款 + 机主耐久不足时经 slot1 关闭任务。
int CDisjointer::complete_disjoint(CUser* requestUser, PacketGuard& packet)
{
    STExpertJobScript* script = GetScript();
    // 耐久消耗 [m_enduranceReduce1(+0x26c), m_enduranceReduce2(+0x270)] 随机
    int enduranceUse =
        get_rand_int(script->m_enduranceReduce2 - script->m_enduranceReduce1 + 1) +
        script->m_enduranceReduce1;

    CCharacExpertJob* ownerExpand = (CCharacExpertJob*)
        ((CUser*)m_machine.get_user())->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0);
    int curEndurance = ownerExpand->get_disjoint_machine_endurance();
    if (curEndurance < enduranceUse)
        enduranceUse = curEndurance;

    ownerExpand = (CCharacExpertJob*)
        ((CUser*)m_machine.get_user())->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0);   // ORIG 二次独立取扩展数据
    ownerExpand->set_disjoint_machine_endurance(curEndurance - enduranceUse);

    // 经验收益 [m_gainExp1(+0x284), m_gainExp2(+0x288)] 随机 + 概率加成项
    script = GetScript();
    int expGain =
        get_rand_int(script->m_gainExp2 - script->m_gainExp1 + 1) +
        script->m_gainExp1;

    // ORIG 0x85d328d 用 getCurCharacInvenR()（const 版）取背包。
    const CInventory* inven = requestUser->getCurCharacInvenR();
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp additionalExp;
    int jobExp = requestUser->GetCurCharacExpertJobExp();
    int jobLevel = requestUser->GetCurExpertJobLevel(jobExp);
    int jobType = requestUser->GetCurCharacExpertJobType();
    inven->GetExpertJobAddtionalExp((ENUM_EXPERT_JOB_TYPE)jobType, jobLevel,
                                    additionalExp);
    if ((float)get_rand_int(100) < additionalExp.m_field0)
        expGain = additionalExp.m_field4 + expGain;

    if (expGain > 0)
    {
        CUser* owner = m_machine.get_user();
        int nextExp = owner->GetCurCharacExpertJobExp() + expGain;
        CExpertJob::IncreaseExpertJobExp(owner, nextExp);
    }

    CUser* owner = m_machine.get_user();
    if (owner == requestUser)
    {
        // 自助拆解：只回填金额与剩余耐久
        packet.put_int(requestUser->getCurCharacMoney());
        ownerExpand = (CCharacExpertJob*)
            ((CUser*)m_machine.get_user())->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0);
        packet.put_int(ownerExpand->get_disjoint_machine_endurance());
    }
    else
    {
        // 他人拆解：请求者扣费 → 回包 → 机主收款 → 机主侧 0xcb 通知
        int cost = get_disjoint_cost();
        CInventory* requesterInven = requestUser->getCurCharacInvenW();
        requesterInven->use_money(cost, (eMoneySubReason)0x1d, true);
        packet.put_int(requestUser->getCurCharacMoney());
        ownerExpand = (CCharacExpertJob*)
            ((CUser*)m_machine.get_user())->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0);
        packet.put_int(ownerExpand->get_disjoint_machine_endurance());

        CInventory* ownerInven =
            ((CUser*)m_machine.get_user())->getCurCharacInvenW();
        ownerInven->gain_money(cost, (eMoneyAddReason)0x16, true, 0);

        PacketGuard notify;
        notify.put_header(0, 0xcb);
        notify.put_byte(1);
        notify.put_int(((CUser*)m_machine.get_user())->getCurCharacMoney());
        ownerExpand = (CCharacExpertJob*)
            ((CUser*)m_machine.get_user())->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0);
        notify.put_int(ownerExpand->get_disjoint_machine_endurance());
        notify.finalize(true);
        ((CUser*)m_machine.get_user())->Send(notify);
    }

    // 机主耐久耗尽 → 经虚槽 slot1 OnCloseMission(机主, 0) 关闭
    ownerExpand = (CCharacExpertJob*)
        ((CUser*)m_machine.get_user())->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0);
    if (ownerExpand->get_disjoint_machine_endurance() < 1)
        OnCloseMission(m_machine.get_user(), ENUM_CLOSE_MISSION_TYPE_0);

    return 1;
}

// get_disjoint_result（ORIG W 0x082347ec）：以机器等级/归属补全六参版本。
int CDisjointer::get_disjoint_result(CItem* item, Inven_Item* inventItem,
                                     DisJointResult& result, CUser* user)
{
    bool mine = isMine(user);
    int grade = get_disjoint_machine_grade();
    return get_disjoint_result_(item, inventItem, grade, result, mine,
                                m_machine.get_user());
}

// ---------------------------------------------------------------------------

// IsAvailableJob（ORIG W 0x849f30c：恒返 1）
bool CEnchanter::IsAvailableJob() { return true; }


// Reset（报告 Reset.md，ORIG T 0x0849c71e sz 0x97）：清材料×2 →
// 清 +0x28..+0x38 五个 dword → SetType(1) → 清 +0x3c/+0x44/+0x40。
void CEnchanter::Reset()
{
    for (int i = 0; i < 2; ++i)
        m_materials[i].Reset();
    m_user = 0;
    m_peer = 0;
    m_uid = 0;
    m_peerUid = 0;
    m_field38 = 0;
    SetType((ENUM_EXPERT_JOB_TYPE)1);
    m_field3c = 0;
    m_peerRequestType = ENUM_PEER_REQUEST_TYPE_0;   // ORIG 写序：+0x44 先于 +0x40
    m_field40 = 0;
}

// OnStartMission（报告 OnStartMission.md，ORIG T 0x0849c7b6）：
// 双方登记到附魔师槽位并各自 SetCurCharacExpertJob(this)。
void CEnchanter::OnStartMission(CUser* targetUser, CUser* requestUser)
{
    m_user = targetUser;
    m_uid = (unsigned int)(targetUser->get_unique_id() & 0xffff);
    // P48：ORIG 在写 m_user(+0x28) 后 `mov 0x28(%eax),%eax` 回读成员再调
    // SetCurCharacExpertJob（0x0849c7df/0x0849c811），不复用形参。改经成员。
    m_user->SetCurCharacExpertJob(this);
    m_peer = requestUser;
    m_peerUid = (unsigned int)(requestUser->get_unique_id() & 0xffff);
    m_peer->SetCurCharacExpertJob(this);
}

// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x0849c822 sz 0x14c）：
// 按 uid 匹配离场方；type∈{1,2} 时向另一方发空 0x9e 包；最后归还池。
void CEnchanter::OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type)
{
    CUser* peer = 0;
    if ((unsigned int)(user->get_unique_id() & 0xffff) == m_uid)
    {
        m_user->SetCurCharacExpertJob(0);
        m_peer->SetCurCharacExpertJob(0);
        peer = m_peer;
    }
    else
    {
        // b28d-61 [P222 正向包裹翻极性，ORIG 0x0849c87x `cmp %eax,%edx; sete
        // %al; test %al,%al; je +0x187`]：旧「否定早返」形产 setne/jne 反极性。
        if ((unsigned int)(user->get_unique_id() & 0xffff) == m_peerUid)
        {
            m_user->SetCurCharacExpertJob(0);
            m_peer->SetCurCharacExpertJob(0);
            peer = m_user;
        }
        else
            return;
    }

    if (type == ENUM_CLOSE_MISSION_TYPE_1 || type == ENUM_CLOSE_MISSION_TYPE_2)
    {
        // b28d-61 [P214① 正条件内联，ORIG 0x0849c9xx 无具名 bool 槽]
        // 旧 `bool sendPeer = A && B && C; if (sendPeer)` 多 `mov %al,-0x9;
        // cmpb $0,-0xa` 槽物化。
        if (peer != 0 && peer->getCurCharacR() != 0 && m_user != m_peer)
        {
            PacketGuard packet;
            packet.put_header(0, 0x9e);
            packet.finalize(true);
            peer->Send(packet);
        }
    }
    GlobalData::s_ExpertJobMgr->FreeExpertJob(this);
}

// ---------------------------------------------------------------------------
// GetPredictResult（报告 GetPredictResult.md，ORIG T 0x085d1f68 sz 0x3e2）。
// 函数级静态结果向量（guard 变量 b 0x09472f80 / 向量 b 0x09472f88）：
//   [0] 机主本人且 rarity>1 → script+0x34（selfService[1]，奖励 A 物品）
//   [1] map 命中项的 stDisjointResult.m_field0（cube 索引）
//   [2..] 附加/大奖物品表中未收录过的 m_itemIndex（std::find 去重）
// ---------------------------------------------------------------------------
std::vector<int>* CDisjointer::GetPredictResult(CUser* user, CItem* item,
                                                Inven_Item* inventItem)
{
    static std::vector<int> result;   // ORIG GetPredictResult(...)::result
    result.clear();

    bool unidentified = false;
    if (inventItem != 0 && !inventItem->m_amp.isIdentified())
        unidentified = true;

    if (isMine(user) && item->get_rarity() > 1)
        result.push_back(GetScript()->m_selfDisjoin.m_field4);

    DisjointMachineGrade key((unsigned char)item->get_rarity(),
                             (unsigned char)get_disjoint_machine_grade(),
                             unidentified);
    STExpertJobScript* script = GetScript();
    std::map<DisjointMachineGrade, stDisjointResult>::iterator it =
        script->m_disjointResultMap.find(key);
    if (it != script->m_disjointResultMap.end())
    {
        stDisjointResult& res = it->second;
        result.push_back(res.m_field0);

        for (std::vector<stItemSelectRate>::const_iterator rate =
                 res.m_additionalItems.begin();
             rate != res.m_additionalItems.end(); ++rate)
        {
            if (std::find(result.begin(), result.end(), rate->m_itemIndex) ==
                result.end())
                result.push_back(rate->m_itemIndex);
        }
        for (std::vector<stItemSelectRate>::const_iterator rate =
                 res.m_bigwinItems.begin();
             rate != res.m_bigwinItems.end(); ++rate)
        {
            if (std::find(result.begin(), result.end(), rate->m_itemIndex) ==
                result.end())
                result.push_back(rate->m_itemIndex);
        }
    }
    return &result;
}

}  // namespace expert_job

// ===========================================================================
// 分解链全局校验函数（LinkMisc5 收口；调用方 DispDebugJoint4.cpp 以 mangled
// 直连，符号与 ORIG 一致）
// ===========================================================================

#include "Arad_DataManager.h"      // ARAD::Singleton<T>
#include "ServiceRestrictManager.h"

// GetPredictDisjointItems（报告 _lt_global_gt_/GetPredictDisjointItems.md，
// ORIG T 0x081f8f8d sret）：无专家职业走 DisJoint 表静态结果向量，
// 否则走分解机实例预测。
std::vector<int>* GetPredictDisjointItems(CUser* user, CItem* item,
                                          Inven_Item& invenItem,
                                          expert_job::CDisjointer* disjointer)
{
    if (disjointer != 0)
        return disjointer->GetPredictResult(user, item, &invenItem);
    else
    {
        return G_CDataManager()->m_disJoint->GetPredictResult(item->get_rarity());
    }
}

// ---- DisJoint::getPredictResultItemTypeCount（ORIG T 0x08473148，
//      docs/class_func_reports/DisJoint/getPredictResultItemTypeCount.md）：
//      rarity ∉ [0,5] → 1；否则读 m_additional[rarity].m_count
//      （this+0x5c8+rarity*0x10，P8：Ghidra 的 (param+0x5c)*0x10+8 即 entry
//      起点 m_count，非 +8 字段），非 0 → 2，否则 → 1。 ----
int DisJoint::getPredictResultItemTypeCount(int rarity) const
{
    if (rarity < 0 || rarity > 5)
        return 1;
    if (m_additional[rarity].m_count != 0)
        return 2;
    return 1;
}

// ---- DisJoint::GetPredictResult（报告 GetPredictResult.md，ORIG T 0x08473116）：
//      rarity ∈ [0,5] → &m_resultVecs[rarity]（this+0x628+rarity*0xc）；
//      越界 → &m_resultVecs[0]（this+0x628）。rarity 按有符号比较（js/jle）。 ----
std::vector<int>* DisJoint::GetPredictResult(int rarity)
{
    if (rarity < 0 || rarity > 5)
        return &m_resultVecs[0];
    return &m_resultVecs[rarity];
}


// ORIG W 0x0822b1f4（声明补入 STExpertJobScript.h；定义置本 TU 保持
// expert_job 域内聚）。m_portableDisjointMap(+0x18) find(level)，未命中 -1。
int STExpertJobScript::GetPortableDisjointPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it = m_portableDisjointMap.find(level);
    if (it == m_portableDisjointMap.end())
        return -1;
    // [b28c-L18] ORIG 尾双段 `movzwl 0x4(%eax),%eax; movzwl %ax,%eax` = int 返回
    // + (unsigned short) 提升（探针 dj.cpp f_d 全等）。
    return (unsigned short)it->second;
}

// ---- DisPatcher_DisJointItem::_checkDisjointableItem
// （报告 _checkDisjointableItem.md，ORIG T 0x081fa0f6）：
// 未鉴定且无目标用户 → 0xae；物品缺失 → 0x11；
// IsImpossibleDisjoint（ORIG vptr+0x38 = vtable 槽16）→ 0x13；GetAttachType==2 → 0x11。 ----
// [b28c-31] N7+N18b：虚调用 callee ORIG %edx vs 4.4 %eax（vptr+0x38 槽）；
// 另 GetAttachType 结果 ORIG 回读槽再 `cmp $0x2,%eax`，4.4 恒折 `cmpl $0x2,-0xc`
// 内存直比（P209 附 / N18b）。
int DisPatcher_DisJointItem::_checkDisjointableItem(const Inven_Item& item,
                                                    CUser* targetUser)
{
    // ORIG 0x081fa0fd..0x081fa12d：item.m_amp(+0x11).isIdentified()==false
    // 且 targetUser==0 → 0xae（未鉴定且无目标用户不可分解）
    if (!item.m_amp.isIdentified() && targetUser == 0)
        return 0xae;

    CItem* itemObj = G_CDataManager()->find_item(item.m_addInfo);
    if (itemObj == 0)
        return 0x11;

    if (itemObj->IsImpossibleDisjoint())   // ORIG vptr+0x38（IsImpossibleDisjoint，非 CharacCargo）
        return 0x13;
    // b28d-61 [P286 单 case switch 复现寄存器比较，ORIG 0x081fa183
    // `mov -0xc(%ebp),%eax; cmp $0x2,%eax; jne`]：if 形（int/unsigned/const/cast
    // 六形探针 /tmp/b61_probe_att{,2}.cpp）4.4 恒折 `cmpl $0x2,-0xc` 内存直比；
    // 单 case switch 得寄存器链。
    int attachType = itemObj->GetAttachType();
    switch (attachType)
    {
    case 2:
        return 0x11;
    }
    return 0;
}

// ---- _check_disjoint_need_slot（报告 _check_disjoint_need_slot.md，
//      ORIG T 0x081f8fe3 sz 0x2e7）：预测产物按背包类型分桶统计空位需求，
//      缺口超出第 0 类空槽数则抛 ENUM_ERROR 4。 ----
void _check_disjoint_need_slot(CUser* user, CItem* item, Inven_Item& invenItem,
                               expert_job::CDisjointer* disjointer)
{
    const int kInvalidSlot = Inven_Item::ITEM_TYPE_INVALID;   // 0xb

    int perType[11];
    int total[11];
    int emptySlots[11];
    for (int i = 0; i < 11; ++i)
    {
        perType[i] = 0;
        total[i] = 0;
        emptySlots[i] = 0;
    }

    std::vector<int>* predicted =
        GetPredictDisjointItems(user, item, invenItem, disjointer);

    for (unsigned int i = 0; i < predicted->size(); ++i)
    {
        int itemIndex = (*predicted)[i];
        if (itemIndex == 0)
            continue;

        CItem* templateItem = G_CDataManager()->find_item(itemIndex);
        if (templateItem == 0)
            continue;

        Inven_Item probe;                       // ORIG 局部 ctor 清零
        probe.m_addInfo = itemIndex;            // 直接写 +0x02
        probe.set_add_info(1);                  // ORIG：m_addInfo2 = 1（set_add_info 语义）
        templateItem->make_item(probe);         // ORIG 0x81f90f3..0x81f9108：vptr+8
                                                // 虚槽 make_item 填充 type 等（CItem.h vtable 槽 2）
        unsigned char probeType = probe.m_field1;   // make_item 填充的 type 字节
        int needSlot = kInvalidSlot;
        CInventory* inven =
            const_cast<CInventory*>(user->getCurCharacInvenR());
        if (inven->IsExistStackable(
                itemIndex, (Inven_Item::ITEM_TYPE)probeType))
        {
            needSlot = (int)probeType;
        }
        else if (inven->IsExistStackable(
                     itemIndex, (Inven_Item::ITEM_TYPE)0))
        {
            needSlot = 0;
        }

        if (needSlot == kInvalidSlot)
        {
            total[probeType] += 1;
        }
        else
        {
            perType[needSlot] += 1;
            total[needSlot] += 1;
        }
    }

    for (int i = 0; i < 11; ++i)
        emptySlots[i] = user->getCurCharacInvenR()->InvenEmptycount(
            (Inven_Item::ITEM_TYPE)i);

    int deficit = 0;
    for (int i = 0; i < 11; ++i)
    {
        if (emptySlots[i] + perType[i] < total[i])
            deficit += total[i] - (emptySlots[i] + perType[i]);
    }

    if (deficit > emptySlots[0])
        throw static_cast<ENUM_ERROR>(4);
}

// ---- _check_disjointable_item_state（报告 _check_disjointable_item_state.md，
//      ORIG T 0x081f8ba1 sz 0x3ec）：取源物品快照、拆解券等级门槛与
//      DisPatcher 静态校验，任一失败抛对应 ENUM_ERROR。 ----
void _check_disjointable_item_state(CUser* user, int slot, int invenType,
                                    Inven_Item& item, CItem** pItem,
                                    CUser* targetUser, unsigned short region)
{
    // 快照：GetInvenSlot(invenType, slot) 全量拷贝（0x3d 字节 sret）
    item = user->getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        throw static_cast<ENUM_ERROR>(0x11);   // 宠物装备不可拆

    if (!item.isEquipableItemType() || item.m_addInfo == 0)
        throw static_cast<ENUM_ERROR>(0x11);

    if (region != 0xffff)
    {
        Inven_Item ticket =
            user->getCurCharacInvenR()->GetInvenSlot(1, (int)region);
        CStackableItem* ticketObj =
            (CStackableItem*)G_CDataManager()->find_item(ticket.m_addInfo);
        if (ticketObj == 0)
            throw static_cast<ENUM_ERROR>(0x15);

        int ticketLevel = ticketObj->m_int324;   // +0x324 使用等级门槛
        if (ticketLevel == -1)
            throw static_cast<ENUM_ERROR>(1);   // ORIG：门槛 -1 直接抛 1

        // ORIG +0x21c：经 expert_job::CDisjointer::GetScript()（缓存单例）取脚本，
        // 非裸 G_CDataManager()->GetExpertJobScript(3)。
        int portableLimit = expert_job::CDisjointer::GetScript()
                                ->GetPortableDisjointPossibleLevel(ticketLevel);
        // ORIG +0x234：portableLimit==-1 亦抛 1（原实现漏该判据）。
        if (portableLimit == -1)
            throw static_cast<ENUM_ERROR>(1);

        CItem* itemObj = G_CDataManager()->find_item(item.m_addInfo);
        if (itemObj == 0)
            throw static_cast<ENUM_ERROR>(0x11);

        int usableLevel = itemObj->getUsableLevel();
        if (usableLevel == 0)
        {
            if (portableLimit < itemObj->get_grade())
                throw static_cast<ENUM_ERROR>(0xd);
        }
        else if (portableLimit < usableLevel)
        {
            throw static_cast<ENUM_ERROR>(0xd);
        }
    }

    int err = DisPatcher_DisJointItem::_checkDisjointableItem(item, targetUser);
    if (err != 0)
        throw static_cast<ENUM_ERROR>(err);

    *pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (*pItem == 0)
        throw static_cast<ENUM_ERROR>(0x11);
}

// ---- _check_disjointable_user_state（报告 _check_disjointable_user_state.md，
//      ORIG T 0x081f8a7c sz 0x125）：状态门 + 服务限制 + 保护字段三查。 ----
bool _check_disjointable_user_state(CUser* user, ENUM_CMDPACKET cmd,
                                    char spaceChar, short slotShort)
{
    if (user->get_state() < 3)
        return false;

    ServiceRestrictManager* restrict =
        ARAD::Singleton<ServiceRestrictManager>::Get();
    if (!restrict->isRestricted(user, (RestrictCategory::Enum)1, 10))
    {
        CSecu_ProtectionField* secu = GlobalData::s_pSecuProtectionField;
        Inven_Item* invenRef =
            secu->GetInvenItemPtr(user, spaceChar, slotShort);
        CItem* itemPtr = secu->GetItemPtr(user, spaceChar, slotShort);
        unsigned int err =
            secu->CheckItemRarity(user, (SECURITY_PROTCTION)0x28, itemPtr,
                                  invenRef);
        if (err == 0)
            return true;
        user->SendCmdErrorPacket(cmd, (unsigned char)(err & 0xff));
        return false;
    }

    user->SendCmdErrorPacket(cmd, 0xd1);
    return false;
}
