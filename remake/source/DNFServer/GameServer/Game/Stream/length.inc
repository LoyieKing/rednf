int Stream::length()
{
    return in_ptr() - out_ptr();
}