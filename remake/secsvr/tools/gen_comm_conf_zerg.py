#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate comm_conf_zerg.cpp for gunnersvr from the TDR codegen pattern.

Semantics were reverse engineered from the gunnersvr binary:
  - fromXml* wraps TdrXmlReader and calls fromXml (root stepIn/entry/stepOut)
  - entryFromXml: getEntryValue/getNodeValue + TdrParse::parse*; missing value ->
    field default; object fields: stepIn success -> entryFromXml(reader, 1),
    failure -> construct()
  - toXml* wraps TdrXmlWriter (openSimple/openComplex + entryToListXml or
    entryToAttrXml + close*); entryToListXml emits list-style tags, entryToAttrXml
    emits name="value" attributes; scalar arrays are written list-style in both
    modes; object arrays use openComplex/closeComplex per element.
"""

from __future__ import print_function

IND = "    "
NOERR = "TdrError::TDR_NO_ERROR"


def E():
    return NOERR


def err(name):
    return "TdrError::%s" % name


HEADER = """\
// Reconstructed from gunnersvr disassembly (TDR-generated config structures)

#include "src/protocol/common/comm_conf_zerg.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"
#include "src/protocol/common/TdrXml.h"
#include "src/protocol/common/TdrIO.h"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml.hpp"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

using namespace tsf4g_tdr;
using namespace conf_zerg;

"""


# Field descriptors:
#   ('name', 'u8'|'u16'|'u32', default)
#   ('name', 'str', default_str, size)
#   ('name', 'str_empty', size)
#   ('name', 'obj', 'Type')
#   ('name', 'obj_last', 'Type')
#   ('name', 'arr_obj', 'Type', max, count_field, var)
#   ('name', 'arr_u32', max, count_field, var)
CLASSES = [
    ("SvrInfo", True, [
        ("svr_type", "u16", 0),
        ("svr_id", "u32", 0),
        ("use_encrypt", "u8", 0),
    ]),
    ("SelfCfg", False, [
        ("self_svr_info", "obj", "SvrInfo"),
        ("slave_svr_count", "u8", 3, 0),
        ("slave_svrs", "arr_obj", "SvrInfo", 3, "slave_svr_count", "slave_svrs_i", "uint8_t"),
    ]),
    ("LogCfg", True, [
        ("log_level", "str", "debug", 32),
    ]),
    ("CommCfg", True, [
        ("max_frame_len", "u32", 0xfc00),
        ("accept_send_buf_size", "u32", 0x20),
        ("connect_send_deque_size", "u32", 0x100),
        ("connect_timeout", "u32", 0xf),
        ("recv_timeout", "u32", 0),
        ("is_proxy", "u8", 0),
        ("max_accept_svr", "u32", 0x400),
        ("is_lock_pipe", "u8", 0),
        ("retry_error", "u8", 3),
        ("recv_pipe_len", "u32", 0x3200000),
        ("send_pipe_len", "u32", 0x3200000),
        ("error_pipe_len", "u32", 0x3200000),
        ("check_pthread", "u8", 1),
        ("insurance", "u8", 1),
        ("opt_key1", "str_empty", 128),
        ("opt_key2", "str_empty", 128),
        ("get_svr_info_type", "str", "cfgfile", 64),
    ]),
    ("CfgSvrCfg", False, [
        ("svr_info", "obj", "SvrInfo"),
        ("svr_ip", "str_empty", 16),
        ("svr_port", "u16", 0),
    ]),
    ("ConsoleCfg", True, [
        ("use_console", "u8", 0),
        ("console_ip", "str", "127.0.0.1", 16),
        ("console_port", "u16", 8001),
    ]),
    ("CheckCfg", False, [
        ("check_frame", "u8", 0),
        ("check_rcvid", "u8", 0),
        ("num_snd_type", "u8", 10, 3),
        ("allow_snd_types", "arr_u32", 10, "num_snd_type", "allow_snd_types_i", "uint8_t"),
    ]),
    ("AutoConnectSvr", False, [
        ("main_svr", "obj", "SvrInfo"),
        ("backup_svr", "obj_last", "SvrInfo"),
    ]),
    ("AutoConnectCfg", False, [
        ("auto_connect_num", "u32", 100, 1),
        ("auto_connect_svrs", "arr_obj", "AutoConnectSvr", 100, "auto_connect_num", "auto_connect_svrs_i", "uint32_t"),
    ]),
    ("RestrictCfg", True, [
        ("allow_ips", "str_empty", 20480),
        ("reject_ips", "str_empty", 20480),
    ]),
    ("MonitorCfg", False, [
        ("monitor_num", "u32", 100),
        ("monitor_cmds", "arr_u32", 100, "monitor_num", "monitor_cmds_i", "uint32_t"),
    ]),
    ("zerg_config", False, [
        ("self_cfg", "obj", "SelfCfg"),
        ("log_cfg", "obj", "LogCfg"),
        ("comm_cfg", "obj", "CommCfg"),
        ("cfgsvr_cfg", "obj", "CfgSvrCfg"),
        ("console_cfg", "obj", "ConsoleCfg"),
        ("check_cfg", "obj", "CheckCfg"),
        ("auto_connect_cfg", "obj", "AutoConnectCfg"),
        ("restrict_cfg", "obj", "RestrictCfg"),
        ("monitor_cfg", "obj", "MonitorCfg"),
    ]),
]


def parse_call(field):
    name, kind = field[0], field[1]
    default = field[2]
    if len(field) >= 4:
        default = field[3]
    fn = {"u8": "TdrParse::parseUInt8", "u16": "TdrParse::parseUInt16",
          "u32": "TdrParse::parseUInt32"}[kind]
    return "%s(this->%s, value4%s, NULL, %d, NULL, NULL)" % (fn, name, name, default)


def list_fmt(kind):
    return {"u8": "0x%02x ", "u16": "%d ", "u32": "%u "}[kind]


def attr_fmt(kind):
    return {"u8": "0x%02x", "u16": "%d", "u32": "%u"}[kind]


def gen_construct(cls):
    cname, simple, fields = cls
    L = ["TdrError::ErrorType %s::construct() {" % cname]
    if cname == "CheckCfg":
        L += [
            IND + "this->check_frame = 0;",
            IND + "this->check_rcvid = 0;",
            IND + "this->num_snd_type = 3;",
            IND + "memset(this->allow_snd_types, 0, 10);",
            IND + "return " + E() + ";",
        ]
    elif cname == "CommCfg":
        L += [
            IND + "this->max_frame_len = 0xfc00;",
            IND + "this->accept_send_buf_size = 0x20;",
            IND + "this->connect_send_deque_size = 0x100;",
            IND + "this->connect_timeout = 0xf;",
            IND + "this->recv_timeout = 0;",
            IND + "this->is_proxy = 0;",
            IND + "this->max_accept_svr = 0x400;",
            IND + "this->is_lock_pipe = 0;",
            IND + "this->retry_error = 3;",
            IND + "this->recv_pipe_len = 0x3200000;",
            IND + "this->send_pipe_len = 0x3200000;",
            IND + "this->error_pipe_len = 0x3200000;",
            IND + "this->check_pthread = 1;",
            IND + "this->insurance = 1;",
            IND + "this->opt_key1[0] = 0;",
            IND + "this->opt_key2[0] = 0;",
            IND + "memset(this->get_svr_info_type, 0, sizeof(this->get_svr_info_type));",
            IND + 'strcpy(this->get_svr_info_type, "cfgfile");',
            IND + "return " + E() + ";",
        ]
    elif cname == "ConsoleCfg":
        L += [
            IND + "this->use_console = 0;",
            IND + "memset(this->console_ip, 0, sizeof(this->console_ip));",
            IND + 'strcpy(this->console_ip, "127.0.0.1");',
            IND + "this->console_port = 8001;",
            IND + "return " + E() + ";",
        ]
    elif cname == "LogCfg":
        L += [
            IND + "memset(this->log_level, 0, sizeof(this->log_level));",
            IND + 'strcpy(this->log_level, "debug");',
            IND + "return " + E() + ";",
        ]
    elif cname == "AutoConnectCfg":
        L += [
            IND + "TdrError::ErrorType ret;",
            IND + "uint32_t auto_connect_svrs_i;",
            IND + "this->auto_connect_num = 1;",
            IND + "for (auto_connect_svrs_i = 0; auto_connect_svrs_i < 100; auto_connect_svrs_i++) {",
            IND * 2 + "ret = this->auto_connect_svrs[auto_connect_svrs_i].construct();",
            IND * 2 + "if (ret != " + E() + ") {",
            IND * 3 + "break;",
            IND * 2 + "}",
            IND + "}",
            IND + "return ret;",
        ]
    elif cname == "zerg_config":
        L.append(IND + "TdrError::ErrorType ret;")
        for fld in ["self_cfg", "log_cfg", "comm_cfg", "cfgsvr_cfg", "console_cfg",
                    "check_cfg", "auto_connect_cfg", "restrict_cfg"]:
            L += [
                IND + "ret = this->%s.construct();" % fld,
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
            ]
        L.append(IND + "return this->monitor_cfg.construct();")
    else:
        L += [
            IND + "memset(this, 0, sizeof(*this));",
            IND + "return " + E() + ";",
        ]
    L.append("}")
    return "\n".join(L)


def gen_fromxml_wrappers(cname):
    L = []
    L += [
        "TdrError::ErrorType %s::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (file != NULL) {",
        IND * 2 + "TdrXmlFile xmlfile;",
        IND * 2 + "ret = xmlfile.load(file);",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + "ret = fromXmlString(xmlfile.buf(), format, cutVer);",
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
        "TdrError::ErrorType %s::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + err("TDR_ERR_NULL_POINTER_PARAMETER") + ";",
        IND + "if (str != NULL) {",
        IND * 2 + "TdrXmlReader reader(str, format);",
        IND * 2 + "ret = reader.state();",
        IND * 2 + "if (ret == " + E() + ") {",
        IND * 3 + "ret = fromXml(reader, cutVer);",
        IND * 2 + "}",
        IND + "}",
        IND + "return ret;",
        "}",
    ]
    return "\n".join(L)


def gen_fromxml(cname):
    L = [
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
    ]
    return "\n".join(L)


def gen_entryfromxml(cls):
    cname, simple, fields = cls
    L = ["TdrError::ErrorType %s::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {" % cname]
    L.append(IND + "TdrError::ErrorType ret;")
    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32", "str", "str_empty"):
            L.append(IND + "const char *value4%s;" % name)
        elif kind == "arr_u32":
            L.append(IND + "const char *value4%s;" % name)
            L.append(IND + "unsigned int tempCount4%s;" % name)
        elif kind == "arr_obj":
            L.append(IND + "%s %s;" % (f[6], f[5]))
    L.append("")

    for f in fields:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32"):
            dflt = f[2]
            L += [
                IND + 'value4%s = reader.getEntryValue("%s");' % (name, name),
                IND + "if (value4%s == NULL) {" % name,
                IND * 2 + "ret = " + E() + ";",
                IND * 2 + "this->%s = %d;" % (name, dflt),
                IND + "} else {",
                IND * 2 + "ret = " + parse_call(f) + ";",
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND + "}",
            ]
            if cname == "SelfCfg" and name == "slave_svr_count":
                L += [
                    IND + "if (this->slave_svr_count > 3) {",
                    IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                    IND + "}",
                    IND + "if (this->slave_svr_count == 0) {",
                    IND * 2 + "return " + E() + ";",
                    IND + "}",
                ]
            if cname == "AutoConnectCfg" and name == "auto_connect_num":
                L += [
                    IND + "if (this->auto_connect_num > 100) {",
                    IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                    IND + "}",
                    IND + "if (this->auto_connect_num == 0) {",
                    IND * 2 + "return " + E() + ";",
                    IND + "}",
                ]
            if cname == "MonitorCfg" and name == "monitor_num":
                L += [
                    IND + "if (this->monitor_num > 100) {",
                    IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                    IND + "}",
                ]
            if cname == "CheckCfg" and name == "num_snd_type":
                L += [
                    IND + "if (this->num_snd_type > 10) {",
                    IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                    IND + "}",
                ]
        elif kind in ("str", "str_empty"):
            size = f[3] if kind == "str" else f[2]
            L += [
                IND + 'value4%s = reader.getEntryValue("%s");' % (name, name),
                IND + "if (value4%s == NULL) {" % name,
            ]
            if kind == "str":
                dflt = f[2]
                L += [
                    IND * 2 + "memset(this->%s, 0, %d);" % (name, size),
                    IND * 2 + 'strcpy(this->%s, "%s");' % (name, dflt),
                ]
            else:
                L.append(IND * 2 + "this->%s[0] = 0;" % name)
            L += [
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
        elif kind == "obj_last":
            L += [
                IND + 'if (reader.stepIn("%s") != TdrXmlReader::WS_NORMAL) {' % name,
                IND * 2 + "return this->%s.construct();" % name,
                IND + "}",
                IND + "ret = this->%s.entryFromXml(reader, 1);" % name,
                IND + "if (ret != " + E() + ") {",
                IND * 2 + "return ret;",
                IND + "}",
                IND + 'reader.stepOut("%s");' % name,
                IND + "return ret;",
            ]
            return "\n".join(L + ["}"])
        elif kind == "arr_obj":
            typ, mx, cnt, var = f[2], f[3], f[4], f[5]
            L += [
                IND + "for (%s = 0; %s < this->%s; %s++) {" % (var, var, cnt, var),
                IND * 2 + 'if (reader.stepIn("%s") != TdrXmlReader::WS_NORMAL) {' % name,
                IND * 3 + "this->%s = %s;" % (cnt, var),
                IND * 3 + "break;",
                IND * 2 + "}",
                IND * 2 + "ret = this->%s[%s].entryFromXml(reader, 1);" % (name, var),
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND * 2 + 'reader.stepOut("%s");' % name,
                IND + "}",
            ]
        elif kind == "arr_u32":
            mx, cnt = f[2], f[3]
            L += [
                IND + 'value4%s = reader.getNodeValue("%s");' % (name, name),
                IND + "if (value4%s == NULL) {" % name,
                IND * 2 + "this->%s = 0;" % cnt,
                IND * 2 + "return " + E() + ";",
                IND + "}",
                IND + "tempCount4%s = 0;" % name,
                IND + "ret = TdrParse::parseUInt32(this->%s, this->%s, value4%s, &tempCount4%s, 0, NULL, NULL);" % (name, cnt, name, name),
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
    xml_decl = 'ret = writer._out.textize("<?xml version=\\"1.0\\" encoding=\\"%s\\" standalone=\\"yes\\" ?>\\n", "GBK");'
    L = []
    L += [
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
        IND * 3 + xml_decl,
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
        IND * 3 + xml_decl,
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
    ]
    return "\n".join(L)


def gen_toxml(cname, simple):
    op, cl = ("openSimple", "closeSimple") if simple else ("openComplex", "closeComplex")
    L = [
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
    ]
    return "\n".join(L)


def field_open(name, indent):
    return "\n".join([
        indent + "if (writer._indentPending) {",
        indent + '    ret = writer._out.textize(">\\n%s<%s> ", writer._indentStr, "' + name + '");',
        indent + "    writer._indentPending = false;",
        indent + "    writer._state = ret;",
        indent + "    if (writer._state != " + E() + ") {",
        indent + "        return writer._state;",
        indent + "    }",
        indent + "} else {",
        indent + '    ret = writer._out.textize("%s<%s> ", writer._indentStr, "' + name + '");',
        indent + "    writer._state = ret;",
        indent + "    if (writer._state != " + E() + ") {",
        indent + "        return writer._state;",
        indent + "    }",
        indent + "}",
    ])


def gen_entryxml(cls, attr):
    cname, simple, fields = cls
    fname = "entryToAttrXml" if attr else "entryToListXml"
    L = ["TdrError::ErrorType %s::%s(TdrXmlWriter &writer, unsigned int cutVer) const {" % (cname, fname)]
    L.append(IND + "TdrError::ErrorType ret;")
    for f in fields:
        if f[1] == "arr_u32":
            L.append(IND + "%s %s;" % (f[5], f[4]))
        elif f[1] == "arr_obj":
            L.append(IND + "%s %s;" % (f[6], f[5]))
    ordered = fields
    if attr:
        ordered = [f for f in fields if f[1] in ("u8", "u16", "u32", "str", "str_empty", "arr_u32")] + \
                  [f for f in fields if f[1] in ("obj", "obj_last", "arr_obj")]

    for f in ordered:
        name, kind = f[0], f[1]
        if kind in ("u8", "u16", "u32"):
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
                    field_open(name, IND),
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
        elif kind in ("str", "str_empty"):
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
                    field_open(name, IND),
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
        elif kind in ("obj", "obj_last"):
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
        elif kind == "arr_obj":
            typ, mx, cnt, var = f[2], f[3], f[4], f[5]
            sub = "entryToAttrXml" if attr else "entryToListXml"
            L += [
                IND + "if (this->%s > %d) {" % (cnt, mx),
                IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                IND + "}",
                IND + "if (this->%s == 0) {" % cnt,
                IND * 2 + "return " + E() + ";",
                IND + "}",
                IND + "for (%s = 0; %s < this->%s; %s++) {" % (var, var, cnt, var),
                IND * 2 + 'ret = writer.openComplex("%s");' % name,
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND * 2 + "ret = this->%s[%s].%s(writer, 1);" % (name, var, sub),
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND * 2 + 'ret = writer.closeComplex("%s");' % name,
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
                IND * 2 + "}",
                IND + "}",
                IND + "return ret;",
            ]
            L.append("}")
            return "\n".join(L)
        elif kind == "arr_u32":
            mx, cnt, var = f[2], f[3], f[4]
            L += [
                IND + "if (this->%s > %d) {" % (cnt, mx),
                IND * 2 + "return " + err("TDR_ERR_REFER_SURPASS_COUNT") + ";",
                IND + "}",
                field_open(name, IND),
                IND + "for (%s = 0; %s < this->%s; %s++) {" % (var, var, cnt, var),
                IND * 2 + 'ret = writer.textize("%s", this->%s[%s]);' % (list_fmt("u32"), name, var),
                IND * 2 + "if (ret != " + E() + ") {",
                IND * 3 + "return ret;",
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
    out = [HEADER]
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
    print("\n".join(out))


if __name__ == "__main__":
    main()
