int getEquipmentTypeToEnumValue(std::string& s)
{
    if (s == "hat avatar")   return 0;
    if (s == "hair avatar")  return 1;
    if (s == "face avatar")  return 2;
    if (s == "coat avatar")  return 3;
    if (s == "pants avatar") return 4;
    if (s == "shoes avatar") return 5;
    if (s == "breast avatar") return 6;
    if (s == "waist avatar") return 7;
    if (s == "skin avatar")  return 8;
    if (s == "aurora avatar") return 9;
    if (s == "weapon")       return 10;
    if (s == "title name")   return 11;
    if (s == "coat")         return 12;
    if (s == "shoulder")     return 13;
    if (s == "pants")        return 14;
    if (s == "shoes")        return 15;
    if (s == "waist")        return 16;
    if (s == "amulet")       return 17;
    if (s == "wrist")        return 18;
    if (s == "ring")         return 19;
    if (s == "support")      return 20;
    if (s == "magic stone")  return 21;
    return 22;
