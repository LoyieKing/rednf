bool AradAppSystem::script_load()
{
    std::cerr << "[!] script loading...\n";
    if (!G_CDataManager()->init())
        return false;
    std::cerr << "[!] script loading Complete!!\n";
    return true;
}