// Tag-394 Comm_Svrd_Config for reconstructed TUs.
//
// This header shadows source/secagent/src/commlib/framework/comm_svrd_config.h
// for TUs compiled with `-I recon`.  The reuse copy includes the protocol
// config headers (comm_conf_zerg/framework/svcid), whose TDR chain cannot be
// co-included with the recon protocol/origsrc headers in one TU.
//
// ORIG DWARF (libtss_sdk.so.1.0.394, CU comm_svrd_config.cpp):
//   struct Comm_Svrd_Config size=166692
//     +0     <vptr>                              (virtual destructor)
//     +4     self_svr_id_                        SERVICES_ID (6)
//     +12    app_run_dir_                        string
//     +16..  if_restore_pipe_ / app_run_daemon_ / app_install_service_ /
//            app_uninstall_service_              bool each
//     +20    zerg_config_       ZERG_CONFIG      43277
//     +43297 framework_config_  FRAMEWORK_CONFIG 456
//     +43753 svcid_config_      SVCID_CONFIG     122884
//     +166640..166660  six std::string paths/ip
//     +166664 instance_id_      uint16_t
//     +166666 is_use_cfgsvr_    bool
//     +166668 monitor_uin_set_  std::set<unsigned int> (24)
//
// The three aggregate members are declared with the ORIG's typedef names over
// anonymous byte aggregates of exactly the ORIG sizes.  This reproduces the
// ORIG DWARF member shape (`ZERG_CONFIG`, lkey anon43277) without pulling the
// protocol headers; the real field layouts are declared once in the protocol
// reconstruction and audited there.  The only fields this SDK's CUs read are
// self_svr_id_, master_cfgsvr_ip_, instance_id_ and is_use_cfgsvr_.
#ifndef TSS_RECON_COMM_SVRD_CONFIG_H_
#define TSS_RECON_COMM_SVRD_CONFIG_H_

#include <stdint.h>
#include <set>
#include <string>

#include "src/commlib/framework/comm_service_info.h"  // SERVICES_ID

// ORIG member type names; anonymous aggregates carry the exact byte sizes.
typedef struct { unsigned char bytes[43277]; } ZERG_CONFIG;
typedef struct { unsigned char bytes[456]; } FRAMEWORK_CONFIG;
typedef struct { unsigned char bytes[122884]; } SVCID_CONFIG;

struct Comm_Svrd_Config {
    virtual ~Comm_Svrd_Config();

    SERVICES_ID self_svr_id_;                     // +0x04
    std::string app_run_dir_;                     // +0x0c
    bool if_restore_pipe_;                        // +0x10
    bool app_run_daemon_;                         // +0x11
    bool app_install_service_;                    // +0x12
    bool app_uninstall_service_;                  // +0x13
    ZERG_CONFIG zerg_config_;                     // +0x14
    FRAMEWORK_CONFIG framework_config_;           // +0xa921
    SVCID_CONFIG svcid_config_;                   // +0xaae9
    std::string log_file_prefix_;                 // +0x28af0
    std::string zerg_cfg_file_;                   // +0x28af4
    std::string app_cfg_file_;                    // +0x28af8
    std::string framework_cfg_file_;              // +0x28afc
    std::string svcid_cfg_file_;                  // +0x28b00
    std::string master_cfgsvr_ip_;                // +0x28b04
    uint16_t instance_id_;                        // +0x28b08
    bool is_use_cfgsvr_;                          // +0x28b0a
    std::set<unsigned int> monitor_uin_set_;      // +0x28b0c

    static Comm_Svrd_Config *instance();
    static void clean_instance();

    int init(int argc, const char **argv);
    int proc_start_arg(int argc, const char **argv);
    int load_config();
    int reload();
    bool is_monitor_uin(unsigned int uin);
    int usage(const char *program_name);

private:
    static Comm_Svrd_Config *instance_;
};

typedef char tss_recon_comm_svrd_config_size[
    sizeof(Comm_Svrd_Config) == 166692 ? 1 : -1];
typedef char tss_recon_zerg_config_size[
    sizeof(ZERG_CONFIG) == 43277 ? 1 : -1];
typedef char tss_recon_framework_config_size[
    sizeof(FRAMEWORK_CONFIG) == 456 ? 1 : -1];
typedef char tss_recon_svcid_config_size[
    sizeof(SVCID_CONFIG) == 122884 ? 1 : -1];

#endif  // TSS_RECON_COMM_SVRD_CONFIG_H_
