// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptExtras/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DNFLexWrapper.h"
#include "Arad_Script.h"
#include "Arad_ScriptLoader.h"   // 权威 Arad_ScriptLoader 声明（6 个脚本子对象基类）
#include "LogManager.h"
#include "RDARScriptStringManager.h"
#include "STNameList.h"
#include "SyncScriptDeps.h"
#include "PackSet.h"             // R3 迁入：loadStringTable 用 g_script_package

#include <cstdio>
#include <cstring>
#include <istream>
#include <vector>
#include "NpcScript.h"
class Comment;

// ============================================================================
// STScriptFileList（与 STScriptFileList.cpp 权威布局一致：+0x00
// std::map<int,std::string> m_fileMap；本 TU 局部完整定义以便成员访问）。
// ============================================================================


#include "ScriptExtras/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);

// R3 迁移（Main 批准）：以下两 static 原宿主 OnlyOrigBatch12/14.cpp（占位），
// ORIG host 属 RDARScript.cpp 域，正位本 TU。
extern PackSet g_script_package;   // 定义 OnlyOrigBatch12.cpp:61（ORIG B 0x09494fe0）
// 定义于 STEquipmentScript.cpp（ORIG B 0x0949b348）。
extern std::map<std::string, int> g_mapKeyNameIndexTable;

// ===========================================================================
// loadStringTable  ORIG t 0x088bb4b4（RDARScript.cpp 域）：
//   从脚本包取 "stringtable.bin"，成功则 setStringTableFromBuffer(data,size)。
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] AE：去 fail/ok 具名 bool 槽（P0：ORIG 短路链物化
// 直接落 al）+ 失败块 `if(data)` 与中/成功块裸 delete（ORIG 2/1 test 形，P141）；
// 72/72 逐条同构，clean 0 残差。ORIG 0x088bb4b4
// [b28e laneB P248 档①] 返回类型 int→bool：ORIG 调用点 0x088bb781 之后为
// `xor $0x1,%eax; test %al,%al; je`（3 条、无 `test %eax,%eax`）⇒ callee 必为 bool；
// int callee 的 `!f()` 在 4.4 恒产 `test %eax,%eax; sete %al`（P292/P168a 定论）。
static bool loadStringTable()
{
    unsigned char* data = 0;
    unsigned int size = 0;
    // [b28d-solo-OnlyOrigBatch12] 去掉 fail/ok 具名 bool 槽（P0：ORIG 短路链
    // 物化直接落 al，无栈槽）；失败块 delete 双条件照抄 ORIG（P184 死块）。
    if ((g_script_package.get("stringtable.bin", &data, &size) != 0) || (data == 0))
    {
        // 失败块显式 if + 裸 delete（ORIG 2 test）；成功/中段为裸 delete（1 test）。
        if (data)
            delete[] data;
        LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x15a,
                              "[compile] can't find string table bin file.");
        return false;
    }
    if (!g_dnfLex.setStringTableFromBuffer(data, static_cast<int>(size)))
    {
        delete[] data;
        LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x161,
                              "[compile] failed to import string table.");
        return false;
    }
    delete[] data;
    return true;
}

// ORIG T 0x088c1558 _ZL23importKeyNameIndexTablev（static）。
// [b28d-solo-OnlyOrigBatch14 判词·AE 归零] 本波三修：①P167 差集把
// `s = toTString(...); map[s]=v;` 改回内联实参 + 具名 `const char* p`
// 中转（ORIG 有 `_ZNSsC1EPKcRKSaIcE`+allocator 而无 `_ZNSsaSEPKc`；且
// 0x088c1668 `mov %eax,-0xc(%ebp)` 为 P0 具名局部槽）；②P15/P283 循环改
// `while ((token = getToken()) != 0)`（ORIG token 获取在循环尾条件块）；
// ③声明序 value 先/token 后（P309 先声明浅 ⇒ token@-0x10/value@-0x14）。
// 结果 AE-EQUAL 81=81，p167 CLEAR 13=13。
static bool importKeyNameIndexTable()
{
    if (!loadRDARScriptFile("", "Skill/RDARKeyIndex.dat"))
        return false;
    // [b28d-solo-OnlyOrigBatch14] P15/P283：ORIG 0x088c15a0 段 token 获取在
    // 循环尾条件块（`call getToken; mov %eax,-0x10; cmpl $0,-0x10; setne;
    // test; jne <body>`）⇒ 源形 `while ((token = getToken()) != 0)`；
    // 声明序 value 先/token 后（4.4 先声明浅 ⇒ token@-0x10/value@-0x14）。
    int value = 0;
    int token;
    while ((token = g_dnfLex.getToken()) != 0)
    {
        if (token == 2)
        {
            value = g_dnfLex.getDecimal();
        }
        else if (token == 6)
        {
            // [b28d-solo-OnlyOrigBatch14] P167 差集：ORIG 无
            // `_ZNSsaSEPKc`（operator=(const char*)），而有
            // `_ZNSsC1EPKcRKSaIcE`+allocator C1/D1（临时 string 构造）
            // ⇒ 原码把 toTString 结果直接作 operator[] 的 key 实参
            // （隐式 const char*→string 转换临时），非函数级 s 赋值。
            // P0：ORIG 0x088c1668 `mov %eax,-0xc(%ebp)`（toTString 结果入槽）
            // ⇒ 原码另有具名 `const char* p` 局部。
            const char* p = toTString(g_dnfLex.getCustomData());
            g_mapKeyNameIndexTable[p] = value;
        }
    }
    return true;
}

#include "ScriptExtras/open.inc"


// ---- ORIG RDARScript.cpp 域全局/静态（nm：g_szScriptFileBaseDirectory B
// 0x949b180；_ZL16s_bUseScriptPack d 0x93f4d24）----
char g_szScriptFileBaseDirectory[0x104];  // ORIG B 0x949b180
// [b28e P248 档② 字段同型化] char→bool：ORIG 访问器本体 `movzbl 0x93f4d24,%eax;
// pop %ebp; ret`（纯直返、无 bool 归一化）在「bool 返回 + char 字段」下 4.4 会插
// `test %al,%al; setne %al`（本体 AE→DIFF）；把**源字段一并 bool 化**（P248 档②/
// P300）后返回表达式本身已是 bool ⇒ 直返形两全（本体 identical 保持 + 消费点
// `call; xor $1; test %al; je` 与 ORIG 同形、gap 归零）。
static bool s_bUseScriptPack = false;     // ORIG d _ZL16s_bUseScriptPack 0x93f4d24

#include "ScriptExtras/UseScriptPack.inc"


// ORIG g_szScriptFilePriorityBaseDirectory B 0x0949b168 (0xc)：
// std::vector<std::string>（SetPriorityBaseDirectory push、
// getScriptFileFullPath 按序探测的脚本优先根目录表）。
std::vector<std::string> g_szScriptFilePriorityBaseDirectory;

#include "ScriptExtras/SetPriorityBaseDirectory.inc"


#include "ScriptExtras/isDbcsLead.inc"


#include "ScriptExtras/LexerOutput.inc"


#include "ScriptExtras/LexerError.inc"


#include "ScriptExtras/InitRDARScript.inc"


#include "ScriptExtras/saveRDARScriptListFile.inc"


#include "ScriptExtras/loadRDARNameFile.inc"


#include "ScriptExtras/addRDARScriptListFile.inc"


#include "ScriptExtras/InitRDARScriptStringManager.inc"


#include "ScriptExtras/parsing.inc"

