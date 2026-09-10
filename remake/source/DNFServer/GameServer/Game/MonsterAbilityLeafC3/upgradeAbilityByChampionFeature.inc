void upgradeAbilityByChampionFeature(int elem, int roleType,
                                     std::vector<int>* features,
                                     ActiveStaticInfo* info)
{
    // ORIG 0x088b7c29 开头 12 条 mov $imm -> 栈槽 ⇒ 原码是**非 static** 局部
    // const 数组（栈上逐元素初始化），不是 .rodata 静态表。
    const float kElemRatio[4] = {0.0f, 0.3f, 0.5f, 0.0f};
    const float kAttackRatio[4] = {0.0f, 1.5f, 2.0f, 0.0f};
    const float kHpRatio[4] = {0.0f, 1.5f, 3.0f, 0.0f};

    // [L14 round2] P251/P286：ORIG 0x088b7c29 该段为
    // `mov 0x8(%ebp),%eax; test %eax,%eax; js; cmp $0x3,%eax; jle; cmp $0x4,%eax; je`
    // （形参载入寄存器一次 + 多路比较链，无栈槽往返）⇒ 原码是 switch(elem) 的
    // 0..3 合并 case + case 4，而非 if/else-if（4.4 对 if 链恒折 `cmpl $0,%ebp` 直比）。
    switch (elem)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    {
        const int oldValue = info->m_elemAttack[elem];
        // ORIG 0x088b7cbf..0x088b7d04：oldValue 与 ROUND 结果各占独立具名槽
        // （Ghidra local_14@-0x10 / local_10@-0xc，写回用后者），P0 槽-局部双向律。
        const int newValue = static_cast<int>(
            static_cast<float>(oldValue) +
            (150 - oldValue) * kElemRatio[roleType]);
        info->m_elemAttack[elem] = newValue;
        break;
    }
    case 4:
        info->m_attack =
            static_cast<int>(static_cast<float>(info->m_attack) *
                             kAttackRatio[roleType]);
        info->m_magicAttack =
            static_cast<int>(static_cast<float>(info->m_magicAttack) *
                             kAttackRatio[roleType]);
        break;
    default:
        break;
    }

    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      kHpRatio[roleType]);

    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2000d)))
        info->m_statBase3 =
            static_cast<int>(static_cast<float>(info->m_statBase3) * 1.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x3000e)))
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) +
            static_cast<float>(info->m_statBase4));  // fadd st,st
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2ff02)))
        info->m_statBase12 =
            static_cast<int>(static_cast<float>(info->m_statBase12) * 2.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2ff03)))
        info->m_statBase14 =
            static_cast<int>(static_cast<float>(info->m_statBase14) * 2.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x3000f)))
        info->m_statBase6 = static_cast<int>(
            static_cast<float>(info->m_statBase6) +
            static_cast<float>(info->m_statBase6));  // fadd st,st

    info->m_attr17Base[13] += 50;                    // +0xac
