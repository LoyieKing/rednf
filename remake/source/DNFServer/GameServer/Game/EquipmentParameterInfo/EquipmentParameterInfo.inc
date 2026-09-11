// ORIG C1（0x8919ce8）：成员默认构造后，先更新全局记账
// （count++、size = count * sizeof），再调 clear。
EquipmentParameterInfo::EquipmentParameterInfo()
{
    ++g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
    clear();
}