// ORIG T _Z14InitRDARScriptPKcbS0_Pi 0x088bb5d2：
//   1) SetHandler 按 compiled 选择 LoadStreamFromPack/Plain；
//   2) 规范化脚本根目录到 g_szScriptFileBaseDirectory（补 '\\'），空目录返回 false；
//   3) 置 s_bUseScriptPack=compiled；param_3 非空时强制 pack 模式并以其为包名；
//   4) pack 模式下 PackSet 打开并装载字符串表，结果写入 param_4（推断：ORIG 的
//      loadStringTable/importKeyNameIndexTable 为 ORIG TU 内 static，未单独还原）。
// [b28e laneB 本轮修 4 处，均有 ORIG 直证；旧 b28d-lane16「跨版本寄存器·槽位残差 N」作废]
//   ①P303 `__builtin_strcat(g_sz,"\\")`（ORIG 0x088bb676..0x088bb6a0 八条内建链；
//     旧形 `memcpy(base+len,"\\",2)` 只有 6 条且无 strlen/add/双 mov 中转）；
//   ②`int len` + `if (len <= 0)`（ORIG 0x088bb644 `cmpl $0x0,-0x10; jg` 有符号直证；
//     旧 `unsigned` 形产 `jne`）；
//   ③`s_bUseScriptPack = compiled ? 1 : 0;` 补**第一次**写（ORIG 0x088bb6a5 与
//     0x088bb6fb 两处同形 `movzbl -0x1c(%ebp),%eax; mov %al,&s_bUseScriptPack`，
//     旧源码只有第二处）；
//   ④`loadStringTable()` 返回 int→bool（ORIG 调用点 0x088bb781 `call; xor $0x1,%eax;
//     test %al,%al; je` 三条形；int callee 的 `!f()` 在 4.4 恒产 `test %eax,%eax;
//     sete %al` 四条，P292/P168a/P248 档①）。
// 预期：ours 131→135 与 ORIG 135 对齐（gap +4→0）。可整块回滚（本块 4 处均为独立语句）。
bool InitRDARScript(const char* dir, bool compiled, const char* file, int* err)
{
    g_dnfLex.SetHandler(&isDbcsLead, &LexerOutput, &LexerError,
                        compiled ? LoadStreamFromPack : LoadStreamFromPlain);

    std::strncpy(g_szScriptFileBaseDirectory, dir,
                 sizeof(g_szScriptFileBaseDirectory));
    // [b28e laneB P321-T2 直证] ORIG 0x088bb644 `cmpl $0x0,-0x10(%ebp); jg` 是**有符号**
    // 比较 ⇒ 原码 `int len`（unsigned 会生 ja/jbe 或 jne 直比，旧形即 `jne`）且判据是
    // `if (len <= 0) return false;`（4.1 生 `jg <跳过 return 块>` 正形）。
    int len = (int)std::strlen(g_szScriptFileBaseDirectory);
    if (len <= 0)
        return false;
    if (g_szScriptFileBaseDirectory[len - 1] != '\\' &&
        g_szScriptFileBaseDirectory[len - 1] != '/')
    {
        // [b28e laneB P303] ORIG 0x088bb676..0x088bb6a0 八条内建展开（`movl $2,0x8;
        // movl $str,0x4; movl $base,(%esp); call strlen; add $base,%eax; mov %eax,(%esp);
        // mov (%esp),%eax; mov %eax,(%esp); call memcpy`）⇒ 源形必为 __builtin_strcat；
        // 手写 memcpy(base+len,"\\",2) 恒 DIFF（旧判词「P209e19/P75 跨版本 N」作废）。
        __builtin_strcat(g_szScriptFileBaseDirectory, "\\");
    }
    // [b28e laneB P171 照抄] ORIG 0x088bb6a5/0x088bb6fb **两处**同形写
    // （`movzbl -0x1c(%ebp),%eax; mov %al,&s_bUseScriptPack`）：第一处在分隔符块之后、
    // packName 构造之前，第二处在 packName 之后 ⇒ 原码确有两句赋值，照抄。
    s_bUseScriptPack = compiled ? 1 : 0;

    std::string packName = "Script.pvf";
    s_bUseScriptPack = compiled ? 1 : 0;
    if (file != 0 && file[0] != 0)
    {
        s_bUseScriptPack = 1;
        packName = file;
    }

    // ORIG 0x088bb72d..0x088bb7ce（R3 迁移后补全的真实语义，替旧「假近似」）：
    //   if (s_bUseScriptPack) {
    //     rc = g_script_package.open(packName.c_str(), 0, PACK_LOAD_INDEX); // +0x18c
    //     if (err) *err = rc;                                              // +0x194..1a0
    //     if (rc) return false;                                            // +0x1a2
    //     if (!loadStringTable()) return false;                            // +0x1af
    //   }
    //   importKeyNameIndexTable();  // +0x1c2（返回值不检查，两路汇入）
    //   return true;                // +0x1c7
    if (s_bUseScriptPack)
    {
        int rc = g_script_package.open(packName.c_str(), 0, PACK_LOAD_INDEX);
        if (err)
            *err = rc;
        if (rc != 0)
            return false;
        if (!loadStringTable())
            return false;
    }
    importKeyNameIndexTable();
    return true;
}