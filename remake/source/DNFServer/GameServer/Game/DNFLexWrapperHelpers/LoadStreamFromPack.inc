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
        // [G17 2026-09-11 结构实证] ORIG 0x88bb1bc..0x88bb1cc：
        //   `xor $0x1,%eax; test %al,%al; je 0x88bb2fe(success: mov $1,%ebx); jmp 0x88bb30f(fail)`
        //   ⇒ 源码形 = 「否定式 goto fail + 显式 goto success」（双跳转布局），
        //   旧写 `if (ok) return true;` 产 `test; je` 无 xor 且块序相反。
        if (!g_dnfLex.setFileStreamData(s, "./channel_info/channel_info.etc"))
            goto fail;
        goto success;
    }
    {
        unsigned char* content = 0;
        // [G17 2026-09-11] ORIG 0x88bb1d1 段仅 `movl $0x0,-0x14(%ebp)`（content=0）后
        //   直接 `lea -0x18(%ebp),%edx`（&len 传参）⇒ 原码 len **无零初始化**
        //   （P171：原码不写即不写；旧写 `= 0` 多 1 条）。
        unsigned int len;
        // ORIG 0x88bb1fc：PACK_ERROR（返回 1）＝包中未命中 → 尝试真实文件。
        if (g_script_package.get(full.c_str(), &content, &len) == PACK_ERROR)
        {
            // [G17 2026-09-11] ORIG 0x88bb20f `movb $0x0,-0x9(%ebp)` = found=false 在
            //   PACK_ERROR 块内、strncpy 之前（旧写把 found 声明在块外 → 4 条差）。
            bool found = false;
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
        // [G17 2026-09-11] 同上：ORIG 0x88bb22b 段 `xor $0x1; test; je <成功>`。
        if (!g_dnfLex.setStringStreamData(s, reinterpret_cast<char*>(content), len))
        {
            if (content)
                delete[] content;
            goto fail;
        }
        goto success;
    }
success:
    return true;
fail:
    if (g_fpScriptLoadingErrorFunction)
        g_fpScriptLoadingErrorFunction(full.c_str());
    return false;
}