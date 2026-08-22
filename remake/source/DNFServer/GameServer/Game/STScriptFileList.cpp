// df_game_r 还原 —— STScriptFileList（ORIG T 0x88bad3a 族）。
//   FindIndexByFullScan(char const*) const（ORIG T 0x88bae56，mangled
//   _ZNK16STScriptFileList19FindIndexByFullScanEPKc）：
//     遍历 std::map<int,std::string> m_fileMap（+0x00），
//     返回 value==path 的 key；未找到返回 0xffffffff（-1）。
//   （证据见 docs/class_func_reports/STScriptFileList/FindIndexByFullScan.md）

#include <map>
#include "LogManager.h"
#include "DNFLexWrapper.h"

// ORIG 0x0949b284 BSS：为 0 时重复索引报错返回 0；非 0 时重复项以负键覆盖插入。
extern char g_isAllowScriptDuplication;

#include <string>

class STScriptFileList
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00（与 CItemShop.cpp / STQuestScript.cpp 一致）
    int FindIndexByFullScan(const char* path) const;
};

int STScriptFileList::FindIndexByFullScan(const char* path) const
{
    for (std::map<int, std::string>::const_iterator it = m_fileMap.begin();
         it != m_fileMap.end(); ++it)
    {
        if (it->second == path)
            return it->first;
    }
    return 0xffffffff;
}

// ORIG 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 用 DNFLex 扫描列表文件，逐行 ScanInt+ScanStr 填充 m_fileMap。
// 第 4 参 map<int,int>* 在 ORIG 中未被使用（可由调用方传 0）。
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
            LogManager::logFormat(
                0, "../../DNFShared/RDARScript.cpp", "loadStringTable", 803,
                "SCRIPT FILE LIST LOAD FAIL! Index Duplicated [%s%s][%d].",
                dir, listPath, idx);
            return 0;
        }
        int sign = isNew ? 1 : -1;
        list->m_fileMap.insert(std::make_pair(idx * sign, line));
    }
}