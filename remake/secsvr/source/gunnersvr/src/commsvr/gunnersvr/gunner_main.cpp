// Reconstructed from gunnersvr disassembly.
#include "src/commsvr/gunnersvr/gunner_predefine.h"
#include "output/commlib/framework/release/include/comm_cfgsvr_sdk.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "src/commsvr/gunnersvr/gunner_app.h"
#include "src/commsvr/gunnersvr/gunner_tool.h"

#include <string.h>

// --as-tool：命令行工具模式（-d 由 ZEN_Server_Toolkit 处理）
bool is_run_as_app(int argc, const char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--as-tool", 10) == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, const char **argv) {
    int ret = 0;
    if (is_run_as_app(argc, argv)) {
        CfgSvrSdk::instance()->add_config("gunnersvr_config.xml");
        CfgSvrSdk::instance()->add_config("zergsvrd.xml");
        CfgSvrSdk::instance()->add_config("framework.xml");
        CfgSvrSdk::instance()->set_service_type(7);
        GunnerSvrApp::instance()->set_service_info(
            "\xd3\xce\xcf\xb7\xb0\xb2\xc8\xab\xd2\xfd\xc7\xe6\xbc\xe0\xbf\xd8"
            "\xca\xfd\xbe\xdd\xb1\xa8\xb7\xfe\xce\xf1",
            "Tencent Gunner Svr");
        GunnerSvrApp::instance()->run(argc, argv);
        return 0;
    }

    ret = GunnerTool::instance()->proc_cmd(argc, argv);
    if (ret < 0) {
        ZEN_Trace_LogMsg::debug_errorex("gunner tool proc cmd error");
    }
    return ret;
}
