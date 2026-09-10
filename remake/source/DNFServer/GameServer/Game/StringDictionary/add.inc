void StringDictionary::add(const char* key, const char* value)
{
    const char* keyP = addString(key);
    const char* valP = addString(value);

    StringEntity e = {};   // [b22] ORIG add @0x08ab2398 先 3×movl $0 值初始化再逐字段覆写
    e.hash = getStringHashValue(keyP);
    e.key = keyP;
    e.value = valP;
    entities_.push_back(e);
