// ============================================================================
// loadAppendageType —— ORIG 0x0889253b（_Z17loadAppendageTypeRKSs）
// 纯字符串→枚举映射链；比较顺序与 ORIG 级联完全一致，未匹配返回 -1。
// ============================================================================
int loadAppendageType(const std::string& token)
{
    if (token == "[physical attack]")                  return 0x00;
    if (token == "[magical attack]")                   return 0x01;
    if (token == "[physical defense]")                 return 0x03;
    if (token == "[magical defense]")                  return 0x02;
    if (token == "[equipment physical attack]")        return 0x04;
    if (token == "[equipment physical defense]")       return 0x05;
    if (token == "[jump power]")                       return 0x08;
    if (token == "[attack speed]")                     return 0x0a;
    if (token == "[move speed]")                       return 0x0b;
    if (token == "[cast speed]")                       return 0x0c;
    if (token == "[hp max]")                           return 0x0d;
    if (token == "[hp regen rate]")                    return 0x0e;
    if (token == "[magical critical hit rate]")        return 0x10;
    if (token == "[physical critical hit rate]")       return 0x0f;
    if (token == "[physical back attack critical rate]") return 0x11;
    if (token == "[magical back attack critical rate]")  return 0x12;
    if (token == "[fire element]")                     return 0x14;
    if (token == "[water element]")                    return 0x15;
    if (token == "[dark element]")                     return 0x16;
    if (token == "[light element]")                    return 0x17;
    if (token == "[stuck]")                            return 0x1b;
    if (token == "[mp regen rate]")                    return 0x1d;
    if (token == "[hit recovery]")                     return 0x1e;
    if (token == "[equipment magical attack]")         return 0x1f;
    if (token == "[equipment magical defense]")        return 0x20;
    if (token == "[good hit]")                         return 0x21;
    if (token == "[all active status resistance]")     return 0x1a;
    if (token == "[all element]")                      return 0x18;
    if (token == "[fire element attack]")              return 0x2a;
    if (token == "[water element attack]")             return 0x2b;
    if (token == "[dark element attack]")              return 0x2c;
    if (token == "[light element attack]")             return 0x2d;
    if (token == "[all element attack]")               return 0x2e;
    if (token == "[mp max]")                           return 0x30;
    if (token == "[separate attack]")                  return 0x35;
    if (token == "[antievil]")                         return 0x37;
    if (token == "[physical absolute damage]")         return 0x38;
    if (token == "[village moving speed]")             return 0x3f;
    if (token == "[magical absolute damage]")          return 0x3c;
    if (token == "[physical absolute defense]")        return 0x3d;
    if (token == "[magical absolute defense]")         return 0x3e;
    if (token == "[physical attack bonus]")            return 0x06;
    if (token == "[magical attack bonus]")             return 0x07;
    return -1;
}