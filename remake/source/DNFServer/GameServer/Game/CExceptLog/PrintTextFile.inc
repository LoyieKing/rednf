void CExceptLog::PrintTextFile(char* name, char* content)
{
    FILE* stream = fopen(name, "a");
    if (stream != NULL)
    {
        fprintf(stream, content);
        fclose(stream);
    }
