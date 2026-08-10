// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_file_load.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/formmog/secagent/secagent_predefine.h"
#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/secagent/secagent_file_load.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

// mangled: _Z13load_bin_filePKcPhPj
// line 13
// Reconstructed from secagent disassembly
int load_bin_file(const char *file_path, unsigned char *data, unsigned int *data_len) {
    FILE *fp;
    int read_len;

    if (file_path == NULL || data == NULL || data_len == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("load_bin_file: invalid argv.");
        return 0x493f1;
    }

    fp = fopen(file_path, "rb");
    if (fp == NULL) {
        ZEN_Trace_LogMsg::debug_errorex("load %s fail. err=%s", file_path, strerror(errno));
        return 0x49406;
    }

    read_len = fread(data, 1, *data_len, fp);
    fclose(fp);
    if (read_len < 0) {
        ZEN_Trace_LogMsg::debug_errorex("read %s fail. ret=%d err=%s", file_path, read_len, strerror(errno));
        return 0x49407;
    }
    if (read_len == *data_len) {
        ZEN_Trace_LogMsg::debug_errorex("read %s fail. buf is too short. data_len=%u", file_path, read_len);
    }
    *data_len = read_len;
    return 0;
}
