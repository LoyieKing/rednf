// T 0x08ad4548/0x08ad45bf/0x081b76??/0x08ad468b/0x08ad473e（Game 目标缺失，
// SecureStdio.cpp 仅入 point/auction 目标；本批补齐到 game）。模板版
// ss_sprintf<N> / ss_strcpy<N> 为 ORIG W（头内联模板），force 实例化。
// （ORIG TU = SecureStdio.cpp）
// ===========================================================================
int ss_sprintf(char* buffer, unsigned int count, const char* format, ...)
{
    // ORIG 0x08ad4548：va_list 槽（lea 0x14(%ebp) 单条）+ 返回值具名局部槽。
    va_list args;
    va_start(args, format);
    int r = ss_vsprintf(buffer, count, format, reinterpret_cast<char*>(args));
    va_end(args);
    return r;
}