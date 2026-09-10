void WriteLastBookmark(char const* path, int value)
{
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        fprintf(f, "bm:%d", value);
        fclose(f);
    }
