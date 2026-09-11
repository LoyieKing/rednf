// ORIG addCreatureModuleRate T 0x08a38a08（报告全文转写）：已有时乘旧值后覆盖写入。
void addCreatureModuleRate(ENUM_MODULE_TYPE type, float rate)
{
    std::map<ENUM_MODULE_TYPE, float>::iterator it =
        g_creatureModuleDamageRateMap.find(type);
    // b28c-l23：ORIG @0x08a38a4x 命中块为 `mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp);
    // flds 0xc(%ebp); fmuls -0xc(%ebp); fstps 0xc(%ebp)` ⇒ old 经**槽往返**后再乘
    // （N85 附「4.4 float 纯拷贝恒 mov 直搬」的突破口 = 具名 float 局部，
    // 与 P211/P218 同族的"值先物化"开关）；直写 `rate * it->second` 时 4.4 出
    // `flds 0x4(%eax); flds 0xc(%ebp); fmulp`（少 2 条且装载序反）。
    if (it != g_creatureModuleDamageRateMap.end())
    {
        float old = it->second;
        rate = rate * old;
    }
    g_creatureModuleDamageRateMap[type] = rate;
}