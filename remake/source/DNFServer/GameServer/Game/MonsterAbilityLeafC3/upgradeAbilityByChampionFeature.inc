// ---------------------------------------------------------------------------
// upgradeAbilityByChampionFeature — ORIG 0x88b7c29。
// 角色系数表为函数内静态初值（ORIG 栈上立即数构造，与 ORIG 数值一致）：
//   elem 0..3：elemAttack[e] += (150-old)*ratioA[role]
//   elem == 4：attack/magicAttack *= ratioB[role]
//   公共：hpBase *= ratioC[role]；随后按特性调整 statBase*；末尾 attr17Base[13]+=50。
// 偏移实证：m_elemAttack@+0x38、m_attack@+0x18、m_magicAttack@+0x20、
// m_hpBase@+0x04、statBase3/4/6@+0x118/+0x120/+0x130、statBase11..14@
// +0x158/+0x160/+0x168/+0x170、attr17Base[13]=+0xac（ActiveStaticInfo.h 布局）。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=DIFF[P209/N18b] ORIG 0x088b7c29
// orig/ours=229/223 ndiff=164。已复现：12 条栈上 float 常量初始化（非 static 局部
// const 数组）、5 处 haveChampionFeature + 立即数实参、fadd st,st 加倍形、lea 加 50。
// 残余：4.4 对 elem 与立即数比较恒 mem 直比（ORIG 单次 mov+test/cmp 复用 %eax）、
// IF/ELSEIF 块序镜像（P47）、索引写回 add-vs-lea（P30/N4）。语义等价封顶。
// [L14 round2 修] P251/P286 switch 化：if/else-if → `switch(elem){case 0..3 合并; case 4;}`
// ⇒ 复现 ORIG `mov 0x8(%ebp),%eax; test; js; cmp $3; jle; cmp $4; je` 寄存器比较链，
// gap **−6→0**（229/229）、差异块 5→2、strict 33→21（无回退）。余槽整块浅 4 ⇒ N126。
// [D07 round5 收口] AE 达成（229/229，verdict DIFF→AE）：补第二枚具名局部
// `const int newValue`（ORIG 0x088b7cf2 `fistpl -0xc(%ebp)` 独立结果槽 + 0x088b7cf8
// `mov -0xc(%ebp),%edx` 写回；旧写把 ROUND 结果复用 -0x4c 临时槽 ⇒ 12×float 数组
// 基址浅 4（-0x40→-0x3c）且初始化块整体错位）。补 `oldValue`+`newValue` 两槽后
// 数组基址与 ORIG 对齐、ROUND 段逐条全等（P0 槽-局部双向律）。
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
}