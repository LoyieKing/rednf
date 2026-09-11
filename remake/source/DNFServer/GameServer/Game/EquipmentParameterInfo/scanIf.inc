void scanIf(EquipmentParameterInfo* info, int idx)
{
    // [b28c-lane24] ORIG 0x0899cd6c：`jmp cond` + 体尾 `ScanType; test %al;
    // jne body`（while 形，无 xor）；原 for(;;)+if(!ScanType)break 生
    // xor$1 物化 + ScanType 位置漂移。
    (void)idx;
    std::vector<int> values;
    std::string token;
    while (ScanType(token, true))
    {
        values.clear();
        if (token == "[/if]")
        {
            break;
        }
        readCondtion(info, token, values);
        values.empty();
    }
}