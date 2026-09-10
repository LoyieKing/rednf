CharString app::getStr(int id)
{
    const CharString* entry;        // ORIG 无初始化槽（@0x8ad54d6 直接取址传参）
    if (app::stringResource()->get(id, &entry))
        return *entry;
    // ORIG 0x08ad5508：静态 sret 直写返回对象，无局部 C1Ev/无拷贝。
    return CharString::format("getStr(%d) == none", id);
