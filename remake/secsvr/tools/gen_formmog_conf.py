#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate formmog_conf_secagent.{h,cpp} for secagent from TDR codegen pattern.

Semantics reverse engineered from the secagent binary:
  - construct(): scalars default; strings strncpy(dst, default, 256);
    empty-default strings set [0]=0; sub-objects construct() with early return;
    CmdListInfo: count=1 + memset(cmd_, 0, 30).
  - entryFromXml(): getEntryValue + TdrParse::parse* (default passed to parse);
    missing value -> field default; strings: strlen > size-1 -> -3,
    strncpy(dst, value, size); sub-objects stepIn/entryFromXml(reader,1)/stepOut,
    stepIn failure -> construct(); CmdListInfo uses getNodeValue("cmd_") +
    parseUInt16(array, count, ...), count default 30, >30 -> -7, missing node ->
    count=0 + return NO_ERROR.
  - toXml: openSimple/openComplex by class; entryToListXml list-style,
    entryToAttrXml attr-style (scalars first, then objects as complex children).
"""
from __future__ import print_function

IND = "    "
NOERR = "TdrError::TDR_NO_ERROR"


def E():
    return NOERR


def err(name):
    return "TdrError::%s" % name


HEADER = """\
// Reconstructed from secagent disassembly (TDR-generated config structures)
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_SECAGENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_SECAGENT_H_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrXml.h"

namespace tsf4g_tdr {
class TdrXmlReader;
class TdrXmlWriter;
}

namespace secagent_config {

#pragma pack(push, 1)

"""

FOOTER = """\
#pragma pack(pop)

} // namespace secagent_config

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_SECAGENT_H_H_
"""

CPP_HEADER = """\
// Reconstructed from secagent disassembly (TDR-generated config structures)

#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/protocol/common/TdrXml.h"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

using namespace tsf4g_tdr;
using namespace secagent_config;

"""


# Field kinds:
#   ('name', 'int32', default)
#   ('name', 'u32', default)
#   ('name', 'u8', default)
#   ('name', 'str', default, size)         default may be "" (empty)
#   ('name', 'obj', 'Type')
#   ('name', 'arr_u16', max, count_field, var)
CLASSES = [
    ("SanlixChecker", True, [
        ("is_use_", "int32", 0),
        ("is_distribute_tenparty_data_", "int32", 1),
        ("tp_data_path_", "str", "./cfg/base_cfg/Tenparty.dat", 256),
        ("is_distribute_ts_data_", "int32", 1),
        ("ts_data_path_", "str", "./cfg/sanlix/TS0001.dat", 256),
        ("is_distribute_te_data_", "int32", 1),
        ("te_data_path_", "str", "./cfg/base_cfg/TE0001.dat", 256),
        ("is_distribute_tk_data_", "int32", 0),
        ("tk_data_path_", "str", "./cfg/base_cfg/TK0001.dat", 256),
    ]),
    ("CmdListInfo", False, [
        ("cmd_list_count_", "u32", 30),
        ("cmd_", "arr_u16", 30, "cmd_list_count_", "cmd__i"),
    ]),
    ("CryptCheckerConf", False, [
        ("is_use_", "int32", 0),
        ("crypt_test_gap_", "int32", 75),
        ("up_cmdlist_info_", "obj", "CmdListInfo"),
        ("down_cmdlist_info_", "obj", "CmdListInfo"),
        ("dib_base_path_", "str", "./cfg/dib/", 256),
        ("exe_path_", "str", "./cfg/game.exe", 256),
        ("exe_version_path_", "str", "./cfg/verlist.ini", 256),
        ("antibot_error_thread_", "u32", 5),
    ]),
    ("config", False, [
        ("is_for_mmog_", "u8", 1),
        ("if_restore_gmsvr_channel_", "u8", 1),
        ("if_check_pthread_", "u8", 1),
        ("if_restore_dpsdk_channel_", "u8", 1),
        ("if_restore_user_info_", "u8", 1),
        ("log_priority_", "u32", 4),
        ("expire_time_", "u32", 300),
        ("table_node_num_", "u32", 1000000),
        ("dp_config_path_", "str", "dp_config.xml", 256),
        ("report_statistic_time_", "u32", 5000),
        ("restrict_file_", "str", "./cfg/restrict.txt", 256),
        ("crypt_checker_conf_", "obj", "CryptCheckerConf"),
        ("max_package_size_", "u32", 0),
        ("if_rpcode_with_key_", "u32", 0),
        ("rpcode_keyinfo_key_", "str", "", 64),
        ("if_open_cs_channel_", "u8", 0),
        ("sanlix_checker_conf_", "obj", "SanlixChecker"),
        ("total_max_send_count_", "u32", 1000),
        ("if_check_rpcode_hash_", "u8", 0),
        ("gamesvr_channel_num_", "u32", 12),
        ("channel_recv_times_", "u8", 1),
    ]),
]


def member_decl(f):
    name, kind = f[0], f[1]
    if kind == "int32":
        return "    int32_t %s;" % name
    if kind == "u32":
        return "    uint32_t %s;" % name
    if kind == "u8":
        return "    uint8_t %s;" % name
    if kind == "str":
        return "    char %s[%d];" % (name, f[3])
    if kind == "obj":
        return "    %s %s;" % (f[2], name)
    if kind == "arr_u16":
        return "    uint16_t %s[%d];" % (name, f[2])
    raise ValueError(kind)


def gen_header():
    out = [HEADER]
    for cname, simple, fields in CLASSES:
        out.append("class %s {" % cname)
        out.append("public:")
        for f in fields:
            out.append(member_decl(f))
        out.append("")
        out.append("    tsf4g_tdr::TdrError::ErrorType construct();")
        out.append("    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);")
        out.append("    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);")
        out.append("    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer);")
        out.append("    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);")
        out.append("    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer);")
        out.append("    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;")
        out.append("    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;")
        out.append("    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;")
        out.append("    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const;")
        out.append("    tsf4g_tdr::TdrError::ErrorType entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;")
        out.append("    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;")
        out.append("};")
        out.append("")
    out.append(FOOTER)
    return "\n".join(out)


def parse_call(field):
    name, kind = field[0], field[1]
    default = field[2]
    fn = {"u8": "TdrParse::parseUInt8", "u16": "TdrParse::parseUInt16",
          "u32": "TdrParse::parseUInt32", "int32": "TdrParse::parseInt32"}[kind]
    return "%s(this->%s, value4%s, NULL, %d, NULL, NULL)" % (fn, name, name, default)


def list_fmt(kind):
    return {"u8": "0x%02x ", "u16": "%d ", "u32": "%u ", "int32": "%d "}[kind]


def attr_fmt(kind):
    return {"u8": "0x%02x", "u16": "%d", "u32": "%u", "int32": "%d"}[kind]


def gen_construct(cls):
    cname, simple, fields = cls
    L = ["TdrError::ErrorType %s::construct() {" % cname]
    if cname == "CmdListInfo":
        L += [
            IND + "this->cmd_list_count_ = 1;",
            IND + "memset(this->cmd_, 0, 30);",
            IND + "return " + E() + ";",
        ]
        L.append("}")
        return "\n".join(L)
    objs = [f for f in fields if f[1] == "obj"]
    if objs:
        L.append(IND + "TdrError::ErrorType ret;")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u32", "int32"):
            L.append(IND + "this->%s = %d;" % (name, f[2]))
        elif kind == "str":
            if f[2]:
                L.append(IND + 'strncpy(this->%s, "%s", %d);' % (name, f[2], f[3]))
            else:
                L.append(IND + "this->%s[0] = 0;" % name)
        elif kind == "obj":
            L += [
                IND + "ret = this->%s.construct();" % name,
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
            ]
    if objs:
        L.append(IND + "return ret;")
    else:
        L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_fromxml_wrappers(cname):
    return "\n".join([
        "TdrError::ErrorType %s::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (file != NULL) {",
        IND * 2 + "TdrXmlFile xmlFile;",
        IND * 2 + "ret = xmlFile.load(file);",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + "ret = fromXmlString(xmlFile.buf(), format, cutVer);",
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
        "",
        "TdrError::ErrorType %s::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (buffer != NULL) {",
        IND * 2 + "TdrXmlReader reader(buffer, size, format);",
        IND * 2 + "ret = reader.state();",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + "ret = fromXml(reader, cutVer);",
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
        "",
        "TdrError::ErrorType %s::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (string != NULL) {",
        IND * 2 + "TdrXmlReader reader(string, format);",
        IND * 2 + "ret = reader.state();",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + "ret = fromXml(reader, cutVer);",
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
    ])


def gen_fromxml(cname):
    return "\n".join([
        "TdrError::ErrorType %s::fromXml(TdrXmlReader &reader, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_INVALID_ROOT_NODE") + ";",
        IND + 'if (reader.stepIn("%s") == TdrXmlReader::WS_NORMAL) {' % cname,
        IND * 2 + "ret = entryFromXml(reader, cutVer);",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + 'reader.stepOut("%s");' % cname,
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
    ])


def gen_entryfromxml(cls):
    cname, simple, fields = cls
    L = ["TdrError::ErrorType %s::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {" % cname]
    L.append(IND + "TdrError::ErrorType ret;")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32", "int32", "str"):
            L.append(IND + "const char *value4%s;" % name)
        elif kind == "arr_u16":
            L.append(IND + "const char *value4%s;" % name)
            L.append(IND + "unsigned int tempCount4%s;" % name)
    L.append("")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u32", "int32"):
            dflt = f[2]
            L += [
                IND + 'value4%s = reader.getEntryValue("%s");' % (name, name),
                IND + "if (value4%s == NULL) {" % name,
                IND * 2 + "this->%s = %d;" % (name, dflt),
                IND + "} else {",
                IND * 2 + "ret = " + parse_call(f) + ";",
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND + "}",
            ]
        elif kind == "str":
            size = f[3]
            if f[2]:
                dflt = f[2]
                L += [
                    IND + 'value4%s = reader.getEntryValue("%s");' % (name, name),
                    IND + "if (value4%s == NULL) {" % name,
                    IND * 2 + 'strncpy(this->%s, "%s", %d);' % (name, dflt, size),
                    IND + "} else {",
                    IND * 2 + "const size_t length4%s = strlen(value4%s);" % (name, name),
                    IND * 2 + "if (length4%s > %d) {" % (name, size - 1),
                    IND * 3 + "return " + err("TDR_ERR_STR_LEN_TOO_BIG") + ";",
                    IND * 2 + "}",
                    IND * 2 + "strncpy(this->%s, value4%s, %d);" % (name, name, size),
                    IND + "}",
                ]
            else:
                L += [
                    IND + 'value4%s = reader.getEntryValue("%s");' % (name, name),
                    IND + "if (value4%s == NULL) {" % name,
                    IND * 2 + "this->%s[0] = 0;" % name,
                    IND + "} else {",
                    IND * 2 + "const size_t length4%s = strlen(value4%s);" % (name, name),
                    IND * 2 + "if (length4%s > %d) {" % (name, size - 1),
                    IND * 3 + "return " + err("TDR_ERR_STR_LEN_TOO_BIG") + ";",
                    IND * 2 + "}",
                    IND * 2 + "strncpy(this->%s, value4%s, %d);" % (name, name, size),
                    IND + "}",
                ]
        elif kind == "obj":
            L += [
                IND + 'if (reader.stepIn("%s") != TdrXmlReader::WS_NORMAL) {' % name,
                IND * 2 + "ret = this->%s.construct();" % name,
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND + "} else {",
                IND * 2 + "ret = this->%s.entryFromXml(reader, 1);" % name,
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND * 2 + 'reader.stepOut("%s");' % name,
                IND + "}",
            ]
        elif kind == "arr_u16":
            mx, cnt, var = f[2], f[3], f[4]
            L += [
                IND + 'value4%s = reader.getNodeValue("%s");' % (name, name),
                IND + "if (value4%s == NULL) {" % name,
                IND * 2 + "this->%s = 0;" % cnt,
                IND * 2 + "return ret;",
                IND + "}",
                IND + "tempCount4%s = 0;" % name,
                IND + "ret = TdrParse::parseUInt16(this->%s, this->%s, value4%s, &tempCount4%s, 0, NULL, NULL);" % (name, cnt, name, name),
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
                IND + "this->%s = tempCount4%s;" % (cnt, name),
            ]
    L += [
        IND + "return ret;",
        "}",
    ]
    return "\n".join(L)


def gen_towxml_wrappers(cname):
    return "\n".join([
        "TdrError::ErrorType %s::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "FILE *fp;",
        IND + 'fp = fopen(file, "wb");',
        IND + "if (fp != NULL) {",
        IND * 2 + "ret = toXmlFile(fp, format, cutVer);",
        IND * 2 + "fclose(fp);",
        IND + "}",
        IND + "return ret;",
        "}",
        "",
        "TdrError::ErrorType %s::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (fp != NULL) {",
        IND * 2 + "TdrXmlWriter writer(fp, format);",
        IND * 2 + "ret = writer.state();",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + 'ret = writer._out.textize("<?xml version=\\"1.0\\" encoding=\\"%s\\" standalone=\\"yes\\" ?>\\n", "GBK");',
        IND * 3 + "if (ret == " + E() + ") {",
        IND * 4 + "ret = toXml(writer, format, cutVer);",
        IND * 3 + "}",
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
        "",
        "TdrError::ErrorType %s::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (buffer != NULL) {",
        IND * 2 + "TdrXmlWriter writer(buffer, size, format);",
        IND * 2 + "ret = writer.state();",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + 'ret = writer._out.textize("<?xml version=\\"1.0\\" encoding=\\"%s\\" standalone=\\"yes\\" ?>\\n", "GBK");',
        IND * 3 + "if (ret == " + E() + ") {",
        IND * 4 + "ret = toXml(writer, format, cutVer);",
        IND * 4 + "if (ret == " + E() + " && usedSize != NULL) {",
        IND * 5 + "*usedSize = writer.getUsedSize();",
        IND * 4 + "}",
        IND * 3 + "}",
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
    ])


def gen_toxml(cname, simple):
    op, cl = ("openSimple", "closeSimple") if simple else ("openComplex", "closeComplex")
    return "\n".join([
        "TdrError::ErrorType %s::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {" % cname,
        IND + "TdrError::ErrorType ret;",
        IND + 'ret = writer.%s("%s");' % (op, cname),
        IND + "if (ret == " + E() + ") {",
        IND * 2 + "if (format == ATTR_ENTRY) {",
        IND * 3 + "ret = entryToAttrXml(writer, cutVer);",
        IND * 2 + "} else {",
        IND * 3 + "ret = entryToListXml(writer, cutVer);",
        IND * 2 + "}",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + 'ret = writer.%s("%s");' % (cl, cname),
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
    ])


def field_open(name):
    return "\n".join([
        IND + "if (writer._indentPending) {",
        IND + '    ret = writer._out.textize(">\\n%s<%s> ", writer._indentStr, "' + name + '");',
        IND + "    writer._indentPending = false;",
        IND + "    writer._state = ret;",
        IND + "    if (writer._state != " + E() + ") {",
        IND + "        return writer._state;",
        IND + "    }",
        IND + "} else {",
        IND + '    ret = writer._out.textize("%s<%s> ", writer._indentStr, "' + name + '");',
        IND + "    writer._state = ret;",
        IND + "    if (writer._state != " + E() + ") {",
        IND + "        return writer._state;",
        IND + "    }",
        IND + "}",
    ])


def gen_entryxml(cls, attr):
    cname, simple, fields = cls
    fname = "entryToAttrXml" if attr else "entryToListXml"
    L = ["TdrError::ErrorType %s::%s(TdrXmlWriter &writer, unsigned int cutVer) const {" % (cname, fname)]
    L.append(IND + "TdrError::ErrorType ret;")
    for f in fields:
        if f[1] == "arr_u16":
            L.append(IND + "unsigned int %s;" % f[4])
    ordered = fields
    if attr:
        ordered = [f for f in fields if f[1] in ("u8", "u16", "u32", "int32", "str")] + \
                  [f for f in fields if f[1] in ("obj", "arr_u16")]
    for f in ordered:
        name, kind = f[0], f[1]
        if kind in ("u8", "u32", "int32"):
            if attr:
                L += [
                    IND + 'ret = writer._out.textize(" %s=\\"", "' + name + '");',
                    IND + "writer._state = ret;",
                    IND + "if (writer._state != " + E() + ") {",
                    IND * 2 + "return writer._state;",
                    IND + "}",
                    IND + 'ret = writer.textize("%s", this->%s);' % (attr_fmt(kind), name),
                    IND + "if (ret != " + E() + ") {",
                    IND * 2 + "return ret;",
                    IND + "}",
                    IND + 'ret = writer._out.textize("\\"");',
                    IND + "writer._state = ret;",
                    IND + "if (writer._state != " + E() + ") {",
                    IND * 2 + "return writer._state;",
                    IND + "}",
                ]
            else:
                L += [
                    field_open(name),
                    IND + 'ret = writer.textize("%s", this->%s);' % (list_fmt(kind), name),
                    IND + "if (ret != " + E() + ") {",
                    IND * 2 + "return ret;",
                    IND + "}",
                    IND + 'ret = writer._out.textize("</%s>\\n", "' + name + '");',
                    IND + "writer._state = ret;",
                    IND + "if (writer._state != " + E() + ") {",
                    IND * 2 + "return writer._state;",
                    IND + "}",
                ]
        elif kind == "str":
            esc = "false" if attr else "true"
            if attr:
                L += [
                    IND + 'ret = writer._out.textize(" %s=\\"", "' + name + '");',
                    IND + "writer._state = ret;",
                    IND + "if (writer._state != " + E() + ") {",
                    IND * 2 + "return writer._state;",
                    IND + "}",
                    IND + "ret = writer.textizeStr(this->%s, %s);" % (name, esc),
                    IND + "if (ret != " + E() + ") {",
                    IND * 2 + "return ret;",
                    IND + "}",
                    IND + 'ret = writer._out.textize("\\"");',
                    IND + "writer._state = ret;",
                    IND + "if (writer._state != " + E() + ") {",
                    IND * 2 + "return writer._state;",
                    IND + "}",
                ]
            else:
                L += [
                    field_open(name),
                    IND + "ret = writer.textizeStr(this->%s, %s);" % (name, esc),
                    IND + "if (ret != " + E() + ") {",
                    IND * 2 + "return ret;",
                    IND + "}",
                    IND + 'ret = writer._out.textize("</%s>\\n", "' + name + '");',
                    IND + "writer._state = ret;",
                    IND + "if (writer._state != " + E() + ") {",
                    IND * 2 + "return writer._state;",
                    IND + "}",
                ]
        elif kind == "obj":
            sub = "entryToAttrXml" if attr else "entryToListXml"
            L += [
                IND + 'ret = writer.openComplex("%s");' % name,
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
                IND + "ret = this->%s.%s(writer, 1);" % (name, sub),
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
                IND + 'ret = writer.closeComplex("%s");' % name,
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
            ]
        elif kind == "arr_u16":
            mx, cnt, var = f[2], f[3], f[4]
            L += [
                IND + "if (this->%s > %d) {" % (cnt, mx),
                IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                IND + "}",
                field_open(name),
                IND + "if (this->%s != 0) {" % cnt,
                IND * 2 + "for (%s = 0; %s < this->%s; %s++) {" % (var, var, cnt, var),
                IND * 3 + 'ret = writer.textize("%s", this->%s[%s]);' % (list_fmt("u16"), name, var),
                IND * 3 + "if (ret != " + E() + ") {",
                IND * 4 + "return ret;",
                IND * 3 + "}",
                IND * 2 + "}",
                IND + "}",
                IND + 'ret = writer._out.textize("</%s>\\n", "' + name + '");',
                IND + "writer._state = ret;",
                IND + "if (writer._state != " + E() + ") {",
                IND * 2 + "return writer._state;",
                IND + "}",
                IND + "return ret;",
            ]
            L.append("}")
            return "\n".join(L)
    L += [
        IND + "return ret;",
        "}",
    ]
    return "\n".join(L)


def main():
    open("/tmp/formmog_conf_secagent.h", "w").write(gen_header())
    out = [CPP_HEADER]
    for cls in CLASSES:
        cname, simple, fields = cls
        out.append(gen_construct(cls))
        out.append("")
        out.append(gen_fromxml_wrappers(cname))
        out.append("")
        out.append(gen_fromxml(cname))
        out.append("")
        out.append(gen_entryfromxml(cls))
        out.append("")
        out.append(gen_towxml_wrappers(cname))
        out.append("")
        out.append(gen_toxml(cname, simple))
        out.append("")
        out.append(gen_entryxml(cls, False))
        out.append("")
        out.append(gen_entryxml(cls, True))
        out.append("")
    open("/tmp/formmog_conf_secagent.cpp", "w").write("\n".join(out))


if __name__ == "__main__":
    main()
