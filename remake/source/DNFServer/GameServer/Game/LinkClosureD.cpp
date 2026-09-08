// LinkClosureD.cpp — 全量链接收敛批 D 散件 producer（LinkClosureD 批次）
//
// 覆盖符号（ORIG nm -S 核对）：
//   fnEnumDungeonClear(short,char,char*)      T 0x08678166（报告 _lt_global_gt_/fnEnumDungeonClear.md）
//   RewardItem(CUser*,long,long,long,uint,PKc,PKc,uint) T 0x084bfb84（报告缺失，objdump 逐指令转写）
//
#include "SigTypes.h"                 // SIG_LOAD_INVENTORY 权威布局
#include "CTitleBook.h"               // item_lock::stItemLockRef（CItemLockTimer/lock 池权威布局）
#include "CDataManager.h"             // G_CDataManager / find_item
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "GlobalData.h"               // s_systemTime_
#include "CSystemTime.h"
#include "CUserCharacInfo.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CItem.h"
#include "CUser.h"
#include "Arad_DataManager.h"     // ARAD::Singleton / getExceptionItem / ExceptionItem
#include "GameTypes.h"            // cMyTrace
#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t
#include "DNFFunctionLib.h"       // DNFFLib::Binary2Hex（GUID 渲染）
#include "CInventory.h"           // GetSlotItemType（_verifyItem type==1 分支）

// ORIG mangled 依赖（仓库无 producer —— §8.1 仅声明，U 引用合规，收敛清单已报 Main）：
//   _Z11_verifyItemiPK10Inven_Item14INVENTORY_TYPEbbP5CUser            W 0x084efcdd
//   _Z39_writeIncorrectItemInventoryForRecoveryjRK10Inven_ItemPKcii    W 0x084f0075
//   _Z20meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item        T 0x084c3da5
//   _Z25cago_meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item   T 0x084c3f2c
// INVENTORY_TYPE 为 ORIG mangled 内嵌枚举（14 长度名，值 0..3 实证；成员名
// 未还原 [推断]），由本 TU 建模承载。
enum INVENTORY_TYPE
{
    INVENTORY_TYPE_0 = 0,
    INVENTORY_TYPE_1 = 1,
    INVENTORY_TYPE_2 = 2,
    INVENTORY_TYPE_3 = 3
};

int _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
                bool b1, bool b2, CUser* user);
void _writeIncorrectItemInventoryForRecovery(unsigned int characNo,
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot);
void meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig, Inven_Item* item);
void cago_meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig,
                               Inven_Item* item);

// ORIG 段内联展开（无独立符号）：collectLock 逐段直接内联，用共享
// 函数级临时 lock/ref 槽对齐 ORIG 的 6 个栈槽（frame 0x38 → disp8 编码）。

// ---------------------------------------------------------------------------
// ORIG W 0x084f0145 sz 0x647（报告缺失；objdump 0x084f0145..0x084f078c 逐指令
// 转写）。SIG_LOAD_INVENTORY 各 wire 段（item stride 0x3d，pack(1)）逐件
// _verifyItem；任一失败 → _writeIncorrectItemInventoryForRecovery 并返 0；
// 途中把各段 amplify lock 非零者收集到 lockRefs（stItemLockRef{lock,space,slot}）。
//   段A equip 区  i 0x00..0x0b  item=sig+0x14aa8  TYPE 0 (b1=0,b2=1)
//                 lock@s+0x14ab9 → space 0x12, slot i+0xa；tag "fairpvp_equip"
//   段B           i 0x0a..0x15  item=sig+0xa3a1   TYPE 1 (0,1)
//                 lock@s+0xa3b2 → space 0x03, slot i；tag "equip"
//                 ※ item+0xb 非零 → meterial_item_change(i,sig,item)
//   段C           i 0x00..0x137 item=sig+0x5949   TYPE 1 (1,0)
//                 lock@s+0x595a → space 0x00, slot i；tag "inven"
//   段D           i 0x00..0x97  item=sig+0xa8e4   TYPE 2 (0,0)
//                 lock@s+0xa8f5 → space 0x02, slot i；tag "cargo"
//                 ※ item+0x06 非零 → cago_meterial_item_change(i,sig,item)
//   段E           i 0x00..0xf1  item=sig+0xeed1   TYPE 3 (0,0)
//                 lock@s+0xeee2 → space 0x07, slot i；tag "creature"
//   段F 仅收集     i 0x00..0x09  lock@s+0xa3b2 → space 0x03, slot i
//   段G 仅收集     i 0x00..0x68  lock@s+0x4065 → space 0x01, slot i
// 全部通过返回 1。
// ---------------------------------------------------------------------------
// [b28d-L8] N82 | 帧差 + 结构寻址（add vs imul 链，ORIG 0x08xxxxxx）
bool _verifyInventory(SIG_LOAD_INVENTORY* sig,
                      std::vector<item_lock::stItemLockRef>& lockRefs,
                      CUser* user)
{
    int i;
    Inven_Item* item;
    int err;
    unsigned char lock;
    item_lock::stItemLockRef ref;

    // 段 A（SIG_LOAD_INVENTORY m_expandEquip.m_items，+0x14aa8 起 0x24 项）
    for (i = 0; i <= 0xb; ++i)
    {
        item = &sig->m_expandEquip.m_items[i];
        lock = item->m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x12;
            ref.m_slot = static_cast<short>(i + 0xa);
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, item, INVENTORY_TYPE_0, true, true, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, *item, "fairpvp_equip", err, i);
            return false;
        }
    }
    // 段 B（m_equipMirror）——ORIG 无 meterial_item_change 调用（仅段 C/D 有）
    for (i = 0xa; i <= 0x15; ++i)
    {
        item = &sig->m_equipMirror[i];
        lock = item->m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x03;
            ref.m_slot = static_cast<short>(i);
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, item, INVENTORY_TYPE_0, true, true, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, *item, "equip", err, i);
            return false;
        }
    }
    // 段 C（m_invenType1）
    for (i = 0; i <= 0x137; ++i)
    {
        item = &sig->m_invenType1[i];
        lock = item->m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x00;
            ref.m_slot = static_cast<short>(i);
            lockRefs.push_back(ref);
        }
        if (item->m_fieldb != 0 || item->m_fieldd != 0)
            meterial_item_change(i, sig, item);
        err = _verifyItem(i, item, INVENTORY_TYPE_1, false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, *item, "inven", err, i);
            return false;
        }
    }
    // 段 D（m_cargo）
    for (i = 0; i <= 0x97; ++i)
    {
        item = &sig->m_cargo[i];
        lock = item->m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x02;
            ref.m_slot = static_cast<short>(i);
            lockRefs.push_back(ref);
        }
        // ORIG *(int*)(item+0x06)：m_attr(+0x06)+m_addInfo2 低 3 字节槽非零判定
        if (item->m_attr != 0 || (item->m_addInfo2 & 0xffffff) != 0)
            cago_meterial_item_change(i, sig, item);
        err = _verifyItem(i, item, INVENTORY_TYPE_2, false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, *item, "cargo", err, i);
            return false;
        }
    }
    // 段 E（m_invenType3）
    for (i = 0; i <= 0xf1; ++i)
    {
        item = &sig->m_invenType3[i];
        lock = item->m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x07;
            ref.m_slot = static_cast<short>(i);
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, item, INVENTORY_TYPE_3, false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, *item, "creature", err, i);
            return false;
        }
    }
    // 段 F（仅收集；m_equipMirror[i].m_amp，+0xa3b2 = 0xa3a1+0x11）
    for (i = 0; i <= 9; ++i)
    {
        lock = sig->m_equipMirror[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x03;
            ref.m_slot = static_cast<short>(i);
            lockRefs.push_back(ref);
        }
    }
    // 段 G（仅收集；ORIG +0x4065 = m_invenType2[i].m_random.m_field2 区，
    // 按 stAmplifyOption_t 视图读取——ORIG 语义保留类型视图）
    for (i = 0; i <= 0x68; ++i)
    {
        lock = reinterpret_cast<const stAmplifyOption_t*>(
            &sig->m_invenType2[i].m_random.m_field2)->GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_space = 0x01;
            ref.m_slot = static_cast<short>(i);
            lockRefs.push_back(ref);
        }
    }
    return true;
}
#include "CItem.h"
#include "CDataManager.h"             // G_CDataManager / find_item
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "GlobalData.h"               // s_systemTime_
#include "CSystemTime.h"

// ---------------------------------------------------------------------------
// ORIG T 0x08678166 sz 0x4e（报告全文已读）：ctx 首槽为 InterfacePacketBuf*，
// put_short(dungeonIdx) → put_byte(diff) → 返回 1。
// 消费点：CUser::SendDungeonClearList 经 enumDungeonClear 回调（CUser.h 已修正拼写）。
// ---------------------------------------------------------------------------
// [b28d-L8] P75/N7 | p+=4 4.4 恒 addl mem（4.1 lea+mov，gA/gB/gD/gE 四形穷举不可造）+ movswl 寄存器角色差（ORIG 0x08678166）
int fnEnumDungeonClear(short dungeonIdx, char diff, char* userData)
// [b28c-L23 真语义 bug 修正 + N7/P75 残余] ①已修游标步长：ORIG @0x0867817c
// `lea 0x4(%eax),%edx; mov %edx,0x10(%ebp)` 表明回调游标按**指针槽 4 字节**前移，
// 原码 `userData++`（char* 步长 1）使第 2 条起读错偏移。②残余 9 行 = 4.4 对
// `p += 4` 恒发单条 `addl $4,mem`（4.1 为 lea+mov 两条，探针 /tmp/l23/p2.cpp
// gA/gB/gD 四形穷举不可造）+ 两处实参装载寄存器差（ORIG `movswl→%edx`、
// 4.4 `movswl→%eax`，N7）。语义与 ORIG 一致。
{
    // ORIG：lea 0x4→写回形参槽 + 旧值解引用（后缀 ++ 语义，4.1 发射写回）。
    // b28c-l23 真语义 bug 修正：ORIG @0x0867817c 为 `lea 0x4(%eax),%edx;
    // mov %edx,0x10(%ebp)` ⇒ 游标按**指针槽 4 字节**前移（枚举多条目时每条占
    // 一个 4 字节槽）；原写法 `userData++`（char* 步长 1）使第 2 条起读错偏移。
    // 探针 /tmp/l23/p2.cpp gA/gB/gE：4.4 恒"解引用在前 + addl 在后"（无法产出
    // 4.1 的 lea 前置 + 双写回），余 1 条为跨版本指令选择差（P75 族）。
    InterfacePacketBuf* buf = *reinterpret_cast<InterfacePacketBuf**>(userData);
    userData += 4;
    buf->put_short(static_cast<int>(dungeonIdx));
    buf->put_byte(static_cast<int>(diff));
    return 1;
}

// ---------------------------------------------------------------------------
// ORIG T 0x084bfb84 sz 0x1a4（报告缺失；objdump 0x084bfb84..0x084bfd28 逐指令
// 转写，下称证据 A）。语义：时间窗检查通过后按 itemIdx 生成物品附 add_info=1，
// RDAR 脚本文案作标题/正文经 ReqDBSendNewSystemMail 系统邮件投递。分支细节：
//   now = GlobalData::s_systemTime_.getCurSec()（证据 A 84bfb8f）
//   end > now → 返回；start3 != 0 且 start3 < now → 返回；begin > end → 返回
//   Inven_Item item;（默认 ctor 0x080cb854）
//   CItem* found = G_CDataManager()->find_item(itemIdx)；空 → 返回
//   itemIdx 存 item+0x02 槽（-0x64，find_item 前预存）；found->make_item(item)
//   （vptr+0x08 槽）→ item.set_add_info(1)
//   title = g_scriptStringManager_.findString(4, titleStr, 0)；strncpy(t,0x14)
//   msg   = g_scriptStringManager_.findString(4, msgStr, 0)；strncpy(m,0xff)
//   ReqDBSendNewSystemMail(title, item, 0, characNo, msg, strlen(msg),
//                          p8, group, false, false)
// 参数表（证据 A 槽序）：0x8 user / 0xc begin / 0x10 end / 0x14 start3 /
//   0x18 itemIdx / 0x1c titleStr / 0x20 msgStr / 0x24 p8(uint)。
// ---------------------------------------------------------------------------
// [b28d-L8] N82 | 帧差 0x190 vs 0x194 + 参数装载序（ORIG 0x084bfb84）
void RewardItem(CUser* user, long begin, long end, long start3,
                unsigned int itemIdx, const char* titleStr,
                const char* msgStr, unsigned int p8)
{
    long now = GlobalData::s_systemTime_.getCurSec();
    if (end > now)
        return;
    if (start3 != 0 && start3 < now)
        return;
    if (begin > end)
        return;

    Inven_Item item;
    const CItem* found = G_CDataManager()->find_item((int)itemIdx);
    if (found == 0)
        return;

    found->make_item(item);
    item.set_add_info(1);

    char title[0x14];
    strncpy(title, g_scriptStringManager_.findString(4, titleStr, 0), 0x14);
    char body[0xff];
    strncpy(body, g_scriptStringManager_.findString(4, msgStr, 0), 0xff);

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, characNo, body, (int)strlen(body), p8, group,
        false, false);
}

// ---------------------------------------------------------------------------
// meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z20meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item @0x084c3da5
// sz0x187。报告 docs/class_func_reports/_lt_global_gt_/meterial_item_change.md。
// sig/item 空判 → logFormat；ExceptionItem::find_index(item->m_addInfo) 且
// m_field1==3 命中 → m_field1=0xa；do-while i=0xc9..0xf8 扫 SIG 材料槽
// （0x5949 + i*0x3d，步长 0x3d）：空槽 setCopy+reset 返回；同 addInfo 槽
// m_addInfo2 累加后 reset 返回；溢出 logFormat 0x161d。
// ---------------------------------------------------------------------------
// [b28d-L8] N82 | 块序/槽平移（ORIG 0x08xxxxxx）
void meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig, Inven_Item* item)
{
    if (sig == 0 || item == 0)
    {
        LogManager::logFormat(1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x1604, "selected_item | pInfo is null");
        return;
    }

    int idx = 0xc9;
    bool hit =
        ARAD::Singleton<ARAD::Arad_DataManager>::Get()->getExceptionItem()
            ->find_index(item->m_addInfo)
        && item->m_field1 == 3;
    if (!hit)
        return;

    item->m_field1 = 0xa;
    do
    {
        Inven_Item* dst = &sig->m_invenType1[idx];
        if (dst->isEmpty())
        {
            dst->setCopy(*item);
            item->reset();
            return;
        }
        if (dst->m_addInfo == item->m_addInfo)     // +0x594b = 0x5949+0x02
        {
            dst->m_addInfo2 += item->m_addInfo2;   // +0x5950 = 0x5949+0x07
            item->reset();
            return;
        }
        ++idx;
    } while (idx <= 0xf8);

    LogManager::logFormat(1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x161d, "new_meterial_slot_index overflow");
}

// ---------------------------------------------------------------------------
// cago_meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z25cago_meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item
// @0x084c3f2c sz0x66。报告 _lt_global_gt_/cago_meterial_item_change.md：
// cargo 版仅做 find_index && m_field1==3 命中后的 m_field1=0xa 标记。
// ---------------------------------------------------------------------------
void cago_meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig, Inven_Item* item)
{
    // [b28c-lane24] ORIG 0x084c3f2c：两个失败路径各有 nop;jmp 中继岛
    //（+0x5c nop+jmp / +0x5f nop），赋值块尾另有 jmp 到统一 epilogue；
    // P223 goto 尾空标签形复现（4.4 对 && 短路恒 jump-thread 无岛）。
    if (sig == 0) goto L_sig;
    if (item == 0) goto L_item;
    if (ARAD::Singleton<ARAD::Arad_DataManager>::Get()->getExceptionItem()
            ->find_index(item->m_addInfo)
        && item->m_field1 == 3)
    {
        item->m_field1 = 0xa;
    }
L_item:;
L_sig:;
}

// ---------------------------------------------------------------------------
// CItemGloballyUniqueIdentifierPrint（ORIG W 0x84ebe58 族）：ctor memset 0x15、
// op() = Binary2Hex(&guid,0xa,buf)、dtor 空。与 cUserHistoryLog.cpp 局部视图同型。
// ---------------------------------------------------------------------------
class CItemGloballyUniqueIdentifierPrint
{
public:
    CItemGloballyUniqueIdentifierPrint() { memset(m_buf, 0, sizeof(m_buf)); }
    ~CItemGloballyUniqueIdentifierPrint() {}

    const char* operator()(const itemGloballyUniqueIdentifier_t& guid)
    {
        DNFFLib::Binary2Hex(reinterpret_cast<const unsigned char*>(&guid), 0xa,
                            m_buf);
        return m_buf;
    }

private:
    char m_buf[0x15];
};

// ---------------------------------------------------------------------------
// _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
//             bool b1, bool b2, CUser* user)
// ORIG W _Z11_verifyItemiPK10Inven_Item14INVENTORY_TYPEbbP5CUser @0x084efcdd
// sz0x398。报告缺失（objdump 0x084efcdd..0x084f0075 逐段转写）。返回错误码，
// 0 = 通过；0x1562..0x15f8 为各校验失败码（语义见各分支注释）。
// 判例（ADeepY4[7]）：DIFF(N封)，gap +9（orig 290 / our 299，was 301）。真 bug 修复：
//   m_field0 校验 `if(m_field0 && !b1)`→`if(m_field0 && b1)`。ORIG 84efe89 区：
//   `movzbl(%item)(m_field0);test;je续;cmpb $0,-0x4c(b1);je续;mov $0x157d`——失败条件是
//   m_field0 非零 **且** b1 非零（P196 布尔反演，equip 段 b1=true 拒带残留 flag 物品、
//   inven 段 b1=false 放行）；原译写 !b1 为反。翻转后 xor $1 消失、代码 301→299。
//   残余：GUID 分支 ORIG 将 guid/characNo/accId 入 edi/esi/ebx 提前(cMyTrace ctor 前)，
//   4.4 下栈槽(N82)；stackable 位测 ORIG `shl %cl;and $0x69c`、我们强度归约 `shr;and $1`
//   (N86b 等价)；非 stackable type 分发 ORIG 链式 cmp、我们区间式。错误码 0x1562..0x15f8
//   与空槽全零/rarity/attachtype 各分支语义全部对应。
// ---------------------------------------------------------------------------
// [b28d-L8] N82 | 帧差 0x6c + 槽平移（ORIG 0x08xxxxxx）
int _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
                bool b1, bool b2, CUser* user)
{
    if (item->m_addInfo == 0)
    {
        // 空槽：必须全零（recovery 校验），残留 → 0x1562
        if (item->m_field0 != 0 || item->m_field1 != 0 ||
            item->GetItemAttr() != 0 || item->m_addInfo2 != 0 ||
            item->m_fieldb != 0 || item->m_fieldd != 0 ||
            *(int*)(reinterpret_cast<const char*>(item) + 0x11) != 0)
            return 0x1562;
        return 0;
    }

    if (item->m_addInfo == 0x71d7)
    {
        // GUID 物品：渲染 guid 串并 trace（校验继续）
        CItemGloballyUniqueIdentifierPrint print;
        const char* guid = print(*(const itemGloballyUniqueIdentifier_t*)(
            const void*)(reinterpret_cast<const char*>(item) + 0x15));
        int characNo = user->get_charac_no(-1);
        unsigned int accId = user->get_acc_id();
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x156a, 5);
        trace("[HaveAccountLog] Invalid item. (mid:%u, charac:%u, item:%u, guid:%s)",
              accId, characNo, (unsigned int)item->m_addInfo, guid);
    }

    const CItem* def = G_CDataManager()->find_item(item->m_addInfo);
    if (def == 0)
        return 0x1576;
    if (def->get_index() != item->m_addInfo)
        return 0x157a;
    if (item->m_field0 != 0 && b1)
        return 0x157d;

    if (def->is_stackable())
    {
        if (b2)
            return 0x1582;
        // ORIG 0x84efec2：movzbl 0x1(%item) → t；t>0xa || (1<<t & 0x69c)==0
        // → 0x158f（0x69c = 位 2,3,4,7,9,10 允许集；t 检查先于 limit 检查）
        unsigned int t = (unsigned char)item->m_field1;
        if (t > 0xa || (0x69c & (1u << t)) == 0)
            return 0x158f;
        const CStackableItem* stackDef =
            static_cast<const CStackableItem*>(def);
        if (item->m_addInfo2 > stackDef->getStackableLimit() ||
            item->m_addInfo2 < 0)
            return 0x159e;
    }
    else
    {
        // 非 stackable：type=1 直接通过；5/6 需 INVENTORY_TYPE 0 或 3；8 需 0
        unsigned int t = (unsigned int)item->m_field1;
        if (t == 1)
            ;
        else if (t >= 5 && t <= 6)
        {
            if (type != INVENTORY_TYPE_0 && type != INVENTORY_TYPE_3)
                return 0x15b0;
        }
        else if (t == 8)
        {
            if (type != INVENTORY_TYPE_0)
                return 0x15b4;
        }
        else
            return 0x15b7;
    }

    if (def->get_rarity() <= 1 && item->GetReSealCount() != 0)
        return 0x15c2;

    if (def->GetAttachType() <= 5)
    {
        if ((0x27 & (1u << (unsigned int)def->GetAttachType())) != 0)
        {
            if (item->GetReSealCount() != 0)
                return 0x15d9;
            if (item->m_field0 != 0)
                return 0x15e1;
        }
    }

    if (type != INVENTORY_TYPE_1)
        return 0;

    if (user->getCurCharacR() == 0)
        return 0x15f0;

    int slotType = user->getCurCharacInvenR()->GetSlotItemType(slot);
    if (slotType != 0)
    {
        if ((unsigned int)item->m_field1 != (unsigned int)slotType)
            return 0x15f8;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// _writeIncorrectItemInventoryForRecovery(uint, const Inven_Item&, const char*,
//                                        int, int)
// ORIG W _Z39_writeIncorrectItemInventoryForRecoveryjRK10Inven_ItemPKcii
// @0x084f0075 sz0xd0。报告缺失（objdump 转写）：单条 ERROR_ITEM_LOG 日志，
// 字段序（含 m_amp 首 4 字节 int 视图）见 logFormat 实参。
// ---------------------------------------------------------------------------
// [b28d-L8] N82 | 帧差 0x6c vs 0x7c + 装载序（ORIG 0x08xxxxxx）
void _writeIncorrectItemInventoryForRecovery(unsigned int characNo,
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot)
{
    int ampHead = *(const int*)(const void*)&item.m_amp;   // +0x11（amp 首 int 视图）
    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x1645,
        "ERROR_ITEM_LOG %u,%d,%d,%u,%d,%d,%d,%u,%d - %s slot(%d), error_line(%d)\n",
        characNo, (int)item.m_field0, (int)item.m_field1, item.m_addInfo,
        (int)item.GetItemAttr(), ampHead, item.m_fieldd, (int)item.m_fieldb,
        item.m_addInfo2, tag, slot, err);
}
