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
// ORIG 0x08adf024：char const* 直拷到 sDst。
void toMbcs(const char* pszSrc, std::string& sDst);
// ORIG 0x08adef49：string 拷贝到 sDst。
void toMbcs(const std::string& sSrc, std::string& sDst);
// ORIG 0x08adeea0：wchar_t const* 经 TLS Mbcs 环形缓冲 wcstombs 转多字节。
const char* toMbcs(const wchar_t* pszSrc);
// ORIG 0x08adede6：wstring 经 TLS Mbcs 环形缓冲。
const char* toMbcs(const std::wstring& sSrc);
// ORIG 0x08adf03e：wchar_t const* 经 TLS Mbcs 缓冲后写入 sDst。
void toMbcs(const wchar_t* pszSrc, std::string& sDst);
// ORIG 0x08adef63：wstring 经 TLS Mbcs 缓冲后写入 sDst。
void toMbcs(const std::wstring& sSrc, std::string& sDst);
// ORIG 0x08aded01：char const* 直拷到 sDst。
void toTString(const char* pszSrc, std::string& sDst);
// ORIG 0x08adec26：string 拷贝到 sDst。
void toTString(const std::string& sSrc, std::string& sDst);
// ORIG 0x08adeb7d：wchar_t const* 经 TLS TChar 环形缓冲。
const char* toTString(const wchar_t* pszSrc);
// ORIG 0x08adeac3：wstring 经 TLS TChar 环形缓冲。
const char* toTString(const std::wstring& sSrc);
// ORIG 0x08aded1b：wchar_t const* 经 TLS TChar 缓冲后写入 sDst。
void toTString(const wchar_t* pszSrc, std::string& sDst);
// ORIG 0x08adec40：wstring 经 TLS TChar 缓冲后写入 sDst。
void toTString(const std::wstring& sSrc, std::string& sDst);

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
#ifndef DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED
// 幂等副本（struct enum-T 权威建模）：主定义见 AdvanceAltarField.h，与
// advancealtar.h 一致。原 namespace 版为错误建模已删除。
struct AdvanceAltarShopType { enum T { T_0 = 0, T_1 = 1, T_2 = 2 }; };
#endif

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
