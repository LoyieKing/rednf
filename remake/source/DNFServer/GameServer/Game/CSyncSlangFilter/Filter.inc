// [b28d-solo-CSyncSlangFilter] AE：sub 去具名块改全表达式实参 + stars/allocator 匿名临时（P221/P226；同时把两析构置于 i+=n 之前，复现 ORIG 块序与槽位）；ORIG 0x0862e46c。
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
}