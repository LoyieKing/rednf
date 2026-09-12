// ORIG 0x0858c54c _ZN6OS_API12WriteLastSigEPKcS1_iiPKvi：
// 256 栈缓冲清零 → sprintf "./log/%s/%s" → fopen("wb") →
// fprintf "K:%d C:%d P:%p SZ:%d\n" → fclose。
void OS_API::WriteLastSig(char const* dir, char const* name, int k, int c, void const* p, int sz)
{
    char path[256] = { '\0' };
    sprintf(path, "./log/%s/%s", dir, name);
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        fprintf(f, "K:%d C:%d P:%p SZ:%d\n", k, c, p, sz);
        fclose(f);
    }
}