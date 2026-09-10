void makeChampionFeatureList(std::vector<int>& features, int level, int seed,
                             std::vector<int>& temp, int diff)
{
    int count =
        static_cast<int>(static_cast<float>(level - 1) * 0.2f + 2.0f);

    // [b28c-lane24] ORIG 0x088ba0fd：`call size(); test %eax,%eax; sete %al;
    // test %al,%al; je`（P197④ call 结果寄存器直比物化）；empty() 在 4.4-O0
    // 生 out-of-line empty 调用且无 sete 段 ⇒ 改显式 size()==0。
    if (features.size() == 0)
        randomSetChampionFeature(seed, count, temp, diff);
    else
        scriptSetChampionFeature(features, count, temp);
