// ORIG C2（0x891a0d4）：成员默认构造 + 函数体 `*this = other`（0x891a2a2 call op=）。
// ORIG C2（0x891a0d4）：成员默认构造 + 全局记账 + `*this = other`。
EquipmentParameterInfo::EquipmentParameterInfo(const EquipmentParameterInfo& other)
{
    ++g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
    *this = other;
}