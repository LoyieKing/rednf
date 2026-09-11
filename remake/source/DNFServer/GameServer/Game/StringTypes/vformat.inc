// ORIG T 0x08ad8b9e：length==0 → this->C1Ev()（**就地默认构造覆盖**，旧 data 不 detach——
// ORIG 体实证无 ~CharString/D1 调用）；否则 create(length)+ss_vsprintf 后 this->C1EP14CharStringData
// 就地覆盖；尾 return *this。
// [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08ad8b9e）：ORIG 两处就地覆盖为
// 单条 `call CharString::C1Ev` / `call CharString::C1EP14CharStringData`（4.1 前端
// 内联 placement new）；4.4 对 `new (this) T(...)` 恒发 `_ZnwjPv` call + 判空
// （每处 +4~5 条，58 vs 49）。显式构造调用语法 `this->CharString::CharString()`
// 在 4.4 编译报「错误地使用了 struct」（探针 /tmp/s23/p3.cpp）⇒ 无合法源形，
// 语义等价（就地构造覆盖、旧 data 不 detach 均保持）。
CharString CharString::vformat(const char* formatStr, va_list argList)
{
    // b28d-Main：static sret 按值（ORIG 0x08ad8b9e）。
    int length = vsnprintf(reinterpret_cast<char*>(0), 0, formatStr, argList);
    if (length == 0)
        return CharString();
    CharStringData* newData = CharStringData::create(length);
    ss_vsprintf(newData->getBuffer(), length + 1, formatStr, argList);
    return CharString(newData);
}