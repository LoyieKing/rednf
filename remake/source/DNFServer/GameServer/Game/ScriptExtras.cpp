// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScriptExtras/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— RDAR 脚本装载辅助（../RDARScript.cpp 域）
//   addRDARScriptListFile        0x088bc1ee
//   loadRDARNameFile             0x088bc068
//   saveRDARScriptListFile       0x088bbf52
//   InitRDARScriptStringManager  0x088bff9c
//   Arad_ScriptLoader::open      0x081a1eec（vtable: open=+0, parsing=+4；
//                                ORIG 无析构符号，C1 仅写 vptr）
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


// 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77）。
// [L10-2] P292/档①：ORIG 调用点 0x088bffdc `xor $0x1,%eax; test %al,%al; je`
// ⇒ 该 callee 实为 bool（int 声明会产 `test%eax; sete%al`）。返回类型不入
// mangled/布局；定义宿主 STScriptFileList.cpp 应同步（已上报 Main）。
#include "ScriptExtras/loadRDARScriptListFile.inc"
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
static int loadStringTable()
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
        return 0;
    }
    if (!g_dnfLex.setStringTableFromBuffer(data, static_cast<int>(size)))
    {
        delete[] data;
        LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x161,
                              "[compile] failed to import string table.");
        return 0;
    }
    delete[] data;
    return 1;
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

// ORIG 0x081a1eec：装载脚本到 DNFLexWrapper 输入流，整块读入缓冲（末字节 0xff），
// 经 vtable 槽1 调 parsing 解析并返回其结果（非零=成功）。装载失败返回 0x10（非零）。
#include "ScriptExtras/open.inc"
}

// ---- ORIG RDARScript.cpp 域全局/静态（nm：g_szScriptFileBaseDirectory B
// 0x949b180；_ZL16s_bUseScriptPack d 0x93f4d24）----
char g_szScriptFileBaseDirectory[0x104];  // ORIG B 0x949b180
// [b28e P248 档② 字段同型化] char→bool：ORIG 访问器本体 `movzbl 0x93f4d24,%eax;
// pop %ebp; ret`（纯直返、无 bool 归一化）在「bool 返回 + char 字段」下 4.4 会插
// `test %al,%al; setne %al`（本体 AE→DIFF）；把**源字段一并 bool 化**（P248 档②/
// P300）后返回表达式本身已是 bool ⇒ 直返形两全（本体 identical 保持 + 消费点
// `call; xor $1; test %al; je` 与 ORIG 同形、gap 归零）。
static bool s_bUseScriptPack = false;     // ORIG d _ZL16s_bUseScriptPack 0x93f4d24

// ORIG T 0x088bb7cf：`movzbl 0x93f4d24,%eax; pop %ebp; ret` 直返字节。
// [D01 属主更新 2026-09-10 · 采纳 E07 实测] 旧 b28d-lane16 判词「返回类型 bool→char」
// **作废**：单改返回类型时「bool 返回 + char 字段」会被 4.4 插 `test %al,%al;
// setne %al`（本体 identical −1，已实测回退）；按 P248 档②/P300 **源字段与返回类型
// 成对 bool 化**后返回表达式本身已是 bool ⇒ 直返形两全：本体仍 `movzbl mem; ret`
// （size 0xc = ORIG 0x088bb7cf 0xc）、消费点 LinkMiscSecuTimerSolo.cpp::
// findFileWithServerToken 182→**181 = ORIG（gap 0）**。跨 TU extern 声明同步 bool
// （该点跨 TU 访问 ORIG TU-local static，受 P189 豁免形不变）。
#include "ScriptExtras/UseScriptPack.inc"
}

// ORIG g_szScriptFilePriorityBaseDirectory B 0x0949b168 (0xc)：
// std::vector<std::string>（SetPriorityBaseDirectory push、
// getScriptFileFullPath 按序探测的脚本优先根目录表）。
std::vector<std::string> g_szScriptFilePriorityBaseDirectory;

// ORIG T _Z24SetPriorityBaseDirectoryPKc 0x088bb40a
// （报告 _lt_global_gt_/SetPriorityBaseDirectory.md）：
// 构造临时 string；非空才 push_back（空串忽略）。
#include "ScriptExtras/SetPriorityBaseDirectory.inc"
}

// ORIG T _Z10isDbcsLeadc 0x88bb367：DBCS 首字节判定（最高位为 1）。
// 写回 `(c & 0x80) != 0` 使 4.4.x 生成 ORIG 完全相同序列
// （movzbl -0x4(%ebp),%eax; shr $0x7,%al; 无 setne）。
#include "ScriptExtras/isDbcsLead.inc"
}

// ORIG T _Z11LexerOutputPKci 0x88bb37c：词法输出走 LogManager 日志。
#include "ScriptExtras/LexerOutput.inc"
}

// ORIG T _Z10LexerErrorPKc 0x88bb3c7：词法错误走 LogManager 日志。
#include "ScriptExtras/LexerError.inc"
}

// ORIG T _Z14InitRDARScriptPKcbS0_Pi 0x088bb5d2：
//   1) SetHandler 按 compiled 选择 LoadStreamFromPack/Plain；
//   2) 规范化脚本根目录到 g_szScriptFileBaseDirectory（补 '\\'），空目录返回 false；
//   3) 置 s_bUseScriptPack=compiled；param_3 非空时强制 pack 模式并以其为包名；
//   4) pack 模式下 PackSet 打开并装载字符串表，结果写入 param_4（推断：ORIG 的
//      loadStringTable/importKeyNameIndexTable 为 ORIG TU 内 static，未单独还原）。
// [b28d-lane16 DIFF] ORIG 088bb5d2：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/InitRDARScript.inc"
}

// ORIG 0x088bbf52：把 list->m_fileMap 以 "%d\t`%s`" 写入 param_3 指定文本文件。
// param_1/2/4/5 在 ORIG 中未被使用。
// [D01 · C05① 尾 fail 块形 2026-09-10] DIFF→AE：ORIG 088bbf52 `cmpl $0,0x10(%ebp);
// je <+0xfb 尾块>`（失败出口落函数尾独立 mov $0,%eax 块）⇒ 主体嵌套进
// `if (file != 0) { … return true; } return false;` 复现尾块；平铺早返恒被 4.4 折成
// 就地 `jne <cont>; mov $0; jmp`。原 lane16 判词（p167 CLEAR/跨版本残差 N）作废。
#include "ScriptExtras/saveRDARScriptListFile.inc"
}

// ORIG 0x088bc068：装载名字表文件（name id 对），插入 STNameList；
// 插入失败（重复键）时记日志；返回列表是否非空（推断：ORIG 尾部 begin()==end() 判断）。
// [b28d-lane16 DIFF] ORIG 088bc068：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/loadRDARNameFile.inc"
}

// ORIG 0x088bc1ee：装载列表文件（id name 对）加入 m_fileMap；重复键记日志并跳过。
// [b28d-lane16 DIFF] ORIG 088bc1ee：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/addRDARScriptListFile.inc"
}

// ORIG 0x088bff9c：
//   1) 用 g_scriptStringManager_ 的文件列表装载 dir 指定的列表文件；
//   2) 逐项按 s_bUseScriptPack 选择 Pack/Plain 装流，调 loadStringFile；
//   3) 任一失败向 err 追加 ErrorLog（code=0x80000003，message=文件名）并置失败。
// [b28d-lane16 DIFF] ORIG 088bff9c：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/InitRDARScriptStringManager.inc"
}

// ORIG W 0x081a4054：默认解析器直接返回成功（1）。
#include "ScriptExtras/parsing.inc"
}
