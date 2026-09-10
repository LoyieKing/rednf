bool CSyncSlangFilter::HasSlangName(const std::string& name)
{
    unsigned i;
    i = 0;
    for (i = 0; i < name.size(); ++i)
    {
        readGuard<rwLock> guard(&lock_);
        if (MatchName(name.substr(i, name.size() - i)) != 0)
            return 1;
    }
    return 0;
