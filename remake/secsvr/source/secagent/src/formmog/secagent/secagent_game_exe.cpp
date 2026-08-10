// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_game_exe.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/formmog/secagent/secagent_predefine.h"
#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/secagent/secagent_game_exe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// line 5
// Reconstructed from secagent disassembly
int SecagentGameExe::load(const char *data_path) {
    FILE *pf;

    pf = fopen(data_path, "rb");
    if (pf == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], open game exe error", "int SecagentGameExe::load(const char*)");
        return 0x493e5;
    }
    exe_data_size_ = fread(exe_data_, 1, MAX_EXEDATA_LEN, pf);
    fclose(pf);
    return 0;
}

// line 21
// Reconstructed from secagent disassembly
int SecagentGameExe::load_version(const char *version_cfg_path) {
    FILE *pf;
    char line_buffer[256];
    bool read_version_succ = false;

    pf = fopen(version_cfg_path, "rb");
    if (pf == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("open version cfg file fail, path=%s", version_cfg_path);
        return 0x4940b;
    }

    while (true) {
        memset(line_buffer, 0, sizeof(line_buffer));
        fscanf(pf, "%s", line_buffer);
        if (line_buffer[0] != '#' && !std::string(line_buffer).empty()) {
            version_ = strtoul(line_buffer, NULL, 10);
            ZEN_Trace_LogMsg::debug_debugEx("muti_version_test, game exe, the config version is %u", version_);
            fclose(pf);
            return 0;
        }
        fgets(line_buffer, 255, pf);
        if (feof(pf)) {
            break;
        }
    }
    fclose(pf);
    ZEN_Trace_LogMsg::debug_errorex("read version fail, version cfg path=%s", version_cfg_path);
    return 0x49409;
}
