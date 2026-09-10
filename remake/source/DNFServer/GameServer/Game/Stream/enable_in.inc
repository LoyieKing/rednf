bool Stream::enable_in(int n)
{
    if (in_ptr() + n > size())
    {
        return false;
    }
    return true;
