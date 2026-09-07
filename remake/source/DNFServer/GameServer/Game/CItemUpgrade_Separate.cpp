// ============================================================================
// CItemUpgrade_Separate.cpp — WongWork 分离强化系统全链 + GenuineDamageUpgradeTable
//                             （Ctor2_ContB）
//
// 全部实现按 docs/class_func_reports/{WongWork__CItemUpgrade_Separate,
// WongWork__IItemUpgrade,GenuineDamageUpgradeTable,CUser__makeGuildItemSeparateUpgradMessage}/
// 报告 + ORIG objdump 逐段转写。符号对照：
//   WongWork::IItemUpgrade        C1 T 0x080fc834 / D1 T 0x080fc842 /
//                                 item_upgrade_system T 0x080fc850
//   WongWork::STUpgradeInfo       C1 W 0x0811edfe
//   GenuineDamageUpgradeInfo      C1 W 0x0891baf0
//   OutLineEFFECTInfoTemp         C1 W 0x0891bb1e
//   GenuineDamageUpgradeTable     C1 W 0x0811ee5e / D1 W 0x0811eeda /
//                                 getGenuineUpgradeDamage T 0x08912a54 /
//                                 getGenuineUpgradeMaterialCnt T(_ZNK) 0x08912b88 /
//                                 getEffectInfo T 0x08912d10
//   UpgradeSeparateInfo           IncUpgradeSeparate W 0x0811ed32（声明补入 Inven_Item.h）
//   WongWork::CItemUpgrade_Separate C1 T 0x0811dfc0 / D1 T 0x0811e0be /
//                                 ProcUpgrade T 0x0811e66e / LoadUpgradeScript T 0x0811eca8 /
//                                 _CheckCondition T 0x0811e138 / _GetPenaltyType T 0x0811e2ce /
//                                 _GetUpgradeInfo T 0x0811e2e2 / _DoProcUpgrade T 0x0811e468 /
//                                 _NoticeUpgrade T 0x0811e53a / getUpgradeInfoTable W 0x0811ee16
//   CUser::makeGuildItemSeparateUpgradMessage T 0x0867a242（CUser.h 补声明）
// importGenuineUpgradeTable(T 0x08912336) 仅声明——定义归脚本导入域批次。
// ============================================================================
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy 模板定义
#include "CItemUpgrade_Separate.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "PacketTypes.h"     // MSG_ITEM_UPGRADE 完整类型（具名字段访问）
#include "Inven_Item.h"
#include "CEquipItem.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "PvPTypes.h"                 // CMTRand 完整类型（0x9c8）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString

#include <cstdio>
#include <cstring>
#include <string>

namespace
{

union FloatBits
{
    unsigned int u;
    float f;
};

// ORIG .rodata 常量（位模式实证）：
FloatBits kMatEpsilon = { 0x33d6bf95UL };   // DAT_08e122f4 ≈ 1e-7f（材料放大前加值）
const float kGoldBase = 9840.0f;            // DAT_08b575b0（金币换算基数）
const float kRoundHalf = 0.5f;              // DAT_08e122e8（截断前置）

int truncateToInt(float value)
{
    // ORIG：fnstcw/fldcw 0xc 置截断后 fistp（含 64 位变体取低 dword）。
    return static_cast<int>(value);
}

}  // namespace

// ============================ POD 生命周期 ============================

GenuineDamageUpgradeInfo::GenuineDamageUpgradeInfo()          // ORIG W 0x0891baf0
{
    m_a = 0;
    m_b = 0;
    m_levelRate = 0.0f;     // +0x08（ctor 清零；import 写 level/10000f）
    m_goldRate = 0.0f;      // +0x0c；+0x10 保持未初始化（ORIG 同）
}

OutLineEFFECTInfoTemp::OutLineEFFECTInfoTemp()                // ORIG W 0x0891bb1e
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_c = 0;                // ORIG 对 +0x8 连续写两次（死存储忠实保留）
}

namespace WongWork
{

STUpgradeInfo::STUpgradeInfo()                                // ORIG W 0x0811edfe
{
    m_materialCnt = 0;
    m_gold = 0;
}

// ============================ IItemUpgrade ============================

IItemUpgrade::IItemUpgrade() {}                               // ORIG T 0x080fc834

IItemUpgrade::~IItemUpgrade() {}                              // ORIG T 0x080fc842（非虚）

// ORIG T 0x080fc850：put_header(1,cmd) → 虚调槽 0 → 0=finalize+Send / 否则错误包。
void IItemUpgrade::item_upgrade_system(CUser* user, MSG_ITEM_UPGRADE* msg, int cmd)
{
    int result = 0;
    PacketGuard pg;
    pg.put_header(1, cmd);
    result = this->ProcUpgrade(user, msg, &pg);
    if (result == 0)
    {
        pg.finalize(true);
        user->Send(pg);
    }
    else
    {
        user->SendCmdErrorPacket2Byte(static_cast<ENUM_CMDPACKET>(cmd),
                                      static_cast<unsigned short>(result & 0xffff));
    }
}

// ============================ GenuineDamageUpgradeTable ============================

}  // namespace WongWork

GenuineDamageUpgradeTable::GenuineDamageUpgradeTable() {}     // ORIG W 0x0811ee5e：仅三 map 构造

GenuineDamageUpgradeTable::~GenuineDamageUpgradeTable() {}    // ORIG W 0x0811eeda：三 map 析构

// ORIG T 0x08912a54。浮点读取：statWeightsByRarity[rarity]（this+(rarity+8)*4）
// 与 weaponWeights[a][b]（this+((6a+b)+12)*4+8）；分母 *(int*)this；useAlt 选择
// info.m_b / info.m_a；尾 +0.5f（DAT_08e122e8）后截断。
int GenuineDamageUpgradeTable::getGenuineUpgradeDamage(ENUM_RARITY rarity, int a, int b,
                                                       int key, int count, bool useAlt)
{
    static const float pointFive = 0.5f;   // ORIG .data 093f4fb0
    static const float two = 2.0f;         // ORIG .data 093f4fb4
    const float altBias = two * pointFive;

    if (key == 0)
    {
        return 0;
    }

    // P48 no-CSE：ORIG 三元两分支各 fresh m_infos[key]（operator[] 共 2 次）。
    const int multiplier = useAlt ? m_infos[key].m_b : m_infos[key].m_a;

    float result =
        m_statWeightsByRarity[static_cast<int>(rarity)] *
        m_weaponWeights[a][b] *
        (static_cast<float>(count) / static_cast<float>(m_divisor) + altBias) *
        static_cast<float>(multiplier);

    result += kRoundHalf;
    return static_cast<int>(result);
}

// ORIG T(_ZNK…) 0x08912b88。
int GenuineDamageUpgradeTable::getGenuineUpgradeMaterialCnt(unsigned char growth, int sepLv,
                                                            ENUM_EQUIPMENTTYPE type,
                                                            ENUM_RARITY rarity) const
{
    if (type != static_cast<ENUM_EQUIPMENTTYPE>(10))
    {
        return -1;
    }
    if (growth > 99)
    {
        return -1;
    }

    float factor = 1.0f * m_itemWeightsByRarity[static_cast<int>(rarity)];

    std::map<int, GenuineDamageUpgradeInfo>::const_iterator it =
        m_infos.find(sepLv + 1);
    if (it == m_infos.end())
    {
        return -1;
    }
    // ORIG 0x08912b88：pt() 后 flds 0x10(%eax) = pair+0x10 = value+0xc = m_goldRate
    // （import 第 4 值；m_materialRate +0x10 从不被 import 写入）
    factor = kMatEpsilon.f + factor * it->second.m_goldRate;

    std::map<int, GenuineDamageUpgradeMaterialInfo>::const_iterator mit =
        m_materials.find(static_cast<int>(growth));
    if (mit == m_materials.end())
    {
        return -1;
    }

    const float scaled =
        static_cast<float>(mit->second.m_materialCount) * factor;
    const long long wide = static_cast<long long>(scaled);   // ORIG fistpll
    int count = static_cast<int>(wide);                      // 取低 dword
    if (count == 0)
    {
        return 1;
    }
    return count;
}

// ORIG T 0x08912d10（sret 返回；静态局部 temp 经 __cxa_guard 初始化）。
OutLineEFFECTInfoTemp GenuineDamageUpgradeTable::getEffectInfo(int key)
{
    static OutLineEFFECTInfoTemp temp;

    if (m_maxLevel >= key)
    {
        std::map<int, OutLineEFFECTInfoTemp>::iterator it =
            m_effects.find(key);
        std::map<int, OutLineEFFECTInfoTemp>::const_iterator cit(it);
        if (cit != m_effects.end())
        {
            return cit->second;
        }
    }
    return temp;
}

// ============================ CItemUpgrade_Separate ============================

namespace WongWork
{

// ORIG T 0x0811dfc0：基类 vptr → 本类 vptr → 表构造 → +0x190=100000 →
// new CMTRand(time) 存 +0x18c → 100 次 randInt(m_randRange) 热身。
CItemUpgrade_Separate::CItemUpgrade_Separate()
{
    // 判词 N（batch-26 lane14）：ORIG time 临时@-0x24/range@-0x20 vs 4.4
    // 临时固定浅槽 -0x20、具名 range 深槽 -0x24 —— 4.4 call 结果临时不随
    // 声明序挪槽（具名化 seed 可对槽但 lea 延后至 new 之后成 DIFF，更差）；
    // 帧/指令数等齐，2 行槽号差 N 封顶（0x0811e0be）。
    m_randRange = 100000;
    unsigned long range;
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        range = m_randRange;
        m_rand->randInt(range);
    }
}

// ORIG T 0x0811e0be：delete m_rand 置空 → 表析构 → 基类析构（成员逆序自动）。
CItemUpgrade_Separate::~CItemUpgrade_Separate()
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}

// ORIG T 0x0811eca8：import 尾透传（int 返回零开销透传 eax；0=成功）。
int CItemUpgrade_Separate::LoadUpgradeScript(const char* path)
{
    return importGenuineUpgradeTable(path, &m_table);
}

// ORIG T 0x0811e2ce：恒返回 1（报告全文仅此）。
int CItemUpgrade_Separate::_GetPenaltyType(unsigned char /*a*/, ENUM_RARITY /*rarity*/,
                                           const GenuineDamageUpgradeTable& /*table*/,
                                           const GenuineDamageUpgradeInfo* /*info*/)
{
    return 1;
}

// ORIG T 0x0811e2e2。
bool CItemUpgrade_Separate::_GetUpgradeInfo(const Inven_Item* item, const CItem* citem,
                                            const GenuineDamageUpgradeTable& table,
                                            STUpgradeInfo* out, int level)
{
    const unsigned char sepLv =
        static_cast<unsigned char>(item->m_upgradeSep.GetUpgradeSeparate());
    if (!(sepLv > static_cast<unsigned char>(table.m_maxLevel)))
    {
        return false;
    }

    const unsigned char growth =
        static_cast<unsigned char>(
            reinterpret_cast<const CEquipItem*>(citem)->getGrowthGrade(level));
    const int rarity = citem->get_rarity();
    const int itemType = citem->GetItemType();
    const int materialCnt = table.getGenuineUpgradeMaterialCnt(
        growth,
        static_cast<int>(static_cast<unsigned char>(
            item->m_upgradeSep.GetUpgradeSeparate())),
        static_cast<ENUM_EQUIPMENTTYPE>(itemType),
        static_cast<ENUM_RARITY>(rarity));
    if (materialCnt < 0)
    {
        return false;
    }

    std::map<int, GenuineDamageUpgradeInfo>::const_iterator it =
        table.m_infos.find(
            static_cast<int>(static_cast<unsigned char>(
                item->m_upgradeSep.GetUpgradeSeparate())) + 1);
    if (it == table.m_infos.end())
    {
        return false;
    }

    out->m_materialCnt = materialCnt;
    // ORIG 0x0811e138：pt() 后 flds 0xc(%eax) = pair+0xc = value+0x8 = m_levelRate
    out->m_gold = static_cast<int>(kGoldBase * it->second.m_levelRate);
    return true;
}

// ORIG T 0x0811e138（const）。
int CItemUpgrade_Separate::_CheckCondition(const CUser* /*user*/, const Inven_Item& item,
                                           const CItem* citem, const Inven_Item& matItem,
                                           const GenuineDamageUpgradeTable& table,
                                           int level) const
{
    const unsigned char growth = static_cast<unsigned char>(
        reinterpret_cast<const CEquipItem*>(citem)->getGrowthGrade(level));

    std::map<int, GenuineDamageUpgradeMaterialInfo>::const_iterator mit =
        table.m_materials.find(static_cast<int>(growth));
    if (mit == table.m_materials.end())
    {
        return 1;
    }
    if (matItem.m_addInfo != mit->second.m_materialItemIdx)
    {
        return 0x16;
    }

    const unsigned int haveCount = matItem.m_addInfo2;         // Inven_Item +0x07（int，具名字段）

    const unsigned char sepLv =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());
    const int rarity = citem->get_rarity();
    const int itemType = citem->GetItemType();
    const int needCnt = const_cast<GenuineDamageUpgradeTable&>(table)
                            .getGenuineUpgradeMaterialCnt(growth, sepLv,
                                                          static_cast<ENUM_EQUIPMENTTYPE>(itemType),
                                                          static_cast<ENUM_RARITY>(rarity));
    if (haveCount < static_cast<unsigned int>(needCnt))
    {
        return 0x16;
    }
    if (itemType != 10)
    {
        return 0x13;
    }
    if (item.m_fieldb !=
        static_cast<unsigned short>(
            reinterpret_cast<const CEquipItem*>(citem)->get_endurance()))
    {
        return 7;
    }
    if (table.m_maxLevel < static_cast<int>(sepLv) + 1)
    {
        return 0x5f;
    }
    return 0;
}

// ORIG T 0x0811e468。
bool CItemUpgrade_Separate::_DoProcUpgrade(CUser* user, Inven_Item* item,
                                           const STUpgradeInfo* info)
{
    if (user->IsGameMasterMode())
    {
        if (user->getUpgradeSeparateTrueFlag() == 1)
        {
            item->m_upgradeSep.IncUpgradeSeparate();
            return true;
        }
        if (user->getUpgradeSeparateFlaseFlag() == 1)
        {
            return false;
        }
    }

    bool failed = false;
    if (info->m_gold != 0)
    {
        const unsigned int limit =
            static_cast<unsigned int>(info->m_gold * 10);
        unsigned long range = m_randRange;
        const unsigned int roll =
            static_cast<unsigned int>(m_rand->randInt(range));
        failed = limit < roll;
    }
    if (!failed)
    {
        item->m_upgradeSep.IncUpgradeSeparate();
    }
    return !failed;
}

// ORIG T 0x0811e53a：header 0x56 全服广播分离强化结果。
void CItemUpgrade_Separate::_NoticeUpgrade(const CUser* user, const Inven_Item& item,
                                           bool success)
{
    PacketGuard pg;
    pg.put_header(0, 0x56);
    pg.put_byte(10);
    // ORIG movzbl -0x1c,%eax 直装参：bool 隐式转 int 即零扩展（三目反生岛）。
    pg.put_byte(success);
    pg.put_short(user->get_unique_id());
    pg.put_int(item.m_addInfo);
    pg.put_byte(item.GetUpgrade());
    pg.put_byte(item.m_upgradeSep.GetUpgradeSeparate());
    // Inven_Item+0x25 = m_random(+0x1f).m_random（RandomOption，+0x06）。
    item.m_random.m_random.put_packet_random_option(pg);
    pg.finalize(true);
    G_GameWorld()->send_all(pg);
}

// ORIG T 0x0811e66e：vtable 槽 0 主流程。
int CItemUpgrade_Separate::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                                       PacketGuard* packet)
{
    CInventory* invenW = user->getCurCharacInvenW();

    Inven_Item item = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                           static_cast<short>(msg->m_targetSlot));  // MSG_ITEM_UPGRADE +0x1b
    if (item.isEmpty())
    {
        return 4;
    }
    if (item.m_addInfo !=        // Inven_Item +0x02
        msg->m_targetItemIdx)    // MSG_ITEM_UPGRADE +0x1d
    {
        return 4;
    }

    Inven_Item matItem = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                              static_cast<short>(msg->m_materialSlot));  // +0x23
    if (matItem.isEmpty())
    {
        return 0x16;
    }

    CDataManager* dataMgr = G_CDataManager();
    CItem* citem = dataMgr->find_item(item.m_addInfo);
    if (citem == 0)
    {
        return 4;
    }

    const GenuineDamageUpgradeTable* table = getUpgradeInfoTable();
    STUpgradeInfo info;
    const int level = user->get_charac_level();

    if (!_GetUpgradeInfo(&item, citem, *table, &info, level))
    {
        return 0xd;
    }

    const int cond = _CheckCondition(user, item, citem, matItem, *table, level);
    if (cond != 0)
    {
        return cond;
    }

    const unsigned char sepOld =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());

    const bool success = _DoProcUpgrade(user, &item, &info);

    const unsigned short matSlot = msg->m_materialSlot;   // MSG_ITEM_UPGRADE +0x23
    CInventory* invenW2 = user->getCurCharacInvenW();
    if (invenW2->delete_item(INVEN_TYPE_EQUIP, matSlot, info.m_materialCnt,
                             static_cast<eItemDelReason>(0xc), true) == 0)
    {
        return 0x11;
    }

    packet->put_byte(1);
    packet->put_short(matSlot);

    Inven_Item refreshed = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, matSlot);
    packet->put_int(refreshed.get_add_info());
    packet->put_byte(static_cast<char>(sepOld));

    if (static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate()) > 5)
    {
        _NoticeUpgrade(user, item, success);
    }

    packet->put_byte(success ? 0 : 1);

    const unsigned char sepNow =
        static_cast<unsigned char>(item.m_upgradeSep.GetUpgradeSeparate());
    packet->put_byte(sepNow);

    const unsigned short slot = msg->m_targetSlot;   // MSG_ITEM_UPGRADE +0x1b
    packet->put_short(slot);

    if (item.m_addInfo != 0)   // Inven_Item +0x02
    {
        invenW->update_item(INVEN_TYPE_EQUIP, slot, item);
    }

    user->m_historyLog.SeparateUpgradeItem(citem->get_index(), sepOld, sepNow, success);

    const char* guildName;
    if (!citem->IsRandomOption())
    {
        guildName = citem->GetItemName();
    }
    else
    {
        guildName = msg->m_28;   // MSG_ITEM_UPGRADE +0x28（guild 名串）
    }
    user->makeGuildItemSeparateUpgradMessage(guildName, sepOld, sepNow, success,
                                             item.GetUpgrade());
    return 0;
}

}  // namespace WongWork

// ============================ UpgradeSeparateInfo ============================

// ORIG W 0x0811ed32：低 5 位自增（模 32），保留高位（bit5=交易限制等）。
void UpgradeSeparateInfo::IncUpgradeSeparate()
{
    m_field = static_cast<unsigned char>((((m_field & 0x1f) + 1) & 0x1f) |
                                         (m_field & 0xe0));
}

// ============================ CUser 公会公告 ============================

// ORIG T 0x0867a242：成功且新等级>5 用 game_server_msg_248；失败且旧等级>5 用
// game_server_msg_249；前缀 game_server_msg_247 取 10 字节；长度<120 才发公会板。
void CUser::makeGuildItemSeparateUpgradMessage(const char* name, int oldLevel,
                                               int newLevel, bool success, int upgrade)
{
    char prefix[10];
    std::memset(prefix, 0, sizeof(prefix));
    char board[120];
    std::memset(board, 0, sizeof(board));

    const char* prefixStr =
        g_scriptStringManager_.findString(4, "game_server_msg_247",
                                          static_cast<bool*>(0));
    std::strncpy(prefix, prefixStr, sizeof(prefix));

    bool boardReady = false;
    if (success && newLevel > 5)
    {
        char msgBuf[255];
        std::strncpy(msgBuf,
                     g_scriptStringManager_.findString(4, "game_server_msg_248",
                                                       static_cast<bool*>(0)),
                     255);

        std::string userName(getCurCharacName());
        std::string line(prefix);
        std::string body(msgBuf);

        char tail[255];
        std::memset(tail, 0, sizeof(tail));
        if (upgrade == 0)
        {
            std::sprintf(tail, "%s %d", name, newLevel);
        }
        else
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, newLevel);
        }
        line += tail;
        line += body;
        userName += line;

        if (userName.length() < sizeof(board))
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
    }
    else
    {
        if (success)
        {
            return;
        }
        if (oldLevel <= 5)
        {
            return;
        }

        char msgBuf[255];
        std::strncpy(msgBuf,
                     g_scriptStringManager_.findString(4, "game_server_msg_249",
                                                       static_cast<bool*>(0)),
                     255);

        std::string userName(getCurCharacName());
        std::string line(prefix);
        std::string body(msgBuf);

        char tail[255];
        std::memset(tail, 0, sizeof(tail));
        if (upgrade == 0)
        {
            std::sprintf(tail, "%s %d", name, oldLevel + 1);
        }
        else
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, oldLevel + 1);
        }
        line += tail;
        line += body;
        userName += line;

        if (userName.length() < sizeof(board))
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
    }

    if (boardReady)
    {
        const int guildkey = get_charac_guildkey();
        const unsigned int characNo =
            static_cast<unsigned int>(getCurCharacNo());
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
        proxy->SendGuildBoardWrite(board, 0u, characNo, -1,
                                   static_cast<char>(-1),
                                   static_cast<unsigned int>(guildkey));
    }
}
