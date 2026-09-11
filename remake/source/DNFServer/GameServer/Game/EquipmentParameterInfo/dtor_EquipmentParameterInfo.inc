// ORIG D1（0x891a4c6）：先递减全局记账，再析构成员（编译器生成）。
EquipmentParameterInfo::~EquipmentParameterInfo()
{
    --g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
}