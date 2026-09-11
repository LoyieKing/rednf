// ORIG _ZN13charac_expand5CData6isSaveEv W 0x832ae80：`movzbl 0x4(%eax); ret` 直返
// m_bIsSave 字节（无 test;setne 布尔归一）。bool 字段 + bool 返回时 4.4 同样直返
// （探针 /tmp/l11p/issave.cpp：bool 字段 isSave 体 = movzbl;ret，调用点 = test %al;je；
// char 返回则调用点物化 setne——P73）。签名 mangled 不含返回类型。
bool CData::isSave()
{
    return m_bIsSave;
}