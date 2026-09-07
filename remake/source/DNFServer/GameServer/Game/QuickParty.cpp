// df_game_r 还原 —— QuickParty 独立实现（batch-5 迁移自 GameStubs.cpp）。
// 本批（S4）：CQuickParty ctor/init、CQuickPartySystemManager ctor/
// init_quick_party_pool/cancel_quick_party 按权威报告实现。
// remove_quick_party_in_pool / push_quick_party_in_pool / destroy 等仍由后续批次实现。

#include "QuickParty.h"
#include "CGameManager.h"

#include <vector>

#include "LogManager.h"

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CDungeon.h"
#include "GameWorld.h"   // send_party_info_to_all / G_GameWorld


// CGameManager.h 目前与 CBattle_Field.h 组合存在重定义冲突（其它批次修复中），
// 此处按 ORIG 符号做最小声明（_ZN12CGameManager13GetQuickPartyEi /
// _ZN12CGameManager26GetQuickPartyRewardManagerEv，非虚函数，链接解析到真实定义）。
#include "CQuickPartyRewardManager.h"
// [dedup] CGameManager 权威定义见 CGameManager.h（namespace WongWork），本 TU 本地桩已删。
CGameManager* G_CGameManager();   // CDungeon.h 同声明

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以
// m_battleField[0x158] 等 pad 占位、未引入 CBattle_Field 类型；Main 裁定
// 本组 TU 局部视图，与 CParty.cpp 口径一致）。布局：pad 0xb24 + BF 0xcd0。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

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
    memset(m_partyIndex, -1, sizeof(m_partyIndex));   // ORIG memset(this,-1,0x10)
    m_quickPartyIndex = -1;
    m_firstMatchingCnt = 0;
    m_dungeonIndex = 0;
    m_dungeonDiff = (char)0xff;
    m_poolIndex = (char)0xff;
    for (int msg = 0x9d; msg <= 0x9e; ++msg)
    {
        int idx = msg - 0x9d + 4;
        *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x0c +
                                idx * 4) = 0;
    }
    m_isChangeQuickPartyCount = 0;
}

void CQuickParty::set_quick_party_index(int idx) { m_quickPartyIndex = idx; }

short CQuickParty::get_dungeon_index() { return m_dungeonIndex; }
int CQuickParty::get_dungeon_diff() { return static_cast<signed char>(m_dungeonDiff); }

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
    // ORIG 0x082695ca 逐条转写：PutQuickParty 归还后，取活跃 QUICK 파티数
    // （hash_map::size）与 pool 占用数（Size()-Remain()），cMyTrace(0x32,0) 输出。
    G_CGameManager()->PutQuickParty(this);
    CGameManager* mgr = G_CGameManager();
    int registeredCnt = mgr->GetActiveQuickPartyCnt();          // ebx: map size
    int occupiedCnt = mgr->GetOccupiedQuickPartyPoolCnt();      // esi: Size-Remain
    cMyTrace trace("void QuickParty::CQuickParty::destroy()", 0x32, 0);
    // ORIG fmt @0x8bf5380（GBK）：
    // "QUICK_PARTY_LOG : QUICK 快捷组队 解除。 QUICK 快捷队<%d>, 剩余 QUICK 快捷队<%d>"
    trace("QUICK_PARTY_LOG : QUICK \xc6\xc4\xc6\xbc\xc7\xd8\xc1\xa6 \xb5\xcb\xb4\xcf\xb4\xd9. QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>, \xb5\xee\xb7\xcf QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>",
          occupiedCnt, registeredCnt);
}

// [moved] GetActiveQuickPartyCnt/GetOccupiedQuickPartyPoolCnt 定义移至 CGameManager.cpp（避免陷入 namespace QuickParty）。

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
        // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
        CBattle_Field* bf =
            &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField;
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
    return m_isChangeQuickPartyCount;
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
    int idx = static_cast<int>(msg) - 0x9d;
    int* slot = reinterpret_cast<int*>(
        reinterpret_cast<char*>(this) + 0x0c + (idx + 4) * 4);
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
    if (it != m_poolMap.end())
    {
        int qpIndex = qp->get_quick_party_index();
        it->second.m_lists[poolIdx].remove(qpIndex);
        return true;
    }

    qp->destroy();
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x2ba, 5);
    trace("QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
    return false;
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
            __PRETTY_FUNCTION__,
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
    bool changed = false;
    int totalCount = qp->get_quick_party_count();
    int firstCnt = static_cast<int>(qp->get_first_matching_cnt());
    if (firstCnt < totalCount ||
        static_cast<char>(qp->is_change_quick_party_count()) != '\0')
    {
        changed = true;
    }
    qp->send_matching_update_quick_party(static_cast<char>(count), masterUid,
                                         changed);
    return true;
}

}  // namespace QuickParty

// ===================== TailB 批：CQuickParty 访问器 + 系统管理器匹配/开局链 =====================
// 报告：docs/class_func_reports/QuickParty__CQuickParty{,SystemManager}/ 各同名 md。

namespace QuickParty
{

// ---- get_timer_key @0x08269668：读 timer key 计数数组槽（与 gen_timer_key 同址）----
int CQuickParty::get_timer_key(TIMER_MESSAGE msg)
{
    return *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(this) + 0x0c +
        (static_cast<int>(msg) - 0x9d + 4) * 4);
}

// ---- check_timer_key @0x082696b0：槽值与给定 key 相等判断 ----
bool CQuickParty::check_timer_key(TIMER_MESSAGE msg, int key)
{
    return *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(this) + 0x0c +
        (static_cast<int>(msg) - 0x9d + 4) * 4) == key;
}

// ---- set_party_index @0x0826978c：写入首个空槽（-1），全满则丢弃 ----
void CQuickParty::set_party_index(int partyIdx)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_partyIndex[i] == -1)
        {
            m_partyIndex[i] = partyIdx;
            return;
        }
    }
}

// ---- set_party_index @0x082697c8：定位写，idx∈[0,3] 才生效 ----
void CQuickParty::set_party_index(int idx, int partyIdx)
{
    if (idx < 4 && idx >= 0)
        m_partyIndex[idx] = partyIdx;
}

// ---- get_party_object() @0x0826972c：返回首个登记且仍存在的成员 party ----
CParty* CQuickParty::get_party_object()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] == -1)
            continue;
        CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
        if (party != 0)
            return party;
    }
    return 0;
}

// ---- delete_party_index() @0x08269856：全部槽位清 -1 ----
void CQuickParty::delete_party_index()
{
    for (int i = 0; i < 4; ++i)
        m_partyIndex[i] = -1;
}

// ---- is_empty_quick_party @0x08269884：所有槽均为 -1 即为空 ----
bool CQuickParty::is_empty_quick_party()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] != -1)
            return false;
    }
    return true;
}

// ---- set_first_matching_cnt @0x0826b75a（W 内联）：直写 +0x14 字节 ----
void CQuickParty::set_first_matching_cnt(char cnt)
{
    m_firstMatchingCnt = static_cast<unsigned char>(cnt);
}

// ---- find_quick_party_in_pool @0x0826a9fe：
// 从档位 list 逐个弹出 index 取 CQuickParty；取不到的记 trace 继续弹。----
CQuickParty* CQuickPartySystemManager::find_quick_party_in_pool(
    std::list<int>& pool)
{
    while (!pool.empty())
    {
        int qpIndex = pool.front();
        pool.pop_front();
        CQuickParty* qp = G_CGameManager()->GetQuickParty(qpIndex);
        if (qp != 0)
            return qp;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x29c, 5);
        trace("QUICK_PARTY_LOG : NO INTENDED ROUTINE!!");
    }
    return 0;
}

// ---- pop_quick_party_in_pool @0x0826ae64：
// 档位 0 直接领新池对象；其余档位先从池 list 弹已有 quick party，
// 弹空则回退为领新池对象并按 firstMatchingCnt 复位计数。----
CQuickParty* CQuickPartySystemManager::pop_quick_party_in_pool(
    int& outCount, STQuickPartyPoolMap_Data& data, int slotIdx,
    int partyIdx, int firstMatchingCnt)
{
    if (slotIdx > 3 || slotIdx < 0)
        return 0;

    CQuickParty* qp = 0;
    if (slotIdx == 0)
    {
        qp = G_CGameManager()->GetQuickParty();
        if (qp == 0)
            return 0;
        qp->set_party_index(partyIdx);
        outCount = firstMatchingCnt;
        qp->set_first_matching_cnt(static_cast<char>(firstMatchingCnt));
        return qp;
    }

    qp = find_quick_party_in_pool(data.m_lists[slotIdx]);
    if (qp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x352, 5);
        trace("QUICK_PARTY_LOG : NO INTENDED ROUTINE!!");
        qp = G_CGameManager()->GetQuickParty();
        if (qp == 0)
            return 0;
        qp->set_party_index(partyIdx);
        outCount = firstMatchingCnt;
        qp->set_first_matching_cnt(static_cast<char>(firstMatchingCnt));
        return qp;
    }

    qp->set_party_index(partyIdx);
    outCount = qp->get_quick_party_count();
    return qp;
}

// ---- regist_quick_party @0x0826adb6：弹池→压池闭环 ----
bool CQuickPartySystemManager::regist_quick_party(
    const STQuickPartyPoolMap_Key& key, STQuickPartyPoolMap_Data& data,
    int slotIdx, CParty* party, int firstMatchingCnt)
{
    if (party == 0)
        return false;

    int outCount = 0;
    CQuickParty* qp = pop_quick_party_in_pool(outCount, data, slotIdx,
                                              party->GetPartyIndex(),
                                              firstMatchingCnt);
    if (qp == 0)
        return false;
    // P197-N-cap：push_quick_party_in_pool 返 bool，`==1` 被 fold（无物化）；改 exec 返回类型属越界，记 N-cap。
    return push_quick_party_in_pool(key, data, party, outCount, qp, true);
}

// ---- check_match_proper_party @0x08269e34：匹配前置校验，返回 ENUM_ERROR ----
int CQuickPartySystemManager::check_match_proper_party(
    const STQuickPartyPoolMap_Key& key, CParty* party)
{
    if (party == 0)
        return 1;
    int members = party->get_member_count();
    if (members <= 0 || members > 3)
        return 1;

    short dungeonIdx = static_cast<short>(key.m_dungeonIndex);
    if (dungeonIdx < 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x16e, "QUICK_PARTY_LOG : DUNGEON INDEX ERROR!! (%d)",
            static_cast<int>(dungeonIdx));
    }

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return 1;

    // 血色副本按人数上限直接放行；否则走常规出入副本条件检查。
    bool bloodPass =
        static_cast<int>(
            static_cast<signed char>(dungeon->get_blood_dungeon_type())) == 2 &&
        static_cast<int>(dungeon->get_limit_party_count()) <= members;
    if (bloodPass)
        return 1;
    return party->checkInoutConditionDungeon(dungeon,
                                             static_cast<ENUM_DUNGEON_MODE>(0),
                                             static_cast<ENUM_CMDPACKET>(0x1bc));
}

// ---- check_quick_party_min_count @0x0826ab9e：开局最低人数校验 ----
bool CQuickPartySystemManager::check_quick_party_min_count(
    const STQuickPartyPoolMap_Key& key, int count)
{
    int dungeonIdx = static_cast<int>(static_cast<short>(key.m_dungeonIndex));
    int diff =
        static_cast<int>(static_cast<signed char>(key.m_dungeonDiff));   // ORIG movsbl 32 位存储
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return false;

    if (dungeon->get_blood_dungeon_type() == 2)
    {
        if (static_cast<int>(
                static_cast<signed char>(dungeon->get_limit_party_count())) !=
            count)
            return false;
    }
    else if (static_cast<signed char>(dungeon->get_dimension_possible()) > 0)
    {
        // 次元可入副本：允许维度最低人数 + 1。
        if (count <
            static_cast<int>(
                static_cast<signed char>(dungeon->get_dimension_min_partymem())) +
                1)
            return false;
    }
    else
    {
        // 次元属性关闭：查 QuickParty.etc 的 minCount 表
        //（表键 = dungeon+0x89c 入场限制组合位非零 ? 1 : 0）。
        int requireMin = -1;
        int tableRow = -1;
        if (dungeon->m_89c != 0)
            tableRow = 1;
        else
            tableRow = 0;
        if (G_CGameManager()->GetQuickPartyRewardManager() != 0)
            requireMin =
                G_CGameManager()->GetQuickPartyRewardManager()
                    ->getRequireQuickPartyMinCount(tableRow, diff);
        if (requireMin != -1 && requireMin > count)
            return false;
        if (count < 2)
            return false;
    }
    return true;
}

// ---- match_proper_party @0x08269f3c：为新 party 匹配同图快速队伍 ----
int CQuickPartySystemManager::match_proper_party(
    const STQuickPartyPoolMap_Key& key, CParty* party, CUser* user)
{
    bool abnormal =
        (party == 0) || (party->get_quick_party_index() != -1);
    if (abnormal)
    {
        if (party == 0 && user != 0)
        {
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x186, "QUICK_PARTY_LOG : PARTY IS NULL!! (%d)",
                reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacNo());
        }
        else
        {
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x188, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY INDEX (%d)!!",
                party ? party->get_quick_party_index() : 0);
        }
        return 0x12;
    }

    int err = check_match_proper_party(key, party);
    if (err != 0)
        return err;

    int memberCount = party->get_member_count();
    CDungeon* dungeon =
        G_CDataManager()->find_dungeon(static_cast<int>(key.m_dungeonIndex));
    if (dungeon == 0)
        return 1;

    int maxMembers = 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        maxMembers = static_cast<int>(dungeon->get_limit_party_count());

    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    if (it == m_poolMap.end())
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1d6, 5);
        trace("QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
        return 1;
    }
    STQuickPartyPoolMap_Data& data = it->second;

    // 从"还差 memberCount 人即可开团"的档位向下尝试归并。
    for (int need = maxMembers - memberCount; need > 0; --need)
    {
        if (data.m_lists[need].empty())
            continue;

        if (need + memberCount != maxMembers)
        {
            // 尚不足开局人数：并入现有等待队列（注册新组合）。
            if (!regist_quick_party(key, data, need, party, memberCount))
                return 1;
            return 0;
        }

        // 正好补齐：弹出该档 quick party 并立即尝试开局。
        CQuickParty* qp = find_quick_party_in_pool(data.m_lists[need]);
        if (qp == 0)
            continue;

        qp->set_party_index(party->GetPartyIndex());
        int total = qp->get_quick_party_count();
        if (total == maxMembers)
        {
            start_game(qp);
            return 0;
        }

        unsigned int qpIndex =
            static_cast<unsigned int>(qp->get_quick_party_index());
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1b4, 0);
        trace("QUICK_PARTY_LOG : REPUSH QUICK PARTY IN POOL!! QUICK_PARTY_INDEX(%d), QUICK_PARTY_COUNT(%d)",
              qpIndex, total);
        if (!push_quick_party_in_pool(key, data, party, total, qp, true))
        {
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x1bc, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY!! (%d)", total);
            return 1;
        }
        return 0;
    }

    // 无可归并组合：以空队首领新池对象挂起等待。
    if (!regist_quick_party(key, data, 0, party, memberCount))
        return 1;
    return 0;
}

// ---- make_quick_party @0x0826b222：把池内 quick party 合并成一个真实 party ----
CParty* CQuickPartySystemManager::make_quick_party(CQuickParty* qp)
{
    std::list<CUser*> detached;
    if (qp == 0)
        return 0;

    // 选 host：优先 EPLP 等待态>1 的队伍，否则任一 quick party 属性队伍。
    CParty* host = 0;
    for (int i = 0; i < 4; ++i)
    {
        int partyIdx = qp->get_party_index(i);
        CParty* member = G_CGameManager()->GetParty(partyIdx);
        if (member == 0)
            continue;
        if (member->GetEPLPState() > 1)
        {
            host = member;
            break;
        }
        if (member->is_quick_party())
            host = member;
    }
    if (host == 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            host = G_CGameManager()->GetParty(qp->get_party_index(i));
            if (host != 0)
                break;
        }
        if (host == 0)
        {
            qp->destroy();
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x3d0, "QUICK_PARTY_LOG : PARTY IS NULL!!");
            return 0;
        }
    }

    host->set_state(1);
    host->set_quick_party(true);
    host->setDungIndex(1);
    host->setDungDiffi(0);
    if (host->IsAutoCreated())
    {
        host->SetSinglePlay(false);
        host->SetAutoCreated(false);
        G_GameWorld()->send_party_info_to_all(host, 0);
    }

    // 其余成员队伍解散，把用户依次收拢（记录进 detached）。
    for (int i = 0; i < 4; ++i)
    {
        int partyIdx = qp->get_party_index(i);
        if (partyIdx == -1)
            continue;
        CParty* member = G_CGameManager()->GetParty(partyIdx);
        if (member == 0 || member == host)
            continue;
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!member->checkValidUser(slot))
                continue;
            CUser* user = member->get_user(slot);
            member->delete_party_for_quick_party(user, detached);
        }
    }

    for (std::list<CUser*>::iterator it = detached.begin();
         it != detached.end(); ++it)
    {
        host->join_user(*it);
    }

    qp->destroy();
    host->init_quick_party_data();
    return host;
}

// ---- insert_timer_waiting_start_game @0x0826ad50：
// InsertTimer(objType=1, partyIdx, msg=0x2d, period=0x5a, key, msg)。 ----
void CQuickPartySystemManager::insert_timer_waiting_start_game(CParty* party)
{
    unsigned int key =
        static_cast<unsigned int>(party->gen_timer_key((TIMER_MESSAGE)0x2d));
    int partyIdx = party->GetPartyIndex();
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, partyIdx,
                                (TIMER_MESSAGE)0x2d, 0x5a, key,
                                static_cast<TIMER_MESSAGE>(0x2d));
}

// ---- start_game @0x0826b58a：合并成团并广播、计时器等待后真正开局 ----
void CQuickPartySystemManager::start_game(CQuickParty* qp)
{
    if (qp == 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x40c, "QUICK_PARTY_LOG : QUICK PARTY IS NULL!!");
        return;
    }
    if (qp->get_quick_party_count() <= 1)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x412,
            "QUICK_PARTY_LOG : NO START!! BECAUSE OF QUICK PARTY COUNT 1 !!");
        return;
    }

    CParty* merged = make_quick_party(qp);
    if (merged == 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x41e, "QUICK_PARTY_LOG : NO EXIST PARTY!!");
        return;
    }

    insert_timer_waiting_start_game(merged);
    merged->send_party_realtime_info();
    merged->send_party_ipinfo();
    G_GameWorld()->send_party_info_to_all(merged, 2);
    merged->send_quick_party_matching_result(true, 0);
    if (merged->GetEPLPState() <= 1)
    {
        merged->game_start(merged->getManager());
    }
}

// ---- start_game @0x0826a972：带 key 版（先校验人数再出池）。----
bool CQuickPartySystemManager::start_game(CQuickParty* qp,
                                          const STQuickPartyPoolMap_Key& key)
{
    if (qp == 0)
        return false;
    if (!check_quick_party_min_count(key, qp->get_quick_party_count()))
        return false;
    if (!G_CGameManager()->GetQuickPartySystemManager()
             ->remove_quick_party_in_pool(qp, key))
        return false;
    start_game(qp);
    return true;
}

// ---- direct_entrance_quick_party @0x0826a762：队长请求直接入场 ----
int CQuickPartySystemManager::direct_entrance_quick_party(CUser* user)
{
    if (user == 0)
        return 1;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return 1;

    CQuickParty* qp =
        G_CGameManager()->GetQuickParty(party->get_quick_party_index());
    if (qp == 0)
        return 1;

    int count = qp->get_quick_party_count();
    signed char firstMatchingCnt = 0;
    {
        char raw = qp->get_first_matching_cnt();
        firstMatchingCnt = static_cast<signed char>(raw);
    }
    // 队伍未满且正好等于首次匹配人数：直接拒绝入场。
    if (count < 2 && firstMatchingCnt == count)
        return 0xf5;

    // 已到匹配目标人数且无人数变动：解除队列并通知散队。
    if (firstMatchingCnt >= count && !qp->is_change_quick_party_count())
    {
        CParty* target = qp->get_party_object();
        if (target == 0)
            return 1;
        if (G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(
                target, true, 0))
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0x17f);
            packet.finalize(true);
            target->send_to_party(packet);
            return 1;
        }
    }

    STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = qp->get_dungeon_index();
    key.m_dungeonDiff = static_cast<unsigned char>(qp->get_dungeon_diff());
    G_CGameManager()->GetQuickPartySystemManager()->start_game(qp, key);
    return 0;
}

}  // namespace QuickParty
