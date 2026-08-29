// ============================================================================
// df_game_r 还原 —— ScriptDepsB.h
//
// 脚本装载依赖 B 组（EtcParameter / GenuineUpgrade / SpPerLevel / TitleBook
// 域）的跨 TU 声明汇总。函数定义唯一出处 ScriptDepsB.cpp；ORIG 符号均经
// df_game_r nm 核实为真实 T/B 符号。
//
//   importEtcParameterScriptForServer  T 0x080966fa
//   importGenuineUpgradeTable          T 0x08912336
//   importSpPerLevelReferenceTable     T 0x08910505
//   importTitleSection                 T 0x08aa8d65
//   importRewards                      T 0x08aa8b79
//   getTitleCollection                 T 0x08aa94e1
//   loadAppendageType                  T 0x0889253b
//   loadValueType                      T 0x08892ac4
//   （loadRDARScriptListFile T 0x088bbd77 定义于 STScriptFileList.cpp，不在此）
//
// g_*Const_ 全局为 ORIG BSS 段真实符号（importEtcParameterScriptForServer
// 为其唯一写入点；伤害公式域后续 TU 经此处 extern 引用）。
// ============================================================================

#ifndef GAME_SCRIPT_DEPS_B_H_
#define GAME_SCRIPT_DEPS_B_H_

#include <string>
#include <vector>
struct stTitleSection;
struct stTitleBookInfo;
struct stTitleElement;
struct stTitleRewardBasis;
struct stSpPerLevelTable;

// ---- Etc/EtcParameter.etc 常量（ORIG B 0x093f5d10..0x093f5d83）----
extern float g_physicalDamageRateConst_;              // 0x093f5d10
extern float g_physicalDamageAddConst_;               // 0x093f5d14
extern float g_physicalReduceRateConst_;              // 0x093f5d18
extern float g_physicalReduceAddConst_;               // 0x093f5d1c
extern float g_physicalRandomBaseConst_;              // 0x093f5d20
extern float g_physicalRandomRateConst_;              // 0x093f5d24
extern float g_magicalDamageRateConst_;               // 0x093f5d28
extern float g_magicalDamageAddConst_;                // 0x093f5d2c
extern float g_magicalReduceRateConst_;               // 0x093f5d30
extern float g_magicalReduceAddConst_;                // 0x093f5d34
extern float g_magicalRandomBaseConst_;               // 0x093f5d38
extern float g_magicalRandomRateConst_;               // 0x093f5d3c
extern float g_commonElementBaseConst_;               // 0x093f5d40
extern float g_commonElementRateConst_;               // 0x093f5d44
extern float g_commonElementRateConstInPVP_;          // 0x093f5d48
extern float g_commonElementRateConstInLadderPVP_;    // 0x093f5d4c
extern float g_hpIncreaseRateConst_;                  // 0x093f5d5c
extern float g_hpRegenRateConst_;                     // 0x093f5d60
extern float g_mpIncreaseRateConst_;                  // 0x093f5d64
extern float g_mpRegenRateConst_;                     // 0x093f5d68
extern float g_activeStatusDamageRateConst_;          // 0x093f5d6c
extern float g_activeStatusDefenseRateConst_;         // 0x093f5d70
extern float g_physicalDefenseAddConst_;              // 0x093f5d74
extern float g_magicalDefenseAddConst_;               // 0x093f5d78
extern float g_hpRegenRateAddRateConst_;              // 0x093f5d7c
extern float g_mpRegenRateAddRateConst_;              // 0x093f5d80

// ---- 函数声明 ----
bool importEtcParameterScriptForServer(void);
    // _Z33importEtcParameterScriptForServerv

int importSpPerLevelReferenceTable(const char* path, stSpPerLevelTable& table);
    // _Z30importSpPerLevelReferenceTablePKcR17stSpPerLevelTable（0=成功）

bool importTitleSection(stTitleSection* section, int idx);
    // _Z18importTitleSectionP14stTitleSectioni（[/section]=false 结束）

int importRewards(std::vector<stTitleRewardBasis*>* out, std::string token);
    // _Z13importRewardsPSt6vectorIP18stTitleRewardBasisSaIS1_EESs
    // （ORIG 恒返回 1；调用方均忽略返回值，符号不含返回类型）

std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab);
    // _Z18getTitleCollectionP15stTitleBookInfoi（未命中返回 0）

int loadAppendageType(const std::string& token);
    // _Z17loadAppendageTypeRKSs（未匹配返回 -1）

int loadValueType(const std::string& token);
    // _Z13loadValueTypeRKSs（"+"=0、"%"=1、其他=2）

#endif  // GAME_SCRIPT_DEPS_B_H_
