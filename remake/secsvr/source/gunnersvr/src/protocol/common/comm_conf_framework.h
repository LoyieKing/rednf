// Auto-generated header stub from DWARF info
// Original path: src/protocol/common/comm_conf_framework.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_FRAMEWORK_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_FRAMEWORK_H_H_

namespace Comm_Svrd_Config {
    typedef struct {
        public:
        // anonymous struct, sizeof = 12
        struct {
            public:
            uint32_t trans_num_;
            uint32_t trans_cmd_num_;
            uint32_t func_cmd_num_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 128
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 130
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 131
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 133
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 134
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 135
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 137
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 138
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 140
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 141
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 142
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 143
        } trans_info_;
        // anonymous struct, sizeof = 8
        struct {
            public:
            uint32_t task_thread_num_;
            uint32_t task_thread_stack_size_;
            tsf4g_tdr::TdrError::ErrorType construct(); // line 88
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 90
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 91
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 93
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 94
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 95
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 97
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 98
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 100
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 101
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 102
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 103
        } task_info_;
        // anonymous struct, sizeof = 436
        struct {
            public:
            uint32_t log_output_;
            uint32_t log_div_type_;
            uint32_t max_log_file_num_;
            uint32_t max_log_file_size_;
            uint32_t bill_output_;
            uint32_t bill_div_type_;
            uint32_t max_bill_file_num_;
            uint32_t max_bill_file_size_;
            uint32_t monitor_uin_count_;
            uint32_t monitor_uin_list_[];
            tsf4g_tdr::TdrError::ErrorType construct(); // line 49
            tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 51
            tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 52
            tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 54
            tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 55
            tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 56
            tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 58
            tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 59
            tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 61
            tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 62
            tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 63
            tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 64
        } log_info_;
        tsf4g_tdr::TdrError::ErrorType construct(); // line 168
        tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 170
        tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 171
        tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 173
        tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 174
        tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 175
        tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 177
        tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 178
        tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 180
        tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 181
        tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 182
        tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 183
    } FRAMEWORK_CONFIG;
} // namespace Comm_Svrd_Config

#endif // SECSVR_SRC_PROTOCOL_COMMON_COMM_CONF_FRAMEWORK_H_H_
