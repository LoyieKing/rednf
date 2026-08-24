// df_game_r 还原 —— QuickParty 独立实现（batch-5 迁移自 GameStubs.cpp）。
// 本批（S4）：CQuickParty ctor/init、CQuickPartySystemManager ctor/
// init_quick_party_pool/cancel_quick_party 按权威报告实现。
// remove_quick_party_in_pool / push_quick_party_in_pool / destroy 等仍由后续批次实现。

#include "QuickParty.h"

#include <vector>

#include "LogManager.h"

#include "CBattle_Field.h"
#include "CDungeon.h"


// CGameManager.h 目前与 CBattle_Field.h 组合存在重定义冲突（其它批次修复中），
// 此处按 ORIG 符号做最小声明（_ZN12CGameManager13GetQuickPartyEi /
// _ZN12CGameManager26GetQuickPartyRewardManagerEv，非虚函数，链接解析到真实定义）。
#include "CQuickPartyRewardManager.h"
class CGameManager
{
public:
    QuickParty::CQuickParty* GetQuickParty(int index);
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
    CParty* GetParty(int partyIndex);   // ORIG _ZN12CGameManager8GetPartyEi
    void PutQuickParty(QuickParty::CQuickParty* qp);  // ORIG _ZN12CGameManager14PutQuickPartyEPN10QuickParty11CQuickPartyE
};
CGameManager* G_CGameManager();   // CDungeon.h 同声明

namespace QuickParty
{

// ===================== STQuickPartyPoolMap_Data =====================

STQuickPartyPoolMap_Data::STQuickPartyPoolMap_Data()
{
    for (int i = 0; i < 4; ++i)
        m_lists[i].clear();
}

STQuickPartyPoolMap_Data::~STQuickPartyPoolMap_Data() {}

// ===================== CQuickParty =====================
// 布局依据 ORIG init @0x8269546 与内联访问器 0x826b74e..0x826b807。

CQuickParty::CQuickParty() { init(); }
CQuickParty::~CQuickParty() {}

int CQuickParty::get_quick_party_index() const { return m_quickPartyIndex; }

void CQuickParty::init()
{
    memset(m_partyIndex, 0xff, sizeof(m_partyIndex));   // ORIG memset(this,-1,0x10)
    m_quickPartyIndex = -1;
    m_firstMatchingCnt = 0;
    m_dungeonIndex = 0;
    m_dungeonDiff = (char)0xff;
    m_poolIndex = (char)0xff;
    m_field1c = 0;                                      // ORIG 循环清零 +0x1c/+0x20
    m_field20 = 0;
    m_isChangeQuickPartyCount = 0;
}

void CQuickParty::set_quick_party_index(int idx) { m_quickPartyIndex = idx; }

short CQuickParty::get_dungeon_index() { return m_dungeonIndex; }
char CQuickParty::get_dungeon_diff() { return m_dungeonDiff; }

void CQuickParty::set_change_quick_party_count(bool flag)
{
    m_isChangeQuickPartyCount = flag ? 1 : 0;
}

// delete_party_index @0x0826980e：遍历 m_partyIndex[0..3]，命中则置 -1 并返回。
void CQuickParty::delete_party_index(int partyIdx)
{
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] == partyIdx) {
            m_partyIndex[i] = -1;
            return;
        }
    }
}

// get_quick_party_count @0x082698be：累加各登记 party 的当前成员数。
int CQuickParty::get_quick_party_count()
{
    int total = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] == -1)
            continue;
        CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
        if (party != 0)
            total += party->get_member_count();
    }
    return total;
}

// destroy @0x082695ca：归还 StaticPool 槽位并输出释放日志（cMyTrace 细节按推断简化）。
void CQuickParty::destroy()
{
    G_CGameManager()->PutQuickParty(this);
}

// ===================== CQuickPartySystemManager =====================

CQuickPartySystemManager::CQuickPartySystemManager()
{
    // ORIG @0x8269bb6：map 构造后显式 clear()。
    m_poolMap.clear();
}

CQuickPartySystemManager::~CQuickPartySystemManager() {}

// init_quick_party_pool @0x08269c0c：
// 该 dungeon 已登记于快速派对奖励副本表 → 直接返回；
// 非绝望之塔副本时，按 4 个难度档位向池 map 插入空数据
// （血/任务 NPC 副本仅插入档位 0）。
void CQuickPartySystemManager::init_quick_party_pool(const CDungeon* dungeon)
{
    if (dungeon == 0)
        return;

    std::vector<int>& idxList =
        G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list();
    for (std::vector<int>::const_iterator it = idxList.begin(); it != idxList.end(); ++it) {
        if (dungeon->get_index() == *it)
            return;                                     // 已是 quick party 奖励副本
    }

    if (!dungeon->isTowerOfDespairDungeon()) {
        STQuickPartyPoolMap_Key key;
        STQuickPartyPoolMap_Data data;

        key.m_dungeonIndex = (short)dungeon->get_index();
        for (int i = 0; i < 4; ++i) {
            bool special = (dungeon->getDungeonKind() == 1)
                || (dungeon->get_blood_dungeon_type() > 0)
                || (dungeon->getQuestNpcDungeon() != 0);
            if (!special || i == 0) {
                key.m_dungeonDiff = (unsigned char)i;
                m_poolMap.insert(std::make_pair(key, data));
            }
        }
    }
}

// cancel_quick_party @0x0826a386：把 party 从其所属 CQuickParty 中注销，
// 剩余成员>1 且非解散(flag=true)时重新入池；EPLP 等待态且 flag 时重掷等待 timer。
int CQuickPartySystemManager::cancel_quick_party(CParty* party, bool flag, CUser* user)
{
    int qpIndex = party ? party->get_quick_party_index() : -1;
    if (party == 0 || qpIndex == -1) {
        if (party == 0 && user != 0) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1e3,
                                  "QUICK_PARTY_LOG : PARTY IS NULL!! (%d)",
                                  reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacNo());  // ORIG: CUserCharacInfo::getCurCharacNo((CUserCharacInfo*)user)
        } else {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1e5,
                                  "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY INDEX (%d)!!",
                                  party ? party->get_quick_party_index() : 0);
        }
        return 0;
    }

    CQuickParty* qp = G_CGameManager()->GetQuickParty(qpIndex);
    if (qp == 0) {
        LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1ec,
                              "QUICK_PARTY_LOG : NO EXIST QUICK PARTY!!");
        return 0;
    }

    if (party->GetEPLPState() > 1 && flag) {
        party->SetSelectedEPLPCmd((char)-1);            // ORIG 写 -1（0x826a4db）
        // ORIG: ((CBattle_Field*)((char*)party + 0xb24)) —— 字段为 CParty 私有，按偏移访问
        CBattle_Field* bf =
            reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
        int buffType = bf->getRandomBuffDungeonType(qp->get_dungeon_index());
        bf->setEplpWaitingTimer((RandomBuffDungeon)buffType);
    }

    STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = qp->get_dungeon_index();
    key.m_dungeonDiff = qp->get_dungeon_diff();

    if (!remove_quick_party_in_pool(qp, key))
        return 0;

    if (!flag)
        qp->set_change_quick_party_count(true);

    int count;
    if (party->get_member_count() == 1 || flag) {
        count = 0;
        qp->delete_party_index(party->GetPartyIndex());
        count = qp->get_quick_party_count();
        party->init_quick_party_data();
        if (count <= 0) {
            qp->destroy();
            return 1;
        }
    } else {
        count = qp->get_quick_party_count() - 1;
        if (count <= 0) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x216,
                                  "QUICK_PARTY_LOG : QUICK PARTY COUNT ERROR!! (%d)", count);
            qp->delete_party_index(party->GetPartyIndex());
            party->init_quick_party_data();
            qp->destroy();
            return 0;
        }
    }

    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    if (it != m_poolMap.end()) {
        if (!push_quick_party_in_pool(key, it->second, 0, count, qp, true)) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x229,
                                  "QUICK_PARTY_LOG : PUSH ERROR!! (%d)", count);
            return 0;
        }
    }
    return 1;
}

}  // namespace QuickParty

// ===================== push/remove_quick_party_in_pool 批次 =====================
// 依赖头：TimerQueue / PacketGuard（ORIG 弱访问器均按反汇编直读字段）。
#include "TimerQueue.h"
#include "PacketGuard.h"

namespace QuickParty
{

// ---- 简单弱访问器（ORIG W 符号，objdump 直读实证） ----

int CQuickParty::get_party_index(int idx)
{
    if (idx > 3 || idx < 0)
        return -1;
    return m_partyIndex[idx];
}

void CQuickParty::set_dungeon_index(short idx) { m_dungeonIndex = idx; }
void CQuickParty::set_dungeon_diff(int diff) { m_dungeonDiff = (char)diff; }

int CQuickParty::get_pool_index()
{
    return static_cast<int>(static_cast<signed char>(m_poolIndex));
}

void CQuickParty::set_pool_index(int idx) { m_poolIndex = (char)idx; }

char CQuickParty::get_first_matching_cnt()
{
    return static_cast<char>(m_firstMatchingCnt);
}

bool CQuickParty::is_change_quick_party_count()
{
    return m_isChangeQuickPartyCount != 0;
}

// get_party_object @0x082696d0：idx∈[0,3] 且槽非 -1 时经 GameManager 取 party。
CParty* CQuickParty::get_party_object(int idx)
{
    if (idx < 0 || idx > 3)
        return 0;
    if (m_partyIndex[idx] == -1)
        return 0;
    return G_CGameManager()->GetParty(m_partyIndex[idx]);
}

// gen_timer_key @0x08269680：计数数组位于 +0x0c 起、槽下标 (msg-0x9d)+4；
// 先自增再返回新值。msg=0x9e 对应 +0x20 槽。
int CQuickParty::gen_timer_key(TIMER_MESSAGE msg)
{
    int* slot = reinterpret_cast<int*>(
        reinterpret_cast<char*>(this) + 0x0c +
        (static_cast<int>(msg) - 0x9d + 4) * 4);
    ++(*slot);
    return *slot;
}

// get_matching_quick_party_master_unique_id @0x08269936：
// 首选 EPLP 状态 >1 的成员 party；否则任一 quick party；否则首个存在的 party；
// 返回其 manager 的 unique_id（无则 0xffff）。
unsigned short CQuickParty::get_matching_quick_party_master_unique_id()
{
    int slot = -1;
    unsigned short uid = 0xffff;
    for (int i = 0; i <= 3; ++i)
    {
        int partyIdx = get_party_index(i);
        CParty* party = G_CGameManager()->GetParty(partyIdx);
        if (party == 0)
            continue;
        if (party->GetEPLPState() > 1)
        {
            slot = i;
            break;
        }
        if (party->is_quick_party())
            slot = i;
    }
    if (slot == -1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (G_CGameManager()->GetParty(get_party_index(i)) != 0)
            {
                slot = i;
                break;
            }
        }
    }
    if (slot != -1 && slot >= 0 && slot <= 3)
    {
        CParty* party = G_CGameManager()->GetParty(get_party_index(slot));
        if (party != 0)
        {
            CUser* mgr = party->getManager();
            if (mgr != 0)
                uid = static_cast<unsigned short>(mgr->get_unique_id());
        }
    }
    return uid;
}

// send_matching_update_quick_party @0x08269a8e：对每个存在的成员 party 发包
// 0x180：byte(poolIdx)、short(masterUid)、byte(changed)。
void CQuickParty::send_matching_update_quick_party(char poolIdx,
                                                   unsigned short masterUid,
                                                   bool changed)
{
    for (int i = 0; i <= 3; ++i)
    {
        CParty* party = get_party_object(i);
        if (party == 0)
            continue;
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x180);
        packet.put_byte(poolIdx);
        packet.put_short(masterUid);
        packet.put_byte(changed ? 1 : 0);
        packet.finalize(true);
        party->send_to_party(packet);
    }
}

// insert_timer_waiting_party_matching @0x0826acea：
// InsertTimer(objType=1, qpIndex, msg=0x9e, sec=0x14, key, 0x9e)。
void CQuickPartySystemManager::insert_timer_waiting_party_matching(
    CQuickParty* qp)
{
    unsigned int key = static_cast<unsigned int>(
        qp->gen_timer_key((TIMER_MESSAGE)0x9e));
    int idx = qp->get_quick_party_index();
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, idx,
                                (TIMER_MESSAGE)0x9e, 0x14, key, 0x9e);
}

// remove_quick_party_in_pool —— ORIG 0x0826aa9a。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/remove_quick_party_in_pool.md
// pool index ∈ [0,3)；map 未命中 → destroy + trace 并返回 false；
// 命中 → 从对应档位 list 移除 quick party index 并返回 true。
bool CQuickPartySystemManager::remove_quick_party_in_pool(
    CQuickParty* qp, const STQuickPartyPoolMap_Key& key)
{
    if (qp == 0)
        return false;

    char poolIdx = qp->get_pool_index();
    if (poolIdx >= 4 || poolIdx < 0)
        return false;

    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    if (it == m_poolMap.end())
    {
        qp->destroy();
        cMyTrace trace(
            "bool QuickParty::CQuickPartySystemManager::remove_quick_party_in_pool(QuickParty::CQuickParty*, const QuickParty::STQuickPartyPoolMap_Key&)",
            0x2ba, 5);
        trace("QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
        return false;
    }

    int qpIndex = qp->get_quick_party_index();
    it->second.m_lists[poolIdx].remove(qpIndex);
    return true;
}

// push_quick_party_in_pool —— ORIG 0x0826afb4。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/push_quick_party_in_pool.md
// 正常路径：写 qp 的 dungeon/pool 字段并挂入对应档位 list、按需插等待 timer、
// 广播 0x180 匹配更新包；异常路径（qp 空/档位越界）：逐 party 清数据并发 0x17f
// 解散包后 destroy，记日志返回 false。
bool CQuickPartySystemManager::push_quick_party_in_pool(
    const STQuickPartyPoolMap_Key& key, STQuickPartyPoolMap_Data& data,
    CParty* party, int count, CQuickParty* qp, bool flag)
{
    if (!(qp != 0 && count < 4 && 0 < count))
    {
        if (qp != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CParty* member = qp->get_party_object(i);
                if (member == 0)
                    continue;
                member->init_quick_party_data();
                PacketGuard packet;
                packet.clear();
                packet.put_header(0, 0x17f);
                packet.finalize(true);
                member->send_to_party(packet);
            }
            qp->destroy();
        }
        LogManager::logFormat(
            1, "QuickParty.cpp",
            "bool QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(const QuickParty::STQuickPartyPoolMap_Key&, QuickParty::STQuickPartyPoolMap_Data&, CParty*, int, QuickParty::CQuickParty*, bool)",
            899, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY!! REGIST POOL INDEX(%d)",
            count);
        return false;
    }

    qp->set_dungeon_index(key.m_dungeonIndex);
    qp->set_dungeon_diff(static_cast<int>(static_cast<char>(key.m_dungeonDiff)));
    qp->set_pool_index(count);

    if (party != 0)
        party->set_quick_party_index(qp->get_quick_party_index());

    int qpIndex = qp->get_quick_party_index();
    data.m_lists[count].push_back(qpIndex);

    if (flag)
        insert_timer_waiting_party_matching(qp);

    unsigned short masterUid =
        qp->get_matching_quick_party_master_unique_id();
    int totalCount = qp->get_quick_party_count();
    bool changed =
        (static_cast<int>(qp->get_first_matching_cnt()) < totalCount)
        || qp->is_change_quick_party_count();
    qp->send_matching_update_quick_party(static_cast<char>(count), masterUid,
                                         changed);
    return true;
}

}  // namespace QuickParty
