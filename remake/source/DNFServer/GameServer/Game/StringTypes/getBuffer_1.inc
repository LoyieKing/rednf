char* CharStringData::getBuffer()
{
    return reinterpret_cast<char*>(this + 1);
