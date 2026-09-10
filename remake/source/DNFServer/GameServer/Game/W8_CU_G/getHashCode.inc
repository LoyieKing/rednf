void CUser::getHashCode(unsigned char* digestOut)
{
    md5_context ctx;
    md5_starts(&ctx);
    // P0/P134：ORIG 无 bytes/head 槽（imul 结果 ebx 跨 call 缓存、operator[]
    // 结果 eax 直传），实参嵌套求值（R2L：size()*0x14d1 先物化，[0] 后算）。
    md5_update(&ctx,
               reinterpret_cast<unsigned char*>(&m_characList[0]),
               static_cast<int>(
                   static_cast<unsigned int>(m_characList.size()) * 0x14d1u));
    md5_finish(&ctx, digestOut);
