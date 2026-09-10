pack_ret_t IndexEntry::get_content(PackSet* owner, hash_t code, unsigned char* dst,
                                   unsigned int dst_size, unsigned int* content_size)
{
    if (owner == 0 || dst == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x138, __FUNCTION__);
        return PACK_ERROR;
    }

    if (owner->UsingEncryptedFile())
    {
        return get_contentEncryptedFile(owner, code, dst, dst_size, content_size);
    }

    *content_size = 0;
    // [b23S3C] P198②：ORIG 直测 call 返回寄存器（test/sete 形，无 fp 槽）——谓词内联，勿提具名局部。
    if (owner->pack_file() == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x145, __FUNCTION__);
        return PACK_ERROR;
    }
    if (dst_size < size_)
    {
        fprintf(stdout, "%s\n", "buffer size is too small");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x14b, __FUNCTION__);
        return PACK_ERROR;
    }

    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac3eac size 0x36c）：
    // ORIG `mov 0x18(%eax),%eax; test; je +0x173`（content_==0 走**跳转**臂，
    // memcpy 直落 then 位）⇒ 源写正极性 `if (content_ != 0) { memcpy } else {...}`
    // （旧形 `==0` 反产 `jne` 跳 else、两块整体互换；P214①/P220/P222）。
    if (content_ != 0)
    {
        memcpy(dst, content_, aligned_size_);
    }
    else
    {
        if (type_ == ENTRY_FILE)
        {
            fprintf(stdout, "%s\n", "not implemented");
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x158, __FUNCTION__);
            return PACK_ERROR;
        }
        if (type_ == ENTRY_PACK && aligned_size_ != 0)
        {
            // b28d-solo-PackSet（P0/P52）：ORIG 无 pos 栈槽（`add %edx,%eax;
            // mov %eax,%ebx` 直接升格 callee-saved 跨 pack_file() 保活）⇒ 实参内联。
            fseek(owner->pack_file(),
                  static_cast<long>(owner->header_size() + offset_), SEEK_SET);
            if (fread(dst, aligned_size_, 1, owner->pack_file()) != 1)
            {
                fprintf(stdout, "%s\n", "fread failed");
                fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x163, __FUNCTION__);
                return PACK_ERROR;
            }
        }
    }

    *content_size = size_;
    if (_decrypt_N_crc32(code, crc_, crc_, dst, aligned_size_) == 0)
    {
        fprintf(stdout, "%s\n", "CRC failed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x16e, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
