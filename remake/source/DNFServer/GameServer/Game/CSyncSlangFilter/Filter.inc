std::string CSyncSlangFilter::Filter(const std::string& str)
{
    std::string out(str);
    unsigned i;
    unsigned n;
    i = 0;
    for (; i < str.size(); )
    {
        readGuard<rwLock> guard(&lock_);
        n = (unsigned)match(str.substr(i, str.size() - i));
        if (n != 0)
        {
            out.replace(i, n, std::string(n, '*', std::allocator<char>()));
            i += n;
        }
        else
            ++i;
    }
    return out;
