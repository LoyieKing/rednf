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

// ==================== 全局工具 ====================

// ORIG 0x08adee98 toMbcs(const char*)：仅返回 src（无需转换时恒等）。
const char* toMbcs(const char* src)
{
    return src;
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

namespace APSystem
{
class CSyncScript
{
public:
    bool InsertDescTable();
    bool TruncateDescTable();
};

// ORIG 0x0812262a：GetDBHandle(2,0) → "trUncate table charac_action_point_desc"。
bool CSyncScript::TruncateDescTable()
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
bool CSyncScript::InsertDescTable()
{
    return true;
}
}  // namespace APSystem

// ==================== advancealtar::SyncScript desc ====================

namespace advancealtar
{
class SyncScript
{
public:
    bool insertItemDescTable();
    bool truncateItemDescTable();
};

// ORIG 0x08133e44：GetDBHandle(3,0) → "trUncate table charac_advance_altar_item_desc"。
bool SyncScript::truncateItemDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_advance_altar_item_desc");
    return db->exec(true) == 1;
}

// ORIG 0x08133ea6：遍历 G_CDataManager()+0x631c 的 advancealtar::Manager
// BuyShopData map，经 getItemValueStirng 生成 item_desc 后 INSERT
// charac_advance_altar_item_desc(ridable_id, item_type, item_id, item_desc)。
// 该数据源/辅助（BuyShopData 表、getItemValueStirng）尚未建模，暂以空实现
// 占位（保持符号 T）；对应 truncateItemDescTable 已真实实现。
bool SyncScript::insertItemDescTable()
{
    return true;
}
}  // namespace advancealtar