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
struct JewelSocketSlot
{
    short m_pad;
    int m_itemId;
} __attribute__((packed));

// [b28d-solo-CAvatarItemMgr] P291：具名数组成员视图使 4.4 把 base+i*6+2 折叠为
// 单条 `mov 0x2(%eax,%ecx,1),%eax`（指针下标形恒折 lea+mov，多 1 条）。
struct JewelSocketData
{
    JewelSocketSlot m_slots[5];
} __attribute__((packed));
}

// ORIG 静态池 B 0x5c @0x943bd80；静态初始化 0x82fa193 实证 ctor(0x3a98=15000, 0x3e8=1000)。
DynamicPool<Avatar_Item> CAvatarItemMgr::m_AvatarItemPool(15000, 1000);

// ORIG 布局（nm/报告实证）：m_registeredItems(+0x00)/m_waitAssignItems(+0x18)
// /m_tempKeyCounter(+0x30)/m_emblemInfo(+0x34)/m_emblemEnduranceMap(+0x54)
// /m_enduranceUpdateMap(+0x6c)，总 0x84（CInventory.h:783 权威注释）。
static_assert(sizeof(CAvatarItemMgr) == 0x84, "CAvatarItemMgr ABI size changed");

// ORIG D1/D2 0x0822d2e0：调 Reset() 后成员 map 自动析构（D1 汇编
// Reset → D1(+0x6c) → D1(+0x54) → D1(+0x18) → D1(+0x00)）。
CAvatarItemMgr::~CAvatarItemMgr()
{
    Reset();
}

void CAvatarItemMgr::Reset()
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
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.begin();
    std::map<int, Avatar_Item*>::const_iterator endIt = m_registeredItems.end();
    for (; it != endIt; ++it)
    {
        // ORIG 无 null 检查、无 item 具名局部（每点重发 operator->）。
        it->second->init();
        m_AvatarItemPool.Free(it->second);
    }
    std::map<int, Avatar_Item*>::const_iterator it2 = m_waitAssignItems.begin();
    std::map<int, Avatar_Item*>::const_iterator endIt2 = m_waitAssignItems.end();
    for (; it2 != endIt2; ++it2)
    {
        m_AvatarItemPool.Free(it2->second);
    }
    m_registeredItems.clear();
    m_waitAssignItems.clear();
    m_emblemInfo.init();
    m_emblemEnduranceMap.clear();
    m_enduranceUpdateMap.clear();
}

bool CAvatarItemMgr::IsTempKey(int key) { return (key >> 31) != 0; }   // ORIG 非 const

int CAvatarItemMgr::_MakeTempKey(int key) const { return key | 0x80000000; }

int CAvatarItemMgr::_AssignTempKey()
{
    // [b28d-solo-CAvatarItemMgr] 判词：NEAR（2 差）＝纯槽号差 ORIG counter@-0xc vs
    // 4.4 -0x4（帧 0x28 已对齐）；本轮再测 double framePad×1/×2、counter 先声明、
    // 8/0x10/0x18 数组维共 6 形，4.4 单标量恒落 -0x4 ⇒ N95/N82 槽基策略封顶。
    // ORIG 帧 0x28：P31 dummy local 撑帧（4.4 对 ≥16 字节数组保槽，标量死局部
    // 会被消除；探针 /tmp/slotprobe5.cpp 帧对齐实证）。残余 counter 槽 -0x4 vs
    // ORIG -0xc 为 4.1 单标量首槽策略差（探针 slotprobe6/7/8x/90/91 全败），
    // N82 族封顶不追。
    char framePad[0x10];
    (void)framePad;
    // [b28 复验] P190 double 撑槽与 P31 dead int 占槽两形均不把 counter 顶至
    // ORIG -0xc（4.4 标量首槽恒 -0x4，声明序/double 对齐/占位写全败）——N82 封顶。
    if (m_tempKeyCounter == 0x7fffffff)
        m_tempKeyCounter = 0;
    m_tempKeyCounter++;
    int counter = m_tempKeyCounter;   // ORIG -0xc 槽往返（读→存→读）
    // ORIG 0x082f9e02：成员读后经 -0xc 槽往返再传 _MakeTempKey（P0 具名局部），
    // 帧 0x28；4.4 折叠该 read→temp→read 往返且帧缩至 0x18/槽 -0x4 ⇒ N76-N82 梳。
    return _MakeTempKey(counter);
}

Avatar_Item* CAvatarItemMgr::_FindItem(int key) const
{
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}

Avatar_Item* CAvatarItemMgr::_FindWaitAssignItem(int key) const
{
    std::map<int, Avatar_Item*>::const_iterator it = m_waitAssignItems.find(key);
    if (it == m_waitAssignItems.end())
        return 0;
    return it->second;
}

Avatar_Item* CAvatarItemMgr::_FindItemAll(int key) const
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

void CAvatarItemMgr::AddAvatarItem(int key, Avatar_Item* item)
{
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
}

// [b28d-solo-CAvatarItemMgr] 判词：DIFF→N18a 封顶（105 vs 104）。本轮 P291 具名数组成员
// 视图（JewelSocketData::m_slots[i].m_itemId）消 lea+mov、P223 取反 goto 尾空标签复现
// 空 then 占位岛（jmp+nop）；残余 1 条 ORIG 实参两段中转 `mov mem,%eax; mov %eax,%ebx`
// = N18a（4.4 恒折单步入 %ebx，P226 语句表达式实测无效）。p167 12/12 CLEAR。
void CAvatarItemMgr::AddEmblemEndurance(int key, const Avatar_Item* item)
{
    const char* jewelData = item->getJewelSocketData();
    short totalEndurance = 0;
    for (int i = 0; i <= 4; i++)
    {
        // [b28d-38] P0/P48：ORIG 对同一表达式读两次（test 与 find_item 实参各一次），
        // 无 itemId 具名槽；grade 无具名槽（P52 链式实参跨 receiver call 保活 ebx）。
        // [b28d-solo-CAvatarItemMgr] P223：ORIG 空 then 占位岛
        // （`je <nop>; body; jmp <incr>; nop; incr:`）⇒ 取反 goto 尾空标签。
        if (((const JewelSocketData*)jewelData)->m_slots[i].m_itemId == 0)
            goto next_slot;
        {
            // [b28d-solo-CAvatarItemMgr] 判词：N18a 封顶——ORIG 实参两段中转
            // `mov 0x2(%eax,%ecx,1),%eax; mov %eax,%ebx`，4.4 恒折单步入 %ebx
            // （P226 语句表达式实测无效）；残余装载序 ecx/edx 互换属 P30 伪影。
            // p167 CLEAR 12/12。
            CItem* citem = G_CDataManager()->find_item(
                ((const JewelSocketData*)jewelData)->m_slots[i].m_itemId);
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

bool CAvatarItemMgr::AssignAvatarItemInfo(int key, int newKey, int expireDate,
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

bool CAvatarItemMgr::AssignAvatarItemKey(int oldKey, int newKey)
{
    Avatar_Item* item = _FindWaitAssignItem(oldKey);
    if (item == 0)
        return false;
    m_waitAssignItems.erase(oldKey);
    m_registeredItems.insert(std::make_pair(newKey, item));
    AddEmblemEndurance(newKey, item);
    return true;
}

unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(int itemIdx, int tick,
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

unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(const Inven_Item& invenItem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    item->SetData(invenItem.m_addInfo2, 0, "");
    item->setColor(0, 0);
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

// ORIG T 0x082f95f4 _ZN8WongWork14CAvatarItemMgr27RegistNotAssignedAvatarItemE10Inven_Item
// 按值版：与引用重载同语义（SetData(item.m_addInfo2,0,"")、setColorEss(0,0)、
// 插入 m_waitAssignItems(+0x18)，返回 tempKey）。
unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(Inven_Item invenItem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    item->SetData(invenItem.m_addInfo2, 0, "");
    item->setColor(0, 0);
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
}

int CAvatarItemMgr::RegistItem(int key, int expireDate, int registTime, const char* ipgAgency,
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

void CAvatarItemMgr::RegistItem(const AvatarDetailInfo& info, bool b)
{
    RegistItem(info.m_key, info.m_expireDate, info.m_registTime, info.m_ipgAgencyNo,
               *(const stAvatarEmblemInfo_t*)&info.m_jewelSocket,
               *(const stAvatarExpansionInfo_t*)&info.m_expansionInfo, b);
}

int CAvatarItemMgr::GetExpireDate(int avatarUid) const
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetExpireDate();
}

int CAvatarItemMgr::GetRegistTime(int avatarUid) const
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetRegistTime();
}

const char* CAvatarItemMgr::GetIPGAgencyNo(int addInfo) const
{
    Avatar_Item* item = _FindItemAll(addInfo);
    if (item == 0)
        return 0;
    return item->GetIPGAgencyNo();
}

int CAvatarItemMgr::GetRemainDate(int nAvatarUid, int nTick) const
{
    Avatar_Item* item = _FindItemAll(nAvatarUid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nTick);
}

int CAvatarItemMgr::GetEmblemReduceRate(int key)   // ORIG 非 const
{
    // [b28d-38] P96/P270：const_iterator 引入转换构造 call（+10 条）⇒ 非 const iterator。
    // P222：ORIG 尾部 ret0 块（`mov $0x0,%eax; leave`）⇒ 正极性包裹（!=end fall-through）。
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    if (it != m_emblemEnduranceMap.end())
        return it->second;
    return 0;
}

const char* CAvatarItemMgr::getJewelSocketData(int key) const
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return (const char*)&m_emblemInfo;
    return item->getJewelSocketData();
}

void* CAvatarItemMgr::GetExpansionInfo(int key) const
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return 0;
    return item->getExpansionInfo();
}

bool CAvatarItemMgr::SetAvatarColor(int key, short color1, short color2)
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return false;
    item->setColor(color1, color2);
    return true;
}

Avatar_Item* CAvatarItemMgr::GetAvatarInfo(int key) const
{
    return _FindItemAll(key);
}

// [b28d-solo-CAvatarItemMgr] 判词：DIFF→IDENTICAL_AE（127=127，TU clean 口径）。
// P13 endIt 具名缓存（ORIG end 循环外单次求值）+ P191 i 双初始化（两条 movl $0）+
// P0 去 jewelData 具名槽内联表达式；槽位 i@-0xc/it@-0x10/end@-0x14 与 ORIG 全等。
// p167 16/16 CLEAR。
bool CAvatarItemMgr::GetItem(AvatarDetailInfo* info, int* count) const
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

bool CAvatarItemMgr::UnRegistItem(int key)
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

Avatar_Item* CAvatarItemMgr::EraseAvatarItem(int key)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    m_registeredItems.erase(key);
    if (m_emblemEnduranceMap.find(key) != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(key);
    return item;
}

bool CAvatarItemMgr::isSocketAvatar(int key)   // ORIG 非 const
{
    const char* jewelData = getJewelSocketData(key);
    if (*(short*)jewelData == 0)
        return false;
    return true;
}

int CAvatarItemMgr::setEmblemSocketData(int key, const stAvatarEmblemInfo_t& emblem)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    item->setEmblemSocketData(emblem);
    AddEmblemEndurance(key, item);
    return 1;
}

void CAvatarItemMgr::UpdateEndurance(int key, int endurance)
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

void CAvatarItemMgr::SaveEndurance()
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

int CAvatarItemMgr::updateEmblemSocket(int key, unsigned int count, char* grades, unsigned long* itemIds)
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
