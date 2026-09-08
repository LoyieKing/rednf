// ============================================================================
// fair_pvp::CFairPvPScore —— ORIG df_game_r 忠实转写
// 报告: docs/class_func_reports/fair_pvp__CFairPvPScore/*.md
//   ctor 0x84a47dc / D1 0x84a4850 / loadData 0x84a48f8 / getData 0x84a55a8 /
//   _saveData 0x84a57fe / _reset 0x84a5942 / UpdateScore 0x84a5a28 /
//   MakePacketSocre 0x84a5cc2 / ResetDaily W 0x84a645a(反汇编实证) /
//   JobKey ctor 0x84a643a / JobKey operator< 0x84a63f2
// W 内联组反汇编实证:
//   SetGiveItem 0x8234f74(写字节+0x5b 后 alter) / SetDailyPlayCount
//   0x8234f92(jns 钳负→0 写 int+0x50) / SetRemainBonusCount 0x8234fae(+0x5a)
// / GetDailyPlayCount 0x8568ec2 读 +0x50；SetLastTrainingSeal(Bonus)Count 与
//   Get* 三族同型读写字节 +0x58/+0x59/+0x5a。
// ============================================================================
#include "PacketGuard.h"              // CFairPvPScore.h 引用 PacketGuard&
#include "STEquipmentScript.h"        // 提供 guarded ENUM_CHARACTERJOB（DNF_ENUM_CHARACTERJOB_DEFINED）
#include "CFairPvPScore.h"

#include <string.h>

#include "GameWorld.h"                // G_GameWorld/GetChannelType
#include "MsgQueueMgr.h"              // s_msgq_mgr->put 完整类型
#include "GlobalData.h"               // s_stream_pool/s_msgq_mgr/s_systemTime_
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"       // PacketGuard 转发层 API
#include "SigTypes.h"                 // SIG_FAIR_PVP_SCORE（0x688）
#include "CItem.h"                    // CItem::make_item
#include "CInventory.h"               // insert_event_items/MakeItemPacket/getCurCharacInven*
#include "CUser.h"                    // Send/GetUID/get_charac_no/get_pvp_grade/GetServerGroup/...
#include "CUserCharacInfo.h"          // getPVPResultRefR/getCurCharFirstGrowType/get_charac_job/getCurCharacNo
#include "PvpResultType.h"            // 权威 PvpResultType（m_pvpGrade=+0x14，ORIG 084a4b94 mov 0x14(%eax)）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "ServerParameterScript.h"    // GetTraningSealGive/GetTraningFirstGive/get_pvp_item_give

extern CDataManager* G_CDataManager();

namespace fair_pvp
{

// ---- ORIG 0x84a47dc CFairPvPScore::CFairPvPScore() --------------------------
CFairPvPScore::CFairPvPScore()
    : charac_expand::CData()
{
}

// ---- ORIG 0x84a4850 ~CFairPvPScore() ---------------------------------------
CFairPvPScore::~CFairPvPScore()
{
}

// ---- ORIG 0x84a645a ResetDaily()（W，反汇编实证：仅清 +0x50） --------------
void CFairPvPScore::ResetDaily()
{
    m_dailyPlayCount = 0;
}

// ---- ORIG 0x84a63f2 JobKey::operator<(const JobKey&) const -----------------
bool JobKey::operator<(const JobKey& rhs) const
{
    if (m_job < rhs.m_job)
        return true;
    // [b28 P59 块序试验] ORIG 0x84a63f2：第二比较 `cmp;jne 尾mov$0`（!= 为跳走
    // 侧、== fall-through 进第三段）；false 出口共享物理尾块（84a6433 mov$0 在
    // 三段之后）。等值嵌套形令 4.4 反向排布尝试。
    if (m_job == rhs.m_job) {
        // ORIG 0x84a63f2: 第三比较 `cmp %al,%dl; jge 尾`（jge=跳 false 尾，
        // < 落 true 岛）。字段 P211 恢复 signed char，直接比较即原有符号序。
        if (m_sub < rhs.m_sub)
            return true;
    }
    return false;
}

// ---- ORIG 0x84a643a JobKey::JobKey(ENUM_CHARACTERJOB, unsigned char) -------
JobKey::JobKey(ENUM_CHARACTERJOB job, unsigned char sub)
{
    // [b28 P211 AE] 字段 enum/schar 化后本体与 ORIG 0x84a643a 13 条逐字同形
    // （探针 /tmp/l2_p2.cpp JK 实证）：store1 this→value→movl；store2 RHS 先
    // movzbl→this 重载→movb。旧 B24D N 判词作废。
    m_job = job;
    m_sub = sub;
}

// ---- ORIG 0x84a5942 _reset() ------------------------------------------------
void CFairPvPScore::_reset()
{
    m_jobScore_.clear();
    m_recentResults_.clear();
    for (int i = 0; i < 2; ++i)
    {
        m_modeStats[i][0] = 0;
        m_modeStats[i][1] = 0;
        m_modeStats[i][2] = 0;
    }
    m_field40 = 0;
    m_field44 = 0;
    m_winStreak = 0;
    m_bestWinStreak = 0;
    m_dailyPlayCount = 0;
    m_field54 = 0;
    m_lastTrainingSealCount = 0;
    m_lastTrainingSealBonusCount = 0;
    m_remainBonusCount = 0;
    m_giveItem = 0;
}

// ---- ORIG 0x84a57fe _saveData(CUser*) --------------------------------------
bool CFairPvPScore::_saveData(CUser* pUser)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("FairPvPScore.cpp", 0x10a),
                       true);

    **guard << 0x261;                       // SIG_FAIR_PVP_SCORE 头
    **guard << (int)pUser->GetUID();

    SIG_FAIR_PVP_SCORE* sig =
        guard.operator->()->GetInBuffer<SIG_FAIR_PVP_SCORE>();
    memset(sig, 0, sizeof(SIG_FAIR_PVP_SCORE));          // 0x688
    sig->m_characNo = pUser->get_charac_no(-1);          // ORIG *(int*)local_10 = m_characNo(+0x00)
    this->getData(reinterpret_cast<char*>(sig));         // ORIG vtable+8 (getData)；线格式序列化
    sig->m_lastPlayTime = GlobalData::s_systemTime_.getCurSec();   // +0x680

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return true;                                          // ORIG mov $0x1,%ebx
}

// ---- ORIG 0x84a55a8 getData(char*) const -----------------------------------
void CFairPvPScore::getData(char* pData) const
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;
    for (std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
             m_jobScore_.begin();
         it != m_jobScore_.end() && idx <= 99; ++it, ++idx)
    {
        // ORIG 基址形态 (idx+2)*0x10 + 0x10（084a55e1 `lea 0x2;shl 4; movb 0x10(%ecx,%eax)`），
        // 经具名条目视图写入，宽度逐字段与反汇编一致。
        SIG_FAIR_PVP_SCORE_ENTRY* entry =
            reinterpret_cast<SIG_FAIR_PVP_SCORE_ENTRY*>(pData + (idx + 2) * 0x10);
        entry[1].m_job = (char)it->first.m_job;
        entry[1].m_sub = (char)it->first.m_sub;
        entry[1].m_cnt0 = it->second.m_cnt0;
        entry[1].m_cnt1 = it->second.m_cnt1;
        entry[1].m_cnt2 = it->second.m_cnt2;
        ++sig->m_jobScoreCount;                          // 字节自增（ORIG movzbl/lea/movb）
    }

    {
        int i = 0;
        for (std::list<unsigned char>::const_iterator it =
                 m_recentResults_.begin();
             it != m_recentResults_.end() && i <= 9; ++it, ++i)
            sig->m_recentResults[i] = *it;
    }

    for (int i = 0; i < 2; ++i)
    {
        sig->m_modeStats[i][0] = m_modeStats[i][0];
        sig->m_modeStats[i][1] = m_modeStats[i][1];
        sig->m_modeStats[i][2] = m_modeStats[i][2];
    }
    sig->m_field1c = m_field40;
    sig->m_field20 = m_field44;
    sig->m_winStreak = m_winStreak;
    sig->m_bestWinStreak = m_bestWinStreak;
    sig->m_dailyPlayCount = m_dailyPlayCount;
    sig->m_lastPlayTime = m_field54;   // [b28] 字段 int 化后同型直存（P211 换型）
    sig->m_giveItem = m_giveItem;
}

// ---- ORIG 0x84a5a28 UpdateScore(int,int,int,int,int,int,int) ---------------
void CFairPvPScore::UpdateScore(int jobId, int subKey, int mode,
                                int cnt0, int cnt1, int cnt2, int resultKind)
{
    if (mode == 1)
    {
        const JobKey key((ENUM_CHARACTERJOB)jobId, (unsigned char)subKey);
        std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::iterator it =
            m_jobScore_.find(key);
        if (it == m_jobScore_.end())
        {
            FAIR_PVP_SCORE_MODE_COUNT zero;
            zero.m_cnt0 = 0;
            zero.m_cnt1 = 0;
            zero.m_cnt2 = 0;
            m_jobScore_.insert(
                std::pair<const JobKey, FAIR_PVP_SCORE_MODE_COUNT>(key, zero));
        }
        else
        {
            it->second.m_cnt0 += cnt0;
            it->second.m_cnt1 += cnt1;
            it->second.m_cnt2 += cnt2;
        }
        m_modeStats[0][0] += cnt0;
        m_modeStats[0][1] += cnt1;
        m_modeStats[0][2] += cnt2;
    }
    else
    {
        if (resultKind == 2)
            ++m_field40;
        else if (resultKind == 3)
            ++m_field44;
        m_modeStats[1][0] += cnt0;
        m_modeStats[1][1] += cnt1;
        m_modeStats[1][2] += cnt2;
    }

    if (cnt0 != 0)
        m_recentResults_.push_back(1);
    else if (cnt1 != 0)
        m_recentResults_.push_back(2);
    else if (cnt2 != 0)
        m_recentResults_.push_back(3);

    if (m_recentResults_.size() > 10)
        m_recentResults_.pop_front();

    if (cnt1 != 0)
    {
        m_winStreak = 0;
    }
    else if (cnt0 != 0)
    {
        ++m_winStreak;
        if ((unsigned int)m_bestWinStreak < (unsigned int)m_winStreak)
            m_bestWinStreak = m_winStreak;
    }

    ++m_dailyPlayCount;
    alter();
}

// ---- ORIG 0x84a5cc2 MakePacketSocre(PacketGuard&, int) const ---------------
void CFairPvPScore::MakePacketSocre(PacketGuard& pkt, int mode) const
{
    pkt.put_byte(mode);
    pkt.put_int(m_modeStats[0][0]);
    pkt.put_int(m_modeStats[1][0]);

    int winCnt = 0;
    int loseCnt = 0;
    int drawCnt = 0;
    for (std::list<unsigned char>::const_iterator it = m_recentResults_.begin();
         it != m_recentResults_.end(); ++it)
    {
        if (*it == 1)
            ++winCnt;
        else if (*it == 2)
            ++loseCnt;
        else if (*it == 3)
            ++drawCnt;
    }
    pkt.put_byte(winCnt);
    pkt.put_byte(loseCnt);
    pkt.put_byte(drawCnt);
    pkt.put_int(m_winStreak);
    pkt.put_int(m_field40);
    pkt.put_int(m_field44);
    pkt.put_int(m_bestWinStreak);

    if (mode != 1)
        return;

    pkt.put_int(m_modeStats[0][1]);
    pkt.put_int(m_modeStats[0][2]);
    pkt.put_int(m_modeStats[1][1]);
    pkt.put_int(m_modeStats[1][2]);
    pkt.put_byte((int)m_jobScore_.size());
    for (std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
             m_jobScore_.begin();
         it != m_jobScore_.end(); ++it)
    {
        pkt.put_byte(it->first.m_job);                     // ORIG emit dword 低字节
        pkt.put_byte((int)(signed char)it->first.m_sub);   // ORIG movsbl
        pkt.put_int(it->second.m_cnt0);
        pkt.put_int(it->second.m_cnt1);
        pkt.put_int(it->second.m_cnt2);
    }
}

// ---- W 内联组（ORIG 地址见头文件注释，逐一反汇编实证） ---------------------
void CFairPvPScore::SetGiveItem(int v)                 // 0x8234f74
{
    m_giveItem = (unsigned char)v;
    alter();                                           // ORIG call CData::alter
}

int CFairPvPScore::GetDailyPlayCount()                 // 0x8568ec2
{
    return m_dailyPlayCount;
}

void CFairPvPScore::SetDailyPlayCount(int v)           // 0x8234f92
{
    // [B24D判词 N] 语义正确：if(v<0)v=0(ORIG jns 钳负)；m_dailyPlayCount=v；
    // diff=mov 0x8(%ebp),%eax 与 mov 0xc(%ebp),%edx 装载序互换（GCC4.1 vs 4.4），
    // 10/10 纯 lowering，语义等价。report NO 由 nm/objdump diff 兜底。
    if (v < 0)
        v = 0;                                         // ORIG jns/clear
    m_dailyPlayCount = v;
}

void CFairPvPScore::SetRemainBonusCount(int v)         // 0x8234fae
{
    m_remainBonusCount = (unsigned char)v;
}

void CFairPvPScore::SetLastTrainingSealCount(int v)    // 0x8568ece
{
    m_lastTrainingSealCount = (unsigned char)v;
}

void CFairPvPScore::SetLastTrainingSealBonusCount(int v)  // 0x8568ede
{
    m_lastTrainingSealBonusCount = (unsigned char)v;
}

int CFairPvPScore::GetRemainBonusCount()               // 0x85dfdcc
{
    return (int)m_remainBonusCount;
}

int CFairPvPScore::GetLastTrainingSealCount()          // 0x85dfdac
{
    return (int)m_lastTrainingSealCount;
}

int CFairPvPScore::GetLastTrainingSealBonusCount()     // 0x85dfdbc
{
    return (int)m_lastTrainingSealBonusCount;
}

namespace
{

inline bool IsFairPvpTrainingChannel()                 // ORIG 双调用模式 0xf/0x10
{
    GameWorld* world = G_GameWorld();
    const int type = world->GetChannelType();
    if (type == 0xf || type == 0x10)
        return true;
    return false;
}

}  // namespace

// ---- ORIG 0x84a48f8 loadData(CUser*, char*) --------------------------------
bool CFairPvPScore::loadData(CUser* pUser, char* pData)
{
    bool ok = true;
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);

    // 区块 1：职业计分表（entry stride 0x10，起于 (i+2)*0x10）
    int count = (int)sig->m_jobScoreCount;
    for (int i = 0; (i < count) && (i <= 99); ++i)
    {
        // ORIG 基址形态 (i+2)*0x10 + 0x10（084a491d `lea 0x2;shl 4` + movzbl
        // 0x10/0x11、lea 0x14 传指针），经具名条目视图读取，宽度逐字段实证。
        const int base = (i + 2) * 0x10;
        SIG_FAIR_PVP_SCORE_ENTRY* entry =
            reinterpret_cast<SIG_FAIR_PVP_SCORE_ENTRY*>(pData + base) + 1;
        const JobKey key((ENUM_CHARACTERJOB)(unsigned char)entry->m_job,
                         entry->m_sub);
        FAIR_PVP_SCORE_MODE_COUNT value;
        value.m_cnt0 = entry->m_cnt0;
        value.m_cnt1 = entry->m_cnt1;
        value.m_cnt2 = entry->m_cnt2;
        std::pair<std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::iterator, bool>
            ret = m_jobScore_.insert(
                std::pair<const JobKey, FAIR_PVP_SCORE_MODE_COUNT>(key, value));
        if (!ret.second)
        {
            LogManager::logFormat(
                1, "FairPvPScore.cpp",
                __PRETTY_FUNCTION__,
                0x20, "job_score_.insert error(%d,%d)",
                (int)(unsigned char)entry->m_job,
                (int)(unsigned char)entry->m_sub);
        }
    }

    // 区块 2：最近对局结果列表（最多 10 条，遇 0 截断）
    for (int i = 0; (i <= 9) && (sig->m_recentResults[i] != '\0'); ++i)
        m_recentResults_.push_back(sig->m_recentResults[i]);

    // 区块 3：两份模式统计记录
    for (int i = 0; i < 2; ++i)
    {
        m_modeStats[i][0] = sig->m_modeStats[i][0];
        m_modeStats[i][1] = sig->m_modeStats[i][1];
        m_modeStats[i][2] = sig->m_modeStats[i][2];
    }
    m_field40 = sig->m_field1c;
    m_field44 = sig->m_field20;
    m_winStreak = sig->m_winStreak;
    m_bestWinStreak = sig->m_bestWinStreak;
    m_dailyPlayCount = sig->m_dailyPlayCount;
    m_field54 = (int)sig->m_lastPlayTime;

    // 训练特训渠道（0xf/0x10）且无上次结算时间 → 补发入场奖励
    if (IsFairPvpTrainingChannel() && (int)sig->m_lastPlayTime == 0)
    {
        std::vector<std::pair<int, int> > items;
        // ORIG 0x084a4b86 call getPVPResultRefR(CUser)——返回值未使用【死调用，
        // 推断为编译器消除前的内部加载】，按忠实口径不再转写。
        const PvpResultType* pvp = pUser->getPVPResultRefR();
        ServerParameterScript* sps =
            &G_CDataManager()->m_serverParameter;
        const int sealGive = sps->GetTraningSealGive(
            pvp->m_pvpGrade);                              // +0x14（084a4b94 mov 0x14(%eax)）
        if (sealGive > 0)
            items.push_back(std::make_pair(0xcf0, sealGive));
        sps->GetTraningFirstGive(pUser->get_charac_job(),
                                 (int)(signed char)pUser->getCurCharFirstGrowType(),
                                 pUser->get_pvp_grade(), items);

        if (!items.empty())
        {
            std::vector<int> slots;
            CInventory* inven = pUser->getCurCharacInvenW();
            int inserted = inven->insert_event_items(
                items, slots, (eMoneyAddReason)3, (eItemAddReason)8, true,
                "game_server_msg_171", "game_server_msg_172");
            if (inserted >= 1 && slots.size() >= 3)
            {
                PacketGuard pkt;
                pkt.put_header(0, 0xe);
                pkt.put_byte(0);
                int fillIndex = pkt.get_index();
                pkt.put_short(0);
                int made = 0;
                for (size_t j = 0; j < slots.size(); j += 3)
                {
                    if (inven->MakeItemPacket((INVEN_TYPE)1,
                                              slots[j], pkt))
                        ++made;
                }
                pkt.put_short(fillIndex, made);
                pkt.finalize(true);
                pUser->Send(pkt);
            }
        }
        m_field54 = GlobalData::s_systemTime_.getCurSec();
        alter();
    }

    // 剩余可领训练印章数（ORIG 读 CDataManager+0x49e0 =
    // m_serverParameter(+0x68)+0x4978 → FairMatchRooms 批已具名
    // m_schoolMatchDailyPlayLimit）
    {
        const unsigned short dailyLimit =
            G_CDataManager()->m_serverParameter.m_schoolMatchDailyPlayLimit;
        if ((unsigned int)m_dailyPlayCount < (unsigned int)dailyLimit)
            m_remainBonusCount =
                (unsigned char)(dailyLimit -
                                (unsigned char)m_dailyPlayCount);  // ORIG sub %al,%dl
        else
            m_remainBonusCount = 0;
    }
    {
        PacketGuard pkt;
        pkt.put_header(0, 0x152);
        pkt.put_byte((int)m_remainBonusCount);
        pkt.finalize(true);
        pUser->Send(pkt);
    }

    m_giveItem = sig->m_giveItem;

    // 特训渠道：可领但尚未领取的道具（发 0x161 提示包）
    if (IsFairPvpTrainingChannel())
    {
        const int giveLimit =
            G_CDataManager()->m_serverParameter.get_pvp_item_give(
                pUser->get_pvp_grade());
        if ((int)m_giveItem <= giveLimit)
        {
            PacketGuard localSender;            // ORIG 复用同一栈 PacketGuard 的 clear
            localSender.clear();
            localSender.put_header(0, 0x161);
            localSender.finalize(true);
            pUser->Send(localSender);
        }
    }

    // 上次结算时间早于维护点 → 按累计成绩补发两封结算邮件
    if (m_field54 != 0 && (unsigned int)m_field54 <= 0x4ddd89efu)
    {
        const int sumA = m_modeStats[0][0] + m_modeStats[0][1] + m_modeStats[0][2];
        const int bonusSumA50 = (sumA >= 10 && sumA <= 0x31) ? 10 : 0;
        const int bonusSumA60 = (sumA > 0x31) ? 60 : 0;
        const int bonusA15 = (m_modeStats[0][0] >= 10 && m_modeStats[0][0] <= 0x31) ? 15 : 0;
        const int bonusA90 = (m_modeStats[0][0] > 0x31) ? 90 : 0;
        const int sumB = m_modeStats[1][0] + m_modeStats[1][1] + m_modeStats[1][2];
        const int bonusSumB50 = (sumB >= 10 && sumB <= 0x31) ? 10 : 0;
        const int bonusSumB60 = (sumB > 0x31) ? 60 : 0;
        const int bonusB15 = (m_modeStats[1][0] >= 10 && m_modeStats[1][0] <= 0x31) ? 15 : 0;
        const int bonusB90 = (m_modeStats[1][0] > 0x31) ? 90 : 0;

        const int total = m_modeStats[1][0] * 2 + m_modeStats[0][0] * 2 +
                          m_modeStats[1][1] +
                          m_modeStats[1][2] + m_modeStats[1][0] * 3 + m_modeStats[0][1] +
                          m_modeStats[0][2] +
                          m_modeStats[0][0] * 2 + bonusSumA50 + bonusSumA60 + bonusA15 +
                          bonusA90 + bonusSumB50 + bonusSumB60 + bonusB15 +
                          bonusB90;

        // —— 第一封：总成绩奖励道具 0xcf0，数量 total+200 ——
        Inven_Item item;                       // 默认构造清零
        item.m_addInfo2 = total + 200;         // ORIG -0xff(%ebp) = item+0x07
        item.m_addInfo = 0xcf0;                // ORIG -0x104(%ebp)= item+0x02
        CItem* def = G_CDataManager()->find_item(item.m_addInfo);
        if (def == NULL)
            return false;                      // ORIG ebx=0 早退（仍析构包守卫）

        def->make_item(item);
        {
            char titleBuf[21];                 // ORIG 0x15 字节
            memset(titleBuf, 0, sizeof(titleBuf));
            const char* title = g_scriptStringManager_.findString(
                4, "game_server_msg_171", 0);
            strncpy(titleBuf, title, 0x14);
            char msgBuf[256];
            memset(msgBuf, 0, sizeof(msgBuf));
            const char* msg = g_scriptStringManager_.findString(
                4, "game_server_msg_177", 0);
            strncpy(msgBuf, msg, 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)pUser->getCurCharacNo(), msgBuf,
                (int)strlen(msgBuf), 0, pUser->GetServerGroup(), true, false);
        }

        // —— 第二封：印章道具 0x289788，数量 1 ——
        Inven_Item item2;
        item2.m_addInfo2 = 1;
        item2.m_addInfo = 0x289788;
        CItem* def2 = G_CDataManager()->find_item(item2.m_addInfo);
        if (def2 == NULL)
            return false;

        def2->make_item(item2);
        {
            char titleBuf[21];
            memset(titleBuf, 0, sizeof(titleBuf));
            const char* title = g_scriptStringManager_.findString(
                4, "game_server_msg_171", 0);
            strncpy(titleBuf, title, 0x14);
            char msgBuf[256];
            memset(msgBuf, 0, sizeof(msgBuf));
            const char* msg = g_scriptStringManager_.findString(
                4, "game_server_msg_178", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item2, 0,
                (unsigned int)pUser->getCurCharacNo(), msgBuf,
                (int)strlen(msgBuf), 0, pUser->GetServerGroup(), true, false);
        }

        m_field54 = GlobalData::s_systemTime_.getCurSec();
        alter();
    }

    return true;
}

}  // namespace fair_pvp
