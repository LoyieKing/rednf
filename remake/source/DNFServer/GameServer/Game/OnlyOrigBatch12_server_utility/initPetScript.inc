// ===========================================================================
// initPetScript  ORIG T 0x08a3581b：基目录 strncpy 0x100 + 尾部补 '\\' +
// loadRDARScriptListFile；返回 load 结果 == 1。
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] AE：P303 `__builtin_strcat` 复现 ORIG 8 条内建展开
// （手写 memcpy 为 P68/P75 实参求值序反）；clean 口径 0 残差（43/43 条逐条同构）。
bool initPetScript(const char* dir, const char* listPath)
{
    strncpy(g_petScriptBaseDirectory, dir, 0x100);
    // ORIG 尾部校验/补 '\\'：strlen 三次内联右值重算（无长度局部，P0/Q189：
    // 数据全程寄存器直传 ⇒ 原码无具名 len 局部）。最后一次 strlen 结果 + base
    // 作 memcpy dest，取 "\\" 2 字节。
    if (g_petScriptBaseDirectory[strlen(g_petScriptBaseDirectory) - 1] != '\\' &&
        g_petScriptBaseDirectory[strlen(g_petScriptBaseDirectory) - 1] != '/')
    {
        // [b28d-solo-OnlyOrigBatch12] P303：目录分隔符追加必须 builtin 形
        // （4.4 -O0 内建展开 = ORIG 8 条形；手写 memcpy 求值序反，P68/P75）。
        __builtin_strcat(g_petScriptBaseDirectory, "\\");
    }
    // ORIG 尾：xor $1/test/je→mov$1 布尔物化。P0/N96 清绑：改 bool callee
    // 裸形（P168a① `if (!call()) return false;`）。
    if (!loadRDARScriptListFile(&g_petScriptFileList, g_petScriptBaseDirectory,
                                listPath, 0))
        return false;
    return true;
}