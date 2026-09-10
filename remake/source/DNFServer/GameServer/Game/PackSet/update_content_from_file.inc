pack_ret_t IndexEntry::update_content_from_file(char const* name, PackSet* owner,
                                                IPackFileProcess* process)
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4476 size 0x37d）：本轮修 5 处——
    // ①content_ 4 字节整型清零（ORIG 单条 movl）；②分支序 size_==0/process!=0/else；
    // ③loadFile 首参为 **name**（ORIG +0x1af `mov 0xc(%ebp)`，真语义修正）；
    // ④删 ret 具名局部（谓词内联）；⑤crc32 首参直传 _hash 返回值（无 cast）+
    // 单表达式形复现 content_/aligned_size_ 的 %ebx/%esi 预载（push esi/ebx）。
    // p167 CLEAR、帧 0x130 与 ORIG 全等。
    char dir[260];
    _correct_filename(name, dir, 0x104, 0, 0);

    FILE* fp = fopen(dir, "rb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed %s", dir);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1fb, __FUNCTION__);
        return PACK_ERROR;
    }

    type_ = ENTRY_FILE;
    name_ = dir;

    fseek(fp, 0, SEEK_END);
    long fpos = ftell(fp);
    if (fpos < 0)
    {
        fclose(fp);
        return PACK_ERROR;
    }

    size_ = static_cast<unsigned int>(fpos);
    fseek(fp, 0, SEEK_SET);
    aligned_size_ = (size_ + 3u) & ~3u;

    if (size_ == 0)
    {
        content_ = new unsigned char[4];
        // b28d-solo-PackSet（P171/P0）：ORIG `mov 0x18(%eax),%eax; movl $0,(%eax)`
        // = 单条 4 字节清零（单次 content_ 装载）⇒ 源为 4 字节整型写，
        // 逐字节 `content_[k]=0` 生 4×(重载+movb)。
        *reinterpret_cast<unsigned int*>(content_) = 0u;
    }
    // b28d-solo-PackSet（P214①/P272）：ORIG `cmpl $0,0x14(%ebp); je +0x1eb` ⇒
    // 分支序为 size_==0 / **process != 0** / else（process==0 走跳转臂）；
    // 旧形 `else if (process == 0)` 使 4.4 反产 `jne` 且两臂整体互换。
    else if (process != 0)
    {
        fclose(fp);
        fp = 0;
        // vtable slot2 → loadFile(name,&content_,&size_)；返回 1 视为失败。
        // b28d-solo-PackSet：ORIG `call *%edx; cmp $1,%eax; sete; test; je` 无 ret 槽
        // ⇒ 谓词内联（具名 ret 多占 4 字节槽使 dir/fp/fpos 整体深移 4）。
        // b28d-solo-PackSet 真语义修正：ORIG +0x1af 压入 `mov 0xc(%ebp),%eax`
        // （name 形参）而非规范化后的 dir ⇒ 原码传 name。
        if (process->loadFile(name, &content_, &size_) == PACK_ERROR)
        {
            return PACK_ERROR;
        }
        aligned_size_ = (size_ + 3u) & ~3u;
    }
    else
    {
        content_ = new unsigned char[aligned_size_];
        memset(content_ + size_, 0, aligned_size_ - size_);
        if (fread(content_, size_, 1, fp) != 1)
        {
            fprintf(stdout, "fread failed %s", dir);
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x226, __FUNCTION__);
            fclose(fp);
            return PACK_ERROR;
        }
    }

    if (fp != 0)
    {
        fclose(fp);
    }

    // ORIG 无 hash 具名局部（`call _hash; mov %eax,(%esp)` 直传）+ 第 1 实参无 cast；
    // 且 content_/aligned_size_ 在 toMbcs 之前预载 %ebx/%esi 跨两 call 保活 ⇒ 单表达式。
    crc_ = Crc32::crc32(_hash(toMbcs(dir)),
                        reinterpret_cast<char const*>(content_), aligned_size_);
    if (_encrypt(crc_, content_, aligned_size_) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x24e, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
