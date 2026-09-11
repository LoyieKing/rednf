int Arad_Script::GetIdx4Row(char* name)
{
    std::map<std::string, int>::iterator it = rowIndex_.find(std::string(name));
    if (it != rowIndex_.end())
        return it->second;
    return -1;
}