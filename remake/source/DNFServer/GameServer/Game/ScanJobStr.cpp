// ============================================================================
// df_game_r ScanJobStr(std::string*, std::string**) _Z10ScanJobStrPSsPS_ 0x08a6143f
// ----------------------------------------------------------------------------
// 语义（objdump 0x08a6143f，全文已读）：
//   token = g_dnfLex.getToken()（全局 @0x9495020）：
//     DNF_TOK_STRING(7)   → *jobName = toTString(g_dnfLex.getString())，found=1；
//     DNF_TOK_PAIR_INT(9) → key = g_dnfLex.getPairInt()；若再取词为
//       DNF_TOK_PAIR_STR(10) → *jobName = g_scriptStringManager_.findString(
//       key, g_dnfLex.getPairStr(), 0)（flag 传 NULL），found=1。
//   out != 0 时：for (i = 0; i <= 10; ++i)：
//     snprintf(buf[0x400], "%s_%d", g_dnfLex.getPairStr(), i)（rodata "%s_%d"）；
//     s = g_scriptStringManager_.findString(key, buf, &ok)；
//     s != 0 && ok → out[i] = new std::string(); *out[i] = s（含异常展开）。
//   返回 found（movzbl 直返，bool）。
// 依赖：DNFLexWrapper（g_dnfLex）/ RDARScriptStringManager（g_scriptStringManager_）
// / toTString(const char*)（SyncScriptDeps.h），均已存在。
// ============================================================================

#include <cstdio>
#include <string>

#include "DNFLexWrapper.h"
#include "RDARScriptStringManager.h"
#include "SyncScriptDeps.h"

bool ScanJobStr(std::string* jobName, std::string** out)
{
    bool found = false;
    int token = g_dnfLex.getToken();
    if (token == DNF_TOK_STRING)
    {
        *jobName = toTString(g_dnfLex.getString());
        found = true;
    }
    else if (token == DNF_TOK_PAIR_INT)
    {
        int key = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
        {
            *jobName = g_scriptStringManager_.findString(
                key, g_dnfLex.getPairStr(), 0);
            found = true;
        }
        if (out != 0)
        {
            for (int i = 0; i <= 10; ++i)
            {
                char buf[0x400];
                std::snprintf(buf, sizeof(buf), "%s_%d",
                              g_dnfLex.getPairStr(), i);
                bool ok = false;
                char const* s = g_scriptStringManager_.findString(
                    key, buf, &ok);
                if (s != 0 && ok)
                {
                    std::string* p = new std::string();
                    out[i] = p;
                    *p = s;
                }
            }
        }
    }
    return found;
}
