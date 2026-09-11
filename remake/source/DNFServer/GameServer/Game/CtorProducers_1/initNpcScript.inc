bool initNpcScript(const char* dir, const char* listPath)
{
    // [b28d-solo-CtorProducers_1 判词·P303] ORIG 0x089fc1a8：strncpy(base,dir,0x100)；
    // strlen 三独立重调（P48 无 CSE：'\\' 判定、'/' 判定、追加 dst 各一次），
    // 末字符非 '\\' 且非 '/' 时**目录分隔符追加**；尾部 xor$1/test/je 双 mov 岛
    // = (load 返 1)。旧「N/P75 实参装载时刻封顶」判词作废：该段正是 P303 的
    // `__builtin_strcat(dst,"\\")` 内建展开（movl $2,0x8; movl $str,0x4; movl
    // base,(%esp); call strlen; add base; 双 mov 中转; call memcpy 8 条形）。
    strncpy(g_npcScriptBaseDirectory, dir, 0x100);
    if (g_npcScriptBaseDirectory[strlen(g_npcScriptBaseDirectory) - 1] != '\\')
    {
        if (g_npcScriptBaseDirectory[strlen(g_npcScriptBaseDirectory) - 1] != '/')
        {
            __builtin_strcat(g_npcScriptBaseDirectory, "\\");
        }
    }
    // 岛形复现（同 isExistNPCScript 技巧）：je 落点 = mov $1 ⇒ r==1 返 true。
    // P0/N96 清绑：改 bool callee 裸形（P168a①）。
    if (!loadRDARScriptListFile(&g_npcScriptFileList, g_npcScriptBaseDirectory,
                                listPath, 0))
        return false;
    return true;
}