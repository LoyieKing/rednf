// df_game_r 还原 —— STScriptFileList（ORIG T 0x88bad3a 族）。
//   FindIndexByFullScan(char const*) const（ORIG T 0x88bae56，mangled
//   _ZNK16STScriptFileList19FindIndexByFullScanEPKc）：
//     遍历 std::map<int,std::string> m_fileMap（+0x00），
//     返回 value==path 的 key；未找到返回 0xffffffff（-1）。
//   （证据见 docs/class_func_reports/STScriptFileList/FindIndexByFullScan.md）

#include "NpcScript.h"
#include "LogManager.h"
#include "DNFLexWrapper.h"

// ORIG 0x0949b284 BSS：为 0 时重复索引报错返回 0；非 0 时重复项以负键覆盖插入。
extern char g_isAllowScriptDuplication;


// ORIG 0x088bacf8：map 默认构造后调 Clear。
STScriptFileList::STScriptFileList()
{
    Clear();
}

void STScriptFileList::Clear() { m_fileMap.clear(); }

// ORIG T 0x088bad4e，mangled _ZNK16STScriptFileList11GetFileNameEi：
//   按 index 查 m_fileMap；命中返回 value.c_str()，未命中返回 0。
//   （证据见 docs/class_func_reports/STScriptFileList/GetFileName.md）
const char* STScriptFileList::GetFileName(int idx) const
{
    std::map<int, std::string>::const_iterator it = m_fileMap.find(idx);
    if (it == m_fileMap.end())
        return 0;
    return it->second.c_str();
}

//   ORIG 循环形态：begin() 一次、end() 缓存于循环外（P13）、后缀 it++（ppEi）。
int STScriptFileList::FindIndexByFullScan(const char* path) const
{
    std::map<int, std::string>::const_iterator it = m_fileMap.begin();
    std::map<int, std::string>::const_iterator e = m_fileMap.end();
    while (it != e)
    {
        if (it->second == path)
            return it->first;
        it++;
    }
    return -1;
}

// ORIG T 0x088badba，mangled _ZN16STScriptFileList8GetIndexEPKc：
//   按文件名遍历 map（与 FindIndexByFullScan 不同：非 const 迭代器、end()
//   每次循环调用、后缀 it++），命中返回索引，未命中返回 -1。
//   （证据见 docs/class_func_reports/STScriptFileList/GetIndex.md）
int STScriptFileList::GetIndex(const char* path)
{
    for (std::map<int, std::string>::iterator it = m_fileMap.begin();
         it != m_fileMap.end(); it++)
    {
        if ((*it).second == path)   // ORIG 用 operator*（de），非 operator->（pt）
            return (*it).first;
    }
    return -1;
}

// ORIG 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 用 DNFLex 扫描列表文件，逐行 ScanInt+ScanStr 填充 m_fileMap。
// 第 4 参 map<int,int>* 在 ORIG 中未被使用（可由调用方传 0）。
// b28d-31: N 封顶（N82）——ORIG 0x088bbd77；134 vs 136（帧/槽）。
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* /*unused*/)
{
    if (!loadRDARScriptFile(dir, listPath))
        return 0;
    list->m_fileMap.clear();

    bool ok = false;
    std::string line;
    for (;;)
    {
        int idx = ScanInt(&ok);
        if (!ok)
            return 1;
        if (!ScanStr(&line))
            return 1;

        bool isNew = (list->m_fileMap.find(idx) == list->m_fileMap.end());
        if (!isNew && !g_isAllowScriptDuplication)
        {
            // ORIG 0x088bf09：logFormat(0,"../RDARScript.cpp",
            // __FUNCTION__,0x323,"SCRIPT FILE LIST LOAD FAIL!
            // Index Duplicated [%s%s][%d]", dir, listPath, idx)
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 0x323,
                "SCRIPT FILE LIST LOAD FAIL! Index Duplicated [%s%s][%d]",
                dir, listPath, idx);
            return 0;
        }
        int sign = isNew ? 1 : -1;
        list->m_fileMap.insert(std::pair<const int, std::string>(idx * sign, line));
    }
}
