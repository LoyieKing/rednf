// ============================================================================
// CreatureOnlySimple.cpp —— OnlyOrig 清零批 UCSimple（user_creature 辅助类与池）
//
// 独占 TU：每函数定义前注释 ORIG mangled + 地址；逐指令依据报告
// docs/class_func_reports/user_creature__{COverSkillTime,CStomach,CCreatureItem,
// CEgg}/ 或 objdump(-d df_game_r) 转写（缺失报告的函数标【objdump】）。
// 声明追加需求见 /tmp/df_batches/CreatureJob_decl_UCSimple.txt（父代理落头）：
//   - CREATURE_ITME_TYPE 枚举 / CCreatureItem 外联 ctor / SetCreatureItemType(enum)
//   - CEgg 继承式布局 / CDBMsgSender::Cancel+ChangeOwner+ChangeSlotNo+AuctionRegist
//   - COverSkillTime::{ctor,SetTime,GetTime} / CStomach ctor
//     / CCreatureItemFactory::{GetSize,GetRemainSize} 声明
// ============================================================================
#include <string.h>

#include "GlobalData.h"    // s_systemTime_ / s_*_stream_pool
#include "Inven_Item.h"    // Inven_Item / Inven_Item_Compatible
#include "SigTypes.h"      // SIG_CREATURE_ITEM
#include "StreamPool.h"    // Stream(m_poolType) / StreamPool/BigStreamPool/SmallStreamPool
#include "CInventory.h"    // user_creature::CCreatureItem / CCreatureMgr 完整布局
#include "CGameManager.h"  // user_creature::COverSkillTime/CStomach/CEgg/CDBMsgSender/
                           // CCreatureItemFactory/GetInstanceCreatureItemFactory
#include "CUser.h"

namespace user_creature
{

// ============================================================================
// COverSkillTime（4B，内嵌于 CCreature +0x50）
// ============================================================================

// ORIG _ZN13user_creature14COverSkillTimeC1Ev T 0x0833d636（C2 同址）：
// *(int*)this = 0。
COverSkillTime::COverSkillTime()
{
    m_time = 0;
}

// ORIG _ZN13user_creature14COverSkillTime7SetTimeEv T 0x0833d644：
// call CSystemTime::getCurSec(GlobalData::s_systemTime_) 后 *(int*)this = eax。
void COverSkillTime::SetTime()
{
    m_time = GlobalData::s_systemTime_.getCurSec();
}

// 【objdump】ORIG _ZN13user_creature14COverSkillTime7GetTimeEv W 0x0833ef04
// （size 0x08）：push %ebp; mov %esp,%ebp; mov 0x8(%ebp),%eax; pop %ebp; ret
//   —— 字面返回 this 本身、不解引用。调用方（CCreatureMgr::SetOverSkillTime
//   0x0833c523/0x0833c637/0x0833c656）在 call 之后立即 `mov (%eax)` 取值，
//   即返回 &m_time。返回类型不参与 mangled；按 ORIG 指令忠实转写为 int*。
int* COverSkillTime::GetTime()
{
    return &m_time;
}

// ============================================================================
// CStomach（8B，内嵌于 CCreature +0x34：m_lastTime@+0 / m_value@+4）
// ============================================================================

// ORIG _ZN13user_creature8CStomachC1Ev T 0x08336cd8（C2 同址）：
// this+0 = getCurSec()；this+4 = 0x42c80000（float 100.0f）。
CStomach::CStomach()
{
    m_lastTime = GlobalData::s_systemTime_.getCurSec();
    m_value = 100.0f;   // ORIG mov $0x42c80000
}

// ============================================================================
// CCreatureItem（权威布局见 CInventory.h；本批提供外联 ctor / 类型写入器）
// ============================================================================

// 【objdump】ORIG _ZN13user_creature13CCreatureItemC1Ev W 0x08340c82（C2 同址，
// size 0x0e）：movl $0x8c2f4d8,(%eax) —— 写 CCreatureItem vtable 指针槽。
// 本镜像以 int m_vptr 字段保持偏移；常量为 ORIG &_ZTVN13user_creature13CCreatureItemE+0x8
// （nm V 0x08c2f4d0 → +8 = 0x08c2f4d8）。
CCreatureItem::CCreatureItem()
{
    m_vptr = (int)0x08c2f4d8;
}

// 【objdump】ORIG _ZN13user_creature13CCreatureItem19SetCreatureItemTypeENS_18CREATURE_ITME_TYPEE
// W 0x0833efe2（size 0x0e）：this+0x08 = type。
// 调用点 CCreatureMgr::Hatch @0x0833a11d 传 1（孵化成宠物）。枚举参数版本为
// worklist 权威符号（现头 inline int 版产生的 Ei 符号 ORIG 不存在，由 decl
// 条目替换掉）。
void CCreatureItem::SetCreatureItemType(user_creature::CREATURE_ITME_TYPE type)
{
    m_creatureItemType = type;
}

// ORIG _ZN13user_creature13CCreatureItem9GetItemIdEv W 0x0833efa4（size 0x0b）：
// mov 0x4(%eax),%eax 直返。ORIG 为外联弱符号，CEgg::SaveCreatureItem 等以 call 调
// （不内联）；故头内仅声明、此处外联定义以产出真实 call。
int CCreatureItem::GetItemId()
{
    return m_itemId;
}

// ORIG _ZN13user_creature13CCreatureItem6GetUidEv W 0x0833efbe（S23B：
// SetStomach 0x08339ee0+0x69 实证调用点为 call，内联形违 ORIG，同 GetItemId 案）。
int CCreatureItem::GetUid()
{
    return m_uid;
}

// ORIG _ZN13user_creature13CCreatureItem11GetNoChargeEv W 0x0833efd6（size 0x0e）：
// movzbl 0x14(%eax),%eax 直返。P9：返回 unsigned char（bool 会生成 setne 规范化）。
// 头内不再保留 inline 定义（无调用点则永不 emission → 仅 ORIG 缺口），外联产出符号。
unsigned char CCreatureItem::GetNoCharge()
{
    return m_noCharge;
}

// ORIG _ZN13user_creature13CCreatureItem7GetStatEv W 0x0833eff0（size 0x0e）：
// movzbl 0x15(%eax),%eax 直返。
unsigned char CCreatureItem::GetStat()
{
    return m_stat;
}

// ============================================================================
// CEgg（继承 CCreatureItem；布局替换 decl 见 txt）
// ============================================================================

// ORIG _ZN13user_creature4CEggC1Ev T 0x08338f4c（报告 CEgg.md 全汇编转写）：
// ① 基类 ctor（写 CCreatureItem vtable 0x8c2f4d8）
// ② vptr ← &_ZTVN13user_creature4CEggE+0x8（nm V 0x08c2f4a0 → +8 = 0x08c2f4a8）
// ③ Reset()
// 镜像以 int m_vptr 槽模拟多态头，无虚表生成。
// ORIG 只调一次 CCreatureItemC1（编译器隐式基类构造）；显式
// `CCreatureItem();` 会额外构造一次临时对象（NEW 反汇编 2 次 C1），删除。
CEgg::CEgg()
{
    m_vptr = (int)0x08c2f4a8;  // ORIG movl $&_ZTVN13user_creature4CEggE+0x8
    Reset();
}

// ORIG _ZN13user_creature4CEgg5ResetEv T 0x08338f82（报告 Reset.md 全汇编转写）：
// +0x08/+0x0c/+0x10 清零、+0x14 字节清零（均为基类字段）。
void CEgg::Reset()
{
    m_creatureItemType = 0;  // +0x08
    m_uid = 0;               // +0x0c
    m_slotNo = 0;            // +0x10
    m_noCharge = 0;          // +0x14（字节）
}

// 【objdump】ORIG _ZN13user_creature4CEgg6IsSaveEv W 0x0833f102（size 0x0a）：
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— ORIG 符号字面
// 返回 0（蛋条目默认不可保存态；全二进制无此 W 符号的 call 引用，仅 linkonce
// 导出）。按指令忠实转写，非编造固定桩。
bool CEgg::IsSave()
{
    return false;
}

// ORIG _ZN13user_creature4CEgg16LoadCreatureItemEPNS_12CCreatureMgrEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x08338fac（LoadCreatureItem.md 全汇编转写）：蛋条目 DB 载入侧。
bool CEgg::LoadCreatureItem(CCreatureMgr* mgr, Inven_Item* item,
                            SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user)
{
    if (info->m_stat != 0)          // +0x27 非 0 → 条目已删除标记
    {
        item->reset();              // ORIG call Inven_Item::reset(item)
        return false;
    }

    SetItemId(item->m_addInfo);     // *(int*)(item+0x02) 驱动脚本解析
    m_uid = info->m_uid;            // +0x0c ← info+0x04
    m_slotNo = info->m_slotNo;      // +0x10 ← info+0x00
    m_creatureItemType = (int)(char)info->m_creatureItemType;  // movsbl +0x24
    m_expireTime = info->m_expireTime;                          // +0x18 ← info+0x28
    item->m_field0 = 1;             // *item = (Inven_Item)0x1（首字节置 1）
    m_noCharge = (info->m_noCharge != 0) ? 1 : 0;               // setne → +0x14
    m_stat = info->m_stat;          // +0x15 ← info+0x27

    // ORIG call CCreatureMgr::RegisterCreatureItem(this, info->m_uid)；
    // 返回 xor $1 后 je 失败路径 → !RegisterCreatureItem(...) 则返 0。
    if (!mgr->RegisterCreatureItem(reinterpret_cast<CCreatureItem*>(this),
                                   info->m_uid))
        return false;

    // 回填 CREATURE 区槽物：ref = getCurCharacInvenW(user)->GetInvenRef(3, slotNo)
    Inven_Item* ref =
        user->getCurCharacInvenW()->GetInvenRef(
            3, reinterpret_cast<CCreatureItem*>(this)->GetSlotNo());
    ref->m_field0 = 1;              // ORIG movb $0x1,(%eax)
    ref->set_add_info(m_uid);       // ORIG call Inven_Item::set_add_info(ref, this+0x0c)
    return true;
}

// ORIG _ZN13user_creature4CEgg16SaveCreatureItemEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x083390c8（SaveCreatureItem.md 全汇编转写）：蛋条目序列化；恒返 1。
// 首写经 GetItemId()（外联 call，见 CCreatureItem::GetItemId 定义）。残余
// [b22判词·N82]：ORIG 帧 `sub $0x18` vs 4.4 `sub $0x4`（帧槽预留宽窄），+0x14
// 字节 ORIG 经 `movzbl;mov%eax,%edx` 双寄存路由 vs 4.4 直 `movzbl 至 edx`，
// 均槽/寄存器分配差、语义等价、源码层不可复现。
bool CEgg::SaveCreatureItem(Inven_Item* item,
                            SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user)
{
    (void)user;                     // ORIG 未读该实参
    item->m_addInfo = GetItemId();  // *(int*)(item+0x02) ← CCreatureItem::GetItemId()（ORIG call）
    info->m_uid = m_uid;            // info+0x04 ← this+0x0c
    info->m_slotNo = m_slotNo;      // info+0x00 ← this+0x10
    info->m_creatureItemType =
        (unsigned char)m_creatureItemType;                      // +0x24 低字节
    info->m_noCharge = m_noCharge;  // info+0x26 ← this+0x14（movzbl 还原再写）
    info->m_stat = m_stat;          // info+0x27 ← this+0x15
    return true;
}

// ============================================================================
// CDBMsgSender（Init/C1/dtor/Send 归 CCreatureMgr.cpp；此处补余下消息填充族）
// ============================================================================

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender6CancelEv W 0x0833f994：
// m_stream 空 → 直接结束（两路径均不设置 eax，返回类型取 void）；
// 按 stream->m_poolType(+0x20) 分池归还后置空持有。
//   ==1 → SmallStreamPool::Free(GlobalData::s_small_stream_pool@0x940bda8)
//   ==2 → BigStreamPool::Free  (GlobalData::s_big_stream_pool@0x940bda4)
//   ==0/default → StreamPool::Free      (GlobalData::s_stream_pool@0x940bd6c)
void CDBMsgSender::Cancel()
{
    if (m_stream == 0)
        return;
    switch (m_stream->m_poolType)
    {
        case 1: GlobalData::s_small_stream_pool->Free(m_stream); break;
        case 2: GlobalData::s_big_stream_pool->Free(m_stream); break;
        default: GlobalData::s_stream_pool->Free(m_stream); break;
    }
    m_stream = 0;
}

// ============================================================================
// 共同骨架（ChangeOwner/ChangeSlotNo/AuctionRegist 同构，objdump 逐条核对）：
//   m_stream 空 → 0；buf = GetInBuffer<SIG_CREATURE_ITEM>()；
//   memset(buf, 0, 0x74)；compat(buf+0x04).setCopy(*item)；
//   buf->m_addInfo2(+0x30) ← item+0x07（低半 addInfo2）；随后各自差异字段。
// ============================================================================

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender11ChangeOwnerEP10Inven_Itemiiiiii
// W 0x0833ffe8：差异体——
//   +0x51(byte)=p3(reason)；+0x48=p1(characNo)；hdr|=0x30；+0x2c=p2(slot)；
//   +0x64=1；+0x68=p4(mapSize)；+0x6c=p5；+0x70=p6；返 1。
bool CDBMsgSender::ChangeOwner(Inven_Item* item, int characNo, int slot,
                               int reason, int mapSize, int field6c, int field70)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x30;                               // or $0x30
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 1;
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_field6c = field6c;                         // +0x6c
    buf->m_field70 = field70;                         // +0x70
    return true;
}

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender12ChangeSlotNoEP10Inven_Itemiii
// W 0x083400a4：差异体——
//   +0x51(byte)=p3(reason)；+0x48=p1(characNo)；hdr|=0x20；+0x2c=p2(slot)；
//   +0x64=2；返 1（无 +0x68 区写入）。
bool CDBMsgSender::ChangeSlotNo(Inven_Item* item, int characNo, int slot,
                                int reason)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x20;                               // or $0x20
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 2;
    return true;
}

// 【objdump】ORIG _ZN13user_creature12CDBMsgSender13AuctionRegistEP10Inven_Itemiiii
// W 0x08340144：差异体——
//   +0x48=p1(characNo)；hdr|=0x100（or $0x1,%dh）；+0x53=1；+0x64=5；
//   +0x68=p4(mapSize)；返 1。p2(@0x14)/p3(@0x18) 在汇编中未被使用（保留形参）。
bool CDBMsgSender::AuctionRegist(Inven_Item* item, int characNo, int unused14,
                                 int unused18, int mapSize)
{
    (void)unused14;   // ORIG 未读
    (void)unused18;   // ORIG 未读
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x100;                              // or $0x1,%dh
    buf->m_field53 = 1;                               // +0x53
    buf->m_field64 = 5;
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
}

// ============================================================================
// 全局池尺寸包装器（均转发 CCreatureItemFactory 单例查询）
// ============================================================================

// 【objdump】ORIG _ZN13user_creature22GetCreatureEggPoolSizeEv T 0x08336c63：
// GetInstanceCreatureItemFactory()->GetSize(type=0)（egg）。
int GetCreatureEggPoolSize()
{
    return GetInstanceCreatureItemFactory()->GetSize(CREATURE_ITME_TYPE_EGG);
}

// 【objdump】ORIG _ZN13user_creature19GetCreaturePoolSizeEv T 0x08336c80：
// GetInstanceCreatureItemFactory()->GetSize(type=1)（creature）。
int GetCreaturePoolSize()
{
    return GetInstanceCreatureItemFactory()->GetSize(CREATURE_ITME_TYPE_CREATURE);
}

// 【objdump】ORIG _ZN13user_creature28GetCreatureEggPoolRemainSizeEv T 0x08336c9d：
// GetInstanceCreatureItemFactory()->GetRemainSize(type=0)。
int GetCreatureEggPoolRemainSize()
{
    return GetInstanceCreatureItemFactory()->GetRemainSize(
        CREATURE_ITME_TYPE_EGG);
}

// 【objdump】ORIG _ZN13user_creature25GetCreaturePoolRemainSizeEv T 0x08336cba：
// GetInstanceCreatureItemFactory()->GetRemainSize(type=1)。
int GetCreaturePoolRemainSize()
{
    return GetInstanceCreatureItemFactory()->GetRemainSize(
        CREATURE_ITME_TYPE_CREATURE);
}

}  // namespace user_creature
