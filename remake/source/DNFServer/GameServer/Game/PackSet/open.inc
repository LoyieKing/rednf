pack_ret_t PackSet::open(char const* name, char const* tag, pack_load_type_t type)   // 0x08ac5a52
{
    if (UsingEncryptedFile())
    {
        return openEncryptedFile(name, tag, type);
    }

    // b28d-solo-PackSet 判词（N 封顶：两处跨版本 codegen 残差，ORIG 0x08ac5a52
    // size 0x69d；本轮 ndiff 330→9、gap→-1）：
    // ①ORIG is_ready() 之前 it C1Ev(-0x50) → index_tag C1Ev(-0x64)，两局部都在
    //   函数头、构造序 it→index_tag（P28/P50/P212）。
    // ②槽序 = 4.4 **声明序**分配：it -0x50 / idxHdrSize -0x54 / idxHdrCrc -0x58
    //   / idxHdrKey -0x5c / cur -0x60 / index_tag -0x64 ⇒ idxHdr*/cur 声明必须
    //   排在 index_tag **之前**（一处改动使 6 处 lea 立即数全对齐）。
    // ③三处遍历改 while 底部条件布局（P283）；④idxHdrSize/Crc/Key 去预清零
    //   （P171：ORIG fread 直接写槽）；⑤`cur = idxHdrData` 移到 resize 之前
    //   （ORIG +0x2ef）；⑥npos 用 volatile 视图复现内存装载。
    // 残余（语义等价，N 封顶）：a) ORIG `mov <A>,%edx; cmp %edx,%eax` 单条绝对
    //   地址装载 vs 4.4 `mov $A,%edx; mov (%edx),%edx`（非 volatile 会被折成
    //   `cmp $-1`）；b) fread 实参装载序（P75：ORIG 先 fp 后 fileSize，4.4 先把
    //   fileSize 载 %eax 再丢弃）——两处净差 1 条 mov。
    // [2026-09-11 L2Free4to15.S4] fresh .o 复核：orig=438 / our=439、gap=−1、ndiff **6**。
    //   试形：`*&tstring::npos`（去 volatile）⇒ 4.4 折 `cmp $<A>,%eax`（our 437、npos 降级
    //   为立即数）而 ndiff 不变 ⇒ 回滚维持 volatile 取址形。残余 a/b 两项均 N 封顶。
    index_vector_it it;
    unsigned int idxHdrSize;        // -0x54：索引头区大小
    hash_t idxHdrCrc;               // -0x58：头 CRC（checksum/key）
    hash_t idxHdrKey;               // -0x5c：解密种子兼条目数
    unsigned char* cur;             // -0x60：索引头游标
    tstring index_tag;

    if (is_ready())
    {
        fprintf(stdout, "%s\n", "Already opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3d8, __FUNCTION__);
        return PACK_ERROR;
    }

    FILE* fp = fopen(name, "rb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3e1, __FUNCTION__);
        return PACK_ERROR;
    }

    _fread_str(index_tag, fp);
    if (tag != 0)
    {
        if (strcmp(index_tag.c_str(), tag) != 0)
        {
            fclose(fp);
            fprintf(stdout, "tag mismatch. %s", index_tag.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3ec, __FUNCTION__);
            return PACK_ERROR;
        }
    }

    fread(&m_revision, 4, 1, fp);
    fread(&idxHdrSize, 4, 1, fp);
    fread(&idxHdrCrc, 4, 1, fp);
    fread(&idxHdrKey, 4, 1, fp);

    unsigned char* idxHdrData = new unsigned char[idxHdrSize];   // -0x1c
    fread(idxHdrData, idxHdrSize, 1, fp);

    m_headerSize = ftell(fp);       // 数据区起点 = 头之后偏移
    m_packFile = fp;

    if (_decrypt_N_crc32(idxHdrKey, idxHdrCrc, idxHdrCrc, idxHdrData, idxHdrSize) == 0)
    {
        delete[] idxHdrData;
        return PACK_ERROR;
    }

    // b28d-solo-PackSet：ORIG +0x2ef `mov -0x1c,%eax; mov %eax,-0x60` 在
    // `resize` 之前 ⇒ cur 赋值必须先于 resize。
    cur = idxHdrData;
    m_indexes.resize(static_cast<std::size_t>(idxHdrKey), index_pair_t());

    // b28d-solo-PackSet（P283）：ORIG 三处遍历均为 while 底部条件布局
    // （init → `jmp cond` → body → cond），4.4 对 for 恒把 cond 置顶。
    it = m_indexes.begin();
    while (it != m_indexes.end())
    {
        it->first = *reinterpret_cast<unsigned long*>(cur);
        cur += 4;
        it->second.read_info(cur);
        ++it;
    }
    delete[] idxHdrData;

    if (type == PACK_LOAD_FULL)
    {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        unsigned char* whole = new unsigned char[fileSize];
        fread(whole, fileSize, 1, fp);
        it = m_indexes.begin();
        while (it != m_indexes.end())
        {
            it->second.update_content_from_memory(whole, this);
            ++it;
        }
        delete[] whole;
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        if (fileSize > 0)
        {
            fseek(fp, 0, SEEK_SET);
            unsigned char* whole = new unsigned char[fileSize];
            fread(whole, fileSize, 1, fp);
            it = m_indexes.begin();
            while (it != m_indexes.end())
            {
                // b28d-solo-PackSet（p167）：ORIG 调 `IndexEntry::name()` getter
                // （0x08ac5ffa/0x08ac6010 两处 call _ZNK10IndexEntry4nameEv）
                // 而非直访 name_ 成员。
                it->second.name();
                // b28d-solo-PackSet（P304 同族）：ORIG `mov &_ZNSs4nposE,%edx;
                // cmp %edx,%eax` 为内存装载 npos；4.4 把 npos 折成立即数 ⇒
                // `*&` 强制取址装载。
                // [2026-09-11 试形记录] `*&tstring::npos`（去 volatile）虽消掉取址中转、
                //   但 4.4 折成 `cmp $<A>,%eax`（1 条）——比 ORIG 的 2 条
                //   （`mov <A>,%edx; cmp %edx,%eax`）**少** 1 条且 npos 语义降级为
                //   立即数；ndiff 不变（6）而 ours 438→437 ⇒ 回滚维持 volatile 取址形。
                if (it->second.name().find(".ani", 0) !=
                    *reinterpret_cast<volatile tstring::size_type const*>(&tstring::npos))
                {
                    it->second.update_content_from_memory(whole, this);
                }
                ++it;
            }
            delete[] whole;
        }
    }

    m_type = type;
    return PACK_OK;
}