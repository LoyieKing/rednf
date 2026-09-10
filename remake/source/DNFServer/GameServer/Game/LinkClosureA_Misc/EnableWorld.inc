bool EnableWorld(const CItem* a, const CItem* b)
{
    if (!a->IsEnableWorld(b))           // [objdump 0x849d76e..0x787]
    {
        return false;                   // [0x849d780 xor $1; 失败 mov $0]
    }
    if (!b->IsEnableWorld(a))           // [0x849d78e this=b, arg=a]
    {
        return false;                   // [0x849d79b..0x7a7]
    }
    return true;                        // [0x849d7ae mov $1]
