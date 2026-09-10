int Arad_Script::GetIdx4Col(char* name)
{
    std::map<std::string, int>::iterator it = colIndex_.find(std::string(name));
    if (it != colIndex_.end())
        return it->second;
    return -1;
