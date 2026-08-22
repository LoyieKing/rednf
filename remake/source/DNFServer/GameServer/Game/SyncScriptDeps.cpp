// df_game_r Game/ —— sync_script.cpp 缺失依赖符号最小定义（2026-08-21）。
// sync_script.cpp 的多数外部依赖（ScanType/ScanInt/ScanStr/std::string*/loadRDARScriptFile/
// GetEscapeString/codePage）已由 DNFLexWrapperHelpers.cpp/DNFFunctionLib.cpp 等已入
// CMake TU 提供；本 TU 仅补齐 ORIG 中仍缺的少量符号（签名匹配 ORIG，符号 T）：
//   toMbcs(const char*)      ORIG 0x08adee98（恒等返回 src）
//   getCharacterJob(const char*)  ORIG 0x088be57e（作业名表查找）
//   APSystem::CSyncScript::InsertDescTable/TruncateDescTable
//   advancealtar::SyncScript::insertItemDescTable/truncateItemDescTable

#include <string.h>

#include <string>

#include "DBMgr.h"
#include "GlobalData.h"
#include "MySQL.h"
#include "SyncScriptDeps.h"

// ==================== 全局工具 ====================

// ORIG 0x08adee98 toMbcs(const char*)：仅返回 src（无需转换时恒等）。
const char* toMbcs(const char* src)
{
    return src;
}

// ORIG 0x08adeab0 toTString(const std::string&)：恒等返回内部缓冲的 const char*（TCHAR*）。
const char* toTString(const std::string& src)
{
    return src.c_str();
}

// ORIG 0x088be57e getCharacterJob(const char*)：
// 按 ORIG 0x93f4d40 作业名表（"[swordman]"…"[creator mage]"）顺序 strcmp，
// 命中返回下标 0..10，未命中返回 11。
int getCharacterJob(const char* name)
{
    static const char* const kJobTable[11] = {
        "[swordman]",        // 0
        "[fighter]",         // 1
        "[gunner]",          // 2
        "[mage]",            // 3
        "[priest]",          // 4
        "[at gunner]",       // 5
        "[thief]",           // 6
        "[at fighter]",      // 7
        "[at mage]",         // 8
        "[demonic swordman]",// 9
        "[creator mage]"     // 10
    };
    for (int i = 0; i <= 0xa; ++i)
    {
        if (strcmp(name, kJobTable[i]) == 0)
        {
            return i;
        }
    }
    return 0xb;
}

// ==================== APSystem::CSyncScript desc ====================

// ORIG 0x0812262a：GetDBHandle(2,0) → "trUncate table charac_action_point_desc"。
bool APSystem::CSyncScript::TruncateDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_action_point_desc");
    return db->exec(true) == 1;
}

// ORIG 0x0812268c：遍历 CDataManager 内 CActionPointEx 全局 map，逐行
// INSERT charac_action_point_desc(action_index, action_group_index,
// action_group_name)。该数据源（APSystem CActionPointEx 全局表）尚未建模，
// 暂以空实现占位（保持符号 T）；对应 TruncateDescTable 已真实实现。
bool APSystem::CSyncScript::InsertDescTable()
{
    return true;
}

// ==================== advancealtar::SyncScript desc ====================

#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#include "CDataManager.h"
#include "DNFFunctionLib.h"  // CodePage::script2Database

namespace advancealtar
{
// BuyUpgradeData / BuyShopData 权威定义在 CDataManager.h（namespace advancealtar）。

// 注：SyncScript 类声明见 SyncScriptDeps.h；getItemValueStirng 为其 static 内部
// 辅助（非 ORIG 独立符号，仅本 TU 使用）。

// ORIG 0x08133e44：GetDBHandle(3,0) → "trUncate table charac_advance_altar_item_desc"。
bool SyncScript::truncateItemDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_advance_altar_item_desc");
    return db->exec(true) == 1;
}

// ORIG 0x08134132：见头注。type==1→+0x10 vec；type==2→+0x1c vec；type==0→+0x04 vec；
// 其它 type 直接返回 0。逐项 CodePage::script2Database 转换 desc 后 sprintf 元组，
// 项间追加 ", "；任一项转换失败返回 0（ORIG 走错误日志路径）。
int SyncScript::getItemValueStirng(int ridableId, const BuyShopData& data,
                                   AdvanceAltarShopType::T type, std::string& out)
{
    const std::vector<BuyUpgradeData>* vec = 0;
    switch (type)
    {
    case AdvanceAltarShopType::T_1: vec = &data.m_vecT1; break;
    case AdvanceAltarShopType::T_2: vec = &data.m_vecT2; break;
    case AdvanceAltarShopType::T_0: vec = &data.m_vecT0; break;
    default: return 0;
    }

    char tuple[1024];
    char conv[1024];
    for (int i = 0; i < (int)vec->size(); ++i)
    {
        const BuyUpgradeData& item = (*vec)[i];
        memset(conv, 0, sizeof(conv));
        if (CodePage::script2Database((char*)item.m_strDesc.c_str(), conv))
        {
            sprintf(tuple, "(%d,%d,%d,'%s')", ridableId, (int)type,
                    item.m_nItemId, conv);
            out += tuple;
            if (i < (int)vec->size() - 1)
            {
                out += ", ";
            }
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

// ORIG 0x08133ea6：遍历 G_CDataManager()+0x631c 的 map<int,BuyShopData>
// （advancealtar::AdvanceAltarShopParameter m_altarShop 的 +0 表），对每个
// ridable_id 依次生成 type 0/1/2 的 values 元组（项间以 "," 分隔），拼装
// "inSert into charac_advance_altar_item_desc (ridable_id, item_type,
// item_id, item_desc) values " 后 INSERT；任一行失败返回 0，全部成功返回 1。
bool SyncScript::insertItemDescTable()
{
    CDataManager* dm = G_CDataManager();
    if (dm == 0)
    {
        return false;
    }

    // ORIG：数据源位于 CDataManager+0x631c（m_altarShop 占位对象 +0）。
    // [推断] CDataManager.h 中 m_altarShop 为 pad[0x30] 占位；此处按 ORIG
    // 偏移访问其 +0 的 map<int,BuyShopData>（importAdvanceAltarShopParameter
    // 填充）。若该表未填充，循环体不会执行，返回 true（与 ORIG 空表行为一致）。
    typedef std::map<int, BuyShopData> BuyShopDataMap;
    BuyShopDataMap* shopMap =
        reinterpret_cast<BuyShopDataMap*>(reinterpret_cast<char*>(dm) + 0x631c);

    std::string query;
    std::string values;
    for (BuyShopDataMap::const_iterator it = shopMap->begin();
         it != shopMap->end(); ++it)
    {
        query =
            "inSert into charac_advance_altar_item_desc \t\t\t\t  "
            "(ridable_id, item_type, item_id, item_desc) values ";
        values.clear();
        if (getItemValueStirng(it->first, it->second,
                               AdvanceAltarShopType::T_0, values) == 0)
        {
            return false;
        }
        values += ",";
        if (getItemValueStirng(it->first, it->second,
                               AdvanceAltarShopType::T_1, values) == 0)
        {
            return false;
        }
        values += ",";
        if (getItemValueStirng(it->first, it->second,
                               AdvanceAltarShopType::T_2, values) == 0)
        {
            return false;
        }
        query += values;

        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
        db->set_query(query.c_str());
        if (db->exec(true) != 1)
        {
            return false;
        }
    }
    return true;
}
}  // namespace advancealtar