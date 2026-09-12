const char* Arad_Script::make_file_path(const char* file)
{
    // [R21scanEC §27.3] ORIG nm -S：`_ZZN11Arad_Script14make_file_pathEPKcE9full_path`
    // 0x9404860 **size 0x400**；旧 [0x200] 尺寸不足（strcpy 无边界 ⇒ 长路径分叉）。
    static char full_path[0x400];
    std::strcpy(full_path, file);
    return full_path;
}