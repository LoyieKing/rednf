// ============================================================================
// df_game_r 还原 —— CItemShop（NPC 物品商店，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CItemShop.md + ORIG 反汇编逐函数核对。
// 布局见 CDataManager.h；ItemShopScript 布局依据 C1/clear/import 推导。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <ctime>

#include "CDataManager.h"
#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "CSystemTime.h"

namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ---- 跨类符号（ORIG 真实符号） ----
class STScriptFileList
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00
    int FindIndexByFullScan(const char* path) const;  // 定义于 STScriptFileList.cpp
};
STScriptFileList g_itemShopScriptFileList;  // ORIG 全局（0x94fe260），原 GameStubs.cpp 定义迁移

// ORIG 0x89dc5b4：脚本目录全局缓冲（initItemShopScript 0x89dc4f8 填充；
// 本 TU 提供符号，空目录等价 loadRDARScriptFile("", path)）。
char g_itemShopScriptDir[0x100];

// ORIG 0x8514046：清零标量字段、清空容器；m_lastUpdateTime 保留旧值。
void ItemShopScript::clear()
{
    m_shopId = 0;
    m_limitType = 0;
    m_limitCount = 0;
    m_field0c = 0;
    m_shopName = "";
    m_curItemList.clear();
    m_strList.clear();
    m_limitNpcBuyList.clear();
    m_dailyList.clear();
    m_isOneADay = 0;
}

int importItemShopScript(ItemShopScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_itemShopScriptDir, path))
        return 0;
    script->clear();
    std::string line;
    std::string line2;
    int value = 0;
    bool ok = false;
    while (ScanType(line, true))
    {
        if (line == "[NPC]")
        {
            script->m_limitCount = ScanInt(&ok);
        }
        else if (line == "[message]")
        {
            ScanStr(&script->m_shopName);
        }
        else if (line == "[type]")
        {
            ScanStr(&line2);
            if (line2 == "[etc shop]")
                script->m_limitType = 0;
            else if (line2 == "[weapon shop]")
                script->m_limitType = 1;
            else if (line2 == "[disjoint shop]")
                script->m_limitType = 2;
        }
        else if (line == "[only buy]")
        {
            script->m_field0c = (char)(ScanInt(&ok) != 0);
        }
        else if (line == "[sell item]")
        {
            script->m_curItemList.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_curItemList.push_back(value);
            }
        }
        else if (line == "[sell limit item]")
        {
            script->m_limitNpcBuyList.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_limitNpcBuyList.push_back(value);
            }
        }
        else if (line == "[tab name]")
        {
            std::string tab;
            while (ScanStr(&tab))
                script->m_strList.push_back(tab);
        }
        else if (line == "[one a day start time]")
        {
            time_t rawTime = time(0);
            struct tm* tmInfo = localtime(&rawTime);
            value = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_year = value - 1900;
            value = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_mon = value - 1;
            value = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_mday = value;
            value = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_hour = value;
            tmInfo->tm_min = 0;
            tmInfo->tm_sec = 0;
            script->m_lastUpdateTime = (int)mktime(tmInfo);
        }
        else if (line == "[one a day item]")
        {
            std::vector<int> items;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                items.push_back(value);
            }
            script->m_dailyList.push_back(items);
        }
    }
    script->m_shopId =
        g_itemShopScriptFileList.FindIndexByFullScan(path);
    if (!script->m_dailyList.empty())
        script->m_isOneADay = 1;
    return 1;
}

// ============================================================================
// 构造（ORIG 0x8374e76，weak）
// ============================================================================
CItemShop::CItemShop()
{
    // map/vector 默认构造
}

// ---- loadItemShopFiles @ 0x8512aa0 ----
bool CItemShop::loadItemShopFiles()
{
    ItemShopScript script;
    for (std::map<int, std::string>::iterator it =
             g_itemShopScriptFileList.m_fileMap.begin();
         it != g_itemShopScriptFileList.m_fileMap.end(); ++it) {
        const char* path = it->second.c_str();
        if (path == 0)
            break;
        if (!importItemShopScript(&script, path)) {
            LogManager::logFormat(1, "item.cpp", "bool CItemShop::loadItemShopFiles()",
                                  0xc26, "ImportEquipmentScript() fail file - %s", path);
            return false;
        }
        script.m_shopId =
            g_itemShopScriptFileList.FindIndexByFullScan(path);
        if (script.m_isOneADay != 0)
            setOneADayItemListNo(script);
        m_shopMap.insert(std::make_pair(script.m_shopId, script));
    }
    return true;
}

// ---- checkValidShopItem @ 0x8512cda ----
bool CItemShop::checkValidShopItem(unsigned int shopId, unsigned long itemIdx)
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.find((int)shopId);
    if (it == m_shopMap.end())
        return false;
    const std::vector<int>& list = it->second.m_curItemList;
    for (std::vector<int>::const_iterator vit = list.begin(); vit != list.end(); ++vit) {
        if ((unsigned long)*vit == itemIdx)
            return true;
    }
    return false;
}

// ---- checkLimitNpcBuyItemList @ 0x8513580 ----
bool CItemShop::checkLimitNpcBuyItemList(unsigned int shopId, unsigned long itemIdx)
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.find((int)shopId);
    if (it == m_shopMap.end())
        return false;
    const std::vector<int>& list = it->second.m_limitNpcBuyList;
    for (std::vector<int>::const_iterator vit = list.begin(); vit != list.end(); ++vit) {
        if ((unsigned long)*vit == itemIdx)
            return true;
    }
    return false;
}

// ---- IsOneADayShop @ 0x8513082 ----
char CItemShop::IsOneADayShop(int shopId) const
{
    std::map<int, ItemShopScript>::const_iterator it = m_shopMap.find(shopId);
    if (it == m_shopMap.end())
        return 0;
    return it->second.m_isOneADay;
}

// ---- GetOneADayItemList @ 0x8513076 ----
std::vector<std::pair<int, int> >& CItemShop::GetOneADayItemList() const
{
    return const_cast<CItemShop*>(this)->m_oneADayList;
}

// ---- setOneADayItemListNo @ 0x8512f9c ----
unsigned int CItemShop::setOneADayItemListNo(ItemShopScript& script)
{
    std::pair<int, int> entry;
    entry.first = script.m_shopId;
    unsigned int listNo = 0;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = curSec - script.m_lastUpdateTime;
    if (diff >= 1) {
        unsigned int day = (unsigned int)(diff / 0x15180);
        if ((int)day >= 0 && (int)script.m_dailyList.size() > (int)day)
            listNo = day;
    }
    m_oneADayList.push_back(entry);
    script.m_curItemList = script.m_dailyList.at(listNo);
    return listNo;
}

// ---- updateOneADayItemList @ 0x8512e98 ----
void CItemShop::updateOneADayItemList()
{
    m_oneADayList.clear();
    for (std::map<int, ItemShopScript>::iterator it = m_shopMap.begin();
         it != m_shopMap.end(); ++it) {
        if (it->second.m_isOneADay != 0) {
            unsigned int listNo = setOneADayItemListNo(it->second);
            cMyTrace tr("void CItemShop::updateOneADayItemList()", 0xc69, 0);
            tr("updateOneADayItemList shop_id:%d list_no:%d", it->second.m_shopId,
               listNo);
        }
    }
}

// ---- testupdateOneADayItemList @ 0x8512f88 ----
void CItemShop::testupdateOneADayItemList()
{
    updateOneADayItemList();
}
