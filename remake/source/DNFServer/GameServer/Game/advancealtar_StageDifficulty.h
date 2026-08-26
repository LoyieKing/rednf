#ifndef DNF_ADVANCEALTAR_STAGE_DIFFICULTY_H_
#define DNF_ADVANCEALTAR_STAGE_DIFFICULTY_H_

// ============================================================================
// advancealtar_StageDifficulty.h —— advancealtar::StageDifficulty 权威叶子头
//
// 自 CDungeon.h 提取（2026-08-24，Prod_GlobalAdv）：CGameManager.h 的
// StageControl::selectStage 声明（ORIG T 0x0812fa7a，mangled 参数
// NS_15StageDifficulty1TE）需要完整枚举类型，而 CGameManager.h 不能反向
// include CDungeon.h（重入环）。本头仅依赖 <string>，双方均可安全包含。
// getDifficulty 实现 STDungeonScript.cpp（ORIG 0x8899038）。
// ============================================================================

// ---- StageType：ORIG getStageType 0x88990ba（自 CDungeon.h 收敛，2026-08-24
// GlobalBossAndAltar；CDungeon.h 改为引用本叶头，枚举值名保持不变）----
#include <string>

namespace advancealtar
{

class StageType
{
public:
    enum T
    {
        STAGE_NORMAL = 0,
        STAGE_SURVIVAL = 1
    };
    static bool getStageType(const std::string& name, T& out);
};

}  // namespace advancealtar

namespace advancealtar
{

class StageDifficulty
{
public:
    enum T
    {
        DIFF_EASY = 0,
        DIFF_MEDIUM = 1,
        DIFF_HARD = 2
    };
    // ORIG T 0x889900a：T(diff) 0→1、1→2、-1→0、其它原值。
    // AltarMisc 批次定义于 advancealtar_misc.cpp。
    static T getNextDifficulty(T diff);
    static bool getDifficulty(const std::string& name, T& out);
};

}  // namespace advancealtar

#endif  // DNF_ADVANCEALTAR_STAGE_DIFFICULTY_H_
