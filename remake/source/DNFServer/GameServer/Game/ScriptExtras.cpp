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
class Comment;

// ============================================================================
// STScriptFileList（与 STScriptFileList.cpp 权威布局一致：+0x00
// std::map<int,std::string> m_fileMap；本 TU 局部完整定义以便成员访问）。
// ============================================================================
class STScriptFileList
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00
};

// 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77）。
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* unused);

// ---- ORIG RDARScript.cpp 域全局/静态（nm：g_szScriptFileBaseDirectory B
// 0x949b180；_ZL16s_bUseScriptPack d 0x93f4d24）----
char g_szScriptFileBaseDirectory[0x104];  // ORIG B 0x949b180
static char s_bUseScriptPack = 0;         // ORIG d _ZL16s_bUseScriptPack 0x93f4d24

// ORIG g_szScriptFilePriorityBaseDirectory B 0x0949b168 (0xc)：
// std::vector<std::string>（SetPriorityBaseDirectory push、
// getScriptFileFullPath 按序探测的脚本优先根目录表）。
std::vector<std::string> g_szScriptFilePriorityBaseDirectory;

// ORIG T _Z24SetPriorityBaseDirectoryPKc 0x088bb40a
// （报告 _lt_global_gt_/SetPriorityBaseDirectory.md）：
// 构造临时 string；非空才 push_back（空串忽略）。
void SetPriorityBaseDirectory(const char* dir)
{
    std::string path(dir ? dir : "");
    if (!path.empty())
        g_szScriptFilePriorityBaseDirectory.push_back(path);
}

// ORIG T _Z10isDbcsLeadc 0x88bb367：DBCS 首字节判定（最高位为 1）。
bool isDbcsLead(char c)
{
    return (static_cast<unsigned char>(c) >> 7) != 0;
}

// ORIG T _Z11LexerOutputPKci 0x88bb37c：词法输出走 LogManager 日志。
void LexerOutput(const char* buf, int size)
{
    (void)size;
    LogManager::logFormat(0, "../RDARScript.cpp", "LexerOutput", 0x12f, "%s",
                          toTString(buf));
}

// ORIG T _Z10LexerErrorPKc 0x88bb3c7：词法错误走 LogManager 日志。
void LexerError(const char* msg)
{
    LogManager::logFormat(0, "../RDARScript.cpp", "LexerError", 0x134, "%s",
                          toTString(msg));
}

// ORIG T _Z14InitRDARScriptPKcbS0_Pi 0x088bb5d2：
//   1) SetHandler 按 compiled 选择 LoadStreamFromPack/Plain；
//   2) 规范化脚本根目录到 g_szScriptFileBaseDirectory（补 '\\'），空目录返回 false；
//   3) 置 s_bUseScriptPack=compiled；param_3 非空时强制 pack 模式并以其为包名；
//   4) pack 模式下 PackSet 打开并装载字符串表，结果写入 param_4（推断：ORIG 的
//      loadStringTable/importKeyNameIndexTable 为 ORIG TU 内 static，未单独还原）。
bool InitRDARScript(const char* dir, bool compiled, const char* file, int* err)
{
    g_dnfLex.SetHandler(&isDbcsLead, &LexerOutput, &LexerError,
                        compiled ? LoadStreamFromPack : LoadStreamFromPlain);

    std::strncpy(g_szScriptFileBaseDirectory, dir ? dir : "",
                 sizeof(g_szScriptFileBaseDirectory) - 1);
    g_szScriptFileBaseDirectory[sizeof(g_szScriptFileBaseDirectory) - 1] = 0;
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
    for (std::map<int, std::string>::const_iterator it = list->m_fileMap.begin();
         it != list->m_fileMap.end(); ++it)
    {
        std::fprintf(fp, "%d\t", it->first);
        std::fprintf(fp, "`%s`", it->second.c_str());
    }
    std::fclose(fp);
    return true;
}

// ORIG 0x088bc068：装载名字表文件（name id 对），插入 STNameList；
// 插入失败（重复键）时记日志；返回列表是否非空（推断：ORIG 尾部 begin()==end() 判断）。
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
        bool isNew = (list->GetID(name.c_str()) == 0xffffffff);
        list->Insert(name.c_str(), id);
        if (!isNew)
        {
            LogManager::logFormat(
                0, "../RDARScript.cpp", "loadRDARNameFile", 1000,
                "ITEM NAME FILE LIST LOAD FAIL! [%s%s][%s]", dir, file,
                name.c_str());
        }
    }
    return !list->IsEmpty();
}

// ORIG 0x088bc1ee：装载列表文件（id name 对）加入 m_fileMap；重复键记日志并跳过。
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
            list->m_fileMap.insert(std::make_pair(idx, name));
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScript.cpp", "addRDARScriptListFile", 0x418,
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
    std::istream* in = static_cast<std::istream*>(g_dnfLex.getInputStream());
    in->read(buffer, size);
    buffer[size] = static_cast<char>(0xff);

    int ok = parsing(buffer);
    delete[] buffer;
    return ok;
}

// ORIG W 0x081a4054：默认解析器直接返回成功（1）。
int Arad_ScriptLoader::parsing(char* line)
{
    (void)line;
    return 1;
}
