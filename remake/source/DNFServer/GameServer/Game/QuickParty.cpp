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
        m_timerKeyCnt[msg - 0x9d] = 0;
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
bool CQuickParty::delete_party_index(int partyIdx)
{
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] == partyIdx) {
            m_partyIndex[i] = -1;
            return true;
        }
    }
    return false;
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
        {
            // ORIG：get_member_count 结果落具名槽 -0xc 后再累加
            int memberCount = party->get_member_count();
            total += memberCount;
        }
    }
    return total;
}

void CQuickParty::destroy()
{
    // ORIG 0x082695ca：G_CGameManager 四独立重调（P48）+ 成员直调——
    // +0x2d4 hash_map::size→ebx；+0x1cc StaticPool Size→esi、Remain→eax、
    // sub ⇒ esi=Size-Remain（占用数）。原经 GetActive*/GetOccupied* wrapper
    // 遮蔽直调形（LinkClosureB GetDBHandle 同案），已展开。
    G_CGameManager()->PutQuickParty(this);
    // b28c-lane3：P227 全表达式临时链——ORIG 0x082695ca 把 size()→%ebx、
    // Size()→%esi、Remain() 结果直接相减（无 registeredCnt/occupiedCnt 具名槽）；
    // 具名局部使 4.4 落 -0x10/-0xc 两槽并多 4 条 mov，改临时链后复现。
    // ORIG fmt @0x8bf5380（GBK，objdump -s .rodata 实证逐字节一致）
    cMyTrace("void QuickParty::CQuickParty::destroy()", 0x32, 0)(
        "QUICK_PARTY_LOG : QUICK \xc6\xc4\xc6\xbc\xc7\xd8\xc1\xa6 \xb5\xcb\xb4\xcf\xb4\xd9. QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>, \xb5\xee\xb7\xcf QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>",
        G_CGameManager()->m_quickPartyPool.Size() -
            static_cast<int>(G_CGameManager()->m_quickPartyPool.Remain()),
        static_cast<int>(G_CGameManager()->m_quickPartyMap.size()));
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
bool CQuickPartySystemManager::cancel_quick_party(CParty* party, bool flag, CUser* user)
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
    if (m_partyIndex[idx] != -1)
    {
        // ORIG：GetParty 结果落具名槽 -0xc；m_partyIndex[idx] 两次完整下标读（P48/P52）
        CParty* party = G_CGameManager()->GetParty(m_partyIndex[idx]);
        if (party != 0)
            return party;
    }
    return 0;
}

// gen_timer_key @0x08269680：计数数组位于 +0x0c 起、槽下标 (msg-0x9d)+4；
// 先自增再返回新值。msg=0x9e 对应 +0x20 槽。
int CQuickParty::gen_timer_key(TIMER_MESSAGE msg)
{
    // ORIG：计数数组 @+0x1c，槽下标 (int)msg-0x9d，先自增再返回新值
    return ++m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
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
    // ORIG：gen_timer_key→%ebx、get_quick_party_index→%esi callee-saved 缓存（P24 内联实参）。
    // b28c-lane3 判 N82附：裸内联实参 4.4 恒 esi/ebx 对调，裸语句表达式形（P226）
    // 实测无效（仍 esi/ebx）⇒ 多实参 call 首值 callee-saved 对调不可复现。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                qp->get_quick_party_index(),
                                (TIMER_MESSAGE)0x9e, 0x14,
                                qp->gen_timer_key((TIMER_MESSAGE)0x9e), 0x9e);
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
    // b28c-lane3：P221 全表达式临时链（单次调用）。
    cMyTrace(__PRETTY_FUNCTION__, 0x2ba, 5)(
        "QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
    return false;
}

// push_quick_party_in_pool —— ORIG 0x0826afb4。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/push_quick_party_in_pool.md
// 正常路径：写 qp 的 dungeon/pool 字段并挂入对应档位 list、按需插等待 timer、
// 广播 0x180 匹配更新包；异常路径（qp 空/档位越界）：逐 party 清数据并发 0x17f
// 解散包后 destroy，记日志返回 false。
// [b23判词·N-XVAL] P167 调用点差集 CLEAR（25/25）：ORIG `list<int>::push_back(int&&)`
// （EOi）——源码以 static_cast<int&&>(qpIndex) 生成 xvalue 精确命中该重载（原 const& 形选错）。
// 残余 gap+2 = gcc 4.4 对 xvalue 实参多一次 `mov %eax,-N(%ebp); mov -N(%ebp),%eax` 存回读
// 往返 + 帧槽 -0x2c→-0x30 位移（N82），跨版本求值/帧结构不可源码复现，语义等价。
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
    // ORIG 存 qpIndex 于 -0x20(%ebp) 后 lea 取址传 push_back(int&&)（EOi）⇒ 源码
    // 以 static_cast<int&&>(qpIndex) 生成 xvalue 精确命中 list<int>::push_back(int&&)。
    data.m_lists[count].push_back(static_cast<int&&>(qpIndex));

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
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
}

// ---- check_timer_key @0x082696b0：槽值与给定 key 相等判断 ----
bool CQuickParty::check_timer_key(TIMER_MESSAGE msg, int key)
{
    // b28c-lane3：ORIG 0x082696c2 元素→%edx、0x082696c6 key→%eax、
    // `cmp %eax,%edx`（12 条）；无 cast 时 4.4 折成 `cmp key_mem,%eax`（11 条，
    // N18b）。两侧同 cast 到 unsigned int 后 4.4 走「两寄存器操作数」形。
    return (unsigned int)key ==
           (unsigned int)m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
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
    // b28c-lane3：P223 双空块岛复现。ORIG 0x082697cf `jg +0x16`→nop;jmp 岛、
    // 0x082697d5 `js +0x11`→nop 岛、成功路径尾 `jmp +4`；4.4 对
    // `if (idx < 4 && idx >= 0)` 恒 jump-thread 掉两岛（12 vs 14）。
    if (idx > 3)
        goto L1;
    if (idx < 0)
        goto L2;
    m_partyIndex[idx] = partyIdx;
    goto L3;
L1: ;
L2: ;
L3: ;
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
    m_firstMatchingCnt = cnt;
}

// ---- find_quick_party_in_pool @0x0826a9fe：
// 从档位 list 逐个弹出 index 取 CQuickParty；取不到的记 trace 继续弹。----
CQuickParty* CQuickPartySystemManager::find_quick_party_in_pool(
    std::list<int>& pool)
{
    CQuickParty* qp = 0;             // ORIG -0x10 头部 movl $0x0 初始化
    int qpIndex;                     // ORIG -0xc 深槽
    while (!pool.empty())
    {
        qpIndex = pool.front();
        pool.pop_front();
        qp = G_CGameManager()->GetQuickParty(qpIndex);
        // b28c-lane3：ORIG 0x0826aa43 `jne 0x96`（直跳函数尾 return 块）。
        // 循环内 `return qp` 会被 4.4 物化成 `je <trace>; mov -0x10,%eax; jmp`，
        // 改 break + 循环外 return 后 4.4 与 ORIG 同为 jne 直达尾块。
        if (qp != 0)
            break;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x29c, 5);
        trace("QUICK_PARTY_LOG : NO INTENDED ROUTINE!!");
    }
    return qp;
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
    else
    {
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
        }
        else
        {
            qp->set_party_index(partyIdx);
            outCount = qp->get_quick_party_count();
        }
    }
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
    if (!push_quick_party_in_pool(key, data, party, outCount, qp, true))
        return false;
    return true;
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
    // ORIG：gen_timer_key→%esi、GetPartyIndex→%ebx callee-saved 缓存（P24 内联实参）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x2d, 0x5a,
                                party->gen_timer_key((TIMER_MESSAGE)0x2d),
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

    CParty* merged = 0;
    merged = make_quick_party(qp);
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
    // b28c-lane3 判 N：ORIG 0x0826b6d1 `setle;test;je 826b6ef`（nop 岛）+
    // 0x0826b6ed `jmp 826b6f0`；4.4 对 `if(c){body}` 恒折掉尾 jmp+nop（81 vs 80）。
    // 已试 P223 goto 岛 / P225 while+break 两形均不产尾 jmp。
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
    // [b23S3C] ORIG 0x0826a762 `call get_first_matching_cnt` 出现两次（+0xb4/+0xd8，
    // 均紧跟 `mov -0x14(%ebp),%eax` 重取 qp），无 firstMatchingCnt 具名槽（P0/P48）
    // ⇒ 删缓存局部与 raw 中转块，改每用点重取 + (int) 直比 count。
    // 队伍未满且正好等于首次匹配人数：直接拒绝入场。
    if (count < 2 && static_cast<int>(qp->get_first_matching_cnt()) == count)
        return 0xf5;

    // 已到匹配目标人数且无人数变动：解除队列并通知散队。
    if (static_cast<int>(qp->get_first_matching_cnt()) >= count &&
        !qp->is_change_quick_party_count())
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
