// ORIG T 0x088a3106（sret 返回 std::string）：FieldType→字符串表条目
// CStringManager::getStr(id)；区间外的 ft 返回空串。
std::string BuyUpgradeData::getStatStringIndex(FieldType::T ft) const
{
    // b28d-solo-advancealtar_misc：ORIG 0x088a3106 = 140 条，每 case 独立
    //   `movl $id,(%esp); call getStr; mov %ebx,%edx; mov %eax,0x4(%esp);
    //   mov %edx,(%esp); call string::operator=(char const*); jmp 共享 sret 尾`
    //   ⇒ 原码是**每 case 独立赋值** `result = CStringManager::getStr(id); break;`
    //   （共享 sret result，非逐臂 return——后者 4.4 展开独立 sret 构造/析构 397 条）。
    //   原「具名 strId + 统一尾调用」形 4.4 只发一次 getStr/op=（73 条）。
    std::string result;
    switch (ft)
    {
    case FieldType::FT_HP:   result = CStringManager::getStr(0x11570); break;  // [objdump 0x88a3134]
    case FieldType::FT_DMG:  result = CStringManager::getStr(0x11571); break;  // [objdump 0x88a3153]
    case FieldType::FT_PIC:  result = CStringManager::getStr(0x11573); break;  // [objdump 0x88a3172]
    case FieldType::FT_RCV:  result = CStringManager::getStr(0x11574); break;  // [objdump 0x88a3191]
    case FieldType::FT_SUM:  result = CStringManager::getStr(0x11575); break;  // [objdump 0x88a31b0]
    case FieldType::FT_MHT:  result = CStringManager::getStr(0x11576); break;  // [objdump 0x88a31cf]
    case FieldType::FT_RGN:  result = CStringManager::getStr(0x1158f); break;  // [objdump 0x88a31ee]
    case FieldType::FT_MSD:  result = CStringManager::getStr(0x11590); break;  // [objdump 0x88a320d]
    case FieldType::FT_ASD:  result = CStringManager::getStr(0x11591); break;  // [objdump 0x88a322c]
    case FieldType::FT_SDT:  result = CStringManager::getStr(0x11592); break;  // [objdump 0x88a324b]
    case FieldType::FT_FDT:  result = CStringManager::getStr(0x11593); break;  // [objdump 0x88a326a]
    case FieldType::FT_ETC:  result = CStringManager::getStr(0x11596); break;  // [objdump 0x88a3286]
    case FieldType::FT_LRG:  result = CStringManager::getStr(0x11572); break;  // [objdump 0x88a32a2]
    case FieldType::FT_SPL:  result = CStringManager::getStr(0x11595); break;  // [objdump 0x88a32be]
    case FieldType::FT_ETC2: result = CStringManager::getStr(0x11596); break;  // [objdump 0x88a32da]
    default:                 return result;            // 跳转表范围外 → 空 string
    }
    return result;
}