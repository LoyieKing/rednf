// ---- generateEquipGrade — ORIG T 0x08533932 -------------------------------
// grades {0x5aa7,0xfa6,0x6a23,0x5df0,0x7270}；weights 全 1000，total=5000；
// w[0] 截断缩放后 total 修正为 5000-w0+w0'，randInt(&total) 落累计权重槽。
int GenerateRandomUpgrade::generateEquipGrade(int levelDelta)
{
    // [W16C 符号名轴·2026-09-11] ORIG 无
    //   `_ZZN8WongWork21GenerateRandomUpgrade18generateEquipGradeEiE6grades` 实体：
    //   ORIG 0x08533938..0x0853395a 为 5 条 `mov DWORD PTR [ebp-0x44+i*4],<imm>`
    //   （**栈上局部数组**、非 static）⇒ 去 `static const`；OUR-only −1、件级 ndiff 14→12。
    int grades[5] = { 0x5aa7, 0xfa6, 0x6a23, 0x5df0, 0x7270 };
    int weights[5] = { 1000, 1000, 1000, 1000, 1000 };
    int total = 5000;

    // P167/§4.1：ORIG 直接 G_CDataManager()+0x540 取 rates，无 equipGradeRates helper。
    // ORIG 空检用 081b48ac=vector::size()（+0x066 sete / +0x08c setne），operator[] 非 const
    // ⇒ rates 去 const、`empty()`→`size()==0`（P197④/P175）。
    std::vector<std::pair<int, float> >* rates =
        reinterpret_cast<std::vector<std::pair<int, float> >*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x540);
    if (rates->size() == 0)
        return 0x5aa7;
    if (rates->size() == 0)                  // ORIG 双重空检照抄
        return 0x5aa7;

    // P167/§4.1：ORIG 内联三元+循环查表，无 lookupGradeRatio helper
    float factor = 1.0f;
    int last = static_cast<int>(rates->size()) - 1;
    if ((*rates)[0].first < levelDelta)
        factor = (*rates)[0].second;
    else if ((*rates)[last].first > levelDelta)
        factor = (*rates)[last].second;
    else
    {
        for (int i = 0; i <= last; ++i)
        {
            if ((*rates)[i].first == levelDelta)
                factor = (*rates)[i].second;
        }
    }

    int weightOld = weights[0];
    weights[0] = static_cast<int>(static_cast<float>(weights[0]) * factor);
    total = total - weightOld + weights[0];

    int roll = m_rand->randInt(static_cast<const unsigned long&>(total));

    int acc = 0;
    for (int i = 0; i <= 4; ++i)
    {
        acc += weights[i];
        if (acc >= roll)                     // acc < roll 继续（jl）
            return grades[i];
    }
    return 0x5aa7;                           // 全 miss 兜底（ORIG 同样返回首个档位）
}