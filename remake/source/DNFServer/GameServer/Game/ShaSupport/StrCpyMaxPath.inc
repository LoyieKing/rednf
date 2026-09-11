// ---------------------------------------------------------------------------
// StrCpyMaxPath(char* dest, const char* src) T @0x0808a5dd
// memset(dest,0,0x104); strncpy(dest,src,0x103)（PLT 调用逐参数转写）。
// ---------------------------------------------------------------------------
void StrCpyMaxPath(char* dest, const char* src)
{
    std::memset(dest, 0, 0x104);
    std::strncpy(dest, src, 0x103);
}