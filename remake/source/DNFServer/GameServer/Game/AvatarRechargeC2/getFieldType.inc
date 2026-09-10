int advancealtar::AdvanceAltarShopParameter::getFieldType(std::string name)
{
    int fieldType = -1;
    if (name.compare("level") == 0)
        fieldType = FieldType::FT_LEVEL;          // 0x00
    else if (name.compare("index") == 0)
        fieldType = FieldType::FT_INDEX;          // 0x01
    else if (name.compare("price") == 0)
        fieldType = FieldType::FT_PRICE;          // 0x02
    else if (name.compare("name") == 0)
        fieldType = FieldType::FT_NAME;           // 0x03
    else if (name.compare("cooltime") == 0)
        fieldType = FieldType::FT_COOLTIME;       // 0x04
    else if (name.compare("need point") == 0)
        fieldType = FieldType::FT_NEED_POINT;     // 0x05
    else if (name.compare("explain") == 0)
        fieldType = FieldType::FT_EXPLAIN;        // 0x06
    else if (name.compare("type explain") == 0)
        fieldType = FieldType::FT_TYPE_EXPLAIN;   // 0x07
    else if (name.compare("set slot") == 0)
        fieldType = FieldType::FT_SET_SLOT;       // 0x08
    else if (name.compare("image index") == 0)
        fieldType = FieldType::FT_IMAGE_INDEX;    // 0x09
    else if (name.compare("hp") == 0)
        fieldType = FieldType::FT_HP;             // 0x0a
    else if (name.compare("dmg") == 0)
        fieldType = FieldType::FT_DMG;            // 0x0b
    else if (name.compare("pic") == 0)
        fieldType = FieldType::FT_PIC;            // 0x0c
    else if (name.compare("rcv") == 0)
        fieldType = FieldType::FT_RCV;            // 0x0d
    else if (name.compare("sum") == 0)
        fieldType = FieldType::FT_SUM;            // 0x0e
    else if (name.compare("mht") == 0)
        fieldType = FieldType::FT_MHT;            // 0x0f
    else if (name.compare("rgn") == 0)
        fieldType = FieldType::FT_RGN;            // 0x10
    else if (name.compare("msd") == 0)
        fieldType = FieldType::FT_MSD;            // 0x11
    else if (name.compare("asd") == 0)
        fieldType = FieldType::FT_ASD;            // 0x12
    else if (name.compare("sdt") == 0)
        fieldType = FieldType::FT_SDT;            // 0x13
    else if (name.compare("fdt") == 0)
        fieldType = FieldType::FT_FDT;            // 0x14
    else if (name.compare("etc") == 0)
        fieldType = FieldType::FT_ETC;            // 0x15
    else if (name.compare("lrg") == 0)
        fieldType = FieldType::FT_LRG;            // 0x16
    else if (name.compare("spl") == 0)
        fieldType = FieldType::FT_SPL;            // 0x17
    else if (name.compare("etc2") == 0)
        fieldType = FieldType::FT_ETC2;           // 0x18
    return fieldType;
