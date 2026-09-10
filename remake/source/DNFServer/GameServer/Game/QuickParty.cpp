// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 QuickParty/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "QuickParty/CQuickParty.inc"
// ===================== STQuickPartyPoolMap_Data =====================

#include "QuickParty/STQuickPartyPoolMap_Data.inc"
{
    // [b28d-L1] 修法：ORIG 0x0826b8b7..0x0826b8e7 为**展开的 4 次**
    // `mov 0x8(%ebp),%eax; [add $K,%eax;] mov %eax,(%esp); call list::clear`
    // （无循环变量/无回边）；原 for 形使 4.4 发 `movl $0,-0x1c` + setle/jne
    // 回边并把帧撑到 0x3c（ORIG 0x2c）、EH 槽 -0x2c（ORIG -0x1c）。展开后逐条对齐。
    m_lists[0].clear();
    m_lists[1].clear();
    m_lists[2].clear();
    m_lists[3].clear();
}

#include "QuickParty/dtor_STQuickPartyPoolMap_Data.inc"

// ===================== CQuickParty =====================
// 布局依据 ORIG init @0x8269546 与内联访问器 0x826b74e..0x826b807。

#include "QuickParty/dtor_CQuickParty.inc"

#include "QuickParty/get_quick_party_index.inc"

#include "QuickParty/init.inc"
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

#include "QuickParty/set_quick_party_index.inc"

#include "QuickParty/get_dungeon_index.inc"
#include "QuickParty/get_dungeon_diff.inc"

#include "QuickParty/set_change_quick_party_count.inc"
{
    m_isChangeQuickPartyCount = flag ? 1 : 0;
}

// delete_party_index @0x0826980e：遍历 m_partyIndex[0..3]，命中则置 -1 并返回。
#include "QuickParty/delete_party_index.inc"
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
// [b28d-L1] IDENTICAL/AE（routed 口径 check_tu_routed）：ORIG 0x082698be 逐条对齐（40/40，P167 CLEAR）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 39=39）；P167 CLEAR（0 missing/0 extra）。
// [L3FLinkClosureQuickParty 2026-09-11] ORIG 两个 `je` 落点不同：+0x23 `je +0x5f`（i++ 语句）
// 与 +0x46 `je +0x5e`（= 循环体末尾 nop，被 continue 引用）。⇒ ORIG 外层为正面 if 包裹
// （`if (m_partyIndex[i] != -1) {...}`），内层才是 `if (party == 0) continue;`。
// 原写法把外层写成 continue，令 je#1 落到 nop、je#2 落到 i++，两处落点互换（40 vs 40 但形不同）。
#include "QuickParty/get_quick_party_count.inc"
{
    int total = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] != -1)
        {
            CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
            if (party == 0)
                continue;
            // ORIG：get_member_count 结果落具名槽 -0xc 后再累加
            int memberCount = party->get_member_count();
            total += memberCount;
        }
    }
    return total;
}

#include "QuickParty/destroy.inc"
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
    cMyTrace(__PRETTY_FUNCTION__, 0x32, 0)(
        "QUICK_PARTY_LOG : QUICK \xc6\xc4\xc6\xbc\xc7\xd8\xc1\xa6 \xb5\xcb\xb4\xcf\xb4\xd9. QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>, \xb5\xee\xb7\xcf QUICK \xc6\xc4\xc6\xbc\xbc\xf6<%d>",
        G_CGameManager()->m_quickPartyPool.Size() -
            static_cast<int>(G_CGameManager()->m_quickPartyPool.Remain()),
        static_cast<int>(G_CGameManager()->m_quickPartyMap.size()));
}

// [moved] GetActiveQuickPartyCnt/GetOccupiedQuickPartyPoolCnt 定义移至 CGameManager.cpp（避免陷入 namespace QuickParty）。

// ===================== CQuickPartySystemManager =====================

#include "QuickParty/CQuickPartySystemManager.inc"
{
    // ORIG @0x8269bb6：map 构造后显式 clear()。
    m_poolMap.clear();
}

#include "QuickParty/dtor_CQuickPartySystemManager.inc"

// init_quick_party_pool @0x08269c0c：
// 该 dungeon 已登记于快速派对奖励副本表 → 直接返回；
// 非绝望之塔副本时，按 4 个难度档位向池 map 插入空数据
// （血/任务 NPC 副本仅插入档位 0）。
// [b28d-L1] 修法（P167/P96）：ORIG 0x08269c0c 只调 `__gnu_cxx::ne<const int*>`（两侧 const_iterator）；非 const 引用使 4.4 多发 begin/end+转换构造+混合 ne(PKi,Pi)（EXTRA×4/MISSING×1）。改 const 引用后迭代器差集清零；残余帧差 0x90 vs 0xa0（N82/P232）。
// [b28d-solo-QuickParty] verdict=AE（163=163，TU routed 口径；P167 CLEAR）：修法=
// P214① 正极性早返 `if (dungeon->isTowerOfDespairDungeon()) return;`（复现 ORIG
// 0x08269cc6 `test %al,%al; jne <nop 岛>`，消 `xor $0x1` 反形）+ 三条件正向 if 包裹
// （ORIG 0x08269d3a or 链物化后 `test %al,%al; je <插入块>` 直落）+ blood 条件
// `(signed char)` 化复现 `test %al,%al; jg`。旧「N96/P292 xor 不可翻」判词作废。
#include "QuickParty/init_quick_party_pool.inc"
{
    if (dungeon == 0)
        return;

    // [b28d-solo-QuickParty] 修法（P0/P24）：ORIG 0x08269c31/0x08269c58 两次调用
    // get_dungeon_idx_list()（begin/end 各一次），无 idxList 引用槽；引用局部使 4.4
    // 多发 `mov %eax,-0x14` 槽往返并把帧撑到 0xa0（ORIG 0x90）。
    std::vector<int>::const_iterator it =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()
                ->get_dungeon_idx_list()).begin();
    std::vector<int>::const_iterator end =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()
                ->get_dungeon_idx_list()).end();
    for (; it != end; ++it) {
        if (dungeon->get_index() == *it)
            return;                                     // 已是 quick party 奖励副本
    }

    if (dungeon->isTowerOfDespairDungeon())
        return;

    {
        STQuickPartyPoolMap_Key key;
        STQuickPartyPoolMap_Data data;

        key.m_dungeonIndex = (short)dungeon->get_index();
        for (int i = 0; i < 4; ++i) {
            // [b28d-solo-QuickParty] 修法（P214① 正向包裹 + P223 变体）：
            // ORIG 0x08269cc6 `test %al,%al; jne <nop 岛>`（正极性早返，非 xor 形），
            // 0x08269d3a 的 or 链物化后 `test %al,%al; je <插入块>` 直落（非 xor 反形）。
            if ((dungeon->getDungeonKind() == 1) ||
                (static_cast<signed char>(dungeon->get_blood_dungeon_type()) > 0) ||
                (dungeon->getQuestNpcDungeon() != 0)) {
                if (i != 0)
                    goto next_diff;
            }
            key.m_dungeonDiff = (signed char)i;
            m_poolMap.insert(std::make_pair(key, data));
        next_diff: ;
        }
    }
}

// cancel_quick_party @0x0826a386：把 party 从其所属 CQuickParty 中注销，
// 剩余成员>1 且非解散(flag=true)时重新入池；EPLP 等待态且 flag 时重掷等待 timer。
// [b28d-L1] 修法（P167）：ORIG 0x0826a386 三处 get_quick_party_index（无 qpIndex 具名局部）——①短路条件 ②日志三元 ③GetQuickParty(内联实参)。原 qpIndex 缓存使 P167 报 MISSING×1；改为表达式直读后差集归零。残余为帧/槽形态差（N82）。
// [b28d-solo-QuickParty] verdict=AE（243=243，TU routed 口径；P167 CLEAR）：修法=
// ①`int count = 0;` 声明预清（ORIG 0x0826a586 在条件前 `movl $0,-0xc`）；
// ②删日志三元 party 判空（P171 照抄 ORIG 直调 get_quick_party_index）；
// ③EPLP 段去 bf/buffType 缓存（P48/P183：ORIG 两次独立 `mov party,%edx; add $0xb24`，
// getRandomBuffDungeonType 结果直传 setEplpWaitingTimer）。旧 N82 判词作废。
#include "QuickParty/cancel_quick_party.inc"
{
    // [b28d-L1] 修法（P167）：ORIG 37 calls 三处 get_quick_party_index（无
    // qpIndex 具名局部）：①短路条件 `party==0 || party->get_quick_party_index()==-1`
    // ②日志三元 ③`GetQuickParty(party->get_quick_party_index())` 内联实参。
    // 原形用 qpIndex 缓存使 4.4 少一次调用（P167 报 MISSING×1）+ 多一槽。
    if (party == 0 || party->get_quick_party_index() == -1) {
        if (party == 0 && user != 0) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1e3,
                                  "QUICK_PARTY_LOG : PARTY IS NULL!! (%d)",
                                  reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacNo());  // ORIG: CUserCharacInfo::getCurCharacNo((CUserCharacInfo*)user)
        } else {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1e5,
                                  "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY INDEX (%d)!!",
                                  party->get_quick_party_index());
        }
        return 0;
    }

    CQuickParty* qp =
        G_CGameManager()->GetQuickParty(party->get_quick_party_index());
    if (qp == 0) {
        LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1ec,
                              "QUICK_PARTY_LOG : NO EXIST QUICK PARTY!!");
        return 0;
    }

    if (party->GetEPLPState() > 1 && flag) {
        party->SetSelectedEPLPCmd((char)-1);            // ORIG 写 -1（0x826a4db）
        // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）；
        // P48/P183：ORIG 两次独立 `mov party,%edx; add $0xb24,%edx`（无 bf 缓存），
        // getRandomBuffDungeonType 结果直传 setEplpWaitingTimer（无 buffType 槽）。
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.setEplpWaitingTimer(
            (RandomBuffDungeon)
                reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.getRandomBuffDungeonType(
                    qp->get_dungeon_index()));
    }

    STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = qp->get_dungeon_index();
    key.m_dungeonDiff = qp->get_dungeon_diff();

    if (!remove_quick_party_in_pool(qp, key))
        return 0;

    if (!flag)
        qp->set_change_quick_party_count(true);

    int count = 0;                                    // ORIG 0x826a586 预清
    if (party->get_member_count() == 1 || flag) {
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

#include "QuickParty/get_party_index.inc"
{
    if (idx > 3 || idx < 0)
        return -1;
    return m_partyIndex[idx];
}

#include "QuickParty/set_dungeon_index.inc"
#include "QuickParty/set_dungeon_diff.inc"

#include "QuickParty/get_pool_index.inc"
{
    return static_cast<int>(static_cast<signed char>(m_poolIndex));
}

#include "QuickParty/set_pool_index.inc"

#include "QuickParty/get_first_matching_cnt.inc"
{
    return static_cast<char>(m_firstMatchingCnt);
}

#include "QuickParty/is_change_quick_party_count.inc"
{
    return m_isChangeQuickPartyCount;
}

// get_party_object @0x082696d0：idx∈[0,3] 且槽非 -1 时经 GameManager 取 party。
#include "QuickParty/get_party_object.inc"
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
#include "QuickParty/gen_timer_key.inc"
{
    // ORIG：计数数组 @+0x1c，槽下标 (int)msg-0x9d，先自增再返回新值
    return ++m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
}

// get_matching_quick_party_master_unique_id @0x08269936：
// 首选 EPLP 状态 >1 的成员 party；否则任一 quick party；否则首个存在的 party；
// 返回其 manager 的 unique_id（无则 0xffff）。
// [b28d-L1] N82 封顶：ORIG 0x08269936；P167 CLEAR（13/13）⇒ 语义完成，残余为帧/槽位形态差（ORIG sub 0x48 vs 4.4 push ebx+sub 0x34）。
// [b28d-solo-QuickParty] verdict=AE（104=104）：修法=P0 第二循环/第三段具名 partyIdx 落槽
// （内联实参使 4.4 升格 %ebx 跨 G_CGameManager()）+ party 循环外声明 `= 0`（ORIG -0x28
// 槽 + movl $0 初始化）+ P171 删 mgr 判空（ORIG 0x08269a75 无 test）。旧 N82 判词作废。
#include "QuickParty/get_matching_quick_party_master_unique_id.inc"
{
    int slot = -1;
    unsigned short uid = 0xffff;
    // [b28d-solo-QuickParty] 修法（N93/P212）：ORIG 槽序 party=-0x28 深于 i=-0x24/
    // partyIdx=-0x20 ⇒ 原码 party 为循环外声明（循环内仅赋值）；循环内声明使 4.4
    // 把它排到 i/partyIdx 之上（-0x20）。
    CParty* party = 0;
    for (int i = 0; i <= 3; ++i)
    {
        int partyIdx = get_party_index(i);
        party = G_CGameManager()->GetParty(partyIdx);
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
            // [b28d-solo-QuickParty] 修法（P0）：ORIG 0x082699ef 具名 partyIdx 落 -0x18 槽
            // 后传 GetParty；内联实参使 4.4 升格 %ebx 跨 G_CGameManager() 保活（push %ebx
            // + 帧 0x34、PartyGuard 槽整体漂移）。
            int partyIdx = get_party_index(i);
            if (G_CGameManager()->GetParty(partyIdx) != 0)
            {
                slot = i;
                break;
            }
        }
    }
    if (slot != -1 && slot >= 0 && slot <= 3)
    {
        // [b28d-solo-QuickParty] 修法（P0）：ORIG 0x08269a4a 具名 partyIdx 落 -0x14 槽。
        int partyIdx = get_party_index(slot);
        CParty* party = G_CGameManager()->GetParty(partyIdx);
        if (party != 0)
        {
            // [b28d-solo-QuickParty] 修法（P171 反向照抄）：ORIG 0x08269a75 无 mgr 判空
            // （getManager 结果直接 `mov -0xc,%eax; call get_unique_id`）。
            CUser* mgr = party->getManager();
            uid = static_cast<unsigned short>(mgr->get_unique_id());
        }
    }
    return uid;
}

// send_matching_update_quick_party @0x08269a8e：对每个存在的成员 party 发包
// 0x180：byte(poolIdx)、short(masterUid)、byte(changed)。
#include "QuickParty/send_matching_update_quick_party.inc"
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
// [b28d-L1] N82附 封顶：ORIG 0x0826acea 首值 gen_timer_key→%ebx、get_quick_party_index→%esi；4.4 恒 esi/ebx 对调（裸内联实参/P226 语句表达式两形实测均不可翻），P167 CLEAR（4/4）语义等价。
// [b28d-solo-QuickParty] verdict=NEAR 封顶（28=28，仅 gen_timer_key→%esi /
// get_quick_party_index→%ebx 与 ORIG 的 %ebx/%esi 角色互换；P167 CLEAR）。本轮实测：
// ①裸内联/双 register/具名 uint key 恒互换（具名 uint 落槽 -0xc 反劣 29 条）；
// ②`register unsigned int key` + 内联 index 令 gen→%ebx 但 4.4 多发 `mov %ebx,%esi`
// 中转（29 条，N18a 反向）；③4.4.6 后端（-B cc1plus446bin）复测同样互换 ⇒ 非 N7
// 后端敏感族，属 N82附「多实参 call 首值 callee-saved 对调」封顶。
#include "QuickParty/insert_timer_waiting_party_matching.inc"
    CQuickParty* qp)
{
    // ORIG：gen_timer_key→%ebx、get_quick_party_index→%esi callee-saved 缓存（P24 内联实参）。
    // b28c-lane3 判 N82附：裸内联实参 4.4 恒 esi/ebx 对调，裸语句表达式形（P226）
    // 实测无效（仍 esi/ebx）⇒ 多实参 call 首值 callee-saved 对调不可复现。
    // [L3FLinkClosureQuickParty 2026-09-11] 复核：补测 `(int)qp->gen_timer_key(...)`
    // 显式转换形亦无效（仍 gen→%esi / index→%ebx，28/28）⇒ N82附 封顶确认。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                qp->get_quick_party_index(),
                                (TIMER_MESSAGE)0x9e, 0x14,
                                qp->gen_timer_key((TIMER_MESSAGE)0x9e), 0x9e);
}

// remove_quick_party_in_pool —— ORIG 0x0826aa9a。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/remove_quick_party_in_pool.md
// pool index ∈ [0,3)；map 未命中 → destroy + trace 并返回 false；
// 命中 → 从对应档位 list 移除 quick party index 并返回 true。
// [b28d-L1] N93/N98 封顶：ORIG 0x0826aa9a 槽序 it=-0x28/end 临时=-0x24/qpIndex=-0x20；块内声明使 4.4 把 qpIndex 排最深。P212 全头部声明已把 it 翻正 -0x28（ndiff 52→46），残余 end 临时槽与 qpIndex 互换——声明序/具名 e 两形实测（r19c/r19d）只交换槽位，4.4 按首次使用序分配不可翻。
// [b28d-solo-QuickParty] verdict=AE（71=71，TU routed 口径；P167 CLEAR）：修法=
// qpIndex 具名局部改内联实参 `m_lists[poolIdx].remove(qp->get_quick_party_index())`
// ——ORIG 0x0826ab40 的 qpIndex 槽实为 list::remove(const int&) 绑定的匿名临时
// （-0x20），end() sret 临时落 -0x24；原具名局部使 4.4 把两者槽号互换。旧
// N93/N98 封顶判词作废。
#include "QuickParty/remove_quick_party_in_pool.inc"
    CQuickParty* qp, const STQuickPartyPoolMap_Key& key)
{
    if (qp == 0)
        return false;

    char poolIdx = qp->get_pool_index();
    if (poolIdx >= 4 || poolIdx < 0)
        return false;

    // [b28d-L1] 修法（P212 全头部声明，ndiff 52→46 为四形最优）：ORIG 槽序
    // it=-0x28 / end 临时=-0x24 / qpIndex=-0x20。块内声明 qpIndex 时 4.4 把它排到
    // 最深 -0x28、it/end 各上移一格；把 qpIndex 提到 it 之前声明（块内仅赋值）后
    // it 回到 -0x28（与 ORIG 全等），残余仅 end 临时槽 -0x20 与 qpIndex -0x24 互换
    // ——4.4 按首次使用序分配（N98），end 条件先于块内赋值使用，声明序/具名 e 两形
    // 实测（/tmp/b28d_l1/r19c.md 52、r19d.md 48）均只交换槽位，判 N93/N98 封顶。
    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    if (it != m_poolMap.end())
    {
        it->second.m_lists[poolIdx].remove(qp->get_quick_party_index());
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
// [b28d-L1] N82/N-XVAL 封顶：ORIG 0x0826afb4；P167 CLEAR（25/25）。残余 = xvalue 实参存回读往返 + 帧槽 -0x2c→-0x30 位移（b23 判词复核仍成立）。
// [b28d-solo-QuickParty] verdict=AE（171=171）：修法=内联 xvalue 实参
// `push_back(static_cast<int&&>(qp->get_quick_party_index()))`，消 4.4 的
// `mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x20` 存回读往返与 PacketGuard
// -0x2c→-0x30 位移（旧 N82/N-XVAL 判词作废）。P167 CLEAR（25/25）。
#include "QuickParty/push_quick_party_in_pool.inc"
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

    // [b28d-solo-QuickParty] 修法：ORIG 0x0826b155 只有单槽 `call get_quick_party_index;
    // mov %eax,-0x20(%ebp); …; lea -0x20(%ebp),%edx; call push_back(int&&)`；具名
    // qpIndex 使 4.4 多发 `mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x20`（+2 条 +
    // 多占 -0x24 槽把 PacketGuard 挤到 -0x30）。改内联 xvalue 实参后单槽直落。
    data.m_lists[count].push_back(
        static_cast<int&&>(qp->get_quick_party_index()));

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
#include "QuickParty/get_timer_key.inc"
{
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
}

// ---- check_timer_key @0x082696b0：槽值与给定 key 相等判断 ----
// [b28d-L1] N7/N18b 封顶：ORIG 0x082696b0 为 `mov 0xc(%eax,%edx,4),%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx`（两寄存器）；4.4 恒折 `mov …,%eax; cmp 0x10(%ebp),%eax`（内存直比，11 vs 12 条）。unsigned/signed、key 置左/置右、&0xff 掩码四形实测同折。
// [b28d-solo-QuickParty] verdict=AE（12=12）：修法=P248 档②字段侧——m_timerKeyCnt
// int[2]→unsigned int[2]（QuickParty.h，同宽零布局影响）+ `elem == (unsigned int)key`；
// 探针 /tmp/b28d_solo/probe_ck.cpp g0/g4 逐条全等（ORIG 两寄存器 12 条）。旧 N7/N18b 判词作废。
#include "QuickParty/check_timer_key.inc"
{
    // b28c-lane3：ORIG 0x082696c2 元素→%edx、0x082696c6 key→%eax、
    // `cmp %eax,%edx`（12 条）；无 cast 时 4.4 折成 `cmp key_mem,%eax`（11 条，
    // N18b）。两侧同 cast 到 unsigned int 后 4.4 走「两寄存器操作数」形。
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d] == (unsigned int)key;
}

// ---- set_party_index @0x0826978c：写入首个空槽（-1），全满则丢弃 ----
#include "QuickParty/set_party_index.inc"
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
#include "QuickParty/set_party_index_1.inc"
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
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x082696d0；P167 CLEAR（2/2）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 33=33）；P167 CLEAR。
// [L3FLinkClosureQuickParty 2026-09-11] ORIG 0x0826972c +0x1c `je +0x46`（i++ 语句，
// 该处无 nop；函数共 33 条，尾块 `mov $0,%eax` 在 +0x55）⇒ ORIG 用正面 if 包裹
// （`if (m_partyIndex[i] != -1) {...}`），无 continue ⇒ 4.4 不生成「循环体末尾 nop」。
// 原 continue 形多出 1 条 nop（34 vs 33）。
#include "QuickParty/get_party_object_1.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] != -1)
        {
            CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
            if (party != 0)
                return party;
        }
    }
    return 0;
}

// ---- delete_party_index() @0x08269856：全部槽位清 -1 ----
#include "QuickParty/delete_party_index_1.inc"
{
    for (int i = 0; i < 4; ++i)
        m_partyIndex[i] = -1;
}

// ---- is_empty_quick_party @0x08269884：所有槽均为 -1 即为空 ----
#include "QuickParty/is_empty_quick_party.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] != -1)
            return false;
    }
    return true;
}

// ---- set_first_matching_cnt @0x0826b75a（W 内联）：直写 +0x14 字节 ----
#include "QuickParty/set_first_matching_cnt.inc"
{
    m_firstMatchingCnt = cnt;
}

// ---- find_quick_party_in_pool @0x0826a9fe：
// 从档位 list 逐个弹出 index 取 CQuickParty；取不到的记 trace 继续弹。----
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x0826a9fe；P167 CLEAR（7/7）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 41=41）；P167 CLEAR。
// [L3FLinkClosureQuickParty 2026-09-11] 负结论（ORIG 0x0826aa7b `nop` @+0x7d，循环条件
// +0x7e 之前，1 条、无任何跳转引用）：三形实测均不可复现 —— ①`continue;`（无条件，
// 42/42，无 nop）；②空语句 `;`（42/42，无 nop）；③`for (; !pool.empty(); )` 空增量
// （42/42，无 nop）。⇒ 4.1 在循环体末尾/增量位置 emit nop 而 4.4 不 emit（版本差异，
// 非源码形）。AE 口径（剥离 nop）已达标；保留 while 原形。
#include "QuickParty/find_quick_party_in_pool.inc"
    QuickParty::LIST_QUICK_PARTY& pool)
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
#include "QuickParty/pop_quick_party_in_pool.inc"
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
#include "QuickParty/regist_quick_party.inc"
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
// [b28d-L1] N82 封顶：ORIG 0x08269e34；P167 CLEAR（7/7 调用点差集归零）⇒ 语义完成，残余为 4.1/4.4 槽位与寄存器分配差（real diff 58 全为 disp/reg 形态）。
// [b28d-solo-QuickParty] verdict=AE（73=73）：修法=P0/P35 删 dungeonIdx 具名 short 局部
// （三处重读 key.m_dungeonIndex：test %ax / cwtl / movswl %ax,%ebx）+ P226 匿名语句
// 表达式复现 bloodPass 零槽物化（旧 N82 判词作废）。P167 CLEAR（7/7）。
#include "QuickParty/check_match_proper_party.inc"
    const STQuickPartyPoolMap_Key& key, CParty* party)
{
    if (party == 0)
        return static_cast<ENUM_ERROR>(1);
    int members = party->get_member_count();
    if (members <= 0 || members > 3)
        return static_cast<ENUM_ERROR>(1);

    // [b28d-solo-QuickParty] 修法（P0/P35）：ORIG 0x08269e6f 无 dungeonIdx 具名槽——
    // 每处 `movzwl (%eax),%eax` 重读 key.m_dungeonIndex（条件 test %ax、日志参 cwtl、
    // find_dungeon 实参 movswl %ax,%ebx）；具名 short 局部多 `mov %ax,-0x12` 槽往返。
    if (key.m_dungeonIndex < 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x16e, "QUICK_PARTY_LOG : DUNGEON INDEX ERROR!! (%d)",
            static_cast<int>(key.m_dungeonIndex));
    }

    CDungeon* dungeon = G_CDataManager()->find_dungeon(key.m_dungeonIndex);
    if (dungeon == 0)
        return static_cast<ENUM_ERROR>(1);

    // 血色副本按人数上限直接放行；否则走常规出入副本条件检查。
    // [b28d-solo-QuickParty] 修法（P226 语句表达式）：ORIG 0x08269ee6 的 bloodPass 物化在
    // %al（`mov $1/jmp`、`mov $0`、`test %al,%al`），无栈槽；具名 bool 局部使 4.4 落 -0x9
    // 并多发 `mov %al,-0x9; cmpb $0,-0x9`。
    if (({
            static_cast<int>(
                static_cast<signed char>(dungeon->get_blood_dungeon_type())) == 2 &&
            static_cast<int>(dungeon->get_limit_party_count()) <= members;
        }))
        return static_cast<ENUM_ERROR>(1);
    return static_cast<ENUM_ERROR>(party->checkInoutConditionDungeon(
                                             dungeon,
                                             static_cast<ENUM_DUNGEON_MODE>(0),
                                             static_cast<ENUM_CMDPACKET>(0x1bc)));
}

// ---- check_quick_party_min_count @0x0826ab9e：开局最低人数校验 ----
#include "QuickParty/check_quick_party_min_count.inc"
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
// [b28d-L1] 修法（P167/P220/P197④）：ORIG 0x08269f3c 调 ne×1 + list::size×1（无 eq/empty）⇒ 改正极性 `if (it != end) { 主体 }` + `size()==0`。差集仅余 N89（4.1 不内联 operator-> 发 ptEv×4）。残余槽位差 N82。
// [b28d-solo-QuickParty] verdict=AE（279=279，TU routed 口径；P167 CLEAR）：修法=
// ①去 `STQuickPartyPoolMap_Data& data = it->second` 引用缓存，每处重发 `it->second`
//   （P183；4.4 亦发 ptEv，N89 调用点差集消）；
// ②档位判空改正极性 `size() != 0` 包裹（P220 setne 形）；
// ③then/else 交换为 `if (need+memberCount == maxMembers)` 正极性（P222）；
// ④qp 两句式预清（ORIG 0x0826a128 `movl $0,-0x10`）；
// ⑤qpIndex 内联进 cMyTrace 实参（P227 全表达式临时链，%ebx 跨 ctor 保活）；
// ⑥`if (total != maxMembers) { trace+push } start_game(qp); return 0;` 块序（N93附）；
// ⑦traceMiss 改原位全表达式临时链。旧 N89/N82 判词作废。
#include "QuickParty/match_proper_party.inc"
    const STQuickPartyPoolMap_Key& key, CParty* party, CUser* user)
{
    // [b28d-solo-QuickParty] 修法（P226）：ORIG 0x08269f4a 的 abnormal 物化在 %al
    // （mov $1/mov $0/test %al），无栈槽；具名 bool 使 4.4 发 `mov %al,-0x2d; cmpb $0,-0x2d`。
    if (({
            (party == 0) || (party->get_quick_party_index() != -1);
        }))
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
                party->get_quick_party_index());   // [b28d-solo-QuickParty] P171 照抄：
                                                   // ORIG 0x08269f7x 无 party 判空直接调用
        }
        return static_cast<ENUM_ERROR>(0x12);
    }

    int err = check_match_proper_party(key, party);
    if (err != 0)
        return static_cast<ENUM_ERROR>(err);

    int memberCount = party->get_member_count();
    CDungeon* dungeon =
        G_CDataManager()->find_dungeon(static_cast<int>(key.m_dungeonIndex));
    if (dungeon == 0)
        return static_cast<ENUM_ERROR>(1);

    int maxMembers = 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        maxMembers = static_cast<int>(dungeon->get_limit_party_count());

    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    // [b28d-L1] 修法（P167/P220）：ORIG 34 calls 调 `_Rb_tree_iterator::ne`×1 +
    // `list::size`×1（无 eq/empty）⇒ 原码是正极性 `if (it != end) { 主体 }`；
    // 且档位判空写 `size() == 0` 而非 `empty()`（P197④）。原 eq/empty 形
    // P167 报 EXTRA eq/empty、MISSING ne/size。改写后调用点差集仅余 N89
    // （4.1 不内联 operator-> 发 ptEv×4，4.4 内联，结构性豁免）。
    if (it != m_poolMap.end())
    {
        // 从"还差 memberCount 人即可开团"的档位向下尝试归并。
        // [b28d-solo-QuickParty] 修法（P183/P220）：ORIG 0x0826a0xx 每处重发
        // `lea it; call ptEv; ...`（无 data 引用缓存），且档位判空为正极性
        // `size()!=0`（setne;test;je 直落 body）。
        for (int need = maxMembers - memberCount; need > 0; --need)
        {
            if (it->second.m_lists[need].size() != 0)
            {
                if (need + memberCount == maxMembers)
                {
                    // 正好补齐：弹出该档 quick party 并立即尝试开局。
                    // ORIG 0x0826a128 预清 qp（movl $0,-0x10）后赋值。
                    CQuickParty* qp = 0;
                    qp = find_quick_party_in_pool(it->second.m_lists[need]);
                    if (qp == 0)
                        continue;

                    qp->set_party_index(party->GetPartyIndex());
                    int total = qp->get_quick_party_count();
                    // [b28d-solo-QuickParty] 修法（P222/N93附）：ORIG 0x0826a18c
                    // `cmp -0x18; je <后置 start_game 块>`（相等时跳过 trace 块）
                    // ⇒ 源码须负极性包 trace 块、start_game 直落后置。
                    if (total != maxMembers)
                    {
                        // [b28d-solo-QuickParty] 修法（P0/P227）：ORIG 0x0826a1a0
                        // 的 get_quick_party_index 结果入 %ebx 跨 cMyTrace ctor
                        // 保活（无具名槽）⇒ 实参内联，勿声明 qpIndex 局部。
                        cMyTrace(__PRETTY_FUNCTION__, 0x1b4, 0)(
                            "QUICK_PARTY_LOG : REPUSH QUICK PARTY IN POOL!! QUICK_PARTY_INDEX(%d), QUICK_PARTY_COUNT(%d)",
                            static_cast<unsigned int>(qp->get_quick_party_index()),
                            total);
                        if (!push_quick_party_in_pool(key, it->second, party,
                                                      total, qp, true))
                        {
                            LogManager::logFormat(
                                1, "QuickParty.cpp",
                                __PRETTY_FUNCTION__,
                                0x1bc, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY!! (%d)", total);
                            return static_cast<ENUM_ERROR>(1);
                        }
                        return static_cast<ENUM_ERROR>(0);
                    }
                    start_game(qp);
                    return static_cast<ENUM_ERROR>(0);
                }
                else
                {
                    // 尚不足开局人数：并入现有等待队列（注册新组合）。
                    if (!regist_quick_party(key, it->second, need, party,
                                            memberCount))
                        return static_cast<ENUM_ERROR>(1);
                    return static_cast<ENUM_ERROR>(0);
                }
            }
        }

        // 无可归并组合：以空队首领新池对象挂起等待。
        if (!regist_quick_party(key, it->second, 0, party, memberCount))
            return static_cast<ENUM_ERROR>(1);
        return static_cast<ENUM_ERROR>(0);
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x1d6, 5)(
        "QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
    return static_cast<ENUM_ERROR>(1);
}

// ---- make_quick_party @0x0826b222：把池内 quick party 合并成一个真实 party ----
// [b28d-L1] N82 封顶：ORIG 0x0826b222；P167 CLEAR（37/37）⇒ 语义完成，残余为槽位/帧形态差（real diff 264 全为 disp/reg）。
// [b28d-solo-QuickParty] verdict=AE（239=239，TU routed 口径；P167 CLEAR）：修法=
// ①第一循环 member 提至函数头、按 ORIG 槽序（member -0x30 深于 host -0x2c）声明
//   `CParty* member; CParty* host = 0;`；
// ②第二循环补具名 partyIdx（ORIG -0x1c）；第三循环 member 两句式预清
//   （ORIG 0x0826b430 `movl $0,-0x10`）+ user 内联实参（ORIG 0x0826b490 无 user 槽）；
// ③尾迭代器循环拆 `iterator it = begin(); iterator itEnd = end();` 后置比较
//   （复现 ORIG it=-0x3c/itEnd=-0x40/detached=-0x38 槽组）。旧 N82/N95 判词作废。
#include "QuickParty/make_quick_party.inc"
{
    if (qp == 0)
        return 0;

    // [b28d-solo-QuickParty] 修法（P0/声明序）：ORIG 0x0826b22a 先 `cmpl $0x0,0xc(%ebp);
    // jne` 再构造 detached（0x0826b23a 后）⇒ 原码 detached 声明在 qp 判空之后；提前
    // 声明使 4.4 把 detached 排深 4 并整体平移全帧槽号。
    std::list<CUser*> detached;

    // 选 host：优先 EPLP 等待态>1 的队伍，否则任一 quick party 属性队伍。
    CParty* member;
    CParty* host = 0;
    for (int i = 0; i < 4; ++i)
    {
        int partyIdx = qp->get_party_index(i);
        member = G_CGameManager()->GetParty(partyIdx);
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
            int partyIdx = qp->get_party_index(i);
            host = G_CGameManager()->GetParty(partyIdx);
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
        CParty* member = 0;
        member = G_CGameManager()->GetParty(partyIdx);
        if (member == 0 || member == host)
            continue;
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!member->checkValidUser(slot))
                continue;
            member->delete_party_for_quick_party(member->get_user(slot),
                                                 detached);
        }
    }

    std::list<CUser*>::iterator it = detached.begin();
    std::list<CUser*>::iterator itEnd = detached.end();
    for (; it != itEnd; ++it)
    {
        host->join_user(*it);
    }

    qp->destroy();
    host->init_quick_party_data();
    return host;
}

// ---- insert_timer_waiting_start_game @0x0826ad50：
// InsertTimer(objType=1, partyIdx, msg=0x2d, period=0x5a, key, msg)。 ----
#include "QuickParty/insert_timer_waiting_start_game.inc"
{
    // ORIG：gen_timer_key→%esi、GetPartyIndex→%ebx callee-saved 缓存（P24 内联实参）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x2d, 0x5a,
                                party->gen_timer_key((TIMER_MESSAGE)0x2d),
                                static_cast<TIMER_MESSAGE>(0x2d));
}

// ---- start_game @0x0826b58a：合并成团并广播、计时器等待后真正开局 ----
// [b28d-L1] N 封顶（P223 尾岛）：ORIG 0x0826b58a 尾段 `setle;test;je <nop 岛>; …; jmp <出口>; nop`；4.4 对 if(c){body} 恒 jump-thread 掉尾 jmp+nop（81 vs 80）。b28c-lane3 已试 P223 goto 岛/P225 while+break，本轮复核不翻。
// [b28d-solo-QuickParty] verdict=AE（82=82）：修法=P285——正向 if 体后补空 `else{return;}`，
// 4.4 即保留体尾 `jmp <leave>; nop` 岛（旧「P223 尾岛不可翻」判词作废）。P167 CLEAR（14/14）。
#include "QuickParty/start_game.inc"
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
    else
    {
        return;
    }
}

// ---- start_game @0x0826a972：带 key 版（先校验人数再出池）。----
#include "QuickParty/start_game_1.inc"
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
#include "QuickParty/direct_entrance_quick_party.inc"
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
