// df_game_r 还原 —— WarField（G2-4 战场域，PvP WarRoom 内嵌）。
// 逐函数对照 docs/class_func_reports/WarField.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径：分支/调用目标→<T>、
// 大绝对地址→<A>、去 @plt、剥尾部 nop、C2/D2 同址别名良性）。
// map_item/map_monster/MapInfo 方法（ctor/dtor/Add_Mob/Add_Item 等）定义于
// 本批次 map_item.cpp；跨类方法（CUser/CDataManager/CDungeon/WongWork 掉落链）
// 以 asm-label extern 直引 ORIG mangled 符号，避免与并行批次头文件冲突。
#include <list>
#include <map>
#include <utility>
#include <vector>
#include <cstdlib>
#include <time.h>

#include "WarField.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "LogManager.h"
#include "CUser.h"            // 权威 CUser / CUserCharacInfo / 共享枚举
#include "CInventory.h"       // stAvatarEmblemInfo_t / stAvatarExpansionInfo_t（含 init）
#include "CEquipItem.h"
#include "CDungeon.h"
#include "CMap.h"             // 权威 CMap / STRivalMapData（m_rivalData1=+0x90）
#include "CMonster.h"         // 权威 CMonster（m_weight=+0x1d24，ConsistMap 086b8d97 movl 直读）
#include "GlobalData.h"       // 权威 GlobalData::s_systemTime_
#include "Arad_DataManager.h" // 权威 ARAD::Singleton<T> 模板（本文件不再重复定义）
#include "ServiceRestrictManager.h" // 权威 ServiceRestrictManager（含 map 成员，Meyers 单例非平凡
                                    // → Get() 发 guard；此前局部平凡类致 Get 退化为 5 指令 stub）

int get_rand_int(int range);  // ORIG T 0x86b1b87

// ============================================================================
// 本地支撑类型（名称/布局与 ORIG 一致；成员在各自批次 TU 定义）
// ============================================================================

// 掉落生成域类型（WongWork::stGenerateRefData_t/stGenerateResult_t/CItemGenerator）
// 对战地图数据 STRivalMapData 用 CMap.h 权威定义（本文件曾用局部重复定义已删，§4.4）。
// 权威声明见 CItemGenerator.h（唯一声明点，经 CDataManager.h 引入）。
// ORIG vtable 槽位（_ZTVN8WongWork14CItemGeneratorE @0x08c9a788）：
//   +0x08 generateRandomItem、+0x0c generateSpecificItem。
using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;
using WongWork::CItemGenerator;

// ============================================================================
// Reset（086b8a14：MapInfo::Clear + stageIdx=0）
// ============================================================================

void WarField::Reset()
{
    m_mapInfo.Clear();
    m_stageIdx = 0;
}

// ============================================================================
// 构造/析构（ORIG 086c0b8e / 086c0ba4：仅 MapInfo 子对象）
// ============================================================================

WarField::WarField()
{
}

WarField::~WarField()
{
}

// ============================================================================
// IsGridClear（086b954c：怪物表为空）
// ============================================================================

bool WarField::IsGridClear()
{
    if (m_mapInfo.m_monsterMap.size() != 0)
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// PickupItem（086ba082：erase）
// ============================================================================

void WarField::PickupItem(std::map<int, map_item>::iterator it)
{
    m_mapInfo.m_itemMap.erase(it);
}

// ============================================================================
// CheckPickupItem（086ba0a0）
// ============================================================================

int WarField::CheckPickupItem(int idx, int owner,
                              std::map<int, map_item>::iterator& out)
{
    std::map<int, map_item>::iterator it;
    it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return 0x15;
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (curSec - it->second.m_createTick < 10)
    {
        if (!(it->second.m_dropIndex == (unsigned short)-1 ||
              it->second.m_dropIndex == owner))
        {
            return 0x1c;
        }
    }
    out = it;
    return 0;
}

// ============================================================================
// DropItem（086ba190：计数器+1，写回 item 索引，加入地图）
// ============================================================================

int WarField::DropItem(map_item item)
{
    int old = m_mapInfo.m_dropItemCnt++;   // ORIG lea 0x1(%eax) 单链自增
    item.m_itemIndex = old;
    m_mapInfo.Add_Item(item);
    return old;
}

// ============================================================================
// GetFieldItem（086b9fe6）
// ============================================================================

bool WarField::GetFieldItem(int idx, map_item& out)
{
    std::map<int, map_item>::iterator it;
    it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return 0;
    }
    out = it->second;
    return 1;
}

// ============================================================================
// GetFieldMob（086b9578）
// ============================================================================

// [b28d-solo-WarField] NEAR 封顶（68=68、语义完整）：mob 作用域外提（块尾析构）已把返回
// 值汇聚 %ebx→%eax、并复现 ORIG「D1 后 mov $1,%eax」。残差全为槽序：ORIG it/end=-0x35c/-0x358
// （深槽）、mob=-0x354；4.4 聚合优先占低槽 ⇒ mob=-0x35c、it/end=-0x10/-0xc。属 N82附
// （4.4 聚合优先占低槽/标量分区 vs 4.1 生命周期穿插），无合法源形可择。@0x086b9578
bool WarField::GetFieldMob(int idx, map_monster& out)
{
    std::map<int, map_monster>::iterator it;
    it = m_mapInfo.m_monsterMap.find(idx);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    {
        map_monster mob(it->second);
        out = mob;
    }
    return 1;
}

// ============================================================================
// MakePacket（086b92e2：0x1d 战场怪物信息包）
// ============================================================================

// [b28d-solo-WarField] AE（158=158）：P286 单 case switch 复现「形参载寄存器比较」
// （if 形 4.4 恒折 cmpl mem）+ P68 cast-this 形（void* buf + ((InterfacePacketBuf*)buf)->）
// 复现 4.1 每个成员调用「先载 this 再压常量」的实参装载序。@0x086b92e2
void WarField::MakePacket(int packetId, void* packet)
{
    void* buf = packet;
    switch (packetId)
    {
    case 0x1d:
        ((InterfacePacketBuf*)buf)->put_header(0, 0x1d);
        ((InterfacePacketBuf*)buf)->put_int(rand());
        ((InterfacePacketBuf*)buf)->put_byte(0);
        ((InterfacePacketBuf*)buf)->put_byte(0);
        ((InterfacePacketBuf*)buf)->put_byte(1);
        ((InterfacePacketBuf*)buf)->put_short(m_mapInfo.m_mapIndex);
        ((InterfacePacketBuf*)buf)->put_byte((int)m_mapInfo.m_monsterMap.size());
        map_monster mob;
        for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
             it != m_mapInfo.m_monsterMap.end(); it++)
        {
            mob = it->second;
            ((InterfacePacketBuf*)buf)->put_short(0);
            ((InterfacePacketBuf*)buf)->put_int(mob.m_mobId);
            ((InterfacePacketBuf*)buf)->put_short((short)mob.m_instId);
            ((InterfacePacketBuf*)buf)->put_int(mob.m_mobIndex);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_level);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_roleType);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_field13);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_field14);
        }
        ((InterfacePacketBuf*)buf)->put_byte(0);
        ((InterfacePacketBuf*)buf)->finalize(true);
        break;
    }
}

// ============================================================================
// HandleWpPerMonster（086ba1ea：战争点数累计）
// ============================================================================

int WarField::HandleWpPerMonster(CUser* user, int monsterIdx, int wp,
                                 map_monster& out)
{
    MapInfo* mi = &m_mapInfo;
    std::map<int, map_monster>::iterator it = mi->m_monsterMap.find(monsterIdx);
    if (it == mi->m_monsterMap.end())
    {
        return 0;
    }
    map_monster* mob = &it->second;
    std::map<int, stWarPoint, std::greater<int> >* warMap =
        &G_CDataManager()->m_warAreaCounter.m_wpPerMonsterHp;  // +0x87b4+0x120=0x88d4
                                                              // （ORIG 086ba266 add $0x88d4 实证）
    std::map<int, stWarPoint, std::greater<int> >::iterator it2 = warMap->find(wp);
    std::map<int, stWarPoint, std::greater<int> >::iterator it3 =
        warMap->find(mob->m_dropRate);
    if (it2 == warMap->end())
    {
        return 0;
    }
    ++it2;
    if (it3 == warMap->end())
    {
        return 0;
    }
    ++it3;
    if (!(wp < mob->m_dropRate))
    {
        return 0;
    }
    int total = 0;
    WpBonusPointResult bonus;
    user->checkBonusPoint();
    user->getWpBonusPoint(bonus);
    while (it3 != it2 && it3 != warMap->end())
    {
        total += it3->second.m_pts[mob->m_roleType];
        ++it3;
    }
    total += bonus.m_base;
    total += (int)((float)total * ((float)bonus.m_add / 100.0f) + 0.5f);
    mob->m_dropRate = wp;
    out = *mob;
    return total;
}

// ============================================================================
// ConsistMap（086b8a38：战场怪物刷出/发包）
// ============================================================================

// [b28d-solo-WarField] 语义完成、byte 层 N 封顶（539=539，p167 CLEAR 55/55）：
// ①p167 揪出真 bug——ORIG 调 const 版 at/operator[]，旧 const_cast 非 const 版（3+2+1 处）；
// ②P183 去引用缓存（每使用点重算 &m_vec44/&m_vec14[type]/&m_rivalData1）；
// ③P68 cast-this 形；④P244 形参 a 直用（删具名 b，消 movzbl+movb 双槽）；
// ⑤P196/P2 循环向 size>i 翻 setg（原 i<size 产 setl）；⑥P160-162 循环变量 i 复用；
// ⑦totalWeight 声明前移复现 movl 落点。
// 残差：ORIG 5 条死比较（m_stageIdx vs m_warRoomData.m_50，无消费者；4.4 tree-DCE 删除，
// `(void)(cond)`/具名 bool/空 if 三形实测均被删或反多 setl+store ⇒ N99 族）+ 标量区槽差 0xc
// （N82附，帧同为 0xb40）。@0x086b8a38
void WarField::ConsistMap(void* packet, const CDungeon* dungeon, const CMap* map,
                          unsigned char a, int playerCount)
{
    if (playerCount == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3b, 5);
        trace("WarField::ConsistMap playerCount is 0");
        return;
    }
    void* buf = packet;
    ((InterfacePacketBuf*)buf)->put_header(0, 0x5b);
    ((InterfacePacketBuf*)buf)->put_byte(dungeon->m_warRoomData.m_d8);  // +0x6f4+0xd8
    int index = ((InterfacePacketBuf*)buf)->get_index();
    ((InterfacePacketBuf*)buf)->put_short(0);
    int uid = m_mapInfo.m_assignItemIdx;
    // +0x6f4+0x44；ORIG 086b8b19 add $0x738 后调 const 版 operator[]（p167 实证）。
    int monsterCount =
        (int)(dungeon->m_warRoomData.m_vec44[m_stageIdx] * playerCount) / 6;
    int minCount = 10;
    monsterCount = std::max(monsterCount, minCount);
    if (m_mapInfo.m_monsterMap.size() != 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x53, 5);
        trace("WarField::ConsistMap Clear");
        m_mapInfo.Clear();
    }
    while (monsterCount > 9)
    {
        int totalProb = 0;
        int i = 0;
        for (; i <= 3; i = i + 1)
        {
            totalProb = totalProb +
                dungeon->m_warRoomData.m_spawnProb[m_stageIdx].m_probability[i];
        }
        if (totalProb == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0x65, "WARROOM totalProb is 0");
            return;
        }
        int randProb = get_rand_int(totalProb);
        totalProb = 0;
        i = 0;
        for (; i <= 3; i = i + 1)
        {
            totalProb = totalProb +
                dungeon->m_warRoomData.m_spawnProb[m_stageIdx].m_probability[i];
            if (randProb < totalProb)
            {
                break;
            }
        }
        int type = i;
        int totalWeight = 0;
        // +0x6f4+0x14+type*0xc（ORIG 每使用点重算地址，P183 禁引用缓存）
        int listSize = (int)dungeon->m_warRoomData.m_vec14[type].size();
        int weights[256];
        for (i = 0; listSize > i; i = i + 1)
        {
            CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                                    ->find_monster(dungeon->m_warRoomData.m_vec14[type].at(i));
            if (monster == 0)
            {
                LogManager::logFormat(
                    1, "WarField.cpp",
                    __PRETTY_FUNCTION__,
                    0x96, "WARROOM Find Monster[%d] Error",
                    dungeon->m_warRoomData.m_vec14[type].at(i));
                return;
            }
            weights[i] = monster->m_weight;               // CMonster +0x1d24
            totalWeight = totalWeight + weights[i];
        }
        if (totalWeight == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0x9f, "WARROOM totalWeight is 0, type %d", type);
            return;
        }
        randProb = get_rand_int(totalWeight);
        totalWeight = 0;
        i = 0;
        for (; listSize > i; i = i + 1)
        {
            totalWeight = totalWeight + weights[i];
            if (randProb < totalWeight)
            {
                break;
            }
        }
        int sel = i;
        CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                                ->find_monster(dungeon->m_warRoomData.m_vec14[type].at(sel));
        if (monster == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0xbc, "WARROOM::ConsistMap pMob is Null");
            return;
        }
        int rivalTotal = 0;
        // CMap +0x90（ORIG 每使用点重算地址）
        for (int i = 0; (int)map->m_rivalData1.size() > i; i = i + 1)
        {
            rivalTotal = rivalTotal + map->m_rivalData1[i].m_field08;
        }
        if (rivalTotal == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0xd0, "WARROOM rivalTotal is 0");
            return;
        }
        randProb = get_rand_int(rivalTotal);
        rivalTotal = 0;
        i = 0;
        while ((int)map->m_rivalData1.size() > i)
        {
            rivalTotal = rivalTotal + map->m_rivalData1[i].m_field08;
            if (randProb < rivalTotal)
            {
                break;
            }
            i = i + 1;
        }
        int rivalSel = i;
        map_monster mob;
        mob.m_instId = uid;
        uid = uid + 1;
        mob.m_roleType = (char)type;
        mob.m_mobIndex = monster->get_index();
        mob.m_level = a;
        *(char*)&mob.m_field11 = 1;         // 字节 +0x11（ORIG 逐字节写，非 movw）
        *((char*)&mob.m_field11 + 1) = 1;   // 字节 +0x12
        mob.m_field18 =
            dungeon->m_warRoomData.m_bc * (float)(6 - playerCount) + 1.0f;   // +0x7b0
        mob.m_field28 =
            dungeon->m_warRoomData.m_c0 * (float)(6 - playerCount) + 1.0f;   // +0x7b4
        m_mapInfo.Add_Mob(mob);
        monsterCount =
            monsterCount - dungeon->m_warRoomData.m_c4[type];                // +0x7b8+type*4
        ((InterfacePacketBuf*)buf)->put_short(mob.m_instId);
        ((InterfacePacketBuf*)buf)->put_byte(type);
        ((InterfacePacketBuf*)buf)->put_byte((int)a);
        ((InterfacePacketBuf*)buf)->put_byte(sel);
        ((InterfacePacketBuf*)buf)->put_byte(rivalSel);
        int rewardCount = dungeon->m_warRoomData.m_ec +                        // +0x7e0
                          get_rand_int(dungeon->m_warRoomData.m_f0);           // +0x7e4
        rewardCount = (int)((float)rewardCount *
                            dungeon->m_warRoomData.m_dc[type]);                // +0x7d0+type*4
        if (playerCount == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x107, 5);
            trace("WarField::ConsistMap playerCount is 0");
            playerCount = 1;
        }
        ((InterfacePacketBuf*)buf)->put_short(
            (int)((double)rewardCount / (0.3 * (double)(playerCount - 1) + 1.0)));
    }
    m_mapInfo.m_assignItemIdx = uid;
    m_stageIdx = m_stageIdx + 1;
    ((InterfacePacketBuf*)buf)->put_short(index, (int)m_mapInfo.m_monsterMap.size());
    ((InterfacePacketBuf*)buf)->finalize(true);
}

// ============================================================================
// HandleMonsterKill（086b9672：掉落生成 + 击杀通知包）
// ============================================================================

// [b28d-solo-WarField] 语义完成、byte 层 N 封顶（605/606，p167 CLEAR 94/94）：
// ①P235 具名字节视图（refData.m_field38/39/3c/3c_byte/3d/3e_byte/40_byte/41/43/44_byte）
// 消 29 条 lea+add（裸 *(char*)& 取址）；②P68/P0 删具名 buf 引用，内联
// ((InterfacePacketBuf*)&packet)->（ORIG 每调用点重载 0x10(%ebp)，无 buf 槽）。
// 残差：整体槽差 4（ORIG userUid=-0x40/refData=-0x110/map_item=-0x164 vs 我们 -0x3c/-0x10c/-0x160，
// 帧同为 0x1bc ⇒ N82 槽分配）；list 迭代器 end() 布局（ORIG 循环外预求值+jmp cond，4.4 旋转到
// 体尾 ⇒ N86/P164）；m_field11 逐字节取址 add $0x11（需 map_monster 匿名 union 字节视图，跨泳道 .h）；
// 2 处寄存器名（%edx vs %eax ⇒ N7 族）+ 尾部对齐 nop（§6.1 豁免）。@0x086b9672
bool WarField::HandleMonsterKill(int uid, PacketGuard& packet, map_monster& mob,
                                 CUser* user, const CDungeon* dungeon)
{
    int userUid = 0xffff;
    int moneyRatio = 0;
    if (user != 0)
    {
        userUid = (unsigned short)user->get_unique_id();
        int moneyLimit = (int)((const CDataManager*)G_CDataManager())
                             ->GetMoneyLimitPerLevel(user->get_charac_level(),
                                                     user->get_acc_name());
        if (moneyLimit < 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x178, 5);
            trace("WarField::HandleMonsterKill limitMoney(%d) is under 0", moneyLimit);
        }
        else
        {
            moneyRatio = (user->getCurCharacMoney() * 100) / moneyLimit;
        }
    }
    MapInfo* mi = &m_mapInfo;
    std::map<int, map_monster>::iterator it = mi->m_monsterMap.find(uid);
    if (it == mi->m_monsterMap.end())
    {
        return 0;
    }
    map_monster* fieldMob = &it->second;
    CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                            ->find_monster(fieldMob->m_mobIndex);
    if (monster == 0)
    {
        LogManager::logFormat(
            1, "WarField.cpp",
            __PRETTY_FUNCTION__,
            0x18c, "G_CDataManager()->find_monster(%u) fail", fieldMob->m_mobIndex);
        return 0;
    }
    mob = *fieldMob;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x26);
    ((InterfacePacketBuf*)&packet)->put_short(uid);
    std::list<map_item> itemList;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    stGenerateRefData_t refData;
    if (user != 0)
    {
        refData.m_dropRate = 100;
        if (ARAD::Singleton<ServiceRestrictManager>::Get()
                ->isRestricted(RestrictCategory::Enum_3, 1))
        {
            refData.m_dropRate = user->getStdDropRate();
        }
    }
    // 掉落上下文区（宽度逐字段 = HandleMonsterKill 086b98b1-0x298 汇编：movb/movl）。
    refData.m_field34 = fieldMob->m_level;               // movb +0x34
    refData.m_field35 = fieldMob->m_roleType;            // movb +0x35
    refData.m_rateSlot = fieldMob->m_level;              // movb +0x36
    refData.m_pad37 = 0;                                 // movb +0x37
    refData.m_field38 = 1;                               // movb +0x38（int 槽低字节）
    refData.m_field39 = *((char*)&fieldMob->m_field11 + 1);  // movb +0x39（mob +0x12）
    refData.m_field3c = moneyRatio;                      // movl +0x3c..0x3f
    refData.m_field40_byte = 1;                          // movb +0x40（指针槽低字节）
    refData.m_field41 = 0;                               // movb +0x41
    stGenerateResult_t result;
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
        ->generateRandomItem(refData, result);
    // 第二段（specific）：+0x38 movl、+0x3c/0x3d/0x35/0x36/0x3e/0x40/0x41/0x43/0x44 movb
    refData.m_rankBonusIndex = fieldMob->m_mobIndex;     // movl +0x38
    refData.m_field3c_byte = fieldMob->m_roleType;       // movb +0x3c
    refData.m_field3d = *(char*)&fieldMob->m_field11;    // movb +0x3d（mob +0x11）
    refData.m_rateSlot = 0;                              // movb +0x36
    refData.m_field3e_byte = 1;                          // movb +0x3e
    refData.m_field35 = fieldMob->m_level;               // movb +0x35
    refData.m_field40_byte = 0;                          // movb +0x40
    refData.m_field41 = 0;                               // movb +0x41
    refData.m_field43 = 0;                               // movb +0x43
    refData.m_field44_byte = 0;                          // movb +0x44
    if (user != 0 && user->isGMUser())
    {
        refData.m_field24 = user->getForceDropFlag();
    }
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
        ->generateSpecificItem(refData, result);
    dungeon->getDropItems();
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
        ->generateSpecificItem(refData, result);
    refData.m_field34 = 1;                               // movb +0x34
    refData.m_field35 = fieldMob->m_level;               // movb +0x35
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_3)
        ->generateRandomItem(refData, result);
    for (std::vector<Inven_Item>::iterator itemIt = result.m_dropItems.begin();
         itemIt != result.m_dropItems.end(); ++itemIt)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = mi->m_dropItemCnt;
        mi->m_dropItemCnt = mi->m_dropItemCnt + 1;
        item.m_dropIndex = userUid;
        item.m_createTick = curSec;
        item.m_item = *itemIt;
        itemList.push_back(item);
        mi->Add_Item(item);
    }
    ((InterfacePacketBuf*)&packet)->put_byte((int)itemList.size());
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    for (std::list<map_item>::iterator itemIt = itemList.begin();
         itemIt != itemList.end(); itemIt++)
    {
        map_item& mi = *itemIt;
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_itemIndex);
        ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.m_addInfo);
        ((InterfacePacketBuf*)&packet)->put_byte((int)mi.m_item.GetItemAttr());
        if (mi.m_item.isAvatarItemType())
        {
            CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(mi.m_item.m_addInfo);
            if (equip == 0)
            {
                ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.get_add_info());
            }
            else
            {
                ((InterfacePacketBuf*)&packet)->put_int(
                    equip->getAvatarPeriod((unsigned char)mi.m_item.m_fieldb) * 0x15180);
                emblem.init();
                equip->getAvatarSocket(mi.m_item.m_fieldb, emblem);
            }
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.get_add_info());
        }
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_item.m_fieldb);
        ((InterfacePacketBuf*)&packet)->put_byte((int)mi.m_item.m_amp.getAbilityType());
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_item.m_amp.getAbilityValue());
        ((InterfacePacketBuf*)&packet)->put_packet(mi.m_item);
        if (mi.m_item.isAvatarItemType())
        {
            ((InterfacePacketBuf*)&packet)->put_byte(1);
            ((InterfacePacketBuf*)&packet)->put_int(0x1e);
            ((InterfacePacketBuf*)&packet)->put_binary((const char*)&emblem, 0x1e);
            stAvatarExpansionInfo_t expansion;
            expansion.init();
            ((InterfacePacketBuf*)&packet)->put_int(4);
            ((InterfacePacketBuf*)&packet)->put_binary((const char*)&expansion, 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_byte(0);
        }
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_dropIndex);
    }
    mi->m_monsterMap.erase(it);
    return 1;
}

// ============================================================================
// WarAreaCounter（ORIG 0x83708c6 C1 / 0x83707a4 clear / Get* 族；类定义见
// CDataManager.h，C1/D1 位于 GameStubs.cpp）。布局（clear 实证）：
//   +0x60  int m_table[24]（时段边界，GetCurrenTimeTable 遍历）
//   +0xc0  int m_fieldC0[10]          +0xe8 int m_curIdx（当前时段行号）
//   +0xec/+0xf0/+0xf4 int             +0xf8 int m_fieldF8[10]
//   +0x120 std::map<int,stWarPoint>   +0x138/+0x13c int
//   +0x140 int m_num（时段边界数）    +0x144 int m_countTable[240]
//       （10 行 × 10 列 = 每时段 10 个 WarRoom 区的计数表，行号 = m_curIdx）
// ============================================================================

// ORIG 0x89024c4 T（_ZN14WarAreaCounter18GetCurrenTimeTableEv）：
// 取当前小时，在 m_table[0..m_num-2] 中查找所在时段，返回时段索引。
int WarAreaCounter::GetCurrenTimeTable()
{
    // b28d-lane11：ORIG 0x089024d4 `lea -0x18(%ebp),%eax; mov %eax,(%esp); call time`
    // = `time(&t)` 形；循环条件直读 m_num（无 num 局部）、t0/t1 内联无槽（P0）。
    time_t t;
    time(&t);
    struct tm tmbuf;
    struct tm* ptm = localtime_r(&t, &tmbuf);
    int curHour = ptm->tm_hour;
    int i = 0;
    for (; i < m_num - 1; ++i)
    {
        // b28d-lane11：P223 配套——单 continue 产额外 jmp 岛，改正向包裹。
        if (m_timeHourList[i] <= curHour)
        {
            if (m_timeHourList[i + 1] > curHour)
                break;
        }
    }
    return i;
}

// ORIG 0x82a3d80 W（_ZN14WarAreaCounter25GetWarRoomCountAtPeekTimeEi）：
// 返回当前时段（m_curIdx）第 idx 个 WarRoom 区的目标房间数；idx 越界记日志返 0。
int WarAreaCounter::GetWarRoomCountAtPeekTime(int idx)
{
    if (idx < 0 || idx > 9)
    {
        LogManager::logFormat(1, "WarField.cpp",
            "int WarAreaCounter::GetWarRoomCountAtPeekTime(int)", 0x317,
            "WarAreaCounter GetWarRoomCountAtPeekTime error idx(%d)", idx);
        return 0;
    }
    // ORIG: 直读 m_curHourIdx（mov 0xe8(%eax),%edx），无意引局部槽（P0）。
    return m_warRoomCount[m_curHourIdx][idx];
}

// ORIG 0x82a3df4 W（_ZN14WarAreaCounter25GetWarRoomCountFirstIndexEi）：
// 当前时段前 idx 个区的计数和（供 WarRoom 扩容定位首索引）。
int WarAreaCounter::GetWarRoomCountFirstIndex(int idx)
{
    if (idx < 0 || idx > 9)
    {
        LogManager::logFormat(1, "WarField.cpp",
            "int WarAreaCounter::GetWarRoomCountFirstIndex(int)", 0x320,
            "WarAreaCounter GetWarRoomCountFirstIndex error idx(%d)", idx);
        return 0;
    }
    int sum = 0;
    for (int j = 0; j < idx; ++j)
    {
        // ORIG: 循环内直读 m_curHourIdx（不 hoist 到局部槽，P0）。
        sum += m_warRoomCount[m_curHourIdx][j];
    }
    return sum;
}

// ORIG 0x82a3e90 W（_ZN14WarAreaCounter24GetWarRoomCountLastIndexEi）：
// 当前时段前 idx+1 个区的计数和（供 WarRoom 收缩定位末索引）。
int WarAreaCounter::GetWarRoomCountLastIndex(int idx)
{
    if (idx < 0 || idx > 9)
    {
        LogManager::logFormat(1, "WarField.cpp",
            "int WarAreaCounter::GetWarRoomCountLastIndex(int)", 0x32f,
            "WarAreaCounter GetWarRoomCountLastIndex error idx(%d)", idx);
        return 0;
    }
    int sum = 0;
    for (int j = 0; j <= idx; ++j)
    {
        // ORIG: 循环内直读 m_curHourIdx（P0）；尾部 sub $0x1（真语义 bug：返 sum-1）。
        sum += m_warRoomCount[m_curHourIdx][j];
    }
    return sum - 1;
}
