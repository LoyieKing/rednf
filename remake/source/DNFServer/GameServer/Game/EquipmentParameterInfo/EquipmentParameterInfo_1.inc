EquipmentParameterInfo::EquipmentParameterInfo(const EquipmentParameterInfo& other)
{
    ++g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
    *this = other;
