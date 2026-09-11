WideString WideString::setAt(int idx, wchar_t ch)
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad9cf8）：同 CharString::setAt，
    // 删 `wchar_t* dest` 缓存 + P212 全头部声明复现槽序。
    WideStringData* thisData;
    WideStringData* newData;
    if (buffer_[idx] == ch)
    {
        return *this;
    }
    else
    {
        thisData = getData();
        newData = WideStringData::create(thisData->getLength());
        memcpy(newData->getBuffer(), thisData->getBuffer(),
               (thisData->getLength() + 1) * sizeof(wchar_t));
        newData->getBuffer()[idx] = ch;
        return WideString(newData);
    }
}