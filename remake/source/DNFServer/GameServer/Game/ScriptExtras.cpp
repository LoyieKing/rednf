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
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* unused);

// ---- ORIG RDARScript.cpp 域全局/静态（nm：g_szScriptFileBaseDirectory B
// 0x949b180；_ZL16s_bUseScriptPack d 0x93f4d24）----
char g_szScriptFileBaseDirectory[0x104];  // ORIG B 0x949b180
static char s_bUseScriptPack = 0;         // ORIG d _ZL16s_bUseScriptPack 0x93f4d24

// ORIG T 0x088bb7cf：`movzbl 0x93f4d24,%eax; pop %ebp; ret` 直返字节。
// [b28d-lane16] 返回类型 bool→char（P9：直返无 setne/test 规范化；bool 返回
// 使 4.4 多 `test %al,%al; setne %al` 两条 ⇒ 本件 DIFF）。调用点
// LinkMiscSecuTimerSolo.cpp 的 extern 声明需同步为 char（该点跨 TU 访问
// ORIG TU-local static，受 P189 豁免）。
char UseScriptPack()
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
void SetPriorityBaseDirectory(const char* dir)
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
bool isDbcsLead(char c)
{
    return (c & 0x80) != 0;
}

// ORIG T _Z11LexerOutputPKci 0x88bb37c：词法输出走 LogManager 日志。
void LexerOutput(const char* buf, int size)
{
    (void)size;
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x12f, "%s - %s",
                          __FUNCTION__, toTString(buf));
}

// ORIG T _Z10LexerErrorPKc 0x88bb3c7：词法错误走 LogManager 日志。
void LexerError(const char* msg)
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
// [b28d-lane16 DIFF] ORIG 088bb5d2：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
bool InitRDARScript(const char* dir, bool compiled, const char* file, int* err)
{
    g_dnfLex.SetHandler(&isDbcsLead, &LexerOutput, &LexerError,
                        compiled ? LoadStreamFromPack : LoadStreamFromPlain);

    std::strncpy(g_szScriptFileBaseDirectory, dir,
                 sizeof(g_szScriptFileBaseDirectory));
    unsigned int len = static_cast<unsigned int>(
        std::strlen(g_szScriptFileBaseDirectory));
    if (len == 0)
        return false;
    if (g_szScriptFileBaseDirectory[len - 1] != '\\' &&
        g_szScriptFileBaseDirectory[len - 1] != '/')
    {
        std::memcpy(g_szScriptFileBaseDirectory + len, "\\", 2);
    }

    std::string packName = "Script.pvf";
    s_bUseScriptPack = compiled ? 1 : 0;
    if (file != 0 && file[0] != 0)
    {
        s_bUseScriptPack = 1;
        packName = file;
    }

    if (err)
        *err = 0;
    if (s_bUseScriptPack)
    {
        // 推断：此处 ORIG 调 g_script_package.open(packName, "", PACK_LOAD)，
        // 失败返回 false、成功继续 loadStringTable()；PackSet 未建模，按
        // Plain 流可用性近似（LoadStreamFromPack 内部已回退文件流）。
        DNFLex::stream_data_t probe;
        if (!LoadStreamFromPack(g_szScriptFileBaseDirectory,
                                packName.c_str(), &probe))
        {
            return false;
        }
        probe.destroy();
    }
    return true;
}

// ORIG 0x088bbf52：把 list->m_fileMap 以 "%d\t`%s`" 写入 param_3 指定文本文件。
// param_1/2/4/5 在 ORIG 中未被使用。
// [b28d-lane16 DIFF] ORIG 088bbf52：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
bool saveRDARScriptListFile(STScriptFileList* list, const char* dir, const char* file,
    std::map<int, int>* a,
    std::multimap<int, Comment*>* b)
{
    (void)dir; (void)a; (void)b;
    if (file == 0)
        return false;
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

// ORIG 0x088bc068：装载名字表文件（name id 对），插入 STNameList；
// 插入失败（重复键）时记日志；返回列表是否非空（推断：ORIG 尾部 begin()==end() 判断）。
// [b28d-lane16 DIFF] ORIG 088bc068：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
bool loadRDARNameFile(STNameList* list, const char* dir, const char* file)
{
    if (!loadRDARScriptFile(dir, file))
        return false;
    list->Clear();

    std::string name;
    bool ok = false;
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
bool addRDARScriptListFile(STScriptFileList* list, const char* dir, const char* file)
{
    if (!loadRDARScriptFile(dir, file))
        return false;

    std::string name;
    bool ok = false;
    for (;;)
    {
        int idx = ScanInt(&ok);
        if (!ok)
            break;
        if (!ScanStr(&name))
            break;
        bool isNew = (list->m_fileMap.find(idx) == list->m_fileMap.end());
        if (isNew)
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
bool InitRDARScriptStringManager(const char* dir, const char* file,
                                 std::vector<ErrorLog>* err)
{
    if (file == 0)
        file = "";

    STScriptFileList* fileList =
        static_cast<STScriptFileList*>(g_scriptStringManager_.getStringFileList());
    if (!loadRDARScriptListFile(fileList, "", dir, 0))
        return false;

    // s_bUseScriptPack 由 InitRDARScript 置位（ORIG 同一 RDARScript.cpp 域静态）。
    DNFLex::TLoadStream loadStream =
        s_bUseScriptPack ? LoadStreamFromPack : LoadStreamFromPlain;

    bool allOk = true;
    int table = 0;
    for (std::map<int, std::string>::const_iterator it = fileList->m_fileMap.begin();
         it != fileList->m_fileMap.end(); ++it, ++table)
    {
        const char* name = it->second.c_str();
        DNFLex::stream_data_t stream;
        if (loadStream(file, name, &stream))
        {
            std::istream* in = static_cast<std::istream*>(stream.stream);
            bool ok = g_scriptStringManager_.loadStringFile(in, name, table, err);
            allOk = ok && allOk;
            stream.destroy();
        }
        else
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
    }
    return allOk;
}

// ORIG 0x081a1eec：装载脚本到 DNFLexWrapper 输入流，整块读入缓冲（末字节 0xff），
// 经 vtable 槽1 调 parsing 解析并返回其结果（非零=成功）。装载失败返回 0x10（非零）。
int Arad_ScriptLoader::open(const char* path)
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

// ORIG W 0x081a4054：默认解析器直接返回成功（1）。
int Arad_ScriptLoader::parsing(char* line)
{
    (void)line;
    return 1;
}
