// ============================================================================
// df_game_r 还原 —— CItemShop（NPC 物品商店，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CItemShop.md + ORIG 反汇编逐函数核对。
// 布局见 CDataManager.h；ItemShopScript 布局依据 C1/clear/import 推导。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"
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
};
extern STScriptFileList g_itemShopScriptFileList;
extern "C" int sub_STScriptFileList_FindIndexByFullScan(const STScriptFileList* self,
                                                        const char* path)
    asm("_ZNK16STScriptFileList19FindIndexByFullScanEPKc");
extern "C" int importItemShopScript(ItemShopScript* script, const char* path)
    asm("_Z20importItemShopScriptP14ItemShopScriptPKc");

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
            sub_STScriptFileList_FindIndexByFullScan(&g_itemShopScriptFileList, path);
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
