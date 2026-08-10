// Reconstructed from gunnersvr DWARF + disassembly.
// Original path: src/commlib/framework/comm_svrd_config.h
// sizeof(Comm_Svrd_Config) = 166692 (0x28b24)
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_CONFIG_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_CONFIG_H_H_

#include <stdint.h>
#include <string>
#include <set>

#include "src/commlib/framework/comm_service_info.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "output/protocol/common/release/include/comm_conf_framework.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"

// sizeof = 166692
struct Comm_Svrd_Config {
public:
    SERVICES_ID self_svr_id_;                     // +0x4
    std::string app_run_dir_;                     // +0xc
    bool if_restore_pipe_;                        // +0x10
    bool app_run_daemon_;                         // +0x11
    bool app_install_service_;                    // +0x12
    bool app_uninstall_service_;                  // +0x13
    conf_zerg::zerg_config zerg_config_;          // +0x14  (43277 bytes)
    conf_framework::framework_config framework_config_; // +0xa921 (456 bytes)
    conf_svcid::svcid_config svcid_config_;       // +0xaae9 (122884 bytes)
    std::string log_file_prefix_;                 // +0x28af0
    std::string zerg_cfg_file_;                   // +0x28af4
    std::string app_cfg_file_;                    // +0x28af8
    std::string framework_cfg_file_;              // +0x28afc
    std::string svcid_cfg_file_;                  // +0x28b00
    std::string master_cfgsvr_ip_;                // +0x28b04
    uint16_t instance_id_;                        // +0x28b08
    bool is_use_cfgsvr_;                          // +0x28b0a
    std::set<unsigned int> monitor_uin_set_;      // +0x28b0c (24 bytes)
private:
    static Comm_Svrd_Config *instance_;           // .bss
public:
    Comm_Svrd_Config();
    virtual ~Comm_Svrd_Config();

    static Comm_Svrd_Config *instance();
    static void clean_instance();

    int init(int argc, const char **argv);
    int proc_start_arg(int argc, const char **argv);
    int load_config();
    int reload();
    bool is_monitor_uin(unsigned int uin);
    int usage(const char *program_name);
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_CONFIG_H_H_
