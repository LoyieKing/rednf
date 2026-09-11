// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <ctime>
#include <memory>

#include "CDataManager.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "NpcScript.h"

// [b28d-38b] 不 include DNFLexWrapper.h：其链上 DNFLex.h 含 <iostream>，会使本 TU
// 发射 ORIG 本 TU 无有的 _ZStL8__ioinit（SIAD 0x69 vs ORIG 0x40，P338 反例）。
// 改用局部权威签名声明（mangled 与 ORIG 一致，定义在 DNFLex TU）。
#include "CItemShop/ScanType.inc"
#include "CItemShop/ScanInt.inc"
#include "CItemShop/ScanStr.inc"

namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ---- 跨类符号（ORIG 真实符号） ----

// [L32 GI 调序] ORIG `_GLOBAL__I_g_itemShopScriptBaseDirectory`（CU=RDARScriptItemShop.cpp）
//   ⇒ 本 TU 首个强符号须为 g_itemShopScriptBaseDirectory。仅交换两块定义的先后：
//   零语义/零布局改动（g_itemShopScriptFileList 仍是本 TU 唯一动态初始化对象，
//   相对构造序不变，P327）。
// ORIG g_itemShopScriptBaseDirectory（B 0x094fe160[0x100]）：脚本目录全局
// 缓冲（initItemShopScript 0x89dc4f8 填充；本 TU 提供符号，空目录等价
// loadRDARScriptFile("", path)）。nm 权威名见 InitScriptProducers_5.cpp。
char g_itemShopScriptBaseDirectory[0x100];

STScriptFileList g_itemShopScriptFileList;  // ORIG 全局（0x94fe260），原 GameStubs.cpp 定义迁移

// [b28d-38b] 返回类型 bool：ORIG loadItemShopFiles 0x08512b33 调用点
// `xor $0x1,%eax; test %al,%al; je`（P292：无 setcc ⇒ bool；原 int 生 test+sete）。
#include "CItemShop/importItemShopScript.inc"
{
    if (!loadRDARScriptFile(g_itemShopScriptBaseDirectory, path))
        return false;
    script->clear();
    // [FragC 试形·P232] ORIG 局部槽基比 4.4 深 4（line -0x18 vs -0x14 等）⇒
    // 补一个 4B 占槽死局部（数组维决定 sub，P232 实证不被 DCE）。无效即回滚。
    char framePad4[4];
    (void)framePad4;
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
#include "CItemShop/loadItemShopFiles.inc"
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
#include "CItemShop/checkValidShopItem.inc"
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
#include "CItemShop/checkLimitNpcBuyItemList.inc"
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
#include "CItemShop/IsOneADayShop.inc"
{
    std::map<int, ItemShopScript>::const_iterator it = m_shopMap.find(shopId);
    if (it != m_shopMap.end())
        return it->second.m_isOneADay;
    return 0;
}

// ---- GetOneADayItemList @ 0x8513076 ----
#include "CItemShop/GetOneADayItemList.inc"
{
    return const_cast<CItemShop*>(this)->m_oneADayList;
}

// ---- setOneADayItemListNo @ 0x8512f9c ----
#include "CItemShop/setOneADayItemListNo.inc"
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
#include "CItemShop/updateOneADayItemList.inc"
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
#include "CItemShop/testupdateOneADayItemList.inc"
{
    updateOneADayItemList();
}
