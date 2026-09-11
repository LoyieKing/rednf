bool CDataManager::hasPreventString(const char* name)
{
    if (getCharacNameFilterPass())
    {
        setCharacNameFilterPass(false);
        return false;
    }
    std::string str(name);
    return CSyncSlangFilter::GetInstance()->HasSlang(str);
}