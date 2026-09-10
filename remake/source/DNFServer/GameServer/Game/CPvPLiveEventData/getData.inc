void CPvPLiveEventData::getData(char* pData) const
{
    // ORIG 0x08269206：函数体为空（仅 push/pop/ret）。
    // 播放计数不在此序列化，由 _saveData 走 CStreamGuard 落库。
