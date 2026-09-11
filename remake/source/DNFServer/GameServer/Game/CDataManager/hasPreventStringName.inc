bool CDataManager::hasPreventStringName(const char* name)
{
    std::string str(name);
    return CSyncSlangFilter::GetInstance()->HasSlangName(str);
}