// [b28d-47] verdict=DIFF(routed446) [N82] ORIG 0x088bb0ff orig/ours=181/171
//   p167 CLEAR（24/24）；残差部分为槽/寄存器分配跨版本差。
// [L32 r3 真缺口修复] ORIG 头部 0x088bb103..0x088bb11c 有三参数判空守卫
//   （`cmpl $0x0,0x8(%ebp); je <fail>` / `cmpl $0x0,0xc(%ebp); je <fail>` /
//    `cmpl $0x0,0x10(%ebp); jne <body>` + `mov $0x0,%ebx; jmp` = `return false`），
//   我方原实现直接解引用三参数（dir/filename/s）⇒ 缺 8 条指令且空指针语义缺失。
//   按 ORIG 原形补回；`== 0` 写法对齐 ORIG 的内存直比 `cmpl $0x0,N(%ebp)`。
bool LoadStreamFromPack(const char* dir, const char* filename, DNFLex::stream_data_t* s)
{
    if (dir == 0 || filename == 0 || s == 0)
        return false;

    std::string full(dir);
    full += filename;
    s->clear();
    // ORIG 0x88bb18f：拼串 == "Etc/channel_info.etc" 时改载
    // "./channel_info/channel_info.etc"（不查包）。
    if (full == "Etc/channel_info.etc")
    {
        if (g_dnfLex.setFileStreamData(s, "./channel_info/channel_info.etc"))
            return true;
        goto fail;
    }
    {
        unsigned char* content = 0;
        unsigned int len = 0;
        bool found = false;
        // ORIG 0x88bb1fc：PACK_ERROR（返回 1）＝包中未命中 → 尝试真实文件。
        if (g_script_package.get(full.c_str(), &content, &len) == PACK_ERROR)
        {
            char buf[0x400];
            std::strncpy(buf, full.c_str(), 0x400);
            if (findFileWithServerToken(buf))
            {
                full.assign(buf);
                if (g_script_package.get(full.c_str(), &content, &len) == PACK_OK)
                    found = 1;
            }
            if (!found)
            {
                if (content)
                    delete[] content;
                goto fail;
            }
        }
        if (g_dnfLex.setStringStreamData(s, reinterpret_cast<char*>(content), len))
            return true;
        if (content)
            delete[] content;
    }
fail:
    if (g_fpScriptLoadingErrorFunction)
        g_fpScriptLoadingErrorFunction(full.c_str());
    return false;
}