// ===========================================================================
// isServiceServerGroup  ORIG T 0x0889456f：枚举值 ∈ {0,7,8,10,13,0x12,0x65}
// 时返回 false，否则 true。（ORIG TU = CSCommonFunction.cpp）
// ===========================================================================
bool isServiceServerGroup(ENUM_SERVER_GROUP group)
{
    // ORIG：无参数槽（比较直读 0x8(%ebp)）；结果字节槽 movb $1 初值、
    // 命中 movb $0，尾 movzbl 返回。
    bool result = true;
    if (group == 8 || group == 10 || group == 7 || group == 13 ||
        group == 0 || group == 0x12 || group == 0x65)
        result = false;
    return result;
}