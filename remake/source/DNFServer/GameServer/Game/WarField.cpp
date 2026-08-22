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

int get_rand_int(int range);  // ORIG T 0x86b1b87

// ============================================================================
// 本地支撑类型（名称/布局与 ORIG 一致；成员在各自批次 TU 定义）
// ============================================================================

// 怪物类型刷新概率（ORIG vector<STMonsterTypeSpawnProb> 元素 0x10 字节，
// ConsistMap 按 int[4] 读）
struct STMonsterTypeSpawnProb
{
    int m_prob[4];
};

// 对战地图数据（ORIG vector<STRivalMapData> 元素 0xc 字节，+8 为权重）
struct STRivalMapData
{
    int m_0;     // +0x00
    int m_4;     // +0x04
    int m_rate;  // +0x08
};

// 战争点数（ORIG 0x10 字节；clear 0x891a87a memset 0x10）
struct stWarPoint
{
    int m_0;          // +0x00
    int m_pts[3];     // +0x04
};

struct WpBonusPointResult
{
    int m_base;   // +0x00
    int m_add;    // +0x04
};

#pragma pack(push, 1)
struct stAvatarEmblemInfo_t
{
    void init();  // ORIG W 0x81506e8

    char m_pad[30];
};
#pragma pack(pop)

struct stAvatarExpansionInfo_t
{
    void init();  // ORIG W 0x833421e

    char m_pad[4];
};

// 掉落生成引用/结果（ctor/dtor 由 WongWork 批次 TU 提供）
struct stGenerateRefData_t
{
    stGenerateRefData_t();
    ~stGenerateRefData_t();

    int m_dropRate;      // +0x00
    char m_pad4[0x20];   // +0x04
    int m_premiumRate;   // +0x20
    char m_forceDrop;    // +0x24
    char m_pad25[0x48 - 0x25];  // +0x25
};  // 0x48 字节

struct stGenerateResult_t
{
    stGenerateResult_t();
    ~stGenerateResult_t();

    std::vector<Inven_Item> m_dropItems;  // +0x00（0xc 字节）
};

class CItemGenerator
{
public:
    virtual void vf0() = 0;          // vtable+0
    virtual void vf1() = 0;          // vtable+4
    virtual void GenerateA(stGenerateRefData_t* ref, stGenerateResult_t* result) = 0;  // vtable+8
    virtual void GenerateB(stGenerateRefData_t* ref, stGenerateResult_t* result) = 0;  // vtable+0xc
};

namespace WongWork
{
// CItemGeneratorMgr 权威声明见 CItemGeneratorMgr.h（经 CDataManager.h 引入），此处不再重复定义。
}

namespace RestrictCategory
{
enum Enum
{
    Enum_0 = 0,
    Enum_3 = 3
};
}

class ServiceRestrictManager
{
public:
    bool isRestricted(RestrictCategory::Enum category, int idx);  // ORIG T 0x816e75e
};

namespace ARAD
{
template <typename T>
class Singleton
{
public:
    static T* Get();  // ORIG W 0x81625e6
};
}

class CMonster
{
public:
    int get_index();  // ORIG T 0x83748ce
};

class CDataManager;
CDataManager* G_CDataManager();  // ORIG T 0x80cc19b

// ============================================================================
// 跨类方法（asm-label extern；mangled 名与 ORIG 一致）
// ============================================================================
extern "C" unsigned int sub_CUser_get_unique_id(const void* self)
    asm("_ZNK5CUser13get_unique_idEv");
extern "C" const char* sub_CUser_get_acc_name(const void* self)
    asm("_ZNK5CUser12get_acc_nameEv");
extern "C" int sub_CUser_getStdDropRate(void* self)
    asm("_ZN5CUser14getStdDropRateEv");
extern "C" bool sub_CUser_isGMUser(void* self)
    asm("_ZN5CUser8isGMUserEv");
extern "C" unsigned char sub_CUser_getForceDropFlag(void* self)
    asm("_ZN5CUser16getForceDropFlagEv");

extern "C" int sub_CUserCharacInfo_get_charac_level(const void* self)
    asm("_ZNK15CUserCharacInfo16get_charac_levelEv");
extern "C" int sub_CUserCharacInfo_getCurCharacMoney(const void* self)
    asm("_ZNK15CUserCharacInfo17getCurCharacMoneyEv");
extern "C" void sub_CUserCharacInfo_checkBonusPoint(void* self)
    asm("_ZN15CUserCharacInfo15checkBonusPointEv");
extern "C" void sub_CUserCharacInfo_getWpBonusPoint(void* self, WpBonusPointResult* out)
    asm("_ZN15CUserCharacInfo15getWpBonusPointER18WpBonusPointResult");

extern "C" void* sub_CDataManager_find_monster(const CDataManager* self, int idx)
    asm("_ZNK12CDataManager12find_monsterEi");
extern "C" void* sub_CDataManager_find_item(const CDataManager* self, int idx)
    asm("_ZNK12CDataManager9find_itemEi");
extern "C" int sub_CDataManager_GetMoneyLimitPerLevel(const CDataManager* self,
                                                      int level, const char* name)
    asm("_ZNK12CDataManager21GetMoneyLimitPerLevelEiPKc");
extern "C" WongWork::CItemGeneratorMgr* sub_CDataManager_getItemGenerator(CDataManager* self)
    asm("_ZN12CDataManager16getItemGeneratorEv");

extern "C" void* sub_CDungeon_getDropItems(const void* self)
    asm("_ZNK8CDungeon12getDropItemsEv");

extern "C" void sub_WpBonusPointResult_C1(void* self) asm("_ZN18WpBonusPointResultC1Ev");
extern "C" void sub_stAvatarEmblemInfo_t_init(void* self) asm("_ZN20stAvatarEmblemInfo_t4initEv");
extern "C" void sub_stAvatarExpansionInfo_t_init(void* self)
    asm("_ZN23stAvatarExpansionInfo_t4initEv");
extern "C" int sub_CEquipItem_getAvatarPeriod(const void* self, unsigned char a)
    asm("_ZNK10CEquipItem15getAvatarPeriodEh");
extern "C" void sub_CEquipItem_getAvatarSocket(const void* self, int slot,
                                               stAvatarEmblemInfo_t* emblem)
    asm("_ZNK10CEquipItem15getAvatarSocketEiR20stAvatarEmblemInfo_t");
extern "C" void sub_CSystemTime_getCurSec(void* self) asm("_ZN11CSystemTime9getCurSecEv");

// ---- GlobalData::s_systemTime_（真实符号 _ZN10GlobalData13s_systemTime_E） ----
extern CSystemTime GlobalData_s_systemTime_ asm("_ZN10GlobalData13s_systemTime_E");

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
// Reset（086b8a14：MapInfo::Clear + stageIdx=0）
// ============================================================================

void WarField::Reset()
{
    m_mapInfo.Clear();
    m_stageIdx = 0;
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
    std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return 0x15;
    }
    int curSec = GlobalData_s_systemTime_.getCurSec();
    if (curSec - it->second.m_createTick < 10)
    {
        if (it->second.m_dropIndex != (unsigned short)-1 &&
            (unsigned int)it->second.m_dropIndex != (unsigned int)owner)
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
    int old = m_mapInfo.m_dropItemCnt;
    m_mapInfo.m_dropItemCnt = old + 1;
    item.m_itemIndex = old;
    m_mapInfo.Add_Item(item);
    return old;
}

// ============================================================================
// GetFieldItem（086b9fe6）
// ============================================================================

int WarField::GetFieldItem(int idx, map_item& out)
{
    std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.find(idx);
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

bool WarField::GetFieldMob(int idx, map_monster& out)
{
    std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.find(idx);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    map_monster mob(it->second);
    out = mob;
    return 1;
}

// ============================================================================
// MakePacket（086b92e2：0x1d 战场怪物信息包）
// ============================================================================

void WarField::MakePacket(int packetId, void* packet)
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)packet;
    if (packetId == 0x1d)
    {
        buf->put_header(0, 0x1d);
        buf->put_int(rand());
        buf->put_byte(0);
        buf->put_byte(0);
        buf->put_byte(1);
        buf->put_short(m_mapInfo.m_mapIndex);
        buf->put_byte((int)m_mapInfo.m_monsterMap.size());
        map_monster mob;
        for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
             it != m_mapInfo.m_monsterMap.end(); ++it)
        {
            mob = it->second;
            buf->put_short(0);
            buf->put_int(mob.m_mobId);
            buf->put_short((short)mob.m_instId);
            buf->put_int(mob.m_mobIndex);
            buf->put_byte((int)mob.m_level);
            buf->put_byte((int)mob.m_roleType);
            buf->put_byte((int)mob.m_field13);
            buf->put_byte((int)mob.m_field14);
        }
        buf->put_byte(0);
        buf->finalize(true);
    }
}

// ============================================================================
// HandleWpPerMonster（086ba1ea：战争点数累计）
// ============================================================================

int WarField::HandleWpPerMonster(CUser* user, int monsterIdx, int wp,
                                 map_monster& out)
{
    std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.find(monsterIdx);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    map_monster* mob = &it->second;
    std::map<int, stWarPoint, std::greater<int> >* warMap =
        (std::map<int, stWarPoint, std::greater<int> >*)((char*)G_CDataManager() + 0x88d4);
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
    sub_WpBonusPointResult_C1(&bonus);
    sub_CUserCharacInfo_checkBonusPoint(user);
    sub_CUserCharacInfo_getWpBonusPoint(user, &bonus);
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

void WarField::ConsistMap(void* packet, const CDungeon* dungeon, const CMap* map,
                          unsigned char a, int playerCount)
{
    unsigned char b = a;
    if (playerCount == 0)
    {
        cMyTrace trace(
            "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
            0x3b, 5);
        trace("WarField::ConsistMap playerCount is 0");
        return;
    }
    InterfacePacketBuf* buf = (InterfacePacketBuf*)packet;
    buf->put_header(0, 0x5b);
    buf->put_byte(*(int*)((char*)dungeon + 0x7cc));
    int index = buf->get_index();
    buf->put_short(0);
    int uid = m_mapInfo.m_assignItemIdx;
    std::vector<int>* stageMonsterList = (std::vector<int>*)((char*)dungeon + 0x738);
    int monsterCount = (int)((*stageMonsterList)[m_stageIdx] * playerCount) / 6;
    int minCount = 10;
    monsterCount = std::max(monsterCount, minCount);
    if (m_mapInfo.m_monsterMap.size() != 0)
    {
        cMyTrace trace(
            "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
            0x53, 5);
        trace("WarField::ConsistMap Clear");
        m_mapInfo.Clear();
    }
    while (monsterCount > 9)
    {
        int totalProb = 0;
        for (int i = 0; i <= 3; i = i + 1)
        {
            std::vector<STMonsterTypeSpawnProb>* spawnProb =
                (std::vector<STMonsterTypeSpawnProb>*)((char*)dungeon + 0x7a4);
            totalProb = totalProb + ((int*)(&(*spawnProb)[m_stageIdx]))[i];
        }
        if (totalProb == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
                0x65, "WARROOM totalProb is 0");
            return;
        }
        int randProb = get_rand_int(totalProb);
        totalProb = 0;
        int i = 0;
        for (; i <= 3; i = i + 1)
        {
            std::vector<STMonsterTypeSpawnProb>* spawnProb =
                (std::vector<STMonsterTypeSpawnProb>*)((char*)dungeon + 0x7a4);
            totalProb = totalProb + ((int*)(&(*spawnProb)[m_stageIdx]))[i];
            if (randProb < totalProb)
            {
                break;
            }
        }
        int type = i;
        int count = 0;
        std::vector<int>* mobList =
            (std::vector<int>*)((char*)dungeon + type * 0xc + 0x708);
        int listSize = (int)mobList->size();
        int totalWeight = 0;
        int weights[256];
        for (int i = 0; i < listSize; i = i + 1)
        {
            int mobIdx = mobList->at(i);
            CMonster* monster = (CMonster*)sub_CDataManager_find_monster(
                G_CDataManager(), mobIdx);
            if (monster == 0)
            {
                LogManager::logFormat(
                    1, "WarField.cpp",
                    "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
                    0x96, "WARROOM Find Monster[%d] Error", mobIdx);
                return;
            }
            weights[i] = *(int*)((char*)monster + 0x1d24);
            totalWeight = totalWeight + weights[i];
        }
        if (totalWeight == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
                0x9f, "WARROOM totalWeight is 0, type %d", type);
            return;
        }
        randProb = get_rand_int(totalWeight);
        totalWeight = 0;
        int i2 = 0;
        for (; i2 < listSize; i2 = i2 + 1)
        {
            totalWeight = totalWeight + weights[i2];
            if (randProb < totalWeight)
            {
                break;
            }
        }
        int sel = i2;
        int mobIdx = mobList->at(sel);
        CMonster* monster = (CMonster*)sub_CDataManager_find_monster(
            G_CDataManager(), mobIdx);
        if (monster == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
                0xbc, "WARROOM::ConsistMap pMob is Null");
            return;
        }
        int rivalTotal = 0;
        std::vector<STRivalMapData>* rivalData =
            (std::vector<STRivalMapData>*)((char*)map + 0x90);
        for (int i = 0; i < (int)rivalData->size(); i = i + 1)
        {
            rivalTotal = rivalTotal + (*rivalData)[i].m_rate;
        }
        if (rivalTotal == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
                0xd0, "WARROOM rivalTotal is 0");
            return;
        }
        randProb = get_rand_int(rivalTotal);
        rivalTotal = 0;
        int rivalSel = 0;
        for (int i = 0; i < (int)rivalData->size(); i = i + 1)
        {
            rivalTotal = rivalTotal + (*rivalData)[i].m_rate;
            if (randProb < rivalTotal)
            {
                break;
            }
            rivalSel = i + 1;
        }
        map_monster mob;
        mob.m_instId = uid;
        uid = uid + 1;
        mob.m_roleType = (char)type;
        mob.m_mobIndex = monster->get_index();
        mob.m_level = (char)b;
        mob.m_field11 = 0x0101;   // 字节 +0x11 与 +0x12 均置 1（ORIG ConsistMap 逐字节写）
        mob.m_field18 = *(float*)((char*)dungeon + 0x7b0) * (float)(6 - playerCount) + 1.0f;
        mob.m_field28 = *(float*)((char*)dungeon + 0x7b4) * (float)(6 - playerCount) + 1.0f;
        map_monster mobCopy(mob);
        m_mapInfo.Add_Mob(mobCopy);
        monsterCount = monsterCount - *(int*)((char*)dungeon + type * 4 + 0x7b8);
        buf->put_short(mob.m_instId);
        buf->put_byte(type);
        buf->put_byte((int)b);
        buf->put_byte(sel);
        buf->put_byte(rivalSel);
        int rewardCount = *(int*)((char*)dungeon + 0x7e0) +
                          get_rand_int(*(int*)((char*)dungeon + 0x7e4));
        rewardCount = (int)((float)rewardCount *
                            *(float*)((char*)dungeon + 0x7d0 + type * 4));
        if (playerCount == 0)
        {
            cMyTrace trace(
                "void WarField::ConsistMap(void*, const CDungeon*, const CMap*, byte, int)",
                0x107, 5);
            trace("WarField::ConsistMap playerCount is 0");
            playerCount = 1;
        }
        buf->put_short((int)((double)rewardCount / (0.3 * (double)(playerCount - 1) + 1.0)));
    }
    m_mapInfo.m_assignItemIdx = uid;
    m_stageIdx = m_stageIdx + 1;
    buf->put_short(index, (int)m_mapInfo.m_monsterMap.size());
    buf->finalize(true);
}

// ============================================================================
// HandleMonsterKill（086b9672：掉落生成 + 击杀通知包）
// ============================================================================

bool WarField::HandleMonsterKill(int uid, PacketGuard& packet, map_monster& mob,
                                 CUser* user, const CDungeon* dungeon)
{
    int moneyRatio = 0;
    unsigned short userUid = 0xffff;
    if (user != 0)
    {
        userUid = (unsigned short)sub_CUser_get_unique_id(user);
        const char* accName = sub_CUser_get_acc_name(user);
        int level = sub_CUserCharacInfo_get_charac_level(user);
        int moneyLimit = sub_CDataManager_GetMoneyLimitPerLevel(
            G_CDataManager(), level, accName);
        if (moneyLimit < 1)
        {
            cMyTrace trace(
                "bool WarField::HandleMonsterKill(int, PacketGuard&, map_monster&, CUser*, const CDungeon*)",
                0x178, 5);
            trace("WarField::HandleMonsterKill limitMoney(%d) is under 0");
        }
        else
        {
            moneyRatio = (sub_CUserCharacInfo_getCurCharacMoney(user) * 100) / moneyLimit;
        }
    }
    std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.find(uid);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    map_monster* fieldMob = &it->second;
    CMonster* monster = (CMonster*)sub_CDataManager_find_monster(
        G_CDataManager(), fieldMob->m_mobIndex);
    if (monster == 0)
    {
        LogManager::logFormat(
            1, "WarField.cpp",
            "bool WarField::HandleMonsterKill(int, PacketGuard&, map_monster&, CUser*, const CDungeon*)",
            0x18c, "G_CDataManager()->find_monster(%u) fail", fieldMob->m_mobIndex);
        return 0;
    }
    mob = *fieldMob;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0x26);
    buf->put_short(uid);
    std::list<map_item> itemList;
    int curSec = GlobalData_s_systemTime_.getCurSec();
    stGenerateRefData_t refData;
    if (user != 0)
    {
        refData.m_dropRate = 100;
        ServiceRestrictManager* srm = ARAD::Singleton<ServiceRestrictManager>::Get();
        if (srm->isRestricted(RestrictCategory::Enum_3, 1))
        {
            refData.m_dropRate = sub_CUser_getStdDropRate(user);
        }
    }
    *(char*)((char*)&refData + 0x34) = fieldMob->m_level;
    *(char*)((char*)&refData + 0x35) = fieldMob->m_roleType;
    *(char*)((char*)&refData + 0x36) = fieldMob->m_level;
    *(char*)((char*)&refData + 0x37) = 0;
    *(char*)((char*)&refData + 0x38) = 1;
    *(char*)((char*)&refData + 0x39) = (char)(fieldMob->m_field11 >> 8);   // 字节 +0x12（short 高位）
    *(char*)((char*)&refData + 0x40) = 1;
    *(char*)((char*)&refData + 0x41) = 0;
    stGenerateResult_t result;
    CItemGenerator* gen = (CItemGenerator*)sub_CDataManager_getItemGenerator(
        G_CDataManager())->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0);
    gen->GenerateA(&refData, &result);
    *(int*)((char*)&refData + 0x38) = fieldMob->m_mobIndex;
    *(char*)((char*)&refData + 0x3c) = fieldMob->m_roleType;
    *(char*)((char*)&refData + 0x3d) = (char)fieldMob->m_field11;         // 字节 +0x11（short 低位）
    *(char*)((char*)&refData + 0x36) = 0;
    *(char*)((char*)&refData + 0x35) = fieldMob->m_level;
    *(char*)((char*)&refData + 0x40) = 0;
    *(char*)((char*)&refData + 0x41) = 0;
    *(char*)((char*)&refData + 0x43) = 0;
    *(char*)((char*)&refData + 0x44) = 0;
    if (user != 0 && sub_CUser_isGMUser(user))
    {
        refData.m_forceDrop = sub_CUser_getForceDropFlag(user);
    }
    gen = (CItemGenerator*)sub_CDataManager_getItemGenerator(
        G_CDataManager())->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0);
    gen->GenerateB(&refData, &result);
    void* drops = sub_CDungeon_getDropItems(dungeon);
    (void)drops;
    gen = (CItemGenerator*)sub_CDataManager_getItemGenerator(
        G_CDataManager())->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4);
    gen->GenerateB(&refData, &result);
    *(char*)((char*)&refData + 0x34) = 1;
    *(char*)((char*)&refData + 0x35) = fieldMob->m_level;
    gen = (CItemGenerator*)sub_CDataManager_getItemGenerator(
        G_CDataManager())->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_3);
    gen->GenerateA(&refData, &result);
    for (std::vector<Inven_Item>::iterator itemIt = result.m_dropItems.begin();
         itemIt != result.m_dropItems.end(); ++itemIt)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = m_mapInfo.m_dropItemCnt;
        m_mapInfo.m_dropItemCnt = m_mapInfo.m_dropItemCnt + 1;
        item.m_dropIndex = userUid;
        item.m_createTick = curSec;
        item.m_item = *itemIt;
        itemList.push_back(item);
        m_mapInfo.Add_Item(item);
    }
    buf->put_byte((int)itemList.size());
    stAvatarEmblemInfo_t emblem;
    sub_stAvatarEmblemInfo_t_init(&emblem);
    for (std::list<map_item>::iterator itemIt = itemList.begin();
         itemIt != itemList.end(); ++itemIt)
    {
        map_item& mi = *itemIt;
        buf->put_short(mi.m_itemIndex);
        buf->put_int(mi.m_item.m_addInfo);
        buf->put_byte((int)mi.m_item.GetItemAttr());
        if (!mi.m_item.isAvatarItemType())
        {
            buf->put_int(mi.m_item.get_add_info());
        }
        else
        {
            void* equip = sub_CDataManager_find_item(
                G_CDataManager(), mi.m_item.m_addInfo);
            if (equip == 0)
            {
                buf->put_int(mi.m_item.get_add_info());
            }
            else
            {
                buf->put_int(sub_CEquipItem_getAvatarPeriod(
                                 equip, (unsigned char)mi.m_item.m_fieldb) *
                             0x15180);
                sub_stAvatarEmblemInfo_t_init(&emblem);
                sub_CEquipItem_getAvatarSocket(equip, mi.m_item.m_fieldb, &emblem);
            }
        }
        buf->put_short(mi.m_item.m_fieldb);
        buf->put_byte((int)mi.m_item.m_amp.getAbilityType());
        buf->put_short(mi.m_item.m_amp.getAbilityValue());
        buf->put_packet(mi.m_item);
        if (!mi.m_item.isAvatarItemType())
        {
            buf->put_byte(0);
        }
        else
        {
            buf->put_byte(1);
            buf->put_int(0x1e);
            buf->put_binary((const char*)&emblem, 0x1e);
            stAvatarExpansionInfo_t expansion;
            sub_stAvatarExpansionInfo_t_init(&expansion);
            buf->put_int(4);
            buf->put_binary((const char*)&expansion, 4);
        }
        buf->put_short(mi.m_dropIndex);
    }
    m_mapInfo.m_monsterMap.erase(it);
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
void* WarAreaCounter::GetCurrenTimeTable()
{
    time_t t = time(NULL);
    struct tm tmbuf;
    struct tm* ptm = localtime_r(&t, &tmbuf);
    int curHour = ptm->tm_hour;
    int num = m_num;
    int i = 0;
    for (; i < num - 1; ++i)
    {
        int t0 = m_timeHourList[i];
        if (t0 > curHour)
            continue;
        int t1 = m_timeHourList[i + 1];
        if (t1 > curHour)
            break;
    }
    return (void*)(size_t)i;
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
    int cur = m_curHourIdx;
    return m_warRoomCount[cur][idx];
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
    int cur = m_curHourIdx;
    int sum = 0;
    for (int j = 0; j < idx; ++j)
    {
        sum += m_warRoomCount[cur][j];
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
    int cur = m_curHourIdx;
    int sum = 0;
    for (int j = 0; j <= idx; ++j)
    {
        sum += m_warRoomCount[cur][j];
    }
    return sum;
}
