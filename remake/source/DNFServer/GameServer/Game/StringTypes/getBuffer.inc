unsigned char* StringData::getBuffer()
{
    return reinterpret_cast<unsigned char*>(this + 1);
