bool scanRandomOptionValue(EquipmentParameterInfo* info, std::string& line)
{
    // ORIG 0x89bf67f：info==0 直接返回 true；否则以 NULL dir 调
    // importEquipmentParameterInfo(info, line, 0, a, b) 并透传其返回值。
    if (info == 0)
    {
        return true;
    }
    int a = 0;
    int b = 0;
    return importEquipmentParameterInfo(info, line, 0, a, b);
}