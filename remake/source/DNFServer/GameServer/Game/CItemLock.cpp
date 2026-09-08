// df_game_r Game/ item_lock::CItemLock + CItemLockTimer 实现（W6_TailA 批）。
// 依据 docs/class_func_reports/item_lock__CItemLock/*.md（汇编为准）与
// docs/class_func_reports/item_lock__CItemLockTimer/*.md；worklist
// /tmp/only_orig/item_lock.txt 全部 33 个符号由本 TU 与共享头产生。
//
// 对象布局（ORIG ctor 0x854156a）：
//   charac_expand::CData 基类（vptr +0x00 / m_bIsSave +0x04）
//   +0x08 map<unsigned char, stItemLockInfo>   当前锁表
//   +0x20 stack<unsigned char, deque<uchar> >  空闲 lock 号池（IsAbleItemLock 判空）
//   +0x48 vector<stItemLockRef>                SendItemLockList 待发列表
//   +0x54 CItemLockTimer                       解锁倒计时计数
// 载体为角色扩展数据区（跨 TU reinterpret_cast，见 CUser/CAccountCargo/CGameManager）。
#include "CTitleBook.h"   // item_lock::stItemLockRef / stItemLockInfo /
                          // CItemLock / CItemLockTimer 权威定义

#include <string.h>
#include <utility>

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "LogManager.h"
#include "SigTypes.h"
#include "TimerQueue.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "PacketGuard.h"
// ---- 最小类型镜像（仓库既有模式，见 CExpandEquipslot.cpp:36）----
// CItemLock.cpp 不 include CUser.h/CInventory.h：二者的联合依赖域含先行批次
// 的 GCC4.4 assert/枚举缺口；此处仅镜像本 TU 调用的方法签名（声明），外部
// 链接定义仍由权威 TU 提供，mangled ABI 完全一致。
// [hc N 记 · b20] CUser 具名字段 m_historyLog（+0x79700）在权威 CUser.h 存在，
// 但本 TU 为镜像隔离 TU（不 include CUser.h，否则与本文件镜像 `class CUser`
// 重复定义 + 先行批次联合依赖缺口），故 cUserHistoryLog* 取址仍按 §4.3「跨 TU
// 隔离」例外保留 `(cUserHistoryLog*)((char*)user + 0x79700)` 裸偏移形态，记 N。
// 证据：CUser.h 内 `cUserHistoryLog m_historyLog;  // +0x79700`；ORIG 各取址点
// lea 0x79700(%reg)。
class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);      // ORIG（槽位访问）
};

class CUserCharacInfo
{
public:
    int getCurCharacNo() const;                            // ORIG 0x080cbc4e
    CInventory* getCurCharacInvenW();                      // ORIG（继承转发）
    unsigned char getCurChannelEquipslotSwitch();          // ORIG 0x086863? 语义一致
    bool FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot,
                      CUser* user) const;                  // ORIG 0x08680e04
};

class cUserHistoryLog
{
public:
    void ItemLockUsingLog(unsigned char lockType, int a, int b, int c,
                          int d);                          // ORIG 0x08689808
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;                           // ORIG 0x080c8c96
    int get_charac_no(int chIdx);                          // ORIG 0x0864dfa0
    Inven_Item* GetCurCharacInventoryRef(unsigned short area,
                                         unsigned short slot); // ORIG 0x08680f2e
    void Send(PacketGuard& packet);                        // ORIG 0x086485ba
};

int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space);   // ORIG W 0x080f7845（switch 映射；权威定义 CTitleBook.cpp）

// [b28d-solo-CItemLock] DB 区块 8 字节条目的 POD 块拷贝视图（布局镜像
// stItemLockInfo）。stItemLockInfo 带用户 ctor（非 POD）⇒ 4.4 对其赋值恒逐字段
// 「读-写」交替、且对 +4 成员先发 `add $4`；**两 int 成员**的 POD 视图令 4.4 发
// ORIG 的 movdi 展开「先读高字、再读低字、后连写两字」（探针
// /tmp/b28d_solo_CItemLock/probe_copy3.cpp v11/v12 逐条同形）。平凡 ctor/op=/dtor
// ⇒ 零符号发射，sizeof/偏移/mangled 不受影响。
struct stItemLockInfoPod
{
    int m_unlockSec;          // +0x00
    int m_stateWord;          // +0x04（m_state + m_pad5[3] 的整字视图）
};
static_assert(sizeof(stItemLockInfoPod) == sizeof(stItemLockInfo),
              "stItemLockInfoPod ABI drift");

// std::pair<const unsigned char, stItemLockInfo> 的 POD 布局镜像：以直接成员访问
// 让 4.4 把 +4/+8 偏移折进 mov 的 disp（v12），避免 `&it->second` 的 `add $4`
// （v11 多一条、v1 退化为逐字段交替）。
struct stItemLockInfoPairPod
{
    unsigned char m_first;        // +0x00
    char m_pad3[3];               // +0x01
    stItemLockInfoPod m_second;   // +0x04
};
static_assert(sizeof(stItemLockInfoPairPod) ==
                  sizeof(std::pair<const unsigned char, stItemLockInfo>),
              "stItemLockInfoPairPod ABI drift");

// SIG_ITEM_LOCK_UPDATE（0x324）载荷视图：+0x00 角色编号、+0x04 起 100×8 锁条目。
// m_characNo 声明 unsigned（与 get_charac_no 的 int 返回值异型，P211 档③）⇒ 复现
// ORIG 0x854182d `call; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,(%eax)`
// （RHS-first+中转）；m_locks 为成员数组 ⇒ 复现 ORIG 0x85418c0 scaled 直存
// `mov %eax,0x4(%ecx,%ebx,8)`（探针 /tmp/b28d_solo_CItemLock/probe_save.cpp g/h2）。
// 仅本 TU 视图；线格式字节与 sizeof 不变。
struct SIG_ITEM_LOCK_UPDATEView
{
    unsigned int m_characNo;          // +0x00
    stItemLockInfoPod m_locks[0x64];  // +0x04（0x64×8 = 0x320）
};
static_assert(sizeof(SIG_ITEM_LOCK_UPDATEView) == sizeof(SIG_ITEM_LOCK_UPDATE),
              "SIG_ITEM_LOCK_UPDATEView ABI drift");

// ORIG 匿名常量（rodata 内联实例 _ZN9item_lockL20CHECK_ITEM_LOCK_TIMEE
namespace item_lock
{

// ORIG C1/C2 0x854156a：CData 基类构造后依次默认构造 map/stack/vector 与
// 计时器成员（全部零态）。
CItemLock::CItemLock()
{
}

// = 0x3c、_ZN9item_lockL24ITEM_UNLOCK_WAITING_TIMEE = 0x3f480，08b25970 取值）。
static const int CHECK_ITEM_LOCK_TIME     = 0x3c;      // 计时器周期 60 秒
static const int ITEM_UNLOCK_WAITING_TIME = 0x3f480;   // 解锁等待期 3 天

// ===========================================================================
// CItemLockTimer（ORIG 方法均 W 内联）
// ===========================================================================

// ORIG 0x8543044：m_count = 0。
CItemLockTimer::CItemLockTimer()
    : m_count(0)
{
}

// ORIG 0x8543052：m_count = 0。
void CItemLockTimer::Reset()
{
    m_count = 0;
}

// ORIG 0x854303a：返回 m_count（非 const）。
int CItemLockTimer::GetTimerCount()
{
    return m_count;
}

// ORIG 0x854147c：计数为 0 时注册 TIMER_MESSAGE 0x88 定时器（周期 60s、
// 参数 uid / characNo）；无论如何计数 +1。
void CItemLockTimer::IncreaseItemLockTimer(CUser* user)
{
    if (m_count == 0)
    {
        // ORIG：getCurCharacNo→%esi、GetUID→%ebx 跨 G_TimerQueue call 缓存，
        // 实参内联复现（P24/P52，禁具名局部槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x88, CHECK_ITEM_LOCK_TIME,
                                    0, (unsigned int)user->getCurCharacNo());
    }
    ++m_count;
}

// ORIG 0x85414f0：计数 -1。
void CItemLockTimer::DecreaseItemLockTimer()
{
    --m_count;
}

// ORIG 0x8541502：计数非 0 时重新注册 TIMER_MESSAGE 0x88 定时器（保证链式触发）。
void CItemLockTimer::NextItemLockTimer(CUser* user)
{
    if (m_count != 0)
    {
        // ORIG：getCurCharacNo→%esi、GetUID→%ebx 跨 G_TimerQueue call 缓存，
        // 实参内联复现（P24/P52，禁具名局部槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x88, CHECK_ITEM_LOCK_TIME,
                                    0, (unsigned int)user->getCurCharacNo());
    }
}

// ===========================================================================
// CItemLock 生命周期
// ===========================================================================

// ORIG D1/D2 0x8541644 / D0 0x85416f6：基类 vptr 回填后容器逆序析构；
// 编译器自动生成等价序列。
CItemLock::~CItemLock()
{
}

std::vector<stItemLockRef>& CItemLock::GetItemLockRefVec()
{
    return m_lockList;
}

// ORIG 0x854261e：map 查找；命中返回 state 并写解锁时刻，未命中返回 0。
// [b28d-41] AE（compare_tu 口径，离开 DIFF）：返回类型 uchar→int 复现 ORIG 尾
// 双段零扩展（详见下行）。
// [b28d-41] 返回类型 uchar→int：ORIG 尾 `movzbl 0x8(%eax),%eax; movzbl %al,%eax`
// 双段零扩展由 int 返回 + uchar 字段表达式复现（探针 /tmp/d41_p4.cpp gB；P248 档①），
// 4.4 对 uchar 返回恒折叠单段。返回类型不入 mangled，调用点隐式截断语义不变。
int CItemLock::GetItemLockInfo(unsigned char lock, int& outSec) const
{
    // [b28c-6] P222 已修（5→1）：残差为返回归一化，已由 int 返回类型消解。
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    // P222 正向包裹：ORIG `test;je 尾部零块; <then>; …; jmp end; 零块:mov $0`
    // ⇒ true 侧包块、false 归共享尾（否定早返 `if(it==end)return 0;` 产反向岛）。
    if (it != m_lockMap.end())
    {
        outSec = it->second.m_unlockSec;
        return it->second.m_state;
    }
    return 0;
}

bool CItemLock::CheckItemLock(unsigned char b) const
{
    // ORIG 0x8541a96：find 结果槽 -0x10 先于 end 临时槽 -0xc（P28 声明序即
    // 栈槽序）⇒ 两条语句（单表达式形把 find 槽排到 -0xc）。
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(b);
    // P56：ORIG `test;je;mov$1;jmp;mov$0` ⇒ 分支返回，勿直 return 比较表达式。
    if (it != m_lockMap.end())
        return true;
    return false;
}

// ORIG 0x85426a6：未锁定 → put_byte(0)；否则 put_byte(state)，state==2 时
// 再 put_int(剩余秒数 = unlockSec + 3天 - now，负值截 0)。
void CItemLock::MakeItemLockPacket(PacketGuard& packet, unsigned char b) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(b);
    // P222 正向包裹：ORIG `test;je <then>; …; jmp end; 尾块 put_byte(0)`
    // ⇒ true 侧包块、失败块归共享尾（早返 `if(it==end){put_byte(0);return;}`
    // 会把失败块前置于 then 之前）。
    if (it != m_lockMap.end())
    {
        packet.put_byte(it->second.m_state);
        if (it->second.m_state == 2)
        {
            int remain = it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME -
                         GlobalData::s_systemTime_.getCurSec();
            if (remain < 0)
                remain = 0;
            packet.put_int(remain);
        }
    }
    else
    {
        packet.put_byte(0);
    }
}

// ===========================================================================
// DB 同步包（StreamPool → MsgQueueMgr DB_Q）
// ===========================================================================

// ORIG 0x8541d42：包 0x1c2 = (0x1c2, uid, item.m_addInfo2(+7), GetLock() & 0xff)。
void CItemLock::UpdateAvatarItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xbf), true);
    **guard << 0x1c2;
    **guard << (int)user->GetUID();
    **guard << item->m_addInfo2;
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x8541c36：包 0x1c1，同构。
void CItemLock::UpdateCreatereItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xb5), true);
    **guard << 0x1c1;
    **guard << (int)user->GetUID();
    **guard << item->m_addInfo2;
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x8541e4e：item 空间字节(+0x01)==8 → UpdateAvatarItem(0x1c2)；
// ==5 → UpdateCreatereItem(0x1c1)。（注意 ORIG 判定次序：==8 先，调用的是 Avatar。）
void CItemLock::UpdateItem(CUser* user, Inven_Item* item)
{
    if (item->m_field1 == 8)
        UpdateAvatarItem(user, item);
    else if (item->m_field1 == 5)
        UpdateCreatereItem(user, item);
}

// ORIG 0x85427a0：包 0xfb（put_header(0,0xfb)），short 计数占位后逐项写；
// 已解锁项清锁并 UpdateItem；倒计时项按频道 equipslot 开关重映射 space
// (3<->0x12)，写 space/slot/state，state==2 再写剩余秒数；末尾回填计数，
// finalize 后发送并清空 m_lockList。
// [b28d-solo-CItemLock] 本轮修法：①`state==0` 用 while+goto 复现 4.1 槽比零
// setcc 物化（P225/P197④）；②`getCurChannelEquipslotSwitch()==1 && …` 拆嵌套 if
// 复现首条件 sete（P225 推广）；③m_slot 比较与 put_short 实参按 unsigned short
// 复现 ORIG `ja`/双段 movzwl；④outSpace/item 提至循环体头（outSpace 先声明）复现
// ORIG 槽序 -0x11/-0x10。put_short(count) 用变量、state/remainSec 声明于循环外且
// state 为 int、space 实参 (unsigned char) 同前。
// [b28d-solo-CItemLock] VERDICT: AE（compare_tu 口径）—— P303/P225 等范式修法见体注。
void CItemLock::SendItemLockList(CUser* user)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfb);
    int countIndex = packet.get_index();
    int count = 0;
    // [b28d-41] ORIG 0x85428a6 `mov -0x20(%ebp),%eax; mov %eax,0x4(%esp)` ⇒
    // 实参来源是具名 count 变量（P97），不是字面量 0。
    packet.put_short(count);
    // [b28d-41] ORIG 在 begin() 之前预清 `movl $0,-0x1c`（state）与
    // `movl $0,-0x38`（remainSec）⇒ 两者声明于循环外；state 为 int
    // （`mov %eax,-0x1c` + `cmpl $0,-0x1c`），非 uchar。
    int state = 0;
    int remainSec = 0;

    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        stItemLockRef& ref = *it;
        // [b28d-solo-CItemLock] ORIG 槽序：ref -0x18、outSpace -0x11、item -0x10
        // （4.1 按声明序，先声明占深槽）⇒ 两局部提至循环体头部、outSpace 先声明。
        unsigned char outSpace;
        Inven_Item* item;
        state = GetItemLockInfo((unsigned char)ref.m_lock, remainSec);
        // [b28d-solo-CItemLock] P225/P197④：ORIG 0x854285b `cmpl $0,-0x1c;
        // sete %al; test %al,%al; je` 的槽比零 setcc 物化用 while+goto 复现
        // （4.4 直写 `if (state==0)` 恒折 `cmpl;jne`；while 条件保留 setcc 段）。
        while (state == 0)
        {
            // 已解锁：清除物品上的锁位并同步（ORIG 经 GetCurCharacInventoryRef
            // 全分派：含 cargo(space=2)/title(space=0x13..0x16)）。
            // [b28d-41] ORIG 0x85428ce `movzbl 0x1(%eax),%eax; movzbl %al,%eax`
            // 双段零扩展 ⇒ 实参写 (unsigned char)（char 直传生 cbtw 符号扩展）。
            item = user->GetCurCharacInventoryRef(
                (unsigned char)ref.m_space, ref.m_slot);
            if (item != 0)
            {
                item->m_amp.SetLock(0);
                UpdateItem(user, item);
            }
            goto send_list_next;   // 跳过 else 路径（ORIG A 路尾 jmp）
        }
        {
            outSpace = (unsigned char)ref.m_space;
            // [b28d-solo-CItemLock] P225 推广：ORIG 0x85428de `cmp $1,%al;
            // sete %al; test %al,%al; je` ⇒ && 必须拆嵌套 if（合并形 4.4 折直跳）。
            // 内层 slot 比较为无符号（ORIG 0x85428f3 `ja`），原码恒真短路照抄。
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                if ((unsigned short)ref.m_slot > 9 ||
                    (unsigned short)ref.m_slot < 0x16)
                {
                    if (ref.m_space == 3)
                        outSpace = 0x12;
                    else if (ref.m_space == 0x12)
                        outSpace = 3;
                }
            }
            packet.put_byte(outSpace);
            // [b28d-solo-CItemLock] ORIG 0x8542936 `movzwl 0x2(%eax),%eax;
            // movzwl %ax,%eax` 双段零扩展 ⇒ 实参 (unsigned short)（signed 生 cwtl）。
            packet.put_short((unsigned short)ref.m_slot);
            packet.put_byte(state);
            if (state == 2)
            {
                int remain = remainSec + ITEM_UNLOCK_WAITING_TIME -
                             GlobalData::s_systemTime_.getCurSec();
                if (remain < 0)
                    remain = 0;
                packet.put_int(remain);
            }
            ++count;
        }
    send_list_next:;
    }
    packet.put_byte(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
    m_lockList.clear();
}

// ===========================================================================
// IsAble* 门禁（返回码即错误表索引；ORIG 精确常量）
// ===========================================================================

// ORIG 0x8541afe：空闲号池空 → 0x16 可锁；非空 → 0（CTitleBook.h:290 注释一致；
// 参数 lock 未使用）。
int CItemLock::IsAbleItemLock(unsigned char /*lock*/) const
{
    return m_lockStack.empty() ? 0x16 : 0;
}

// ORIG 0x8541b2a：map 未命中或 state!=1 → 0x13；state==1 → 0。
int CItemLock::IsAbleItemUnlock(unsigned char lock, int& outSec) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    // P133 独立守卫（同 IsAbleItemUnlockCancel）：ORIG `test;je <then>;
    // mov$0x13;jmp end; then: cmp $1;setne;test;je <0x13>` ⇒ 两条独立 if。
    if (it == m_lockMap.end())
        return 0x13;
    if (it->second.m_state != 1)
        return 0x13;
    // P171 照抄原码：ORIG 0x8541b2a 全 43 条内**没有** outSec 写入（唯一 ptEv
    // 只读 +0x8 m_state，无 +0x4 装载）⇒ 原码不写 outSec，保留形参未用。
    (void)outSec;
    return 0;
}

// ORIG 0x8541bb0：map 未命中或 state!=2 → 0x13；state==2 → 0。
int CItemLock::IsAbleItemUnlockCancel(unsigned char lock) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    // P133 独立守卫：ORIG `test;je <then>; mov$0x13;jmp end; then: cmp $2;setne;
    // test;je <0x13 尾>` ⇒ 两条件各自独立 if（合并短路会产 bool 汇聚异形）。
    if (it == m_lockMap.end())
        return 0x13;
    if (it->second.m_state != 2)
        return 0x13;
    return 0;
}

// ===========================================================================
// 锁 / 解锁业务
// ===========================================================================

// ORIG 0x8542192：物品未带锁号时从空闲池取出一个号，写 {unlockSec=now,
// state=1} 入 map；alter 后把新号写回物品并发请求包；日志 reason=0。
// [b28d-solo-CItemLock] 本轮修法：insert 实参改内联直构（无具名 value；槽序
// make_pair/-0x18 → pair/-0x24 → insert sret/-0x2c 与 ORIG 一致）。既有权宜：
// 删多余 memset、具名 uchar lock、m_state 先于 m_unlockSec 赋值、删 empty()。
// [b28d-solo-CItemLock] VERDICT: AE —— 内联 insert 直构实参 + 内联 history receiver。
int CItemLock::DoItemLock(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    // [b28d-41] ORIG 0x85421d3：`call GetLock; mov %al,-0x2d; movzbl -0x2d,%eax;
    // test %al,%al; je` ⇒ 具名 uchar 局部（P0 槽往返）；内联两次调用形产 setne
    // 归一 + 帧 +0x10。
    unsigned char lock = item->m_amp.GetLock();
    if (lock != 0)
        return 0;
    // ORIG 0x85421f6 无 empty() 调用，直接 top()→pop()（P171：ORIG 不写即删）。
    lock = m_lockStack.top();
    m_lockStack.pop();

    stItemLockInfo info;
    // ORIG 0x854221a 顺序：C1Ev 清零后先 `movb $0x1,-0x34`（m_state）再 getCurSec
    // → `mov %eax,-0x38`（m_unlockSec）；无显式 memset（ctor 自身逐字节清零）。
    info.m_state = 1;
    info.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    // [b28d-solo-CItemLock] 内联直构实参（同 PushItemLock 判据）：ORIG
    // 0x854222d make_pair@-0x18 → converting ctor@-0x24 → insert(const&) sret@-0x2c。
    m_lockMap.insert(
        std::pair<std::map<unsigned char, stItemLockInfo>::key_type,
                  std::map<unsigned char, stItemLockInfo>::mapped_type>(
            std::make_pair(lock, info)));

    alter();
    item->m_amp.SetLock(lock);
    UpdateItem(user, item);
    SendRequestItemLock(user, space, slot);

    // [b28d-solo-CItemLock] 内联 receiver（§4.3 跨 TU 隔离例外见顶注）：ORIG
    // 0x85422e3 无 history 具名槽，`mov 0xc(%ebp),%ecx; add $0x79700,%ecx` 在
    // item 字段实参之后求值（P0/P52 内联形）。
    ((cUserHistoryLog*)((char*)user + 0x79700))
        ->ItemLockUsingLog(0, space, slot, item->m_addInfo,
                           item->m_addInfo2);
    return 1;
}

// ORIG 0x854231a：物品已上锁且 map 命中时置 state=2 / unlockSec=now，
// 计时器 +1 并回发剩余秒数；日志 reason=1。
// [b28d-solo-CItemLock] 本轮修法：remain 改 P198① 预清两句式（ORIG 0x8542475
// `movl $0x0,-0xc(%ebp)`）+ history receiver 内联。既有权宜：具名 uchar lock
// 局部（find 复用同槽，消除二次 GetLock 与 sete 归一）。
// [b28d-solo-CItemLock] VERDICT: AE —— P198① remain 预清两句式 + 内联 history receiver。
int CItemLock::DoItemUnlock(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    // [b28d-41] ORIG 0x854235a：具名 uchar 局部 -0x15（`mov %al,-0x15;
    // movzbl -0x15,%eax; test; jne`），find 复用同槽；内联两次调用形产 sete
    // 归一 + 帧 +0x10。
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0;

    std::map<unsigned char, stItemLockInfo>::iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end())
        return 0;

    it->second.m_state = 2;
    it->second.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    alter();
    m_lockTimer.IncreaseItemLockTimer(user);

    // [b28d-solo-CItemLock] P198① 预清两句式：ORIG 0x8542475 在 call 前有
    // `movl $0x0,-0xc(%ebp)`（remain 预清），合写初始化式会丢该行。
    int remain = 0;
    remain = it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME -
             GlobalData::s_systemTime_.getCurSec();
    if (remain < 0)
        remain = 0;

    SendRequestItemUnlock(user, space, slot, remain);

    // [b28d-solo-CItemLock] 内联 receiver（ORIG 0x8542488 无 history 槽）。
    ((cUserHistoryLog*)((char*)user + 0x79700))
        ->ItemLockUsingLog(1, space, slot, item->m_addInfo,
                           item->m_addInfo2);
    return 1;
}

// ORIG 0x85424c2：物品已上锁且 map 命中时置 state=1 / unlockSec=now，
// 计时器 -1；日志 reason=2。
// [b28d-solo-CItemLock] 本轮修法：history receiver 内联（去多余具名槽，帧
// 0x44→0x34 与 ORIG 一致）。既有权宜：具名 uchar lock 局部 + find 复用同槽。
// [b28d-solo-CItemLock] VERDICT: AE —— 内联 history receiver（去多余具名槽）。
int CItemLock::DoItemUnlockCancel(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    // [b28d-41] ORIG 0x85424f0：`call GetLock; mov %al,-0x11; movzbl -0x11,%eax;
    // test %al,%al; jne` ⇒ 具名 uchar 局部（P0 槽往返），且 find 复用同一槽
    // （`lea -0x11,%edx`）；内联两次调用形多一次 GetLock 调用 + 帧 +0x10。
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0;

    std::map<unsigned char, stItemLockInfo>::iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end())
        return 0;

    it->second.m_state = 1;
    it->second.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    alter();
    m_lockTimer.DecreaseItemLockTimer();

    SendRequestItemUnlockCancel(user, space, slot);

    // [b28d-solo-CItemLock] 内联 receiver（ORIG 0x85425e5 无 history 槽）。
    ((cUserHistoryLog*)((char*)user + 0x79700))
        ->ItemLockUsingLog(2, space, slot, item->m_addInfo,
                           item->m_addInfo2);
    return 1;
}

// ORIG 0x8542a6a：定时器到期入口 → PopItemLock，恒返 1。
int CItemLock::OnItemUnlockWaitTimeout(CUser* user)
{
    PopItemLock(user);
    return 1;
}

// ORIG 0x8542a8a：
//   新条目 state==0                → 号回收进空闲池；
//   state==2 且已过等待期          → 清 info 后回收 + alter；
//   其余                           → 继续计时再入 map（state==2 时续上计时器）。
// [b28d-solo-CItemLock] VERDICT: AE —— insert 实参内联直构（无具名 value 临时）。
void CItemLock::PushItemLock(CUser* user, unsigned char lock,
                             stItemLockInfo* info)
{
    if (info->m_state == 0)
    {
        m_lockStack.push(lock);
        return;
    }

    if (info->m_state == 2)
    {
        if (info->m_unlockSec <=
            GlobalData::s_systemTime_.getCurSec() - ITEM_UNLOCK_WAITING_TIME)
        {
            info->m_state = 0;
            info->m_unlockSec = 0;
            m_lockStack.push(lock);
            alter();
            return;
        }
        m_lockTimer.IncreaseItemLockTimer(user);
    }

    // [b28d-solo-CItemLock] AE：旧 NEAR 残差（tmp/value/ret 三槽置换，判 N82）
    // 作废 —— 真因是原码**无具名 value**、insert 实参为内联直构临时：
    // ORIG 0x8542b2d make_pair@-0x14 → converting ctor(pair&&)@-0x20 →
    // insert(const&) sret@-0x28（探针 /tmp/b28d_solo_CItemLock/probe_pair.cpp
    // fB 逐条同形；具名 value 形 fA 恰产我方旧槽序）。语义等价（返回值本即丢弃）。
    m_lockMap.insert(
        std::pair<std::map<unsigned char, stItemLockInfo>::key_type,
                  std::map<unsigned char, stItemLockInfo>::mapped_type>(
            std::make_pair(lock, *info)));
}

// ORIG 0x8542b80：遍历 map，将已过 3 天等待期且 state==2 的条目逐个收尾：
// FindItemLock 定位物品槽 → 清物品锁号 + DB 包；equipslot 开关频道空间重映射
// (3<->0x12)；SendItemUnlock 后号回收进空闲池、erase 条目、计时器 -1、alter、
// 日志 reason=3；FindItemLock 失败直接返回。结束后 NextItemLockTimer 链式续期。
// [b28d-solo-CItemLock] 本轮修法：①`if (A && B)` 正极性 + 体尾 continue 复现
// ORIG 0x8542bfa 的 mov$1/mov$0 物化与「体尾 jmp 条件、++it 置体后」布局；
// ②`getCurChannelEquipslotSwitch()==1 && …` 拆嵌套 if 复现首条件 sete；③history
// receiver 内联。既有权宜：删 space/slotVal 预清（直接传址）、erase(it++)、
// 直接改 space 出参、日志前不设 item 守卫（P171）。
// [b28d-solo-CItemLock] VERDICT: AE —— 正极性 if(A&&B)+体尾 continue + && 拆嵌套 if。
void CItemLock::PopItemLock(CUser* user)
{
    if (m_lockTimer.GetTimerCount() == 0)
        return;

    int now = GlobalData::s_systemTime_.getCurSec();

    std::map<unsigned char, stItemLockInfo>::iterator it = m_lockMap.begin();
    while (it != m_lockMap.end())
    {
        // [b28d-solo-CItemLock] ORIG 0x8542bfa `mov $1,%eax; jmp; mov $0,%eax;
        // test %al,%al; je <body>` —— 正极性 `if (A && B)` + 体尾 continue
        // 复现该物化与 ORIG 的「体尾 jmp 条件、++it 块置体后」布局（旧 `if
        // (!(A&&B)) { ++it; continue; }` 形 4.4 产反向极性且 ++it 块前置）。
        if (now >= it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME &&
            it->second.m_state == 2)
        {
            // [b28d-41] ORIG 0x8542c29 直接把 &space/-&slotVal 入参（lea
            // -0x20/-0x24 后即 call），无 `movl $0,-N` 预清 ⇒ 原码不初始化这两个
            // 出参（P131/P171）。
            ENUM_ITEMSPACE space;
            int slotVal;
            if (!((const CUserCharacInfo*)user)->FindItemLock(
                    it->first, space, slotVal, (CUser*)user))
                return;  // ORIG：定位失败立即退出（保留遍历位置）

            Inven_Item* item =
                user->GetCurCharacInventoryRef((unsigned short)space,
                                               (unsigned short)slotVal);
            if (item != 0)
            {
                item->m_amp.SetLock(0);
                UpdateItem(user, item);
            }

            // [b28d-41] ORIG 0x8542cc9 `mov -0x20(%ebp),%eax; cmp $3; jne;
            // movl $0x12,-0x20(%ebp)` —— 直接改 space 出参本身，无 sendSpace 局部
            // （具名副本多 2 条 + 一槽，P0/P171）。
            // [b28d-solo-CItemLock] P225 推广：ORIG 0x8542c55 `cmp $1,%al;
            // sete %al; test %al,%al; je` ⇒ && 拆嵌套 if（合并形 4.4 折直跳 +
            // 多 bool 物化块）。
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                if (slotVal > 9 && slotVal < 0x16)
                {
                    if (space == (ENUM_ITEMSPACE)3)
                        space = (ENUM_ITEMSPACE)0x12;
                    else if (space == (ENUM_ITEMSPACE)0x12)
                        space = (ENUM_ITEMSPACE)3;
                }
            }

            SendItemUnlock(user, (int)space, slotVal);
            m_lockStack.push(it->first);
            // [b28d-41] ORIG 0x8542d?? 用 ppEi（后置 it++ 带 sret 临时）⇒ 原码
            // erase(it++)（P28 后置增量），具名 next + 前置 ++next 形多一槽+一条调用。
            m_lockMap.erase(it++);

            m_lockTimer.DecreaseItemLockTimer();
            alter();

            // [b28d-41] ORIG 0x8542d71 直接读 item->m_addInfo2（无
            // `if (item != 0)` 守卫）⇒ 删除多余条件（P171）。
            // [b28d-solo-CItemLock] 内联 receiver（ORIG 0x8542d88 无 history 槽）。
            ((cUserHistoryLog*)((char*)user + 0x79700))
                ->ItemLockUsingLog(3, (int)space, slotVal,
                                   item->m_addInfo, item->m_addInfo2);
            // 体尾 continue：erase(it++) 已推进迭代器，跳过下方 ++it（ORIG 体尾
            // jmp 至条件块）。
            continue;
        }
        ++it;
    }

    m_lockTimer.NextItemLockTimer(user);
}

// ORIG 0x8542e12：m_lockList 中存在指定 lock 号的引用 → 1，否则 0。
// [b28d-41] 返回类型 int→bool：ORIG VerifyItemLock 调用点 `call; xor $0x1,%eax;
// test %al,%al; je` 是 bool callee 裸形（P248 档①），int 返回恒产 sete 归一。
bool CItemLock::_findItem(unsigned char lock)
{
    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        // ORIG 0x8542e12 段 `mov %eax,-0xc; mov -0xc,%eax; movzbl (%eax)`：
        // operator* 结果指针落槽回读（P0 有槽往返 ⇒ 原码具名引用）。
        stItemLockRef& ref = *it;
        if (ref.m_lock == lock)
            return true;
    }
    return false;
}

// ORIG 0x8542e98：核对 map 中所有锁定/倒计时条目是否仍被 m_lockList 引用；
// 未被引用的（角色扩展数据加载不全时）退回空闲池并从 map 删除（state==2 时
// 同时 -1 计时器）；发生过修正则 alter。
// [b28d-41] AE（compare_tu 口径，离开 DIFF）：三处修复——①`state==1||state==2`
// 正向包裹复现 ORIG 0x8542ede `cmp $1; je; cmp $2; jne`（4.4 对 `!=1 && !=2` 恒
// 合并 je/je 同落点）；②`erase(it++)` 复现 0x8542f47 ppEi（P28）；③_findItem
// 返回类型 bool 复现 0x8542f0e `xor $1,%eax; test %al,%al; je`（P248 档①）。
void CItemLock::VerifyItemLock(CUser* user)
{
    if (user == 0)
        return;

    bool changed = false;
    std::map<unsigned char, stItemLockInfo>::iterator it = m_lockMap.begin();
    while (it != m_lockMap.end())
    {
        unsigned int state = (unsigned int)it->second.m_state;
        // [b28d-41] ORIG 0x8542ede：`cmp $1; je <继续块>; cmp $2; jne <++it 块>`
        // = 正极性 `state==1 || state==2` 短路；4.4 对 `!=1 && !=2` 恒合并
        // `je/je` 同落点（少一条 jne 分支），故改正向包裹。
        if (state == 1 || state == 2)
        {
            if (!_findItem(it->first))
            {
                m_lockStack.push(it->first);
                // [b28d-41] ORIG 0x8542f47 `call ppEi`（sret 临时 -0x14）⇒
                // 原码 erase(it++)（P28 后置增量）；`next=it; ++next; erase(it)`
                // 形产 ppEv 且多拷贝槽。
                m_lockMap.erase(it++);
                if (state == 2)
                    m_lockTimer.DecreaseItemLockTimer();
                changed = true;
            }
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }

    if (changed)
        alter();
}

// ===========================================================================
// 客户端应答包
// ===========================================================================

// ORIG 0x85420e0：包 0xfc = put_header(0,0xfc)、byte(space)、short(slot)。
void CItemLock::SendItemUnlock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfc);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x8541ea0：包 0x10f = put_header(1,0x110 族第 1 条)、byte(1)、
// byte(space)、short(slot)。注：ORIG put_header 第二参 0x10f。
void CItemLock::SendRequestItemLock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x10f);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x8541f5a：包 0x110 = put_header(1,0x110)、byte(1)、byte(space)、
// short(slot)、int(remainSec)。
void CItemLock::SendRequestItemUnlock(CUser* user, int space, int slot,
                                      int remainSec)
{
    PacketGuard packet;
    packet.put_header(1, 0x110);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x8542026：包 0x111 = put_header(1,0x111)、byte(1)、byte(space)、
// short(slot)。
void CItemLock::SendRequestItemUnlockCancel(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x111);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// 角色扩展数据生命周期（charac_expand::CData 虚接口）
// ===========================================================================

// ORIG 0x8541714：pData 为 100 × 8 字节 DB 区块；自 i=99 向下 PushItemLock
// （lock 号 = i+1），使低号位最终落在栈顶（与空闲号池 LIFO 序一致）。恒真。
// [b28d-solo-CItemLock] VERDICT: AE —— 具型指针 data+i 复现 ORIG 偏移 add 序。
bool CItemLock::loadData(CUser* user, char* pData)
{
    // [b28d-solo-CItemLock] AE：ORIG 0x08541714 `mov i,%eax; shl $3,%eax;
    // mov %eax,%edx; add -0x10(%ebp),%edx` —— 索引先物化入 %edx、data 作 add
    // 内存源。旧判 N7/N18a（13 形 char* 算术全产 LHS-first）作废：真因是原码
    // **具型指针局部** `stItemLockInfo* data`，`data + i` 由 sizeof=8 自然缩放出
    // 同一条 shl $3 链（探针 /tmp/b28d_solo_CItemLock/probe_add.cpp fF 逐条全等，
    // 同族 P255 数组名退化直传）。偏移/语义不变（stItemLockInfo 恰 8 字节）。
    stItemLockInfo* data = (stItemLockInfo*)pData;   // ORIG 0x854171d 存槽 -0x10
    for (int i = 99; i >= 0; --i)
    {
        PushItemLock(user, (unsigned char)(i + 1), data + i);
    }
    return true;
}

// ORIG 0x8541958：map 逆编号回写 DB 区块（idx = key-1 ∈ [0,99]；每项 8 字节
// = unlockSec + {state,pad} 4 字节整体）；越界记 LogManager 错误。
// [b28d-solo-CItemLock] 本轮修法：具型 POD 指针 `data + idx` 复现 ORIG 0x85419da
// 偏移 add 序；POD 视图赋值复现 8 字节「先读全再写全」块拷贝（旧 N7/N84/N90
// 判词作废）。
// [b28d-solo-CItemLock] VERDICT: AE —— 具型 POD 指针 + POD 视图块拷贝（先读全再写全）。
void CItemLock::getData(char* pData) const
{
    // [b28d-solo-CItemLock] AE：旧 DIFF（N7 偏移 add 序 + N84/N90 拷贝读序）
    // 作废 —— ①具型 POD 指针 `data + idx` 复现 ORIG 0x85419da `mov i,%eax;
    // shl $3,%eax; mov %eax,%ebx; add -0x10(%ebp),%ebx`（同 loadData）；②POD 视图
    // 赋值复现 `mov 0x8(%eax),%edx; mov 0x4(%eax),%eax; mov %eax,(%ebx);
    // mov %edx,0x4(%ebx)` 先读全再写全（探针 fD）。语义为同一 8 字节块拷贝。
    stItemLockInfoPod* data = (stItemLockInfoPod*)pData;   // ORIG 存槽 -0x10
    for (std::map<unsigned char, stItemLockInfo>::const_iterator it =
             m_lockMap.begin();
         it != m_lockMap.end(); ++it)
    {
        int idx = (int)it->first - 1;
        if (idx > 99 || idx < 0)
        {
            LogManager::logFormat(
                1, "ItemLock.cpp",
                __PRETTY_FUNCTION__,
                0x6c, "itemlock error index %d", idx);
            continue;
        }
        // 直接成员访问（保留 const_iterator::operator-> 调用链，P103）⇒ 4.4 把
        // +4/+8 折进 disp 并发 movdi 展开（ORIG 0x85419da 段逐条同形）。
        *(data + idx) =
            ((const stItemLockInfoPairPod*)it.operator->())->m_second;
    }
}

// ORIG 0x854176e：SIG_ITEM_LOCK_UPDATE（0x324 字节）出队至 LOGDB→DB_Q(2)：
// << 0x1b8、<< uid；GetInBuffer 清零后 +0 = get_charac_no(-1)，随后与 getData
// 相同的 idx/key-1 校验与写入；越界 LogManager 错误；put(DB_Q) 后析构。恒真。
// [b28d-solo-CItemLock] 本轮修法：SIG 载荷视图 SIG_ITEM_LOCK_UPDATEView
// （unsigned m_characNo + m_locks[0x64]）⇒ characNo store 复现 ORIG 0x854182d
// RHS-first+%edx 中转（unsigned 字段与 int 返回值异型，P211 档③），锁条目复现
// ORIG 0x85418c0 scaled 直存 + POD 块拷贝（旧 N76/N84/N90 判词作废）。
// [b28d-solo-CItemLock] VERDICT: AE —— SIG 载荷视图（unsigned characNo + m_locks[0x64]）+ POD 块拷贝。
bool CItemLock::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0x4b), true);
    **guard << 0x1b8;
    **guard << (int)user->GetUID();

    // [b28d-solo-CItemLock] AE：旧 DIFF（N76 store 序 + N84/N90 拷贝序 + 索引
    // 复合乘式）作废 —— ①载荷视图 SIG_ITEM_LOCK_UPDATEView（+0 characNo、
    // +4 起 100×8）复现 ORIG scaled 直存 0x4(%ecx,%ebx,8)；②m_characNo unsigned
    // 化（与 int 返回值异型）复现 ORIG RHS-first + %edx 中转；③POD 视图赋值复现
    // ORIG 先读全再写全块拷贝（同 getData）。
    SIG_ITEM_LOCK_UPDATEView* sig =
        (SIG_ITEM_LOCK_UPDATEView*)guard->GetInBuffer<SIG_ITEM_LOCK_UPDATE>();
    memset(sig, 0, 0x324);
    sig->m_characNo = user->get_charac_no(-1);

    for (std::map<unsigned char, stItemLockInfo>::iterator it =
             m_lockMap.begin();
         it != m_lockMap.end(); ++it)
    {
        int idx = (int)it->first - 1;
        if (idx > 99 || idx < 0)
        {
            LogManager::logFormat(
                1, "ItemLock.cpp",
                __PRETTY_FUNCTION__,
                0x59, "itemlock error index %d", idx);
            continue;
        }
        sig->m_locks[idx] =
            ((const stItemLockInfoPairPod*)it.operator->())->m_second;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ORIG 0x8541a3e：清空锁表 / 待发列表，抽干空闲号池，复位计时器。
void CItemLock::_reset()
{
    m_lockMap.clear();
    m_lockList.clear();
    while (!m_lockStack.empty())
        m_lockStack.pop();
    m_lockTimer.Reset();
}

}  // namespace item_lock
