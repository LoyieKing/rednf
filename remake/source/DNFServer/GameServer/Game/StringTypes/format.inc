CharString CharString::format(const char* formatStr, ...)
{
    // b28d-Main：改为 static sret 按值（ORIG 0x08ad8afa 尾 ret $0x4、fmt@0xc/varargs@0x10）。
    va_list argList;
    va_start(argList, formatStr);
    // [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08ad8afa）：ORIG format 体是 vformat 逻辑的
    // **独立副本**（体内无 call vformat；4.1 前端把头内 inline vformat 展开进 format）。
    // 旧「委托 vformat」形只 15 条 vs ORIG 53 条且调用点差集巨大，改为展开完整逻辑；
    // 展开后 53 vs 62，余下 9 条全为 N83（placement operator new 4.4 恒发
    // _ZnwjPv call+判空，两处 ×4~5）⇒ 封顶，语义等价。
    int length = vsnprintf(reinterpret_cast<char*>(0), 0, formatStr, argList);
    if (length == 0)
    {
        va_end(argList);
        return CharString();
    }
    CharStringData* newData = CharStringData::create(length);
    ss_vsprintf(newData->getBuffer(), length + 1, formatStr, argList);
    va_end(argList);
    return CharString(newData);
