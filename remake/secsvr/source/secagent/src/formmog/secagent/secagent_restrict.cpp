// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_restrict.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/secagent/secagent_restrict.h"

// line 11
SecagentRestrict::SecagentRestrict() {
    is_use_restrict_ = false;
}

// line 16
SecagentRestrict::~SecagentRestrict() {
    allow_login_list_.~DigitList();
}

// line 61
bool SecagentRestrict::is_allow_login(unsigned int uin) {
    if (!is_use_restrict_) {
        return true;
    }
    return allow_login_list_.is_exist(uin);
}

// line 41
int SecagentRestrict::reload(const char *uin_list_file) {
    int ret = allow_login_list_.load(uin_list_file, DigitList::EDT_10);
    if (ret == 0 && allow_login_list_.get_list().size() != 0) {
        return 0;
    }
    ZEN_Trace_LogMsg::debug_errorex("load restrict uin fail. path=%s restrict function will closed", uin_list_file);
    is_use_restrict_ = false;
    return 0;
}

// line 21
int SecagentRestrict::init(const char *uin_list_file) {
    int ret = allow_login_list_.load(uin_list_file, DigitList::EDT_10);
    if (ret != 0 || allow_login_list_.get_list().size() == 0) {
        ZEN_Trace_LogMsg::debug_errorex("load restrict uin fail. path=%s restrict function will closed", uin_list_file);
        is_use_restrict_ = false;
        return 0;
    }
    is_use_restrict_ = true;
    ZEN_Trace_LogMsg::debug_infoex("restrict function is opened.");
    return 0;
}
