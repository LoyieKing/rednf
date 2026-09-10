// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAvatarItemMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [b28d-solo-CAvatarItemMgr 判词] 本 TU 最终状态（单文件泳道 4 件 + 既有件）：
//  _AssignTempKey 0x082f9e02 NEAR（N95/N82 槽基差：ORIG counter@-0xc vs 4.4 恒 -0x4；13 形探针）
//  UnRegistItem 0x082f9d.. AE（单表达式 find()!=end() 翻求值序 + erase(key)）
//  EraseAvatarItem AE（同上）
//  SaveEndurance 0x082f9f9e AE（迭代器声明序 it 先于 endIt）
//  GetEmblemReduceRate 0x082f9f.. AE（非 const iterator P96/P270 + P222 正极性包裹）
//  UpdateEndurance AE（P222 否定条件块序 + pair 直构 + endIt 具名）
//  RegistItem(iiiPKc…) 0x082f901c AE（早返形 + _FindItem/Acquire 独立变量）
//  RegistNotAssignedAvatarItem(iiPKcb…) 0x082f9xxx AE（SetData 三分支物理内联 P165）
//  AddEmblemEndurance 0x082f9e4c DIFF→N18a 封顶（p167 CLEAR 12/12；本轮 P291 具名数组成员
//   视图消 lea+mov（104 vs 105）、P223 取反 goto 尾标签复现空 then 占位岛；残余 1 条
//   ORIG `mov mem,%eax; mov %eax,%ebx` 中转 = N18a「4.4 恒折单步不可造」+ 装载序 NEAR）
//  Reset 0x082f9ae4 NEAR（p167 CLEAR 25/25；const_iterator 双端转换+去缓存/null 检查；N93 槽向）
//  GetItem 0x082f925c AE（p167 CLEAR 16/16；P13 endIt 具名缓存 + P191 i 双初始化 +
//   P0 去 jewelData 槽内联表达式；槽位 i@-0xc/it@-0x10/end@-0x14 与 ORIG 全等）
// df_game_r Game/ WongWork::CAvatarItemMgr 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_registeredItems/m_waitAssignItems/m_tempKeyCounter/m_emblemEnduranceMap
//          /m_enduranceUpdateMap。
#include "CAvatarItemMgr.h"
#include "AvatarRechargeServer.h"
#include "GameTypes.h"
#include "CDataManager.h"
#include <cstring>
namespace WongWork
{

// [b28d-38] jewel socket 槽视图：步长 6（+0 short pad、+2 int itemId）。
// ORIG AddEmblemEndurance `mov 0x2(%eax,%ecx,1),%eax`（i*6 与 base 各占寄存器、
// +2 折 disp）实证；裸偏移 `*(int*)(jd + i*6 + 2)` 使 4.4 把 +2 并入索引（多 2 条）。
namespace
{
// [sweep-CAvatarItemMgr] P254 档③：ORIG 第二读为两段 `mov 0x2(...),%eax; mov %eax,%ebx`
// （4.1 对 enum 字段保留转换中转），int 字段下 4.4 恒折单段 `mov ...,%ebx` ⇒ 字段 enum 化。
// enum 底层 4 字节、packed 布局不变（sizeof(JewelSocketSlot) 仍 6）。
enum JewelSocketItemId
{
    JEWEL_SOCKET_ITEM_ID_NONE = 0
};

struct JewelSocketSlot
{
    short m_pad;
    JewelSocketItemId m_itemId;
} __attribute__((packed));

// [b28d-solo-CAvatarItemMgr] P291：具名数组成员视图使 4.4 把 base+i*6+2 折叠为
// 单条 `mov 0x2(%eax,%ecx,1),%eax`（指针下标形恒折 lea+mov，多 1 条）。
struct JewelSocketData
{
    JewelSocketSlot m_slots[5];
} __attribute__((packed));
}

// ORIG 静态池 B 0x5c @0x943bd80；静态初始化 0x82fa193 实证 ctor(0x3a98=15000, 0x3e8=1000)。
#include "CAvatarItemMgr/m_AvatarItemPool.inc"

// ORIG 布局（nm/报告实证）：m_registeredItems(+0x00)/m_waitAssignItems(+0x18)
// /m_tempKeyCounter(+0x30)/m_emblemInfo(+0x34)/m_emblemEnduranceMap(+0x54)
// /m_enduranceUpdateMap(+0x6c)，总 0x84（CInventory.h:783 权威注释）。
static_assert(sizeof(CAvatarItemMgr) == 0x84, "CAvatarItemMgr ABI size changed");

// ORIG D1/D2 0x0822d2e0：调 Reset() 后成员 map 自动析构（D1 汇编
// Reset → D1(+0x6c) → D1(+0x54) → D1(+0x18) → D1(+0x00)）。
#include "CAvatarItemMgr/dtor_CAvatarItemMgr.inc"
{
    Reset();
}

#include "CAvatarItemMgr/Reset.inc"
{
    m_tempKeyCounter = 1;
    if (m_registeredItems.empty() && m_waitAssignItems.empty())
        return;

    // [b28d-solo-CAvatarItemMgr] 判词：NEAR（15 差，全为槽号/reloc 显示）＝
    // ORIG 具名迭代器分区 it1@-0x24/endIt2@-0x20，4.4 按首次使用序给
    // it1@-0x20/endIt2@-0x24（互换）；4.4 恒先求 end 再 begin，声明序/块作用域
    // 穷举不可翻 ⇒ N93/N82附 槽向封顶。p167 CLEAR 25/25。
    // [b28d-38] P167：ORIG 用 _Rb_tree_const_iterator（转换构造×4/op->×3/op++×2/
    // op!=×2）⇒ 两侧均先转 const_iterator（混合比较会生成 operator!=<PK,P> 变体）。
    // [M3FreeNext15B.M3S6AucAvBattle] 槽阶梯对齐：ORIG 分配序 = endIt1(-0x1c) →
    // it1(-0x24) → endIt2(-0x20) → it2(-0x28)（082f9ae4 逐 lea 实录）。4.4 按
    // 构造序浅→深单调分配，故把**循环迭代器声明进内层块**（it 内层/it2 内层、
    // endIt/endIt2 外层）⇒ 4.4 块内槽重开使 it1/it2 落 -0x24/-0x28，四点全中
    // （探针 /tmp/p_iter2/q1.cpp 77=77 与 TU 实测一致）。
    std::map<int, Avatar_Item*>::const_iterator endIt = m_registeredItems.end();
    {
        std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.begin();
        for (; it != endIt; ++it)
        {
            // ORIG 无 null 检查、无 item 具名局部（每点重发 operator->）。
            it->second->init();
            m_AvatarItemPool.Free(it->second);
        }
    }
    std::map<int, Avatar_Item*>::const_iterator endIt2 = m_waitAssignItems.end();
    {
        std::map<int, Avatar_Item*>::const_iterator it2 = m_waitAssignItems.begin();
        for (; it2 != endIt2; ++it2)
        {
            m_AvatarItemPool.Free(it2->second);
        }
    }
    m_registeredItems.clear();
    m_waitAssignItems.clear();
    m_emblemInfo.init();
    m_emblemEnduranceMap.clear();
    m_enduranceUpdateMap.clear();
}

#include "CAvatarItemMgr/IsTempKey.inc"

#include "CAvatarItemMgr/_MakeTempKey.inc"

#include "CAvatarItemMgr/_AssignTempKey.inc"
{
    // [b28d-solo-CAvatarItemMgr] 判词：NEAR（2 差）＝纯槽号差 ORIG counter@-0xc vs
    // 4.4 -0x4（帧 0x28 已对齐）；本轮再测 double framePad×1/×2、counter 先声明、
    // 8/0x10/0x18 数组维共 6 形，4.4 单标量恒落 -0x4 ⇒ N95/N82 槽基策略封顶。
    // ORIG 帧 0x28：P31 dummy local 撑帧（4.4 对 ≥16 字节数组保槽，标量死局部
    // 会被消除；探针 /tmp/slotprobe5.cpp 帧对齐实证）。残余 counter 槽 -0x4 vs
    // ORIG -0xc 为 4.1 单标量首槽策略差（探针 slotprobe6/7/8x/90/91 全败），
    // N82 族封顶不追。
    // [b28 复验] P190 double 撑槽与 P31 dead int 占槽两形均不把 counter 顶至
    // ORIG -0xc（4.4 标量首槽恒 -0x4，声明序/double 对齐/占位写全败）——N82 封顶。
    // [M3FreeNext15B.M3S6AucAvBattle 2026-09-11] 本波按 P339 全头部声明复验：
    // 4.4 函数头声明恒「先声明深」⇒ 全头部「8B 撑槽(深) → counter(中) → 8B 撑槽(浅)」
    // 可把 counter 顶到 -0xc 且帧仍 0x28（两个 double 取址保槽，i386 下 double 仅 4B 对齐）。
    // 实测：counter 仍恒落 -0x4 且帧仍 0x28（*两个 double 确占 16B*，但 4.4 标量/聚合
    // 分区为「标量恒自浅端、聚合自深端」⇒ 声明序对标量无杠杆）⇒ 该形亦败，见下。
    // 其余新钥匙排除：P331 无「常数额整体平移」（差仅 counter 一槽）；P326 不适用
    // （无迭代器对/无双端转换）；§6.6.2 不适用（本件无 landing pad，ORIG 亦无）。
    // 结论：N95/N82 槽基策略封顶仍成立（本波新增 2 形读数全记录，无一命中）。
    char framePad[0x10];
    (void)framePad;
    if (m_tempKeyCounter == 0x7fffffff)
        m_tempKeyCounter = 0;
    m_tempKeyCounter++;
    int counter = m_tempKeyCounter;   // ORIG -0xc 槽往返（读→存→读）
    // ORIG 0x082f9e02：成员读后经 -0xc 槽往返再传 _MakeTempKey（P0 具名局部），
    // 帧 0x28；4.4 折叠该 read→temp→read 往返且帧缩至 0x18/槽 -0x4 ⇒ N76-N82 梳。
    return _MakeTempKey(counter);
}

#include "CAvatarItemMgr/_FindItem.inc"
{
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}

#include "CAvatarItemMgr/_FindWaitAssignItem.inc"
{
    std::map<int, Avatar_Item*>::const_iterator it = m_waitAssignItems.find(key);
    if (it == m_waitAssignItems.end())
        return 0;
    return it->second;
}

#include "CAvatarItemMgr/_FindItemAll.inc"
{
    // ORIG：_FindItemAll 调 IsTempKey（W 0x0822d20d）判定符号位；
    // IsTempKey 为 static（mangled 不变），const 方法可直接调用。
    if (IsTempKey(key))
        return _FindWaitAssignItem(key);
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}

#include "CAvatarItemMgr/AddAvatarItem.inc"
{
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
}

// [sweep-CAvatarItemMgr] 判词：DIFF(105/104) → IDENTICAL_AE(105/105)。
// 修法①P254 档③：字段 int→enum（JewelSocketItemId）复现 ORIG 实参两段中转
//   `mov 0x2(...),%eax; mov %eax,%ebx`（int 字段下 4.4 恒折单段，旧判 N18a 作废）；
// 修法②P235：cast 移出表达式、局部改完整类型 JewelSocketData* ⇒ 复现 ORIG
//   「i 先、base 后」装载序（cast 在表达式内时 4.4 恒 base 先载）。
// p167 12/12 CLEAR（ORIG C2 与我方 C1 同址 0x082faa8e，P93 豁免）。报告
// docs/class_func_reports/WongWork__CAvatarItemMgr/AddEmblemEndurance.md 两段已读。
#include "CAvatarItemMgr/AddEmblemEndurance.inc"
{
    // [sweep-CAvatarItemMgr] P235：cast 留在表达式内时 4.4 恒 base 先载
    // （`mov -0x14(%ebp),%ecx` 在 `mov -0x10(%ebp),%edx` 之前），ORIG 是
    // 完整类型成员直访问 ⇒ i 先、base 后。把 cast 移到赋值点，局部即完整类型。
    const JewelSocketData* jewelData =
        (const JewelSocketData*)item->getJewelSocketData();
    short totalEndurance = 0;
    for (int i = 0; i <= 4; i++)
    {
        // [b28d-38] P0/P48：ORIG 对同一表达式读两次（test 与 find_item 实参各一次），
        // 无 itemId 具名槽；grade 无具名槽（P52 链式实参跨 receiver call 保活 ebx）。
        // [b28d-solo-CAvatarItemMgr] P223：ORIG 空 then 占位岛
        // （`je <nop>; body; jmp <incr>; nop; incr:`）⇒ 取反 goto 尾空标签。
        if (jewelData->m_slots[i].m_itemId == 0)
            goto next_slot;
        {
            // [sweep-CAvatarItemMgr] P254：enum 字段使 4.4 保留 `mov mem,%eax;
            // mov %eax,%ebx` 两段中转（int 字段恒折单段）。p167 12/12 CLEAR。
            CItem* citem = G_CDataManager()->find_item(
                jewelData->m_slots[i].m_itemId);
            if (citem == 0)
                return;
            totalEndurance += (short)ARAD::Singleton<AvatarRechargeServer>::Get()
                                  ->GetRateFromEmblemGrade(citem->get_grade());
        }
    next_slot:;
    }
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    // [b28d-38] P167：ORIG 调 operator==（非 !=）、insert 实参构造成员模板为
    // pair<int&,short&>（totalEndurance 为 short 左值、pair 直构非 make_pair）。
    if (it == m_emblemEnduranceMap.end())
    {
        m_emblemEnduranceMap.insert(std::pair<const int, int>(key, totalEndurance));
    }
    else
    {
        it->second = (int)totalEndurance;
    }
}

#include "CAvatarItemMgr/AssignAvatarItemInfo.inc"
                                          int itemIdx, const char* ipgAgency)
{
    Avatar_Item* item = _FindWaitAssignItem(key);
    if (item == 0)
        return false;
    item->SetData(expireDate, itemIdx, ipgAgency);
    m_waitAssignItems.erase(key);
    m_registeredItems.insert(std::make_pair(newKey, item));
    AddEmblemEndurance(newKey, item);
    return true;
}

#include "CAvatarItemMgr/AssignAvatarItemKey.inc"
{
    Avatar_Item* item = _FindWaitAssignItem(oldKey);
    if (item == 0)
        return false;
    m_waitAssignItems.erase(oldKey);
    m_registeredItems.insert(std::make_pair(newKey, item));
    AddEmblemEndurance(newKey, item);
    return true;
}

#include "CAvatarItemMgr/RegistNotAssignedAvatarItem.inc"
                                                          const char* ipgAgency, bool b,
                                                          const stAvatarEmblemInfo_t& emblem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    // [b28d-38] ORIG 0x82f9xxx：SetData 三分支各物理内联一份（P165 多份独立块），
    // 无 expireDate 具名局部（各分支参数直接算）。
    if (itemIdx == 0 || itemIdx == 0x16d)
    {
        item->SetData(0, tick, ipgAgency);
    }
    else if (!b)
    {
        item->SetData(itemIdx * 0x15180 + tick, tick, ipgAgency);
    }
    else
    {
        item->SetData(tick + itemIdx, tick, ipgAgency);
    }
    item->setEmblemSocketData(emblem);
    item->setColor(0, 0);   // ORIG 真名 setColor(short,short)
    // ORIG 0x82f95d3: lea 0x18(%eax),%ecx → 插入 m_waitAssignItems(+0x18)
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

// ORIG T 0x082f95f4 _ZN8WongWork14CAvatarItemMgr27RegistNotAssignedAvatarItemE10Inven_Item
// 按值版：与引用重载同语义（SetData(item.m_addInfo2,0,"")、setColorEss(0,0)、
// 插入 m_waitAssignItems(+0x18)，返回 tempKey）。
#include "CAvatarItemMgr/RegistNotAssignedAvatarItem_1.inc"
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    item->SetData(invenItem.m_addInfo2, 0, "");
    item->setColor(0, 0);
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

#include "CAvatarItemMgr/RegistItem.inc"
                               const stAvatarEmblemInfo_t& emblem,
                               const stAvatarExpansionInfo_t& expansion, bool b)
{
    Avatar_Item* found = _FindItem(key);
    // [b28d-38] ORIG -0x10=_FindItem 结果、-0x2c=Acquire 结果（两个独立局部）；
    // 早返形（`cmpl $0,-0x10; je <主流程>` 由 4.1 早返布局产生）。
    if (found != 0)
        return 0;
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    int expireDate2 = 0;
    if (b)
    {
        if (expireDate != 0 && expireDate != 0x16d)
            expireDate2 = expireDate * 0x15180 + registTime;
    }
    else
    {
        expireDate2 = expireDate;
    }
    item->SetData(expireDate2, registTime, ipgAgency);
    item->setEmblemSocketData(emblem);
    item->setExpansionInfo(expansion);
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
    return 1;
}

#include "CAvatarItemMgr/RegistItem_1.inc"
{
    RegistItem(info.m_key, info.m_expireDate, info.m_registTime, info.m_ipgAgencyNo,
               *(const stAvatarEmblemInfo_t*)&info.m_jewelSocket,
               *(const stAvatarExpansionInfo_t*)&info.m_expansionInfo, b);
}

#include "CAvatarItemMgr/GetExpireDate.inc"
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetExpireDate();
}

#include "CAvatarItemMgr/GetRegistTime.inc"
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetRegistTime();
}

#include "CAvatarItemMgr/GetIPGAgencyNo.inc"
{
    Avatar_Item* item = _FindItemAll(addInfo);
    if (item == 0)
        return 0;
    return item->GetIPGAgencyNo();
}

#include "CAvatarItemMgr/GetRemainDate.inc"
{
    Avatar_Item* item = _FindItemAll(nAvatarUid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nTick);
}

#include "CAvatarItemMgr/GetEmblemReduceRate.inc"
{
    // [b28d-38] P96/P270：const_iterator 引入转换构造 call（+10 条）⇒ 非 const iterator。
    // P222：ORIG 尾部 ret0 块（`mov $0x0,%eax; leave`）⇒ 正极性包裹（!=end fall-through）。
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    if (it != m_emblemEnduranceMap.end())
        return it->second;
    return 0;
}

#include "CAvatarItemMgr/getJewelSocketData.inc"
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return (const char*)&m_emblemInfo;
    return item->getJewelSocketData();
}

#include "CAvatarItemMgr/GetExpansionInfo.inc"
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return 0;
    return item->getExpansionInfo();
}

#include "CAvatarItemMgr/SetAvatarColor.inc"
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return false;
    item->setColor(color1, color2);
    return true;
}

#include "CAvatarItemMgr/GetAvatarInfo.inc"
{
    return _FindItemAll(key);
}

// [b28d-solo-CAvatarItemMgr] 判词：DIFF→IDENTICAL_AE（127=127，TU clean 口径）。
// P13 endIt 具名缓存（ORIG end 循环外单次求值）+ P191 i 双初始化（两条 movl $0）+
// P0 去 jewelData 具名槽内联表达式；槽位 i@-0xc/it@-0x10/end@-0x14 与 ORIG 全等。
// p167 16/16 CLEAR。
#include "CAvatarItemMgr/GetItem.inc"
{
    // [b28d-solo-CAvatarItemMgr] P13/P191：end 迭代器具名缓存（ORIG end() 在循环外
    // 单次求值），i 双初始化（`int i=0;` + `for(i=0;…)` 两条 movl $0 都要写）。
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.begin();
    std::map<int, Avatar_Item*>::const_iterator endIt = m_registeredItems.end();
    int i = 0;
    for (i = 0; it != endIt && i <= 0x72; ++it, ++i)
    {
        // [b28d-38] P157/P167：ORIG 4 次 operator->（每点重发 it->second），无 item 缓存。
        info[i].m_key = it->first;
        info[i].m_expireDate = it->second->GetExpireDate();
        info[i].m_registTime = it->second->GetRegistTime();
        strncpy(info[i].m_ipgAgencyNo, it->second->GetIPGAgencyNo(), 0x19);
        // [b28d-solo-CAvatarItemMgr] P0/P167：ORIG 无 jewelData 具名槽（值寄存器直传）
        // ⇒ 内联表达式；结构体直接赋值（逐字段展开，无 memcpy call）。
        info[i].m_jewelSocket =
            *(const stAvatarEmblemInfo_t*)it->second->getJewelSocketData();
        info[i].m_expansionInfo =
            *(const stAvatarExpansionInfo_t*)it->second->getExnansionInfoRef();
    }
    *count = i;
    return 1;
}

#include "CAvatarItemMgr/UnRegistItem.inc"
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return false;
    m_registeredItems.erase(key);
    m_AvatarItemPool.Free(item);
    // [b28d-38] 单表达式形：ORIG 先求 end（-0x14）后求 find（-0x10），
    // 且 erase 传 key 地址（0xc(%ebp)）非迭代器副本；具名 it 形 4.4 恒 find 先。
    if (m_emblemEnduranceMap.find(key) != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(key);
    return true;
}

#include "CAvatarItemMgr/EraseAvatarItem.inc"
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    m_registeredItems.erase(key);
    if (m_emblemEnduranceMap.find(key) != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(key);
    return item;
}

#include "CAvatarItemMgr/isSocketAvatar.inc"
{
    const char* jewelData = getJewelSocketData(key);
    if (*(short*)jewelData == 0)
        return false;
    return true;
}

#include "CAvatarItemMgr/setEmblemSocketData.inc"
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    item->setEmblemSocketData(emblem);
    AddEmblemEndurance(key, item);
    return 1;
}

#include "CAvatarItemMgr/UpdateEndurance.inc"
{
    std::map<int, int>::iterator it = m_enduranceUpdateMap.find(key);
    std::map<int, int>::iterator endIt = m_enduranceUpdateMap.end();
    // [b28d-38] ORIG then 块（it->second=endurance）在函数尾、insert 块直落 ⇒
    // 否定条件写法（P222 镜像）；pair 直构替代 make_pair（省一次转换构造 call，P192）。
    if (it == endIt)
        m_enduranceUpdateMap.insert(std::pair<const int, int>(key, endurance));
    else
        it->second = endurance;
}

#include "CAvatarItemMgr/SaveEndurance.inc"
{
    if (m_enduranceUpdateMap.empty())
        return;
    // [b28d-38] ORIG begin@-0xc/end@-0x10；4.4 对两迭代器 sret 恒 begin 结果深、
    // end 结果浅（与声明序无关，N93/N82附 聚合对象槽分区）⇒ 槽号互换 NEAR 封顶。
    std::map<int, int>::iterator it = m_enduranceUpdateMap.begin();
    std::map<int, int>::iterator endIt = m_enduranceUpdateMap.end();
    for (; it != endIt; ++it)
    {
        AvatarRechargeServer::SaveAvatarEnduranceUnit(it->first, it->second);
    }
    m_enduranceUpdateMap.clear();
}

#include "CAvatarItemMgr/updateEmblemSocket.inc"
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    for (unsigned int i = 0; i < count; i++)
    {
        item->updateEmblemSocket((unsigned int)grades[i], (unsigned int)itemIds[i]);
    }
    AddEmblemEndurance(key, item);
    return 1;
}

}  // namespace WongWork
