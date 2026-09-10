pack_ret_t PackSet::write(char const* name, char const* tag, unsigned int rev)   // 0x08ac60fa
{
    // b28d-solo-PackSet 判词（N 封顶：tail-merge 残差，ORIG 0x08ac60fa size
    // 0x88d；本轮 ndiff 455→32、gap→2）：
    // ①p167：ORIG 缺两个函数级迭代器默认构造 ⇒ 原码函数头声明 `it`/`it2`、
    //   循环内赋值（P28/P50 两段式）。
    // ②槽序 = 4.4「先声明深」：fp -0x18 / accumOffset -0x14 / sz -0x10 /
    //   headerBuf -0xc；发射序 fp 清零 → it(-0x34 C1Ev) → it2(-0x38) →
    //   writeIdx(-0x44) → headerBuf 清零（一处重排使 10+ 处槽引用归位）。
    // ③dir/file 声明序 file 先（ORIG dir@-0x260 深、file@-0x15c 浅）；
    // ④删 tmp.first/second 预清零（P171）；⑤accumOffset 改「值先」形 + int 化
    //   复现 `lea (%edx,%eax,1)` 序列；⑥hdrDataSz int 化复现两段装载。
    // 残余（语义等价，N121 tail-merge）：4.1 把 4 处 `return PACK_ERROR` 汇到
    //   +0x82d 共享块（`if(fp) fclose` + 双判空 delete[] + `mov $1,%ebx`），
    //   4.4 -O0 恒内联各失败路径（我们多 3 条 mov $1,%ebx、少共享块 2×cmpl/je
    //   与 jmp）。显式 goto 尾标签（+内层块）实测劣化槽位（ndiff 41/127）⇒ 保持内联。
    FILE* fp = 0;
    int accumOffset;                // -0x14（ORIG 在 sort 之后才清零）
    long sz;                        // -0x10
    index_vector_it it;
    index_vector2_it it2;
    index_vector2_t writeIdx;
    unsigned char* headerBuf = 0;

    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4e8, __FUNCTION__);
        return PACK_ERROR;
    }

    if (is_dirty())
    {
        if (sort() == PACK_ERROR)
        {
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4f0, __FUNCTION__);
            return PACK_ERROR;
        }
    }

    {
        // b28d-solo-PackSet：ORIG dir@-0x260 / file@-0x15c（先声明深）⇒ 4.4
        // 先声明浅，故 file 必须先声明。
        char file[260];
        char dir[260];
        for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
        {
            index_pair2_t tmp;
            if (_correct_filename(it->second.name_.c_str(), file, 0x104,
                                  dir, 0x104) == PACK_ERROR)
            {
                fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4ff, __FUNCTION__);
                return PACK_ERROR;      // ORIG tail-merge 至 +0x82d（N121）
            }
            tmp.first = _write_hash(file, dir);
            tmp.second = &(it->second);
            writeIdx.push_back(tmp);
        }
    }
    std::sort(writeIdx.begin(), writeIdx.end(), &_index_less2);

    accumOffset = 0;
    for (it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
    {
        it2->second->offset_ = accumOffset;
        // b28d-solo-PackSet（P38/P183）：ORIG 为值先 lea 形
        // （`mov 0xc(%eax),%edx; mov -0x14,%eax; lea (%edx,%eax,1),%eax;
        // mov %eax,-0x14`），`+=` 在 4.4 生 `add %eax,-0x14` 内存自增。
        accumOffset = it2->second->aligned_size_ + accumOffset;
    }

    fp = fopen(TEMP_HEADER_FILENAME, "wb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", TEMP_HEADER_FILENAME);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x514, __FUNCTION__);
        return PACK_ERROR;              // ORIG tail-merge 至 +0x82d
    }

    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        fwrite(&(it->first), 4, 1, fp);
        if (it->second.write_info(fp) == PACK_ERROR)
        {
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x51c, __FUNCTION__);
            fclose(fp);
            return PACK_ERROR;
        }
    }
    // ORIG 循环结束后不 fclose（直接重开同名文件，实证 0x08ac654f fopen）

    fp = fopen(TEMP_HEADER_FILENAME, "rb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x526, __FUNCTION__);
        return PACK_ERROR;
    }

    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    if (sz < 0)
    {
        // ORIG sz<0 不 fclose，直接返回（实证 0x08ac65ef）
        return PACK_ERROR;
    }
    fseek(fp, 0, SEEK_SET);

    int hdrDataSz = static_cast<int>(sz + 3) & ~3;
    headerBuf = new unsigned char[hdrDataSz];
    memset(headerBuf, 0, hdrDataSz);
    fread(headerBuf, sz, 1, fp);
    fclose(fp);

    unsigned int entryCount = static_cast<unsigned int>(m_indexes.size());
    unsigned int hdrCrc = Crc32::crc32(entryCount,
                                       reinterpret_cast<char const*>(headerBuf), hdrDataSz);
    _encrypt(hdrCrc, headerBuf, hdrDataSz);

    fp = fopen(name, "wb");
    if (fp == 0)
    {
        delete[] headerBuf;
        fprintf(stdout, "_tfopen failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x551, __FUNCTION__);
        return PACK_ERROR;
    }

    _fwrite_str(tag, fp);
    fwrite(&rev, 4, 1, fp);
    fwrite(&hdrDataSz, 4, 1, fp);
    fwrite(&hdrCrc, 4, 1, fp);
    fwrite(&entryCount, 4, 1, fp);
    fwrite(headerBuf, hdrDataSz, 1, fp);

    for (it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
    {
        if (it2->second->content_ == 0)
        {
            fprintf(stdout, "content is NULL. %s", it2->second->name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x560, __FUNCTION__);
            fclose(fp);
            delete[] headerBuf;
            return PACK_ERROR;
        }
        fwrite(it2->second->content_, it2->second->aligned_size_, 1, fp);
    }
    fclose(fp);
    // ORIG 成功路径不回收 headerBuf（透明泄漏），忠实保留。
    return PACK_OK;
