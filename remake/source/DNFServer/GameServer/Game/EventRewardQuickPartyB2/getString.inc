const char* ContentsType::getString(ContentsType::T type)
{
    switch ((int)type)
    {
    case 1:  return "[normal]";
    case 2:  return "[ancient]";
    case 3:  return "[dimension]";
    case 4:  return "[tower]";
    case 5:  return "[infinite]";
    case 6:  return "[ultimate]";
    case 7:  return "[despair]";
    case 8:  return "[tournament]";
    case 9:  return "[pvp]";
    case 10: return "[fair pvp]";
    case 11: return "[warroom]";
    case 12: return "[assault]";
    default: return "";
    }
