WideStringData* WideString::getData() const
{
    return reinterpret_cast<WideStringData*>(reinterpret_cast<char*>(buffer_) - 8);
