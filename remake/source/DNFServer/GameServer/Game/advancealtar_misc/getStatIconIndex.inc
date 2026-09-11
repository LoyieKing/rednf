// ORIG T 0x088a3056：ft∈[10,24] → 序号 0..14，缺省 -1。
int BuyUpgradeData::getStatIconIndex(FieldType::T ft) const
{
    // b28d-35：ORIG 为「具名 result 局部（-0x4，初值 -1）+ 每 case
    //   `movl $N,-0x4; jmp` 汇聚尾 `mov -0x4,%eax`」；逐 case return 形使 4.4
    //   直接 `mov $N,%eax`（无 result 槽）⇒ 改 result 赋值 + break 形。
    int result = -1;
    switch (ft)
    {
    case FieldType::FT_HP:   result = 0;   break;   // 0x0a
    case FieldType::FT_DMG:  result = 1;   break;   // 0x0b
    case FieldType::FT_PIC:  result = 2;   break;   // 0x0c
    case FieldType::FT_RCV:  result = 3;   break;   // 0x0d
    case FieldType::FT_SUM:  result = 4;   break;   // 0x0e
    case FieldType::FT_MHT:  result = 5;   break;   // 0x0f
    case FieldType::FT_RGN:  result = 6;   break;   // 0x10
    case FieldType::FT_MSD:  result = 7;   break;   // 0x11
    case FieldType::FT_ASD:  result = 8;   break;   // 0x12
    case FieldType::FT_SDT:  result = 9;   break;   // 0x13
    case FieldType::FT_FDT:  result = 10;  break;   // 0x14
    case FieldType::FT_ETC:  result = 0xb; break;   // 0x15
    case FieldType::FT_LRG:  result = 0xc; break;   // 0x16
    case FieldType::FT_SPL:  result = 0xd; break;   // 0x17
    case FieldType::FT_ETC2: result = 0xe; break;   // 0x18
    default:                 break;
    }
    return result;
}