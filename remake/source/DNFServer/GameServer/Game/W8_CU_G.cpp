// ============================================================================
// W8_CU_G.cpp — CUser ORIG Wave8-C 残余 G 批：hack/secu/capcha/exp/inform 族
//
// 本 TU 符号清单（25 个成员函数 + 2 个支撑小类 ctor；来源=报告 md 或 objdump）：
//   1. _ZN5CUser21gainMoneyFromHackUserEjyjy          0x0867fbba CUser/gainMoneyFromHackUser.md
//   2. _ZN5CUser11setHackUserEjjjjtj                  0x0867f748 CUser/setHackUser.md
//   3. _ZN5CUser11getHashCodeEPh                      0x084fa298 CUser/getHashCode.md
//   4. _ZN5CUser17checkHumanCertifyEiPt               0x0867f4c8 CUser/checkHumanCertify.md
//   5. _ZN5CUser22VerifyCleanPadVeriDataEj            0x08649b44 CUser/VerifyCleanPadVeriData.md
//   6. _ZN5CUser18reqSendMailCertifyEv                0x0868a51a CUser/reqSendMailCertify.md
//   7. _ZN5CUser24incTotalHackUserTradeCntEj          0x086972ee objdump（m_379.m_hackTradeCnt += n）
//   8. _ZN5CUser25incTotalHackUserTradeGoldEy         0x086972b6 objdump（m_379 hackTradeGold 64 位累加）
//   9. _ZNK5CUser24getTotalHackUserTradeCntEv         0x0853208c objdump（读 m_379.m_hackTradeCnt）
//  10. _ZNK5CUser25getTotalHackUserTradeGoldEv        0x08532078 objdump（edx:eax 读 lo/hi）
//  11. _ZNK5CUser25getCharacAntibotSerialNumEv        0x0823089c objdump（读 m_field8ec24）
//  12. _ZN5CUser18sendSecuRewardItemEP10Inven_Item25ENUM_SECURITY_REWARD_TYPEPi 0x0868833c CUser/sendSecuRewardItem.md
//  13. _ZN5CUser21SetAccountCargoDBDataEP10Inven_Itemjj 0x0867d86e CUser/SetAccountCargoDBData.md
//  14. _ZN5CUser25VerifyPresentAvengerTitleEv         0x0868b552 CUser/VerifyPresentAvengerTitle.md
//  15. _ZN5CUser9rewardExpEiRiS0_S0_S0_13eExpAddReasonb 0x0868b20c CUser/rewardExp.md
//  16. _ZN5CUser11gain_exp_spEiRiS0_13eExpAddReasonib 0x0866a3fe CUser/gain_exp_sp.md
//  17. _ZN5CUser22handleInformNoticeFlagEj            0x0868f956 CUser/handleInformNoticeFlag.md
//  18. _ZN5CUser20loadInformNoticeFlagEPc             0x0868f900 CUser/loadInformNoticeFlag.md
//  19. _ZN5CUser19getInformNoticeFlagEi               0x0868fa76 CUser/getInformNoticeFlag.md
//  20. _ZN5CUser24getInformNoticeFlagIndexERSt6vectorIhSaIhEE 0x0868fa8a 同上目录
//  21. _ZN5CUser20sendInformNoticeFlagEv              0x0868fb22 CUser/sendInformNoticeFlag.md
//  22. _ZN5CUser18SendEffectItemInfoEj                0x08659366 CUser/SendEffectItemInfo.md
//  23. _ZN5CUser18LoadEffectItemInfoEP20CONTINUOUS_ITEM_INFO 0x086590f0 CUser/LoadEffectItemInfo.md
//  24. _ZN5CUser20LoadCoolTimeItemInfoEP20CONTINUOUS_ITEM_INFO 0x08658ff6 CUser/LoadCoolTimeItemInfo.md
//  25. _ZN5CUser20SendCoolTimeItemInfoEj              0x086591fe CUser/SendCoolTimeItemInfo.md
//  支撑：_ZN10stLookOutsC1Ejj 0x0869380e、_ZN12tyIPCountersC1Ejj 0x08693824
//        （ORIG W ctor 仅置 +0/+4；声明位于 CUser.h，W 域批次未产出时由本 TU 定义）
//
// 本 TU 独占/已广播的共享头改动：
//   CUser.h      — stLookOuts/tyIPCounters 拆 pad+ctor 声明；+0x8e101 区拆
//                  m_field8e05c/m_field8e068；SendEffectItemInfo→int；
//                  追加 LoadEffectItemInfo 声明（均 sizeof 不变）
//   CMCAPManager.h — m_pad560 → bool m_hackIpLogged（sizeof 不变）
//   CEnvironment.h — m_llsKey[0x2d] → {0x28 键体, int m_field1a8, 1 pad}
//   新增头 Game/CStatisticMgr.h — s_statisticMgr 视图（+0x00 子对象实证）
// ============================================================================

#include "CUser.h"
#include "Inven_Item.h"
#include "CDataManager.h"                    // G_CDataManager / getItemList(+0x0c)
#include "GlobalData.h"                      // s_systemTime_/s_capchaDataManager/
                                             // g_autoPunishRuleHackTypeMgr/s_guild_proxy_mgr/s_monitor_proxy_mgr/s_statisticMgr
#include "CStatisticMgr.h"                   // 本 TU 新建权威视图
#include "CGuildServerProxy.h"               // SendCharLevelGrowType
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"                 // GetServerProxy<T> 完整模板
#include "CInventory.h"                      // insertItemIntoInventory / md5_* 声明（md5_context 兼容视图）
#include "GameWorld.h"
#include "TimerQueue.h"                      // TIMER_MESSAGE/InsertTimer/G_TimerQueue
#include "GameTypes.h"                       // cMyTrace
#include "Divide_C_ClassIP_EndIP.h"
#include "CUserCharacInfo.h"                 // ContinuousItemInfo/CSystemTime 等
#include "CSystemTime.h"
#include "CCapchaDataManager.h"              // WongWork::CCapchaDataManager + MSG_UDP_CAPCHA_CERTIFY
#include "CMailBoxHelper.h"                  // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"         // g_scriptStringManager_/findString(4,key,0)
#include "Arad_DataManager.h"                // ARAD::Singleton<Arad_DataManager>::Get/findGameScript
#include "Arad_EventPeriodDataManager.h"     // isApplied/checkCreateTime（producer 批提供定义）
#include "CUserProc.h"                       // APSystem::CUserProc::ClearActionAndSendtoUser
#include "CLevelDungeonPlayStatistic.h"
#include "CExpertJob.h"                      // expert_job::CExpertJob::IncreaseExpertJobExp(static)
#include "SkillSlot.h"                      // SkillSlot::get_remain_*_at_index
#include "CParty.h"                         // send_to_party（GetParty 完整类型）
#include "W6_HackPunishDeps.h"               // CAutoPunishRuleHackTypeMgr::checkTotalTradeRuleOver

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// 外部 DB 请求类最小声明区（owner=DB 域批次，定义不在本批）；
// namespace/class 形态与 ORIG mangled 对齐，均 nm 实证。
// ---------------------------------------------------------------------------
struct DB_AutoPunishedSecondUserData {       // ORIG makeRequest T 0x0843a790 _ZN...makeRequestEjyjj
    int makeRequest(unsigned int accId, unsigned long long gold,
                    unsigned int uid, unsigned int who);
};
struct DB_InsertAutoPunishSecondUser {       // ORIG makeRequest T 0x0842e34e _ZN...makeRequestEjjyb
    int makeRequest(unsigned int accId, unsigned int who,
                    unsigned long long gold, bool punishFlag);
};
struct DB_HackIPSave {                       // ORIG makeRequest T 0x08436ff8 _ZN...makeRequestEPKcS1_jj
    int makeRequest(const char* startIp, const char* rawIp,
                    unsigned int a, unsigned int b);
};
struct DB_InsertAutoPunishFirstUser {        // ORIG makeRequest T 0x0842dd7c _ZN...makeRequestEjPKcjjjjtj
    int makeRequest(unsigned int accId, const char* ip,
                    unsigned int p1, unsigned int p2, unsigned int p3,
                    unsigned int p4, unsigned short type, unsigned int p6);
};
struct DB_ForceSaveSimpleSSO {               // ORIG makeRequest T 0x084281ae _ZN...makeRequestEji
    int makeRequest(unsigned int accId, int errCnt);
};
struct DB_CountOfHumanCertify {              // ORIG makeRequest T 0x084283ca _ZN...makeRequestEj
    int makeRequest(unsigned int accId);
};
struct DB_SaveGiveAvengerTitleFlag {         // ORIG makeRequest T 0x084422dc _ZN...makeRequestEijj
    int makeRequest(int uid, unsigned int accId, unsigned int characNo);
};


// ===========================================================================
// 支撑小类 ctor（ORIG W，仅写前两个 uint；objdump 0x869380e/0x8693824 直转写）
// ===========================================================================
stLookOuts::stLookOuts(unsigned int a, unsigned int b) : m_a(a), m_b(b) {}
tyIPCounters::tyIPCounters(unsigned int a, unsigned int b) : m_a(a), m_b(b) {}

// ===========================================================================
// Hack trade money 统计族（objdump 直转写；m_379 字段组）
// ===========================================================================
long long CUser::getTotalHackUserTradeGold() const
{
    return m_379.m_hackTradeGold;
}

int CUser::getTotalHackUserTradeCnt() const
{
    return static_cast<int>(m_379.m_hackTradeCnt);
}

void CUser::incTotalHackUserTradeCnt(unsigned int n)
{
    m_379.m_hackTradeCnt += n;
}

void CUser::incTotalHackUserTradeGold(unsigned long long gold)
{
    m_379.m_hackTradeGold = getTotalHackUserTradeGold() + gold;
}

int CUser::getCharacAntibotSerialNum() const
{
    return m_field8ec24;
}

// ORIG 0x0867fbba：已 loading 时累加并按规则判定惩罚入库；否则置 loading 标志
// 并记首条未加载流水（DB_AutoPunishedSecondUserData::makeRequest(accId,gold,uid,who)）。
void CUser::gainMoneyFromHackUser(unsigned int who, unsigned long long gold,
                                  unsigned int cnt, unsigned long long totalGold)
{
    if (!isLoadingHackGold())
    {
        setLoadHackGold(true);
        DB_AutoPunishedSecondUserData().makeRequest(get_acc_id(), gold, GetUID(), who);
        return;
    }

    incTotalHackUserTradeGold(totalGold + gold);
    incTotalHackUserTradeCnt(cnt);

    unsigned long long totalNow = getTotalHackUserTradeGold();
    unsigned int cntNow = static_cast<unsigned int>(getTotalHackUserTradeCnt());
    if (GlobalData::g_autoPunishRuleHackTypeMgr->checkTotalTradeRuleOver(cntNow, totalNow))
    {
        SetTradePunishType(8);
    }
    bool punish8 = isTradePunishType(8);
    DB_InsertAutoPunishSecondUser().makeRequest(
        get_acc_id(), who, totalNow, punish8 != false);
}

// ORIG 0x0867f748：黑名单/惩罚注册入口。
// type==2 lookOuts1；type==3 IP 监视登记（一次性）；type==10 ARS 延迟惩罚定时器；
// 其它走即时 OnPunishUser + 0x81 定时器；尾部对非惩罚状态账号补发首犯入库包。
void CUser::setHackUser(unsigned int p1, unsigned int p2, unsigned int p3,
                        unsigned int p4, unsigned short type, unsigned int p6)
{
    unsigned short notifyType = type;

    {
        cMyTrace trace("void CUser::setHackUser(unsigned int, unsigned int, "
                       "unsigned int, unsigned int, short unsigned int, unsigned int)",
                       0x7e55, 0);
        char* idStr = NumberToString(get_acc_id(), 0);
        trace("m_id : %s, SetHackUser : %d(%d)\t  %d(%d)   ApplyFlag = %d\n",
              idStr, p1, p2, p3, p4, static_cast<unsigned int>(type));
    }

    if (type == 2)
    {
        m_lookOuts1.push_back(stLookOuts(p1, p3));
    }
    else if (type == 3)
    {
        if (m_mcap.m_hackIpLogged)
        {
            return;
        }
        char startIp[17];
        memset(startIp, 0, sizeof(startIp));
        char rawIp[17];
        memset(rawIp, 0, sizeof(rawIp));
        m_network.GetPeerIP2(rawIp, 0x10);
        Divide_C_ClassIP_EndIP(startIp, static_cast<char*>(0), rawIp);

        m_ipCounters.push_back(tyIPCounters(p1, p3));
        m_mcap.m_hackIpLogged = true;
        m_historyLog.IPCounterLog_InsertIp(static_cast<int>(p1),
                                           static_cast<int>(p3), startIp);
        DB_HackIPSave().makeRequest(startIp, rawIp, p1, p3);
        return;
    }
    else if (type == 10)
    {
        m_lookOuts2.push_back(stLookOuts(p1, p3));
        if (isTradePunishType(4))
        {
            return;
        }
        printf("ARS TRADE PUNISH USER - Rand Sec : %d\n", get_rand_int(0x122) + 10);
        int waitSec = get_rand_int(0x122) + 10;
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                    static_cast<int>(GetUID()),
                                    static_cast<TIMER_MESSAGE>(0x97), waitSec,
                                    get_unique_id(), 0xffffffffu);
    }
    else
    {
        int delaySec = get_rand_int(0x122) + 10;
        m_379.m_field0 = true;
        if (type == 8)
        {
            m_mcap.m_field561 = true;
            delaySec = 1;
        }
        if (type == 7 || type == 9 || type == 12 || type == 5)
        {
            m_mcap.m_field561 = true;
        }
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                    static_cast<int>(GetUID()),
                                    static_cast<TIMER_MESSAGE>(0x81), delaySec,
                                    get_unique_id(), 0xffffffffu);
        OnPunishUser();
        if (m_field796d8 != 0 && GetTradePunishType() != 0)
        {
            return;
        }
        if (m_field796d8 != 0 && (type == 1 || type == 4 || type == 9))
        {
            notifyType = 10;
        }
    }

    char ip[17];
    m_network.GetPeerIP2(ip, 0x10);
    DB_InsertAutoPunishFirstUser().makeRequest(
        get_acc_id(), ip, p1, p2, p3, p4, notifyType, p6);
}

// ORIG 0x084fa298：对角色列表首元素起 size()*0x14d1 字节做 MD5 摘要。
void CUser::getHashCode(unsigned char* digestOut)
{
    md5_context ctx;
    md5_starts(&ctx);
    unsigned int bytes =
        static_cast<unsigned int>(m_characList.size()) * 0x14d1u;
    unsigned char* head = reinterpret_cast<unsigned char*>(&m_characList[0]);
    md5_update(&ctx, head, static_cast<int>(bytes));
    md5_finish(&ctx, digestOut);
}

// ORIG 0x0867f4c8：CleanPad 人机认证应答比对（验证码头 5×ushort 逐项日志后 memcmp）。
bool CUser::checkHumanCertify(int len, unsigned short* answer)
{
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x7e07, 0);
        char* idStr = NumberToString(get_acc_id(), 0);
        const unsigned short* orig =
            reinterpret_cast<const unsigned short*>(m_capchaCertify);
        trace("checkHumanCertify m_id:%s, orig:%x%x%x%x%x, ans:%x%x%x%x%x",
              idStr,
              static_cast<unsigned int>(orig[4]), static_cast<unsigned int>(orig[3]),
              static_cast<unsigned int>(orig[2]), static_cast<unsigned int>(orig[1]),
              static_cast<unsigned int>(orig[0]),
              static_cast<unsigned int>(answer[0]), static_cast<unsigned int>(answer[1]),
              static_cast<unsigned int>(answer[2]), static_cast<unsigned int>(answer[3]),
              static_cast<unsigned int>(answer[4]));
    }

    DB_CountOfHumanCertify().makeRequest(get_acc_id());

    if (memcmp(answer, m_capchaCertify, static_cast<size_t>(len)) == 0)
    {
        if (m_humanCertifyErrorCnt != 0)
        {
            DB_ForceSaveSimpleSSO().makeRequest(get_acc_id(), 0);
        }
        m_humanCertifyErrorCnt = 0;
        setHumanCertified(true);
        return true;
    }

    ++m_humanCertifyErrorCnt;
    if (isCleanPadVerifyLimit() && m_field8e3ed)
    {
        DB_ForceSaveSimpleSSO().makeRequest(get_acc_id(), m_humanCertifyErrorCnt);
        SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x91), 8);
    }
    setHumanCertified(true);
    return false;
}

// ORIG 0x08649b44：验证三段 CleanPad 种子和是否等于上报校验值。
bool CUser::VerifyCleanPadVeriData(unsigned int value)
{
    unsigned int sum =
        static_cast<unsigned int>(m_mcap.m_cleanPadVeriA) +
        static_cast<unsigned int>(m_mcap.m_cleanPadVeriB) +
        static_cast<unsigned int>(m_mcap.m_cleanPadVeriC);
    if (value == sum)
        return true;
    return false;
}

// ORIG 0x0868a51a：向客户端下发人机认证图（capcha 队列随机一条 + 新鲜种子三段
// + 错误计数），并重置请求态/失败标志与 60 秒认证超时定时器（TIMER_MESSAGE 9）。
unsigned int CUser::reqSendMailCertify()
{
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return 0;
    }

    MSG_UDP_CAPCHA_CERTIFY* data = GlobalData::s_capchaDataManager.peekData();
    memcpy(m_capchaCertify, data, 10);

    PacketGuard packet;
    packet.put_header(0, 0x7d);
    packet.put_byte(5);
    packet.put_int(data->m_size);
    packet.put_binary(reinterpret_cast<char*>(data->m_image), data->m_size);
    packet.put_byte(0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    packet.put_short(static_cast<int>(veriA));
    packet.put_byte(static_cast<int>(veriB));
    packet.put_short(static_cast<int>(veriC));

    packet.put_byte(static_cast<int>(static_cast<unsigned int>(m_humanCertifyErrorCnt) & 0xff));
    packet.put_byte(1);
    packet.finalize(true);
    Send(packet);

    setHumanCertified(false);
    m_field8e3ed = false;

    if (static_cast<unsigned int>(m_humanCertifyTimerKey) > 0x7ffffffeu)
    {
        m_humanCertifyTimerKey = 0;
    }
    ++m_humanCertifyTimerKey;

    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                static_cast<int>(GetUID()),
                                static_cast<TIMER_MESSAGE>(9), 0x3c,
                                m_humanCertifyTimerKey, 0xffffffffu);
    return 1;
}

// ===========================================================================
// 安全奖励物品发放（sendSecuRewardItem）
// ORIG 0x0868833c：交易中或背包满 → 系统邮件退还；否则入背包并回填落位。
// ===========================================================================
void CUser::sendSecuRewardItem(Inven_Item* item, ENUM_SECURITY_REWARD_TYPE type,
                               int* outSlotVec)
{
    if (item == 0)
    {
        return;
    }

    Inven_Item local(*item);   // ORIG 将 Inven_Item 原样拷贝入栈局部
    bool failed = false;
    int slot = -1;

    if (!CheckInTrade())
    {
        slot = getCurCharacInvenW()->insertItemIntoInventory(
            local, static_cast<eItemAddReason>(0xd), true, true);
        if (slot == -1)
        {
            failed = true;
        }
    }
    else
    {
        failed = true;
    }

    if (!failed)
    {
        outSlotVec[static_cast<int>(type)] = slot;
        return;
    }

    // ORIG rodata：DAT_08cf10ec（"아이템 지급" 类标题）/ DAT_08cf10fc（正文），
    // GBK/EUC 多字节原样字节保真转写（objdump -s .rodata 实证）。
    static const char kSenderTitle[] =
        "\260\355\272\355\270\260\300\307\040\274\261\271\260";
    static const char kFailMessage[] =
        "\260\355\270\260\300\307\040\274\261\271\260\300\304\314\040\277\354\306"
        "\355\300\270\267\316\040\265\265\302\360\307\317\277\264\275\300\264\317"
        "\264\231.";

    ENUM_SERVER_GROUP group = static_cast<ENUM_SERVER_GROUP>(GetServerGroup());
    unsigned int characNo = getCurCharacNo();
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        kSenderTitle, local, 0, characNo, kFailMessage,
        static_cast<int>(strlen(kFailMessage)), 0, group, false, false);
    outSlotVec[static_cast<int>(type)] = -1;
}

// ORIG 0x0867d86e：账务仓库 DB 同步转发给内嵌 CAccountCargo。
void CUser::SetAccountCargoDBData(Inven_Item* items, unsigned int money,
                                  unsigned int capacity)
{
    m_accountCargo.SetDBData(this, items, money, capacity);
}

// ===========================================================================
// Avenger 计划（复仇者转职前礼物）事件
// ORIG 0x0868b552：事件码 0x8e + 创建时间窗判定，job==4 且未成长一级角色首次
// 登录 → 发放 0x28d3af 物品邮件并置 flag/DB 记录。
// ===========================================================================
char CUser::VerifyPresentAvengerTitle()
{
    // ORIG T 0x868b552 全录（报告 docs/class_func_reports/CUser/VerifyPresentAvengerTitle.md
    // 汇编+Ghidra C 双段读完；调用序 Counter 48 项对齐重建 2026-09-05 CUserC8）：
    //  accId 先取 → createTime → 临时 string 两段（allocator C1/string C1/dtor）
    //  → Singleton Get → findGameScript → checkCreateTime(0x8e, createTime, accId)
    //  → 三 guard 短路 → getCurSec → flag!=1 才 **第二次独立** Get/findGameScript
    //    +isApplied（orig 各 2 次，勿复用 period）→ Inven_Item C1 → **先写
    //    item.m_random.m_rand4=0x28d3af 再 create_item**（判据读同一 local_90 槽，
    //    原 const 局部为语义误植）→ findString/strncpy 交替对 → ReqDBSend 系
    //    参数序 group→strlen→characNo → makeRequest 系 characNo→acc→GetUID →
    //    trace 段 idxVal→characNo→acc→NumberToString→C1→operator()。
    unsigned int accId = (unsigned int)get_acc_id();
    unsigned int createTime =
        static_cast<unsigned int>(getCurCharacCreateTime());
    ARAD::Arad_EventPeriodDataManager* period =
        static_cast<ARAD::Arad_EventPeriodDataManager*>(
            ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                std::string("Arad_EventPeriodDataManager")));
    if (period->checkCreateTime(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e),
                                createTime, accId) != 1)
    {
        return 0;
    }

    if (get_charac_level() < 0x14 || get_charac_job() != 4 ||
        getCurCharFirstGrowType() != '\0')
    {
        return 0;
    }

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();

    bool eventOn = false;
    if (getFlagPresentAvengerTitle() != 1)
    {
        ARAD::Arad_EventPeriodDataManager* period2 =
            static_cast<ARAD::Arad_EventPeriodDataManager*>(
                ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                    std::string("Arad_EventPeriodDataManager")));
        eventOn = period2->isApplied(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e),
                                     curSec);
    }
    if (!eventOn)
    {
        return 0;
    }

    Inven_Item item;
    const int itemIdx = 0x28d3af;   // ORIG 0x868b77c 独立栈槽 -0x8c（非 Inven_Item
                                    // 成员——0x868b77c 与 item@-0x8e 尺寸不重叠实证；
                                    // ChronicleRandomOptionInfo 亦无 +1 偏移 int 成员）
    // ORIG 0x868b78b `mov 0xc(%eax),%eax` 直读 m_itemList 成员（getItemList
    // 转发 call orig=0，具名字段直访）。
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(0), item, 1);
    if (itemIdx != 0 && itemIdx != -1)
    {
        setFlagPresentAvengerTitle(true);

        // ORIG 0x868b7e2/82c 两段 peel+rep stos 清零块 = `= {0}` 聚合初始化
        //（makeGuildItemUpgradMessage board 同形，4.4 产 memset 调用 orig=0）。
        char subject[0x15] = {0};
        char body[0x100] = {0};
        strncpy(subject,
                g_scriptStringManager_.findString(4, "ready_to_avenger_title", 0),
                0x14);
        strncpy(body,
                g_scriptStringManager_.findString(4, "ready_to_avenger_mail", 0),
                0xff);

        ENUM_SERVER_GROUP group = static_cast<ENUM_SERVER_GROUP>(GetServerGroup());
        unsigned int blen = (unsigned int)strlen(body);
        unsigned int characNo = getCurCharacNo();
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            subject, item, 0, characNo, body, blen, 0, group, false, false);

        unsigned int characNo2 = getCurCharacNo();
        unsigned int accId2 = get_acc_id();
        int uid = (int)GetUID();
        DB_SaveGiveAvengerTitleFlag().makeRequest(uid, accId2, characNo2);

        int idxVal = itemIdx;
        unsigned int characNo3 = getCurCharacNo();
        char* idStr = NumberToString(get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa6eb, 0);
        trace("avenger plan event item post m_id(%s),charac_no(%u), itemidx(%u)",
              idStr, characNo3, static_cast<unsigned int>(idxVal));
        return 1;
    }
    return 0;
}

// ===========================================================================
// 经验主链
// ===========================================================================
// ORIG 0x0868b20c：结算前后 SP 快照 + gain_exp_sp；升级后按场景广播基础信息包，
// 成长事件触发 decide_growth_power_reward_system（producer=CUserOnlyOrig.cpp）。
void CUser::rewardExp(int expBase, int& bonusA, int& bonusB,
                      int& spBefore, int& spAfter,
                      ::eExpAddReason reason, bool triggerGrowth)
{
    PacketGuard packet;

    SkillSlot* skillR = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()));
    spBefore = skillR->get_remain_sp_at_index(
        static_cast<ENUM_SKILL_TREE_KIND>(GetCurCharacSkillTreeIndex()));

    calcHelpAbuseRatio(static_cast<unsigned int>(expBase), false);
    bool levelUp = gain_exp_sp(expBase, bonusA, bonusB,
                               (eExpAddReason)reason, 0, triggerGrowth);

    skillR = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()));
    spAfter = skillR->get_remain_sp_at_index(
        static_cast<ENUM_SKILL_TREE_KIND>(GetCurCharacSkillTreeIndex()));

    if (!levelUp)
    {
        return;
    }

    if (m_chState == 3)
    {
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        make_basic_info(reinterpret_cast<char*>(&packet), '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, this);
    }
    else if (m_chState == 5 || m_chState == 8)
    {
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(1);
        packet.put_short(1);
        make_basic_info(reinterpret_cast<char*>(&packet), '\1');
        packet.finalize(true);
        CParty* party = static_cast<CParty*>(GetParty());
        if (party != 0)
        {
            party->send_to_party(packet);
        }
    }

    if (triggerGrowth)
    {
        decide_growth_power_reward_system();
    }
}

// ORIG 0x0866a3fe：经验结算主流程 —— 入口节流（环境 workmode==2 时 49 级以上不
// 入经验）、累计字段、升级判定（_check_level_up，producer=CUserCheckLevel.cpp）、
// SP/SFP 返还与日志、升级后任务/公会/监控器通知、疲劳奖励 channel 组收尾、
// 专家职业经验结算与 55 级清动作点。
bool CUser::gain_exp_sp(int exp, int& sp, int& sfp,
                        eExpAddReason reason, int param, bool flag)
{
    // 环境 work-mode==2（G_CEnvironment()+0x1a8 具名化见 CEnvironment.h:m_field1a8）
    // 时，49 级以上角色不入经验。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        if (get_charac_level() > 0x31)
        {
            exp = 0;
        }
    }
    if (exp < 0)
    {
        exp = 0;
    }
    const int reasonVal = static_cast<int>(reason);
    if (reasonVal == 1 || reasonVal == 2)
    {
        m_character.m_f34 += exp;
    }
    m_character.m_f28 += exp;
    incPlayExpAdd(exp);

    bool levelUp = false;
    if (get_charac_level() < 0x55)
    {
        levelUp = _check_level_up(exp, sp, sfp, reason, param);
        SkillSlot* skillR = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()));
        int spBefore = skillR->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(0));
        gain_sp(sp);
        skillR = reinterpret_cast<SkillSlot*>(const_cast<char*>(getCurCharacSkillR()));
        int sfpBefore = skillR->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2));
        gain_sfp(sfp);
        history_log_sp(spBefore, sp, eSPAddReason_0);
        history_log_sfp(sfpBefore, sfp, eSPAddReason_0);
    }

    PacketGuard questPacket;
    if (levelUp)
    {
        CalcurateUserMaxLevel();

        // 当前等级追平最高装备等级时清除该标记。
        if (GetCurCharacMaxEquipLevel() != 0 &&
            get_charac_level() == GetCurCharacMaxEquipLevel())
        {
            SetCurCharacMaxEquipLevel(0);
        }

        getCurCharacQuestW()->ResetUrgentQuestWaitingList();
        UserQuest* questR = getCurCharacQuestR();
        questR->get_quest_info(reinterpret_cast<char*>(&questPacket));
        m_characterView.enableSaveCharacView();
        Send(questPacket);

        if (get_charac_level() == 0x55)
        {
            resetCharacFatigueGrownUpBuff();
        }

        _UpdateUserInfo4Guild();

        {
            unsigned int serverGroup = GetServerGroup();
            CGuildServerProxy* guildProxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(
                    static_cast<ENUM_SERVER_GROUP>(serverGroup));
            guildProxy->SendCharLevelGrowType(get_acc_id(),
                                              static_cast<unsigned int>(get_charac_no(-1)),
                                              static_cast<short>(get_charac_level()),
                                              static_cast<short>(getCurCharacGrowType()));
        }
        {
            unsigned int serverGroup = GetServerGroup();
            CMonitorServerProxy* monitorProxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    static_cast<ENUM_SERVER_GROUP>(serverGroup));
            monitorProxy->SendCharLevelGrowType(get_acc_id(),
                                                static_cast<unsigned int>(get_charac_no(-1)),
                                                static_cast<short>(get_charac_level()),
                                                static_cast<short>(getCurCharacGrowType()));
        }

        if (!VerifyPresentAvengerTitle())
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x5046, 0);
            char* idStr = NumberToString(get_acc_id(), 0);
            trace("CUser::VerifyPresentAvengerTitle() m_id(%s), charac_no(%u)",
                  idStr, static_cast<unsigned int>(getCurCharacNo()));
        }

        AddCurCharacMercenaryInfo();
    }

    // 疲劳奖励 channel 组（5/8/7/10/12）。
    if (m_chState == 5 || m_chState == 8 || m_chState == 7 ||
        m_chState == 10 || m_chState == 12)
    {
        if (!flag)
        {
            SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                           static_cast<ENUM_NOTIPACKET>(0x25), 0);
            set_charac_fatigue_buf_bonus_exp(0);
        }
        else if (levelUp)
        {
            SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                           static_cast<ENUM_NOTIPACKET>(0x25), 0);
        }
        if (levelUp)
        {
            decide_growth_power_reward_system();
        }
    }

    if (levelUp)
    {
        int playCount = GetCurCharacDungeonPlayCount();
        int level = get_charac_level();
        GlobalData::s_statisticMgr->m_levelDungeonPlay.IncreaseLevelDungeonPlay(level, playCount);
        ResetCurCharacDungeonPlayCount();

        int expertExp = GetCurCharacExpertJobExp();
        expert_job::CExpertJob::IncreaseExpertJobExp(this, expertExp);

        ReCalcChattingEmoticon();
        SendChattingEmoticon();

        if (get_charac_level() == 0x55)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                this, APSystem::_ActionGroupIndex(0x21),
                0, static_cast<ENUM_PACKETCLASS>(0));
        }
    }
    return levelUp;
}

// ===========================================================================
// Inform notice flag 族（m_informNoticeFlags 16 字节 × 8 bit 标志板）
// ===========================================================================
unsigned char CUser::getInformNoticeFlag(int idx)
{
    return m_informNoticeFlags[idx];   // ORIG 0x0868fa76 单 movzbl 0x8ebb1(%edx,%eax,1)
}

void CUser::handleInformNoticeFlag(unsigned int bitIndex)
{
    unsigned int bit = bitIndex & 7;
    unsigned int byte = bitIndex >> 3;
    if (bit < 8 && byte < 16 &&
        checkInformNoticeFlag(bit, byte))
    {
        m_informNoticeFlags[byte] |=
            static_cast<unsigned char>(1 << bit);
        enableSaveInformNoticeFlag();
    }
}

void CUser::loadInformNoticeFlag(char* src)
{
    resetInformNoticeFlag();
    if (src != 0)
    {
        for (int i = 0; i < 0x10; ++i)
        {
            m_informNoticeFlags[i] = static_cast<unsigned char>(src[i]);
        }
        sendInformNoticeFlag();
    }
}

void CUser::getInformNoticeFlagIndex(std::vector<unsigned char>& out)
{
    out.clear();
    for (int byteIdx = 0; byteIdx < 0x10; ++byteIdx)
    {
        unsigned char bits = static_cast<unsigned char>(getInformNoticeFlag(byteIdx));
        for (int bitIdx = 0; bitIdx < 8; ++bitIdx)
        {
            if (((bits >> bitIdx) & 1) != 0)
            {
                out.push_back(static_cast<unsigned char>(byteIdx * 8 + bitIdx));
            }
        }
    }
}

void CUser::sendInformNoticeFlag()
{
    std::vector<unsigned char> indexes;
    getInformNoticeFlagIndex(indexes);

    PacketGuard packet;
    packet.put_header(0, 0x1a0);
    packet.put_byte(static_cast<int>(indexes.size()));
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        packet.put_byte(static_cast<int>(indexes[i]));
    }
    packet.finalize(true);
    Send(packet);
}

// ===========================================================================
// 连续效果道具（effect / cooltime）载入与下发
// ===========================================================================
// ORIG 0x08659366：0xae 包头 + 数量 + 逐条目{idx, expire-now}下发。
int CUser::SendEffectItemInfo(unsigned int itemIdx)
{
    PacketGuard packet;
    packet.put_header(0, 0xae);
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (unsigned int i = 0;
         static_cast<int>(i) < static_cast<int>(count); ++i)
    {
        ContinuousItemInfo& info = (*list)[i];
        packet.put_int(info.m_itemIdx);
        packet.put_int(static_cast<int>(info.m_expireTime - itemIdx));
    }
    packet.finalize(true);
    return Send(packet);
}

// ORIG 0x086591fe：0xac 包头 + cooltime 表项迭代下发。
unsigned int CUser::SendCoolTimeItemInfo(unsigned int nowMs)
{
    PacketGuard packet;
    packet.put_header(0, 0xac);
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (std::vector<ContinuousItemInfo>::iterator it = list->begin();
         it != list->end(); ++it)
    {
        packet.put_int(it->m_itemIdx);
        packet.put_int(static_cast<int>(it->m_expireTime - nowMs));
    }
    packet.finalize(true);
    return static_cast<unsigned int>(Send(packet));
}

// ORIG 0x086590f0：DB 载入 effect 连用表 → 过滤有效条目 → 特效回调 → 清理无效并发送。
bool CUser::LoadEffectItemInfo(CONTINUOUS_ITEM_INFO* info)
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    list->clear();
    for (int i = 0; i < static_cast<int>(static_cast<signed char>(info->m_count)); ++i)
    {
        const CONT_ENTRY& entry = info->m_entries[i];
        if (curSec < entry.m_expireTime &&
            static_cast<unsigned int>(entry.m_itemIdx) < 0x7fffffffu &&
            entry.m_expireTime < 0x77359401u)
        {
            ContinuousItemInfo ci;
            ci.m_itemIdx = entry.m_itemIdx;
            ci.m_expireTime = entry.m_expireTime;
            _processSpeicalContinuousTimeItem(static_cast<unsigned int>(ci.m_itemIdx));
            list->push_back(ci);
        }
    }
    ClearUselessEffectItem(curSec);
    return SendEffectItemInfo(curSec) == 1;
}

// ORIG 0x08658ff6：DB 载入 cooltime 表 → 过滤 → 清理无效 → 下发 0xac 包。
bool CUser::LoadCoolTimeItemInfo(CONTINUOUS_ITEM_INFO* info)
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    list->clear();
    for (int i = 0; i < static_cast<int>(static_cast<signed char>(info->m_count)); ++i)
    {
        const CONT_ENTRY& entry = info->m_entries[i];
        if (curSec < entry.m_expireTime &&
            static_cast<unsigned int>(entry.m_itemIdx) < 0x7fffffffu &&
            entry.m_expireTime < 0x77359401u)
        {
            ContinuousItemInfo ci;
            ci.m_itemIdx = entry.m_itemIdx;
            ci.m_expireTime = entry.m_expireTime;
            list->push_back(ci);
        }
    }
    ClearUselessCooltimeItem(curSec);
    return SendCoolTimeItemInfo(curSec) == 1;
}

