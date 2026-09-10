void WriteLastSig(char const* dir, char const* name, int k, int c, void const* p, int sz)
{
    char path[256] = { '\0' };
    sprintf(path, "./log/%s/%s", dir, name);
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        fprintf(f, "K:%d C:%d P:%p SZ:%d\n", k, c, p, sz);
        fclose(f);
    }
