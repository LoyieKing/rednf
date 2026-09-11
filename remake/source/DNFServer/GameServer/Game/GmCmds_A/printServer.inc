// ORIG printServer 0x084aebae：fopen(path,"wt") + fputs + fclose。
void CBoosterTest::printServer(char const* path, char const* contents)
{
    FILE* fp = fopen(path, "wt");
    if (fp == 0)
    {
        goto done;
    }
    fputs(contents, fp);
    fclose(fp);
done:
    return;
}