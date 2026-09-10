bool CacheCharacterMemory(int characNo, CACHE_CHARACTER_TYPE* mem)
{
    return GetInstanceCacheCharacterMgr()->CacheCharacter(characNo, mem);
