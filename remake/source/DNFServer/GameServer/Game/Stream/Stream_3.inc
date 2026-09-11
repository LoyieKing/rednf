Stream::Stream(Stream& other)
{
    Stream();
    put_binary(other.get_buffer(), other.length());
}