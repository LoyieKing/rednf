CharStringData* CharString::getData() const
{
    return reinterpret_cast<CharStringData*>(buffer_ - 8);
