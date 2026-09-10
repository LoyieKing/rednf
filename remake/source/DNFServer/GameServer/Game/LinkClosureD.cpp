// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureD/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "LinkClosureD/_verifyItem.inc"
                bool b1, bool b2, CUser* user);
#include "LinkClosureD/_writeIncorrectItemInventoryForRecovery.inc"
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot);
#include "LinkClosureD/meterial_item_change.inc"
#include "LinkClosureD/cago_meterial_item_change.inc"
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
//   段C           i 0x00..0x137 item=sig+0x5949   TYPE 1 (1,0)
//                 lock@s+0x595a → space 0x00, slot i；tag "inven"
//                 ※ [Q7S3 修正] 守卫读绝对 0x594b（= item+0x02 = m_addInfo）
//                   → meterial_item_change(i,sig,item)；旧注 "item+0xb" 系
//                   把「相对 0x5940 基址的偏移」误当 item 偏移（真 bug 已修）。
//   段D           i 0x00..0x97  item=sig+0xa8e4   TYPE 2 (0,0)
//                 lock@s+0xa8f5 → space 0x02, slot i；tag "cargo"
//                 ※ [Q7S3 修正] 守卫读绝对 0xa8e6（= item+0x02 = m_addInfo）
//                   → cago_meterial_item_change(i,sig,item)；旧注 "item+0x06"
//                   同上为基址偏移误记（真 bug 已修）。
//   段E           i 0x00..0xf1  item=sig+0xeed1   TYPE 3 (0,0)
//                 lock@s+0xeee2 → space 0x07, slot i；tag "creature"
//   段F 仅收集     i 0x00..0x09  lock@s+0xa3b2 → space 0x03, slot i
//   段G 仅收集     i 0x00..0x68  lock@s+0x4065 → space 0x01, slot i
// 全部通过返回 1。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | 已收：P183 去 item 指针缓存（每点重发
// imul/add 链，gap 372→424 vs ORIG 427）+ P171/P191 预清与双初始化（lock/i/err
// 槽序 -0x11/-0x10/-0xc 全等）。残余 = 4.1 对部分段 `if(x!=0)` 物化
// `setne;test;je`（4.4 直跳，setcc-fold 负例；P225 while+break 实测可造该物化但
// 净增 23 条、助记符差异不减，故回退）+ 少量常量折叠（ORIG 0x14ab0+9 合并 vs
// 我们 0x14aa0+8+11，N4/P30）。语义与 ORIG 逐段一致（ORIG 0x084f0145 sz0x647）。
#include "LinkClosureD/_verifyInventory.inc"
                      std::vector<item_lock::stItemLockRef>& lockRefs,
                      CUser* user)
{
    // ORIG 0x084f0185..19a 预清（P171 照抄）：lock(-0x11)=0、i(-0x10)=0、err(-0xc)=0、
    // 随后 `movl $0x0,-0x10` 二次初始化（P191 双初始化形）。
    // ORIG 每处访问重发完整 imul 链（P183 地址重物化铁律）⇒ 无 item 指针缓存。
    // ref 赋值序 ORIG 为 m_lock→m_slot→m_space（0x084f019f..1b0）。
    // ORIG 预清序 lock(-0x11) → i(-0x10) → err(-0xc) ⇒ 声明序 lock, i, err。
    unsigned char lock = 0;
    int i = 0;
    int err = 0;
    item_lock::stItemLockRef ref;

    // 段 A（SIG_LOAD_INVENTORY m_expandEquip.m_items，+0x14aa8 起 0x24 项）
    for (i = 0; i <= 0xb; ++i)
    {
        lock = sig->m_expandEquip.m_items[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i + 0xa);
            ref.m_space = 0x12;
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, &sig->m_expandEquip.m_items[i], INVENTORY_TYPE_0,
                          true, true, user);
        // [D01 · C05④ 步骤 2 实测负例 2026-09-10] P225 原形（5 处 `if` 整块换
        // `while (err != 0) { _write…; return false; }`）**劣化**：our_insns
        // 424→**434**、gap −3→**+7**（ORIG 427）、TU 四数不变 ⇒ 已回滚。
        // 与 C01 三正例的差别：本处 body 仅「单条调用 + return」、无多分支早退 +
        // 末尾 break 结构 ⇒ while 恒多回边/结构 jmp。⇒ 本件归 **N-88/P197④**
        // （槽值 vs 常量比较物化：ORIG `mov %eax,-0xc; cmpl $0,-0xc; setne; test; je`）。
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_expandEquip.m_items[i],
                "fairpvp_equip", err, i);
            return false;
        }
    }
    // 段 B（m_equipMirror）——ORIG 无 meterial_item_change 调用（仅段 C/D 有）
    for (i = 0xa; i <= 0x15; ++i)
    {
        lock = sig->m_equipMirror[i].m_amp.GetLock();
        // ORIG 段 B 为 bool 物化形（cmpb $0; setne %al; test %al,%al; je）
        // 与段 A 的 cmpb $0; je 不同 ⇒ P225 while+break 复现。
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x03;
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, &sig->m_equipMirror[i], INVENTORY_TYPE_0,
                          true, true, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_equipMirror[i], "equip", err, i);
            return false;
        }
    }
    // 段 C（m_invenType1）
    for (i = 0; i <= 0x137; ++i)
    {
        lock = sig->m_invenType1[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x00;
            lockRefs.push_back(ref);
        }
        // [Q7S3 语义轴 · 真 bug 修正] ORIG 0x084f03a1..0x084f03e0：基址取
        // sig+i*0x3d+0x5940（= &m_invenType1[i] - 9，数组 +0x5949 由 m_amp
        // GetLock 传参 0x595a-0x11 双证）后 `mov 0xb(%eax); test %eax,%eax`
        // ⇒ 绝对 0x594b = &m_invenType1[i] + 2 = m_addInfo（4 字节整体判）。
        // 旧写 "item+0xb/item+0xd" 系把「相对 0x5940 基址的偏移」误当 item
        // 偏移（对应绝对 0x5954/0x5956，字段为 m_fieldb/m_fieldd）⇒ 守卫条件
        // 与原版不同（真语义 bug，已修）。
        if (sig->m_invenType1[i].m_addInfo != 0)
            meterial_item_change(i, sig, &sig->m_invenType1[i]);
        err = _verifyItem(i, &sig->m_invenType1[i], INVENTORY_TYPE_1,
                          false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_invenType1[i], "inven", err, i);
            return false;
        }
    }
    // 段 D（m_cargo）
    for (i = 0; i <= 0x97; ++i)
    {
        lock = sig->m_cargo[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x02;
            lockRefs.push_back(ref);
        }
        // [Q7S3 语义轴 · 真 bug 修正] ORIG 0x084f051c..0x084f0560：基址取
        // sig+i*0x3d+0xa8e0（= &m_cargo[i] - 4，数组 +0xa8e4 由 _verifyItem/
        // cago_ 传参的 `add $0x4` 双证）后 `mov 0x6(%eax); test %eax,%eax`
        // ⇒ 绝对 0xa8e6 = &m_cargo[i] + 2 = m_addInfo（4 字节整体判）。
        // 旧写 "item+0x06/低 3 字节槽" 对应绝对 0xa8ea/0xa8eb（m_attr/
        // m_addInfo2）⇒ 守卫条件与原版不同（真语义 bug，已修）。
        if (sig->m_cargo[i].m_addInfo != 0)
            cago_meterial_item_change(i, sig, &sig->m_cargo[i]);
        err = _verifyItem(i, &sig->m_cargo[i], INVENTORY_TYPE_2, false, false,
                          user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_cargo[i], "cargo", err, i);
            return false;
        }
    }
    // 段 E（m_invenType3）
    for (i = 0; i <= 0xf1; ++i)
    {
        lock = sig->m_invenType3[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x07;
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, &sig->m_invenType3[i], INVENTORY_TYPE_3,
                          false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_invenType3[i], "creature", err, i);
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
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x03;
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
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x01;
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
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | N7+P75（ORIG 0x08678166）：4.4 对
// `userData += 4` 恒「解引用在前 + addl mem 在后」，4.1 为 lea 前置写回+旧值
// 解引用（后缀 *p++ 发射形）；引用形 `InterfacePacketBuf**& p` 引入引用槽
// 24→29 insn 反劣；movswl/movsbl 目标 %eax vs ORIG %edx（N7，本 TU 未路由
// 4.4.6，P230 限定）。语义等价（先取旧值再按 4 字节前移）。
#include "LinkClosureD/fnEnumDungeonClear.inc"
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
    // ORIG 0x0867817c 为后缀 `*p++` 发射形（lea 0x4 前置写回 + 旧值解引用）。
    // 4.4 无槽别名不可得：引用形 `InterfacePacketBuf**& p` 引入引用槽
    //（lea 0x10(%ebp) → -0x10，+5 insn 反劣 24→29，b28d-S120 实测）；
    // 具名 `p` 形写回的是局部槽非形参槽且 +1 槽。本形语义与 ORIG 全等
    //（先取旧值再按 4 字节前移），残余 = P75 指令调度 + N7 寄存器角色。
    InterfacePacketBuf* buf = *reinterpret_cast<InterfacePacketBuf**>(userData);
    userData += 4;
    buf->put_short(dungeonIdx);
    buf->put_byte(diff);
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
// [b28d-solo-LinkClosureD] NEAR | 真语义 bug 修正：ORIG 0x084bfbfb
// `mov 0x18(%ebp),%eax; mov %eax,-0x64(%ebp)` = make_item 前写 item.m_addInfo
// （原译漏写）。另修：nAddInfo 具名 int（ORIG 0x084bfbc8）、body[0x100]（ORIG
// 栈距 0x100，Ghidra local_16a[256]）、P227 内联实参（group→%ebx/strlen→%esi/
// characNo→%eax 无槽）。帧 0x190 与全部具名槽对齐；残余 = title/item/body 槽
// 整体浅 1 字节（4.1 在 title 与 now 间留 1B 空洞，N95/P28 族）。
#include "LinkClosureD/RewardItem.inc"
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

    // ORIG 0x084bfbc8 `movl $0x1,-0x10(%ebp)` 在 item ctor 之前 ⇒ 具名 int 局部
    int nAddInfo = 1;
    Inven_Item item;
    const CItem* found = G_CDataManager()->find_item((int)itemIdx);
    if (found == 0)
        return;

    // ORIG 0x084bfbfb `mov 0x18(%ebp),%eax; mov %eax,-0x64(%ebp)`：make_item 前
    // 把 itemIdx 写入 item+0x02（真语义缺口，原译漏写）。
    item.m_addInfo = (int)itemIdx;
    found->make_item(item);
    item.set_add_info(nAddInfo);

    // ORIG 深区顶部：title 起点 -0x29（= now -0x14 上方 0x15 字节，Ghidra
    // local_2d[21]）⇒ 数组声明 char[0x15]；strncpy 长度仍 0x14（0x084bfc4f）。
    char title[0x15];
    strncpy(title, g_scriptStringManager_.findString(4, titleStr, 0), 0x14);
    // ORIG 栈距 -0x166..-0x66 = 0x100 字节（Ghidra local_16a[256]），
    // strncpy 长度 0xff（0x084bfc89）。
    char body[0x100];
    strncpy(body, g_scriptStringManager_.findString(4, msgStr, 0), 0xff);

    // ORIG 0x084bfca3..cf4：group→%ebx、strlen→%esi、characNo→%eax 全程无槽
    //（P0）⇒ 三值均内联实参。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, (unsigned int)user->getCurCharacNo(), body,
        (int)strlen(body), p8, user->GetServerGroup(), false, false);
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
// [b28d-solo-LinkClosureD] NEAR | 已收：P183 去 dst 缓存（ORIG 每点重发
// imul 链，0x084c3e40/3e5e/3ed7/3efb 四组）+ P214① 正极性 if(A&&B) 包裹
// （0x084c3e11..e2e 短路物化 mov$1/mov$0 + test 无槽）+ P283 goto 固定物理
// 块序（入口 jmp→非空处理→++idx/尾测 jle 直跳→overflow→体头 isEmpty
// xor$1;jne→空处理）。残余 = ORIG `add $0x5950` vs 我们 `add $0x5940;
// mov 0x10(%eax)`（N4/P30 常量折叠）+ 2 条对齐 nop（§6.1）。
#include "LinkClosureD/meterial_item_change_1.inc"
{
    if (sig == 0 || item == 0)
    {
        LogManager::logFormat(1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x1604, "selected_item | pInfo is null");
        return;
    }

    int idx = 0xc9;
    // ORIG 0x084c3e11..e2e：&& 短路物化 mov$1/mov$0 + test（无槽、无 xor）⇒
    // P214① 正极性包裹 `if (A && B) { 主流程 }`（负极性/取反形恒反演块序）。
    // ORIG 每处访问重发完整 imul/add 链（0x084c3e40/3e5e/3ed7/3efb 四组）⇒
    // 无 dst 指针缓存（P183 地址重物化铁律），逐点全表达式访问。
    if (ARAD::Singleton<ARAD::Arad_DataManager>::Get()->getExceptionItem()
            ->find_index(item->m_addInfo)
        && item->m_field1 == 3)
    {
        item->m_field1 = 0xa;
        // ORIG 物理块序：入口 jmp → 非空处理(+0x9b) → ++idx/尾测(+0xf7..102,
        // `jle` 直跳) → overflow(+0x104) → 体头 isEmpty(+0x132, `xor$1;jne`) →
        // 空处理(+0x156)。用 goto 固定物理顺序（P283），do-while 在 4.4 恒 setle。
        goto L_check;
    L_body:
        if (sig->m_invenType1[idx].m_addInfo == item->m_addInfo)
        {
            sig->m_invenType1[idx].m_addInfo2 += item->m_addInfo2;
            item->reset();
            return;
        }
        ++idx;
        if (idx <= 0xf8)
            goto L_check;

        LogManager::logFormat(1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x161d, "new_meterial_slot_index overflow");
        return;

    L_check:
        if (!sig->m_invenType1[idx].isEmpty())
            goto L_body;
        sig->m_invenType1[idx].setCopy(*item);
        item->reset();
        return;
    }
}

// ---------------------------------------------------------------------------
// cago_meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z25cago_meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item
// @0x084c3f2c sz0x66。报告 _lt_global_gt_/cago_meterial_item_change.md：
// cargo 版仅做 find_index && m_field1==3 命中后的 m_field1=0xa 标记。
// ---------------------------------------------------------------------------
#include "LinkClosureD/cago_meterial_item_change_1.inc"
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
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | 已收：P227 全表达式临时链消 GUID
// 段具名局部（guid→%edi/characNo→%esi/accId→%ebx 跨 ctor 保活，帧 0x80→0x6c
// 与全部槽 -0x1c/-0x20/-0x30/-0x45/-0x4c/-0x50/-0x54 对齐）、def 直用槽消
// stackDef、switch 化 type 分发（二分树 jg/jge/je 全等）、register t（P231）。
// 残余 N 类：①位测 `(1<<t)&0x69c`/`(1<<at)&0x27` 被 4.4 强度归约 shr/and $1
// （ORIG shl/and，N6/N86b，P231 已试）；②`get_rarity()<=1` ORIG `add $0x0;ja`
// 我们 `jg`（返回类型宽度/setcc-fold）；③空槽 `return 0` 独立块 vs ORIG je 直
// 达 epilogue（P47/P59 块布局）；④GUID 实参 ebx 前置（P52 变体）。
#include "LinkClosureD/_verifyItem_1.inc"
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
        // GUID 物品：渲染 guid 串并 trace（校验继续）。ORIG 0x084efd9b..efe0c：
        // guid→%edi、characNo→%esi、accId→%ebx 均在 cMyTrace ctor 之前求值并跨
        // ctor 保活 ⇒ P227 全表达式临时链（具名局部会各自占槽，帧 +0x14）。
        CItemGloballyUniqueIdentifierPrint print;
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x156a, 5)("[HaveAccountLog] Invalid item. (mid:%u, charac:%u, item:%u, guid:%s)",
              user->get_acc_id(), user->get_charac_no(-1),
              (unsigned int)item->m_addInfo,
              print(*(const itemGloballyUniqueIdentifier_t*)(
                  const void*)(reinterpret_cast<const char*>(item) + 0x15)));
    }

    const CItem* def = G_CDataManager()->find_item(item->m_addInfo);
    if (def == 0)
        return 0x1576;
    if (item->m_addInfo != def->get_index())
        return 0x157a;
    if (item->m_field0 != 0 && b1)
        return 0x157d;

    if (def->is_stackable())
    {
        if (b2)
            return 0x1582;
        // ORIG 0x84efec2：t 全程寄存器（无槽，P0）⇒ 内联表达式；位测写
        // `(1u<<t) & 0x69c`（左操作数先物化，复现 ORIG shl 形；0x69c 在右
        // 会触发 4.4 强度归约 shr/and $1，N6/N86b）。t>0xa 检查先于位测。
        register unsigned int t = (unsigned char)item->m_field1;
        if (t > 0xa || ((1u << t) & 0x69c) == 0)
            return 0x158f;
        // ORIG 0x84efef8 直接以 def 槽为 this（无 stackDef 具名局部）
        if (item->m_addInfo2 >
                static_cast<const CStackableItem*>(def)->getStackableLimit() ||
            item->m_addInfo2 < 0)
            return 0x159e;
    }
    else
    {
        // 非 stackable：ORIG 0x84eff2f..ff4f 为二分决策树（cmp $6 jg /
        // cmp $5 jge / cmp $1 je / cmp $8 je）⇒ 原码是 switch（数值升序 case）。
        switch ((int)(unsigned char)item->m_field1)
        {
        case 1:
            break;
        case 5:
        case 6:
            if (type != INVENTORY_TYPE_0 && type != INVENTORY_TYPE_3)
                return 0x15b0;
            break;
        case 8:
            if (type != INVENTORY_TYPE_0)
                return 0x15b4;
            break;
        default:
            return 0x15b7;
        }
    }

    if (def->get_rarity() <= 1 && item->GetReSealCount() != 0)
        return 0x15c2;

    // p167：ORIG 只 call 一次 GetAttachType（结果留 %eax 供 cmp+shl）⇒ 单次求值，
    // 纯 register 免槽（P231）。
    register unsigned int at = (unsigned int)def->GetAttachType();
    if (at <= 5)
    {
        if ((0x27 & (1u << at)) != 0)
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
// [b28d-solo-LinkClosureD] AE | 实参槽序按 ORIG 0x084f00b9..010d 修正为
// characNo,field0,field1,addInfo,GetItemAttr,addInfo2(+0x7),fieldb(+0xb),
// fieldd(+0xd),amp 首 int(+0x11),tag,slot,err；amp 内联表达式消具名槽（P0）后
// 帧 0x7c→0x6c 与 ORIG 全等，全部槽/寄存器分配逐条一致（残余仅 .o 重定位占位，
// compare_tu AE 归一化后 clean）。
#include "LinkClosureD/_writeIncorrectItemInventoryForRecovery_1.inc"
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot)
{
    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x1645,
        "ERROR_ITEM_LOG %u,%d,%d,%u,%d,%d,%d,%u,%d - %s slot(%d), error_line(%d) \n",
        // ORIG 实参槽序（0x28=+0x7, 0x2c=+0xb, 0x30=+0xd, 0x34=+0x11）：
        // 第 6..9 参 = m_addInfo2, m_fieldb, m_fieldd, amp 首 int（P0 内联）
        characNo, (int)item.m_field0, (int)item.m_field1, item.m_addInfo,
        (int)item.GetItemAttr(), item.m_addInfo2, (int)item.m_fieldb,
        item.m_fieldd, *(const int*)(const void*)&item.m_amp, tag, slot, err);
}
