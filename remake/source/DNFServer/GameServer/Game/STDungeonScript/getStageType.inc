// ---- advancealtar 字符串枚举解析器 ----
// ORIG: StageType::getStageType 0x88990ba / StageDifficulty::getDifficulty
// 0x8899038 / RewardItemType::getRewardItemType 0x889acaa。
bool advancealtar::StageType::getStageType(const std::string& name, T& out)
{
    // P101 同族：ORIG 每分支独立 `mov $1,%eax; jmp` 出口岛，须独立 return true。
    if (name == "normal")
    {
        out = STAGE_NORMAL;
        return true;
    }
    if (name == "survival")
    {
        out = STAGE_SURVIVAL;
        return true;
    }
    // [推断] ORIG 未识别时写 0 并仍返回 true（见报告反编译）。
    out = STAGE_NORMAL;
    return true;
}