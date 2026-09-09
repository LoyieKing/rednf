// ============================================================================
// [b28d-38b 判词] 本 TU 9 件最终状态（check_tu_routed fresh，改前 132/262/0/10
// → 改后 132/267/0/5，净 +5 AE）：
//  checkValidShopItem 0x08512cda AE（bool 标志 + 空 then/else 守卫 + 条件处 xorb
//   （P168a④ 栈槽物化）+ 内层 while(bFlag) 单次体；与 ORIG 89/89 逐条 identical）
//  checkLimitNpcBuyItemList 0x08513580 AE（同构；逐条 identical）
//  IsOneADayShop 0x08513082 AE（ItemShopScript::m_isOneADay char→bool，
//   P292 消费端判据：ORIG 0x085130da `movzbl 0x48(%eax); test %al,%al` 无 setcc）
//  updateOneADayItemList 0x08512e98 AE（P227 全表达式临时链：ORIG 0x08512f1a 在
//   cMyTrace ctor 前把 it->second.m_shopId 载入 %ebx 跨 ctor 保活）
//  loadItemShopFiles 0x08512aa0 AE（importItemShopScript 返回类型 int→bool
//   （ORIG 0x08512b33 调用点 `xor $1`，P292）+ path 局部前置声明（ORIG 0x08512ab9））
//  setOneADayItemListNo 0x08512f9c / testupdateOneADayItemList 0x08512f88 / 
//   GetOneADayItemList 0x08513076 AE/identical
//  importItemShopScript 0x089dc5b4 DIFF（p167 CLEAR 59/59，仅 C1/C2·D1/D2 同址
//   别名 P93；残差 = P164 循环旋转 + P223 岛数（ORIG 循环头 2 个 nop 岛 / 尾部
//   5 nop+4 jmp vs 我方 4 nop+3 jmp）+ N82 槽复用（allocator -0xd vs -0x25，
//   字符串槽整体浅 4）⇒ 语义完成、byte 层 N 封顶）
//  _Z41__static_initialization_and_destruction_0ii N123 豁免（含共享头
//   DNFLex.h→DNFLex.h→<iostream> 引入的 _ZStL8__ioinit，ORIG 本 TU 无）
//  __uninitialized_copy ×3 N89/N130（STL 模板实例；move_iterator 版 67/54）
// df_game_r 还原 —— CItemShop（NPC 物品商店，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CItemShop.md + ORIG 反汇编逐函数核对。
// 布局见 CDataManager.h；ItemShopScript 布局依据 C1/clear/import 推导。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <ctime>
#include <memory>

#include "CDataManager.h"
#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "NpcScript.h"

namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ---- 跨类符号（ORIG 真实符号） ----

STScriptFileList g_itemShopScriptFileList;  // ORIG 全局（0x94fe260），原 GameStubs.cpp 定义迁移

// ORIG g_itemShopScriptBaseDirectory（B 0x094fe160[0x100]）：脚本目录全局
// 缓冲（initItemShopScript 0x89dc4f8 填充；本 TU 提供符号，空目录等价
// loadRDARScriptFile("", path)）。nm 权威名见 InitScriptProducers_5.cpp。
char g_itemShopScriptBaseDirectory[0x100];

// [b28d-38b] 返回类型 bool：ORIG loadItemShopFiles 0x08512b33 调用点
// `xor $0x1,%eax; test %al,%al; je`（P292：无 setcc ⇒ bool；原 int 生 test+sete）。
bool importItemShopScript(ItemShopScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_itemShopScriptBaseDirectory, path))
        return false;
    script->clear();
    std::string line;
    std::string line2;
    int value = 0;
    bool ok = false;
    while (ScanType(line, true))
    {
        if (line == "[NPC]")
        {
            script->m_limitCount = ScanInt(static_cast<bool*>(0));
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
            script->m_field0c = (char)(ScanInt(static_cast<bool*>(0)) != 0);
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
        else if ("[tab name]" == line)
        {
            std::string tab;
            while (ScanStr(&tab))
            {
                std::allocator<char> allocator;
                script->m_strList.push_back(
                    std::string(tab.c_str(), allocator));
            }
        }
        else if (line == "[one a day start time]")
        {
            time_t rawTime = time(0);
            struct tm* tmInfo = localtime(&rawTime);
            tmInfo->tm_year = ScanInt(&ok) - 1900;
            if (!ok)
                break;
            tmInfo->tm_mon = ScanInt(&ok) - 1;
            if (!ok)
                break;
            tmInfo->tm_mday = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_hour = ScanInt(&ok);
            if (!ok)
                break;
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
    if (script->m_dailyList.size() != 0)
        script->m_isOneADay = 1;
    return true;
}

// ---- loadItemShopFiles @ 0x8512aa0 ----
bool CItemShop::loadItemShopFiles()
{
    ItemShopScript script;
    // [b28d-38b] ORIG 0x08512ab9：path 槽清零发射在 ItemShopScript::C1 之后、
    // iterator 默认构造之前 ⇒ 声明序 script→path→it。
    const char* path = 0;
    std::map<int, std::string>::iterator it;
    it = g_itemShopScriptFileList.m_fileMap.begin();
    // [b28d-38] P167：ORIG 调 _Rb_tree_iterator::operator!=（我们原用 ==）⇒ 正极性 while。
    while (it != g_itemShopScriptFileList.m_fileMap.end())
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        if (!importItemShopScript(&script, path)) {
            LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__,
                                  0xc26, "ImportEquipmentScript() fail file - %s", path);
            return false;
        }
        script.m_shopId =
            g_itemShopScriptFileList.FindIndexByFullScan(path);
        if (script.m_isOneADay)
            setOneADayItemListNo(script);
        m_shopMap.insert(std::make_pair(script.m_shopId, script));
        ++it;
    }
    return true;
}

// ---- checkValidShopItem @ 0x8512cda ----
bool CItemShop::checkValidShopItem(unsigned int shopId, unsigned long itemIdx)
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.find((int)shopId);
    if (it == m_shopMap.end())
        return false;
    // [b28d-38b] ORIG 0x08512cda 形态：bool 标志预清 + 空 then/else 守卫（4.4 对
    // if(!b) 会多 movzbl+xor 归一化，空 then 形才复现 ORIG 的 cmpb/jne 尾跳布局）
    // + 外层条件处 xorb（P168a④ 栈槽物化）+ 内层 while(bFlag) 单次体。
    bool bFlag = false;
    if (bFlag)
    {
    }
    else
    {
        std::vector<int>::iterator vit = it->second.m_curItemList.begin();
        std::vector<int>::iterator vend = it->second.m_curItemList.end();
        while (vit != vend && (bFlag = !bFlag))
        {
            int* pItem = &*vit;
            while (bFlag)
            {
                if ((unsigned long)*pItem == itemIdx)
                    return true;
                bFlag = false;
            }
            vit++;
        }
    }
    return false;
}

// ---- checkLimitNpcBuyItemList @ 0x8513580 ----
bool CItemShop::checkLimitNpcBuyItemList(unsigned int shopId, unsigned long itemIdx)
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.find((int)shopId);
    if (it == m_shopMap.end())
        return false;
    // [b28d-38b] ORIG 0x08513580：与 checkValidShopItem 同构（xorb 栈槽物化 +
    // 空 then 守卫 + 内层 while 单次体）。
    bool bFlag = false;
    if (bFlag)
    {
    }
    else
    {
        std::vector<int>::iterator vit = it->second.m_limitNpcBuyList.begin();
        std::vector<int>::iterator vend = it->second.m_limitNpcBuyList.end();
        while (vit != vend && (bFlag = !bFlag))
        {
            int* pItem = &*vit;
            while (bFlag)
            {
                if ((unsigned long)*pItem == itemIdx)
                    return true;
                bFlag = false;
            }
            vit++;
        }
    }
    return false;
}

// ---- IsOneADayShop @ 0x8513082 ----
bool CItemShop::IsOneADayShop(int shopId) const
{
    std::map<int, ItemShopScript>::const_iterator it = m_shopMap.find(shopId);
    if (it != m_shopMap.end())
        return it->second.m_isOneADay;
    return 0;
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
    entry.second = 0;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = curSec - script.m_lastUpdateTime;
    if (diff >= 1) {
        int day = diff / 0x15180;
        if (day >= 0 && (int)script.m_dailyList.size() > day)
            entry.second = day;
    }
    m_oneADayList.push_back(entry);
    script.m_curItemList = script.m_dailyList.at(entry.second);
    return entry.second;
}

// ---- updateOneADayItemList @ 0x8512e98 ----
void CItemShop::updateOneADayItemList()
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.begin();
    std::map<int, ItemShopScript>::iterator end = m_shopMap.end();
    m_oneADayList.clear();
    while (it != end) {
        if (it->second.m_isOneADay) {
            unsigned int listNo = setOneADayItemListNo(it->second);
            // [b28d-38b] P227 全表达式临时链：ORIG 0x08512f1a 在 cMyTrace ctor 前把
            // it->second.m_shopId 载入 %ebx 跨 ctor 保活（具名 tr 形会被 4.4 改为 ctor
            // 后即时读取）。
            cMyTrace(__PRETTY_FUNCTION__, 0xc69, 0)(
                "updateOneADayItemList shop_id:%d list_no:%d", it->second.m_shopId,
                listNo);
        }
        ++it;
    }
}

// ---- testupdateOneADayItemList @ 0x8512f88 ----
void CItemShop::testupdateOneADayItemList()
{
    updateOneADayItemList();
}
