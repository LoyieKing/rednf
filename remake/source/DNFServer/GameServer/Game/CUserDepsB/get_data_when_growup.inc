bool stLevelUpRewardItemScript::get_data_when_growup(
    const LevelUpRewardItemTableKey& key,
    std::vector<LevelUpRewardItemData>& out)
{
    // b28d-solo-CUserDepsB: AE——ORIG 0x081b895a 是 std::for_each(begin,end,
    // GetGrowUpRewardItemFunctor(key,out)) 单语句（functor sret -0x18 / end -0x10 /
    // begin -0xc / for_each 返回临时 -0x20）+ `return !out.empty()`（xor $1）；
    // 手写三条件循环多 36 条（谓词逐条物化 + 三个 jmp/nop 岛）。
    std::for_each(m_map0.begin(), m_map0.end(),
                  GetGrowUpRewardItemFunctor(key, out));
    return !out.empty();
