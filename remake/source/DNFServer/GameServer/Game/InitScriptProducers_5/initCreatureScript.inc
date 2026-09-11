// ============================================================================
// initCreatureScript —— ORIG 0x08a35d37（init.md:4579 +0x5d8a；失败 TRACE
// "- Import Creature Creature.lst fail" 0x215f）。
// ① 目录拷贝（0x94ff4e0，0x100）；② loadRDARScriptFile(baseDir, expTable)
//   失败即 false（ExpTable.tbl 先行装载）；③ 循环 i=0..0x62 共 99 次
//   ScanInt(NULL) 写入 CreatureScript::ExpTable_E[i]
//   （B _ZN14CreatureScript9ExpTable_E 0x94fefa0，readelf size 396=99*4）；
// ④ loadRDARScriptListFile 列表装载（0x94ff5e0）取非 0。
// g_creatureScriptFileList 权威型 STScriptFileList（CreatureScript.h:285）。
// ============================================================================
// [b28d-solo AE 66/66] 旧 N(P209/P75) 判词作废：①__builtin_strcat 内建展开；
// ②第 1 实参须**直接** `&g_creatureScriptFileList`（权威头已是 STScriptFileList）——
// 旧代码 `reinterpret_cast<STScriptFileList*>(&map视图)` 令 4.4 物化
// `mov $imm,%eax; mov %eax,(%esp)` 且挤占 %eax（后续实参退化 %edx），去 cast 后
// `movl $imm,(%esp)` + `mov 0xc(%ebp),%eax` 与 ORIG 全等（探针 /tmp/probe55d.cpp）。
// ExpTable 循环体（ScanInt/倒计数/setle 物化）本已逐条全等。
bool initCreatureScript(const char* dir, const char* list, const char* expTable)
{
    strncpy(g_creatureScriptBaseDirectory, dir, 0x100);
    if (g_creatureScriptBaseDirectory[strlen(g_creatureScriptBaseDirectory) - 1] != '\\' &&
        g_creatureScriptBaseDirectory[strlen(g_creatureScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_creatureScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, expTable))
        return false;
    for (int i = 0; i <= 0x62; ++i)
        CreatureScript::ExpTable_[i] = ScanInt((bool*)0);
    if (!loadRDARScriptListFile(&g_creatureScriptFileList,
                                g_creatureScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}