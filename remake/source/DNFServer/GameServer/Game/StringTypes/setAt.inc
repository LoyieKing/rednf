CharString CharString::setAt(int idx, char ch)
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad6ca2）：①删 `char* dest` 具名缓存
    // （P183），memcpy 三实参按 ORIG 求值序内联——size(`getLength()+1`)→edi、
    // src(`thisData->getBuffer()`)→esi、dst(`newData->getBuffer()`)→eax；
    // ②两个指针局部按 P212 全头部声明（thisData 先 ⇒ 4.4 先声明深 -0x20、
    // newData 后 ⇒ -0x1c），复现 ORIG 槽序（旧 N82 判词作废）。
    CharStringData* thisData;
    CharStringData* newData;
    if (buffer_[idx] == ch)
    {
        return *this;
    }
    else
    {
        thisData = getData();
        newData = CharStringData::create(thisData->getLength());
        memcpy(newData->getBuffer(), thisData->getBuffer(), thisData->getLength() + 1);
        newData->getBuffer()[idx] = ch;
        return CharString(newData);
    }
