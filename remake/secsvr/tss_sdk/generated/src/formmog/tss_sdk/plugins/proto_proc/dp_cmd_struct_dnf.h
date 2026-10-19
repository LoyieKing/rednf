// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/proto_proc/dp_cmd_struct_dnf.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_CMD_STRUCT_DNF_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_CMD_STRUCT_DNF_H_H_

namespace tss_dpsdk {
    namespace DPHackDnfMailBody {
        // sizeof = 11
        union ._497 { // line 78
            // sizeof = 11
            struct ._498 {
            public:
            char pad_;
            // anonymous struct, sizeof = 10
            struct {
                public:
                short unsigned int item_posi_;
                unsigned int item_id_;
                unsigned int item_num_;
            } item_;
            };
        public:
        ._498 items_[];
        // anonymous struct, sizeof = 10
        struct {
            public:
            short unsigned int item_posi_;
            unsigned int item_id_;
            unsigned int item_num_;
        } item_;
        };
        namespace ._497 {
            // sizeof = 11
            struct ._498 { // line 80
            public:
            char pad_;
            // anonymous struct, sizeof = 10
            struct {
                public:
                short unsigned int item_posi_;
                unsigned int item_id_;
                unsigned int item_num_;
            } item_;
            };
        } // namespace ._497
    } // namespace DPHackDnfMailBody
} // namespace tss_dpsdk

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_CMD_STRUCT_DNF_H_H_
