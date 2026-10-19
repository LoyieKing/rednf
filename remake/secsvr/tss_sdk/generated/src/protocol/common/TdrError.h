// Auto-generated header stub from DWARF info
// Original path: src/protocol/common/TdrError.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRERROR_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRERROR_H_H_

namespace tsf4g_tdr {
    namespace TdrError {
        enum ErrorType {
            TDR_NO_ERROR = 0,
            TDR_ERR_SHORT_BUF_FOR_WRITE = -1,
            TDR_ERR_SHORT_BUF_FOR_READ = -2,
            TDR_ERR_STR_LEN_TOO_BIG = -3,
            TDR_ERR_STR_LEN_TOO_SMALL = -4,
            TDR_ERR_STR_LEN_CONFLICT = -5,
            TDR_ERR_MINUS_REFER_VALUE = -6,
            TDR_ERR_REFER_SURPASS_COUNT = -7,
            TDR_ERR_ARG_POINTER_IS_NULL = -8,
            TDR_ERR_CUTVER_TOO_SMALL = -9,
            TDR_ERR_CUTVER_CONFILICT = -10,
            TDR_ERR_PARSE_TDRIP_FAILED = -11,
            TDR_ERR_INVALID_TDRIP_VALUE = -12,
            TDR_ERR_INVALID_TDRTIME_VALUE = -13,
            TDR_ERR_INVALID_TDRDATE_VALUE = -14,
            TDR_ERR_INVALID_TDRDATETIME_VALUE = -15,
            TDR_ERR_FUNC_LOCALTIME_FAILED = -16,
            TDR_ERR_INVALID_HEX_STR_LEN = -17,
            TDR_ERR_INVALID_HEX_STR_FORMAT = -18,
            TDR_ERR_NULL_POINTER_PARAMETER = -19,
            TDR_ERR_NET_CUTVER_INVALID = -20,
            TDR_ERR_VALUE_UNDER_OR_OVER_FLOW = -21,
            TDR_ERR_OPEN_FILE_READ_FAILED = -22,
            TDR_ERR_OPEN_FILE_WRITE_FAILED = -23,
            TDR_ERR_READ_FILE_FAILED = -24,
            TDR_ERR_WRITE_FILE_FAILED = -25,
            TDR_ERR_ALLOC_MEMORY_FAILED = -26,
            TDR_ERR_PARSE_XML_FAILED = -27,
            TDR_ERR_INVALID_ROOT_NODE = -28,
            TDR_ERR_FAILED_TO_PARSE_NUMERIC = -29,
            TDR_ERR_UNDEFINED_MACRO_NAME = -30,
        };
    } // namespace TdrError
} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRERROR_H_H_
