#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate formmog_proto_public_secsvr / formmog_proto_public_tsssdk for secagent.

Semantics reverse engineered from the secagent binary:
  - construct(): memset(this, 0, sizeof(*this)); return TDR_NO_ERROR
  - pack(buf): big-endian scalars via TdrWriteBuf API; strings written with
    NUL-terminated length prefix (writeString, buffer NUL-forced first);
    variable byte buffers: u16 len prefix + len>size -> -7 + raw bytes;
    fixed byte arrays: raw bytes; object arrays: count>max -> -7, count==0 -> OK,
    loop pack(sub, 1).
  - unpack(buf): mirrored; strings via bounded read helper; buffers: u16 len,
    len>size -> -7, short -> -2, raw bytes.
  - visualize(buf): printVariable ("%d"/"%u"/"0x%02x"), printString for str,
    printArray + " 0x%02x" loop for byte arrays (variable ones append separator
    and NUL), object arrays printVariable(..., i, true) + sub visualize with
    indent+1 when indent >= 0.
  - visualize_ex(buf): NUL at pos when pos != limit; char* variant clamps to
    size-1.
"""
from __future__ import print_function

IND = "    "


def E():
    return "TdrError::TDR_NO_ERROR"


HEADER_TMPL = """\
// Reconstructed from secagent disassembly
#ifndef {guard}
#define {guard}

#include <stdint.h>
#include <stddef.h>
#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {{
class TdrWriteBuf;
class TdrReadBuf;
}}

namespace sec_proto {{

#pragma pack(push, 1)

{classes}
#pragma pack(pop)

}} // namespace sec_proto

#endif // {guard}
"""

CPP_HEADER_TMPL = """\
// Reconstructed from secagent disassembly

#include "src/protocol/formmog/{basename}.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <string.h>
#include <stdint.h>

namespace {{

// 读取带长度前缀的字符串（长度字段含结尾 NUL），单个元素最大 maxLen 字节。
tsf4g_tdr::TdrError::ErrorType readBoundedString(tsf4g_tdr::TdrReadBuf& destBuf, char* dest, size_t maxLen) {{
    uint32_t size;
    tsf4g_tdr::TdrError::ErrorType ret = destBuf.readUInt32(size);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < size) return tsf4g_tdr::TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    if (size > maxLen) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
    if (size == 0) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_SMALL;
    if (dest == NULL) return tsf4g_tdr::TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    for (uint32_t i = 0; i < size; ++i) {{
        ret = destBuf.readChar(dest[i]);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }}
    if (dest[size - 1] != '\\0') return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    if (strlen(dest) + 1 != size) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}}

}} // anonymous namespace

namespace sec_proto {{

using namespace tsf4g_tdr;

"""


# Field kinds:
#   ('name', 'u8'|'u16'|'u32'|'int32')
#   ('name', 'str', size)
#   ('name', 'buf', size)              variable buffer + '<name>_len_' u16
#   ('name', 'fixed8', size)
#   ('name', 'objarr', 'Type', max, count_field, var)
SECSVR_CLASSES = [
    ("AddUserReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("time_stamp_", "u32"),
        ("client_version_", "u32"),
        ("client_ip_", "u32"),
        ("server_ip_", "u32"),
        ("player_name_", "str", 256),
    ]),
    ("ConfirmRpcodeInfo", [
        ("world_id_", "u16"),
        ("dib_id_", "u8"),
        ("key_offset_", "u32"),
    ]),
    ("DelUserReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("time_stamp_", "u32"),
    ]),
    ("RecvDataFromSDKReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("time_stamp_", "u32"),
        ("to_busi_id_", "u16"),
        ("from_busi_id_", "u16"),
        ("to_secsvr_seq_", "u32"),
        ("data_", "buf", 32767),
    ]),
    ("ReportErrReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("error_id_", "u32"),
    ]),
    ("ReportHeartbeatReq", [
        ("room_id_", "u16"),
    ]),
    ("ReportLostAntibotPkg", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("last_seq_", "u32"),
        ("now_seq_", "u32"),
    ]),
    ("ReportPkgTime", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("last_report_time_", "u32"),
        ("now_report_time_", "u32"),
        ("now_cmd_", "int32"),
    ]),
    ("ReqSecsvrForwardData", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("data_", "buf", 32767),
    ]),
    ("ReqSecsvrFreeLock", [
        ("world_id_", "u32"),
    ]),
    ("RspSecsvrFreeLock", [
        ("world_id_", "u32"),
    ]),
    ("SecsvrForwardDataToClientReq", [
        ("world_id_", "u16"),
        ("data_", "buf", 32767),
    ]),
    ("SecsvrNotifyIsChangeSvrRsp", [
        ("world_id_", "u16"),
        ("is_change_svr_", "u8"),
    ]),
    ("SecsvrNotifyKeyOffsetRsp", [
        ("world_id_", "u16"),
        ("channel_id_", "u16"),
        ("dib_id_", "u8"),
        ("key_offset_", "u32"),
        ("key_size_", "u8"),
    ]),
    ("SecsvrNotifyMACRsp", [
        ("world_id_", "u16"),
        ("src_id_", "u32"),
        ("client_ip_", "u32"),
        ("login_time_", "u32"),
        ("mac_", "fixed8", 16),
        ("first_login_", "u8"),
        ("report_from_", "u8"),
    ]),
    ("SecsvrNotifyTcjHeartbeatTimeoutRsp", [
        ("world_id_", "u16"),
        ("src_id_", "u32"),
        ("login_time_", "u32"),
        ("check_time_", "u32"),
        ("timeout_", "u32"),
    ]),
    ("SecsvrNotifyUserLoginRsp", [
        ("world_id_", "u16"),
        ("src_id_", "u32"),
        ("client_ip_", "u32"),
        ("login_time_", "u32"),
    ]),
    ("SecsvrNotifyUserLogoutRsp", [
        ("world_id_", "u16"),
        ("src_id_", "u32"),
        ("client_ip_", "u32"),
        ("login_time_", "u32"),
        ("logout_time_", "u32"),
    ]),
]

TSSSDK_CLASSES = [
    ("SendToSDK", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("time_stamp_", "u32"),
        ("to_busi_id_", "u16"),
        ("from_busi_id_", "u16"),
        ("data_", "buf", 32767),
    ]),
    ("PunishClientReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("time_stamp_", "u32"),
        ("punish_type_", "u32"),
        ("punish_duration_", "u32"),
        ("punish_info_", "buf", 1024),
        ("punish_reason_", "buf", 1024),
    ]),
    ("PunishClientRsp", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("time_stamp_", "u32"),
        ("result_", "u16"),
    ]),
    ("DpBanModeDetail", [
        ("mode_id_", "u32"),
        ("keep_time_", "u32"),
    ]),
    ("DpBanModeReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("count_", "u32"),
        ("modes_", "objarr", "DpBanModeDetail", 32, "count_", "modes__i"),
    ]),
    ("DpDropCmdlistInfo", [
        ("cmdlist_id_", "u32"),
        ("keep_time_", "u32"),
    ]),
    ("DpDropCmdlistReq", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("count_", "u32"),
        ("cmd_lists_", "objarr", "DpDropCmdlistInfo", 100, "count_", "cmd_lists__i"),
    ]),
    ("SafemodeStatusRsp", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("ipc_days_", "u16"),
        ("stat_level_", "u8"),
        ("has_verify_mb_", "u8"),
        ("event_type_", "u16"),
        ("reserve_", "fixed8", 32),
    ]),
    ("SafemodeLimitStatusRsp", [
        ("ret_", "int32"),
        ("uin_", "u32"),
        ("limit_status_", "int32"),
        ("unlock_url_", "buf", 1024),
    ]),
    ("DPCaptchaResultRsp", [
        ("channel_id_", "u16"),
        ("room_id_", "u16"),
        ("result_", "u8"),
    ]),
]


def member_decl(f):
    name, kind = f[0], f[1]
    if kind in ("u8", "u16", "u32", "int32"):
        return {"u8": "uint8_t", "u16": "uint16_t", "u32": "uint32_t", "int32": "int32_t"}[kind] + " " + name + ";"
    if kind == "str":
        return "    char %s[%d];" % (name, f[2])
    if kind == "buf":
        return "    uint16_t %s_len_;\n    uint8_t %s[%d];" % (name.rstrip("_"), name, f[2])
    if kind == "fixed8":
        return "    uint8_t %s[%d];" % (name, f[2])
    if kind == "objarr":
        return "    %s %s[%d];" % (f[2], name, f[3])
    raise ValueError(kind)


def gen_header(classes, guard, basename):
    parts = []
    for cname, fields in classes:
        parts.append("class %s {" % cname)
        parts.append("public:")
        for f in fields:
            parts.append(member_decl(f))
        parts.append("")
        parts.append("    tsf4g_tdr::TdrError::ErrorType construct();")
        parts.append("    tsf4g_tdr::TdrError::ErrorType pack(tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;")
        parts.append("    tsf4g_tdr::TdrError::ErrorType pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;")
        parts.append("    tsf4g_tdr::TdrError::ErrorType unpack(tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);")
        parts.append("    tsf4g_tdr::TdrError::ErrorType unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);")
        parts.append("    tsf4g_tdr::TdrError::ErrorType visualize(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;")
        parts.append("    tsf4g_tdr::TdrError::ErrorType visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;")
        parts.append("    const char* visualize_ex(tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;")
        parts.append("    const char* visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;")
        parts.append("};")
        parts.append("")
    return HEADER_TMPL.format(guard=guard, classes="\n".join(parts))


def wname(kind):
    return {"u8": "writeUInt8", "u16": "writeUInt16", "u32": "writeUInt32", "int32": "writeUInt32"}[kind]


def rname(kind):
    return {"u8": "readUInt8", "u16": "readUInt16", "u32": "readUInt32", "int32": "readUInt32"}[kind]


def vfmt(kind):
    return {"u8": "0x%02x", "u16": "%d", "u32": "%u", "int32": "%d"}[kind]


def gen_construct(cname):
    return "\n".join([
        "TdrError::ErrorType %s::construct() {" % cname,
        IND + "memset(this, 0, sizeof(*this));",
        IND + "return " + E() + ";",
        "}",
    ])


def gen_pack(cname, fields):
    L = ["TdrError::ErrorType %s::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {" % cname]
    L.append(IND + "TdrError::ErrorType ret = " + E() + ";")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32", "int32"):
            L += [
                IND + "ret = destBuf.%s(%s);" % (wname(kind), ("(uint32_t)" if kind == "int32" else "") + name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "str":
            L += [
                IND + "const size_t sizePos4%s = destBuf.size();" % name,
                IND + "((char*)%s)[sizeof(%s) / sizeof(%s[0]) - 1] = '\\0';" % (name, name, name),
                IND + "size_t realSize4%s = strlen(%s) + 1;" % (name, name),
                IND + "ret = destBuf.writeString(%s);" % name,
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "buf":
            maxn = f[2]
            lenf = name.rstrip("_") + "_len_"
            L += [
                IND + "ret = destBuf.writeUInt16(%s);" % lenf,
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;" % (lenf, maxn),
                IND + "if (destBuf.capacity() - destBuf.size() < %s) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;" % lenf,
                IND + "for (uint16_t i = 0; i < %s; ++i) {" % lenf,
                IND * 2 + "ret = destBuf.writeUInt8(%s[i]);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "fixed8":
            n = f[2]
            L += [
                IND + "if (destBuf.capacity() - destBuf.size() < %d) return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;" % n,
                IND + "for (unsigned int i = 0; i < %d; ++i) {" % n,
                IND * 2 + "ret = destBuf.writeUInt8(%s[i]);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "objarr":
            typ, mx, cnt, var = f[2], f[3], f[4], f[5]
            L += [
                IND + "if (%s > %d) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;" % (cnt, mx),
                IND + ("if (%s == 0) return " + E() + ";") % cnt,
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "ret = %s[i].pack(destBuf, 1);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_unpack(cname, fields):
    L = ["TdrError::ErrorType %s::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {" % cname]
    L.append(IND + "TdrError::ErrorType ret = " + E() + ";")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32", "int32"):
            if kind == "int32":
                L += [
                    IND + "uint32_t tmp4%s;" % name,
                    IND + "ret = srcBuf.readUInt32(tmp4%s);" % name,
                    IND + "if (ret != " + E() + ") return ret;",
                    IND + "%s = (int32_t)tmp4%s;" % (name, name),
                ]
            else:
                L += [
                    IND + "ret = srcBuf.%s(%s);" % (rname(kind), name),
                    IND + "if (ret != " + E() + ") return ret;",
                ]
        elif kind == "str":
            L += [
                IND + "ret = readBoundedString(srcBuf, %s, sizeof(%s));" % (name, name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "buf":
            maxn = f[2]
            lenf = name.rstrip("_") + "_len_"
            L += [
                IND + "ret = srcBuf.readUInt16(%s);" % lenf,
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;" % (lenf, maxn),
                IND + "if (srcBuf.capacity() - srcBuf.size() < %s) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;" % lenf,
                IND + "for (uint16_t i = 0; i < %s; ++i) {" % lenf,
                IND * 2 + "char c;",
                IND * 2 + "ret = srcBuf.readChar(c);",
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "%s[i] = (uint8_t)c;" % name,
                IND + "}",
            ]
        elif kind == "fixed8":
            n = f[2]
            L += [
                IND + "if (srcBuf.capacity() - srcBuf.size() < %d) return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;" % n,
                IND + "for (unsigned int i = 0; i < %d; ++i) {" % n,
                IND * 2 + "char c;",
                IND * 2 + "ret = srcBuf.readChar(c);",
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "%s[i] = (uint8_t)c;" % name,
                IND + "}",
            ]
        elif kind == "objarr":
            typ, mx, cnt, var = f[2], f[3], f[4], f[5]
            L += [
                IND + "if (%s > %d) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;" % (cnt, mx),
                IND + ("if (%s == 0) return " + E() + ";") % cnt,
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "ret = %s[i].unpack(srcBuf, 1);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_visualize(cname, fields):
    L = ["TdrError::ErrorType %s::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {" % cname]
    L.append(IND + "TdrError::ErrorType ret = " + E() + ";")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32", "int32"):
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%s", %s);' % (name, vfmt(kind), name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "str":
            L += [
                IND + 'ret = TdrBufUtil::printString(destBuf, indent, separator, "[%s]", %s);' % (name, name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "buf":
            lenf = name.rstrip("_") + "_len_"
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%%u", %s);' % (lenf, lenf),
                IND + "if (ret != " + E() + ") return ret;",
                IND + 'ret = TdrBufUtil::printArray(destBuf, indent, separator, "[%s]", 0);' % name,
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s != 0) {" % lenf,
                IND * 2 + "for (uint16_t i = 0; i < %s; ++i) {" % lenf,
                IND * 3 + 'ret = destBuf.textize(" 0x%%02x", %s[i]);' % name,
                IND * 3 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "}",
                IND + "}",
                IND + "ret = destBuf.writeChar(separator);",
                IND + "if (ret != " + E() + ") return ret;",
                IND + "ret = destBuf.writeChar('\\0');",
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "fixed8":
            n = f[2]
            L += [
                IND + 'ret = TdrBufUtil::printArray(destBuf, indent, separator, "[%s]", %d);' % (name, n),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "for (unsigned int i = 0; i < %d; ++i) {" % n,
                IND * 2 + 'ret = destBuf.textize(" 0x%%02x", %s[i]);' % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "objarr":
            typ, mx, cnt, var = f[2], f[3], f[4], f[5]
            L += [
                IND + "if (%s > %d) return TdrError::TDR_ERR_REFER_SURPASS_COUNT;" % (cnt, mx),
                IND + "if (%s == 0) return ret;" % cnt,
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", i, true);' % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "if (indent >= 0) {",
                IND * 3 + "ret = %s[i].visualize(destBuf, indent + 1, separator);" % name,
                IND * 2 + "} else {",
                IND * 3 + "ret = %s[i].visualize(destBuf, indent, separator);" % name,
                IND * 2 + "}",
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_pack_ptr(cname):
    return "\n".join([
        "TdrError::ErrorType %s::pack(char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {" % cname,
        IND + "if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;",
        IND + "TdrWriteBuf destBuf(buffer, size);",
        IND + "TdrError::ErrorType ret = pack(destBuf, cutVer);",
        IND + "if (usedSize != NULL) *usedSize = destBuf.size();",
        IND + "return ret;",
        "}",
    ])


def gen_unpack_ptr(cname):
    return "\n".join([
        "TdrError::ErrorType %s::unpack(const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) {" % cname,
        IND + "if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;",
        IND + "TdrReadBuf srcBuf(buffer, size);",
        IND + "TdrError::ErrorType ret = unpack(srcBuf, cutVer);",
        IND + "if (usedSize != NULL) *usedSize = srcBuf.size();",
        IND + "return ret;",
        "}",
    ])


def gen_visualize_ptr(cname):
    return "\n".join([
        "TdrError::ErrorType %s::visualize(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {" % cname,
        IND + "if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;",
        IND + "TdrWriteBuf destBuf(buffer, size);",
        IND + "TdrError::ErrorType ret = visualize(destBuf, indent, separator);",
        IND + "if (usedSize != NULL) *usedSize = destBuf.size();",
        IND + "return ret;",
        "}",
    ])


def gen_visualize_ex_buf(cname):
    return "\n".join([
        "const char* %s::visualize_ex(TdrWriteBuf& destBuf, int indent, char separator) const {" % cname,
        IND + "if (destBuf.data() == NULL || destBuf.capacity() == 0) return \"\";",
        IND + "visualize(destBuf, indent, separator);",
        IND + "size_t targetPos = destBuf.size();",
        IND + "const size_t totalSize = destBuf.capacity();",
        IND + "if (targetPos != totalSize) {",
        IND * 2 + "((char*)destBuf.data())[targetPos] = '\\0';",
        IND + "}",
        IND + "return destBuf.data();",
        "}",
    ])


def gen_visualize_ex_ptr(cname):
    return "\n".join([
        "const char* %s::visualize_ex(char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const {" % cname,
        IND + "if (buffer == NULL || size == 0) return \"\";",
        IND + "TdrWriteBuf destBuf(buffer, size);",
        IND + "visualize(destBuf, indent, separator);",
        IND + "size_t targetPos = destBuf.size();",
        IND + "const size_t totalSize = size;",
        IND + "if (targetPos >= totalSize) targetPos = totalSize - 1;",
        IND + "buffer[targetPos] = '\\0';",
        IND + "if (usedSize != NULL) *usedSize = destBuf.size();",
        IND + "return buffer;",
        "}",
    ])


def gen_cpp(classes, basename, guard):
    out = [CPP_HEADER_TMPL.format(basename=basename)]
    for cname, fields in classes:
        out.append(gen_construct(cname))
        out.append("")
        out.append(gen_pack(cname, fields))
        out.append("")
        out.append(gen_pack_ptr(cname))
        out.append("")
        out.append(gen_unpack(cname, fields))
        out.append("")
        out.append(gen_unpack_ptr(cname))
        out.append("")
        out.append(gen_visualize(cname, fields))
        out.append("")
        out.append(gen_visualize_ptr(cname))
        out.append("")
        out.append(gen_visualize_ex_buf(cname))
        out.append("")
        out.append(gen_visualize_ex_ptr(cname))
        out.append("")
    out.append("} // namespace sec_proto")
    return "\n".join(out)


def main():
    for classes, basename in ((SECSVR_CLASSES, "formmog_proto_public_secsvr"),
                              (TSSSDK_CLASSES, "formmog_proto_public_tsssdk")):
        guard = "SECSVR_SRC_PROTOCOL_FORMMOG_%s_H_H_" % basename.upper()
        h = gen_header(classes, guard, basename)
        c = gen_cpp(classes, basename, guard)
        open("/tmp/%s.h" % basename, "w").write(h)
        open("/tmp/%s.cpp" % basename, "w").write(c)


if __name__ == "__main__":
    main()
