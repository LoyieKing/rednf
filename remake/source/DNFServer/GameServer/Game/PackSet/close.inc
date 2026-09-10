void PackSet::close()   // @0x08ac49da
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac49da size 0x18b）：
    // ORIG else 分支开头即 `lea -0x14; call iterator::C1Ev`（fclose 检查**之前**），
    // 两个循环各自 `it = …begin()` 复用同一槽（-0x14）⇒ 原码是**函数级**
    // `index_vector_it it;` 声明 + for 内赋值（P28/P50/P212）；旧形两个 for 各在
    // init 处直构，既缺 C1Ev 又使 begin sret/end 临时槽整体漂移。
    if (UsingEncryptedFile())
    {
        closeEncryptedFile();
        return;
    }

    index_vector_it it;
    if (m_packFile != 0)
    {
        fclose(m_packFile);
    }

    // 释放两份索引中的内容缓冲（operator delete[]）
    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
    for (it = m_addeds.begin(); it != m_addeds.end(); ++it)
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
    clear();
