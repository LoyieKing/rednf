// ---------------------------------------------------------------------------
// app::getStr —— ORIG T 0x08ad54cc：get(id) 命中 → 返回表项拷贝；
// 未命中 → CharString::format("getStr(%d) == none", id)。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource AE: 32=32 逐条全等（CharString::format 静态 sret 化后；旧
// 非静态形 32/55 系共享头签名错） | ORIG 0x08ad54cc]
CharString app::getStr(int id)
{
    const CharString* entry;        // ORIG 无初始化槽（@0x8ad54d6 直接取址传参）
    if (app::stringResource()->get(id, &entry))
        return *entry;
    // ORIG 0x08ad5508：静态 sret 直写返回对象，无局部 C1Ev/无拷贝。
    return CharString::format("getStr(%d) == none", id);
}