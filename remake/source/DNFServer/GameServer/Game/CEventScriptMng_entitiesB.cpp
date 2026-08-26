// df_game_r Game/ EventClassify 第二组实体实现（group 2，FixEventEntitiesB）。
// 覆盖 ctor/dtor/脚本导入路径方法（set_event_data / get_db_table_name）。
// 每个实体在 CEventEntity（sizeof 0x1c）之上追加绑定脚本指针 m_script1c(+0x1c)，
// sizeof = 0x20。get_db_table_name 为虚方法（ORIG 虚表 slot 8 / +0x20）。
#include "CEventScriptMng.h"

#include "PacketGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"           // getCurCharacNo / getCurCharacUsedFatigue
#include "GameTypes.h"                 // cMyTrace
#include "LogManager.h"
// 事件发奖动作域（process_action_send_mail 定义 EventClassify_Action.cpp）
#include "EventClassify_Action.h"
#include "DB_UpdatePurchaseCashItemBonus.h"
#include "DB_InsertAccountFirstLoginDailyFlag.h"

// process_action_send_mail 形参用的是 EventClassify_Action.h 的命名空间内
// Action_SendMail；脚本子对象 m_mail 为全局 ::Action_SendMail（ORIG 符号口径，
// 见 InGameEventScripts.h）。两者逐字段同布局（sizeof 均 0x44）。用宏桥接：
// -O0 下 inline 函数会产生多余调用帧（ORIG 无）。同 CEventScriptMng_entities.cpp。
typedef char dnf_mail_layout_assert_b_[
    (sizeof(::Action_SendMail) == sizeof(Action_SendMail)) ? 1 : -1];
#define EVENT_MAIL_REF(mail) (*reinterpret_cast<Action_SendMail*>(&(mail)))

namespace EventClassify
{

// ============ CFatigueQuantity ============
// ORIG ctor 0x08111030 W / D1 0x08114aac / D0 0x08114adc / sizeof 0x20。
CFatigueQuantity::CFatigueQuantity()
    : CEventEntity(), m_script1c(0)
{
}

CFatigueQuantity::~CFatigueQuantity()
{
}

// ORIG 0x0810f95e：虚调度 set_event_record_type（vtable+0x18）写 m_recordType=key，
// 然后把绑定脚本存到 +0x1c。
void CFatigueQuantity::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

char* CFatigueQuantity::get_db_table_name()
{
    // ORIG 0x0810f988：return &m_dbTableName（FatigueQuantityScript +0x08）。
    return reinterpret_cast<char*>(
        &static_cast<FatigueQuantityScript*>(m_script1c)->m_dbTableName);
}

// ORIG W 0x08110edc：返回绑定脚本 +0x0c 的 [user type] 字节。
char CFatigueQuantity::get_user_type() const
{
    return static_cast<char>(
        static_cast<FatigueQuantityScript*>(m_script1c)->m_userType);
}

// ============ CExchangeRandomItemReward ============
// ORIG ctor 0x08110ff8 W / D1 0x08114b48 / D0 0x08114b78 / sizeof 0x20。
CExchangeRandomItemReward::CExchangeRandomItemReward()
    : CEventEntity(), m_script1c(0)
{
}

CExchangeRandomItemReward::~CExchangeRandomItemReward()
{
}

// ORIG 0x0810eb06：同 CFatigueQuantity 模式。
void CExchangeRandomItemReward::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ============ CAvatarDisjointRandomReward ============
// ORIG ctor 0x08111014 W / D1 0x08114afa / D0 0x08114b2a / sizeof 0x20。
CAvatarDisjointRandomReward::CAvatarDisjointRandomReward()
    : CEventEntity(), m_script1c(0)
{
}

CAvatarDisjointRandomReward::~CAvatarDisjointRandomReward()
{
}

// ORIG 0x0810f37a：同 CFatigueQuantity 模式。
void CAvatarDisjointRandomReward::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ============ CObjectBringUp ============
// ORIG ctor 0x08111068 W / D1 0x08114a10 / D0 0x08114a40 / sizeof 0x20。
CObjectBringUp::CObjectBringUp()
    : CEventEntity(), m_script1c(0)
{
}

CObjectBringUp::~CObjectBringUp()
{
}

// ORIG 0x0810fcb2：同 CFatigueQuantity 模式。
void CObjectBringUp::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

char* CObjectBringUp::get_db_table_name()
{
    // ORIG 0x0810fcdc：return &m_dbTableName（ObjectBringUpScript +0x08）。
    return reinterpret_cast<char*>(
        &static_cast<ObjectBringUpScript*>(m_script1c)->m_dbTableName);
}

// ORIG 0x08110386：按已用疲劳档位发放事件物品邮件。
// 遍历 m_ruleMails1c（[eventitem send] 规则-邮件对），内层阈值列表任一 t 满足
// 「(当前已用疲劳 - 增量) < t 且 当前已用疲劳 >= t」即发该对邮件；
// 发信成功读 m_mail.m_itemIdx 记日志后返回（ORIG 单次发放语义）。
// 全程不缓存脚本指针（与 ORIG 同）。
void CObjectBringUp::sendEventItemFromFatigue(CUser* user, unsigned int fatigue)
{
    if (static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.empty())
        return;
    unsigned int usedBase =
        (unsigned int)user->getCurCharacUsedFatigue() & 0xffffu;
    unsigned int remaining = usedBase - fatigue;   // ORIG local_14（32 位回绕忠实）
    CEventActionMng mng;
    for (std::vector<std::pair<std::vector<unsigned int>, ::Action_SendMail> >::iterator it =
             static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.begin();
         it != static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.end(); ++it)
    {
        for (std::vector<unsigned int>::iterator vit = (*it).first.begin();
             vit != (*it).first.end(); ++vit)
        {
            unsigned int threshold = *vit;
            bool hit = (remaining < threshold) &&
                       (((unsigned int)user->getCurCharacUsedFatigue() & 0xffffu)
                            >= threshold);
            if (hit)
            {
                int sent = mng.process_action_send_mail(
                    user, EVENT_MAIL_REF((*it).second), true);
                if (sent != 0)
                {
                    LogManager::logFormat(1, "EventClassify.cpp",
                        "void EventClassify::CObjectBringUp::sendEventItemFromFatigue(CUser*, unsigned int)",
                        0x803,
                        "CObjectBringUp::sendEventItemFromFatigue(m_id:%d, charac_no:%d, reward:%d)",
                        user->get_acc_id(), user->getCurCharacNo(),
                        (*it).second.m_itemIdx);
                    return;
                }
            }
        }
    }
}

// ============ CPurchaseCashItemBonus ============
// ORIG ctor 0x0811104c W / D1 0x08114a5e / D0 0x08114a8e / sizeof 0x20。
CPurchaseCashItemBonus::CPurchaseCashItemBonus()
    : CEventEntity(), m_script1c(0)
{
}

CPurchaseCashItemBonus::~CPurchaseCashItemBonus()
{
}

// ORIG 0x0810f996：同 CFatigueQuantity 模式。
void CPurchaseCashItemBonus::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ORIG 0x0810f9c0：return &m_dbTableName（PurchaseCashItemBonusScript +0x08）。
char* CPurchaseCashItemBonus::get_db_table_name()
{
    return reinterpret_cast<char*>(
        &static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_dbTableName);
}

// ORIG 0x0810f9ce：目标商品购买计数更新。
// user 为空走 cMyTrace 告警；否则取 acc_id，当 IPG 请求条目 m_itemIdx
// 命中脚本 m_set0c（[item idx start]/[item idx end] 区间导入的目标商品集合）
// 时向 DB 队列发 DB_UpdatePurchaseCashItemBonus（m_id, acc_id）请求，
// 并以 cMyTrace 记 "updateTargetGoodsCount(%d)"（acc_id）。
void CPurchaseCashItemBonus::updateTargetGoodsCount(
    CUser* user, unsigned int characNo, stCeraShopIPGRequest_t* request)
{
    if (user == 0)
    {
        cMyTrace trace(
            "void EventClassify::CPurchaseCashItemBonus::updateTargetGoodsCount(CUser*, unsigned int, stCeraShopIPGRequest_t*)",
            0x737, 0);
        trace("Purchase Error Not Found User(charac_no:%d)", characNo);
        return;
    }
    int accId = user->get_acc_id();
    if (static_cast<PurchaseCashItemBonusScript*>(m_script1c)
            ->m_set0c.find(request->m_itemIdx) !=
        static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_set0c.end())
    {
        DB_UpdatePurchaseCashItemBonus::makeRequest(user->GetUID(), accId);
        cMyTrace trace(
            "void EventClassify::CPurchaseCashItemBonus::updateTargetGoodsCount(CUser*, unsigned int, stCeraShopIPGRequest_t*)",
            0x742, 0);
        trace("CPurchaseCashItemBonus::updateTargetGoodsCount(%d)", accId);
    }
}

// ORIG 0x0810fae0：档位规则校验（count=本次比较基准购买数）。
// count < start 不发；end 非 0 且 count > end 不发；(count-start)%step!=0 不发。
bool CPurchaseCashItemBonus::verifyGiveBonusReward(
    PurchaseCashItemBonusScript::PurchaseBonusRule& rule, int count)
{
    if (count < rule.m_purchaseStartCount)
        return false;
    if (rule.m_purchaseEndCount != 0 && rule.m_purchaseEndCount < count)
        return false;
    if ((count - rule.m_purchaseStartCount) % rule.m_purchaseMutipleNumber != 0)
        return false;
    return true;
}

// ORIG 0x0810fb34：按购买数遍历 m_ruleMails24 规则-邮件对发奖励邮件。
// user 为空仅记 LogManager 告警返回 false；命中规则且发信成功读
// mail.m_itemIdx 记日志并将返回置 false（ORIG 返回值语义：true=本轮未发放）。
bool CPurchaseCashItemBonus::sendPurchaseBonusReward(CUser* user, int count)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "EventClassify.cpp",
            "bool EventClassify::CPurchaseCashItemBonus::sendPurchaseBonusReward(CUser*, int)",
            0x755,
            "CPurchaseCashItemBonus::sendPurchaseBonusReward(pUser(NULL))");
        return false;
    }
    bool sent = true;                       // ORIG local_d
    CEventActionMng mng;                    // ORIG 循环外构造一次
    std::vector<std::pair<PurchaseCashItemBonusScript::PurchaseBonusRule,
                          ::Action_SendMail> >& rules =
        static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_ruleMails24;
    for (std::vector<std::pair<PurchaseCashItemBonusScript::PurchaseBonusRule,
                               ::Action_SendMail> >::iterator it = rules.begin();
         it != rules.end(); ++it)
    {
        if (verifyGiveBonusReward((*it).first, count))
        {
            int result = mng.process_action_send_mail(
                user, EVENT_MAIL_REF((*it).second), true);
            if (result != 0)
            {
                LogManager::logFormat(1, "EventClassify.cpp",
                    "bool EventClassify::CPurchaseCashItemBonus::sendPurchaseBonusReward(CUser*, int)",
                    0x763,
                    "CPurchaseCashItemBonus::sendPurchaseBonusReward(m_id:%d, charac_no:%d, reward:%d)",
                    user->get_acc_id(), user->getCurCharacNo(),
                    (*it).second.m_itemIdx);
                sent = false;
            }
        }
    }
    return sent;
}

// ============ CAccountFirstLoginDaily ============
// 派生自 CAccountFirstLogin（group1，FixEventEntitiesA）。
// ORIG ctor 0x0811110a W / D1 0x08114c80 / D0 0x08114cb0。
// 不新增成员，+0x1c 继承自 CAccountFirstLogin。
// ORIG T 0x810dfc6：直接委托基类（CAccountFirstLogin::set_event_data）。
void CAccountFirstLoginDaily::set_event_data(int key, InGameEventScript* script)
{
    CAccountFirstLogin::set_event_data(key, script);
}

CAccountFirstLoginDaily::CAccountFirstLoginDaily()
    : CAccountFirstLogin()
{
}

CAccountFirstLoginDaily::~CAccountFirstLoginDaily()
{
}

// ORIG T 0x0810dfe8：发脚本邮件（AccountFirstLoginScript::m_mail @ +0xc）后
// 向 DB 插入账号每日首次登录标记（GetUID, acc_id, charac_no, flags）。
void CAccountFirstLoginDaily::do_action(int flags, CUser* user,
                                        std::bitset<32u>)
{
    if (user == 0)
        return;

    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(
            static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail),
        true);
    unsigned int characNo = user->getCurCharacNo();
    DB_InsertAccountFirstLoginDailyFlag::makeRequest(
        user->GetUID(), user->get_acc_id(), characNo, flags);
}

}  // namespace EventClassify
