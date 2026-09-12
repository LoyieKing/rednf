// ORIG 0x0858c6d4 _ZN6OS_API17WriteLastBookmarkEPKci：
// fopen(path,"wb") 成功则 fprintf "bm:%d" 后 fclose。
void OS_API::WriteLastBookmark(char const* path, int value)
{
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        fprintf(f, "bm:%d", value);
        fclose(f);
    }
}