#ifndef GAME_SYNCSCRIPTDEPS_H_
#define GAME_SYNCSCRIPTDEPS_H_

// ============================================================================
// df_game_r sync_script.cpp 缺失依赖符号 —— 唯一声明点
// 实现见 SyncScriptDeps.cpp（符号 T，ORIG 证据见文件头注）。
// ============================================================================

#include <string>

// ORIG 0x08adee98：TCHAR 转多字节，失败时返回原指针（恒等）。
const char* toMbcs(const char* src);
// ORIG 0x08adedd3：std::string 恒等 c_str()。
const char* toMbcs(const std::string& sSrc);
// ORIG 0x08adeab0：tstring 恒等 c_str()。
const char* toTString(const std::string& sSrc);
// ORIG 0x08adeb75：char const* 恒等返回。
const char* toTString(const char* pszSrc);

// ORIG 0x088be57e：按作业名表查找返回下标 0..10，未命中 11。
int getCharacterJob(const char* name);
// ORIG 0x088be5c5：作业名表命中检查（与 getCharacterJob 同表），命中 1 否则 0。
int checkValiedJobName(const char* name);

namespace APSystem
{
// ORIG 调用点（insert_charac_action_point_desc 0x8625db8）call 前无 ECX/this
// 压栈 → static 成员；实现仅用全局 DB handle，不读 this。
class CSyncScript
{
public:
    static bool InsertDescTable();    // ORIG 0x0812268c
    static bool TruncateDescTable();  // ORIG 0x0812262a
};
}  // namespace APSystem

namespace advancealtar
{
namespace AdvanceAltarShopType
{
enum T
{
    T_0 = 0,
    T_1 = 1,
    T_2 = 2
};
}

class BuyShopData;

// ORIG 调用点（insert_charac_advance_altar_item_desc 0x8625dfc）call 前无
// ECX/this 压栈 → static 成员；实现仅用全局 DB handle，不读 this。
class SyncScript
{
public:
    static bool insertItemDescTable();    // ORIG 0x08133ea6
    static bool truncateItemDescTable();  // ORIG 0x08133e44

    // ORIG 0x08134132：生成 "(%d,%d,%d,'%s')" 元组追加到 out，成功返回 1。
    static int getItemValueStirng(int ridableId, const BuyShopData& data,
                                  AdvanceAltarShopType::T type, std::string& out);
};
}  // namespace advancealtar

#endif  // GAME_SYNCSCRIPTDEPS_H_
