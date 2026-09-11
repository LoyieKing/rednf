// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0846d640。
void CExceptLog::PrintTextFile(char* name, char* content)
{
    FILE* stream = fopen(name, "a");
    if (stream != NULL)
    {
        fprintf(stream, content);
        fclose(stream);
    }
}