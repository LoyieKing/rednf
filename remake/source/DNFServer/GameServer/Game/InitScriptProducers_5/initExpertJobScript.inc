// ============================================================================
// initExpertJobScript —— ORIG 0x0896d830（init.md:524 +0x0a44 调用点，
// 失败路径 TRACE "Load expertjob.lst fail" 0x1982 区）。
//   strncpy(g_expertJobScriptBaseDirectory(0x94f7660), dir, 0x100) + 追加 '\\'
//   → loadRDARScriptListFile(&g_expertJobScriptFileList(0x94f7760), baseDir,
//     list, NULL) → 结果非 0 返回 true。
// 消费方：CExpertJobList::init（ORIG 0x896d8ec 直接遍历该对象 m_fileMap）。
// ============================================================================
// [b28d-solo-InitScriptProducers_5 AE 43/43] 旧 N(P209/P75) 判词作废：
// `__builtin_strcat(dst,"\\")` 内建展开即 ORIG @0x0896d87x 的
// `movl $0x2,0x8(%esp); movl $<str>,0x4(%esp); movl $<base>,(%esp); call strlen;
// add $<base>,%eax; mov %eax,(%esp); mov (%esp),%eax; mov %eax,(%esp); call memcpy`
// 8 条形（探针 /tmp/probe55b.cpp fSB 逐条同形）；手写 memcpy(dst+strlen,..) 才 DIFF。
bool initExpertJobScript(char* dir, char* list)
{
    strncpy(g_expertJobScriptBaseDirectory, dir, 0x100);
    // P49 禁 TU helper：内联 strlen×2 条件 + 内建 strcat 展开（无空串防御，
    // ORIG len==0 UB，P140）。
    if (g_expertJobScriptBaseDirectory[strlen(g_expertJobScriptBaseDirectory) - 1] != '\\' &&
        g_expertJobScriptBaseDirectory[strlen(g_expertJobScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_expertJobScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_expertJobScriptFileList,
                                g_expertJobScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}