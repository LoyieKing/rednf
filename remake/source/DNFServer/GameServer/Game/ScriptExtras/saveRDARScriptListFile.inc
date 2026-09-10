bool saveRDARScriptListFile(STScriptFileList* list, const char* dir, const char* file,
    std::map<int, int>* a,
    std::multimap<int, Comment*>* b)
{
    (void)dir; (void)a; (void)b;
    // [D01 · C05① 尾 fail 块形] ORIG 0x088bbf52 段 `cmpl $0,0x10(%ebp); je
    // <+0xfb 函数尾独立 mov $0,%eax 块>`；平铺早返 `if(file==0) return false;`
    // 被 4.4 折成 `jne <cont>; mov $0; jmp`（就地失败块）与 ORIG 尾块形不符
    // ⇒ 主体嵌套进 `if (file != 0) { … return true; } return false;`（探针
    // /tmp/tail_probe.cpp fA/fB，Main 转发 C05 实证）。
    if (file != 0)
    {
    std::FILE* fp = std::fopen(file, "wt");
    if (fp == 0)
        return false;
    // [b28d-lane16 P96/P270] ORIG 用非 const `_Rb_tree_iterator`（C1Ev/ppEv/ne/ptEv）；
    // const_iterator 会引入转换构造 C1ERKS_ + const 版 operator->（p167 EXTRA 2+2）。
    // 另：ORIG 有独立 `iterator::C1Ev`（默认构造）+ begin() 临时槽 ⇒ 先声明后赋值
    // （P50 复活形；p167 MISSING -1 C1Ev）。
    std::map<int, std::string>::iterator it;
    it = list->m_fileMap.begin();
    for (; it != list->m_fileMap.end(); ++it)
    {
        std::fprintf(fp, "%d\t", it->first);
        std::fprintf(fp, "`%s`", it->second.c_str());
    }
    std::fclose(fp);
    return true;
    }
    return false;
