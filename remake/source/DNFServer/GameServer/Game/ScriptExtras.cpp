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


// 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77）。
// [L10-2] P292/档①：ORIG 调用点 0x088bffdc `xor $0x1,%eax; test %al,%al; je`
// ⇒ 该 callee 实为 bool（int 声明会产 `test%eax; sete%al`）。返回类型不入
// mangled/布局；定义宿主 STScriptFileList.cpp 应同步（已上报 Main）。
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

// ORIG 0x081a1eec：装载脚本到 DNFLexWrapper 输入流，整块读入缓冲（末字节 0xff），
// 经 vtable 槽1 调 parsing 解析并返回其结果（非零=成功）。装载失败返回 0x10（非零）。
#include "ScriptExtras/open.inc"
{
    if (!loadRDARScriptFile("", path))
        return 0x10;

    int size = g_dnfLex.getInputStreamSize();
    char* buffer = new char[size + 1];
    std::memset(buffer, 0, size + 1);
    static_cast<std::istream*>(g_dnfLex.getInputStream())->read(buffer, size);
    buffer[size] = static_cast<char>(0xff);

    int ok = parsing(buffer);
    if (ok)
    {
        delete[] buffer;
        return ok;
    }
    delete[] buffer;
    return 0;
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
{
    return s_bUseScriptPack;
}

// ORIG g_szScriptFilePriorityBaseDirectory B 0x0949b168 (0xc)：
// std::vector<std::string>（SetPriorityBaseDirectory push、
// getScriptFileFullPath 按序探测的脚本优先根目录表）。
std::vector<std::string> g_szScriptFilePriorityBaseDirectory;

// ORIG T _Z24SetPriorityBaseDirectoryPKc 0x088bb40a
// （报告 _lt_global_gt_/SetPriorityBaseDirectory.md）：
// 构造临时 string；非空才 push_back（空串忽略）。
#include "ScriptExtras/SetPriorityBaseDirectory.inc"
{
    std::string path(dir);
    if (path.length() == 0)
    {
    }
    else
        g_szScriptFilePriorityBaseDirectory.push_back(path);
}

// ORIG T _Z10isDbcsLeadc 0x88bb367：DBCS 首字节判定（最高位为 1）。
// 写回 `(c & 0x80) != 0` 使 4.4.x 生成 ORIG 完全相同序列
// （movzbl -0x4(%ebp),%eax; shr $0x7,%al; 无 setne）。
#include "ScriptExtras/isDbcsLead.inc"
{
    return (c & 0x80) != 0;
}

// ORIG T _Z11LexerOutputPKci 0x88bb37c：词法输出走 LogManager 日志。
#include "ScriptExtras/LexerOutput.inc"
{
    (void)size;
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x12f, "%s - %s",
                          __FUNCTION__, toTString(buf));
}

// ORIG T _Z10LexerErrorPKc 0x88bb3c7：词法错误走 LogManager 日志。
#include "ScriptExtras/LexerError.inc"
{
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x134, "%s",
                          toTString(msg));
}

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
#include "ScriptExtras/InitRDARScript.inc"
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

// ORIG 0x088bbf52：把 list->m_fileMap 以 "%d\t`%s`" 写入 param_3 指定文本文件。
// param_1/2/4/5 在 ORIG 中未被使用。
// [D01 · C05① 尾 fail 块形 2026-09-10] DIFF→AE：ORIG 088bbf52 `cmpl $0,0x10(%ebp);
// je <+0xfb 尾块>`（失败出口落函数尾独立 mov $0,%eax 块）⇒ 主体嵌套进
// `if (file != 0) { … return true; } return false;` 复现尾块；平铺早返恒被 4.4 折成
// 就地 `jne <cont>; mov $0; jmp`。原 lane16 判词（p167 CLEAR/跨版本残差 N）作废。
#include "ScriptExtras/saveRDARScriptListFile.inc"
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
}

// ORIG 0x088bc068：装载名字表文件（name id 对），插入 STNameList；
// 插入失败（重复键）时记日志；返回列表是否非空（推断：ORIG 尾部 begin()==end() 判断）。
// [b28d-lane16 DIFF] ORIG 088bc068：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/loadRDARNameFile.inc"
{
    if (!loadRDARScriptFile(dir, file))
        return false;
    list->Clear();

    // [L3] P309 头内声明序按 ORIG 槽浅→深重排：ORIG dead(-0xc) 最浅、ok(-0x2d)、
    // name(-0x34) 最深 ⇒ 4.4 头内「先声明浅」故 dead→ok→name。
    // [L10-2] 判据⑫ widthscan：ORIG `clr_b=0`（无带初始化器的 bool 局部）⇒
    // 按 P171 照抄去掉 `= false`；ok 由 ScanInt 写入后读取，语义等价。
    bool ok;
    std::string name;
    // [L3] ORIG 0x088bc0a9 `movl $0x1,-0xc(%ebp)`，全函数无读点 = 原码遗留的
    // 死初始化具名 int 局部（P171/P232 照抄原则）；补回以复现 ORIG 槽图。
    int deadFlag = 1;
    for (;;)
    {
        if (!ScanStr(&name))
            break;
        int id = ScanInt(&ok);
        if (!ok)
            break;
        // [b28d-lane16 P192] ORIG 0x088bc068 直接调 StringUnorderedMap::insert(PKc,RKi)
        // 并用返回值 .second 判重复（ORIG nm 无 STNameList::Insert/IsEmpty 符号）；
        // 原 GetID+Insert 两查形多 EXTRA c_str/GetID 并缺 insert 调用。
        StringUnorderedMap<CharStringHashTrait, int>::InsertResult r =
            list->m_map.insert(name.c_str(), id);
        if (!r.second)
        {
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 1000,
                "ITEM NAME FILE LIST LOAD FAIL! [%s%s][%s]", dir, file,
                name.c_str());
        }
    }
    // ORIG 尾部 begin()==end() 直发（无 STNameList::IsEmpty 符号；p167 用 eq 非 ne）。
    return !(list->m_map.begin() == list->m_map.end());
}

// ORIG 0x088bc1ee：装载列表文件（id name 对）加入 m_fileMap；重复键记日志并跳过。
// [b28d-lane16 DIFF] ORIG 088bc1ee：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/addRDARScriptListFile.inc"
{
    if (!loadRDARScriptFile(dir, file))
        return false;

    // [L3] P309 头部声明律：ORIG name@-0x2c（深）/ idx@-0x24（浅）⇒ 4.4 头内声明
    // 「先声明浅」⇒ idx 必须先声明（原循环内声明使 name 占浅槽、idx 占深槽，反向）。
    int idx;
    // [L3] 头内声明序 = ORIG 浅→深（idx -0x24 / ok -0x25 / name -0x2c）：
    // 4.4 头内声明「先声明浅」，故 ok 必须在 name 之前。
    // [L10-2] 判据⑫：同 loadRDARNameFile（ORIG clr_b=0）⇒ 去初始化器。
    bool ok;
    std::string name;
    for (;;)
    {
        idx = ScanInt(&ok);
        if (!ok)
            break;
        if (!ScanStr(&name))
            break;
        // [L3] P198② 镜像：ORIG `test %al,%al` 直测（无槽无 cmpb）⇒ 谓词整体内联
        // 回 if 条件位（具名 bool isNew 使 4.4 落 `mov %al,-0x9; cmpb $0,-0x9`）。
        if (list->m_fileMap.find(idx) == list->m_fileMap.end())
        {
            // [b28d-lane16 P192] ORIG 0x088bc2?? 直接构造 value_type（mangled
            // `C1IRiRSsEEOT_OT0_` = pair<const int,string>(int&,string&)）；
            // make_pair 生成值 pair + 转换构造 + 额外 D1（p167 EXTRA D1Ev×2）。
            list->m_fileMap.insert(std::map<int, std::string>::value_type(idx, name));
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 0x418,
                "SCRIPT FILE LIST ADD FAIL! - DUPLICATED DATA [%s%s][%d]",
                dir, file, idx);
        }
    }
    return true;
}

// ORIG 0x088bff9c：
//   1) 用 g_scriptStringManager_ 的文件列表装载 dir 指定的列表文件；
//   2) 逐项按 s_bUseScriptPack 选择 Pack/Plain 装流，调 loadStringFile；
//   3) 任一失败向 err 追加 ErrorLog（code=0x80000003，message=文件名）并置失败。
// [b28d-lane16 DIFF] ORIG 088bff9c：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
#include "ScriptExtras/InitRDARScriptStringManager.inc"
                                 std::vector<ErrorLog>* err)
{
    if (file == 0)
        file = "";

    // ORIG 在本函数发**三处** getStringFileList() 调用点（0x088bff9c 内
    // +0x1c / +0x6f / +0x92）：列表装载、循环外 size()、循环内 GetFileName(i)
    // 各自重新取一次（无缓存局部变量）。照抄调用点结构（4.4 -O0 逐点对应）。
    if (!loadRDARScriptListFile(
            static_cast<STScriptFileList*>(g_scriptStringManager_.getStringFileList()),
            "", dir, 0))
        return false;

    // s_bUseScriptPack 由 InitRDARScript 置位（ORIG 同一 RDARScript.cpp 域静态）。
    DNFLex::TLoadStream loadStream =
        s_bUseScriptPack ? LoadStreamFromPack : LoadStreamFromPlain;

    bool allOk = true;
    // ORIG 0x088c001a：循环外一次 map<int,string>::size()（4.1 out-of-line
    // 0x086d2cd8）存槽 -0x14；循环 i<size、name 经 STScriptFileList::
    // GetFileName(i)（0x088bad4e，0x088c0044 call）取，索引 i 同时是
    // loadStringFile 的 table 形参（-0x10 槽）。
    // [L10-2] P183/P0：ORIG 0x088c0012 `call getStringFileList; mov %eax,(%esp);
    // call size()`（无具名 fileList 槽往返）⇒ 内联，省 -0x18 槽。
    int count = static_cast<int>(static_cast<STScriptFileList*>(
        g_scriptStringManager_.getStringFileList())->m_fileMap.size());
    for (int table = 0; table < count; ++table)
    {
        const char* name = static_cast<STScriptFileList*>(
            g_scriptStringManager_.getStringFileList())->GetFileName(table);
        DNFLex::stream_data_t stream;
        // [L10-2] 判据①/档①：ORIG 0x088c0065 `call *%eax; xor $0x1,%eax; test
        // %al,%al; je <成功路径>` ⇒ 负向判定（bool callee），失败路径直落；
        // 且成功路径 allOk 物化为 `movzbl %al,%eax; and %ebx,%eax; test; setne
        // %al; mov %al,-0x15`（0x088c0109..0x088c0113）= 单表达式 `f() && allOk`，
        // 无具名 ok 局部、无具名 istream* in（ORIG 直接读 stream 首字段）。
        if (!loadStream(file, name, &stream))
        {
            allOk = false;
            if (err != 0)
            {
                ErrorLog entry;
                entry.m_code = static_cast<int>(0x80000003);
                entry.m_message = name;
                err->push_back(entry);
            }
        }
        else
        {
            // [L10-2] ORIG 0x088c0109 段无短路分支，直接
            // `movzbl %al,%eax; and %ebx,%eax; test; setne %al; mov %al,-0x15`
            //（allOk 先入 %ebx 跨 call 保活）⇒ 原码为按位与物化（非短路 &&）。
            allOk = allOk & g_scriptStringManager_.loadStringFile(
                                 static_cast<std::istream*>(stream.stream), name, table, err);
            stream.destroy();
        }
    }
    return allOk;
}

// ORIG W 0x081a4054：默认解析器直接返回成功（1）。
#include "ScriptExtras/parsing.inc"
{
    (void)line;
    return 1;
}
