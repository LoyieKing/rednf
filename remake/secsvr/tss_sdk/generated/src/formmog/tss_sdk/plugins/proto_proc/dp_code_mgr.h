// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_CODE_MGR_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_CODE_MGR_H_H_

extern tss_dpsdk::DPCodeMgr::DpCodeInfo *info; // line 98
extern int i; // line 217
extern const unsigned int MAX_VERSION_HASH; // line 25
extern const unsigned int MAX_CODE_BUF_LEN; // line 157
extern const unsigned int MAX_CODE_ITEM; // line 122
extern const unsigned int MAX_BIN_FILE_LEN; // line 162
extern const unsigned int MAX_VERSION_COUNT; // line 203
extern const unsigned int MAX_VERSION_CODE; // line 230
extern unsigned int rand_num; // line 169
namespace tss_dpsdk {
    namespace DPCodeMgr {
        // sizeof = 2004
        struct CodeItem<2000> { // line 111
        public:
        short unsigned int weight_;
        short unsigned int code_len_;
        unsigned char code_buf_[];
        };
        // sizeof = 200402
        struct CodeItems<2000> { // line 119
        public:
        short unsigned int total_weight_;
        static const unsigned int MAX_CODE_ITEM;
        tss_dpsdk::DPCodeMgr::CodeItem<2000> items_[];
        short unsigned int select_code_id(unsigned int arg0); // line 125
        unsigned char * get_code_buf(short unsigned int arg0, short unsigned int &arg1); // line 143
        int load_cfg(const char *dir); // line 152
        };
        // sizeof = 20004
        struct CodeItem<20000> { // line 111
        public:
        short unsigned int weight_;
        short unsigned int code_len_;
        unsigned char code_buf_[];
        };
        // sizeof = 2000402
        struct CodeItems<20000> { // line 119
        public:
        short unsigned int total_weight_;
        static const unsigned int MAX_CODE_ITEM;
        tss_dpsdk::DPCodeMgr::CodeItem<20000> items_[];
        short unsigned int select_code_id(unsigned int arg0); // line 125
        unsigned char * get_code_buf(short unsigned int arg0, short unsigned int &arg1); // line 143
        int load_cfg(const char *dir); // line 152
        };
        // sizeof = 2401206
        struct CodeInfo { // line 156
        public:
        static const unsigned int MAX_CODE_BUF_LEN;
        tss_dpsdk::DPCodeMgr::CodeItems<2000> cryptdata_code_;
        tss_dpsdk::DPCodeMgr::CodeItems<2000> crypt_code_;
        static const unsigned int MAX_BIN_FILE_LEN;
        tss_dpsdk::DPCodeMgr::CodeItems<20000> bin_file_;
        short unsigned int select_bin_code_id(short unsigned int arg0); // line 165
        short unsigned int select_bin_file_id(); // line 181
        unsigned char * get_bin_code_buf(short unsigned int arg0, short unsigned int &arg1); // line 186
        unsigned char * get_bin_file_buf(short unsigned int arg0, short unsigned int &arg1); // line 194
        int load_cfg(const char *dir); // line 198
        };
        // sizeof = 4802464
        struct DpCodeInfo { // line 202
        public:
        static const unsigned int MAX_VERSION_COUNT;
        short unsigned int version_count_;
        unsigned int version_support_list_[];
        unsigned char version_hash_[];
        tss_dpsdk::DPCodeMgr::CodeInfo client_code_;
        tss_dpsdk::DPCodeMgr::CodeInfo server_code_;
        int load_cfg(const char *arg0); // line 213
        int is_support(unsigned int arg0); // line 215
        };
        // sizeof = 9604936
        struct DpCodeInfos { // line 229
        public:
        static const unsigned int MAX_VERSION_CODE;
        unsigned int code_update_time_;
        short unsigned int code_count_;
        tss_dpsdk::DPCodeMgr::DpCodeInfo code_infos_[];
        tss_dpsdk::DPCodeMgr::DpCodeInfo * get_code_info(unsigned int arg0); // line 235
        };
    } // namespace DPCodeMgr
} // namespace tss_dpsdk

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DP_CODE_MGR_H_H_
