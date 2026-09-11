pack_ret_t PackSet::verify()    // 0x08ac6c8a
{
    if (UsingEncryptedFile())
    {
        return verifyEncryptedFile();
    }

    unsigned int bufferSize = 0x500000u;
    unsigned char* buff_file = 0;
    unsigned char* buff_pack = 0;
    unsigned int contentSize = 0;
    // b28d-solo-PackSet 判词（N 封顶：4.4 栈槽分配策略差异，ORIG 0x08ac6c8a
    // size 0x973）：
    // ①ORIG 序言 5 标量清零 → it C1Ev(-0x70) → is_ready；迭代器为函数级声明
    //   （p167 MISSING 该 C1Ev；两循环复用同一槽）。
    // ②两处遍历改 while 底部条件布局（P283）；③删 printf 首参 `+0.5f`（ORIG 为
    //   `flds 100.0f; fmulp; fistpl`，无 faddp）；④`if (1000 < since_refresh)`
    //   改 `while (…) { …; break; }` 复现 ORIG `cmp $0x3e8; setg %al;
    //   test %al,%al; je`（P225）。
    // 残余（语义等价，N82/N98）：4.1 按生命周期/类型分组分配（contentSize
    //   -0x6c、bufferSize -0x48、size_file -0x3c、proc_count -0x38…），4.4 按
    //   首次使用序紧密分配（contentSize -0x74、bufferSize -0x50…）⇒ 全帧槽号
    //   整体深 8、帧 0xac vs 0xcc。contentSize 声明位置两形实测均不翻 ⇒ 封顶。
    // [2026-09-11 L2Free4to15.S4] fresh .o 复核：orig=628 / our=634、gap=−6、ndiff **254**
    //   —— 254 行中仅 1 行为真 insert（fread(buff_file,size_file,1,fp) 实参装载序，P75），
    //   其余全是整体深 8 的槽号 replace（首槽基址常数 = N126/N82，非源级可翻）。
    index_vector_it it;

    if (!is_ready())
    {
        fprintf(stdout, "%s\n", "Not yet opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5a5, __FUNCTION__);
        return PACK_ERROR;
    }

    buff_file = new unsigned char[bufferSize];
    buff_pack = new unsigned char[bufferSize];

    size_t proc_count = 0;
    size_t total_count = 0;
    unsigned int proc_size = 0;
    unsigned int total_size = 0;
    Tick start_time;                   // ORIG：先默认构造再赋值（TickC1+now+aSES）
    Tick frame_start;                  // 最后一次刷新时刻
    int run_time = 0;
    float avg_time = 0.0f;
    int remain_time = 0;

    // b28d-solo-PackSet（P283）：ORIG 两处遍历均为 while 底部条件布局。
    it = m_indexes.begin();
    while (it != m_indexes.end())
    {
        total_size += it->second.size_;
        ++it;
    }
    total_count = m_indexes.size();

    start_time = Tick::now();          // 进度更新基准
    frame_start = Tick::now();

    it = m_indexes.begin();
    while (it != m_indexes.end())
    {
        if (bufferSize < it->second.aligned_size_)
        {
            delete[] buff_file;
            buff_file = 0;
            delete[] buff_pack;
            buff_pack = 0;
            bufferSize = it->second.aligned_size_;
            buff_file = new unsigned char[bufferSize];
            buff_pack = new unsigned char[bufferSize];
        }

        *buff_file = 0;
        *buff_pack = '\0';

        hash_t code = it->first;
        if (it->second.get_content(this, code, buff_pack, bufferSize, &contentSize) == PACK_ERROR)
        {
            fprintf(stdout, "get content. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5e0, __FUNCTION__);
            return PACK_ERROR;      // ORIG：此处不释放缓冲
        }

        FILE* fp = fopen(it->second.name_.c_str(), "rb");
        if (fp == 0)
        {
            fprintf(stdout, "fopen failed. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5e7, __FUNCTION__);
            return PACK_ERROR;
        }
        fseek(fp, 0, SEEK_END);
        std::size_t size_file = static_cast<std::size_t>(ftell(fp));
        fseek(fp, 0, SEEK_SET);
        fread(buff_file, size_file, 1, fp);
        fclose(fp);

        if (size_file != contentSize)
        {
            fprintf(stdout, "size is different. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5f4, __FUNCTION__);
            return PACK_ERROR;
        }

        if (memcmp(buff_file, buff_pack, contentSize) != 0)
        {
            fprintf(stdout, "contents is different. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5fa, __FUNCTION__);
            return PACK_ERROR;
        }

        proc_size += it->second.size_;
        proc_count += 1;

        int since_refresh = Tick::now() - frame_start;
        // b28d-solo-PackSet（P225）：ORIG `cmp $0x3e8,%eax; setg %al; test %al,%al;
        // je` 为 4.1 条件物化 ⇒ while+break 形复现（4.4 对 if 恒直跳 jle）。
        while (1000 < since_refresh)
        {
            frame_start = Tick::now();
            run_time = Tick::now() - start_time;
            avg_time = static_cast<float>(run_time) / static_cast<float>(proc_size);
            remain_time = static_cast<int>((static_cast<float>(total_size - proc_size) * avg_time));
            // b28d-solo-PackSet：ORIG 无 `+0.5f` 的 faddp（序列为
            // `flds 100.0f; fmulp; fistpl`）⇒ 直接截断转换。
            printf("%3d%% (%2dmin %2dsec)/(%2dmin %2dsec) (%d/%d)    \n",
                   static_cast<int>((100.0f * (static_cast<float>(proc_size)
                                                / static_cast<float>(total_size)))),
                   run_time / 60000, (run_time % 60000) / 1000,
                   remain_time / 60000, (remain_time % 60000) / 1000,
                   static_cast<int>(proc_count), static_cast<int>(total_count));
            break;
        }
        ++it;
    }

    run_time = Tick::now() - start_time;
    avg_time = static_cast<float>(run_time) / static_cast<float>(proc_size);
    remain_time = static_cast<int>((static_cast<float>(total_size - proc_size) * avg_time));
    printf("%3d%% (%2dmin %2dsec)/(%2dmin %2dsec) (%d/%d)    \n",
           static_cast<int>((100.0f * (static_cast<float>(proc_size)
                                        / static_cast<float>(total_size)))),
           run_time / 60000, (run_time % 60000) / 1000,
           remain_time / 60000, (remain_time % 60000) / 1000,
           static_cast<int>(proc_count), static_cast<int>(total_count));

    delete[] buff_file;
    delete[] buff_pack;
    return PACK_OK;
}