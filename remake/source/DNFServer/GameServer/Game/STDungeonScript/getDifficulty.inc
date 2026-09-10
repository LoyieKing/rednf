bool advancealtar::StageDifficulty::getDifficulty(const std::string& name,
                                                   T& out)
{
    // P63 反向：ORIG 各成功分支独立 `mov$1;jmp` 尾岛，return false 独占尾块。
    if (name == "easy")
    {
        out = DIFF_EASY;
        return true;
    }
    if (name == "medium")
    {
        out = DIFF_MEDIUM;
        return true;
    }
    if (name == "hard")
    {
        out = DIFF_HARD;
        return true;
    }
    return false;
