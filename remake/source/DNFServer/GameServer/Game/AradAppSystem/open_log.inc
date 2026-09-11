bool AradAppSystem::open_log(char* name)
{
    G_CEnvironment()->set_file_name(name);
    char local_d4[200] = { '\0' };
    sprintf(local_d4, "./log/%s/", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(local_d4, (char*)0, 0);
    std::cerr << "[!] Log file init Sucess!!\n";
    return true;
}