const char* Arad_Script::make_file_path(const char* file)
{
    static char full_path[0x200];
    std::strcpy(full_path, file);
    return full_path;
