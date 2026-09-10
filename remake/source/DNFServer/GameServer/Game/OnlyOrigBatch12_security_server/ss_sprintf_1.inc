int ss_sprintf(char (&buffer)[N], const char* format, ...)
{
    // [b28d-solo-OnlyOrigBatch12] 主模板体 = ORIG 256/32/512 三实例形
    // （0x08aa69b4/0x08ac36de/…：sub $0x28 + va_list 槽 -0x10 + 返回槽 -0xc
    // 中转），三件与本源形逐条全等 AE。1024 实例 @0x08acf0e0 为 sub $0x18
    // 无槽直传形（同模板自相矛盾 = N105）⇒ 由下方 ss_sprintf<1024> 显式特化
    // 单独复现（判词见该处）。反向删 va_list/返回槽会把 256/32/512 三件打成
    // DIFF（P211 陷阱法联判已否）。
    va_list args;
    va_start(args, format);
    int r = ss_vsprintf(buffer, N, format, (char*)args);
    va_end(args);
    return r;
