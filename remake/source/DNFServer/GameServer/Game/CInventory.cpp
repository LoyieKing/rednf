#include "LogManager.h"
// df_game_r CInventory 还原（第八批 H3，2026-08-16）。
// 布局/语义以 docs/class_func_reports/CInventory.md 逐条对照（AE 口径）。
// 依赖类（Inven_Item/CItem/CUser/CAvatarItemMgr/CCreatureMgr 等）由
// H1/H2/后续批次提供，本 TU 仅声明引用。

#include "CInventory.h"

#include <stdlib.h>
#include <string.h>

#include "GameTypes.h"
#include "STItemScript.h"
#include "CCirculationStatistic.h"
#include "LogManager.h"
#include "CTitleBook.h"   // item_lock::CItemLock::MakeItemLockPacket（TSV）
#include "SigTypes.h"     // SIG_LOAD_*（GameWorld.h→CUser.h 依赖；须先于其 include）
#include "GameWorld.h"    // GameWorld::IsEnchantRevisionChannel/IsIntegratedPvPBaseChannel
#include "ServiceRestrictManager.h"   // ServiceRestrictManager::isRestricted（move_item 封印限制）
#include "Arad_DataManager.h"         // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "CSystemTime.h"              // CSystemTime::getCurSec（解封期限/头像剩余天数）
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper 静态邮件（insert_event_items 兜底）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（邮件标题/正文 key）
#include "CCubeStatistic.h"           // GetInstanceCubeStatistic/collectCubeStatistics（reason 8 统计）
#include <algorithm>                  // std::swap<int>/std::swap<INVEN_TYPE>

// GetIntegratedPvPItemAttr(3 参)/_getIntegratedPvPItemAttr 的自由函数（PvP 物品属性，
// ORIG 0x84fc651/0x84fc541）。1 参版本定义于 CDataManagerScripts.cpp，3 参版本见下方。
extern int _getIntegratedPvPItemAttr(const Inven_Item& item);

// 空槽占位 Inven_Item 全局（ORIG 0x943ddc0；定义于 CAccountCargo.cpp）
extern Inven_Item g_emptySlot;

// ============================================================================
// 三个自由辅助函数（ORIG 全局符号，Money 簇自由函数）
//   isGainedGoldFromDungeonReason  @0x850d32a (W)  : reason==4 || reason==0xf
//   write_log_gain_money           @0x84febd9 (T)  : AddGold + 流通统计跳表
//   write_log_use_money            @0x84fed9b (T)  : SubGold + 流通/Value统计跳表
// 语义依据 CInventory.md 逐条对照；amount<0 日志分支忠实 ORIG logFormat 串。
// 注：ORIG 各 case 额外调用 CUser::AddMoneyPlus/AddMoneyMinus(@0x868f266/
//   @0x868f310)，该符号尚未在源实现，此处省略以避免引入新未定义。
// ============================================================================
bool isGainedGoldFromDungeonReason(eMoneyAddReason reason)
{
    // ORIG 0x867d9a2：switch 形态（先 mov 参数到寄存器再 cmp/je 链）。
    switch ((int)reason)
    {
    case 4:
    case 0xf:
        return true;
    }
    return false;
}

void write_log_gain_money(eMoneyAddReason reason, int amount, CUser* user)
{
    // ORIG 0x84febd9：if (amount < 1) { if (amount < 0) logFormat } else { AddGold + switch }。
    // switch 体布局序 = ORIG 块序（jump table 实证）：case 4,0,3,0xf,0xa,0x1a。
    if (amount < 1)
    {
        if (amount < 0)
        {
            LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__, 0xd71,
                "write_log_gain_money() reason(%d), gold(%d), gold(%d)",
                reason, amount, user->get_charac_level());
        }
        // b28d-37：ORIG 0x84febea `jns 0x84fed94(nop 岛)` + log 分支 `jmp 0x84fed95`
        // ⇒ 内层 if 失败与 log 分支共享函数尾出口（P223 空岛形），改共享 return。
        return;
    }
    else
    {
        if (user != 0)
        {
            user->GetGoldControl()->AddGold((unsigned int)amount, reason);
        }
        switch ((int)reason)
        {
        case 4:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 0:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)2, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 3:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)3, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 0xf:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)6, user, (unsigned int)amount);
            break;
        case 0xa:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)7, user, (unsigned int)amount);
            break;
        case 0x1a:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0xe, user, (unsigned int)amount);
            // [b28c-l23 N 判词·4.1 break 跳板 vs 4.4 jump threading]
            // ORIG @0x84fed94 最后一个 case 的 call 之后仍发 `jmp <统一出口>`
            // （4.1 即便出口即下一条指令也保留 break 跳板），4.4 恒做 jump threading
            // 省掉该 jmp ⇒ our 111 vs orig 112，其后果连锁 8 处 jmp 目标偏移差。
            // 实测复形失败：把 `default: break;` 改为显式 `goto L_SWITCH_EXIT;` +
            // 块外空标签后 4.4 仍折叠（ndiff 9→9，零改善），已回退为自然 break 形。
            // 语义完全等价（末 case 落函数尾）。
            // [b28d-37 复核] 函数头 `if(amount<1){if(amount<0)log;}return;` 改共享
            // return 后已复现 ORIG 0x84fed94 nop 岛（our 111→113=ORIG 113）；仅剩
            // 末 case break 跳板 1 条 jmp 差（4.1 保留跳板 / 4.4 jump-threading）⇒
            // goto/空标签形仍不可翻，N 封顶（P223 边界：我方多出岛不可消，缺岛可补）。
        default:
            break;
        }
    }
}

void write_log_use_money(eMoneySubReason reason, int amount, CUser* user)
{
    // ORIG 0x84fed9b：先无条件取 charac_level 存局部，再 if(amount<1){if(<0)log}else{...}。
    // switch 体布局序 = ORIG 块序（jump table 实证）。
    // b28d-37 [N 封顶]：共享 return 后复现 ORIG nop 岛（our 302→304=ORIG 304）；
    // 仅剩末 case break 跳板 1 条 jmp（4.1 保留 / 4.4 jump-threading，goto+空标签形
    // 实测不可翻，同 write_log_gain_money）。
    int level = user->get_charac_level();
    if (amount < 1)
    {
        if (amount < 0)
        {
            LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__, 0xde9,
                "write_log_use_money() reason(%d), gold(%d), gold(%d)",
                reason, amount, level);
        }
        // b28d-37：同 write_log_gain_money——ORIG 0x84fee5? `jns <nop 岛>` 与 log
        // 分支共享函数尾出口（P223），改共享 return 复现 nop 岛。
        return;
    }
    else
    {
        if (user != 0)
        {
            user->GetGoldControl()->SubGold((unsigned int)amount, reason);
        }
        switch ((int)reason)
        {
        case 0:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x12, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0xa:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x13, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 1:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x14, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x14:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x16, user, (unsigned int)amount);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x18, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 6:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x18, user, (unsigned int)amount);
            break;
        case 0xd:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x19, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 4:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1a, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x10:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1b, user, (unsigned int)amount);
            break;
        case 0xb:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1e, user, (unsigned int)amount);
            break;
        case 0x13:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1f, user, (unsigned int)amount);
            break;
        case 0x16:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x20, user, (unsigned int)amount);
            break;
        case 0xc:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x21, user, (unsigned int)amount);
            break;
        case 0xf:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x23, user, (unsigned int)amount);
            break;
        case 0x15:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x24, user, (unsigned int)amount);
            break;
        case 0x22:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x25, user, (unsigned int)amount);
            break;
        case 0x21:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x27, user, (unsigned int)amount);
            break;
        case 0x24:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x28, user, (unsigned int)amount);
            break;
        case 0x2a:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x29, user, (unsigned int)amount);
            break;
        case 0x2b:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2b, user, (unsigned int)amount);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x1d, user, (unsigned int)amount);
            break;
        case 0x2c:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2c, user, (unsigned int)amount);
            break;
        case 0x2d:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2d, user, (unsigned int)amount);
            break;
        case 0x2e:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2e, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x2f:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2f, user, (unsigned int)amount);
            // ORIG switch 末尾带 default: break（产生 jmp+nop 尾部形态）
        default:
            break;
        }
    }
}


// ---- 发包依赖（StreamPool/CStreamGuard 经 GameBase.h 链已可见；MsgQueueMgr/GlobalData 本地声明） ----
class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2,
        TIMER_Q = 3,
        LOGDB_Q = 4
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;   // ORIG B 0x941f714（GlobalData.cpp 定义）
}
// ===================== 简单访问器（ORIG weak inline 形态） =====================


// 原 GetInvenSlotSelfHelper（ORIG 隐藏返回指针形态）已并入成员
// CInventory::GetInvenSlot（CInventory.h 返回 Inven_Item），删除替身。

unsigned int CInventory::GetCoin() const
{
    return m_coin;
}

void CInventory::SetCoin(unsigned int coin)
{
    m_coin = coin;
}

unsigned int CInventory::GetPayCoin() const
{
    return m_payCoin;
}

void CInventory::SetPayCoin(unsigned int coin)
{
    m_payCoin = coin;
}

unsigned int CInventory::GetEventCoin() const
{
    return m_eventCoin;
}

void CInventory::SetEventCoin(unsigned int coin)
{
    m_eventCoin = coin;
}

unsigned int CInventory::getTotalCoin() const
{
    return m_coin + m_eventCoin + m_payCoin;
}

bool CInventory::IsCoinUsable() const
{
    return m_coin != 0 || m_payCoin != 0 || m_eventCoin != 0;
}

int CInventory::get_money() const
{
    return m_money;
}

void CInventory::set_money(unsigned int money)
{
    m_money = money;
}

int CInventory::get_inventory_capacity() const
{
    return m_inventoryCapacity;
}

void CInventory::set_inventory_capacity(int capacity)
{
    m_inventoryCapacity = capacity;
}

void CInventory::set_parent(CUser* parent)
{
    m_pParent = parent;
    m_creatureMgr.SetUser(parent);
}

void CInventory::SetInventoryMemory(InventoryMemory* mem)
{
    // InventoryMemory 布局（CUser.h 权威，ctor 0x822d3b8 四段构造实证）：
    // m_items1[312]@+0x0000、m_items2[105]@+0x4a58、m_pCreatureSlot 段 m_items3[242]@+0x635d。
    m_pEquipSlot = mem->m_items1;
    m_pAvatarSlot = mem->m_items2;
    m_pCreatureSlot = mem->m_items3;
}

user_creature::CCreatureMgr* CInventory::GetCreatureMgrW()
{
    return &m_creatureMgr;
}

const user_creature::CCreatureMgr* CInventory::GetCreatureMgrR() const
{
    return &m_creatureMgr;
}

WongWork::CAvatarItemMgr* CInventory::GetAvatarItemMgrW()
{
    return &m_avatarItemMgr;
}

const WongWork::CAvatarItemMgr* CInventory::GetAvatarItemMgrR() const
{
    return &m_avatarItemMgr;
}

void CInventory::AvatarVisibleData::init()
{
    m_field6 = 0;
    m_field4 = m_field6;
    m_field0 = m_field4;
}

int CInventory::GetClearAvatar(int idx) const
{
    if (idx >= 0 && idx <= 9)
        return m_clearAvatar[idx];
    return 0;
}

void CInventory::SetClearAvatar(int idx, int value)
{
    if (idx >= 0 && idx <= 9)
    {
        m_clearAvatar[idx] = value;
    }
}

void CInventory::GetClearAvatar(unsigned long* out) const
{
    for (int i = 0; i < 10; i++)
    {
        out[i] = m_clearAvatar[i];
    }
}

void CInventory::SetClearAvatar(unsigned long* arr)
{
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = arr[i];
    }
}

// ORIG 0x8504d0d：比较两个槽位排序键；键 = 槽空(addInfo==0)?0xb:m_field1(ITEM_TYPE)。
int _CompareSlot(const void* va, const void* vb)
{
    const Inven_Item* a = (const Inven_Item*)va;
    const Inven_Item* b = (const Inven_Item*)vb;
    int sa = a->m_addInfo != 0 ? a->m_field1 : 0xb;
    int sb = b->m_addInfo != 0 ? b->m_field1 : 0xb;
    if (sa < sb) return -1;
    if (sa > sb) return 1;
    return 0;
}

// ORIG C1 0x822d52c W：26 个 m_invenItem 逐元素构造（+0x1c 步进 0x3d）→
// m_pEquipSlot(+0x650)/m_pAvatarSlot(+0x654)/m_pCreatureSlot(+0x6e4) 置 0 →
// AvatarCoin C1(+0x658) → CCreatureMgr C1(+0x6e8) → CAvatarItemMgr C1(+0x734)
// → reset()。成员初始化列表按声明序即复现该序列。
CInventory::CInventory()
    : m_invenItem(), m_pEquipSlot(0), m_pAvatarSlot(0), m_avatarCoin(),
      m_pCreatureSlot(0), m_creatureMgr(), m_avatarItemMgr()
{
    reset();
}   // 三槽指针(+0x650/+0x654/+0x6e4)声明先于 AvatarCoin/CCreatureMgr 自动构造，故入 init-list(ORIG 序)

// ORIG D1 0x822d600 W：CAvatarItemMgr D1(+0x734) → CCreatureMgr D1(+0x6e8)；
// 其余成员平凡析构，由编译器逆序发射。
CInventory::~CInventory()
{
}
// [b28d-Main·检查点16 S425] m_creatureMgr 改**成员初始化列表**（P147）：
//   隐式拷贝构造的语义本就是成员拷贝构造；改后 `_ZN13user_creature12CCreatureMgrC1ERKS0_`
//   由本 TU 发射（ORIG 该 W 符号的天然发射点），S425 可删其 TU 级 force 使用点。
CInventory::CInventory(const CInventory& other)
    : m_creatureMgr(other.m_creatureMgr), m_avatarItemMgr(other.m_avatarItemMgr)
{
    // b28d-solo-CInventory：157/196，N84 封顶（ORIG 隐式 rep movsl 块拷贝；全仓无 U 引用不可删显式定义）。
    // b28d-37 [N84 封顶，ORIG 0x869??]：同 aSERKS_——C1ERKS_ 为 W 隐式拷贝但全仓无 U 引用（无发射点），回退显式定义封顶。157/196。
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    m_inventoryCapacity = other.m_inventoryCapacity;
    for (int i = 25; i >= 0; i--)
    {
        m_invenItem[i] = other.m_invenItem[i];
    }
    m_pEquipSlot = other.m_pEquipSlot;
    m_pAvatarSlot = other.m_pAvatarSlot;
    m_avatarCoin = other.m_avatarCoin;
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = other.m_clearAvatar[i];
    }
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i] = other.m_avatarVisible[i];
    }
    m_pCreatureSlot = other.m_pCreatureSlot;
}

CInventory& CInventory::operator=(const CInventory& other)
{
    // b28d-solo-CInventory：116/116，N84 封顶（ORIG rep movsl 隐式拷贝，全仓无 U 引用不可删显式定义）。
    // b28d-37 [N84 封顶，ORIG 0x869d73c]：ORIG aSERKS_ 为 W 隐式合成（rep movsl 0x18c dwords + word）；隐式版与 ORIG AE 但全仓 `nm -u` 无 U 引用（_Charac_info 以 blob 建模、外部走 setCopy）⇒ 无发射点，回退显式定义封顶。116/116。
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    m_inventoryCapacity = other.m_inventoryCapacity;
    // ORIG 0x869d7a3..0x869d7b5：rep movsl 0x18c dwords + 2 字节 = 0x632 字节，
    // 完整复制 m_invenItem[26]（26×0x3d = 0x632）。旧注释 0x18c 误引
    // GetInvenData（0x84fbfc8）站点，此处必须整数组复制。
    // [N归因] ORIG 0x632 整体复制被其编译器展开为 rep movsl+word（无 call），
    // c6root 4.4.7 -O0 下 memcpy 常量参数恒展开为 call memcpy，源层无法复现
    // 该 rep 形态；语义等价（0x630 dword 块 + 2 字节尾）。
    // [hc N 记] +0x630 处 2 字节尾 = m_invenItem[26] 连续 POD blob（0x632 字节）
    // 的块拷贝延续（跨 item[25].m_reserved.m_field4 高字节 + m_field8，无单一名
    // ORIG 字段可锚），属 §4.3「数据区/数组 blob 拷贝」例外，非具名成员访问；
    // 字段化需臆造 2 字节字段（违 §4.1/§4.3 getter 名权威）或改 rep 拆解致倒退，
    // 故保留块拷贝原语形态，记 N。orig/our insns 116=116（verdict 未倒退）。
    memcpy(m_invenItem, other.m_invenItem, 0x630);
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(m_invenItem) + 0x630) =
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(other.m_invenItem) + 0x630);
    m_pEquipSlot = other.m_pEquipSlot;
    m_pAvatarSlot = other.m_pAvatarSlot;
    m_avatarCoin = other.m_avatarCoin;
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = other.m_clearAvatar[i];
    }
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i] = other.m_avatarVisible[i];
    }
    m_pCreatureSlot = other.m_pCreatureSlot;
    m_creatureMgr = other.m_creatureMgr;
    m_avatarItemMgr = other.m_avatarItemMgr;
    return *this;
}

void CInventory::reset()
{
    m_pParent = 0;
    m_money = 0;
    m_coin = 1;
    m_payCoin = 0;
    m_eventCoin = 0;
    m_field14 = 0;
    memset(m_invenItem, 0, 0x632);
    if (m_pEquipSlot != 0)
    {
        memset(m_pEquipSlot, 0, 0x4a58);
    }
    if (m_pAvatarSlot != 0)
    {
        memset(m_pAvatarSlot, 0, 0x1905);
    }
    if (m_pCreatureSlot != 0)
    {
        memset(m_pCreatureSlot, 0, 0x39aa);
    }
    m_creatureMgr.Reset();
    m_creatureMgr.SetUser(m_pParent);
    m_avatarItemMgr.Reset();
    memset(m_clearAvatar, 0, 0x28);
    m_inventoryCapacity = 0;
    m_avatarCoin.Set(0);
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i].init();
    }
}

void CInventory::setCopy(const CInventory& other)
{
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    for (int i = 0; i < 26; i++)
    {
        m_invenItem[i].setCopy(other.m_invenItem[i]);
    }
    for (int i = 0; i < 0x138; i++)
    {
        m_pEquipSlot[i].setCopy(other.m_pEquipSlot[i]);
    }
    for (int i = 0; i < 0x69; i++)
    {
        m_pAvatarSlot[i].setCopy(other.m_pAvatarSlot[i]);
    }
    m_creatureMgr.SetUser(m_pParent);
    for (int i = 0; i < 0xf2; i++)
    {
        m_pCreatureSlot[i].setCopy(other.m_pCreatureSlot[i]);
    }
    m_inventoryCapacity = other.get_inventory_capacity();
}

// ===================== 槽 / 数据访问 =====================

bool CInventory::CheckValidSlot(int invenType, int slot) const
{
    if (slot < 0)
    {
        return false;
    }
    switch (invenType)
    {
    case 0:
        if (slot > 0x19)
        {
            return false;
        }
        break;
    case 1:
        if (slot > 0x137)
        {
            return false;
        }
        break;
    case 2:
        if (slot > 0x68)
        {
            return false;
        }
        break;
    case 3:
        if (slot > 0xf1)
        {
            return false;
        }
        break;
    case 5:
        if (slot < 10 || slot > 0x16)
        {
            return false;
        }
        break;
    }
    return true;
}

Inven_Item* CInventory::GetInvenRef(int invenType, int slot)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        return &m_invenItem[slot];
    case 1:
        return &m_pEquipSlot[slot];
    case 2:
        return &m_pAvatarSlot[slot];
    case 3:
        return &m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        return pExpand->GetInvenRefW(INVEN_TYPE_5, slot);
    }
    default:
        return 0;
    }
}

const Inven_Item* CInventory::GetInvenRef(int invenType, int slot) const
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        return &m_invenItem[slot];
    case 1:
        return &m_pEquipSlot[slot];
    case 2:
        return &m_pAvatarSlot[slot];
    case 3:
        return &m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        return pExpand->GetInvenRefR(INVEN_TYPE_5, slot);
    }
    default:
        return 0;
    }
}

Inven_Item* CInventory::GetInvenStart(int invenType) const
{
    switch (invenType)
    {
    case 0:
        return const_cast<Inven_Item*>(&m_invenItem[10]);
    case 1:
        return m_pEquipSlot;
    case 2:
        return m_pAvatarSlot;
    case 3:
        return m_pCreatureSlot;
    case 5:
    {
        CExpandEquipslot* expand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        return expand->getExpandEquipslotW((ENUM_EQUIPSLOT)1);
    }
    }
    return 0;
}

Inven_Item CInventory::GetInvenSlot(int invenType, int slot) const
{
    // b28d-solo-CInventory：256/170，P167 调用点 CLEAR；差为 libstdc++ 模板内联 ⇒ N89 封顶。
    // b28d-37 [N89/N82 封顶，ORIG 0x850?]：P167 调用点差集 CLEAR（0 missing/0 extra）⇒ 语义闭合；ORIG 256 vs our 170 差为 libstdc++ 模板内联/帧复用（N89 族）。
    Inven_Item item;
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return item;
    }
    if (m_pEquipSlot == 0 || m_pAvatarSlot == 0 || m_pCreatureSlot == 0)
    {
        return item;
    }
    switch (invenType)
    {
    case 0:
        return m_invenItem[slot];
    case 1:
        return m_pEquipSlot[slot];
    case 2:
        return m_pAvatarSlot[slot];
    case 3:
        return m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        if (pExpand != 0)
        {
            return pExpand->GetInvenSlot(INVEN_TYPE_5, slot);
        }
        return item;
    }
    default:
        break;
    }
    return item;
}

int CInventory::GetInvenSlotByRef(int invenType, int slot, Inven_Item& item)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        item = m_invenItem[slot];
        break;
    case 1:
        item = m_pEquipSlot[slot];
        break;
    case 2:
        item = m_pAvatarSlot[slot];
        break;
    case 3:
        item = m_pCreatureSlot[slot];
        break;
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        item = pExpand->GetInvenSlot(INVEN_TYPE_5, slot);
        break;
    }
    default:
        break;
    }
    return 1;
}


void CInventory::GetInvenData(int invenType, void* out, int size) const
{
    // b28d-solo-CInventory：已修——5 处 memcpy 长度改用形参 size（ORIG 实证 0x14(%ebp)），73/73 仅余重定位伪差 ⇒ AE（待 relink）。
    switch (invenType)
    {
    case 0:
        if (size == 0x632)
        {
            memcpy(out, m_invenItem, size);
        }
        break;
    case 1:
        if (size == 0x4a58)
        {
            memcpy(out, m_pEquipSlot, size);
        }
        break;
    case 2:
        if (size == 0x1905)
        {
            memcpy(out, m_pAvatarSlot, size);
        }
        break;
    case 3:
        if (size == 0x39aa)
        {
            memcpy(out, m_pCreatureSlot, size);
        }
        break;
    case 5:
        if (size == 0x2dc)
        {
            memcpy(out, &m_invenItem[10], size);
        }
        break;
    }
}

void CInventory::SetInvenData(int invenType, void* in, int size)
{
    switch (invenType)
    {
    case 0:
        if (size == 0x632)
        {
            memcpy(m_invenItem, in, size);
        }
        break;
    case 1:
        if (size == 0x4a58)
        {
            memcpy(m_pEquipSlot, in, size);
        }
        break;
    case 2:
        if (size == 0x1905)
        {
            memcpy(m_pAvatarSlot, in, size);
        }
        break;
    case 3:
        if (size == 0x39aa)
        {
            memcpy(m_pCreatureSlot, in, size);
        }
        break;
    case 5:
        if (size == 0x2dc)
        {
            memcpy(&m_invenItem[10], in, size);
        }
        break;
    }
}

void CInventory::ResetSlot(int invenType, int slot)
{
    // ORIG 0x84fb32a：switch(invenType) 形态（一次载入寄存器后 cmp/je 二叉判定树）。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return;
    }
    switch (invenType)
    {
    case 0:
        m_invenItem[slot].reset();
        break;
    case 1:
        m_pEquipSlot[slot].reset();
        break;
    case 2:
        m_pAvatarSlot[slot].reset();
        break;
    case 3:
        m_pCreatureSlot[slot].reset();
        break;
    }
}

void CInventory::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const
{
    if (!g_slotBoundChecker.get_slot_bound(type, m_inventoryCapacity, start, end))
    {
        start = 0;
        end = 0;
    }
}

void CInventory::getStartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const
{
    StartEndPos(type, start, end);
}

int CInventory::get_empty_slot(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    // ORIG 0x84fb82a..+0x1a: switch 下探（mov 0xc,%eax 缓存；cmp2 je→+0x7c；
    // cmp3 je→+0xb4；cmp1 jne→+0xed），体排布 case1/case2/case3（技巧34）。
    switch (invenType)
    {
    case 1:
    {
        // ORIG 0x84fb844: start=-0x18 end=-0x1c 无初始化 dead store；i=-0x14。
        int start;
        int end;
        StartEndPos(type, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
        break;
    }
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 0xf2; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
        break;
    default:
        break;
    }
    return -1;
}

int CInventory::get_inven_slot_no(int itemIdx) const
{
    for (int i = 0; i <= 0x137; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::get_avatar_slot_no(int avatarUid) const
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == avatarUid)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::get_item_count(int itemIdx) const
{
    int count = 0;
    // ORIG 0x84fb3f1..+0x31: 4.1 switch 二叉下探（先 mov 0xc,%eax 缓存，
    // cmp $1 je case1；cmp $1 jg→{2,3}；test je case0），case 体按 0/1/2/3
    // 顺序排布（技巧 34）。i 槽 case0=-0x10 case1=-0xc case2=-0x8 case3=-0x4。
    switch (itemIdx)
    {
    case 0:
        for (int i = 0; i < 0x16; i++)
        {
            if (m_invenItem[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    case 1:
        for (int i = 3; i < 0x138; i++)
        {
            if (m_pEquipSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 0xf2; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    default:
        break;
    }
    return count;
}

int CInventory::get_item_count(int itemIdx, unsigned long param) const
{
    int count = 0;
    // ORIG 0x84fb50d..+0x35: 4.1 switch 下探形（与 get_item_count(int) 同构）；
    // case1 命中体二次装载 0x7 字段全字 add（0x84fb595..+0xaa）。
    switch (itemIdx)
    {
    case 0:
        for (int i = 0; i < 0x16; i++)
        {
            if ((unsigned long)m_invenItem[i].m_addInfo == param)
            {
                count++;
            }
        }
        break;
    case 1:
        for (int i = 3; i < 0x138; i++)
        {
            if ((unsigned long)m_pEquipSlot[i].m_addInfo == param)
            {
                count += m_pEquipSlot[i].m_addInfo2;
            }
        }
        break;
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if ((unsigned long)m_pAvatarSlot[i].m_addInfo == param)
            {
                count++;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 0xf2; i++)
        {
            if ((unsigned long)m_pCreatureSlot[i].m_addInfo == param)
            {
                count++;
            }
        }
        break;
    default:
        break;
    }
    return count;
}

int CInventory::InvenEmptycount(Inven_Item::ITEM_TYPE type) const
{
    int count = 0;
    int start;
    int end;
    StartEndPos(type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        return count;
    }
    else
    {
        if (type == 8)
        {
            for (int i = 0; i < 0x69; i++)
            {
                if (m_pAvatarSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
            // ORIG 0x8505132 +0xb2: avatar 循环耗尽后本块自带 mov -0x18,%eax
            // return（4.4 合并到公共尾会少一条）。
            return count;
        }
        else
        {
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
        }
        return count;
    }
}

int CInventory::QuickEmptyCount() const
{
    int count = 0;
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            count++;
        }
    }
    return count;
}

int CInventory::GetRemainCapacity(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    // b28d-solo-CInventory：113/129，type switch 被 4.4 折 bitmask（ORIG 用跳表 jmp *%eax）+ push %ebx 差 ⇒ N18a/N82 封顶。
    // b28d-37 [N18a/N82 封顶，ORIG 0x84f?]：push %ebx 差 + 帧/槽残差（113/129）。
    int count = 0;
    switch (invenType)
    {
    case 0:
    case 6:
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x4c3, "Invalid inventory type");
        break;
    case 1:
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        switch (type)
        {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
            break;
        default:
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__,
                0x493, "Invalid Item Type");
            break;
        case 10:
            break;
        }
        break;
    }
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        break;
    case 3:
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        break;
    }
    }
    return count;
}

int CInventory::check_slot_empty(int slot) const
{
    if (slot > 0x137 || slot <= 0)
        return 0;
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 1;
    }
    return 0;
}

// 返回类型 bool（batch-13 Main 终裁，P22：调用点 test %al/xor$1 形均为 bool 产物）；
// 体内 return 1/0 即 return true/false（0/1 语义与全部调用点兼容）。
bool CInventory::check_empty_count(Inven_Item::ITEM_TYPE type, int count) const
{
    int empty = 0;
    int start;
    int end;
    StartEndPos(type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                empty++;
            }
            if (empty >= count)
            {
                return 1;
            }
        }
        return 0;
    }
    else
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == 0)
            {
                empty++;
            }
            if (empty >= count)
            {
                return 1;
            }
        }
        return 0;
    }
}

int CInventory::check_item_exist(int itemIdx) const
{
    // ORIG 槽序(近→远)= type(-0xc) i(-0x10) start(-0x14) end(-0x18)（0x8505178/
    // 0x85051cd/0x850520c）；三个循环共用单槽 -0x10（4.1 scope 复用），
    // 改函数级具名 i 强制单槽。
    int start;
    int end;
    int i;
    Inven_Item::ITEM_TYPE type;
    for (i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    if (type == 0)
    {
        LogManager::logFormat(1, "inventory.cpp",
                              // ORIG 0x85051cf +0x5d: vararg 直落 mov -0xc(%ebp),%eax
                              // → 传 type 值而非 0。
                              __PRETTY_FUNCTION__, 0x1aa4,
                              "Unknown Type %d", type);
        // ORIG 0x8505202 +0x90: log 后 mov $0xffffffff,%eax; jmp end（体内早退）。
        return -1;
    }
    // ORIG 0x850520c +0x9a：无 start/end 初始化 dead store。
    StartEndPos(type, start, end);
    // ORIG 0x8505237 +0xc5: call IsCreatureItemType; test %al,%al; je +0x10a
    // → 正向判断 + creature(0x6e4) loop 为 fall-through 块（技巧 17/1 组合）。
    if (IsCreatureItemType(type))
    {
        for (i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == itemIdx)
            {
                return i;
            }
        }
        // ORIG 0x8505275 +0x103: creature 循环耗尽后本块自带 mov $0xffffffff
        // 尾岛（4.4 若合并到公共尾块会少一条）。
        return -1;
    }
    else
    {
        for (i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                return i;
            }
        }
    }
    return -1;
}

// ===================== 物品类型 / 哈希 =====================

int CInventory::GetItemType(int itemIdx) const
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
    {
        if (itemIdx != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x13c2,
                "No Item %d", itemIdx);
        }
        return 0;
    }
    if (item->IsAvatarItem())
    {
        return 8;
    }
    if (!item->is_stackable())
    {
        if (item->GetItemType() == 0x16)
        {
            return 5;
        }
        if (item->GetItemType() == 0x17)
        {
            return 6;
        }
        return 1;
    }
    if (item->GetItemType() == 9)
    {
        return 7;
    }
    if (item->GetItemType() == 1)
    {
        return 3;
    }
    if (item->GetItemType() == 2)
    {
        return 10;
    }
    if (item->GetItemType() == 8)
    {
        return 4;
    }
    if (item->GetItemType() == 0x14)
    {
        return 9;
    }
    return 2;
}

int CInventory::GetSlotItemType(int slot) const
{
    Inven_Item::ITEM_TYPE type;
    if (g_slotBoundChecker.get_item_type(slot, m_inventoryCapacity, type))
    {
        return type;
    }
    return Inven_Item::ITEM_TYPE_INVALID;
}

void CInventory::getHashCode(unsigned char* out)
{
    // b28d-solo-CInventory：41/41，6 处 lea -0xe0↔-0xe8 全帧槽基差 ⇒ N95 封顶（4.4 帧顶恒留 8B gap）。
    // b28d-37 [N95 封顶，ORIG 0x084fa91c]：fresh 复验 orig 41 / our 41（NDIFF=6，
    // 全为 6 处 `lea -0xe0(%ebp)` vs `lea -0xe8(%ebp)`）。md5_context 大局部
    // （0xdc 字节）4.1 可顶到 ebp（outgoing 区含 8B gap），4.4-O0 局部槽基恒从
    // ebp-8 起 ⇒ 全帧平移 8（范式库 N95 即以本函数为实证；P190 double framePad
    // 反致下移、int pad/声明序调整均不可上移）。清零/更新/收尾调用序与实参宽度
    // 与 ORIG 逐条一致，语义闭合。
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, (unsigned char*)&m_invenItem[0], 0x53e);
    md5_update(&ctx, (unsigned char*)m_pEquipSlot, 0x4a58);
    md5_update(&ctx, (unsigned char*)m_pAvatarSlot, 0x1905);
    md5_update(&ctx, (unsigned char*)m_pCreatureSlot, 0x39aa);
    md5_finish(&ctx, out);
}

int CInventory::getPackedCodeArray1(unsigned long* out, int maxCount)
{
    // b28d-solo-CInventory：138/141，帧/槽与寄存器分配残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x84f?]：帧/槽与寄存器分配残差（138/141）。
    int count = 0;
    for (int i = 0; i < 0x1a; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            out[count] = m_invenItem[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (int i = 0; i < 0x138; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            out[count] = m_pEquipSlot[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (int i = 0; i < 0x69; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo != 0)
        {
            out[count] = m_pAvatarSlot[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (int i = 0; ; i++)
    {
        if (i > 0xf1)
        {
            return count;
        }
        if (m_pCreatureSlot[i].m_addInfo != 0)
        {
            out[count] = m_pCreatureSlot[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
}

size_t CInventory::getSortedKeyArray(unsigned long* out, int maxCount)
{
    int i = 0;
    for (i = 0; i < maxCount; i++)
    {
        out[i] = 0;
    }
    int count = 0;
    for (i = 0; i < 0x1a; i++)
    {
        Inven_Item* pInvenItem = &m_invenItem[i];
        if (pInvenItem->m_addInfo != 0)
        {
            out[count] = pInvenItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i < 0x138; i++)
    {
        Inven_Item* pEquipItem = &m_pEquipSlot[i];
        if (pEquipItem->m_addInfo != 0)
        {
            out[count] = pEquipItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i < 0x69; i++)
    {
        Inven_Item* pAvatarItem = &m_pAvatarSlot[i];
        if (pAvatarItem->m_addInfo != 0)
        {
            out[count] = pAvatarItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i <= 0xf1; i++)
    {
        Inven_Item* pCreatureItem = &m_pCreatureSlot[i];
        if (pCreatureItem->m_addInfo != 0)
        {
            out[count] = pCreatureItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    qsort(out, count, 4, my_compare_unsigned_short);
    return count;
}

// ORIG 0x0850ae7a 返回类型为 int（尾调用 finishItemChecksum 的 eax 流出；
// mangled 不编码返回类型，符号不变）。CUser::getCurCharacInvenCheckSum 直接
// 返回该值（docs/class_func_reports/CUser/getCurCharacInvenCheckSum.md）。
// [N82 封顶] batch-10：差集 7=7 归零（三分支各 1 处 GetUpgrade 独立物化，循环界改 <=）。
// 残余 |gap|=4 = ORIG movzwl 双段零扩展 vs register 变量 spill 形态差（勿再按 ndiff 迭代）。
int CInventory::makeCheckSum(int a, int b) const
{
    // b28d-solo-CInventory：169/173，ORIG v 零扩展（movzwl %ax,%ebx）跨 GetUpgrade 保活 vs 我方 %esi ⇒ N18a 封顶。
    // b28d-37 [N18a 封顶，ORIG 0x84f?]：movzwl 两段 vs 单段 + 寄存器分配（169/173）。
    int sum = 0;
    // ORIG 三处 GetUpgrade 独立调用点（两循环分支各自物化，勿合并共享）。
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            register unsigned short v = (unsigned short)m_invenItem[i].m_fieldb;
            register unsigned char upgrade = m_invenItem[i].GetUpgrade();
            sum += getItemChecksum(i, m_invenItem[i].m_addInfo, 1, upgrade, v);
        }
    }
    for (int i = 3; i <= 0x137; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (m_pEquipSlot[i].m_field1 == 1)
            {
                register unsigned short v = (unsigned short)m_pEquipSlot[i].m_fieldb;
                register unsigned char upgrade = m_pEquipSlot[i].GetUpgrade();
                sum += getItemChecksum(i, m_pEquipSlot[i].m_addInfo, 1, upgrade, v);
            }
            else
            {
                register unsigned short v = (unsigned short)m_pEquipSlot[i].m_fieldb;
                register unsigned char upgrade = m_pEquipSlot[i].GetUpgrade();
                sum += getItemChecksum(i, m_pEquipSlot[i].m_addInfo,
                                       m_pEquipSlot[i].m_addInfo2, upgrade, v);
            }
        }
    }
    return finishItemChecksum(&sum, m_money, m_coin + m_payCoin + m_eventCoin, a, b);
}

// ===================== 货币（第九批 I2 补全） =====================

void CInventory::UpdateDailyCoin(int value)
{
    if ((int)m_coin < value)
    {
        m_coin = value;
        ((CUserCharacInfo*)m_pParent)->enableSaveInven();
    }
}

int CInventory::UseCoin(eCoinSubReason reason)
{
    // ORIG 0x84faa98：!=0 递降取向（coin→event→pay），history 调用内联、
    // trace 块跳转目标。b28c-L2：ORIG 三 trace 块槽为 首块 -0x38 / 后两块 -0x18
    // （互斥块同型临时槽方向差）⇒ 具名对象改全表达式临时链（P221，4.4 复现 ORIG 槽深）。
    if (m_coin != 0)
    {
        m_coin--;
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.CoinSub(m_coin, 1, reason);
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x28e, 5)("History ERROR, m_pParent NULL, CoinSub");
        }
    }
    else
    {
        if (m_eventCoin != 0)
        {
            m_eventCoin--;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.EventCoinSub(m_eventCoin, 1, reason);
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x29d, 5)("History ERROR, m_pParent NULL, EventCoinSub");
            }
        }
        else
        {
            if (m_payCoin != 0)
            {
                m_payCoin--;
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.PayCoinSub(m_payCoin, 1, reason);
                }
                else
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x2ad, 5)("History ERROR, m_pParent NULL, PayCoinSub");
                }
            }
            else
            {
                m_coin = 0;
                m_payCoin = 0;
                m_eventCoin = 0;
            }
        }
    }
    return m_coin;
}

int CInventory::UseUpCoin(unsigned int amount)
{
    if (amount < m_coin)
    {
        m_coin -= amount;
        amount = 0;
    }
    else
    {
        amount -= m_coin;
        m_coin = 0;
    }
    if (amount < (unsigned int)m_eventCoin)
    {
        m_eventCoin -= amount;
        amount = 0;
    }
    else
    {
        amount -= m_eventCoin;
        m_eventCoin = 0;
    }
    if (amount < (unsigned int)m_payCoin)
    {
        m_payCoin -= amount;
        // ORIG 0x84faa63 +0x8f: if 体 amount = 0。
        amount = 0;
    }
    else
    {
        // ORIG 0x84faa6c +0x98: else 体 amount -= m_payCoin 后置 0。
        amount -= m_payCoin;
        m_payCoin = 0;
    }
    return m_coin + m_eventCoin + m_payCoin;
}

// [b28d-PoolMisc2 structdiff 判据] 本件结构残差 **struct=0**、ndiff=8（AE 行的
// `-0xNN(%ebp)`/寄存器/立即数归一后与 ORIG 逐行全等）⇒ 纯 4.4 栈分配差，硬判
// N82附/N95 封顶（工具 /tmp/structdiff.py；同判据件：getHashCode struct=0/ndiff=6）。
int CInventory::gain_money(int amount, eMoneyAddReason reason, bool bLog, int param)
{
    // b28d-solo-CInventory：207/207，帧 0x6c/0x5c + bool spill/trace/money 三槽位差 ⇒ N82附（聚合对象优先占低槽）封顶。
    // b28d-37：ORIG 0x84ff29c 槽序 money(-0x40) < trace(-0x38) < limit(-0x28)
    // < oldMoney(-0x24) < gained(-0x20) < i(-0x1c)。块内 `int money` 被 4.4
    // 分配到浅槽(-0x1c)且整帧 +0x10；按 P212 提到函数头并置于槽深序首位复现。
    // [N82附 封顶] 实测 money 仍落 -0x2c（4.4 聚合对象优先占低槽 + 标量分区
    // 排布，ORIG 4.1 按生命周期穿插），limit/oldMoney/gained/i 已与 ORIG 逐槽
    // 相同；帧 0x6c vs 0x5c 与 bool 截断槽 -0x4c vs -0x3c 为分区后果 ⇒ NEAR 封顶。
    // [b28d-PoolMisc2 复验] 再补「money[2] 数组化推深槽」形：4.4 反把 trace 对象
    // 挤到 -0x40、money[0] 读回多一条 mov ⇒ ndiff **16→24** 劣化，已回滚标量形。
    int money;
    register int level;
    register const char* accName;
    int limit = 0x7fffffff;
    int oldMoney;
    unsigned int gained;
    unsigned int i;

    if (m_pParent != 0)
    {
        accName = m_pParent->get_acc_name();
        level = ((CUserCharacInfo*)m_pParent)->get_charac_level();
        limit = G_CDataManager()->GetMoneyLimitPerLevel(level, accName);
    }
    oldMoney = m_money;
    if (limit - amount < m_money)
    {
        m_money = limit;
        money = m_money;
        register int level2 = ((CUserCharacInfo*)m_pParent)->get_charac_level();
        register int characNo = m_pParent->get_charac_no(-1);
        register char* numStr = NumberToString(m_pParent->get_acc_id(), 0);
        // [b28d-PoolMisc2 复验零效] 本件把 cMyTrace 改 P227 全表达式临时链（use_money
        // 靠它 61→24）⇒ ndiff **16→16 零效**：本件三个保活值已是 `register`（level2/
        // characNo/numStr），ctor call 位置本就在其后，无块可翻；结构残差 struct=0，
        // 纯槽号。已回滚具名形。
        cMyTrace local_3c(__PRETTY_FUNCTION__, 0xef1, 4);
        local_3c("over money limit:m_id(%s),charac_no(%d),level(%d) gain_money inven(%d)+add(%d)",
                 numStr, characNo, level2, money, amount);
    }
    else
    {
        m_money += amount;
    }
    if (m_pParent != 0 && isGainedGoldFromDungeonReason(reason))
    {
        gained = m_pParent->getDungeonGainedGold()->incGainedGold(amount);
        for (i = 0; i < gained; i++)
        {
            m_pParent->getHackAnalyzer()->addServerHackCnt(
                m_pParent, (WongWork::ENUM_HACK_TYPE)0x2bf, 1u, 0u, 0u);
        }
    }
    if (bLog && m_pParent != 0)
    {
        m_pParent->m_historyLog.MoneyAdd(m_money, amount, reason);
    }
    if (m_pParent != 0)
    {
        if (reason == 4 || reason == 0x1a)
        {
            write_log_gain_money(reason, m_money - oldMoney - param, m_pParent);
        }
        else
        {
            write_log_gain_money(reason, m_money - oldMoney, m_pParent);
        }
        if (m_money - oldMoney > 0)
        {
            m_pParent->GetGoldControl()->AddGold(m_money - oldMoney, reason);
        }
    }
    return m_money - oldMoney;
}

bool CInventory::use_money(int amount, eMoneySubReason reason, bool bLog)
{
    // b28d-solo-CInventory：163/166，N82 槽号错位封顶（P0 具名局部已还原）。
    // ORIG 0x084ff54c 槽序 -0x40(具名 money 缓存) / -0x3c(bool spill) / -0x30(trace)
    // / -0x20(oldMoney) / -0x1c(limit)；4.4 分配为 -0xc/-0x10 族 ⇒ 全帧槽号差
    // [b28d-PoolMisc2 修成 ndiff 61→8（行口径）/ struct 15→0，件 DIFF→NEAR]
    // 两处源形：①P227 cMyTrace 改全表达式临时（ORIG trace ctor 在三值保活拷贝之后）；
    // ②P212 `int overMoney;` 提到**函数头第一个声明**（4.4 同 size 组按声明序由深到浅）。
    // 结构残差归零后余 8 行全为槽号 ⇒ structdiff 硬判 N82 封顶。
    // （N82：4.1 不复用槽/聚合优先低槽）。具名局部 overMoney 已按 P0 还原（store+load
    // 两处），语义闭合；残余仅槽号与 cMyTrace 变参装载序，判 N82 封顶。
    // [b28d-PoolMisc2 P212 试验] ORIG overMoney 落**最深**槽 -0x40（在 bool spill
    // -0x3c 之下）⇒ 按 4.4「先声明=更深」把声明提到函数头，块内仅赋值。
    // [b28d-PoolMisc2 修成] 声明必须放**函数头第一位**（4.4 同 size 组按声明序由
    // 深到浅）⇒ 单此一改 ndiff 61→24→**8**。撑 overMoney[4]（16B）再抢深度实测反劣
    // （帧 0x5c→0x6c、多 `mov -0x40; mov -0x50` 拷贝，ndiff 8→14）⇒ 回滚标量形。
    int overMoney;
    int oldMoney;
    int limit;

    if (m_money < amount || amount < 0)
    {
        return 0;
    }
    oldMoney = m_money;
    m_money -= amount;
    limit = 0x7fffffff;
    if (m_pParent != 0)
    {
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            ((CUserCharacInfo*)m_pParent)->get_charac_level(), m_pParent->get_acc_name());
    }
    if (limit < m_money)
    {
        m_money = limit;
        // b28d-solo-CInventory [P0]：ORIG `mov 0x4(%eax),%eax; mov %eax,-0x40(%ebp)` 与
        // 日志处 `mov -0x40(%ebp),%eax` 两次槽往返 ⇒ 原码有具名局部缓存（非成员直读）。
        // [b28d-PoolMisc2 P227 试验] ORIG 0x084ff54c：`mov %eax,-0x40(%ebp)` 之后是
        // 三条 callee-saved 保活拷贝（`mov %eax,%edi/%esi/%ebx` = NumberToString /
        // get_charac_no / get_charac_level 返回值），**cMyTrace ctor call 在其后**
        // ⇒ 原码 cMyTrace 是全表达式临时（P227），非具名局部（具名形 4.4 把 ctor
        // 提前到 vararg 求值之前，产 trace 块错位 6 行）。
        overMoney = m_money;
        cMyTrace(__PRETTY_FUNCTION__, 0xf30, 4)(
            "over money limit:m_id(%s),charac_no(%d),level(%d) use_money inven(%d)-sub(%d)",
            NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
            ((CUserCharacInfo*)m_pParent)->get_charac_level(), overMoney, amount);
    }
    if (m_money < 0)
    {
        m_money = 0;
    }
    if (m_pParent != 0)
    {
        write_log_use_money(reason, oldMoney - m_money, m_pParent);
        if (oldMoney - m_money > 0)
        {
            m_pParent->GetGoldControl()->SubGold(oldMoney - m_money, reason);
        }
    }
    if (bLog && m_pParent != 0 && amount > 0)
    {
        m_pParent->m_historyLog.MoneySub(m_money, amount, reason);
    }
    return 1;
}

int CInventory::getValidMoney(int add)
{
    int limit = 0x7fffffff;
    register const char* name = m_pParent->get_acc_name();
    register int level = ((CUserCharacInfo*)m_pParent)->get_charac_level() - 1;
    limit = G_CDataManager()->GetMoneyLimitPerLevel(level, name);
    if (limit < m_money + add)
    {
        return 1;
    }
    return 0;
}

// ===================== 槽 / 数据访问（第九批 I2 补全） =====================

int CInventory::GetInvenData(int itemIdx, Inven_Item& out) const
{
    // b28d-Inventory2 本轮：214/213 DIFF → 214/214 AE。两处真因：①type 局部声明为
    // Inven_Item::ITEM_TYPE（enum）⇒ 复现 ORIG `lea -0x20,%eax … lea -0x1c,%eax …
    // mov -0x18,%eax` 的 %eax 串行装载（int 局部时 4.4 提前载 type 并使 lea 走 %edx）；
    // ②creature 分支循环后显式 return -1 ⇒ 复现 ORIG 0x84fc041 内联
    // `mov $0xffffffff,%eax; jmp <尾>`（共享尾形丢该 store，P63）。
    Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    int start;
    int end;
    if (type == 0)
    {
        return -1;
    }
    StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == itemIdx)
            {
                out = m_pCreatureSlot[i];
                return i;
            }
        }
        // b28d-Inventory2：ORIG 0x84fc041 内联 `mov $0xffffffff,%eax; jmp <尾>`，
        // 共享尾形会丢该 store（P63/P223 族）⇒ 分支内显式 return。
        return -1;
    }
    else
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                out = m_pEquipSlot[i];
                return i;
            }
        }
        StartEndPos(Inven_Item::ITEM_TYPE_0, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                out = m_pEquipSlot[i];
                return i;
            }
        }
    }
    return -1;
}

int CInventory::GetEmptyCreatureSlot(Inven_Item& item) const
{
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pCreatureSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::GetExistCreatureSlot(Inven_Item& item) const
{
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pCreatureSlot[i].m_addInfo == item.m_addInfo)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::getAvatarEmptySlot(int itemIdx, int param) const
{
    for (int i = itemIdx; i <= param; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
}

unsigned char CInventory::getEquipWeaponUpgradeSeparate() const
{
    // ORIG 以绝对偏移访问（+0x280 标志、+0x2b1 UpgradeSeparateInfo），逐指令还原。
    if (m_invenItem[10].m_addInfo == 0)
    {
        return 0;
    }
    return m_invenItem[10].m_upgradeSep.GetUpgradeSeparate();
}

int CInventory::getExistEquipItem(int itemIdx, int count, int slot)
{
    int nField = 0;  // ORIG 顶部有未使用局部（-0x8(%ebp) 初始化为 0）
    // b28c-L2: ORIG 0x850348b..0x8503494 四个失败目标各落 nop 岛 + `jmp <addl>`
    // （0x850348d/490/493 三条）⇒ 负条件 goto 尾标签形（探针 l2_probe7 P 命中）。
    for (int i = 3; i <= 0xc8; i++)
    {
        if (i == slot) goto next_slot;
        if (m_pEquipSlot[i].m_field1 != 1) goto next_slot;
        if (m_pEquipSlot[i].m_addInfo != itemIdx) goto next_slot;
        if (m_pEquipSlot[i].m_addInfo2 != count) goto next_slot;
        return i;
    next_slot:;
    }
    return -1;
}

int CInventory::doesExistEquipItem(int itemIdx, int slot)
{
    // b28c-L2: ORIG 0x85033f8/fb/fe 三失败目标 + 0x85033ff/402 两条 `jmp <addl>`
    // nop 岛 ⇒ 负条件 goto 尾标签形（探针 l2_probe7 Q 命中）。
    for (int i = 3; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != itemIdx) goto next_slot;
        if (m_pEquipSlot[i].m_addInfo2 != slot) goto next_slot;
        if (m_pEquipSlot[i].m_field1 != 1) goto next_slot;
        return 1;
    next_slot:;
    }
    return 0;
}

int CInventory::MoveItemToEmptySlotOfInventory(int slot, int start, int end)
{
    // b28c-L2: ORIG 0x85049c8 形——入口 `jmp cond` + 尾 `cmp/setle/test/jne`
    // 即 while(i<=end)（P25/P51）；item 的 Inven_Item C1 call 落在循环之后
    // （0x8504a22），故声明置于早退检查之后。
    int dst = -1;
    int i = start;
    while (i <= end)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            dst = i;
            break;
        }
        i++;
    }
    if (dst == -1)
    {
        return 0x16;
    }
    Inven_Item item;
    item.setCopy(m_pEquipSlot[slot]);
    m_pEquipSlot[slot].reset();
    m_pEquipSlot[dst].setCopy(item);
    return 0;
}

bool CInventory::IsEquipAvatar() const
{
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            return true;
        }
    }
    return false;
}

bool CInventory::IsExistStackable(int itemIdx, Inven_Item::ITEM_TYPE type) const
{
    // b28d-37 [已归零，ORIG 0x0850a016]：旧「跨版本块序封顶」判词作废——提前
    // `if (field != itemIdx) continue;` 形（P223② 同层 continue）令 4.4 复现 ORIG
    // 块序 BODY(cmp)/`je HIT`/ADDL/`jmp COND`/HIT(mov$1)/COND，36=36 逐条全等
    // （仅 .o 未解析 call 重定位伪差）。直 return 形产 `jne COND`+HIT 内联。
    int start;
    int end;
    StartEndPos(type, start, end);
    for (int i = start; i <= end; i++)
    {
        // b28d-37：ORIG 0x0850a05e 块序为 BODY(cmp)/`je HIT`/ADDL/`jmp COND`/
        // HIT(mov$1)/COND；4.4 直 return 形产 `jne COND`+HIT 内联（块序反）。
        // P223② 提前 continue 形试复现 HIT 落 ADDL 之后。
        if (m_pEquipSlot[i].m_addInfo != itemIdx)
        {
            continue;
        }
        return true;
    }
    return false;
}

// ORIG 0x850a088：entry 即 spill count(0xc,%ebp)->%al,-0x4（volatile 形参强制
// 槽落）。纯常量体会被 4.4 折叠掉 caller 的 call，volatile 挡折叠。
bool CInventory::IsOverStackableItem(int itemIdx, volatile unsigned char count)
{
    return true;
}

bool CInventory::isAnyExpiredAvatarItem() const
{
    // b28d-37：ORIG 0x850937e 两循环各带 1 个 nop 岛（je 落 +0x64/+0xd0 nop，
    // 后两条守卫 je/jge 落 +0x65/+0xd1 addl）。P223 定案①复现形=负条件 goto 到
    // 共享尾空标签串（标签序=守卫序），使 4.4 在标签处发 nop 中继。
    int now = OS_API::GetDateTimeTick();
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo == 0) goto L1;
        if (m_invenItem[i].m_addInfo2 != 0 && m_invenItem[i].m_addInfo2 < now)
            return true;
    L1:;
    }
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0) goto L2;
        if (m_pAvatarSlot[i].m_addInfo2 != 0 && m_pAvatarSlot[i].m_addInfo2 < now)
            return true;
    L2:;
    }
    return false;
}

bool CInventory::isEquipmentPlaceableAtInventory(int slot)
{
    if (slot > 2 && slot <= 8)
        return true;
    if (slot > 8 && slot <= 0x38)
        return true;
    return false;
}

// ===================== 背包插入 / 堆叠（第九批 I2 重点） =====================

int CInventory::insertInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog)
{
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            if (item.m_field1 != 1)
            {
                if (!checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
                {
                    return -10;
                }
            }
            m_pEquipSlot[i].setCopy(item);
            if (bLog)
            {
                if (m_pParent != 0)
                {
                    if (item.m_field1 == 1)
                    {
                        m_pParent->m_historyLog.ItemAdd(
                            (INVEN_TYPE)1, 1, 1, item, reason);
                    }
                    else
                    {
                        m_pParent->m_historyLog.ItemAdd(
                            (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                            item, reason);
                    }
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x1528, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
            return i;
        }
    }
    return -2;
}

int CInventory::insertInvenItemIntoCoresspondingSpace(Inven_Item item, eItemAddReason reason,
                                                      bool bLog)
{
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            if (item.m_field1 != 1)
            {
                if (!checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
                {
                    return -10;
                }
            }
            m_pEquipSlot[i].setCopy(item);
            if (bLog)
            {
                if (m_pParent != 0)
                {
                    if (item.m_field1 == 1)
                    {
                        m_pParent->m_historyLog.ItemAdd(
                            (INVEN_TYPE)1, 1, 1, item, reason);
                    }
                    else
                    {
                        m_pParent->m_historyLog.ItemAdd(
                            (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                            item, reason);
                    }
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x155c, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
            return i;
        }
    }
    return -2;
}

// [N82 封顶] batch-10：差集 18=18 归零；残余 gap=4 同上（esi/edi 持值形态差）。
int CInventory::stackInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 192 / 我 192（gap 0，p167 仅 cMyTrace C1/C2 同址别名 P93）。
    // 真修：日志 cMyTrace 由具名两语句改**全表达式临时链**（P227/P37）⇒ 4.4 在 ctor 前
    // 右起求值全部实参并跨 ctor 以 esi/edi/ebx 保活，与 ORIG +0x0a4..+0x128 逐段同形。
    // 旧判词「残余 3 = N115 帧差」被推翻（帧/push 已全等，仅尾 nop 对齐差豁免）。
    if (item.m_field1 == 1)
    {
        return -1;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            if (bLog && m_pParent != 0)
            {
                int itemAddInfo = item.m_addInfo;
                int itemInfo = item.get_add_info();
                cMyTrace(__PRETTY_FUNCTION__,
                         0x157c, 4)
                    ("INVEN INT_MAX setting 1 : m_id(%s),charac_id(%d) inven(%d)+add(%d) item_id(%d)",
                     NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
                     m_pEquipSlot[i].get_add_info(), itemInfo, itemAddInfo);
            }
            m_pEquipSlot[i].set_add_info(0);
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info() + item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].set_add_info(m_pEquipSlot[i].get_add_info() + item.m_addInfo2);
            if (bLog)
            {
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x1597, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
        }
        return i;
    }
    return -2;
}

// [N82 封顶] batch-10：调用点差集 20=20 归零（set 处 item.get_add_info 补回）；
// 残余 gap=4 = ORIG push/pop %esi/%edi 跨块持值 vs 本工具链栈槽 spill。
int CInventory::stackInvenItemIntoCorrespondingSpace(Inven_Item item, eItemAddReason reason,
                                                     bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 209 / 我 209（gap 0）。同 stackInvenItemIntoQuickSlot：
    // 日志 cMyTrace 改全表达式临时链（P227/P37），实参求值序/跨 ctor 保活与 ORIG 同形。
    if (item.m_field1 == 1)
    {
        return -1;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            if (bLog && m_pParent != 0)
            {
                int itemAddInfo = item.m_addInfo;
                int itemInfo = item.get_add_info();
                cMyTrace(__PRETTY_FUNCTION__,
                         0x15b7, 4)
                    ("INVEN INT_MAX setting 2 : m_id(%s),charac_id(%d) inven(%d)+add(%d) item_id(%d)",
                     NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
                     m_pEquipSlot[i].get_add_info(), itemInfo, itemAddInfo);
            }
            m_pEquipSlot[i].set_add_info(0);
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info() + item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].set_add_info(m_pEquipSlot[i].get_add_info() + item.get_add_info());
            if (bLog)
            {
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x15d2, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
        }
        return i;
    }
    return -2;
}

int CInventory::tryInsertInvenItemIntoQuickSlot(Inven_Item item) const
{
    if (!item.isEquipableItemType() &&
        !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
    {
        return -10;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -2;
}

int CInventory::tryInsertInvenItemIntoCoresspondingSpace(Inven_Item item) const
{
    if (!item.isEquipableItemType() &&
        !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
    {
        return -10;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -2;
}

int CInventory::tryStackInvenItemIntoQuickSlot(Inven_Item item) const
{
    if (item.m_field1 == 1)
    {
        return -1;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            return -2;
        }
        // b28d-37：ORIG 0x08501ca5..0xb2 对和值有槽往返（`lea (%ebx,%eax,1),%eax;
        // mov %eax,-0xc(%ebp); mov -0xc(%ebp),%edx`）⇒ P0 原码有具名局部；
        // 直传形少 2 条且循环变量槽上移 4（-0xc vs ORIG -0x10）。
        int sum = m_pEquipSlot[i].get_add_info() + item.get_add_info();
        if (!checkStackableLimit((unsigned long)item.m_addInfo, sum))
        {
            return -10;
        }
        return i;
    }
    return -4;
}

int CInventory::tryStackInvenItemIntoCorrespondingSpace(Inven_Item item) const
{
    // b28d-37 [已修复 AE，ORIG 0x08501cea]：P0 具名 sum 局部（ORIG 0x08501dca..0xe3 槽往返）复现 87→89=ORIG；fresh 复验零差（仅 .o 重定位伪差）。
    if (item.m_field1 == 1)
    {
        return -1;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            return -2;
        }
        // b28d-37：同 tryStackInvenItemIntoQuickSlot——ORIG 0x08501dca..0xe3 有
        // `mov %eax,-0xc(%ebp); mov -0xc(%ebp),%edx` 槽往返 ⇒ P0 具名 sum 局部。
        int sum = m_pEquipSlot[i].get_add_info() + item.get_add_info();
        if (!checkStackableLimit((unsigned long)item.m_addInfo, sum))
        {
            return -10;
        }
        return i;
    }
    return -4;
}

// ORIG T 0x08502d86 (size 0x60b)。
// b28d-PoolMisc2 [P251/P286 **修成**：ndiff 18→7、426→427/428]：`dbLogType != -1`
// 块内 `if (m_field1==1) … else if (m_field1==2) …` 改 **switch (item.m_field1) {
// case 1/break; case 2/break; }** ⇒ 逐条复现 ORIG 0x08502e4c..0x08502e5d 的
// `movzbl 0xd(%ebp); movzbl %al,%eax; cmp $0x1,%eax; je; cmp $0x2,%eax; je; jmp`
// 正向决策链（原 if/else-if 形 4.4 恒产 `cmp $N,%al; jne` 反向链 + 两 case 体
// 错位，共 11 行残差归零）。余 7 行 = ① 0x08502e44 `mov -0x1c(%ebp),%eax;
// cmp $0xffffffff,%eax` vs 4.4 `cmpl $-1,-0x1c(%ebp)`（P75 mem-cmp 折叠）；
// ② 0x08502e67/0x08502ea? 的 `mov -0x1c(%ebp),%ecx/%ebx` 在 receiver 载入
// **之后**取槽（4.4 提前到实参链首，P211 档③装载时刻）⇒ 跨版本 codegen 封顶。
// [b28d-Inventory2 本轮] ② 已修：dbLogType 声明 unsigned int（比较处同步 unsigned）
// ⇒ 三处 `mov -0x1c,%ecx/%ebx` 落回 receiver 之后，gap 1→0 条数 428/427。
// ① 维持封顶：unsigned 局部 + P286 单 case switch + P310 int+cast 三形实测仍产
// `cmpl $-1,-0x1c`（4.4 对局部与常量比较恒 mem 直比，P209/N18b）⇒ 语义完成、
// byte 层余 1 条（p167 CALLSITE CLEAR 21/21）。
int CInventory::insertItemIntoInventory(Inven_Item item, eItemAddReason reason, bool bLog, bool b)
{
    if (item.m_addInfo == 0)
    {
        if (item.get_add_info() < 0)
        {
            return -1;
        }
        if (item.get_add_info() == 0)
        {
            return 0;
        }
        gain_money(item.get_add_info(), (eMoneyAddReason)7, bLog, 0);
        return 0;
    }
    if (reason != 7 && reason != 2)
    {
        // b28d-Inventory2：ORIG 0x08502e44 `mov -0x1c,%eax; cmp $0xffffffff,%eax`
        // （两段寄存器形）。unsigned 局部可复现三处 `mov -0x1c,%ecx/%ebx` 的
        // receiver 后装载位置；比较处保持 unsigned（改 signed 比较会使装载位置回退）。
        // 残余 1 条 `cmpl $0xffffffff,-0x1c`：4.4 对局部与常量比较恒折 mem 直比
        // （P209/N18b；P286 单 case switch、P310 int+cast 两形实测均不可翻）⇒ 封顶。
        unsigned int dbLogType = (unsigned int)G_CDataManager()->IsDBLogItem(item.m_addInfo);
        if (dbLogType != (unsigned int)-1)
        {
            // [b28d-PoolMisc2 P251/P286 试验] ORIG 0x84f?（本段）为 4.1 的
            // `movzbl;movzbl %al,%eax; cmp $1,%eax; je; cmp $2,%eax; je; jmp`
            // **正向 switch 决策链**；`if/else if` 形 4.4 恒产 `cmp $N,%al; jne`
            // 反向链 ⇒ 改 switch 复现。
            switch (item.m_field1)
            {
            case 1:
                m_pParent->AddDBLogItem(item.m_addInfo, 1u,
                                        (ENUM_DBLOG_ITEM_TYPE)dbLogType);
                break;
            case 2:
                m_pParent->AddDBLogItem(item.m_addInfo, (unsigned int)item.get_add_info(),
                                        (ENUM_DBLOG_ITEM_TYPE)dbLogType);
                break;
            }
        }
    }
    if (item.IsCreatureItemType())
    {
        return insertItemIntoCreature(item, reason, bLog, b);
    }
    if (item.m_field1 == 8)
    {
        char ipgNo[24];
        WongWork::CGenUniqueNo::genIPGNo(
            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_6,
            ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipgNo);
        int slot = m_pParent->addAvatarItem(item.m_addInfo, (char)0, (char)item.m_fieldb,
                                            ipgNo, (eAvatarItemAddReason)0x14);
        m_pParent->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1, slot);
        return slot;
    }
    if (item.m_field1 != 1)
    {
        int result = stackInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = stackInvenItemIntoCorrespondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    if (item.m_field1 == 2)
    {
        int result = insertInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = insertInvenItemIntoCoresspondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    else
    {
        int result = insertInvenItemIntoCoresspondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        result = insertInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
    }
    return -1;
}

int CInventory::tryInsertItemIntoInventory(Inven_Item item) const
{
    if (item.m_addInfo == 0)
    {
        if (item.m_addInfo2 < 0)
        {
            return -1;
        }
        return 0;
    }
    if (item.m_field1 == 8)
    {
        int result = tryInsertAvatarIntoInventory(item);
        if (result >= 0)
        {
            return result;
        }
        return -1;
    }
    if (item.m_field1 != 1)
    {
        int result = tryStackInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = tryStackInvenItemIntoCorrespondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    if (item.m_field1 == 2)
    {
        int result = tryInsertInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = tryInsertInvenItemIntoCoresspondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    else
    {
        int result = tryInsertInvenItemIntoCoresspondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        result = tryInsertInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
    }
    return -1;
}

int CInventory::insertItemIntoEquipment(Inven_Item item, eItemAddReason reason, bool bLog, bool b)
{
    // b28d-solo-CInventory：88/88，if/else 极性已改（ORIG je 落 logFormat 尾块）⇒ 仅余 N7 间接 call 目标 %eax↔%edx 封顶。
    // b28d-37 [N7 封顶，ORIG 0x84ff8??]：间接 call 目标寄存器 %eax↔%edx 互换（ORIG `mov (%eax),%edx;...call *%edx` vs 4.4 `mov (%eax),%eax;...call *%eax`）；P230 换 4.4.6 后端可翻，本波不改路由 ⇒ 封顶。88/88，语义闭合。
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        return -1;
    }
    if (m_invenItem[pItem->GetItemType()].m_addInfo != 0)
    {
        return -2;
    }
    m_invenItem[pItem->GetItemType()].setCopy(item);
    if (bLog)
    {
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemAdd(
                (INVEN_TYPE)1, 1, 1, item, reason);
        }
        else
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__,
                0x14f9, "History ERROR, m_pParent NULL, ItemAdd ");
        }
    }
    return pItem->GetItemType();
}

int CInventory::tryInsertItemIntoEquipment(Inven_Item item) const
{
    // b28c-L2 [N7 封顶]：ORIG 0x0850a(报告) 两处 CItem::GetItemType 虚调用为
    // `mov (%eax),%edx; mov -0xc(%ebp),%eax; mov %eax,(%esp); call *%edx`
    // （目标指针落 %edx、实参落 %eax）；4.4-O0 对间接 call 目标恒 %eax（范式库 N7，
    // 附 4.1 按上下文用 ecx/edx），临时/引用/限定名/语句表达式均不可翻（N7 已 8 件
    // 实证）。语义（虚槽 + 单实参 + 调用序）与 ORIG 全等，仅剩寄存器名差 ⇒ 保持
    // 可读直调形，ndiff 8 封顶（fresh 复验：orig 44 / our 44）。
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        return -1;
    }
    if (m_invenItem[pItem->GetItemType()].m_addInfo != 0)
    {
        return -2;
    }
    return pItem->GetItemType();
}

int CInventory::insertItemIntoCreatureEquipment(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    return slot;
}

int CInventory::insertItemIntoArtifactEquipment(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    if (bLog)
    {
        WriteCreatureLog((INVEN_TYPE)3, item.m_addInfo, 1, 1, item.m_field1, reason);
    }
    return slot;
}

int CInventory::insertItemIntoCreatureStackable(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetExistCreatureSlot(item);
    if (slot >= 0)
    {
        if (0x7fffffff - item.get_add_info() <= m_pCreatureSlot[slot].get_add_info())
        {
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pCreatureSlot[slot].m_addInfo,
                                     m_pCreatureSlot[slot].get_add_info() + item.get_add_info()))
            {
                return -10;
            }
            m_pCreatureSlot[slot].set_add_info(
                m_pCreatureSlot[slot].get_add_info() + item.get_add_info());
            if (bLog)
            {
                WriteCreatureLog((INVEN_TYPE)1, item.m_addInfo, m_pCreatureSlot[slot].get_add_info(),
                                 item.get_add_info(), item.m_field1, reason);
            }
            return slot;
        }
    }
    slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    if (bLog)
    {
        WriteCreatureLog((INVEN_TYPE)3, item.m_addInfo, m_pCreatureSlot[slot].get_add_info(),
                         item.get_add_info(), item.m_field1, reason);
    }
    return slot;
}

int CInventory::insertItemIntoCreature(Inven_Item& item, eItemAddReason reason, bool bLog, bool b)
{
    // ORIG 0x0850a86a：单出口 result 变量形（三分支 call→mov %eax,-0xc→jmp 尾
    // reload；error 路径 `mov $-1; jmp` 跳过 reload）。gap 6→0，AE 级闭合（残差
    // 仅为 .o 切片 call/jmp 目标渲染，助记符序列全等）。
    int result = 0;
    if (item.m_field1 == 7)
    {
        result = insertItemIntoCreatureStackable(item, reason, bLog);
    }
    else if (item.m_field1 == 5)
    {
        result = insertItemIntoCreatureEquipment(item, reason, bLog);
    }
    else if (item.m_field1 == 6)
    {
        result = insertItemIntoArtifactEquipment(item, reason, bLog);
    }
    else
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x2549, "History ERROR, unknown inven_Item type, insertItemIntoCreature ");
        return -1;
    }
    return result;
}

int CInventory::tryInsertItemIntoCreature(Inven_Item& item) const
{
    int result = -1;
    if (item.isEquipableItemType())
    {
        result = GetEmptyCreatureSlot(item);
    }
    else
    {
        result = GetExistCreatureSlot(item);
        // b28c-L2: ORIG 0x850aaef `shr $0x1f,%eax; test %al,%al; je`（4.1 负数判定
        // 经无符号右移 31 物化到 al）；4.4 对 `result < 0` 恒折 `cmpl/jns`（探针 l2_probe2
        // c2 形命中）⇒ 保留等价语义的显式无符号移位形。
        if (static_cast<unsigned char>(static_cast<unsigned int>(result) >> 31))
        {
            result = GetEmptyCreatureSlot(item);
        }
    }
    return result;
}

// ===================== 日志 / 检查类（第九批 I2 补全） =====================

void CInventory::WriteCreatureLog(INVEN_TYPE invenType, int a, int b, int c, int d,
                                  eItemAddReason reason) const
{
    if (m_pParent != 0)
    {
        m_pParent->m_historyLog.CreatureItemAdd(
            invenType, a, b, c, d, reason);
    }
    else
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x24ce, "History ERROR, m_pParent NULL, ItemAdd ");
    }
}

// SIGFIX b28c-L2：返回类型 int→bool（ORIG rodata 串 "bool CInventory::CheckExist…"）。
bool CInventory::CheckExistItemInInventoryOrEquipment(int itemIdx, int& slot, bool& bFound) const
{
    bFound = false;
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            slot = i;
            return 1;
        }
    }
    // ORIG 0x8505322：type 槽 -0x18；enum 槽免 cast（StartEndPos 实参序依赖）。
    Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    if (type == 0)
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            // ORIG 0x850532b +0x6d: vararg 形 mov -0x18(%ebp),%eax → 传 type 值。
            0x1ace, "Unknown Type %d", type);
        return 0;
    }
    for (int i = 0; i <= 0x19; i++)
    {
        if (m_invenItem[i].m_addInfo == itemIdx)
        {
            bFound = true;
            slot = i;
            return 1;
        }
    }
    int start = 0;
    int end = 0;
    StartEndPos(type, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            slot = i;
            return 1;
        }
    }
    return 0;
}

int CInventory::check_vilmark_dungeon_item_exist(int itemIdx) const
{
    // ORIG 0x8505530：i(-0xc)/start(-0x10)/end(-0x14) 单槽复用、无初始化
    // dead store（0x850556c 直接 lea）。声明序=近槽优先（技巧21 实测反向）。
    int i;
    int start;
    int end;
    for (i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    StartEndPos((Inven_Item::ITEM_TYPE)3, start, end);
    for (i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    StartEndPos((Inven_Item::ITEM_TYPE)4, start, end);
    for (i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    return -1;
}

bool CInventory::possible_to_insert(int itemIdx) const
{
    Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    if (type == 0)
    {
        return 0;
    }
    if (check_empty_count(type, 1))
    {
        return 1;
    }
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return 0;
    }
    // ORIG 0x8505692 +0x70: is_stackable 返回直落 test %al,%al; je -> +0x99 尾岛
    // (mov $0,%eax)；正向判断（技巧 1 之反例：非 ! 形不出 xor）。
    if (pItem->is_stackable())
    {
        return (unsigned int)(~(unsigned int)check_item_exist(itemIdx)) >> 31;
    }
    return 0;
}

bool CInventory::removeDupliacteEquipItem()
{
    // ORIG 0x85034b5：i=-0x14、found=-0x10（具名槽 cmpl $-1,-0x10 技巧9）、
    // characNo=-0xc（0x8503523 先 spill 后 je 直落 0 形，非三目）；
    // itemIdx 跨 cMyTrace ctor 存活于 %ebx（0x8503562 mov 0x2(%eax),%ebx）。
    // b28c-L2：清除历史 register asm("ebx") 绑定（P0 硬约束），改 P227 全表达式
    // 临时链令 4.4 在 ctor 前求 varargs 并以 %ebx 跨 ctor 保活。
    int i;
    int found;
    for (i = 3; i <= 0xc8; i++)
    {
        // ORIG 0x85034d9 +0x2b: field1!=1 → jne +0x120（4.1 continue 岛 jmp+nop）；
        // b28c-L2：原码为「提前 continue」形（两个 continue 同层），4.4 才发射
        // ORIG 的 `jmp <addl>; nop` 岛（探针 l2_probe5 H 形命中）。
        if (m_pEquipSlot[i].m_field1 != 1)
        {
            continue;
        }
        found = getExistEquipItem(m_pEquipSlot[i].m_addInfo,
                                  m_pEquipSlot[i].m_addInfo2, i);
        if (found == -1)
        {
            continue;   // ORIG 0x850352a +0x7c: je → +0x123 岛
        }
        // ORIG 0x8503530..+0x9f：两分支均产 %eax、merge 单点 spill -0xc。
        // b28c-L2：ORIG else 分支 `mov $0x0,%eax` 后与 call 结果共用 merge 点
        // 0x850354d `mov %eax,-0xc(%ebp)` ⇒ 原码为单点赋值的三目（非 if/else 各自写槽）。
        int characNo = (m_pParent != 0)
                           ? ((CUserCharacInfo*)m_pParent)->getCurCharacNo()
                           : 0;
        cMyTrace(__PRETTY_FUNCTION__, 0x16d2, 5)("DUPLICATE ITEM REMOVED %d at %d of %d",
                                                 m_pEquipSlot[i].m_addInfo, i, characNo);
        m_pEquipSlot[i].reset();
        return 1;
    }
    return 0;
}

void CInventory::ShowAllExistItemsForErrorCheck() const
{
    for (int i = 3; i < 0x138; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x1af0,
                "Inventory(%d) : %d\n", i, m_pEquipSlot[i].m_addInfo);
        }
    }
    for (int i = 0; i < 0x1a; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x1af6,
                "Equip(%d) : %d\n", i, m_invenItem[i].m_addInfo);
        }
    }
}

int CInventory::checkToFix() const
{
    // ORIG 0x8509616：i 单槽 -0xc（两循环复用）；两条件拆双层 if——
    // checkStackableLimit 实参形为 get_add_info 先 call(0x850966a，结果留 %eax
    // 免槽)，随后独立二次寻址装载 m_addInfo(0x8509688)。
    int i;
    for (i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                return 1;
            }
        }
    }
    for (i = 0x39; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                return 1;
            }
        }
    }
    return 0;
}

void CInventory::CheckNFix()
{
    // ORIG 0x8509738 起：i 单槽 -0xc；与 checkToFix 同拆双层 if（get_add_info
    // 先 call 留 %eax、二次独立寻址装 m_addInfo）。
    int i;
    for (i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                m_pEquipSlot[i].m_addInfo2 = 1;
            }
        }
    }
    for (i = 0x39; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                m_pEquipSlot[i].m_addInfo2 = 1;
            }
        }
    }
}

void CInventory::ChecknFixWineAndMoru()
{
    // ORIG 0x8509886：i 单槽 -0xc；两循环均含 IsOverStackableItem 的 uchar
    // 实参双零扩展形（movzbl 0x1,%eax; movzbl %al,%edx → 0x4(%esp)）。
    char pad[8];
    int i;
    int d1;
    int d2;
    for (i = 3; i <= 8; i++)
    {
        // ORIG 0x85098a7..+0xa4：四级平铺 je/jne 同跳 INC(+0xbf)，无 bool 物化。
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (m_pEquipSlot[i].m_field1 != 1)
            {
                if (!IsOverStackableItem(m_pEquipSlot[i].m_addInfo,
                                         (unsigned char)m_pEquipSlot[i].m_field1))
                {
                    if (m_pEquipSlot[i].m_addInfo2 > 799)
                    {
                        m_pEquipSlot[i].m_addInfo2 = 1;
                    }
                }
            }
        }
    }
    for (i = 0x39; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (!IsOverStackableItem(m_pEquipSlot[i].m_addInfo,
                                     (unsigned char)m_pEquipSlot[i].m_field1))
            {
                if (m_pEquipSlot[i].m_addInfo2 > 799)
                {
                    m_pEquipSlot[i].m_addInfo2 = 1;
                }
            }
        }
    }
    (void)pad;
    (void)d1;
    (void)d2;
}

// ===================== 专家 Job（第九批 I2 补全） =====================

void CInventory::GetExpertJobAddtionalExp(
    ENUM_EXPERT_JOB_TYPE type, int param,
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const
{
    if (type == 0)
    {
        return;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                STItemScript::SEXPERTJOB::stExpertJobAdditionalExp exp;
                pItem->GetExpertJobAdditionalExp(exp);
                // b28c-L2: ORIG 0x850cd3e `flds (%eax)`(out) 先于 `flds -0x18`(exp)
                // ⇒ 原码书写序为 out + exp（x87 左操作数先入栈）。
                out.m_field0 = out.m_field0 + exp.m_field0;
                out.m_field4 = out.m_field4 + exp.m_field4;
            }
        }
    }
}

float CInventory::GetExpertJobCompoundMaterialVariation(ENUM_EXPERT_JOB_TYPE type, int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return total;
    }
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item((int)m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobCompoundMaterialVariation();
            }
        }
    }
    return total;
}

float CInventory::GetExpertJobCompoundRateVariation(ENUM_EXPERT_JOB_TYPE type, int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return total;
    }
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobCompoundRateVariation();
            }
        }
    }
    return total;
}

void CInventory::GetExpertJobCompoundResultVariation(
    ENUM_EXPERT_JOB_TYPE type, int param,
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const
{
    if (type == 0)
    {
        return;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                STItemScript::SEXPERTJOB::stExpertCompoundResultVariation var;
                pItem->GetExpertJobCompoundResultVariation(var);
                // b28c-L2: ORIG 0x850cae2/0x850caee 均先 `flds` out 再 `flds` var
                // ⇒ 原码书写序为 out + var（x87 左操作数先入栈）。
                out.m_field0 = out.m_field0 + var.m_field0;
                out.m_field4 = out.m_field4 + var.m_field4;
            }
        }
    }
}

float CInventory::GetExpertJobSelfDisjointBigWinRate(ENUM_EXPERT_JOB_TYPE type, int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return total;
    }
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item((int)m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobSelfDisjointBigWinRate();
            }
        }
    }
    return total;
}

float CInventory::GetExpertJobSelfDisjointResultVariation(ENUM_EXPERT_JOB_TYPE type,
                                                          int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return total;
    }
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item((int)m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobSelfDisjointResultVariation();
            }
        }
    }
    return total;
}

// ===================== 物品操作 / 槽处理（第九批 I2 补全） =====================

void CInventory::update_item(INVEN_TYPE invenType, int slot, Inven_Item item)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return;
    }
    switch (invenType)
    {
    case 0:
        m_invenItem[slot] = item;
        break;
    case 1:
        m_pEquipSlot[slot] = item;
        break;
    case 2:
        m_pAvatarSlot[slot] = item;
        break;
    case 3:
        m_pCreatureSlot[slot] = item;
        break;
    }
}

// ============================================================================
// GetIntegratedPvPItemAttr（3 参重载，ORIG T 0x084fc651）。MakeItemPacket 直接依赖。
// ORIG 反汇编（84fc651）：G_GameWorld()->IsIntegratedPvPBaseChannel() 且
// G_CDataManager()->GetUpgradeRevisionPvPChannel()==0 时，若 invenType 非 0/非 1
// 且 slot 在 (2, 0x38] 区间 → _getIntegratedPvPItemAttr(item)；否则 → item.GetItemAttr()。
// 返回 unsigned char（ORIG 无 movzbl，值在 %al）。依赖 _getIntegratedPvPItemAttr。
// ============================================================================
unsigned char GetIntegratedPvPItemAttr(const Inven_Item& item, INVEN_TYPE invenType,
                                       int slot)
{
    // ORIG 0x84fc651：外层 && 控制流；内层 switch(invenType)，case 0 空 break
    // （产生 je→nop→GetItemAttr 的 pad 形态），case 1 内做 slot 区间 (2,0x38] 判定。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        switch (invenType)
        {
        case INVEN_TYPE_INVEN:
            break;
        case INVEN_TYPE_EQUIP:
            if (slot > 2 && slot <= 0x38)
            {
                return (unsigned char)_getIntegratedPvPItemAttr(item);
            }
            break;
        }
    }
    return item.GetItemAttr();
}

// ============================================================================
// CInventory::MakeItemPacket（ORIG 0x084fc6bc，0x1015 字节 / 1073 insn）。
// [N82 封顶标注] batch-10：gap 242→1（1072 vs 1073），ORIG 调用点与 .o R_386_PC32
// 调用多重集 149=149 零差集；残余 1 条 = ORIG 远跳 epilogue 共享多 1 个 `jmp`
// （trace 块布局差异，§6.1/§6.2），另有 add↔lea、shl↔sar、je↔jne 等 N82 形态别名。
// 判据：语义完成、byte 层 N 封顶，勿再按 ndiff 迭代。
// 按 INVEN_TYPE 分 4 组槽位发包：EQUIP(1) 含 0/1/2 三个特殊槽（0=money、
// 1=coin+payCoin+eventCoin、2=GetWinPoint）与普通装备槽；AVATAR(2) 走
// m_pAvatarSlot（含 jewel socket / expansion info）；CREATURE(3) 走
// m_pCreatureSlot（ITEM_TYPE 5 宠时补 GetRemainDate）；INVEN(0) 走
// m_invenItem[slot]（含 PvP 综合通道/封印数分支、avatar 扩展、宠物日期、
// make3rdChroniclePacket）。空槽 / 非期望类型返回 false。
// 依赖：CCreatureMgr::GetRemainDate（头已声明，BLOCKED——producer 提供定义）。
// ============================================================================
bool CInventory::MakeItemPacket(INVEN_TYPE invenType, int slot, PacketGuard& packet) const
{
    // b28d-solo-CInventory：1073/1072，P167 CLEAR；N82/N89 封顶。
    // [b28d-tu-CInventory] P329 大捷：删具名引用 `InterfacePacketBuf& buf`
    //   （98 处调用点内联 cast，ast_edit）⇒ 4.4 与 ORIG 0x084fc6bc 同形
    //   「每调用点 `mov 0x14(%ebp),%eax` 形参直读 + eax 先装载」，
    //   ndiff 1329→567、**struct 505→113**，P167 149/149 CLEAR 保持。
    //   方法3（case 书写序改 EQUIP→AVATAR→INVEN→CREATURE 追 ORIG 叶块物理序
    //   6f5/d93/29c/063）实测**反劣** struct 113→668/ndiff 567→789，已回滚
    //   ⇒ 4.4 密集值 switch 落体不跟随 ORIG 比较树，方法3 对本件不适用。
    //   残 113 = 帧 0x80/0x70（−16B/4 槽 N95）+ return-1 tail-merge 岛（N89）
    //   + shl/sar·je/jne 跨版本别名。
    // b28d-37 [N82/N89 封顶，ORIG 0x84fc6bc]：P183 去视图缓存已应用；P167 调用点 CLEAR；1073/1072。
    // [§6.2 闭合 · b20] P167 149/149 归零；ORIG 在本 const 方法内走
    // put_binary(const char*,int)（PKc 重载），expansion/expInfo 已常量化对齐
    // 该 call 目标（同宽同槽，零扰动）。新鲜 .o 1072 vs ORIG 1073（gap 1）。
    // [b28d-tu-CInventory] P329：删具名引用 `buf`（4.4 为其发 -0x24 槽 +
    //   prologue store/load，ORIG 0x084fc6bc 全部 98 处 this 装载直读形参槽
    //   `mov 0x14(%ebp),%eax`）⇒ 调用点改内联 cast 形（ast_edit 98 处）。
    GameWorld* world;
    // 本方法为 const；ORIG 0x084fc6bc 对 expansion/expInfo 走 put_binary(const
    // char*,int)（PKc 重载），故指针常量化（同 4B 槽，仅换 call 目标，零扰动）。
    const char* expansion;

    // ORIG(-O0) 在每次字段访问前完整重物化数组元素地址（无指针缓存），以下各
    // case 用 m_pXxx[slot].field 全索引形式逐段对应 0x084fc6bc 起各 put 链。
    switch (invenType)
    {
    case INVEN_TYPE_EQUIP:
        ((InterfacePacketBuf&)packet).put_short(slot);
        if (slot == 0)
        {
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_int(m_money);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(g_emptySlot);
            return true;
        }
        if (slot == 1)
        {
            ((InterfacePacketBuf&)packet).put_int(1);
            ((InterfacePacketBuf&)packet).put_int(m_coin + m_payCoin + m_eventCoin);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(g_emptySlot);
            return true;
        }
        if (slot == 2)
        {
            ((InterfacePacketBuf&)packet).put_int(2);
            ((InterfacePacketBuf&)packet).put_int(m_pParent->GetWinPoint());
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(g_emptySlot);
            return true;
        }
        if (m_pEquipSlot[slot].m_addInfo != 0)
        {
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo);
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], INVEN_TYPE_EQUIP, slot));
            ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_fieldb);
            ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_field0);
            world = G_GameWorld();
            if (world->IsEnchantRevisionChannel())
                ((InterfacePacketBuf&)packet).put_int(0);
            else
                ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_fieldd);
            ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_amp.getAbilityType());
            ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, m_pEquipSlot[slot], &packet);
            ((InterfacePacketBuf&)packet).put_packet(m_pEquipSlot[slot]);
            return true;
        }
        // ORIG 0x084fcd43：外层已判 ==0，此处重复同条件 → trace(0x758) return 0。
        // 静态不可达但 ORIG -O0 实际发射，保留以对齐逐块结构。
        if (m_pEquipSlot[slot].m_addInfo != 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x758, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x758);
            return false;
        }
        ((InterfacePacketBuf&)packet).put_item_idx(0xffffffff);
        ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo2);
        ((InterfacePacketBuf&)packet).put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], INVEN_TYPE_EQUIP, slot));
        ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_fieldb);
        ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_field0);
        world = G_GameWorld();
        if (world->IsEnchantRevisionChannel())
            ((InterfacePacketBuf&)packet).put_int(0);
        else
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_fieldd);
        ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_amp.getAbilityType());
        ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
        ((InterfacePacketBuf&)packet).put_byte(0);
        ((InterfacePacketBuf&)packet).put_packet(m_pEquipSlot[slot]);
        return true;

    case INVEN_TYPE_AVATAR:
        if (m_pAvatarSlot[slot].m_addInfo == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7a0, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x7a0);
            return false;
        }
        {
            int nowTick = 0;
            ((InterfacePacketBuf&)packet).put_short(slot);
            ((InterfacePacketBuf&)packet).put_int(m_pAvatarSlot[slot].m_addInfo);
            nowTick = OS_API::GetDateTimeTick();
            ((InterfacePacketBuf&)packet).put_int(m_avatarItemMgr.GetRemainDate(m_pAvatarSlot[slot].m_addInfo2,
                                                      nowTick));
            ((InterfacePacketBuf&)packet).put_byte(m_pAvatarSlot[slot].GetItemAttr());
            ((InterfacePacketBuf&)packet).put_short(m_pAvatarSlot[slot].m_fieldb);
            ((InterfacePacketBuf&)packet).put_byte(m_pAvatarSlot[slot].m_field0);
            ((InterfacePacketBuf&)packet).put_int(m_pAvatarSlot[slot].m_fieldd);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_int(0x1e);
            ((InterfacePacketBuf&)packet).put_binary(m_avatarItemMgr.getJewelSocketData(m_pAvatarSlot[slot].m_addInfo2),
                           0x1e);
            expansion = (const char*)m_avatarItemMgr.GetExpansionInfo(m_pAvatarSlot[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_int(4);
            if (expansion != 0)
            {
                ((InterfacePacketBuf&)packet).put_binary(expansion, 4);
            }
            else
            {
                stAvatarExpansionInfo_t expInfo;
                expInfo.init();
                ((InterfacePacketBuf&)packet).put_binary((const char*)&expInfo, 4);
            }
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(m_pAvatarSlot[slot]);
            return true;
        }

    case INVEN_TYPE_CREATURE:
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x801, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x801);
            return false;
        }
        {
            int nowTick = 0;
            ((InterfacePacketBuf&)packet).put_short(slot);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_addInfo);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_byte(m_pCreatureSlot[slot].GetItemAttr());
            ((InterfacePacketBuf&)packet).put_short(m_pCreatureSlot[slot].m_fieldb);
            ((InterfacePacketBuf&)packet).put_byte(m_pCreatureSlot[slot].m_field0);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_fieldd);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            if (m_pCreatureSlot[slot].m_field1 ==
                (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
            {
                nowTick = OS_API::GetDateTimeTick();
                ((InterfacePacketBuf&)packet).put_int(GetCreatureMgrR()->GetRemainDate(
                    m_pCreatureSlot[slot].m_addInfo2, nowTick));
            }
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(m_pCreatureSlot[slot]);
            return true;
        }

    // [b28d-tu-CInventory] 方法3 实测**反劣已回滚**：本件 switch 改书写序
    //   EQUIP→AVATAR→INVEN→CREATURE（ORIG 叶块物理序 6f5/d93/29c/063）⇒
    //   struct 113→668、ndiff 567→789 双反劣（4.4 对 0..3 密集值 switch 的
    //   落体布局不跟随 ORIG 比较树，方法3 对本件不适用）⇒ 保持原书写序。
    case INVEN_TYPE_INVEN:
        if (m_invenItem[slot].m_addInfo == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x884, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x884);
            return false;
        }
        ((InterfacePacketBuf&)packet).put_short(slot);
        ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_addInfo);
        ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_addInfo2);
        world = G_GameWorld();
        if (world->IsIntegratedPvPBaseChannel() &&
            G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
        {
            unsigned char attr = 0;
            if ((unsigned int)slot <= 0x15u && (((1 << slot) & 0x3ff400) != 0))
                attr = (unsigned char)(m_invenItem[slot].GetReSealCount() << 5);
            else
                attr = (unsigned char)m_invenItem[slot].GetItemAttr();
            ((InterfacePacketBuf&)packet).put_byte(attr);
        }
        else
        {
            ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].GetItemAttr());
        }
        ((InterfacePacketBuf&)packet).put_short(m_invenItem[slot].m_fieldb);
        ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].m_field0);
        world = G_GameWorld();
        if (world->IsEnchantRevisionChannel())
            ((InterfacePacketBuf&)packet).put_int(0);
        else
            ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_fieldd);
        ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].m_amp.getAbilityType());
        ((InterfacePacketBuf&)packet).put_short(m_invenItem[slot].m_amp.getAbilityValue());
        if (m_invenItem[slot].isAvatarItemType())
        {
            ((InterfacePacketBuf&)packet).put_int(0x1e);
            ((InterfacePacketBuf&)packet).put_binary(m_avatarItemMgr.getJewelSocketData(m_invenItem[slot].m_addInfo2),
                           0x1e);
            expansion = (const char*)m_avatarItemMgr.GetExpansionInfo(m_invenItem[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_int(4);
            if (expansion != 0)
            {
                ((InterfacePacketBuf&)packet).put_binary(expansion, 4);
            }
            else
            {
                stAvatarExpansionInfo_t expInfo;
                expInfo.init();
                ((InterfacePacketBuf&)packet).put_binary((const char*)&expInfo, 4);
            }
        }
        if (m_invenItem[slot].m_field1 == (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            int nowTick = 0;
            nowTick = OS_API::GetDateTimeTick();
            ((InterfacePacketBuf&)packet).put_int(GetCreatureMgrR()->GetRemainDate(m_invenItem[slot].m_addInfo2,
                                                         nowTick));
        }
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
            m_pParent, m_invenItem[slot], &packet);
        ((InterfacePacketBuf&)packet).put_packet(m_invenItem[slot]);
        return true;

    default:
        return false;
    }
}

// ============================================================================
// CInventory::MakeSecuRewarditemPacket（ORIG 0x084fd6d2）。仅 EQUIP(1) 生效：
// 空装备槽返回 0；否则写 put_short(slot)，item_idx 依 m_addInfo2 为 0 时置 -1，
// 再 put_int(m_addInfo2) 并返回 1。
// ============================================================================
int CInventory::MakeSecuRewarditemPacket(INVEN_TYPE invenType, int slot,
                                         PacketGuard& packet) const
{
    // b28d-solo-CInventory [P0+P223 双岛]：①ORIG 无 `buf` 局部槽——每调用点重新
    // `mov 0x14(%ebp),%eax; mov %eax,(%esp)`（形参直用）⇒ 删除具名引用局部；
    // ②ORIG 两 fail 块独立落 +0xd5/+0xdc（`jne <tail>` / `je <mid>`）⇒ 双 goto
    // 标签（fail2 前、fail1 后）复现（探针 /tmp/probe_guard2.cpp fD 逐条同构）。
    if (invenType != INVEN_TYPE_EQUIP)
        goto l_msr_fail1;
    if (m_pEquipSlot[slot].m_addInfo == 0)
        goto l_msr_fail2;
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    if (m_pEquipSlot[slot].m_addInfo == 0 || m_pEquipSlot[slot].m_addInfo2 == 0)
        ((InterfacePacketBuf*)&packet)->put_item_idx(0xffffffff);
    else
        ((InterfacePacketBuf*)&packet)->put_item_idx(m_pEquipSlot[slot].m_addInfo);
    ((InterfacePacketBuf*)&packet)->put_int(m_pEquipSlot[slot].m_addInfo2);
    return 1;
l_msr_fail2:
    return 0;
l_msr_fail1:
    return 0;
}

int CInventory::RemoveFPItem()
{
    return RemoveFPItem(m_invenItem, m_pEquipSlot);
}

int CInventory::RemoveFPItem(Inven_Item* inven, Inven_Item* equip)
{
    // b28d-solo-CInventory：73/77，N108/P223 负例确认（4.4 岛数恒=条件数或 0）封顶。
    // b28d-37 [N108/P223 负例，ORIG 0x850a2bc]：我方多 2 个 `jmp+nop` 岛（ORIG 双岛 je 落点 +0x314/+0x317，4.4 岛数恒=条件数或 0，探针 l2_probe9 R1-R4 四式全败）⇒ 封顶。73/77。
    // ORIG 0x850a2d7..+0x43：三条件各为 || 短路 je/jbe/ja → INC 岛（0x850a314/
    // 0x850a317 4.1 nop 落点）；COND setle i<=0x19 / i<=0x137（i 槽 -0x10/-0xc）。
    // [N 归因·4.1 pad 形] batch-13 P168a 五步亲验：非 xor 族；ORIG GCC 4.1 为空
    // continue 出口铺 nop+二次 jmp 落点 pad（je 直落+jbe/ja 双岛），4.4.7 对 ||链/
    // 正向嵌套/三层嵌套均直连 thread（实测 77/65 两形，无 nop 岛形可出）——源层
    // 不可翻，维持 `||`+continue 形（gap 4 = 2×nop+2×jmp pad 缺失，版本指令选择）。
    // b28c-L2 fresh 复算：本形式 ndiff 2（orig 69/our 71）；另测「负条件 goto 尾
    // 标签」形（探针 /tmp/l2_probe9.cpp R2/R3）与「正向 &&」/「空 else」（R1/R4）
    // 四式：R2/R3 出 3 岛（多 1 对 jmp+nop，同现形）、R1/R4 出 0 岛（缺 2 对）
    // ⇒ 4.4 无「1 直落 + 2 岛」组合形，维持现形 N 封顶 2。
    for (int i = 0; i <= 0x19; i++)
    {
        // b28d-solo-CInventory [P296+P223]：ORIG「条件1 直落 addl + 条件2/3 各带
        // nop 岛」（je +0x5c / jbe +0x58 / ja +0x5b）由「正向包裹 + 两条独立 continue」
        // 复现（探针 /tmp/probe_island2.cpp fP 逐条同形）；短路 || 形恒 3 岛（fO）。
        if (inven[i].m_addInfo != 0)
        {
            if (inven[i].m_addInfo <= 0x1963) continue;
            if (inven[i].m_addInfo > 0x1b57) continue;
            inven[i].reset();
        }
    }
    for (int i = 3; i <= 0x137; i++)
    {
        if (equip[i].m_addInfo != 0)
        {
            if (equip[i].m_addInfo <= 0x1963) continue;
            if (equip[i].m_addInfo > 0x1b57) continue;
            equip[i].reset();
        }
    }
}

int CInventory::RemoveKCItem()
{
    return RemoveKCItem(m_invenItem, m_pEquipSlot);
}
// [N 归因·4.1 pad 形] batch-13 P168a 亲验：同 RemoveFPItem——ORIG 空 continue
// 出口的 nop+jmp 落点 pad 为 GCC 4.1 指令选择，4.4.7 直连 thread 无该形，
// ||/嵌套两式实测 146/138 vs ORIG 142，维持 `||`+continue 形封顶。

int CInventory::RemoveKCItem(Inven_Item* inven, Inven_Item* equip)
{
    // b28d-solo-CInventory：142/146，同 RemoveFPItem（N108/P223 负例）封顶。
    // b28d-37 [N108/P223 负例，ORIG 0x850a0b7]：同 RemoveFPItem——我方多 2 个
    // jmp+nop 岛（4.4 不给『1 直落+2 岛』组合）；142/146。
    // ORIG 0x850a0b7..+0x4f：三条件 || 短路 je/jbe/ja → INC 岛(0x850a17f/
    // 0x850a182 4.1 nop)；i 槽 -0x18、pItem 槽 -0x14；addInfo 经 %ebx 跨
    // G_CDataManager 存活(0x850a0f7)。COND setle i<=0x19/i<=0x137。
    for (int i = 0; i <= 0x19; i++)
    {
        // b28d-solo-CInventory [P296+P223]：同 RemoveFPItem——正向包裹 + 两条独立
        // continue 复现「条件1 直落 + 条件2/3 nop 岛」（探针 /tmp/probe_island2.cpp fP）。
        if (inven[i].m_addInfo != 0)
        {
            if (inven[i].m_addInfo <= 0x1963) continue;
            if (inven[i].m_addInfo > 0x1b57) continue;
            CItem* pItem = G_CDataManager()->find_item(inven[i].m_addInfo);
            if (pItem == 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__, 0x23fe,
                    "CInventory::RemoveKCItem No EquipItem(%d) Found loopcount(%d)",
                    inven[i].m_addInfo, i);
            }
            else if (pItem->GetFinishPointPrice() != 0)
            {
                inven[i].reset();
            }
        }
    }
    for (int i = 3; i <= 0x137; i++)
    {
        if (equip[i].m_addInfo != 0)
        {
            if (equip[i].m_addInfo <= 0x1963) continue;
            if (equip[i].m_addInfo > 0x1b57) continue;
            CItem* pItem = G_CDataManager()->find_item(equip[i].m_addInfo);
            if (pItem == 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__, 0x2416,
                    "CInventory::RemoveKCItem No InvenItem(%d) Found loopcount(%d)",
                    equip[i].m_addInfo, i);
            }
            else if (pItem->GetFinishPointPrice() != 0)
            {
                equip[i].reset();
            }
        }
    }
}

int CInventory::sort(INVEN_TYPE invenType)
{
    if (invenType == 1)
    {
        int start;
        int end;
        for (int i = 1; i < 5; i++)
        {
            start = 0;
            end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)i, start, end);
            qsort(m_pEquipSlot + start, end - start + 1, 0x3d, _CompareSlot);
        }
        int start2 = 0;
        int end2 = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)10, start2, end2);
        qsort(m_pEquipSlot + start2, end2 - start2 + 1, 0x3d, _CompareSlot);
        return 0;
    }
    if (invenType == 3)
    {
        for (int i = 6; i < 8; i++)
        {
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)i, start, end);
            qsort(m_pCreatureSlot + start, end - start + 1, 0x3d, _CompareSlot);
        }
        return 0;
    }
    return 1;
}

bool CInventory::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const
{
    space = (ENUM_ITEMSPACE)3;
    slot = 0;
    while (slot < 0x16)
    {
        // ORIG 0x84fac9e-0x84faca4：GetLock(this+i*0x3d+0x2d)，+0x2d=this+0x1c+0x11
        // = m_invenItem[i].m_amp（FindItemLock.md）。
        if (m_invenItem[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)0;
    slot = 0;
    while (slot < 0x138)
    {
        if (m_pEquipSlot[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)7;
    slot = 0;
    while (slot < 0xf2)
    {
        if (m_pCreatureSlot[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)1;
    slot = 0;
    while (slot <= 0x68)
    {
        if (m_pAvatarSlot[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    return 0;
}

int CInventory::tryInsertAvatarIntoInventory(Inven_Item item) const
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::insertAvatarIntoInventory(Inven_Item item, eAvatarItemAddReason reason, bool bLog)
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            m_pAvatarSlot[i] = item;
            if (bLog)
            {
                const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
                    m_pAvatarSlot[i].get_add_info());
                m_pParent->m_historyLog.AvatarItemAdd(
                    item.m_addInfo, m_pAvatarSlot[i].get_add_info(),
                    agency == 0 ? "" : agency, reason);
            }
            return i;
        }
    }
    return -1;
}

int CInventory::insertAvatarIntoSpecificSlot(Inven_Item item, int slot,
                                              eAvatarItemAddReason reason, bool bLog)
{
    if (slot >= 0x69)
    {
        return -1;
    }
    int result = -1;
    if (m_pAvatarSlot[slot].m_addInfo == 0)
    {
        m_pAvatarSlot[slot] = item;
        result = slot;
    }
    else
    {
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                m_pAvatarSlot[i] = item;
                result = i;
                break;
            }
        }
    }
    if (result >= 0 && bLog)
    {
        const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
            m_pAvatarSlot[result].get_add_info());
        m_pParent->m_historyLog.AvatarItemAdd(
            item.m_addInfo, m_pAvatarSlot[result].get_add_info(),
            agency == 0 ? "" : agency, reason);
    }
    return result;
}

int CInventory::use_equip(int slot)
{
    if (slot < 0 || slot > 0x19)
    {
        return 1;
    }
    if (m_invenItem[slot].m_field1 != 1)
    {
        return 0x13;
    }
    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(m_invenItem[slot].m_addInfo);
    if (pItem == 0)
    {
        return 0x11;
    }
    if (m_invenItem[slot].m_fieldb == 0 || pItem->get_endurance() == 0)
    {
        return 0xb;
    }
    if (pItem->m_itemType < 10)   // ORIG 0x84ff81c：CEquipItem+0x234（m_itemType）< 10 → 不可用
    {
        return 0xb;
    }
    if (m_invenItem[slot].m_addInfo == 0)
    {
        return 0x11;
    }
    if (m_invenItem[slot].m_fieldb == 0)
    {
        return 0x16;
    }
    if (m_invenItem[slot].m_fieldb != 0)
    {
        m_invenItem[slot].m_fieldb--;
    }
    return 0;
}

int CInventory::try_use_item(int slot, int count, int invenType) const
{
    // b28c-L2 [N7 封顶]：唯一虚调用 pItem->GetItemType()（ORIG 0x0850… 区间）为
    // `mov (%eax),%edx; mov -0xc(%ebp),%eax; mov %eax,(%esp); call *%edx`，4.4-O0
    // 对间接 call 目标恒 %eax（N7，附 4.1 按上下文选 edx/ecx）⇒ 4 行寄存器名差封顶。
    if (count < 0)
    {
        return 1;
    }
    if (invenType == 7)
    {
        return 1;
    }
    if ((slot < 3 || slot > 8) && (slot < 0x39 || slot > 0x68))
    {
        return 4;
    }
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 0x17;
    }
    CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo);
    if (pItem == 0)
    {
        return 1;
    }
    if (pItem->GetItemType() == 0x12)
    {
        return 0;
    }
    if (m_pEquipSlot[slot].m_addInfo2 < count)
    {
        return 1;
    }
    return 0;
}

void CInventory::checkEquipmentState(int a, int b, int& c, int& d, int& e, int& f, int& g,
                                     int& h)
{
    // b28d-solo-CInventory [173/171；P223 岛已复现，余 2 条跨版本 codegen 封顶]
    // ORIG 0x850ce40：①两守卫 `cmpl $8; jg <nop 岛>`/`cmpl $4; jg <nop 岛>`
    // （+0x20c/+0x20f 两独立 landing）由双否定 goto 尾标签串复现；②for 内
    // `if (i <= 0x13)` 翻正条件 `i > 0x13` 复现 ORIG「else 直落 + then 后置跳转」。
    // [b28d-Inventory2 本轮] 循环内两岛（0x850d033 nop;jmp / 0x850d036 nop）由
    // `if (addInfo==0) goto l_ces_island1; … if (i>0x10) goto l_ces_island2;`
    // + 尾标签串 `l_ces_island1:; continue; l_ces_island2:;` 复现（P223），gap 6→2。
    // 残余 2 条跨版本 codegen 无源形旋钮（探针 f1/f2/f5/f6 实测）：①`mov 0x10(%ebp),%ecx`
    // 先载形参（P75，4.4 恒 `add mem,%eax`；unsigned cast 反产 3 条中转）②`shl $2`+
    // `mov TBL(%eax)` 强度归约（N4/N86b，4.4 恒 scaled `mov TBL(,%eax,4)`；指针算术
    // 反产 `add $TBL,%eax; mov (%eax)` 3 条）⇒ 判语义完成、byte 层余 2 条。
    if (a > 8) goto l_ces_skip1;
    if (b > 4) goto l_ces_skip2;
    {
        if (m_invenItem[10].m_addInfo == 0)
        {
            c++;
        }
        else if (m_invenItem[10].m_fieldb == 0)
        {
            f++;
        }
        for (int i = 0xc; i < 0x16; i++)
        {
            // b28d-solo-CInventory [P59/P222]：ORIG 为 else 块直落、then 块后置跳转
            // （`cmpl $0x13; jle <then>`）⇒ 源码翻转成正条件 `i > 0x13` 包 else 体，
            // 4.4 即产 `jle <else>` 与 ORIG 同布局。
            if (i > 0x13)
            {
                // b28d-Inventory2 [P223]：ORIG 0x850ceb9 `je 0850d033`（岛1 nop;jmp
                // 到循环尾）⇒ 裸条件 goto + 尾空标签串；标签序 = 守卫序。
                if (m_invenItem[i].m_addInfo == 0) goto l_ces_island1;
                g++;
            }
            else
            {
                if (m_invenItem[i].m_addInfo == 0)
                {
                    c++;
                }
                if (i > 0x10) goto l_ces_island2;
                {
                    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(
                        m_invenItem[i].m_addInfo);
                    if (pItem != 0)
                    {
                        if (m_invenItem[i].m_fieldb == 0)
                        {
                            f++;
                        }
                        int subType = pItem->get_sub_type();
                        if (a == 4 && b == 3)
                        {
                            if (subType == 0 || subType == 3 || subType == 4)
                            {
                                h++;
                            }
                            else
                            {
                                d += 2;
                                e = 2;
                            }
                        }
                        // ORIG 0x850cfbe：mov _S_CHARAC_JOB_MASTARY(,%eax,4)（eax=a*5+b）
                        // —— int 数组下标读（checkEquipmentState.md）。
                        // [b28d-PoolMisc2 实测 2 形均不闭合] ①`b + a*5` 加数交换 ⇒ 4.4 仍
                        // `add 0x10(%ebp),%eax`（零效）；②`(a*5)` 改显式指针算术
                        // `*(int*)((char*)TBL + ((a*5)<<2))` ⇒ 复现 ORIG `shl $2` 但 4.4 改产
                        // `add $TBL,%eax; mov (%eax)`（1 条差异换成 1 条，ndiff 8→8，指令数
                        // 169→171 无收益，已回滚）。⇒ P75/N86b 封顶维持。
                        else if (_S_CHARAC_JOB_MASTARY[a * 5 + b] == subType)
                        {
                            h++;
                        }
                        // ORIG 0x850cfe6：mov _S_CHARAC_JOB_MASTARY(%eax)，eax=(a*5)*4。
                        else if (_S_CHARAC_JOB_MASTARY[a * 5] == subType)
                        {
                            d++;
                            e = e == 2 ? 2 : 1;
                        }
                        else
                        {
                            d += 2;
                            e = 2;
                        }
                    }
                }
            }
        l_ces_island1:;
            continue;
        l_ces_island2:;
        }
    }
l_ces_skip1:;
l_ces_skip2:;
}

int CInventory::erase_repeated_item(const std::vector<std::pair<int, int> >& items,
                                    std::vector<std::pair<int, int> >& out)
{
    // b28d-solo-CInventory：158/159，帧/槽残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差（158/159）。
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        int need = it->second;
        for (int i = start; i <= end; i++)
        {
            if (IsCreatureItemType(type))
            {
                if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                {
                    if (isEquipableItemType(type))
                    {
                        need--;
                    }
                    else
                    {
                        need -= m_pCreatureSlot[i].m_addInfo2;
                    }
                }
            }
            else if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                if (m_pEquipSlot[i].m_field1 == 1)
                {
                    need--;
                }
                else
                {
                    need -= m_pEquipSlot[i].m_addInfo2;
                }
            }
        }
        if (it->second > 0)
        {
            out.push_back(std::make_pair(itemIdx, need));
        }
    }
    return out.size();
}

// ---- 事件物品批处理（ORIG 0x85058da / 0x85067b2 / 0x8505db4 / 0x8506bb2 / 0x8508fd0 / 0x8509466）----

bool CInventory::tryDeleteEventItems(const std::vector<std::pair<int, int> >& items)
{
    // b28d-solo-CInventory：362/347，P167 MISSING ppEi×2 ⇒ N113 封顶。
    if (m_pParent->getGmQuestFlag())
    {
        return true;
    }
    int okCount = 0;
    if (items.size() == 0)
    {
        return false;
    }
    // b28d-37：同 tryInsertEventItems——P167 差集 MISSING ppEi/C1Ev/pair::C1Ev/aSEOS0_，
    // 按 P50 反例复活（先声明后赋值）+ P135（具名 pair 右值赋值）改形。
    // [结果] our 633→651（ORIG 675），仅剩 ppEi 残差 ⇒ N 封顶，语义闭合。
    std::pair<int, int> entry;
    std::vector<std::pair<int, int> >::const_iterator it;
    it = items.begin();
    while (it != items.end())
    {
        bool found = false;
        entry = std::pair<int, int>(*it);
        int itemIdx = entry.first;
        int amount = entry.second;
        if (itemIdx == 0)
        {
            if (use_money(amount, (eMoneySubReason)4, false))
            {
                okCount++;
            }
            it++;
            continue;
        }
        else if (itemIdx == 2)
        {
            if ((unsigned int)((CUserCharacInfo*)m_pParent)->GetWinPoint() >=
                (unsigned int)amount)
            {
                okCount++;
            }
            it++;
            continue;
        }
        else
        {
            int type = GetItemType(itemIdx);
            if (type == 0)
            {
                continue;
            }
            int start;
            int end;
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        okCount++;
                        m_pEquipSlot[i].reset();
                        amount = amount - 1;
                        if (amount == 0)
                        {
                            break;
                        }
                    }
                }
                if (amount > 0)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            okCount++;
                            m_pEquipSlot[i].reset();
                            amount = amount - 1;
                            if (amount == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (amount > 0)
                {
                    for (int i = 0; i < 0x1a; i++)
                    {
                        if (m_invenItem[i].m_addInfo == itemIdx)
                        {
                            okCount++;
                            m_invenItem[i].reset();
                            amount = amount - 1;
                            if (amount == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (amount == 0)
                {
                    found = true;
                }
            }
            else
            {
                for (int i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        amount <= m_pEquipSlot[i].m_addInfo2)
                    {
                        if (m_pEquipSlot[i].m_addInfo2 == amount)
                        {
                            m_pEquipSlot[i].reset();
                        }
                        else
                        {
                            m_pEquipSlot[i].m_addInfo2 -= amount;
                        }
                        found = true;
                        okCount++;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            amount <= m_pEquipSlot[i].m_addInfo2)
                        {
                            if (m_pEquipSlot[i].m_addInfo2 == amount)
                            {
                                m_pEquipSlot[i].reset();
                            }
                            else
                            {
                                m_pEquipSlot[i].m_addInfo2 -= amount;
                            }
                            found = true;
                            okCount++;
                            break;
                        }
                    }
                }
            }
        }
        it++;
    }
    if (okCount != (int)items.size())
    {
        return false;
    }
    return true;
}

int CInventory::tryInsertEventItems(const std::vector<std::pair<int, int> >& items)
{
    // b28d-solo-CInventory 复修：ORIG 297 条。旧判词漏掉三处真结构：
    // ① entry/inven 在循环外声明（ORIG pair::C1Ev@+0x02c、Inven_Item::C1Ev@+0x063 各一次）；
    // ② inven.m_addInfo=itemIdx 在 is_stackable 之前（ORIG +0x144 mov -0x14→-0x83）；
    // ③ make_item 在三个分支内各调一次（ORIG +0x166/+0x24c/+0x321），非循环外一次；
    // ④ insertItem/Avatar 返回值先落具名槽 -0x10 再比较（ORIG mov %eax,-0x10）。
    int insertCount = 0;
    CItem* pItem;
    int count;
    int itemIdx;
    int slot;
    if (items.size() == 0)
    {
        return 0;
    }
    std::pair<int, int> entry;
    std::vector<std::pair<int, int> >::const_iterator it;
    it = items.begin();
    Inven_Item inven;
    while (it != items.end())
    {
        entry = std::pair<int, int>(*it);
        itemIdx = entry.first;
        count = entry.second;
        if (itemIdx == 0)
        {
            insertCount++;
            it++;
            continue;
        }
        if (itemIdx == 1)
        {
            insertCount++;
            it++;
            continue;
        }
        pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            it++;
            return insertCount;
        }
        inven.m_addInfo = itemIdx;
        if (pItem->is_stackable())
        {
            inven.m_addInfo2 = count;
            pItem->make_item(inven);
            slot = insertItemIntoInventory(inven, (eItemAddReason)8, false, true);
            if (slot == -1)
            {
                goto fail_return;
            }
            insertCount++;
        }
        else
        {
            if (pItem->IsAvatarItem())
            {
                pItem->make_item(inven);
                slot = insertAvatarIntoInventory(inven, eAvatarItemAddReason_0, false);
                if (slot == -1)
                {
                    goto fail_return;
                }
                insertCount++;
            }
            else
            {
                for (int i = 0; i < count; i++)
                {
                    pItem->make_item(inven);
                    slot = insertItemIntoInventory(inven, (eItemAddReason)8, false, true);
                    if (slot == -1)
                    {
                        break;
                    }
                    insertCount++;
                }
            }
        }
        it++;
    }
fail_return:
    return insertCount;
}

int CInventory::delete_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, bool& bFound,
                                   eMoneySubReason moneyReason, eItemDelReason itemReason,
                                   bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 675 / 我 674（gap 1，p167 CLEAN）。旧判词「ppEi 残差」被推翻，
    // 真结构差四处：①need 副本应为 amount 直接递减（ORIG +0x336 mov/sub/mov 三段）；
    // ②entry 声明在循环外（ORIG pair::C1Ev@+0x058）；③found 初始化在循环体首（ORIG +0x08e movb）；
    // ④start/end 无 =0 初始化（ORIG 直接传引用）。残余 1 = 局部与常量比较 4.4 恒折 mem 直比（N18b）。
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
    int deleteCount = 0;
    if (items.size() == 0)
    {
        return 0;
    }
    // P50 先声明后赋值 + P135 具名 pair 右值赋值保持。
    std::pair<int, int> entry;
    std::vector<std::pair<int, int> >::const_iterator it;
    it = items.begin();
    while (it != items.end())
    {
        bool found = false;
        entry = std::pair<int, int>(*it);
        int itemIdx = entry.first;
        int amount = entry.second;
        if (itemIdx == 0)
        {
            if (use_money(amount, moneyReason, true))
            {
                deleteCount++;
                outSlots.push_back(0);
                outSlots.push_back(0);
                outSlots.push_back(amount);
            }
            if (moneyReason == (eMoneySubReason)4 && m_pParent != 0)
            {
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x11, m_pParent, (unsigned int)amount);
            }
            it++;
            continue;
        }
        else if (itemIdx == 2)
        {
            if ((unsigned int)((CUserCharacInfo*)m_pParent)->GetWinPoint() >=
                (unsigned int)amount)
            {
                m_pParent->UseWinPoint(amount, (eWPSubReason)3);
                deleteCount++;
                outSlots.push_back(0);
                outSlots.push_back(2);
                outSlots.push_back(amount);
            }
            it++;
            continue;
        }
        else
        {
            int type = GetItemType(itemIdx);
            if (type == 0)
            {
                continue;
            }
            int start;
            int end;
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        deleteCount++;
                        outSlots.push_back(0);
                        outSlots.push_back(i);
                        outSlots.push_back(m_pEquipSlot[i].m_addInfo2);
                        delete_item(INVEN_TYPE_EQUIP, i, 1, itemReason, true);
                        amount = amount - 1;
                        if (amount == 0)
                        {
                            break;
                        }
                    }
                }
                if (amount > 0)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            deleteCount++;
                            outSlots.push_back(0);
                            outSlots.push_back(i);
                            outSlots.push_back(m_pEquipSlot[i].m_addInfo2);
                            delete_item(INVEN_TYPE_EQUIP, i, 1, itemReason, true);
                            amount = amount - 1;
                            if (amount == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (amount > 0)
                {
                    for (int i = 0; i < 0x1a; i++)
                    {
                        if (m_invenItem[i].m_addInfo == itemIdx)
                        {
                            if (i < 0xc)
                            {
                                bFound = true;
                            }
                            deleteCount++;
                            outSlots.push_back(3);
                            outSlots.push_back(i);
                            outSlots.push_back(m_invenItem[i].m_addInfo2);
                            delete_item(INVEN_TYPE_INVEN, i, 1, itemReason, true);
                            amount = amount - 1;
                            if (amount == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (amount == 0)
                {
                    found = true;
                }
            }
            else
            {
                for (int i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        amount <= m_pEquipSlot[i].m_addInfo2)
                    {
                        delete_item(INVEN_TYPE_EQUIP, i, amount, itemReason, true);
                        found = true;
                        deleteCount++;
                        outSlots.push_back(0);
                        outSlots.push_back(i);
                        outSlots.push_back(amount);
                        if (m_pParent != 0 && (int)itemReason == 8 && bLog)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x65);
                        }
                        else if (m_pParent != 0 && (int)itemReason == 8 && !bLog)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x64);
                        }
                        else if (m_pParent != 0 && (int)itemReason == 4)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x66);
                        }
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            amount <= m_pEquipSlot[i].m_addInfo2)
                        {
                            delete_item(INVEN_TYPE_EQUIP, i, amount, itemReason, true);
                            found = true;
                            deleteCount++;
                            outSlots.push_back(0);
                            outSlots.push_back(i);
                            outSlots.push_back(amount);
                            if (m_pParent != 0 && (int)itemReason == 8 && bLog)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x65);
                            }
                            else if (m_pParent != 0 && (int)itemReason == 8 && !bLog)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x64);
                            }
                            else if (m_pParent != 0 && (int)itemReason == 4)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x66);
                            }
                            break;
                        }
                    }
                }
            }
            if (!found)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1d3a, 5);
                trace("NOT CRITICAL BUT FOR OBSERVING, No Event Item %d FOUND!", itemIdx);
                return 0;
            }
        }
        it++;
    }
    if (deleteCount != (int)items.size())
    {
        deleteCount = 0;
    }
    return deleteCount;
}

int CInventory::insert_item_special_slot(Inven_Item item, int a, int b,
                                         eMoneyAddReason moneyReason, eItemAddReason itemReason)
{
    // b28d-solo-CInventory：429/371，P167 EXTRA setCopy×2（ORIG 内联拷贝）⇒ N82/N84 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差；P167 EXTRA `Inven_Item::setCopy`×2 系 ORIG 内联拷贝形差异（无 MISSING）；429/371。
    if (item.m_addInfo == 0)
    {
        if (a < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1742, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x1742);
            return -2;
        }
        gain_money(a, moneyReason, true, 0);
        return 0;
    }
    if (item.m_field1 == 1)
    {
        if (m_pEquipSlot[b].m_addInfo == 0)
        {
            m_pEquipSlot[b] = item;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemAdd(
                    INVEN_TYPE_EQUIP, 1, 1, item, itemReason);
            }
            return b;
        }
    }
    else
    {
        for (int i = 3; i < 9; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == item.m_addInfo)
            {
                if (0x7fffffff - a < m_pEquipSlot[i].m_addInfo2)
                {
                    if (m_pParent != 0)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x177c, 4)
                            ("INVEN INT_MAX setting 3 : mid(%d),characNo(%d),inven(%d)+add(%d) item_id(%d)",
                             NumberToString(m_pParent->get_acc_id(), 0),
                             m_pParent->get_charac_no(-1), m_pEquipSlot[i].m_addInfo2, a,
                             item.m_addInfo);
                    }
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, itemReason);
                }
                return i;
            }
        }
        int start;
        int end;
        StartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == item.m_addInfo)
            {
                if (0x7fffffff - a < m_pEquipSlot[i].m_addInfo2)
                {
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                    if (m_pParent != 0)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x179f, 4)
                            ("INT_MAX setting 3 : mid(%d),characNo(%d), inven(%d)+add(%d)",
                             NumberToString(m_pParent->get_acc_id(), 0),
                             m_pParent->get_charac_no(-1), m_pEquipSlot[i].m_addInfo2, a);
                    }
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, itemReason);
                }
                return i;
            }
        }
        if (m_pEquipSlot[b].m_addInfo == 0)
        {
            m_pEquipSlot[b] = item;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemAdd(
                    INVEN_TYPE_EQUIP, m_pEquipSlot[b].get_add_info(), item.get_add_info(), item,
                    itemReason);
            }
            return b;
        }
    }
    return -1;
}

void CInventory::MoveClearAvatar(int a, int b, int c, int d, int e, int f)
{
    // b28d-solo-CInventory：268/273，N7 间接 call 目标寄存器封顶（4.4.7 后端固有）。
    // b28d-37 [N7 封顶，ORIG 0x850?]：间接 call 目标寄存器 %eax↔%edx 互换（P230 换后端可翻）；268/273。
    CItem* pClearItem = 0;
    CItem* pTargetItem = 0;
    if (c != 0)
    {
        pClearItem = G_CDataManager()->find_item(c);
        if (pClearItem != 0 && pClearItem->IsClearAvatar() && f != 0)
        {
            pTargetItem = G_CDataManager()->find_item(f);
            if (pTargetItem != 0 && !pTargetItem->IsClearAvatar())
            {
                SetClearAvatar(d, f);
                if (d < 0 || d > 0x19)
                {
                    return;
                }
                stAvatarExpansionInfo_t* expInfo =
                    (stAvatarExpansionInfo_t*)GetAvatarItemMgrW()->GetExpansionInfo(
                        m_invenItem[d].m_addInfo2);
                if (expInfo == 0)
                {
                    return;
                }
                if (a - 10 < 0 || a - 10 > 0x68)
                {
                    return;
                }
                Inven_Item* slotItem = &m_pAvatarSlot[a - 10];
                std::map<int, AvatarVariation::colorRGB>& colorMap =
                    G_CDataManager()->m_avatarColor.m_colorMap;
                if (colorMap.find((int)expInfo->m_color[0]) == colorMap.end())
                {
                    LogManager::logFormat(
                        1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21db,
                        "Avatar color error. ItemID(%d), colorId(%d)",
                        m_invenItem[d].m_addInfo2, (int)expInfo->m_color[0]);
                    return;
                }
                if (colorMap.find((int)expInfo->m_color[1]) == colorMap.end())
                {
                    LogManager::logFormat(
                        1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21e2,
                        "Avatar color error. ItemID(%d), colorId(%d)",
                        m_invenItem[d].m_addInfo2, (int)expInfo->m_color[1]);
                    return;
                }
                if (!GetAvatarItemMgrW()->SetAvatarColor(
                        slotItem->m_addInfo2, expInfo->m_color[0], expInfo->m_color[1]))
                {
                    return;
                }
                SendAvatarEvent(0x31a, slotItem->m_addInfo2, expInfo->m_color[0],
                                expInfo->m_color[1], 0);
            }
        }
    }
    if (f != 0)
    {
        if (pTargetItem == 0)
        {
            pTargetItem = G_CDataManager()->find_item(f);
        }
        if (pTargetItem != 0 && pTargetItem->IsClearAvatar())
        {
            SetClearAvatar(d, 0);
        }
    }
}
// [N82 封顶] batch-10 新立项：差集 30=30 归零（parent==0 改 cMyTrace(0x212a,5) 形、
// 0x2c3/0x31a 改 GetInBuffer<SIG_AVATAR_OPTION/COLOR_CHANGE> 就地写域、五处 char buf
// 去 memset——ORIG 未初始化 pad 原样发送，忠实保留）。残余 +13 = 非对齐字段写形差。

void CInventory::SendAvatarEvent(int a, int b, int c, int d, int e) const
{
    // b28d-solo-CInventory：249/262，比较链块序 + 槽残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：bool 物化/槽残差（249/262）。
    if (m_pParent == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x212a, 5);
        trace("CInventory::SendAvatarEven ERROR, m_pParent NULL ");
        return;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x212f), true);
    int characNo = m_pParent->get_charac_no(-1);
    **guard << a;
    **guard << (int)m_pParent->GetUID();
    unsigned char b0 = (unsigned char)b;
    if (a == 0x34)
    {
        char buf[7];
        *(int*)(buf + 0) = characNo;
        buf[4] = (char)b0;
        *(short*)(buf + 5) = (short)c;
        guard->put_binary(buf, 7);
    }
    else if (a < 0x35)
    {
        if (a == 0x32)
        {
            char buf[0x16];
            *(int*)(buf + 0) = characNo;
            *(int*)(buf + 4) = d;
            *(int*)(buf + 8) = e;
            buf[0xc] = (char)b0;
            buf[0xd] = (char)c;
            // ORIG（0x08508dfd/+0x1cf 段）：+0x0e 槽判定用 (byte)c、写 e；
            // +0x12 槽判定用 (byte)b、写 d。结构未初始化字节按 ORIG 原样发送。
            *(int*)(buf + 0xe) = GetClearAvatar((unsigned char)c) != 0 ? e : 0;
            *(int*)(buf + 0x12) = GetClearAvatar(b & 0xff) != 0 ? d : 0;
            guard->put_binary(buf, 0x16);
        }
        else if (a == 0x31)
        {
            char buf[0x53];
            *(int*)(buf + 0) = characNo;
            *(int*)(buf + 4) = b;
            // ORIG 布局（0x08508d5a 起）：[0]characNo、[4]b、[8](char)c、
            // [0xa]d、[0xe](char)e；pad 字节不初始化（ORIG 原样）。
            buf[8] = (char)c;
            *(int*)(buf + 0xa) = d;
            buf[0xe] = (char)e;
            guard->put_binary(buf, 0x53);
        }
        else
        {
            char buf[0x70c];
            *(int*)(buf + 0) = 1;
            *(int*)(buf + 4) = characNo;
            buf[8] = (char)b0;
            guard->put_binary(buf, 0x70c);
        }
    }
    else if (a == 0x2c3)
    {
        // ORIG 0x08508f0f：GetInBuffer<SIG_AVATAR_OPTION_CHANGE> 就地写域。
        SIG_AVATAR_OPTION_CHANGE* p =
            guard->GetInBuffer<SIG_AVATAR_OPTION_CHANGE>();
        p->m_uiId = b;
        p->m_field4 = (char)c;
        p->m_characNo = characNo;
    }
    else if (a == 0x31a)
    {
        // ORIG 0x08508f43：GetInBuffer<SIG_AVATAR_COLOR_CHANGE> 就地写域。
        SIG_AVATAR_COLOR_CHANGE* p =
            guard->GetInBuffer<SIG_AVATAR_COLOR_CHANGE>();
        p->m_characNo = characNo;
        p->m_uiId = b;
        p->m_field8 = (short)c;
        p->m_fieldA = (short)d;
    }
    else if (a == 0x36)
    {
        // ORIG GetInBuffer<SIG_AVATAR_CHANGE_STAT>（0x08508ed2 段，sizeof 0x51）：
        // +0x00=b(int ui_id)、+0x04=c(int it_id)、+0x08=(char)d(slot)、
        // +0x09=characNo(int 非对齐 movl)。（参数 e 未用）
        SIG_AVATAR_CHANGE_STAT* s =
            guard->GetInBuffer<SIG_AVATAR_CHANGE_STAT>();
        s->m_uiId = b;
        s->m_itId = c;
        s->m_slot = (unsigned char)d;
        s->m_characNo = characNo;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CInventory::ExpiredAvatarItem(PacketGuard* packet)
{
    int now = OS_API::GetDateTimeTick();
    // b28c-L2: ORIG 0x8509501/0x8509524 两 je 目标差 1 字节（cond1 落 nop 岛、
    // cond2/3 落 addl）⇒ 原码是「外层 m_addInfo!=0 + 内层短路」双层嵌套，
    // 非单条 && 短路链（4.4 对合并链三 je 同目标）。
    // 续（b28c-L2）：ORIG body 尾 `jmp <addl>` + nop 岛 ⇒ 负条件 goto 尾标签形
    // （探针 l2_probe6 Z 命中，4.4 产 je/jge 尾标签 + body 尾 jmp）。
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo == 0) goto next_inven;
        if (m_invenItem[i].m_addInfo2 == 0 || m_invenItem[i].m_addInfo2 >= now) goto next_inven;
        SendAvatarEvent(0x33, m_pParent->get_charac_no(-1), i, 0, 0);
        m_invenItem[i].reset();
    next_inven:;
    }
    for (int i = 0; i < 0x69; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0) goto next_avatar;
        if (m_pAvatarSlot[i].m_addInfo2 == 0 || m_pAvatarSlot[i].m_addInfo2 >= now) goto next_avatar;
        SendAvatarEvent(0x33, m_pParent->get_charac_no(-1), i + 10, 0, 0);
        m_pAvatarSlot[i].reset();
    next_avatar:;
    }
}

int CInventory::CalBlankSlot(int* out,
                             const std::vector<std::pair<int, int> >& items) const
{
    // b28d-solo-CInventory：312/326，分支/帧残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：分支/帧残差（312/326）。
    if (items.size() == 0)
    {
        return 0;
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int count = it->second;
        if (itemIdx == 0 || itemIdx == 2)
        {
            continue;
        }
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        if (type == 1)
        {
            int found = 0;
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == itemIdx)
                {
                    found++;
                    out[1]++;
                    if (found == count)
                    {
                        break;
                    }
                }
            }
            if (found < count)
            {
                for (int i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        found++;
                        out[0]++;
                        if (found == count)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (IsCreatureItemType(type))
            {
                if (isEquipableItemType(type))
                {
                    int found = 0;
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            found++;
                            out[type]++;
                            if (found == count)
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    bool found = false;
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                        {
                            found = true;
                            out[type]++;
                            break;
                        }
                    }
                    if (!found)
                    {
                        for (int i = 3; i < 9; i++)
                        {
                            if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                                (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                            {
                                out[0]++;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                bool found = false;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                    {
                        found = true;
                        out[type]++;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                        {
                            out[0]++;
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int CInventory::CalNeedSlot(int* out,
                            const std::vector<std::pair<int, int> >& items) const
{
    // b28d-solo-CInventory：238/239，帧/槽残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差（238/239）。
    if (items.size() == 0)
    {
        return 0;
    }
    // ORIG（0x0850891a）：begin 具名量 + pair 默认构造 cur、循环头 found=false、
    // cur = 临时 pair 右值赋值（范式 P-358）、it++ 后置自增 ×3。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    Iterator it = items.begin();
    std::pair<int, int> cur;
    while (it != items.end())
    {
        bool found = false;
        cur = std::pair<int, int>(*it);
        int itemIdx = cur.first;
        int count = cur.second;
        if (itemIdx == 0)
        {
            it++;
            continue;
        }
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            it++;
            continue;
        }
        if (type == 1)
        {
            out[type] += count;
        }
        else if (IsCreatureItemType(type))
        {
            if (isEquipableItemType(type))
            {
                out[type] += count;
            }
            else
            {
                int start;
                int end;
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                for (int i = start; i <= end; i++)
                {
                    if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    out[type]++;
                }
            }
        }
        else
        {
            if (type == 8)
            {
                if (getAvatarEmptySlot(0, 0x68) != -1)
                {
                    out[type]++;
                }
            }
            else
            {
                int start;
                int end;
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                {
                    out[type]++;
                }
            }
        }
        it++;
    }
    return 0;
}
// [N82 封顶] batch-10 新立项：差集 15=15 归零（while+it++ 后置×4、pair 临时&& 赋值、
// 循环头 found 初始化、winPoint !(>=) setge 形、i<=8/i<=0x19 setle 界）。gap 43→7。

bool CInventory::CheckNeedItemList(const std::vector<std::pair<int, int> >& items) const   // [b28d-Main 代落·S351 P292] int→bool
{
    // b28d-solo-CInventory [P212 全头部声明]：ORIG 0x08507b2b..0x08507b59 表明 8 个局部
    // （found/itemIdx/start/end/i/count/foundCnt/type=0xb）在循环前声明并初始化、
    // 循环内仅赋值；照此还原后 324/315 → 321/320（gap 9→1）。
    // 残余 1 = StartEndPos 实参装载序（P75 跨版本）+ 槽号，封 N82。
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
    if (items.size() == 0)
    {
        return 1;
    }
    // ORIG（0x08507acc）：具名 it=begin() + pair 默认构造 + 循环头 found=false +
    // 临时 pair 右值赋值 + it++ 后置 ×4（三个前置判定分支各自推进 + item 处理共用尾）。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    Iterator it = items.begin();
    std::pair<int, int> cur;
    // [b28d-solo] ORIG 0x08507b2b..0x08507b59：8 个局部在循环前声明并初始化
    // （found/itemIdx/start/end/i/count/foundCnt/type=0xb），循环内仅赋值（P212）。
    bool found = false;
    int itemIdx = 0;
    int start = 0;
    int end = 0;
    int i = 0;
    int count = 0;
    int foundCnt = 0;
    int type = 0xb;
    while (it != items.end())
    {
        found = false;
        cur = std::pair<int, int>(*it);
        itemIdx = cur.first;
        count = cur.second;
        if (itemIdx == 0)
        {
            if (m_money >= count)
            {
                it++;
            }
            else
            {
                return 0;
            }
        }
        else if (itemIdx == 2)
        {
            // ORIG 0x8507be8 call getPVPResultR → mov 0x58(%eax) = m_winPoint；
            // setge 物化形（正向 >= 判定，P222 正向包裹）。
            if (((CUserCharacInfo*)m_pParent)->getPVPResultR()->m_winPoint >= count)
            {
                it++;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            type = GetItemType(itemIdx);
            if (type == 0)
            {
                it++;
            }
            else
            {
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                if (type == 1)
                {
                    foundCnt = 0;
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo != itemIdx)
                        {
                            continue;
                        }
                        foundCnt++;
                        if (foundCnt == count)
                        {
                            break;
                        }
                    }
                    if (foundCnt < count)
                    {
                        for (i = 3; i <= 8; i++)
                        {
                            if (m_pEquipSlot[i].m_addInfo == itemIdx)
                            {
                                foundCnt++;
                                if (foundCnt == count)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if (foundCnt < count)
                    {
                        for (i = 0; i <= 0x19; i++)
                        {
                            if (m_invenItem[i].m_addInfo == itemIdx)
                            {
                                foundCnt++;
                                if (foundCnt == count)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if (foundCnt < count)
                    {
                        return 0;
                    }
                }
                else if (type == 5)
                {
                    // ORIG 0x8507d8a：mov 0x55c(%eax) = m_invenItem[22].m_addInfo。
                    if (m_invenItem[22].m_addInfo == itemIdx)
                    {
                        return 1;
                    }
                    for (i = start; i <= end; i++)
                    {
                        if (m_pCreatureSlot[i].m_addInfo != itemIdx)
                        {
                            continue;
                        }
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            if (m_pEquipSlot[i].m_addInfo2 < count)
                            {
                                return 0;
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        for (i = 3; i <= 8; i++)
                        {
                            if (m_pEquipSlot[i].m_addInfo == itemIdx)
                            {
                                if (m_pEquipSlot[i].m_addInfo2 < count)
                                {
                                    return 0;
                                }
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            return 0;
                        }
                    }
                }
                it++;
            }
        }
    }
    return 1;
}

// ===================== 使用 / 装备 / Avatar（第九批 I2 补全） =====================

int CInventory::use_item(int slot, int count, int invenType)
{
    // b28c-L2 [N7 封顶 · §6.2 口径]：本函数 458/458 指令，P167 调用点差集归零；
    // 唯一残差 4 行 = 一处 CItem 虚调用的 `%edx`↔`%eax` 目标寄存器互换
    // （4.4 间接 call 目标恒 %eax，范式库 N7）。语义完成、byte 层 N 封顶。
    if (count < 0)
    {
        LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__,
                              0xf91, "count(%d) < 0", count);
        return 1;
    }
    if (invenType == 7)
    {
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            return 0x17;
        }
        if (m_pParent != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_pCreatureSlot[slot].m_addInfo);
            if (pItem != 0)
            {
                int price = 0;
                G_Store()->GetSellItemPrice(m_pCreatureSlot[slot], pItem, (short)count, false,
                                            price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x15, m_pParent, (unsigned int)price);
            }
        }
        if (m_pCreatureSlot[slot].m_addInfo2 == count)
        {
            int result = m_creatureMgr.UseItem(&m_pCreatureSlot[slot], slot);
            if (result != 0)
            {
                return result;
            }
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemDel(
                    (INVEN_TYPE)3, 0, count, m_pCreatureSlot[slot], (eItemDelReason)3);
            }
            m_pCreatureSlot[slot].reset();
        }
        else
        {
            if (m_pCreatureSlot[slot].m_addInfo2 > count)
            {
            int result = m_creatureMgr.UseItem(&m_pCreatureSlot[slot], slot);
            if (result != 0)
            {
                return result;
            }
            m_pCreatureSlot[slot].m_addInfo2 -= count;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemDel(
                    (INVEN_TYPE)3, m_pCreatureSlot[slot].m_addInfo2, count,
                    m_pCreatureSlot[slot], (eItemDelReason)3);
            }
            }
            else
            {
                m_pCreatureSlot[slot].reset();
                LogManager::logFormat(
                    1, "inventory.cpp", __PRETTY_FUNCTION__, 0xfd5,
                    "%s - m_CreatureSlot[slot_no].add_info(%d) < count(%d)",
                    ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                    m_pCreatureSlot[slot].m_addInfo2, count);
                return 1;
            }
        }
        return 0;
    }
    if ((slot < 3 || slot > 8) && (slot < 0x39 || slot > 0x68))
    {
        return 4;
    }
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 0x17;
    }
    CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo);
    if (pItem == 0)
    {
        return 1;
    }
    if (pItem->GetItemType() == 0x12)
    {
        return 0;
    }
    if (m_pParent != 0)
    {
        int price = 0;
        G_Store()->GetSellItemPrice(m_pEquipSlot[slot], pItem, (short)count, false, price);
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x15, m_pParent,
                                                       (unsigned int)price);
    }
    if (m_pEquipSlot[slot].m_addInfo2 == count)
    {
        int result = m_creatureMgr.UseItem(&m_pEquipSlot[slot], slot);
        if (result != 0)
        {
            return result;
        }
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemDel(
                (INVEN_TYPE)1, 0, count, m_pEquipSlot[slot], (eItemDelReason)3);
        }
        m_pEquipSlot[slot].reset();
    }
    else
    {
        if (m_pEquipSlot[slot].m_addInfo2 > count)
        {
        int result = m_creatureMgr.UseItem(&m_pEquipSlot[slot], slot);
        if (result != 0)
        {
            return result;
        }
        m_pEquipSlot[slot].m_addInfo2 -= count;
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemDel(
                (INVEN_TYPE)1, m_pEquipSlot[slot].m_addInfo2, count, m_pEquipSlot[slot],
                (eItemDelReason)3);
        }
        }
        else
        {
            m_pEquipSlot[slot].reset();
            LogManager::logFormat(
                1, "inventory.cpp", __PRETTY_FUNCTION__, 0x1021,
                "%s - m_InvenSlot[slot_no].add_info(%d) < count(%d)",
                ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                m_pEquipSlot[slot].m_addInfo2, count);
            return 1;
        }
    }
    return 0;
}

int CInventory::insertCreatureIntoSpecificSlot(Inven_Item item, int slot,
                                                eItemAddReason reason, bool bLog)
{
    if (slot >= 0xf2)
    {
        return -1;
    }
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int result = -1;
    if (m_pCreatureSlot[slot].m_addInfo == 0)
    {
        m_pCreatureSlot[slot] = item;
        result = slot;
    }
    else if (!m_pCreatureSlot[slot].isEquipableItemType())
    {
        if (m_pCreatureSlot[slot].m_addInfo == item.m_addInfo)
        {
            if (!checkStackableLimit((unsigned long)m_pCreatureSlot[slot].m_addInfo,
                                     m_pCreatureSlot[slot].get_add_info() + item.get_add_info()))
            {
                return -10;
            }
            m_pCreatureSlot[slot].set_add_info(
                m_pCreatureSlot[slot].get_add_info() + item.get_add_info());
            result = slot;
        }
        else
        {
            for (int i = 0; i < 0xf2; i++)
            {
                if (m_pCreatureSlot[i].m_addInfo == 0)
                {
                    m_pCreatureSlot[i] = item;
                    result = i;
                    break;
                }
                if (m_pCreatureSlot[i].m_addInfo == item.m_addInfo)
                {
                    m_pCreatureSlot[i].set_add_info(
                        m_pCreatureSlot[i].get_add_info() + item.get_add_info());
                    result = i;
                    break;
                }
            }
        }
    }
    if (result >= 0 && item.m_field1 != 5 && bLog)
    {
        m_pParent->m_historyLog.CreatureItemAdd(
            (INVEN_TYPE)3, item.m_addInfo, m_pCreatureSlot[result].get_add_info(),
            item.get_add_info(), item.m_field1, reason);
    }
    return result;
}
// [N82 封顶] batch-13：差集归零、207/208 gap=1。残 1 = ORIG 跨调用以 %ecx 存参/装载时刻
// （`mov A(%ebp),%ecx`）vs 4.4.7 spill 直写形 + 尾 nop 豁免差，源层无操作柄。

int CInventory::AddAvatarItem(int itemIdx, int avatarUid, bool b, char c, char d,
                              const char* agency, eAvatarItemAddReason reason, bool e,
                              unsigned short f)
{
    // b28d-solo-CInventory：208/207。真修：getAvatarSocket 第 2 实参 c→d
    // （ORIG 0x08509c2d `movsbl -0x64(%ebp),%edx`，-0x64 是第 5 参 d 的溢出槽，
    // 序言 0x08509bb3 `mov 0x1c(%ebp),%ecx; mov %cl,-0x64`；c 在 -0x60）。
    // 残余 1 = 间接 call 寄存器角色互换（N7）+ 槽残差。
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0 || !pItem->IsAvatarItem())
    {
        return -1;
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    // [b28d-solo] ORIG 0x08509c2d `movsbl -0x64(%ebp),%edx`：-0x64 是第 5 参 d 的
    // 溢出槽（序言 0x08509bb3 `mov 0x1c(%ebp),%ecx; mov %cl,-0x64`），非 c(-0x60)。
    ((CEquipItem*)pItem)->getAvatarSocket((int)d, emblem);
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            int tick = OS_API::GetDateTimeTick();
            m_pAvatarSlot[i].m_addInfo = itemIdx;
            m_pAvatarSlot[i].m_addInfo2 = (int)m_avatarItemMgr.RegistNotAssignedAvatarItem(
                avatarUid, tick, agency, e, emblem);
            m_pAvatarSlot[i].m_field1 = 8;
            m_pAvatarSlot[i].m_fieldb = (short)c;
            m_pAvatarSlot[i].m_field0 = 0;
            // ORIG AddAvatarItem.md：0x8509d30 movw $0,0xd / 0x8509d60 movw %ax,0xd /
            // 0x8509d76 movw $0,0xf —— +0x0d/+0x0f 各 2 字节，同 union 低/高半字。
            m_pAvatarSlot[i].m_fieldSplit.m_fielddLo = 0;
            CItem* pHidden = pItem;
            if (pHidden->IsHiddenOption())
            {
                m_pAvatarSlot[i].m_fieldSplit.m_fielddLo = f;
            }
            m_pAvatarSlot[i].m_fieldSplit.m_avatarEndur = 0;
            m_pAvatarSlot[i].ResetItemAttr();
            stAvatarExpansionInfo_t expansion;
            expansion.init();
            _SaveAvatarItem(i, m_avatarItemMgr.GetExpireDate(m_pAvatarSlot[i].m_addInfo2), b, c,
                            emblem, agency, m_pAvatarSlot[i].m_fieldd, expansion);
            m_pParent->m_historyLog.AvatarItemAdd(
                itemIdx, 0xffffffff, agency, reason);
            return i;
        }
    }
    return -1;
}

// [N82 封顶] batch-13：三处 `start <= i` 翻 `i >= start` 复现 ORIG setge 向（已消除
// replace 块）；残余 gap=5 = ORIG 跨 MoveItemToEmptySlot 调用以 %esi/%ebx 持两下标
// （push/pop esi,ebx prologue）vs 4.4.7 caller-save 直用形，register-asm 强改有
// qsort 段串扰风险，判 N82 不强求。
int CInventory::SortInvenSlot(Inven_Item::ITEM_TYPE type)
{
    // b28d-solo-CInventory [P212/P332]：ORIG 0x08504a92..0x08504ac7 表明
    // start/end/i 于 StartEndPos 前清零、last 于其后赋值；三个外层循环复用同一 i
    // （4.4 块作用域各自占槽，4.1 复用）；内层两循环各用 j/k。
    // ORIG 0x08504bc4 在 cMyTrace ctor 前把 end/start 载入 esi/ebx（P227 临时链）。
    // ⇒ AE（169/169 norm；TU diff 31→30、ae 260→261）。
    int start = 0;
    int end = 0;
    int i = 0;
    int last;
    StartEndPos(type, start, end);
    last = 0;
    for (i = end; i >= start; i--)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            last = i;
            break;
        }
    }
    if (last == start || last == 0)
    {
        return 0x12;
    }
    bool full = true;
    for (i = start; i <= last; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            full = false;
            break;
        }
    }
    if (full)
    {
        return 0x12;
    }
    int dst = -1;
    for (i = end; i >= start; i--)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            int moveResult = MoveItemToEmptySlotOfInventory(i, start, end);
            if (moveResult != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x194d, 5)(
                    "MoveItemToEmptySlotOfInventory return %d ( index %d,s %d,e %d )",
                    moveResult, i, start, end);
                return moveResult;
            }
            int newLast = 0;
            for (int j = end; j >= start; j--)
            {
                if (m_pEquipSlot[j].m_addInfo != 0)
                {
                    newLast = j;
                    break;
                }
            }
            bool newFull = true;
            for (int k = start; k <= newLast; k++)
            {
                if (m_pEquipSlot[k].m_addInfo == 0)
                {
                    newFull = false;
                    break;
                }
            }
            if (newFull)
            {
                break;
            }
        }
    }
    return 0;
}

int CInventory::ChangeEquip(INVEN_TYPE invenType, int slot, int equipSlot)
{
    // b28d-solo-CInventory：124/123，两处 4.1 槽比零 setne 物化（P197④ 4.4 恒折直跳）+ 尾 nop ⇒ setcc-fold/N93附 封顶；P225 while+break 实测 127 条劣化已回退。
    // b28d-37 [setcc-fold + N93附 封顶，ORIG 0x084fc37e]：ORIG 两处 `cmpl $0,-N; setne %al; test` 为 4.1 槽比零物化（P197④ 4.4 恒折直跳）；具名 bool 局部实测槽化劣化；尾 nop 落点属 N93附。详注见 case 1/2 体。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 1;
    }
    switch (invenType)
    {
    case 1:
    {
        if (equipSlot <= 9)
        {
            return 0x15;
        }
        Inven_Item slotItem1 = GetInvenSlot(1, slot);
        if (slotItem1.m_field0 != 0)
        {
            int secuResult = GlobalData::s_pSecuProtectionField->Check(m_pParent, (SECURITY_PROTCTION)0x16);
            if (secuResult != 0)
            {
                return secuResult;
            }
        }
        if (m_pParent != 0)
        {
            if (m_pParent->CheckItemLock(invenType, slot) &&
                GetInvenSlot(1, slot).m_field0 != 0)
            {
                return 0xd5;
            }
        }
        int result = 0;
        result = move_item((INVEN_TYPE)0, equipSlot, (INVEN_TYPE)1, slot);
        // b28d-solo-CInventory [P225+P223]：ORIG `cmpl $0,-0x14; setne %al; test %al,%al;
        // je <nop 岛>` 为 4.1 槽比零物化（裸 if 4.4 恒折直跳）⇒ `while (r != 0) { return r; }`
        // 复现物化；两 case 各自的 `return 0` 在 ORIG 共享到函数尾且 case1 落 nop 岛、
        // case2 直落 mov$0（两独立 landing）⇒ 双 goto 尾标签串复现（探针 /tmp/probe_ce3.cpp fJ）。
        while (result != 0)
        {
            return result;
        }
        goto l_end1;
    }
    case 2:
    {
        if (equipSlot > 9)
        {
            return 0x15;
        }
        int result = 0;
        result = move_item((INVEN_TYPE)0, equipSlot, (INVEN_TYPE)2, slot);
        while (result != 0)
        {
            return result;
        }
        goto l_end2;
    }
    default:
    {
        goto l_end2;
    }
    }
l_end1:;
l_end2:;
    return 0;
}

bool IsCreatureItemType(int itemType)
{
    return itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_CREATURE_STACK ||
           itemType == Inven_Item::ITEM_TYPE_ARTIFACT;
}

bool isEquipableItemType(int itemType)
{
    return itemType == Inven_Item::ITEM_TYPE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_ARTIFACT;
}

int my_compare_unsigned_short(const void* a, const void* b)
{
    return *(const unsigned short*)a - *(const unsigned short*)b;
}

bool checkStackableLimit(unsigned long itemIdx, int count)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item != 0 && item->is_stackable())
    {
        CStackableItem* sitem = static_cast<CStackableItem*>(item);
        if (sitem->getStackableLimit() < count || count < 0)
        {
            return false;
        }
    }
    return true;
}

bool CheckEquipable(const Inven_Item& a, const Inven_Item& b)
{
    if (a.m_addInfo != 0)
    {
        if (a.m_field1 != Inven_Item::ITEM_TYPE_EQUIP &&
            a.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
        {
            return false;
        }
    }
    if (b.m_addInfo != 0)
    {
        if (b.m_field1 != Inven_Item::ITEM_TYPE_EQUIP &&
            b.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
        {
            return false;
        }
    }
    if (a.m_addInfo == 0 && b.m_addInfo == 0)
    {
        return false;
    }
    return true;
}

int getItemChecksum(int slot, int itemIdx, int a, int b, int c)
{
    // ORIG 0x889234d：先置 0 的局部复用；slot*10 产生 shl2/add/add 形态；/10 为 magic 常数除。
    int checksum = 0;
    checksum = (itemIdx + a + b + c) * (slot * 10) / 10;
    int high = checksum & 0xffff0000;
    checksum &= 0xffff;
    checksum += high >> 16;
    return checksum;
}
int finishItemChecksum(int* sum, int money, int coin, int a, int b)
{
    if (sum == 0)
    {
        return 0;
    }
    *sum = *sum + (money + coin + a);
    *sum = *sum ^ b;
    return *sum;
}

// ---- 迁移自 GameStubs.cpp 的 CInventory 桩方法（ORIG 符号签名已核对）----
// ORIG _ZN10CInventory15_SaveAvatarItemEiibcRK20stAvatarEmblemInfo_tPKcmRK23stAvatarExpansionInfo_t
// @0x08509e78：m_pParent 为空返回 0；否则经 DB_Q 投递 SIG_AVATAR_INSERT（0x31 头 +
// UID），set(槽位+10/到期/标志/agency/hidden) + setJewelSocket + sig+0x4f 写 expansion
// 前 4 字节，MsgQueueMgr::put(DB_Q)。
bool CInventory::_SaveAvatarItem(int slot, int expireDate, bool b, char c,
                                 const stAvatarEmblemInfo_t& emblem,
                                 const char* agency, unsigned long hidden,
                                 const stAvatarExpansionInfo_t& expansion)
{
    // b28d-solo-CInventory：117/117，ORIG push %edi + sub 0x5c vs 我方 sub 0x60（callee-saved 缓存差）⇒ N18a/N82 封顶。
    // b28d-37 [N18a/N82 封顶，ORIG 0x8509e78]：callee-saved push 差 + 槽位残差（117=117 指令数，语义闭合）。
    if (m_pParent == 0)
        return false;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x2360), true);
    **guard << 0x31;
    **guard << (int)m_pParent->GetUID();
    SIG_AVATAR_INSERT* sig = guard->GetInBuffer<SIG_AVATAR_INSERT>();
    unsigned int avatarUid = m_pAvatarSlot[slot].m_addInfo;
    int characNo = m_pParent->get_charac_no(-1);
    sig->set(characNo, avatarUid, slot + 10, expireDate, b, c, agency, 0, hidden);
    sig->setJewelSocket(emblem);
    // ORIG 0x8509fbf：mov (%edx),%edx; mov %edx,0x4f(%eax) —— 单条 movl 把
    // expansion 前 4 字节（m_color[0..1]）写入 m_expansion(+0x4f)。
    sig->m_expansion = *reinterpret_cast<const int*>(&expansion);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ---- SIG_AVATAR_INSERT 非静态成员（ORIG 0x850d44c，this=ins） ----
// ORIG 反汇编逐字段：emblem(+0x00..0x1c) 7 组 dword + 1 word 写入 this+0x2d..+0x4a，
// 与 stAvatarEmblemInfo_t(pack(1), 5×(u16+u32)=0x1e) 完整镜像。
// ORIG 0x850d44c 为 pack(1) 30B 整块 7 dword + 1 word 直拷（跨 stEmblemSocket
// 子字段，无字段语义）；逐子字段写会 10 写 ≠ 8 写而倒退，故用 pack(1) blob
// 视图的具名字段表达（基址仍为具名子对象，消裸偏移）。零扰动（同 disp 同宽）。
namespace
{
#pragma pack(push, 1)
struct EmblemBlobV
{
    int m_d0;               // +0x00
    int m_d1;               // +0x04
    int m_d2;               // +0x08
    int m_d3;               // +0x0c
    int m_d4;               // +0x10
    int m_d5;               // +0x14
    int m_d6;               // +0x18
    unsigned short m_tail;  // +0x1c
};
#pragma pack(pop)
static_assert(sizeof(EmblemBlobV) == 0x1e, "EmblemBlobV == stAvatarEmblemInfo_t size");
}  // namespace

void SIG_AVATAR_INSERT::setJewelSocket(const stAvatarEmblemInfo_t& emblem)
{
    // b28d-37：ORIG 0x850d44c 为 this 侧 disp 折叠（`mov %ecx,0x2d(%eax)` 7 dword
    // + 尾 `mov %dx,0x49(%eax)`），P82 视图取址形（reinterpret_cast<EmblemBlobV*>
    // (&m_jewelSocket)）每次重发 `mov 0x8(%ebp),%eax; add $0x2d,%eax` 使 22→44 条。
    // 结构整体赋值让 4.4 走成员链 disp 折叠（N84/P83 形，30B pack(1) 直拷）。
    m_jewelSocket = emblem;
}

// ============================================================================
// CInventory::move_item（ORIG 0x08500688，_ZN10CInventory9move_itemE10INVEN_TYPEiS0_i）。
// [§6.2 闭合 · b20] 帧 604=604 与 ORIG 全等；P167 调用点差集 80/80 归零
// （0 MISSING / 0 EXTRA，CREATURE 分支接收者已按 ORIG 0x08501752 改直接成员
// m_creatureMgr，删 2 处误加 GetCreatureMgrW 调用）；新鲜 .o insns 1279 vs
// ORIG 1282（gap 3）。剩余 byte 层残差属 N82（4.1 不按 lifetime 复用栈槽）
// 与跨版本寄存器物化，判「语义完成、N 封顶」。
// 语义（docs/class_func_reports/CInventory/move_item.md 逐指令对照）：
//   - 双端 CheckValidSlot 失败 → cMyTrace + 0x11；
//   - 源为 0..2 且目的为 CREATURE 时 swap 两端（CREATURE 恒为源）；
//   - EQUIP↔INVEN：isIdentified 成对校验、IsAvatarItem/IsCreatureItem/IsEquipable、
//     IsRandomOption&&m_random.m_random.empty 拒绝、PeriodAfterUnsealing 写 m_fieldb
//     剩余天数、装备时清 m_field0 并 seal_Item 日志、0xd1 服务限制分支；
//   - EQUIP↔EQUIP / AVATAR↔AVATAR 三段交换；AVATAR 分支 MoveClearAvatar/
//     SendAvatarEvent(0x32)；CREATURE 走 CCreatureMgr::SwapCreatureItem。
// [§6.2 巨件核销 · b25] 真修 1 处：EQUIP→AVATAR 段 find_item 后 ORIG 有
// vtbl+0x10(IsAvatarItem)==1 通过 + vtbl+0x14(IsCreatureItem)!=0 拒绝两次虚调用
// （CEquipItem vtable 0x08c96e28 槽 4/5 实证），重建漏写 IsCreatureItem 拒绝分支
// ⇒ 间接虚调用 12→13 与 ORIG 对齐、P167 80/80 保持归零；新鲜 .o 1290 vs 1282，
// 残差归因 N82（槽 -0x20 复用 2 次）+ N80（cMyTrace 变参先物化 edi/esi/ebx）
// + N6（位测 fold）+ 跨版本块排布，判「语义完成、N 封顶」。
// ============================================================================
int CInventory::move_item(INVEN_TYPE invenType, int slot, INVEN_TYPE dstType, int dstSlot)
{
    // b28d-solo-CInventory：1282/1290，P167 CLEAR（80/80）；N82/N80/N6 封顶。
    // b28d-37 [N82 封顶，ORIG 0x08500688]：P167 调用点差集 CLEAR（80/80）；帧/槽复用残差（1282/1290），语义闭合。
    // [b28d-tu-CInventory] 本轮复核（struct 1463 为 difflib 巨型件 autojunk
    //   口径放大，N126 已录）：主体 = ① cMyTrace 变参 4 实参 ORIG 于 ctor 前
    //   一次物化 edi/esi/ebx、4.4 于装载点逐条读 ⇒ N80；② `if (dstType==1)
    //   {EQUIP↔EQUIP 大块}` ORIG 4.1 大块后置（je <远块>）热路前置，4.4 恒
    //   then-inline ⇒ N104（insert_item 三形实测同族，源码级不可翻）；
    //   ③ sete/setne 物化方向 ×2（N6/P248 三档已试）。无具名中间局部可删
    //   （P329 不适用）、平移非恒定常数（4.1 专属槽族）⇒ 维持封顶。
    if (!(CheckValidSlot((int)invenType, slot) == 1 &&
          CheckValidSlot((int)dstType, dstSlot) == 1))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x112c, 5);
        trace("CInventory::move_item move_item return false 0, %d,%d,%d,%d",
              (int)invenType, slot, (int)dstType, dstSlot);
        return 0x11;
    }
    if ((unsigned)invenType < 3 && dstType == INVEN_TYPE_CREATURE)
    {
        std::swap<int>(slot, dstSlot);
        std::swap<INVEN_TYPE>(invenType, dstType);
    }
    if ((int)invenType == 1)
    {
        if ((int)dstType == 1)
        {
            // ---- EQUIP ↔ EQUIP ----
            int srcType = GetSlotItemType(slot);
            int dstItemType = GetSlotItemType(dstSlot);
            if (srcType == Inven_Item::ITEM_TYPE_INVALID ||
                dstItemType == Inven_Item::ITEM_TYPE_INVALID)
            {
                return 0x11;
            }
            if (srcType != 0 && dstItemType != 0 && srcType != dstItemType)
            {
                return 0x11;
            }
            if (srcType != 0 && m_pEquipSlot[dstSlot].m_addInfo != 0 &&
                (int)m_pEquipSlot[dstSlot].m_field1 != srcType)
            {
                return 0x11;
            }
            if (dstItemType != 0 && m_pEquipSlot[slot].m_addInfo != 0 &&
                (int)m_pEquipSlot[slot].m_field1 != dstItemType)
            {
                return 0x11;
            }
            Inven_Item tmp;
            tmp.setCopy(m_pEquipSlot[slot]);
            m_pEquipSlot[slot].setCopy(m_pEquipSlot[dstSlot]);
            m_pEquipSlot[dstSlot].setCopy(tmp);
            return 0;
        }
        if ((int)dstType > 1)
        {
            if ((int)dstType == 2)
            {
                return 0x11;
            }
            if ((int)dstType != 3)
            {
                return 0;
            }
            return 0x11;
        }
        if ((int)dstType != 0)
        {
            return 0;
        }
        // ---- EQUIP(src) → INVEN(dst)：脱下并换回原背包格 ----
        if (!CheckEquipable(m_pEquipSlot[slot], m_invenItem[dstSlot]))
        {
            return 0x11;
        }
        if (!m_pEquipSlot[slot].m_amp.isIdentified() ||
            !m_invenItem[dstSlot].m_amp.isIdentified())
        {
            return 0x11;
        }
        if (GetSlotItemType(slot) == Inven_Item::ITEM_TYPE_INVALID)
        {
            return 0x11;
        }
        Inven_Item tmp;
        tmp.setCopy(m_pEquipSlot[slot]);
        if (tmp.m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(tmp.m_addInfo);
            if (pItem == 0 || m_pParent == 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1276, 5);
                trace("CInventory::move_item move_item return false 4,%d", tmp.m_addInfo);
                return 0x11;
            }
            if (pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, dstSlot);
            if (equipable != 0)
            {
                return equipable;
            }
            if (pItem->IsRandomOption() && m_pEquipSlot[slot].m_random.m_random.empty())
            {
                return 0x11;
            }
            if (pItem->PeriodAfterUnsealing() != 0)
            {
                int nExpire = GlobalData::s_systemTime_.getCurSec() +
                              pItem->PeriodAfterUnsealing() * 0x15180;
                if (nExpire != 0)
                {
                    tmp.m_fieldb =
                        (unsigned short)((nExpire - 0x44a54a80) / 0x15180);
                }
            }
        }
        m_pEquipSlot[slot].setCopy(m_invenItem[dstSlot]);
        tmp.m_field0 = 0;
        m_invenItem[dstSlot].setCopy(tmp);
        return 0;
    }
    if ((int)invenType > 1)
    {
        if ((int)invenType == 2)
        {
            if ((int)dstType == 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1306, 5);
                trace("CInventory::move_item move_item return false 8");
                return 0x11;
            }
            if ((int)dstType == 2)
            {
                // ---- AVATAR ↔ AVATAR ----
                int srcAddInfo2 = m_pAvatarSlot[slot].m_addInfo2;
                int dstAddInfo2 = m_pAvatarSlot[dstSlot].m_addInfo2;
                Inven_Item tmp;
                tmp.setCopy(m_pAvatarSlot[slot]);
                m_pAvatarSlot[slot].setCopy(m_pAvatarSlot[dstSlot]);
                m_pAvatarSlot[dstSlot].setCopy(tmp);
                SendAvatarEvent(0x32, slot + 10, dstSlot + 10, srcAddInfo2, dstAddInfo2);
                return 0;
            }
            if ((int)dstType != 0)
            {
                return 0;
            }
            // ---- AVATAR(src) → INVEN(dst) ----
            Inven_Item tmp;
            tmp.setCopy(m_pAvatarSlot[slot]);
            if (!CheckEquipable(m_pAvatarSlot[slot], m_invenItem[dstSlot]))
            {
                return 0x11;
            }
            if (m_invenItem[dstSlot].m_addInfo != 0)
            {
                CItem* pItem = G_CDataManager()->find_item(m_invenItem[dstSlot].m_addInfo);
                if (m_pParent != 0 && pItem != 0)
                {
                    if (!pItem->IsAvatarItem())
                    {
                        return 0x11;
                    }
                    int equipable = IsEquipable(m_pParent, pItem, dstSlot);
                    if (equipable != 0)
                    {
                        return equipable;
                    }
                }
            }
            int srcAddInfo2 = m_pAvatarSlot[slot].m_addInfo2;
            int dstAddInfo2 = m_invenItem[dstSlot].m_addInfo2;
            MoveClearAvatar(slot + 10, srcAddInfo2, m_pAvatarSlot[slot].m_addInfo,
                            dstSlot, dstAddInfo2, m_invenItem[dstSlot].m_addInfo);
            m_pAvatarSlot[slot].setCopy(m_invenItem[dstSlot]);
            m_invenItem[dstSlot].setCopy(tmp);
            SendAvatarEvent(0x32, slot + 10, dstSlot, srcAddInfo2, dstAddInfo2);
            return 0;
        }
        if ((int)invenType != 3)
        {
            return 0;
        }
        if ((int)dstType == 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x131c, 5);
            trace("CInventory::move_item move_item return false 9");
            return 0x11;
        }
        if ((int)dstType == 3)
        {
            // ---- CREATURE ↔ CREATURE（首参为目的侧，与 ORIG 压栈序一致）----
            // ORIG 0x08501752 `mov 0x8(%ebp),%eax; add $0x6e8,%eax` = 直接取
            // &m_creatureMgr 作接收者，不经 GetCreatureMgrW（P167 EXTRA 清零）。
            if (m_creatureMgr.SwapCreatureItem(
                    &m_pCreatureSlot[dstSlot], dstSlot, (int)dstType,
                    &m_pCreatureSlot[slot], slot, (int)invenType))
            {
                return 0;
            }
            return 0x11;
        }
        if ((int)dstType != 0)
        {
            return 0;
        }
        // ---- CREATURE(src) → INVEN(dst) ----
        if (m_creatureMgr.SwapCreatureItem(
                &m_invenItem[dstSlot], dstSlot, (int)dstType,
                &m_pCreatureSlot[slot], slot, (int)invenType))
        {
            return 0;
        }
        return 0x11;
    }
    if ((int)invenType != 0)
    {
        return 0;
    }
    if ((int)dstType == 1)
    {
        // ---- INVEN(src) → EQUIP(dst)：穿戴并与原装备交换 ----
        if (!CheckEquipable(m_invenItem[slot], m_pEquipSlot[dstSlot]))
        {
            return 0x11;
        }
        if (!isEquipmentPlaceableAtInventory(dstSlot))
        {
            return 0x11;
        }
        CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[dstSlot].m_addInfo);
        if (m_pParent != 0 && pItem != 0)
        {
            if (pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, slot);
            if (equipable != 0)
            {
                return equipable;
            }
            if (pItem->IsRandomOption() &&
                m_pEquipSlot[dstSlot].m_random.m_random.empty())
            {
                return 0x11;
            }
        }
        if (GetSlotItemType(dstSlot) == Inven_Item::ITEM_TYPE_INVALID)
        {
            return 0x11;
        }
        if (!m_invenItem[slot].m_amp.isIdentified() ||
            !m_pEquipSlot[dstSlot].m_amp.isIdentified())
        {
            return 0x11;
        }
        if (m_pEquipSlot[dstSlot].m_field0 != 0 &&
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                m_pParent, (RestrictCategory::Enum)1, 0xe))
        {
            return 0xd1;
        }
        Inven_Item tmp;
        tmp.setCopy(m_invenItem[slot]);
        m_invenItem[slot].setCopy(m_pEquipSlot[dstSlot]);
        if (m_pParent != 0 && m_invenItem[slot].m_field0 != 0)
        {
            m_pParent->m_historyLog.seal_Item(
                m_invenItem[slot].m_addInfo,
                (short)m_invenItem[slot].GetReSealCount(),
                eSealItemReason_1);
        }
        m_invenItem[slot].m_field0 = 0;
        if (pItem != 0 && pItem->PeriodAfterUnsealing() != 0)
        {
            int nExpire = GlobalData::s_systemTime_.getCurSec() +
                          pItem->PeriodAfterUnsealing() * 0x15180;
            if (nExpire != 0)
            {
                m_invenItem[slot].m_fieldb =
                    (unsigned short)((nExpire - 0x44a54a80) / 0x15180);
            }
        }
        m_pEquipSlot[dstSlot].setCopy(tmp);
        return 0;
    }
    if ((int)dstType < 2)
    {
        if ((int)dstType != 0)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x122a, 5);
        trace("CInventory::move_item move_item return false 3");
        return 0x11;
    }
    if ((int)dstType != 2)
    {
        if ((int)dstType != 3)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1224, 5);
        trace("CInventory::move_item move_item return false 10");
        return 0x11;
    }
    // ---- INVEN(src) → AVATAR(dst) ----
    if (!CheckEquipable(m_invenItem[slot], m_pAvatarSlot[dstSlot]))
    {
        return 0x11;
    }
    if (m_pAvatarSlot[dstSlot].m_addInfo != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(m_pAvatarSlot[dstSlot].m_addInfo);
        if (m_pParent != 0 && pItem != 0)
        {
            if (!pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, slot);
            if (equipable != 0)
            {
                return equipable;
            }
        }
    }
    int srcAddInfo2 = m_invenItem[slot].m_addInfo2;
    int dstAddInfo2 = m_pAvatarSlot[dstSlot].m_addInfo2;
    MoveClearAvatar(dstSlot + 10, dstAddInfo2, m_pAvatarSlot[dstSlot].m_addInfo,
                    slot, srcAddInfo2, m_invenItem[slot].m_addInfo);
    Inven_Item tmp;
    tmp.setCopy(m_invenItem[slot]);
    m_invenItem[slot].setCopy(m_pAvatarSlot[dstSlot]);
    m_pAvatarSlot[dstSlot].setCopy(tmp);
    SendAvatarEvent(0x32, slot, dstSlot + 10, srcAddInfo2, dstAddInfo2);
    return 0;
}
// [N82 封顶] batch-13 专项逐块：差集 80=80 保持归零（4 处 GetAvatarItemMgrR()->
// 改回成员直访 m_avatarItemMgr. + 输出点二次 GetUpgrade(&0x7f)*2 物化）；
// gap 30→1（658/659 长度近等）。定形项：函数头 listCount/count/countIdx 三初始化
// +声明序（ORIG +0x1e）、flag 区 integrated/非 integrated 两组 dead 槽三步折叠
// （未初始化 ampPad/resByte char + 双 `&1u` 掩码复现 and/mov/and 链，ORIG 语义
// 忠实保留）、非 integrated 二次 GetUpgrade 直调、slotNo-0xc unsigned 界、
// (unsigned)<=0x15。残余 ~10 replace 块（ja/jbe 向、shl↔sar 位测折叠、add/lea、
// push/pop esi+ebx）= 跨版本指令选择+寄存器分配，AE 槽号归一口径形态尾差。

// ============================================================================
// CInventory::MakeEquipList（ORIG 0x084fe2e2，_ZNK10CInventory13MakeEquipListEPvb13ENUM_USERINFO14ENUM_EQUIPSLOT）。
// out 为 InterfacePacketBuf：put_byte 计数占位 → 逐非空槽写（flag=0 精简格式 /
// flag=1 全格式含 amplify、avatar jewel/expansion、creature 剩余天数、
// make3rdChroniclePacket、put_packet）→ 回填计数 → flag=0 时尾部写
// base+0x295（槽 10 m_amp 起始，ORIG 以 UpgradeSeparateInfo::GetUpgradeSeparate
// 单字节读取器跨类型读取）。槽位基址：userInfo==2 或 equipSlot==0 → m_invenItem，
// 否则 CExpandEquipslot::getExpandEquipslotW(equipSlot)（GetCharacExpandData(9)）。
// [§6.2 巨件核销 · b25] 无真修。P167 80/80 保持归零（0 缺 0 多）；新鲜 .o 659 vs
// ORIG 658（gap -1）。残差归因：N6 位测 fold（ORIG `(1<<slot)&0x3ff400` shl 形 vs
// 重建 `(0x3ff400>>slot)&1` sar 形，位测语义等价）、ORIG jl/ja vs 重建 jle/jbe
// 比较方向 fold（slotNo 值域 ≤0x15 无 ≥0x80 歧义，非 P196 真反）、N1 lea/add
// 折叠、xor $1 flag 物化与跨版本块排布，判「语义完成、N 封顶」。
// ============================================================================
int CInventory::MakeEquipList(void* out, bool flag, ENUM_USERINFO userInfo,
                              ENUM_EQUIPSLOT equipSlot) const
{
    // b28d-solo-CInventory：658/659，帧/槽 + libstdc++ 模板内联 ⇒ N82/N89 封顶。
    // b28d-37 [N82/N89 封顶，ORIG 0x84f?]：帧/槽 + libstdc++ 模板内联残差（658/659）。
    // [§6.2 闭合 · b20] P167 80/80 归零：本 const 方法 6 处 put_binary 已按
    // ORIG 走 PKc（const char*）重载（含 pExpansion 常量化，同宽同槽零扰动）。
    // 新鲜 .o 660 vs ORIG 658（gap -2）；帧 124 vs ORIG 112（+12B/3 槽）属
    // N82 槽分配残差，判「语义完成、N 封顶」。
    // [b28d-tu-CInventory] P329：删具名 `InterfacePacketBuf* buf`（原
    //   `= (InterfacePacketBuf*)out`）、60+ 调用点内联 cast ⇒ MakeEquipList
    //   ndiff 989→945（657/660，P167 CLEAR 保持）。ORIG 虽有 buf 槽 -0x34
    //   （store+load 2 行），但内联形的**装载行序**与 ORIG 逐点对齐，
    //   净收益为正；具名形装载序反置 ⇒ 不恢复。
    if (out == 0)
    {
        return 0;
    }
    // ORIG 序言 +0x1e：三变量依声明序初始化写 0（listCount/-0x40、count/-0x38、
    // countIdx/-0x44——countIdx 先 0 后 get_index 覆写，slot 按 AE 口径归一）。
    int listCount = 0;
    int count = 0;
    int countIdx = 0;
    if (!flag)
    {
        listCount = 0xc;
    }
    else if ((int)equipSlot == 0)
    {
        listCount = 0x1a;
    }
    else
    {
        listCount = 0xc;
    }
    countIdx = ((InterfacePacketBuf*)out)->get_index();
    ((InterfacePacketBuf*)out)->put_byte(count);
    int nowTick = OS_API::GetDateTimeTick();
    CExpandEquipslot* expand = 0;
    if (m_pParent != 0)
    {
        expand = m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    }
    const Inven_Item* slots;
    if ((int)userInfo == 2 || (int)equipSlot == 0)
    {
        slots = m_invenItem;
    }
    else
    {
        slots = expand->getExpandEquipslotW(equipSlot);
    }
    for (int i = 0; i < listCount; ++i)
    {
        int slotNo = ((int)equipSlot == 0) ? i : i + 10;
        if (slots[i].m_addInfo == 0)
        {
            continue;
        }
        ++count;
        if (!flag)
        {
            ((InterfacePacketBuf*)out)->put_byte(slotNo);
            if ((int)userInfo == 2 && GetClearAvatar(slotNo) != 0)
            {
                ((InterfacePacketBuf*)out)->put_int(GetClearAvatar(slotNo));
            }
            else
            {
                ((InterfacePacketBuf*)out)->put_int(slots[i].m_addInfo);
            }
            if ((int)userInfo == 2)
            {
                stAvatarExpansionInfo_t expansion;
                expansion.init();
                if (-1 < i && i < 10)
                {
                    // ORIG 读 this+(i+0xd0)*8+8/+10 → m_avatarVisible[i].m_field4/m_field6
                    expansion.m_color[0] = m_avatarVisible[i].m_field4;
                    expansion.m_color[1] = m_avatarVisible[i].m_field6;
                }
                ((InterfacePacketBuf*)out)->put_int(4);
                ((InterfacePacketBuf*)out)->put_binary((const char*)&expansion, 4);
            }
            else if ((int)userInfo == 0)
            {
                stAvatarExpansionInfo_t expansion;
                expansion.init();
                if (-1 < i && i < 10 && slots[i].m_addInfo != 0)
                {
                    void* pExpansion =
                        m_avatarItemMgr.GetExpansionInfo(slots[i].m_addInfo2);
                    if (pExpansion != 0)
                    {
                        memcpy(&expansion, pExpansion, 4);
                    }
                }
                ((InterfacePacketBuf*)out)->put_int(4);
                ((InterfacePacketBuf*)out)->put_binary((const char*)&expansion, 4);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
            {
                // ORIG +0x45d..+0x4fc：三步位折叠 —— upgradeByte(-0x17 物化)/
                // ampFlag(-0x16)/dead 结果槽(-0x4d，ORIG 读未初始化栈字节，忠实保留)。
                char upgradeByte = 0;
                if (slotNo != 10)
                {
                    if (slotNo < 10 || (unsigned int)(slotNo - 0xc) > 9)
                    {
                        upgradeByte = (char)slots[i].GetUpgrade();
                    }
                    else
                    {
                        upgradeByte = 0;
                    }
                }
                char ampFlag = 0;
                if (slots[i].m_amp.getAbilityType() != 0)
                {
                    ampFlag = 1;
                }
                unsigned char ampBit = (unsigned char)(ampFlag & 1u);
                unsigned char resByte;
                resByte = (unsigned char)((resByte & 0xfffffffeu) | (ampBit & 1u));
                resByte = (unsigned char)((resByte & 1u) |
                                          (unsigned char)((upgradeByte & 0x7f) * 2));
                ((InterfacePacketBuf*)out)->put_byte(resByte);
            }
            else
            {
                // ORIG +0x4fe..+0x56f：ampFlag(-0x15) 具名 + dead ampPad(-0x16)/
                // dead 结果槽(-0x4e) 三步折叠，upgrade 位 = **二次 GetUpgrade 调用**
                // （非变量复用）——未初始化读忠实保留。
                char ampFlag = 0;
                if (slots[i].m_amp.getAbilityType() != 0)
                {
                    ampFlag = 1;
                }
                unsigned char ampPad;
                unsigned char ampBit = (unsigned char)(ampPad & 1u);
                (void)ampFlag;
                unsigned char resByte;
                resByte = (unsigned char)((resByte & 0xfffffffeu) | (ampBit & 1u));
                resByte = (unsigned char)((resByte & 1u) |
                                          (unsigned char)((slots[i].GetUpgrade() & 0x7f) * 2));
                ((InterfacePacketBuf*)out)->put_byte(resByte);
            }
        }
        else
        {
            ((InterfacePacketBuf*)out)->put_byte(slotNo);
            ((InterfacePacketBuf*)out)->put_int(slots[i].m_addInfo);
            if (slotNo < 10)
            {
                ((InterfacePacketBuf*)out)->put_int(m_avatarItemMgr.GetRemainDate(slots[i].m_addInfo2, nowTick));
            }
            else
            {
                ((InterfacePacketBuf*)out)->put_int(slots[i].m_addInfo2);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
            {
                unsigned char attrByte = 0;
                if ((unsigned int)slotNo <= 0x15u && (((1 << slotNo) & 0x3ff400) != 0))
                {
                    attrByte = (unsigned char)(slots[i].GetReSealCount() << 5);
                }
                else
                {
                    attrByte = slots[i].GetItemAttr();
                }
                ((InterfacePacketBuf*)out)->put_byte(attrByte);
            }
            else
            {
                ((InterfacePacketBuf*)out)->put_byte((unsigned char)(slots[i].GetItemAttr() & 0xff));
            }
            ((InterfacePacketBuf*)out)->put_short((int)slots[i].m_fieldb);
        }
        ((InterfacePacketBuf*)out)->put_int(GetClearAvatar(slotNo));
        if (!G_GameWorld()->IsEnchantRevisionChannel())
        {
            ((InterfacePacketBuf*)out)->put_int(slots[i].m_fieldd);
        }
        else
        {
            ((InterfacePacketBuf*)out)->put_int(0);
        }
        if (flag)
        {
            ((InterfacePacketBuf*)out)->put_byte((int)slots[i].m_amp.getAbilityType() & 0xff);
            ((InterfacePacketBuf*)out)->put_short((int)slots[i].m_amp.getAbilityValue() & 0xffff);
            if (slots[i].isAvatarItemType())
            {
                ((InterfacePacketBuf*)out)->put_int(0x1e);
                ((InterfacePacketBuf*)out)->put_binary(m_avatarItemMgr.getJewelSocketData(slots[i].m_addInfo2),
                               0x1e);
                ((InterfacePacketBuf*)out)->put_int(4);
                if (m_pParent != 0 &&
                    (const void*)((CUserCharacInfo*)m_pParent)->getTagCharacInvenR() ==
                        (const void*)this)
                {
                    stAvatarExpansionInfo_t expansion;
                    expansion.init();
                    if (-1 < i && i < 10)
                    {
                        expansion.m_color[0] = m_avatarVisible[i].m_field4;
                        expansion.m_color[1] = m_avatarVisible[i].m_field6;
                    }
                    ((InterfacePacketBuf*)out)->put_binary((const char*)&expansion, 4);
                }
                else
                {
                    const char* pExpansion =
                        (const char*)m_avatarItemMgr.GetExpansionInfo(slots[i].m_addInfo2);
                    if (pExpansion == 0)
                    {
                        stAvatarExpansionInfo_t expansion;
                        expansion.init();
                        ((InterfacePacketBuf*)out)->put_binary((const char*)&expansion, 4);
                    }
                    else
                    {
                        ((InterfacePacketBuf*)out)->put_binary(pExpansion, 4);
                    }
                }
            }
            if (slots[i].m_field1 == 5)
            {
                int creatureTick = OS_API::GetDateTimeTick();
                int remainDate =
                    GetCreatureMgrR()->GetRemainDate(slots[i].m_addInfo2, creatureTick);
                ((InterfacePacketBuf*)out)->put_int(remainDate);
            }
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, slots[i], (PacketGuard*)out);
            ((InterfacePacketBuf*)out)->put_packet(slots[i]);
        }
    }
    ((InterfacePacketBuf*)out)->put_byte(countIdx, count);
    if (flag)
    {
        return 1;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        ((InterfacePacketBuf*)out)->put_byte(0);
    }
    else
    {
        // ORIG 0x84febb0（MakeEquipList.md 伪 C `local_2c + 0x295`）：
        // slots + 10*0x3d + 0x33 = slots[10].m_upgradeSep（原注释 m_amp 系笔误，
        // m_amp 在 +0x11）；以单字节读取器 GetUpgradeSeparate 读取（ORIG 原样）。
        ((InterfacePacketBuf*)out)->put_byte(slots[10].m_upgradeSep.GetUpgradeSeparate());
    }
    return 1;
}

// [N82 封顶] batch-10 续派：差集 39=39 归零；8 处 log 块 parent 判向按 ORIG
// 正向 if(m_pParent!=0){log}else{trace} 翻转后 je/jne 计数逼近（22/8 vs 18/12）。
// 残余 gap=8：ORIG cur-count<0 物化用 add/sub+jle 三 insn 形、cmp/cmpl 与 movl 立即数
// 落槽别名，源层不可控（范式回滚实验见 git 历史），判 N82。
// [b28d-Inventory2 本轮推翻 N82] 625/617 DIFF → 623/623 AE（p167 余 cMyTrace
// C1/C2 同址别名伪差，P93）。三处真因：①外层 if/else 链误译 ⇒ ORIG 0x0850405a
// 是 switch(INVEN_TYPE) 二分决策树（探针实测 4.4.7 同形、叶块序=case 书写序 0,1,2,3，
// P324）；②8 处 cMyTrace 具名局部 ⇒ 全表达式临时链（P221）复现 ORIG 槽阶梯
// -0x88..-0x18；③两处 `cur-count<=0` 早返回 ⇒ 正向 if/else（ORIG `sub;test;jle
// <log 块>`）；另 (INVEN_TYPE)1/0/3 硬编码改回 invenType 形参、删 cur 具名局部
// （P183/P329）。
bool CInventory::delete_item(INVEN_TYPE invenType, int slot, int count,
                             eItemDelReason reason, bool bLog)
{
    // b28d-Inventory2：623/623 AE（原 625/617），p167 余 cMyTrace C1/C2 同址伪差。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    if (count < 1)
    {
        return 0;
    }
    // b28d-Inventory2：ORIG 0x0850405a 起为 switch(INVEN_TYPE) 二分决策树
    // （cmp $1/je、cmp $1/jg、test/je、cmp $2、cmp $3），探针实测 4.4.7 同形；
    // 叶块物理序 = case 书写序 0,1,2,3（P324）。历史 if-else 链为误译。
    switch (invenType)
    {
    case INVEN_TYPE_INVEN:
        if (m_invenItem[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (bLog)
        {
            if (m_pParent != 0)
            {
                    m_pParent->m_historyLog.ItemDel(invenType, 0, 1, m_invenItem[slot], reason);
            }
            else
            {
                cMyTrace(
                    __PRETTY_FUNCTION__,
                    0x1811, 5)("History ERROR, m_pParent NULL, ItemDel ");
            }
        }
        m_invenItem[slot].reset();
        if (slot < 10)
        {
            SendAvatarEvent(0x33, slot, 0, 0, 0);
        }
        break;

    case INVEN_TYPE_EQUIP:
        if (slot == 0)
        {
            return use_money(count, (eMoneySubReason)9, bLog);
        }
        if (m_pEquipSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (m_pEquipSlot[slot].m_field1 == 1)
        {
            if (bLog)
            {
                if (m_pParent != 0)
                {
                        m_pParent->m_historyLog.ItemDel(invenType, 0, 1, m_pEquipSlot[slot], reason);
                }
                else
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x1831, 5)("History ERROR, m_pParent NULL, ItemDel ");
                }
            }
            m_pEquipSlot[slot].reset();
        }
        else if (m_pEquipSlot[slot].m_addInfo2 == count)
        {
            if (bLog)
            {
                if (m_pParent != 0)
                {
                        m_pParent->m_historyLog.ItemDel(invenType, 0, m_pEquipSlot[slot].m_addInfo2,
                                                        m_pEquipSlot[slot], reason);
                }
                else
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x184d, 5)("History ERROR, m_pParent NULL, ItemDel ");
                }
            }
            m_pEquipSlot[slot].reset();
        }
        else
        {
            // b28d-Inventory2：ORIG 0x85043b8 `sub; test; jle <log 块>`（log 为跳转
            // 目标、正常流程直落）⇒ 必须写正向 if/else，早返回形会产 `jg <after>`。
            if (m_pEquipSlot[slot].m_addInfo2 - count > 0)
            {
                m_pEquipSlot[slot].m_addInfo2 = m_pEquipSlot[slot].m_addInfo2 - count;
                if (bLog)
                {
                    if (m_pParent != 0)
                    {
                            m_pParent->m_historyLog.ItemDel(invenType, m_pEquipSlot[slot].m_addInfo2, count,
                                                            m_pEquipSlot[slot], reason);
                    }
                    else
                    {
                        cMyTrace(
                            __PRETTY_FUNCTION__,
                            0x1868, 5)("History ERROR, m_pParent NULL, ItemDel ");
                    }
                }
            }
            else
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__,
                    0x186f, "%s - m_InvenSlot[slot_no].add_info(%d) < count(%d)",
                    ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                    m_pEquipSlot[slot].m_addInfo2, count);
                return 0;
            }
        }
        break;

    case INVEN_TYPE_AVATAR:
    {
        if (m_pAvatarSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (bLog)
        {
            if (m_pParent != 0)
            {
                    m_pParent->m_historyLog.AvatarItemDel(m_pAvatarSlot[slot].m_addInfo,
                                                          m_pAvatarSlot[slot].m_addInfo2,
                                                          m_avatarItemMgr.GetIPGAgencyNo(
                                                              m_pAvatarSlot[slot].m_addInfo2),
                                                          (eAvatarItemDelReason)reason);
            }
            else
            {
                cMyTrace(
                    __PRETTY_FUNCTION__,
                    0x187e, 5)("History ERROR, m_pParent NULL, ItemDel ");
            }
        }
        m_pAvatarSlot[slot].reset();
        break;
    }

    case INVEN_TYPE_CREATURE:
    {
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (m_pCreatureSlot[slot].isEquipableItemType())
        {
            if (bLog)
            {
                if (m_pParent != 0)
                {
                        m_pParent->m_historyLog.CreatureItemDel(
                            invenType, m_pCreatureSlot[slot].m_addInfo, m_pCreatureSlot[slot].m_addInfo2, 1,
                            m_pCreatureSlot[slot].m_field1, reason);
                }
                else
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x18a3, 5)("History ERROR, m_pParent NULL, ItemDel ");
                }
            }
            m_pCreatureSlot[slot].reset();
        }
        else
        {
            if (m_pCreatureSlot[slot].m_addInfo2 == count)
            {
                if (bLog)
                {
                    if (m_pParent != 0)
                    {
                            m_pParent->m_historyLog.CreatureItemDel(
                                invenType, m_pCreatureSlot[slot].m_addInfo, 0,
                                m_pCreatureSlot[slot].m_addInfo2, m_pCreatureSlot[slot].m_field1,
                                reason);
                    }
                    else
                    {
                        cMyTrace(
                            __PRETTY_FUNCTION__,
                            0x18bb, 5)("History ERROR, m_pParent NULL, ItemDel ");
                    }
                }
                m_pCreatureSlot[slot].reset();
            }
            else
            {
                // ORIG 0x850484f `sub; test; jle <log 块>`（同上，正向 if/else）。
                if (m_pCreatureSlot[slot].m_addInfo2 - count > 0)
                {
                    m_pCreatureSlot[slot].m_addInfo2 = m_pCreatureSlot[slot].m_addInfo2 - count;
                    if (bLog)
                    {
                        if (m_pParent != 0)
                        {
                                m_pParent->m_historyLog.CreatureItemDel(
                                    invenType, m_pCreatureSlot[slot].m_addInfo, m_pCreatureSlot[slot].m_addInfo2,
                                    count, m_pCreatureSlot[slot].m_field1, reason);
                        }
                        else
                        {
                            cMyTrace(
                                __PRETTY_FUNCTION__,
                                0x18d2, 5)("History ERROR, m_pParent NULL, ItemDel ");
                        }
                    }
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x18da, "%s - m_CreatureSlot[slot_no].add_info(%d) < count(%d)",
                        ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                        m_pCreatureSlot[slot].m_addInfo2, count);
                    return 0;
                }
            }
        }
        break;
    }
    }
    return 1;
}

// ============================================================================
// CInventory::insert_event_items（ORIG 0x08506bb2，_ZN10CInventory18insert_event_itemsERKSt6vectorISt4pairIiiESaIS2_EERS0_IiSaIiEE15eMoneyAddReason14eItemAddReasonbPKcSD_）。
// items 元素 {itemIdx, count}；itemIdx==0 为金钱条目（>499999 走 cMyTrace 审计，
// gain_money 后 outSlots.push(0,0,gained)，差额邮件兜底）。
// 物品条目按 is_stackable/IsAvatarItem/造物蛋(type 0x16) 三路分发：
//   堆叠 → make_item+insertItemIntoInventory(reason,1,1)；
//   头像 → genIPGNo+AddAvatarItem(itemIdx,usablePeriod,...)+GetRemainDate；
//   造物蛋 → create_item+SetEggItem+insertItemIntoInventory(8,1,0)
//            +InsertCreatureItem；其余非堆叠逐份插入，失败邮件兜底。
// reason==8 时累计 cube 统计与 Store 卖价，函数尾 AddValueStatistic(10)。
// ORIG 怪癖（忠实保留）：头像/造物蛋在 itemReason!=8、SetEggItem 失败时
// 不推进迭代器直接回到循环条件；堆叠/头像插入失败且无邮件时跳到函数尾。
// 依赖：docs/class_func_reports/CInventory/insert_event_items.md。
// [N82 封顶] batch-10 续派：调用点差集 95=95 归零（it++ 后置×5/共用尾、cur=pair 临时
// && 赋值、Iterator 默认构造+赋值、push_back(int&&) 物化 5 处、GetRemainDate 返回值
// 直推、money 段 cnt 覆写单槽形）；残余 |gap|=4 与 jmp/nop/xor 布局为 N82 残差。
// [b28d-tu-CInventory] 本轮复核（P329/方法3 均不适用）：本件无具名中间局部可删
//   （buf 族不存在）；残差主体 = 浅槽全体 4-8B 平移 + 帧 0x2fc vs 0x2dc（我方多
//   8 槽 = 4.4 循环体内 pair/push_back 临时不塌槽，P331 探针1）+ 深槽 bLog/
//   临时组 0x20 平移（4.1 lifetime 复用）⇒ N82 封顶维持。struct 573/ndiff 917。
// ============================================================================
int CInventory::insert_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, eMoneyAddReason moneyReason,
                                   eItemAddReason itemReason, bool bLog,
                                   const char* a, const char* b)
{
    // b28d-solo-CInventory：871/874，N113（ppEi）封顶，语义闭合。
    // b28d-37 [N82/N89 封顶，ORIG 0x8506bb2]：P167 调用点差集 CLEAR（95/95）⇒ 语义闭合；871/874 差为帧/槽与模板内联。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    int count = 0;
    if (items.size() == 0)
    {
        return count;
    }
    // ORIG 序言块序：pair 默认构造（+0x038）→ 迭代器默认构造（+0x046）→
    // it = items.begin()（+0x06c 赋值，非拷贝初始化）→ item（+0x078）→ totalValue=0。
    std::pair<int, int> cur;
    Iterator it;
    it = items.begin();
    Inven_Item item;
    int totalValue = 0;
    while (it != items.end())
    {
        item.reset();
        // ORIG +0x0bf：临时 pair 物化 + operator=(pair&&)（范式库:358）。
        cur = std::pair<int, int>(*it);
        int itemIdx = cur.first;
        int cnt = cur.second;
        if (itemIdx == 0)
        {
            // ---- 金钱条目 ----
            if (cnt > 499999 && m_pParent != 0)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x1dc7, 4);
                trace("Quest reward money(%d) mid(%s) characno(%d) reason(%d)", cnt,
                      NumberToString(m_pParent->get_acc_id(), 0),
                      m_pParent->get_charac_no(-1), (int)moneyReason);
            }
            if (m_pParent != 0 && ((CUserCharacInfo*)m_pParent)->getCurCharacR() != 0)
            {
                // ORIG 0x08506dbd：gain 结果直接覆写 cnt（单槽 -0xe0），
                // 邮件差额判据用 cur.second - cnt。
                cnt = ((CUserCharacInfo*)m_pParent)->getCurCharacInvenW()->
                    gain_money(cnt, moneyReason, true, 0);
            }
            outSlots.push_back(0);
            outSlots.push_back(0);
            outSlots.push_back(cnt);
            ++count;
            it++;   // ORIG：迭代器先推进（+0x283），再判定邮件
            if ((int)moneyReason == 3)
            {
                totalValue += cnt;
            }
            if (cur.second > cnt && bLog)
            {
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, cur.second - cnt, m_pParent->getCurCharacNo(), body,
                    strlen(body), 0, m_pParent->GetServerGroup(), false, false);
            }
            continue;
        }

        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x1e14, 5);
            trace("CInventory::insert_event_items, G_CDataManager()->find_item(%d)",
                  itemIdx);
            it++;
            continue;
        }

        item.m_addInfo = itemIdx;
        if (!pItem->is_stackable())
        {
            if (!pItem->IsAvatarItem())
            {
                if (pItem->IsCreatureItem() && pItem->GetItemType() == 0x16)
                {
                    // ---- 造物蛋 ----
                    if ((int)itemReason != 8)
                    {
                        continue;   // ORIG 怪癖：不推进迭代器
                    }
                    G_CDataManager()->m_itemList->create_item(
                        ENUM_ITEM_CREATE_TYPE_0, item, cnt);
                    if (item.m_field1 == 5)
                    {
                        if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
                        {
                            continue;   // ORIG 怪癖：不推进迭代器
                        }
                        int r = insertItemIntoInventory(item, (eItemAddReason)8, true, false);
                        if (r >= 0)
                        {
                            int expirationDate = pItem->getExpirationDate();
                            int usablePeriod = pItem->getUsablePeriod();
                            GetCreatureMgrW()->InsertCreatureItem(&item, r, 8,
                                                                  usablePeriod,
                                                                  expirationDate);
                            ++count;
                            outSlots.push_back(r);
                            // ORIG +0xb68：item.m_addInfo 经临时物化走 push_back(int&&)
                            // （其 m_addInfo 为无符号型，本仓声明 int，以 +x 复现转换形态）。
                            outSlots.push_back(+item.m_addInfo);
                            outSlots.push_back(cnt);
                        }
                    }
                }
                else
                {
                    // ---- 普通非堆叠：逐份插入 ----
                    for (int k = 0; k < cnt; ++k)
                    {
                        pItem->make_item(item);
                        int r = insertItemIntoInventory(item, itemReason, true, true);
                        if (r == -1)
                        {
                            if (!bLog)
                            {
                                break;   // ORIG：跳出循环并走共用尾推进
                            }
                            char title[0x15];
                            strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0),
                                    0x14);
                            char body[0x100];
                            strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0),
                                    0xff);
                            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                title, item, 0, m_pParent->getCurCharacNo(), body,
                                strlen(body), 0, m_pParent->GetServerGroup(), false, false);
                            continue;   // 尝试下一份
                        }
                        ++count;
                        outSlots.push_back(r);
                        outSlots.push_back(+item.m_addInfo);
                        outSlots.push_back(item.m_addInfo2);
                        if ((int)itemReason == 8)
                        {
                            int price = 0;
                            G_Store()->GetSellItemPrice(item, pItem,
                                                        (short)item.m_addInfo2, false,
                                                        price);
                            totalValue += price;
                        }
                    }
                }
            }
            else
            {
                // ---- Avatar 物品 ----
                if ((int)itemReason != 8)
                {
                    continue;   // ORIG 怪癖：不推进迭代器
                }
                char ipg[0x18];
                WongWork::CGenUniqueNo::genIPGNo(
                    (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)2,
                    (unsigned int)((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipg);
                int usablePeriod = pItem->getUsablePeriod();
                int r = AddAvatarItem(itemIdx, usablePeriod, false, 0, (char)-1, ipg,
                                      (eAvatarItemAddReason)8, false, 0);
                if (r == -1)
                {
                    if (!bLog)
                    {
                        goto finish_value_statistic;   // ORIG：跳到函数尾统计
                    }
                    WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                        m_pParent, ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), itemIdx,
                        0, '\0', (char)-1, ipg, (char*)0, 0);
                    it++;
                }
                else
                {
                    ++count;
                    outSlots.push_back(r);
                    outSlots.push_back(+item.m_addInfo);
                    int nowTick = GlobalData::s_systemTime_.getCurSec();
                    // ORIG +0x974：GetRemainDate 返回值直接作实参（右值 → int&&）。
                    outSlots.push_back(GetAvatarItemMgrR()->GetRemainDate(
                        GetInvenSlot(2, r).m_addInfo2, nowTick));
                }
            }
        }
        else
        {
            // ---- 堆叠物品 ----
            item.m_addInfo2 = cnt;
            pItem->make_item(item);
            int r = insertItemIntoInventory(item, itemReason, true, true);
            if (r == -1)
            {
                if (!bLog)
                {
                    goto finish_value_statistic;   // ORIG：跳到函数尾统计
                }
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0, m_pParent->getCurCharacNo(), body, strlen(body), 0,
                    m_pParent->GetServerGroup(), false, false);
                it++;
            }
            else
            {
                ++count;
                outSlots.push_back(r);
                outSlots.push_back(+item.m_addInfo);
                outSlots.push_back(item.m_addInfo2);
                if ((int)itemReason == 8)
                {
                    if (m_pParent != 0)
                    {
                        GetInstanceCubeStatistic()->collectCubeStatistics(
                            pItem, item.m_addInfo2, m_pParent, (CUBE_STATISTIC_FIELD)4);
                    }
                    int price = 0;
                    G_Store()->GetSellItemPrice(item, pItem, (short)item.m_addInfo2, false,
                                                price);
                    totalValue += price;
                }
            }
        }
        // ORIG 共用尾 it++（+0xe7d）：堆叠/头像/造物蛋/非堆叠各成功路径汇流。
        it++;
    }

finish_value_statistic:
    if (((int)itemReason == 8 || (int)moneyReason == 3) && m_pParent != 0)
    {
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xa,
                                                       m_pParent, totalValue);
    }
    return count;
}

void CInventory::SendItemLockList(const Inven_Item* items, int count,
                                  ENUM_ITEMSPACE space) const
{
    // b28d-solo-CInventory：113/115，槽平移 4/8；P212 全头部声明实测无改善已回退 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差（112/114）。
    if (m_pParent == 0)
        return;
    PacketGuard packet;
    // ORIG 不落 buf 局部：每调用点重新 lea packet（P157 逐字段重读）。
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xfb);
    int countIdx = ((InterfacePacketBuf*)&packet)->get_index();
    int cnt = 0;
    ((InterfacePacketBuf*)&packet)->put_short(cnt);   // ORIG 以 cnt 变量传参（此刻为 0）
    for (int i = 0; i < count; ++i) {
        if (items[i].m_amp.GetLock() != 0) {
            ((InterfacePacketBuf*)&packet)->put_byte((int)space);
            ((InterfacePacketBuf*)&packet)->put_short(i);
            CExpandEquipslot* expand = m_pParent->GetCharacExpandDataR(
                (ENUM_CHARAC_EXPAND_TYPE)2);
            reinterpret_cast<const item_lock::CItemLock*>(expand)
                ->MakeItemLockPacket(packet,
                                     (unsigned char)items[i].m_amp.GetLock());
            ++cnt;
        }
    }
    ((InterfacePacketBuf*)&packet)->put_byte(countIdx, cnt);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    m_pParent->Send(packet);
}

void CInventory::SendItemLockListInven() const
{
    SendItemLockList(m_pEquipSlot, 0x138, (ENUM_ITEMSPACE)0);
}

// ORIG 0x084fafbe / 0x084fafee：分别把 m_pAvatarSlot(0x69 槽)/m_pCreatureSlot(0xf2 槽)
// 的锁清单通过 SendItemLockList 发包（ENUM_ITEMSPACE 1=avatar、7=creature）。
void CInventory::SendItemLockListAvatar() const
{
    SendItemLockList(m_pAvatarSlot, 0x69, (ENUM_ITEMSPACE)1);
}

void CInventory::SendItemLockListCreature() const
{
    SendItemLockList(m_pCreatureSlot, 0xf2, (ENUM_ITEMSPACE)7);
}
// [N82 封顶] batch-10 新立项：差集 108=108 归零（删多余 clear()、creature 日期改
// GetCreatureMgrR()、三分支 item 指针缓存全物化为 m_pXxx[slot].field，P183）。
// gap 114→6，残余 6 为槽位 spill 形差。
// [N82 封顶] batch-15 深度块序专项：if 链→switch(invenType)（body 落序
// EQUIP→AVATAR→CREATURE 复现 ORIG cmp $2/$3/$1 决策树 + fall-through 布局，
// difflib 等值率 79.4%）；gap 6→2。定形项：put_header 先行（clear 删除）、
// count=3 槽物化 `put_short(count+3)`、AVATAR/CREATURE `put_short(count)`
// 变量传参、CREATURE tick 先 0 物化。残 2 = push/pop %edi RA 差 + 实参装载
// 次序（4.1 object 先载 vs 4.4 后载，全文件成对 swap 噪声）——版本级行为，勿再迭代。

// ============================================================================
// CInventory::MakeItemList（ORIG 0x084fd7b6 T，docs/class_func_reports/
// CInventory/MakeItemList.md 逐指令转写）。
// m_pParent 为空 → cMyTrace 报错返回 0；否则 interface packet 写头 0xd 后按
// INVEN_TYPE 分发：
//   EQUIP(1)：space 0 + 容量 + 计数槽；slot0/1/2 为固定货币/胜负点虚拟条目
//     （addInfo2 分别为 m_money、三币之和、win point），slot3..0x137 遍历
//     m_pEquipSlot 非空槽（IsEnchantRevisionChannel 时 fieldd 置 0、写
//     amplify type/value、make3rdChroniclePacket）；
//   AVATAR(2)：space 1，遍历 m_pAvatarSlot[0..0x68]，含剩余天数/jewel socket/
//     expansion info；
//   CREATURE(3)：space 7，遍历 m_pCreatureSlot[0..0xf1]，ITEM_TYPE 5 宠补
//     GetRemainDate。
// 各分支回填 short 计数后 finalize 返回 1。
// [§6.2 巨件核销 · b25] 无真修。P167 108/108 保持归零（0 缺 0 多）；新鲜 .o
// 726 vs ORIG 728（gap 2）。残差归因：N18a（ORIG `mov %eax,%edx` regmove 中转
// 出参 4.1 形不可造）、N82（全帧槽号平移 0x10 + push edi +4，帧 0x6c vs 0x60）、
// ORIG `lea 0x3(%eax),%edx` vs 重建 `add $0x3,%eax`（N1 族）与 4.1 尾部
// jmp 中继岛布局，判「语义完成、N 封顶」。
// ============================================================================
bool CInventory::MakeItemList(INVEN_TYPE invenType, void* out) const
{
    // b28d-solo-CInventory：728/726，P167 108/108 归零；N82/N89 封顶。
    // b28d-37 [N82/N89 封顶，ORIG 0x84f?]：同 MakeEquipList（728/726）。
    // [§6.2 闭合 · b20] P167 108/108 归零：3 处 put_binary 已对齐 ORIG 的
    // PKc（const char*）重载（去 (char*) 强转 + expansion 常量化，零扰动）。
    // 新鲜 .o 726 vs ORIG 728（gap 2）；帧 108 vs ORIG 96（+12B/3 槽）属
    // N82 槽分配残差，判「语义完成、N 封顶」。
    // b28d-tu-CInventory] P329：删具名 buf 内联调用点 ⇒ ndiff 1219→906、
    //   指令 723→**728（gap 0）**、struct 202→122，P167 108/108 CLEAR 保持。
    //   [修正版 p167 复判] 108=108，1 对 cMyTrace C2↔C1 镜像别名，豁免后零差。
    //   [§3] CInventory/MakeItemList.md 存在；本轮改动仅删具名 buf（装载序），
    //   取证以 objdump 0x084fd7b6 每调用点装载形直证（§3.2 兜底）。
    //   残 = ORIG buf 槽 store/load 4 行 + push edi/frame 0x6c vs 0x60（N82/
    //   N95）+ `lea 0x3(%eax)` N1 形 + jmp 中继岛 ⇒ 维持封顶。
    if (m_pParent == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xa6b, 5);
        trace("inventory::MakeItemList m_pParent is NULL.");
        return 0;
    }

    // [b28d-tu-CInventory] P329 试验：暂删具名 buf（测 ORIG -0x34 槽形）。
    GameWorld* world;
    ((InterfacePacketBuf*)out)->put_header(0, 0xd);

    // ORIG switch(invenType)：决策树 cmp $2/$3/$1、body 落序 EQUIP→AVATAR→CREATURE
    // （fall-through 布局），default 直达 finalize。switch 形式复现块序。
    switch (invenType)
    {
    case INVEN_TYPE_EQUIP:
    {

            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(get_inventory_capacity());
            int countIdx = ((InterfacePacketBuf*)out)->get_index();
            int count = 3;
            // ORIG +0x00cb..+0x00e2：count=3 槽物化 + `lea 0x3(%eax)` 二次求值后
            // put_short（固定前导 short 0x6，语义未证——物化形按 ORIG 保留）。
            ((InterfacePacketBuf*)out)->put_short(count + 3);

            // slot0：金币条目（addInfo=0，addInfo2=m_money）
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_int(0);
            ((InterfacePacketBuf*)out)->put_int(m_money);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_int(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_packet(g_emptySlot);

            // slot1：代币条目（addInfo=1，addInfo2=coin+payCoin+eventCoin）
            ((InterfacePacketBuf*)out)->put_short(1);
            ((InterfacePacketBuf*)out)->put_int(1);
            ((InterfacePacketBuf*)out)->put_int(m_coin + m_payCoin + m_eventCoin);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_int(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_packet(g_emptySlot);

            // slot2：胜利点条目（addInfo=2，addInfo2=GetWinPoint）
            ((InterfacePacketBuf*)out)->put_short(2);
            ((InterfacePacketBuf*)out)->put_int(2);
            ((InterfacePacketBuf*)out)->put_int(((CUserCharacInfo*)m_pParent)->GetWinPoint());
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_int(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_short(0);
            ((InterfacePacketBuf*)out)->put_byte(0);
            ((InterfacePacketBuf*)out)->put_packet(g_emptySlot);

            for (int slot = 3; slot < 0x138; ++slot)
            {
                if (m_pEquipSlot[slot].m_addInfo == 0)
                    continue;
                ((InterfacePacketBuf*)out)->put_short(slot);
                ((InterfacePacketBuf*)out)->put_int(m_pEquipSlot[slot].m_addInfo);
                ((InterfacePacketBuf*)out)->put_int(m_pEquipSlot[slot].m_addInfo2);
                ((InterfacePacketBuf*)out)->put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], invenType, slot));
                ((InterfacePacketBuf*)out)->put_short(m_pEquipSlot[slot].m_fieldb);
                ((InterfacePacketBuf*)out)->put_byte(m_pEquipSlot[slot].m_field0);
                world = G_GameWorld();
                if (!world->IsEnchantRevisionChannel())
                    ((InterfacePacketBuf*)out)->put_int(m_pEquipSlot[slot].m_fieldd);
                else
                    ((InterfacePacketBuf*)out)->put_int(0);
                ((InterfacePacketBuf*)out)->put_byte((unsigned char)m_pEquipSlot[slot].m_amp.getAbilityType() & 0xff);
                ((InterfacePacketBuf*)out)->put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
                G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                    m_pParent, m_pEquipSlot[slot], (PacketGuard*)out);
                ((InterfacePacketBuf*)out)->put_packet(m_pEquipSlot[slot]);
                ++count;
            }
            ((InterfacePacketBuf*)out)->put_short(countIdx, count);
        }
    case INVEN_TYPE_AVATAR:
    {

            ((InterfacePacketBuf*)out)->put_byte(1);
            int countIdx = ((InterfacePacketBuf*)out)->get_index();
            int count = 0;
            ((InterfacePacketBuf*)out)->put_short(count);   // ORIG +0x05ec/+0x08db：以 count 变量传参（此刻 0）
            int nowTick = OS_API::GetDateTimeTick();
            for (int slot = 0; slot < 0x69; ++slot)
            {
                if (m_pAvatarSlot[slot].m_addInfo == 0)
                    continue;
                ((InterfacePacketBuf*)out)->put_short(slot);
                ((InterfacePacketBuf*)out)->put_int(m_pAvatarSlot[slot].m_addInfo);
                ((InterfacePacketBuf*)out)->put_int(m_avatarItemMgr.GetRemainDate(m_pAvatarSlot[slot].m_addInfo2, nowTick));
                ((InterfacePacketBuf*)out)->put_byte(m_pAvatarSlot[slot].GetItemAttr());
                ((InterfacePacketBuf*)out)->put_short(m_pAvatarSlot[slot].m_fieldb);
                ((InterfacePacketBuf*)out)->put_byte(m_pAvatarSlot[slot].m_field0);
                ((InterfacePacketBuf*)out)->put_int(m_pAvatarSlot[slot].m_fieldd);
                ((InterfacePacketBuf*)out)->put_byte(0);
                ((InterfacePacketBuf*)out)->put_short(0);
                ++count;
                ((InterfacePacketBuf*)out)->put_int(0x1e);
                ((InterfacePacketBuf*)out)->put_binary(m_avatarItemMgr.getJewelSocketData(m_pAvatarSlot[slot].m_addInfo2),
                0x1e);
                const char* expansion =
                    (const char*)m_avatarItemMgr.GetExpansionInfo(m_pAvatarSlot[slot].m_addInfo2);
                ((InterfacePacketBuf*)out)->put_int(4);
                if (expansion == 0)
                {
                    stAvatarExpansionInfo_t expInfo;
                    expInfo.init();
                    ((InterfacePacketBuf*)out)->put_binary((const char*)&expInfo, 4);
                }
                else
                {
                    ((InterfacePacketBuf*)out)->put_binary(expansion, 4);
                }
                ((InterfacePacketBuf*)out)->put_byte(0);
                ((InterfacePacketBuf*)out)->put_packet(m_pAvatarSlot[slot]);
            }
            ((InterfacePacketBuf*)out)->put_short(countIdx, count);
        }
    case INVEN_TYPE_CREATURE:
    {

            ((InterfacePacketBuf*)out)->put_byte(7);
            int countIdx = ((InterfacePacketBuf*)out)->get_index();
            int count = 0;
            ((InterfacePacketBuf*)out)->put_short(count);   // ORIG +0x05ec/+0x08db：以 count 变量传参（此刻 0）
            for (int slot = 0; slot < 0xf2; ++slot)
            {
                if (m_pCreatureSlot[slot].m_addInfo == 0)
                    continue;
                ((InterfacePacketBuf*)out)->put_short(slot);
                ((InterfacePacketBuf*)out)->put_int(m_pCreatureSlot[slot].m_addInfo);
                ((InterfacePacketBuf*)out)->put_int(m_pCreatureSlot[slot].m_addInfo2);
                ((InterfacePacketBuf*)out)->put_byte(m_pCreatureSlot[slot].GetItemAttr());
                ((InterfacePacketBuf*)out)->put_short(m_pCreatureSlot[slot].m_fieldb);
                ((InterfacePacketBuf*)out)->put_byte(m_pCreatureSlot[slot].m_field0);
                ((InterfacePacketBuf*)out)->put_int(m_pCreatureSlot[slot].m_fieldd);
                ((InterfacePacketBuf*)out)->put_byte(0);
                ((InterfacePacketBuf*)out)->put_short(0);
                if (m_pCreatureSlot[slot].m_field1 ==
                    (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                {
                    int nowTick = 0;   // ORIG +0x0a50 先 0 物化再 call 回填
                    nowTick = OS_API::GetDateTimeTick();
                    ((InterfacePacketBuf*)out)->put_int(GetCreatureMgrR()->GetRemainDate(m_pCreatureSlot[slot].m_addInfo2, nowTick));
                }
                ((InterfacePacketBuf*)out)->put_byte(0);
                ((InterfacePacketBuf*)out)->put_packet(m_pCreatureSlot[slot]);
                ++count;
            }
            ((InterfacePacketBuf*)out)->put_short(countIdx, count);
        }
    }
    ((InterfacePacketBuf*)out)->finalize(true);
    return 1;
}

// 链接桩：ORIG ChangeEquip 以 4 参自由函数形态调用成员 GetInvenSlot
// （压栈 out,this,1,slot），转发到成员实现。
// [b28d-Main·检查点18] 原自由函数 shim `int GetInvenSlot(Inven_Item*, CInventory*, int, int)`
// 已删：全仓无调用点（成员调用一律走 `->GetInvenSlot(...)`），ORIG 亦无该符号
// （仅成员 `_ZNK10CInventory12GetInvenSlotEii` 0x084fb918）⇒ 纯 only-NEW 发明符号。
