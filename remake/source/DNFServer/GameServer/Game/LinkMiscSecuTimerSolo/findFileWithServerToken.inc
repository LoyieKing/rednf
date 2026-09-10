bool findFileWithServerToken(char* path)
{
    std::string pathStr(path);
    std::string full;
    int pos = pathStr.find_last_of("/", std::string::npos);
    if (pos == -1)
        pos = pathStr.find_last_of("\\", std::string::npos);
    if (pos != -1)
    {
        full = pathStr.substr(0, pos + 1);
        full += "(R)";
        full += pathStr.substr(pos + 1, pathStr.size());
        ss_strcpy(path, 0x104, full.c_str());
        // ORIG 982 `movzbl s_b;xor$1;test;je pack`：pack==0 fall=access，
        // pack==1 je=PackSet；P22 `if(!f())`⇒xor+test 同 line 数。
        if (!UseScriptPack())
        {
            if (access(path, 0) == 0)
                return true;
        }
        else
        {
            if (g_script_package.is_exist(path))
                return true;
        }
        ss_strcpy(path, 0x104, pathStr.c_str());
    }
    return false;
