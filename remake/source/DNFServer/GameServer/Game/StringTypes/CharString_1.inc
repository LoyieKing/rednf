CharString::CharString(const CharString& src)
{
    CharStringData* newData = src.getData();
    attachData(newData);
}