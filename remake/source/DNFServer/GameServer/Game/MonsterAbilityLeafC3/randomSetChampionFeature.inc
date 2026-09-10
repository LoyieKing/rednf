void randomSetChampionFeature(int seed, int& count, std::vector<int>& temp,
                              int diff)
{
    std::srand(static_cast<unsigned int>(seed));

    if (diff > 3)
    {
        // [D07 试形3 · Main 轴A（cv 限定符）] 去 `const` 实测**逐字节相同**（本处值是
        // `CHAMPION_FEATURE_LIST[...]` 运行时读，非编译期常量 ⇒ 无常量传播可阻）；
        // 轴 A 适用于「我方把合计算折成立即数」的件，本形不属 ⇒ 回退维持 const。
        const int f = CHAMPION_FEATURE_LIST[rand() % 7 + 0x1c];
        addChampionFeature(static_cast<CHAMPION_FEATURES>(f), count, temp);
        return;
    }

    // ORIG 0x088b9dcf：do-while（体尾 subl/cmpl/setg/test/jne）+ 六路
    // continue 型 nop 岛（P223②/P51）；过滤判定为纯跳转链，无 bool 槽。
    int remain = 0xe74;
    do
    {
        if (count <= 1)
            return;

        // [试形记录] `register int f` 实测与无 register 逐字节相同（P307 提示对本形
        // 零效）⇒ 维持普通具名局部；cmp 形态差归 P209/N18b 封顶（见函数头判词）。
        // [D07 试形2 · C05 F2 族] 局部类型改 `long`（i386 仍 32 位、语义等价）实测
        // 逐字节相同 ⇒ 本条残差非「局部类型→装载形式」族（F2 对本形不适用，因比较
        // RHS 是立即数 `cmp $imm`，非 F2 的「局部 vs 局部」形），维持 P209/N18b。
        const int f = CHAMPION_FEATURE_LIST[rand() % 28];   // [D07 试形3：去 const 无效]
        if (f > 0x30007)
            goto BIG;
        if (f >= 0x30004)
            goto SKIP1;
        if (f == 0x1ff00)
            goto SKIP2;
        if (f == 0x20013)
            goto SKIP3;
        if (f == 0xff01)
            goto SKIP4;
        goto ADD;
    BIG:
        if (f < 0x3fe09)
            goto ADD;
        if (f <= 0x3fe0c)
            goto SKIP5;
        if (f == 0x40008)
            goto SKIP6;
    ADD:
        addChampionFeature(static_cast<CHAMPION_FEATURES>(f), count, temp);
        goto NEXT;
    SKIP1:
        goto NEXT;
    SKIP2:
        goto NEXT;
    SKIP3:
        goto NEXT;
    SKIP4:
        goto NEXT;
    SKIP5:
        goto NEXT;
    SKIP6:
        goto NEXT;
    NEXT: ;
    } while (--remain > 0);
