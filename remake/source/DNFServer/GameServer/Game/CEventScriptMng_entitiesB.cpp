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
CFatigueQuantity::~CFatigueQuantity()
{
}

// ORIG 0x0810f95e：虚调度 set_event_record_type（vtable+0x18）写 m_recordType=key，
// 然后把绑定脚本存到 +0x1c。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：P211 档③——m_script1c
//   具型化（FatigueQuantityScript*）+ 赋值点 static_cast 异型化 ⇒ store 复现 ORIG
//   RHS-first（0x810f97d 三条逐条一致）；4.4.6 后端同时消 N7（vfn %ecx / this %eax
//   与 ORIG 同形）。4.4.7 下该件仍 NEAR（纯 N7 后端差：vfn %eax / this %edx）；
//   实测 TU identical 30→35 / ae 42 / near 5→0 / diff 1（N123 撞名假 DIFF）零回归。
//   路由已落 dnf_game_cxx.sh:370（b28d-S188）（ORIG 0x0810f95e sz 0x0000002a）
void CFatigueQuantity::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810f95e：call vtable+0x18（set_event_record_type(key)），再写 +0x1c。
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<FatigueQuantityScript*>(script);
}

std::string& CFatigueQuantity::get_db_table_name()
{
    // ORIG 0x0810f988：return &m_dbTableName（FatigueQuantityScript +0x08）。
    return static_cast<FatigueQuantityScript*>(m_script1c)->m_dbTableName;
}

// ORIG W 0x08110edc：返回绑定脚本 +0x0c 的 [user type] 字节。
char CFatigueQuantity::get_user_type() const
{
    return static_cast<char>(
        static_cast<FatigueQuantityScript*>(m_script1c)->m_userType);
}

// ============ CExchangeRandomItemReward ============
// ORIG ctor 0x08110ff8 W / D1 0x08114b48 / D0 0x08114b78 / sizeof 0x20。
CExchangeRandomItemReward::~CExchangeRandomItemReward()
{
}

// ORIG 0x0810eb06：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （ExchangeRandomItemRewardScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下
//   仍 NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810eb06 sz 0x0000002a）
void CExchangeRandomItemReward::set_event_data(int key, InGameEventScript* script)
{
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<ExchangeRandomItemRewardScript*>(script);
}

// ============ CAvatarDisjointRandomReward ============
// ORIG ctor 0x08111014 W / D1 0x08114afa / D0 0x08114b2a / sizeof 0x20。
CAvatarDisjointRandomReward::~CAvatarDisjointRandomReward()
{
}

// ORIG 0x0810f37a：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （AvatarDisjointRandomRewardScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下
//   仍 NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810f37a sz 0x0000002a）
void CAvatarDisjointRandomReward::set_event_data(int key, InGameEventScript* script)
{
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<AvatarDisjointRandomRewardScript*>(script);
}

// ============ CObjectBringUp ============
// ORIG ctor 0x08111068 W / D1 0x08114a10 / D0 0x08114a40 / sizeof 0x20。
CObjectBringUp::~CObjectBringUp()
{
}

// ORIG 0x0810fcb2：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化（ObjectBringUpScript*）⇒
//   store RHS-first；4.4.6 消 N7。4.4.7 下仍 NEAR（纯 N7 后端差）。路由已落
//   dnf_game_cxx.sh:370（ORIG 0x0810fcb2 sz 0x0000002a）
void CObjectBringUp::set_event_data(int key, InGameEventScript* script)
{
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<ObjectBringUpScript*>(script);
}

std::string& CObjectBringUp::get_db_table_name()
{
    // ORIG 0x0810fcdc：return &m_dbTableName（ObjectBringUpScript +0x08）。
    return static_cast<ObjectBringUpScript*>(m_script1c)->m_dbTableName;
}

// ORIG 0x08110386：按已用疲劳档位发放事件物品邮件。
// 遍历 m_ruleMails1c（[eventitem send] 规则-邮件对），内层阈值列表任一 t 满足
// 「(当前已用疲劳 - 增量) < t 且 当前已用疲劳 >= t」即发该对邮件；
// 发信成功读 m_mail.m_itemIdx 记日志后返回（ORIG 单次发放语义）。
// 全程不缓存脚本指针（与 ORIG 同）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，138=138）。修法：
// P0 去 usedBase/threshold/hit 具名局部（ORIG 无槽往返）⇒ 帧 0x50→0x40；
// CEventActionMng mng 移入内层循环作用域复现 ORIG 槽布局（mng -0x21/it -0x1c/
// vit -0x20）；第二条件改写 `*vit <= used` 保左→右求值序（*vit→%ebx 先于
// getCurCharacUsedFatigue→%eax）；外层 it->/ptEv 替 (*it)./deEv（P167）。
void CObjectBringUp::sendEventItemFromFatigue(CUser* user, unsigned int fatigue)
{
    if (static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.empty())
        return;
    // P0：ORIG 无 usedBase 槽往返（movzwl; sub 0x10(%ebp),%eax; mov %eax,-0x10）
    // ⇒ 原码是内联右值，remaining 为唯一具名局部（32 位回绕忠实）。
    unsigned int remaining =
        ((unsigned int)user->getCurCharacUsedFatigue() & 0xffffu) - fatigue;
    for (std::vector<std::pair<std::vector<unsigned int>, ::Action_SendMail> >::iterator it =
             static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.begin();
         it != static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.end(); ++it)
    {
        // ORIG 外层访问走 operator->（ptEv），内层走 operator*（deEv）；
        // threshold 无具名槽（每次重发 *vit，P157/P183）。
        for (std::vector<unsigned int>::iterator vit = it->first.begin();
             vit != it->first.end(); ++vit)
        {
            CEventActionMng mng;
            // ORIG 第二条件先求 *vit（→%ebx）后求 getCurCharacUsedFatigue（→%eax）
            // ⇒ 写成 `*vit <= used` 保持左→右求值序（等价无符号比较）。
            if ((*vit > remaining) &&
                (*vit <= ((unsigned int)user->getCurCharacUsedFatigue()
                          & 0xffffu)))
            {
                int sent = mng.process_action_send_mail(
                    user, EVENT_MAIL_REF(it->second), true);
                if (sent != 0)
                {
                    LogManager::logFormat(1, "EventClassify.cpp",
                        __PRETTY_FUNCTION__,
                        0x803,
                        "CObjectBringUp::sendEventItemFromFatigue(m_id:%d, charac_no:%d, reward:%d)",
                        user->get_acc_id(), user->getCurCharacNo(),
                        it->second.m_itemIdx);
                    return;
                }
            }
        }
    }
}

// ============ CPurchaseCashItemBonus ============
// ORIG ctor 0x0811104c W / D1 0x08114a5e / D0 0x08114a8e / sizeof 0x20。
CPurchaseCashItemBonus::~CPurchaseCashItemBonus()
{
}

// ORIG 0x0810f996：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （PurchaseCashItemBonusScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下仍
//   NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810f996 sz 0x0000002a）
void CPurchaseCashItemBonus::set_event_data(int key, InGameEventScript* script)
{
    // [b28d-solo-CEventScriptMng_entitiesB] P211 档③：字段具型化 ⇒ store RHS-first。
    set_event_record_type(key);
    m_script1c = static_cast<PurchaseCashItemBonusScript*>(script);
}

// ORIG 0x0810f9c0：return &m_dbTableName（PurchaseCashItemBonusScript +0x08）。
std::string& CPurchaseCashItemBonus::get_db_table_name()
{
    return static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_dbTableName;
}

// ORIG 0x0810f9ce：目标商品购买计数更新。
// user 为空走 cMyTrace 告警；否则取 acc_id，当 IPG 请求条目 m_itemIdx
// 命中脚本 m_set0c（[item idx start]/[item idx end] 区间导入的目标商品集合）
// 时向 DB 队列发 DB_UpdatePurchaseCashItemBonus（m_id, acc_id）请求，
// 并以 cMyTrace 记 "updateTargetGoodsCount(%d)"（acc_id）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，71=71，帧 0x48 无
// callee-saved）。修法：P293 具名迭代器固定 find→end 求值序（ORIG sret -0x38/-0x20；
// 4.4 单表达式 `find()!=end()` 恒先 end 后 find）；accId 声明 unsigned 消 %ebx
// 跨 GetUID 保活（P290 反向）；两处 cMyTrace 改全表达式临时链复现 ORIG 槽深（P221）。
void CPurchaseCashItemBonus::updateTargetGoodsCount(
    CUser* user, unsigned int characNo, stCeraShopIPGRequest_t* request)
{
    if (user == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
                 0x737, 0)(
            "Purchase Error Not Found User(charac_no:%d)", characNo);
        return;
    }
    unsigned int accId = (unsigned int)user->get_acc_id();
    // P293：ORIG find 先求值（sret -0x38）、end 后求值（sret -0x20）；
    // 4.4 对单表达式 `find()!=end()` 恒先 end 后 find ⇒ 具名迭代器固定求值序。
    std::set<unsigned int>::iterator it =
        static_cast<PurchaseCashItemBonusScript*>(m_script1c)
            ->m_set0c.find(request->m_itemIdx);
    if (it != static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_set0c.end())
    {
        DB_UpdatePurchaseCashItemBonus::makeRequest(user->GetUID(), accId);
        cMyTrace(__PRETTY_FUNCTION__,
                 0x742, 0)(
            "CPurchaseCashItemBonus::updateTargetGoodsCount(%d)", accId);
    }
}

// ORIG 0x0810fae0：档位规则校验（count=本次比较基准购买数）。
// count < start 不发；end 非 0 且 count > end 不发；(count-start)%step!=0 不发。
bool CPurchaseCashItemBonus::verifyGiveBonusReward(
    PurchaseCashItemBonusScript::PurchaseBonusRule& rule, int count)
{
    if (count >= rule.m_purchaseStartCount)
    {
        if (rule.m_purchaseEndCount >= count || rule.m_purchaseEndCount == 0)
        {
            if ((count - rule.m_purchaseStartCount) %
                    rule.m_purchaseMutipleNumber == 0)
                return true;
        }
    }
    return false;
}

// ORIG 0x0810fb34：按购买数遍历 m_ruleMails24 规则-邮件对发奖励邮件。
// user 为空仅记 LogManager 告警返回 false；命中规则且发信成功读
// mail.m_itemIdx 记日志并将返回置 false（ORIG 返回值语义：true=本轮未发放）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，97=97；sent 槽 -0x9）。
// 修法：P198① result 循环外预清零（ORIG movl $0x0,-0x10）；删 rules 引用缓存，
// 每轮重发 `m_script1c->m_ruleMails24` 取址链（P157/P183）；it->/ptEv 替
// (*it)./deEv 并对 verifyGiveBonusReward/process_action_send_mail 逐次重发（P167）。
bool CPurchaseCashItemBonus::sendPurchaseBonusReward(CUser* user, int count)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "EventClassify.cpp",
            __PRETTY_FUNCTION__,
            0x755,
            "CPurchaseCashItemBonus::sendPurchaseBonusReward(pUser(NULL))");
        return false;
    }
    // ORIG local_14 在循环前预清零（movl $0x0,-0x10）⇒ 声明于循环外（P198①）。
    int result = 0;
    bool sent = true;                       // ORIG local_d
    CEventActionMng mng;                    // ORIG 循环外构造一次
    // ORIG 每次 begin()/end() 重发完整 `m_script1c->m_ruleMails24` 取址链
    // （无 rules 引用缓存，P157/P183）；迭代器访问走 operator->（ptEv）。
    for (std::vector<std::pair<PurchaseCashItemBonusScript::PurchaseBonusRule,
                               ::Action_SendMail> >::iterator it =
             static_cast<PurchaseCashItemBonusScript*>(m_script1c)
                 ->m_ruleMails24.begin();
         it != static_cast<PurchaseCashItemBonusScript*>(m_script1c)
                   ->m_ruleMails24.end();
         ++it)
    {
        if (verifyGiveBonusReward(it->first, count))
        {
            result = mng.process_action_send_mail(
                user, EVENT_MAIL_REF(it->second), true);
            if (result != 0)
            {
                LogManager::logFormat(1, "EventClassify.cpp",
                    __PRETTY_FUNCTION__,
                    0x763,
                    "CPurchaseCashItemBonus::sendPurchaseBonusReward(m_id:%d, charac_no:%d, reward:%d)",
                    user->get_acc_id(), user->getCurCharacNo(),
                    it->second.m_itemIdx);
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
    DB_InsertAccountFirstLoginDailyFlag::makeRequest(
        user->GetUID(), user->get_acc_id(), user->getCurCharacNo(), flags);
}

}  // namespace EventClassify
