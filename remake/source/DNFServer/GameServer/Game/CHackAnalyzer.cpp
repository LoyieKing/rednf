// df_game_r Game/ WongWork::CHackAnalyzer 实现（S5 语义对齐批次）。
// 依据 docs/class_func_reports/WongWork__CHackAnalyzer/*.md 逐函数还原：
//   addServerHackCnt 0x080f8c7e / reportHackInfo 0x080f8080 /
//   resetHackInfo 0x080f81a6 / CheckExceptionCnt 0x080f81fc /
//   CheckHackRateByPartyCount 0x080f8320 / CheckHackRateByDifficult 0x080f842e /
//   ReqDBSaveHackInfo 0x080f8f62 / AddCheckRateHackTypeList 0x080f8550 /
//   getBaseHackType 0x080f9868 / setBaseHackType 0x080f9856 /
//   resetServerHackAccumulatedCnt 0x086950b6 / onUpdateHackCount 0x080f8c4e
#include "CUser.h"

#include <string.h>
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameRand.h"
#include "CParty.h"
#include "CDungeon.h"
#include "CBattle_Field.h"
#include "CUserCharacInfo.h"

// ---- 静态概率表（ORIG .rodata 0x8b3e094 / 0x8b3e09c，objdump -s 实测）----
static const short _S_HACK_CHECK_RATE_BY_PARTYCOUNT[4] = {100, 50, 33, 25};
static const short _S_HACK_CHECK_RATE_BY_DIFFICULT[5] = {25, 20, 15, 10, 0};
namespace WongWork
{

struct SIG_SAVE_CLIENT_HACK_LOG
{
    unsigned int m_uid;
    unsigned int m_type;
    unsigned short m_cnt;
    int m_a;
    int m_b;
};

// 计数区寻址（ORIG：this + 4*(idx)+8，idx = t-0x65 与 t-0x65+0xf44）。
// m_cntRegion 自 this+0x28 起。两数组均落在 ctor memset 区域内。
static inline int& hackCntA(CHackAnalyzer* self, int type)
{
    return *(int*)(self->m_cntRegion + ((type - 0x65) * 4 + 8) - 0x28);
}
static inline int& hackCntB(CHackAnalyzer* self, int type)
{
    return *(int*)(self->m_cntRegion + ((type - 0x65 + 0xf44) * 4 + 8) - 0x28);
}

// ORIG WongWork::CAutoPunishRuleHackTypeMgr 布局镜像（权威定义在
// GlobalData.cpp；此处仅按偏移只读访问 m_exceptionMap @+0x58）。「推断」标注：
// 偏移来自 mgr ctor 反汇编 0x82a5b1a。
struct stHacktypeExceptionInfoView_t
{
    unsigned short m_cnt;
    unsigned short m_flag;
};

CHackAnalyzer::CHackAnalyzer()
{
    std::vector<stHackInfo_t>* list =
        new (&m_hackInfoList) std::vector<stHackInfo_t>();
    (void)list;
    new (&m_checkTypes) std::vector<int>();
    new (&m_exceptionCntMap) std::map<int, int>();
    new (&m_checkRateHackTypes) std::vector<int>();
    new (&m_vec7a30) std::vector<int>();
    resetHackInfo();
    memset(m_cntRegion + 0x00, 0, 0x3cf0);  // ORIG memset(this+0x28,0,0x3cf0)
    AddCheckRateHackTypeList();
}

CHackAnalyzer::~CHackAnalyzer() {}

ENUM_HACKTYPE CHackAnalyzer::getBaseHackType() { return (ENUM_HACKTYPE)m_baseHackType; }

void CHackAnalyzer::setBaseHackType(ENUM_HACKTYPE t) { m_baseHackType = (int)t; }

void CHackAnalyzer::resetServerHackAccumulatedCnt(ENUM_HACKTYPE type)
{
    // ORIG W 0x086950b6：清 B 表累计项（推断：与 addServerHackCnt 的 B 数组对应）。
    hackCntB(this, (int)type) = 0;
}

void CHackAnalyzer::setLastMonsterDeadTime(long t)
{
    // ORIG W 0x085df98c：写 +0x7a08 时间戳（推断：resetHackInfo 清零项）。
    m_field7a08 = (int)t;
}

void CHackAnalyzer::beginCollectHackInfo(CUser*)
{
    // ORIG 0x080f79e8（本批未展开）：清空收集列表并记录用户指针。
    m_hackInfoList.clear();
}

bool CHackAnalyzer::CheckExceptionCnt(ENUM_HACKTYPE type, int cnt)
{
    stHacktypeExceptionInfoView_t info;
    info.m_cnt = 0;
    info.m_flag = 0;

    // CAutoPunishRuleHackTypeMgr::GetExceptionCtx(int)（ORIG T 0x080f972a）：
    // 查 m_exceptionMap(+0x58)，命中复制 {cnt,flag}，否则置零。
    {
        // 布局镜像视图（权威定义见 GlobalData.cpp 同布局类）。
        struct MgrView
        {
            char m_pad58[0x58];
            std::map<int, stHacktypeExceptionInfoView_t> m_map;
        };
        MgrView* view = (MgrView*)GlobalData::g_autoPunishRuleHackTypeMgr;
        std::map<int, stHacktypeExceptionInfoView_t>::iterator it =
            view->m_map.find((int)type);
        if (it != view->m_map.end())
            info = it->second;
    }

    if (info.m_cnt == 0)
    {
        if (info.m_flag == 0 || (int)info.m_flag < cnt)
            return false;
        return true;
    }

    std::pair<std::map<int, int>::iterator, bool> res =
        m_exceptionCntMap.insert(std::make_pair((int)type, 0));
    if (!res.second)
    {
        res.first->second += cnt;
        if ((int)info.m_cnt <= res.first->second)
            return false;
    }
    return true;
}

bool CHackAnalyzer::CheckHackRateByPartyCount(ENUM_HACKTYPE type, int count)
{
    for (std::vector<int>::iterator it = m_checkRateHackTypes.begin();
         it != m_checkRateHackTypes.end(); ++it)
    {
        if (*it == (int)type)
        {
            int idx = count - 1;
            if (idx < 0 || idx > 3 ||
                _S_HACK_CHECK_RATE_BY_PARTYCOUNT[idx] == 100)
                return true;
            return !(get_rand_int(100) > _S_HACK_CHECK_RATE_BY_PARTYCOUNT[idx]);
        }
    }
    return true;
}

bool CHackAnalyzer::CheckHackRateByDifficult(ENUM_HACKTYPE type, int lvDiff,
                                             int difficult)
{
    bool found = false;
    for (std::vector<int>::iterator it = m_checkRateHackTypes.begin();
         it != m_checkRateHackTypes.end(); ++it)
    {
        if (*it == (int)type)
        {
            found = true;
            break;
        }
    }
    if (!found)
        return true;

    if ((int)type == 0x86)
        lvDiff = lvDiff << 1;
    if (difficult < 0 || difficult > 4)
        return true;

    int chance = 100 - lvDiff - _S_HACK_CHECK_RATE_BY_DIFFICULT[difficult];
    if (chance < 0x19 && (int)type != 0x86)
        chance = 0x19;
    return !(chance < get_rand_int(100));
}

void CHackAnalyzer::AddCheckRateHackTypeList()
{
    static const int kTypes[] = {
        0x84,  0xdc,  0x138, 0x139, 0x86,  0xde,  0x1a4, 0x1a5,
        0x1a7, 0xdf,  0xe0,  0xe1,  0x1a4, 0x1a5, 0x1a7, 0xdf,
        0xe0,  0xe1,  0x84,  0xca,  0xcd,  0xce,  0xd0,  0xd1,
        0xd7,  0xd8,  0xd9,  0xda,  0xdb,  0xdc,  0x138, 0x139,
        0x193, 0x19e, 0x86,  0xde,  0x1a4, 0x1a5, 0x1a7, 0xdf,
        0xe0,  0xe1,
    };
    for (unsigned int i = 0; i < sizeof(kTypes) / sizeof(kTypes[0]); ++i)
        m_checkRateHackTypes.push_back(kTypes[i]);
}

void CHackAnalyzer::ReqDBSaveHackInfo(unsigned int uid, unsigned int type,
                                      unsigned short cnt, int a, int b)
{
    // SIG_SAVE_CLIENT_HACK_LOG（0x12 字节）：uid/type/cnt/a/b。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("HackAnalyzer.cpp", 0x249), true);
    **guard << 0x9a;
    **guard << (int)-1;
    SIG_SAVE_CLIENT_HACK_LOG* sig =
        guard->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>();
    sig->m_uid = uid;
    sig->m_type = type;
    sig->m_cnt = cnt;
    sig->m_a = a;
    sig->m_b = b;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

void CHackAnalyzer::reportHackInfo()
{
    if (m_hackInfoList.empty())
        return;
    for (std::vector<stHackInfo_t>::iterator it = m_hackInfoList.begin();
         it != m_hackInfoList.end(); ++it)
    {
        ReqDBSaveHackInfo(it->m_uid, it->m_hackType, it->m_cnt, it->m_time, 1);
    }
}

void CHackAnalyzer::resetHackInfo()
{
    m_hackInfoList.clear();
    m_flag04 = 0;
    m_baseHackType = 0;
    m_checkTypes.clear();
    m_field20 = 0;
    m_field24 = 0;
    m_field7a08 = 0;
}

void CHackAnalyzer::onUpdateHackCount(CUser* user, ENUM_HACKTYPE type,
                                      unsigned int a, unsigned int b,
                                      unsigned int c)
{
    user->onUpdateHackCount(type, (int)a, b, c);
}

void CHackAnalyzer::addServerHackCnt(CUser* user, ENUM_HACKTYPE type,
                                     unsigned int count, unsigned int a,
                                     unsigned int b)
{
    int t = (int)type;
    if (t < 0 || 0xfa1 < t)
    {
        int base = (int)getBaseHackType();
        int etc = user->SetETC(0x34e, base);
        addServerHackCnt(user, (ENUM_HACKTYPE)0x34e, count,
                         (unsigned int)etc, 0u);
        setBaseHackType((ENUM_HACKTYPE)0);
        return;
    }

    if (!CheckExceptionCnt(type, (int)count))
        return;
    // 账号盗用类且 B 表累计非 0 时跳过统计（ORIG bVar1 分支）。
    if (Secu_AccountHacking::checkAccountHackingHackType(type) &&
        hackCntB(this, t) != 0)
        return;


    CParty* party = (CParty*)user->GetParty();
    if (party != 0 && user->get_state() == 5)
    {
        int memberCount = party->get_member_count();
        if (!CheckHackRateByPartyCount(type, memberCount))
            return;
        // ORIG：party+0xcac 为 CDungeon*，party+0xb24 为 CBattle_Field。
        char* pc = (char*)party;
        CDungeon* dungeon = *(CDungeon**)(pc + 0xcac);
        if (dungeon != 0)
        {
            int lv = ((CUserCharacInfo*)user)->get_charac_level();
            int stdLv = dungeon->get_standard_level();
            int lvDiff = (lv - stdLv < 0) ? 0 : lv - stdLv;
            int diff = ((CBattle_Field*)(pc + 0xb24))->get_dungeon_diff();
            if (!CheckHackRateByDifficult(type, lvDiff, diff))
                return;
        }
    }

    if (t != 0x25b && t != 0x25c && t != 0x25d && t != 0x25e && t != 0x25f &&
        t != 0x260 && t != 0x325 && t != 0x6f)
    {
        hackCntA(this, t) += (int)count;
        hackCntB(this, t) += (int)count;
        onUpdateHackCount(user, type, count, a, b);
    }
}



}  // namespace WongWork
// Secu_AccountHacking::checkAccountHackingHackType —— ORIG 0x08278a48（0x4f 字节）：
// 纯静态判定：hacktype ∈ {0x2c6..0x2cb, 0x2cd} 返回 true，否则 false。
bool Secu_AccountHacking::checkAccountHackingHackType(WongWork::ENUM_HACKTYPE type)
{
    const int t = static_cast<int>(type);
    if (t == 0x2c6 || t == 0x2c7 || t == 0x2c8 ||
        t == 0x2c9 || t == 0x2ca || t == 0x2cb || t == 0x2cd)
        return true;
    return false;
}