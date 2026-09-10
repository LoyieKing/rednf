long file_ctrl(bio_st_st* b, int cmd, long larg, void* parg)
{
    static const char kFile[] = "bss_file.c";   // ORIG 0x8de22ee
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);   // [b+0x20]

    switch (cmd) {
    case 0x0b:                                  // 87ecaff==FLUSH fflush
        std::fflush(fp);
        return 1;

    case 0x03:                                  // 87ecc10 ftell
        return std::ftell(fp);

    case 0x08:                                  // 87eccf8 GET_CLOSE
        return b->shutdown;

    case 0x09:                                  // 87ecb72 SET_CLOSE
        b->shutdown = static_cast<int>(larg & 0xffffffffUL);   // mov %ebx,0x10
        return 1;

    case 0x06:                                  // 无此 case→default（ORIG cmp 0xb 先行）
    default:
        break;
    }

    if (cmd < 0x0b) {
        switch (cmd) {
        case 0x03: return std::ftell(fp);       // 0x03 已在上处理（不可达），保留对齐
        case 0x08: return b->shutdown;
        case 0x09: b->shutdown = static_cast<int>(larg); return 1;
        case 0x01:                              // fseek RESET
        case 0x80:
        case 0x85:
            return std::fseek(fp, static_cast<long>(larg), 0 /*SEEK_SET*/);
        case 0x02:                              // feof
            return std::feof(fp);
        case 0x6a: {                            // SET_FP（106）
            if (b->shutdown != 0) {
                if (b->init != 0 && fp != 0) {
                    std::fclose(fp);
                    b->ptr      = 0;            // +0x20=0
                    b->shutdown = 0;            // +0x14=0
                }
                b->init = 0;                    // +0x0c=0
            }
            int want_close = static_cast<int>(larg) & 1;
            b->shutdown = want_close;           // mov %ebx,0x10(%esi)
            b->ptr = parg;                      // mov %edi,0x20(%esi)
            b->init  = 1;
            return 1;
        }
        default:
            break;
        }
        if (cmd == 0x0c) return 1;              // 87ecba0 je 0xc → ret 1
        return 0;                               // 其余 <0x0b 分支 xor eax,eax
    }

    // cmd > 0x0b 区段
    if (cmd == 0x6b) {                          // GET_FP
        if (parg == 0)
            return 1;                           // 87ecc28 edi==0 → ccee ret1
        *static_cast<void**>(parg) = fp;        // mov %edx,(%edi)
        return 1;
    }
    if (cmd == 0x80 || cmd == 0x85)             // fseek 变体（ORIG je→共用 fseek 块）
        return std::fseek(fp, static_cast<long>(larg), 0);
    if (cmd == 0x6c) {                          // SET_FILENAME
        if (b->shutdown != 0) {
            if (b->init != 0 && fp != 0) {
                std::fclose(fp);
                b->ptr      = 0;
                b->shutdown = 0;
            }
            b->init = 0;
        }
        int close_flag = static_cast<int>(larg) & 1;
        b->shutdown = close_flag;               // mov %eax,0x10

        char mode_buf[16];                       // esp+0x2c 栈缓冲（5 字节足够）
        const bool is_append = (larg & 0x8) != 0;
        if (is_append) {
            if ((larg & 0x2) == 0) {
                BUF_strlcpy(mode_buf, "a", sizeof(mode_buf));        // 0x8de707d
            } else {
                BUF_strlcpy(mode_buf, "a+", sizeof(mode_buf));       // 0x8d01651
            }
        } else {
            int rw = static_cast<int>(larg) & 0x6;
            if (rw == 6) {
                BUF_strlcpy(mode_buf, "r+", sizeof(mode_buf));       // 0x8d0165e
            } else if ((larg & 0x4) != 0) {
                BUF_strlcpy(mode_buf, "w", sizeof(mode_buf));        // 0x8d034c7
            } else if ((larg & 0x2) != 0) {
                BUF_strlcpy(mode_buf, "r", sizeof(mode_buf));        // 0x8d78111
            } else {
                ERR_put_error(0x20, 0x74, 0x65, kFile, 0x17b);
                return 0;                       // xor eax,eax
            }
        }

        std::FILE* new_fp =
            reinterpret_cast<std::FILE*>(
                fopen64(static_cast<const char*>(parg), mode_buf));
        if (new_fp == 0) {
            int e = *__errno_location();
            ERR_put_error(2, 1, e, kFile, 0x18e);
            ERR_add_error_data(5,
                               static_cast<const char*>(parg),  // 文件名（ebx/edi）
                               "', '",                          // ORIG 0x8de22fc
                               mode_buf,                        // ebx 模式缓冲
                               "fopen('(",                      // ORIG 0x8de2300
                               "')");                           // ORIG 0x8de22f9
            ERR_put_error(0x20, 0x74, 0x74, kFile, 0x190);
            return 0;
        }
        b->ptr = new_fp;                        // mov %eax,0x20(%esi)
        b->init = 1;
        BIO_clear_flags(b, 0);                  // 87eccdf call BIO_clear_flags
        return 1;
    }
    return 0;                                   // 默认 xor eax,eax
