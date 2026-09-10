bool Stream::enable_out(int n)
{
    if (out_ptr() + n > in_ptr())
    {
        return false;
    }
    return true;
