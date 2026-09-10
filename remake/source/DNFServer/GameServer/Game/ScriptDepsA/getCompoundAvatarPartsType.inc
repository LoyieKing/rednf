int getCompoundAvatarPartsType(const std::string& name)
{
    if (name == "hat") return 0;
    if (name == "hair") return 1;
    if (name == "face") return 2;
    if (name == "neck") return 6;
    if (name == "coat") return 3;
    if (name == "pants") return 4;
    if (name == "belt") return 7;
    if (name == "shoes") return 5;
    if (name == "skin") return 8;
    if (name == "aurora") return 9;
    return 0x16;
