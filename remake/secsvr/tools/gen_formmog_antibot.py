#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate formmog_proto_antibot_client.{h,cpp} for secagent.

Semantics reverse engineered from the secagent binary (GCC 4.1.0 -O2):
  - construct(): memset(this, 0, sizeof(*this)); return 0
    (union classes dispatch on the int64_t selector to the sub-object
     construct() or zero the scalar member; default returns 0)
  - pack(buf): big-endian scalars; fixed uint8_t/char arrays written raw;
    variable byte/char arrays: count field (u8/u16/u32) then raw bytes,
    count > capacity -> -7; room check -> -1; string members (char[] without
    a count field in the schema, e.g. info_/comment_/virus_name_/scan_module_)
    are NUL-forced and written with a u32 length prefix via writeString;
    object arrays: count > max -> -7, plain for loop (count==0 falls through
    to the next member); single objects pack(sub, 1); union-typed members
    dispatch on a selector member of the parent (cmd_id_/sub_protocol_id_/
    type_/id_) and call the union class method with that selector.
  - unpack(buf): mirror; short -> -2; strings via readBoundedString with
    maxLen = array size (NUL check -> -5, empty -> -4, too big -> -3,
    NULL -> -8); fixed arrays room-check then read bytes.
  - visualize(buf): printVariable("[name]", fmt) per scalar with
    fmt = u8 "0x%02x", u16 "%d", u32 "%u", int32 "%d", u64 "%llu";
    printArray("[name]", count) + textize(" 0x%02x") loop for byte arrays,
    textize(" %d"/" %u") loops for typed arrays; strings via printString;
    object members via printVariable("[name]", true) then sub.visualize with
    indent+1 when indent >= 0; object arrays via printVariable("[name]", i,
    true) then sub.visualize.
  - visualize_ex(buf): visualize then NUL at pos (if pos != limit) and return
    data(); the char* variant clamps to size-1 and returns buffer.
"""
from __future__ import print_function

import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"
OUT_H = os.path.join(ROOT, "source/secagent/src/protocol/formmog/formmog_proto_antibot_client.h")
OUT_CPP = os.path.join(ROOT, "source/secagent/src/protocol/formmog/formmog_proto_antibot_client.cpp")

IND = "    "
NOERR = "TdrError::TDR_NO_ERROR"


def E():
    return NOERR


def err(name):
    return "TdrError::%s" % name


# ---------------------------------------------------------------- DWARF parse

def parse_loc(s):
    parts = [int(x) for x in s.split(",")]
    if len(parts) == 1:
        return parts[0]
    if parts[0] == 0x23:
        rest = parts[1:]
    elif parts[0] == 0:
        return 0
    else:
        rest = parts
    val = 0
    shift = 0
    for b in rest:
        val |= (b & 0x7f) << shift
        if not (b & 0x80):
            break
        shift += 7
    return val


def get_classes():
    """Run dwarf_proto_extract and build {name: {'size', 'members': [...]}}."""
    out = subprocess.check_output(
        [sys.executable, os.path.join(ROOT, "toolchain/dwarf_proto_extract.py"),
         BIN, "formmog_proto_antibot_client"], cwd=ROOT).decode("utf-8", "replace")
    classes = {}
    cur = None
    for ln in out.splitlines():
        m = re.match(r"==== (\S+) size=(\d+) .*====", ln)
        if m:
            cur = {"name": m.group(1), "size": int(m.group(2)), "members": []}
            classes[cur["name"]] = cur
            continue
        if cur is None:
            continue
        m = re.match(r"  member (\S+) @\[([0-9, ]+)\] line(None|[0-9]+) : (.*)", ln)
        if m:
            cur["members"].append({
                "name": m.group(1),
                "off": parse_loc(m.group(2)),
                "line": m.group(3),
                "type": m.group(4).strip(),
            })
    return classes


# ------------------------------------------------- semantic tables (verified)

# char[] members that are TDR <string> (u32 length prefix, NUL-terminated)
STR_MEMBERS = {
    "APCReportInfo": ["info_"],
    "SLogReport": ["comment_"],
    "SanlixScanResult": ["virus_name_", "scan_module_"],
}

# char[] members that are raw fixed byte arrays (no length prefix)
RAW_CHAR_MEMBERS = {
    "TROJANINFO": ["path_name_"],
    "TAB_CS_VERIFYRESULT": ["rc_check_"],
}

# variable array pairs: class -> (count member, array member)
# count member type comes from the DWARF member table.
VAR_PAIRS = {
    "SCS_SLN_DATA_BUF": [("send_data_len_", "send_data_")],
    "SAB_SIGN_INFO": [("sig_size_", "sig_data_")],
    "SLogReport": [("int_count_", "int_values_")],
    "SLX_TROJAN_INFO": [("trojan_count_", "trojans_")],
    "SRP_RPCODE_HASH": [("pad_data_len_", "pad_"), ("hash_count_", "hash_"),
                        ("garbage_data_len_", "garbage_")],
    "SSC_SE_SCAN_DATA_ITEM": [("cmd_size_", "cmd_data_")],
    "STP_CUSTOM_STRING": [("send_data_len_", "send_data_")],
    "STVMP_REPORT_CALL_INFO": [("func_call_info_size_", "func_call_infos_")],
    "TAB_CS_ANTICLTKEY": [("pad_len_", "pad_data_")],
    "TAB_CS_ANTITIME": [("pad_len_", "pad_data_")],
    "TAB_CS_CAPTCHAREQ": [("send_data_len_", "send_data_")],
    "TAB_CS_CAPTCHARESULT": [("send_data_len_", "send_data_")],
    "TAB_CS_CLIENTINFO": [("client_info_msg_size_", "client_info_msg_")],
    "TAB_CS_CLINETINFO": [("file_num_", "file_hash_data_")],
    "TAB_CS_CSCHECK": [("rpcs_result_len_", "rpcs_result_")],
    "TAB_CS_DELOG": [("pad_size_", "buf_")],
    "TAB_CS_DEREADY": [("err_msg_size_", "err_msg_")],
    "TAB_CS_DETESTPKG": [("content_size_", "content_")],
    "TAB_CS_ENCRYPT": [("pkg_len_", "pkg_data_")],
    "TAB_CS_ERRPACKET": [("err_info_size_", "err_info_")],
    "TAB_CS_EXTINFO": [("ext_info_len_", "ext_info_")],
    "TAB_CS_FMCRESULT": [("fms_result_len_", "fms_result_data_"),
                         ("pad_len_", "pad_data_")],
    "TAB_CS_IATCHECK": [("pad_len_", "pad_"), ("iat_info_size_", "iat_info_")],
    "TAB_CS_INITINFO": [("rp_data_info_size_", "rp_data_info_"),
                        ("len_", "send_data_")],
    "TAB_CS_LAPKGRESULT": [("la_result_len_", "aucLaResultData"),
                           ("pad_len_", "pad_data_")],
    "TAB_CS_LOCALCHECK": [("local_check_data_len_", "local_check_data_"),
                          ("num_msg_size_", "num_msg_")],
    "TAB_CS_NUMINFO": [("num_info_size_", "num_info_msg_")],
    "TAB_CS_RPFMCSCANRESULT": [("rpfmc_scan_result_len_", "rpfmc_scan_result_data_")],
    "TAB_CS_RPPKGRESULT": [("rpcode_result_data_size_", "rpcode_result_data_"),
                           ("pad_len_", "pad_data_")],
    "TAB_CS_SUSMODINFO": [("sus_mod_info_len_", "sus_mod_info_data_"),
                          ("pad_len_", "pad_data_")],
    "TAB_CS_SYSMEM": [("system_error_len_", "system_error_data_"),
                      ("pad_len_", "pad_data_")],
    "TAB_CS_SYSMEMLOG": [("system_log_size_", "system_log_"),
                         ("pad_len_", "pad_data_")],
    "TAB_CS_TPINFO": [("tp_info_msg_size_", "tp_info_msg_")],
    "TAB_CS_TVINFO": [("thread_num_", "thread_verify_infos_"),
                      ("pad_size_", "pad_buf_")],
    "TAB_CS_VERIFYRESULT": [("err_msg_size_", "err_msg_"),
                            ("send_data_len_", "send_data_")],
    "TAB_SC_CFGDATA": [("len_", "send_data_")],
    "TAB_SC_CSCFGDATA": [("rpcs_data_len_", "rpcs_data_")],
    "TAB_SC_DEDIBDATA": [("dib_data_len_", "dib_data_")],
    "TAB_SC_DEKEY": [("pad_data_size_", "pad_data_")],
    "TAB_SC_DE_CMDID_LIST": [("cmd_count_", "cmd_")],
    "TAB_SC_ENCRYPT": [("pkg_size_", "pkg_data_")],
    "TAB_SC_FMSCMD": [("fms_scn_cmd_len_", "fms_scn_cmd_data_")],
    "TAB_SC_LAPKGCMD": [("la_cmd_size_", "la_cmd_data_"),
                        ("pad_data_size_", "pad_data_")],
    "TAB_SC_PKGCODE": [("rpcode_data_size_", "rpcode_data_")],
    "TAB_SC_PKGCODE_KEY": [("key_info_len_", "key_info_")],
    "TAB_SC_RESTRICTUIN": [("pad_data_size_", "pad_data_")],
    "TAB_SC_RPFMCSCANCMD": [("rpfmc_scan_cmd_len_", "rpfmc_scan_cmd_data_")],
    "TAB_SC_SLXSHAREINFO": [("slx_data_len_", "slx_data_")],
    "TAB_SC_SUSMODQUEST": [("sus_addr_data_len_", "sus_addr_data_")],
    "TAB_SC_TENPRTYDATA": [("tenpary_data_len_", "tenpary_data_")],
    "TAB_SC_TESTPKG": [("content_size_", "content_")],
    "TFILE_HASH_INFO": [("count_", "file_hash_item_")],
    "TMEM_RECOVER": [("count_", "mem_item_")],
    "TMODULE_NAME": [("name_len_", "module_name_")],
    "TQD_CS_SCANRESULT": [("data_len_temp_", "data_temp_"),
                          ("virus_cnt_", "virus_info_")],
    "PKGCODE_KEY_INFO": [("key_size_", "key_data_")],
}

# union classes: member name -> C++ type (order = schema member order)
UNION_MEMBERS = {
    "TROJAN_REPORT": [
        ("trojans_", "SLX_TROJAN_INFO"),
        ("trojan_count_", "uint32_t"),
    ],
    "SAB_CS_SLN_DATA": [
        ("se_error_info_", "SSE_ERROR_INFO"),
        ("se_data_", "SCS_SLN_DATA_BUF"),
        ("shell_module_verify_", "ShellModuleVerify"),
        ("sanlix_scan_result_", "SanlixScanResult"),
        ("tp_error_info_", "STP_ERROR_INFO"),
        ("tp_custom_string_", "STP_CUSTOM_STRING"),
        ("tp_bin_log_report_", "SLogReport"),
        ("client_guid_info_", "SSchemeClientGUIDInfo"),
        ("tp_kr_report_info_", "STPKRReportInfo"),
        ("tp_apc_report_info_", "APCReportInfo"),
        ("ab_auth_code_", "SCS_SLN_DATA_BUF"),
        ("ab_mac_info_", "SAB_MAC_INFO"),
        ("ab_sign_info_", "SAB_SIGN_INFO"),
        ("ab_exp_data_", "SCS_SLN_DATA_BUF"),
        ("rp_msg_result_", "SCS_SLN_DATA_BUF"),
        ("rp_thread_exp_", "SRP_THREAD_EXP"),
        ("rp_func_exp_", "SCS_SLN_DATA_BUF"),
        ("rp_rpcode_hash_", "SRP_RPCODE_HASH"),
        ("tvmp_report_call_info_", "STVMP_REPORT_CALL_INFO"),
        ("tvmp_report_hash_info_", "STVMP_REPORT_HASH_INFO"),
    ],
    "SC_SCHEMEDATA": [
        ("se_scan_data_", "SSC_SE_SCAN_DATA"),
        ("sx_judge_result_", "SanlixScanJudgeResult"),
        ("tp_svr_config_", "SSC_SLN_DATA"),
        ("rp_cp_data_", "SSC_SLN_DATA"),
        ("sx_cfg_info_", "SSC_SLN_DATA"),
    ],
    "TORDINAL_OR_NAME": [
        ("name_", "TMODULE_NAME"),
        ("ordinal_", "TORDINAL"),
    ],
    "AntibotPkgBody": [
        ("sc_send_antibot_", "TAB_SC_SENDANTIBOT"),
        ("sc_rand_check_", "TAB_SC_RANDCHECK"),
        ("cs_init_info_", "TAB_CS_INITINFO"),
        ("cs_verify_result_", "TAB_CS_VERIFYRESULT"),
        ("sc_stop_l_check_", "TAB_SC_STOPLCHECK"),
        ("cs_resend_data_", "TAB_CS_RESENDDATA"),
        ("sc_resend_data_", "TAB_SC_RESENDDATA"),
        ("cs_p2p_stat_info_", "TAB_CS_P2PSTATINFO"),
        ("sc_cfg_data_", "TAB_SC_CFGDATA"),
        ("sc_req_cli_resend_data_", "TAB_SC_REQCLIRESENDDATA"),
        ("cs_client_info_", "TAB_CS_CLIENTINFO"),
        ("cs_tp_info_", "TAB_CS_TPINFO"),
        ("sc_qd_cfg_data_", "TQD_SC_CFGDATA"),
        ("sc_qd_cmd_data_", "TQD_SC_CMDDATA"),
        ("cs_qd_scan_result_", "TQD_CS_SCANRESULT"),
        ("cs_qd_init_data_", "TQD_CS_INITDATA"),
        ("cs_sanlix_report_", "TSANLIX_CS_REPORT"),
        ("sc_tenparty_data_", "TAB_SC_TENPRTYDATA"),
        ("sc_file_hash_", "TAB_SC_FILE_HASH"),
        ("cs_cli_net_info_", "TAB_CS_CLINETINFO"),
        ("sc_mem_recover_", "TAB_SC_MEM_RECOVER"),
        ("cs_ext_info_", "TAB_CS_EXTINFO"),
        ("cs_local_check_", "TAB_CS_LOCALCHECK"),
        ("cs_num_info_", "TAB_CS_NUMINFO"),
        ("sc_de_cmd_id_list_", "TAB_SC_DE_CMDID_LIST"),
        ("sc_de_dib_data_", "TAB_SC_DEDIBDATA"),
        ("sc_de_key_", "TAB_SC_DEKEY"),
        ("cs_de_ready_", "TAB_CS_DEREADY"),
        ("cs_de_test_pkg_", "TAB_CS_DETESTPKG"),
        ("sc_test_pkg_", "TAB_SC_TESTPKG"),
        ("cs_de_log_", "TAB_CS_DELOG"),
        ("sc_encrypt_", "TAB_SC_ENCRYPT"),
        ("cs_encrypt_", "TAB_CS_ENCRYPT"),
        ("cs_err_packet_", "TAB_CS_ERRPACKET"),
        ("cs_sys_mem_", "TAB_CS_SYSMEM"),
        ("cs_sys_mem_log_", "TAB_CS_SYSMEMLOG"),
        ("sc_restrict_uin_", "TAB_SC_RESTRICTUIN"),
        ("cs_iat_check_", "TAB_CS_IATCHECK"),
        ("sc_cs_cfg_data_", "TAB_SC_CSCFGDATA"),
        ("cs_cs_check_", "TAB_CS_CSCHECK"),
        ("sc_susmod_quest_", "TAB_SC_SUSMODQUEST"),
        ("cs_sus_mod_info_", "TAB_CS_SUSMODINFO"),
        ("cs_tv_info_", "TAB_CS_TVINFO"),
        ("sc_tv_check_", "TAB_SC_TVCHECK"),
        ("sc_slx_share_info_", "TAB_SC_SLXSHAREINFO"),
        ("sc_scheme_data_", "TAB_SC_SCHEMEDATA"),
        ("cs_scheme_data_", "TAB_CS_SCHEMEDATA"),
        ("sc_slx_te_share_info_", "TAB_SC_SLXSHAREINFO"),
        ("sc_rpfmc_scan_cmd_", "TAB_SC_RPFMCSCANCMD"),
        ("cs_rp_fmc_scan_result_", "TAB_CS_RPFMCSCANRESULT"),
        ("sc_la_pkg_cmd_", "TAB_SC_LAPKGCMD"),
        ("cs_la_pkg_result_", "TAB_CS_LAPKGRESULT"),
        ("sc_fms_cmd_", "TAB_SC_FMSCMD"),
        ("cs_fmc_result_", "TAB_CS_FMCRESULT"),
        ("sc_anti_cfg_info_", "TAB_SC_ANTICFGINFO"),
        ("cs_anti_time_", "TAB_CS_ANTITIME"),
        ("cs_anti_clt_key_", "TAB_CS_ANTICLTKEY"),
        ("cs_captcha_result_", "TAB_CS_CAPTCHARESULT"),
        ("cs_captcha_req_", "TAB_CS_CAPTCHAREQ"),
        ("cs_report_client_guid_", "SClientGUIDInfo"),
        ("cs_report_error_", "AB_CS_REPORT_ERROR"),
        ("sc_pkg_code_", "TAB_SC_PKGCODE"),
        ("cs_rp_pkg_result_", "TAB_CS_RPPKGRESULT"),
        ("sc_pkg_code_key_", "TAB_SC_PKGCODE_KEY"),
        ("sc_channel_server_id_", "TAB_SC_CHANNEL_SERVER_ID"),
        ("cs_cschannel_result_", "TAB_CS_CSCHANNEL_RESULT"),
        ("sc_channel_delay_check_", "TAB_SC_DELAY_CHECK"),
        ("cs_channel_delay_check_", "TAB_CS_DELAY_CHECK"),
    ],
}

# dispatch tables: union class -> {case: member name}
UNION_DISPATCH = {
    "TROJAN_REPORT": {
        0: "trojans_",
        1: "trojan_count_",
    },
    "SAB_CS_SLN_DATA": {
        1: "se_error_info_",
        19: "shell_module_verify_",
        20: "sanlix_scan_result_",
        100: "tp_error_info_",
        101: "tp_custom_string_",
        106: "tp_bin_log_report_",
        107: "client_guid_info_",
        108: "tp_kr_report_info_",
        109: "tp_apc_report_info_",
        201: "ab_auth_code_",
        202: "ab_mac_info_",
        203: "ab_sign_info_",
        204: "ab_exp_data_",
        302: "rp_msg_result_",
        304: "rp_thread_exp_",
        305: "rp_func_exp_",
        306: "rp_rpcode_hash_",
        601: "tvmp_report_call_info_",
        602: "tvmp_report_hash_info_",
    },
    "SC_SCHEMEDATA": {
        3: "se_scan_data_",
        21: "sx_judge_result_",
        105: "tp_svr_config_",
        308: "rp_cp_data_",
        402: "sx_cfg_info_",
    },
    "TORDINAL_OR_NAME": {
        0: "name_",
        1: "ordinal_",
    },
    "AntibotPkgBody": {
        1: "sc_send_antibot_",
        2: "sc_rand_check_",
        3: "cs_init_info_",
        4: "cs_verify_result_",
        5: "sc_stop_l_check_",
        6: "cs_resend_data_",
        8: "sc_resend_data_",
        10: "cs_p2p_stat_info_",
        11: "sc_cfg_data_",
        12: "sc_req_cli_resend_data_",
        13: "cs_client_info_",
        15: "cs_tp_info_",
        16: "sc_qd_cfg_data_",
        17: "sc_qd_cmd_data_",
        18: "cs_qd_scan_result_",
        19: "cs_qd_init_data_",
        20: "cs_sanlix_report_",
        21: "sc_tenparty_data_",
        22: "sc_file_hash_",
        23: "cs_cli_net_info_",
        24: "sc_mem_recover_",
        25: "cs_ext_info_",
        26: "cs_local_check_",
        27: "cs_num_info_",
        28: "sc_de_cmd_id_list_",
        29: "sc_de_dib_data_",
        30: "sc_de_key_",
        31: "cs_de_ready_",
        32: "cs_de_test_pkg_",
        33: "sc_test_pkg_",
        34: "cs_de_log_",
        37: "sc_encrypt_",
        38: "cs_encrypt_",
        39: "cs_err_packet_",
        40: "cs_sys_mem_",
        41: "cs_sys_mem_log_",
        42: "sc_restrict_uin_",
        43: "cs_iat_check_",
        44: "sc_cs_cfg_data_",
        45: "cs_cs_check_",
        46: "sc_susmod_quest_",
        47: "cs_sus_mod_info_",
        48: "cs_tv_info_",
        49: "sc_tv_check_",
        50: "sc_slx_share_info_",
        51: "sc_scheme_data_",
        52: "cs_scheme_data_",
        53: "sc_slx_te_share_info_",
        54: "sc_rpfmc_scan_cmd_",
        55: "cs_rp_fmc_scan_result_",
        56: "sc_la_pkg_cmd_",
        57: "cs_la_pkg_result_",
        58: "sc_fms_cmd_",
        59: "cs_fmc_result_",
        60: "sc_anti_cfg_info_",
        61: "cs_anti_time_",
        62: "cs_anti_clt_key_",
        64: "cs_captcha_result_",
        65: "cs_captcha_req_",
        66: "cs_report_client_guid_",
        70: "cs_report_error_",
        100: "sc_pkg_code_",
        101: "cs_rp_pkg_result_",
        102: "sc_pkg_code_key_",
        104: "sc_channel_server_id_",
        105: "cs_cschannel_result_",
        120: "sc_channel_delay_check_",
        121: "cs_channel_delay_check_",
    },
}

# range cases for SAB_CS_SLN_DATA: cases 2..18 all map to se_data_
SAB_RANGE = (2, 18, "se_data_")

# parent classes with a union sub-object: member -> selector expression
UNION_SELECTOR = {
    "AntibotClientPkg": ("body_", "head_.cmd_id_"),
    "TAB_SC_SCHEMEDATA": ("data_", "sub_protocol_id_"),
    "TAB_CS_SCHEMEDATA": ("data_", "sub_protocol_id_"),
    "TSANLIX_CS_REPORT": ("trojan_", "type_"),
    "TMEM_RECOVER_ITEM": ("name_or_ordinal_", "id_"),
}

UNION_CLASSES = set(UNION_MEMBERS.keys())


# -------------------------------------------------------------- field model

SCALAR_TYPES = {
    "uint8_t": "u8",
    "uint16_t": "u16",
    "uint32_t": "u32",
    "uint64_t": "u64",
    "int32_t": "int32",
}

WIRE_TYPES = {
    "u8": ("uint8_t", "writeUInt8", "readUInt8", "0x%02x"),
    "u16": ("uint16_t", "writeUInt16", "readUInt16", "%d"),
    "u32": ("uint32_t", "writeUInt32", "readUInt32", "%u"),
    "u64": ("uint64_t", "writeUInt64", "readUInt64", "%llu"),
    "int32": ("int32_t", "writeUInt32", "readUInt32", "%d"),
}


def pair_map(cls):
    out = {}
    for cnt, arr in VAR_PAIRS.get(cls, []):
        out[arr] = cnt
    return out


def classify(cls, members, classes):
    """Return (fields, consumed) where fields are (kind, payload...) tuples."""
    pairs = pair_map(cls)
    consumed = set(pairs.values())
    fields = []
    def cnt_kind(cnt):
        for mm in members:
            if mm["name"] == cnt:
                t = mm["type"]
                if t in SCALAR_TYPES:
                    return SCALAR_TYPES[t]
        raise ValueError("count %s not found in %s" % (cnt, cls))
    for m in members:
        name, typ = m["name"], m["type"]
        if name in consumed and name in pairs.values():
            continue  # count field handled with its array
        if typ in SCALAR_TYPES:
            fields.append(("scalar", name, SCALAR_TYPES[typ]))
            continue
        if name in STR_MEMBERS.get(cls, []):
            size = classes[cls]["size"] - m["off"]
            for mm in members:
                if mm["off"] > m["off"]:
                    size = mm["off"] - m["off"]
                    break
            fields.append(("str", name, size))
            continue
        if name in RAW_CHAR_MEMBERS.get(cls, []):
            size = classes[cls]["size"] - m["off"]
            for mm in members:
                if mm["off"] > m["off"]:
                    size = mm["off"] - m["off"]
                    break
            fields.append(("fixed8", name, size))
            continue
        if typ == "uint8_t[]" or typ == "char[]":
            size = classes[cls]["size"] - m["off"]
            for mm in members:
                if mm["off"] > m["off"]:
                    size = mm["off"] - m["off"]
                    break
            if name in pairs:
                fields.append(("buf", name, size, pairs[name], cnt_kind(pairs[name])))
            else:
                fields.append(("fixed8", name, size))
            continue
        if typ in ("uint16_t[]", "uint32_t[]", "int32_t[]"):
            size = classes[cls]["size"] - m["off"]
            for mm in members:
                if mm["off"] > m["off"]:
                    size = mm["off"] - m["off"]
                    break
            elem = typ.replace("[]", "")
            fields.append(("typedarr", name, elem, size, pairs[name], cnt_kind(pairs[name])))
            continue
        if typ.endswith("[]"):
            size = classes[cls]["size"] - m["off"]
            for mm in members:
                if mm["off"] > m["off"]:
                    size = mm["off"] - m["off"]
                    break
            fields.append(("objarr", name, typ[:-2], size, pairs[name], cnt_kind(pairs[name])))
            continue
        if typ in classes:
            fields.append(("obj", name, typ))
            continue
        raise ValueError("%s: unknown member %s of type %s" % (cls, name, typ))
    return fields


# ---------------------------------------------------------------- generation

HEADER = """\
// Reconstructed from secagent disassembly (TDR-generated protocol classes)
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_

#include <stdint.h>
#include <stddef.h>
#include "src/protocol/common/TdrError.h"

namespace tsf4g_tdr {
class TdrWriteBuf;
class TdrReadBuf;
}

namespace sec_proto {

#pragma pack(push, 1)

{classes}
#pragma pack(pop)

} // namespace sec_proto

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_PROTO_ANTIBOT_CLIENT_H_H_
"""

CPP_HEADER = """\
// Reconstructed from secagent disassembly (TDR-generated protocol classes)

#include "src/protocol/formmog/formmog_proto_antibot_client.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <string.h>
#include <stdint.h>

namespace {

// 读取带长度前缀的字符串（长度字段含结尾 NUL），单个元素最大 maxLen 字节。
tsf4g_tdr::TdrError::ErrorType readBoundedString(tsf4g_tdr::TdrReadBuf& destBuf, char* dest, size_t maxLen) {
    uint32_t size;
    tsf4g_tdr::TdrError::ErrorType ret = destBuf.readUInt32(size);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (destBuf.capacity() - destBuf.size() < size) return tsf4g_tdr::TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
    if (size > maxLen) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
    if (size == 0) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_SMALL;
    if (dest == NULL) return tsf4g_tdr::TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
    for (uint32_t i = 0; i < size; ++i) {
        char c;
        ret = destBuf.readChar(c);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        dest[i] = c;
    }
    if (dest[size - 1] != '\\0') return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    if (strlen(dest) + 1 != size) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_CONFLICT;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

} // anonymous namespace

namespace sec_proto {

using namespace tsf4g_tdr;

"""


def member_decl(f):
    kind = f[0]
    if kind == "scalar":
        return "    %s %s;" % (WIRE_TYPES[f[2]][0], f[1])
    if kind == "str":
        return "    char %s[%d];" % (f[1], f[2])
    if kind == "fixed8":
        return "    uint8_t %s[%d];" % (f[1], f[2])
    if kind == "buf":
        return "    %s %s;\n    uint8_t %s[%d];" % (
            WIRE_TYPES[f[4]][0], f[3], f[1], f[2])
    if kind == "typedarr":
        return "    %s %s;\n    %s %s[%d];" % (
            WIRE_TYPES[f[5]][0], f[4], f[2], f[1], f[3])
    if kind == "objarr":
        cnt, arr = f[4], f[1]
        cntkind = f[5]
        maxn = f[3] // class_size(f[2])
        return "    %s %s;\n    %s %s[%d];" % (
            WIRE_TYPES[cntkind][0], cnt, f[2], arr, maxn)
    if kind == "obj":
        return "    %s %s;" % (f[2], f[1])
    raise ValueError(kind)


def class_size(name):
    return classes[name]["size"]


# -------- per-method generators

def gen_construct(cname, fields):
    L = ["TdrError::ErrorType %s::construct() {" % cname]
    L.append(IND + "memset(this, 0, sizeof(*this));")
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_pack(cname, fields, classes):
    L = ["TdrError::ErrorType %s::pack(TdrWriteBuf& destBuf, unsigned int cutVer) const {" % cname]
    L.append(IND + "TdrError::ErrorType ret = " + E() + ";")
    for f in fields:
        kind = f[0]
        if kind == "scalar":
            name, wt = f[1], f[2]
            ctype, wfn, _, _ = WIRE_TYPES[wt]
            L += [
                IND + "ret = destBuf.%s(%s);" % (wfn, ("(uint32_t)" if wt == "int32" else "") + name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "str":
            name, size = f[1], f[2]
            L += [
                IND + "((char*)%s)[sizeof(%s) / sizeof(%s[0]) - 1] = '\\0';" % (name, name, name),
                IND + "ret = destBuf.writeString(%s);" % name,
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "fixed8":
            name, n = f[1], f[2]
            L += [
                IND + "if (destBuf.capacity() - destBuf.size() < %d) return %s;" % (n, err("TDR_ERR_SHORT_BUF_FOR_WRITE")),
                IND + "for (unsigned int i = 0; i < %d; ++i) {" % n,
                IND * 2 + "ret = destBuf.writeUInt8(%s[i]);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "buf":
            name, maxn, cnt, ck = f[1], f[2], f[3], f[4]
            ctype, wfn, _, _ = WIRE_TYPES[ck]
            L += [
                IND + "ret = destBuf.%s(%s);" % (wfn, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, maxn, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + "if (destBuf.capacity() - destBuf.size() < %s) return %s;" % (cnt, err("TDR_ERR_SHORT_BUF_FOR_WRITE")),
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "ret = destBuf.writeUInt8(%s[i]);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "typedarr":
            name, elem, maxn, cnt, ck = f[1], f[2], f[3], f[4], f[5]
            ctype, wfn, _, _ = WIRE_TYPES[ck]
            et = WIRE_TYPES[SCALAR_TYPES.get(elem, "u32")][1]
            esize = 2 if elem == "uint16_t" else 4
            L += [
                IND + "ret = destBuf.%s(%s);" % (wfn, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, maxn // esize, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "ret = destBuf.%s(%s[i]);" % (et, name),
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "objarr":
            name, typ, maxn, cnt, ck = f[1], f[2], f[3], f[4], f[5]
            ctype, wfn, _, _ = WIRE_TYPES[ck]
            mx = maxn // class_size(typ)
            L += [
                IND + "ret = destBuf.%s(%s);" % (wfn, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, mx, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "ret = %s[i].pack(destBuf, 1);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "obj":
            name, typ = f[1], f[2]
            if cname in UNION_SELECTOR and UNION_SELECTOR[cname][0] == name:
                sel = UNION_SELECTOR[cname][1]
                L += [
                    IND + "ret = %s.pack((int64_t)%s, destBuf, 1);" % (name, sel),
                    IND + "if (ret != " + E() + ") return ret;",
                ]
            else:
                L += [
                    IND + "ret = %s.pack(destBuf, 1);" % name,
                    IND + "if (ret != " + E() + ") return ret;",
                ]
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_unpack(cname, fields, classes):
    L = ["TdrError::ErrorType %s::unpack(TdrReadBuf& srcBuf, unsigned int cutVer) {" % cname]
    L.append(IND + "TdrError::ErrorType ret = " + E() + ";")
    for f in fields:
        kind = f[0]
        if kind == "scalar":
            name, wt = f[1], f[2]
            ctype, _, rfn, _ = WIRE_TYPES[wt]
            if wt == "int32":
                L += [
                    IND + "uint32_t tmp4%s;" % name,
                    IND + "ret = srcBuf.%s(tmp4%s);" % (rfn, name),
                    IND + "if (ret != " + E() + ") return ret;",
                    IND + "%s = (int32_t)tmp4%s;" % (name, name),
                ]
            else:
                L += [
                    IND + "ret = srcBuf.%s(%s);" % (rfn, name),
                    IND + "if (ret != " + E() + ") return ret;",
                ]
        elif kind == "str":
            name, size = f[1], f[2]
            L += [
                IND + "ret = readBoundedString(srcBuf, %s, sizeof(%s));" % (name, name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "fixed8":
            name, n = f[1], f[2]
            L += [
                IND + "if (srcBuf.capacity() - srcBuf.size() < %d) return %s;" % (n, err("TDR_ERR_SHORT_BUF_FOR_READ")),
                IND + "for (unsigned int i = 0; i < %d; ++i) {" % n,
                IND * 2 + "char c;",
                IND * 2 + "ret = srcBuf.readChar(c);",
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "%s[i] = (uint8_t)c;" % name,
                IND + "}",
            ]
        elif kind == "buf":
            name, maxn, cnt, ck = f[1], f[2], f[3], f[4]
            ctype, _, rfn, _ = WIRE_TYPES[ck]
            L += [
                IND + "ret = srcBuf.%s(%s);" % (rfn, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, maxn, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + "if (srcBuf.capacity() - srcBuf.size() < %s) return %s;" % (cnt, err("TDR_ERR_SHORT_BUF_FOR_READ")),
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "char c;",
                IND * 2 + "ret = srcBuf.readChar(c);",
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "%s[i] = (uint8_t)c;" % name,
                IND + "}",
            ]
        elif kind == "typedarr":
            name, elem, maxn, cnt, ck = f[1], f[2], f[3], f[4], f[5]
            ctype, _, rfn, _ = WIRE_TYPES[ck]
            esize = 2 if elem == "uint16_t" else 4
            L += [
                IND + "ret = srcBuf.%s(%s);" % (rfn, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, maxn // esize, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
            ]
            if elem == "int32_t":
                L += [
                    IND * 2 + "uint32_t tmp4%s;" % name,
                    IND * 2 + "ret = srcBuf.readUInt32(tmp4%s);" % name,
                    IND * 2 + "if (ret != " + E() + ") return ret;",
                    IND * 2 + "%s[i] = (int32_t)tmp4%s;" % (name, name),
                ]
            else:
                et = WIRE_TYPES[SCALAR_TYPES[elem]][2]
                L += [
                    IND * 2 + "ret = srcBuf.%s(%s[i]);" % (et, name),
                    IND * 2 + "if (ret != " + E() + ") return ret;",
                ]
            L.append(IND + "}")
        elif kind == "objarr":
            name, typ, maxn, cnt, ck = f[1], f[2], f[3], f[4], f[5]
            ctype, _, rfn, _ = WIRE_TYPES[ck]
            mx = maxn // class_size(typ)
            L += [
                IND + "ret = srcBuf.%s(%s);" % (rfn, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, mx, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 2 + "ret = %s[i].unpack(srcBuf, 1);" % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "obj":
            name, typ = f[1], f[2]
            if cname in UNION_SELECTOR and UNION_SELECTOR[cname][0] == name:
                sel = UNION_SELECTOR[cname][1]
                L += [
                    IND + "ret = %s.unpack((int64_t)%s, srcBuf, 1);" % (name, sel),
                    IND + "if (ret != " + E() + ") return ret;",
                ]
            else:
                L += [
                    IND + "ret = %s.unpack(srcBuf, 1);" % name,
                    IND + "if (ret != " + E() + ") return ret;",
                ]
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_visualize(cname, fields, classes):
    L = ["TdrError::ErrorType %s::visualize(TdrWriteBuf& destBuf, int indent, char separator) const {" % cname]
    L.append(IND + "TdrError::ErrorType ret = " + E() + ";")
    for f in fields:
        kind = f[0]
        if kind == "scalar":
            name, wt = f[1], f[2]
            fmt = WIRE_TYPES[wt][3]
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%s", %s);' % (name, fmt, name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "str":
            name = f[1]
            L += [
                IND + 'ret = TdrBufUtil::printString(destBuf, indent, separator, "[%s]", %s);' % (name, name),
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "fixed8":
            name, n = f[1], f[2]
            L += [
                IND + 'ret = TdrBufUtil::printArray(destBuf, indent, separator, "[%s]", %d);' % (name, n),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "for (unsigned int i = 0; i < %d; ++i) {" % n,
                IND * 2 + 'ret = destBuf.textize(" 0x%%02x", %s[i]);' % name,
                IND * 2 + "if (ret != " + E() + ") return ret;",
                IND + "}",
            ]
        elif kind == "buf":
            name, maxn, cnt, ck = f[1], f[2], f[3], f[4]
            fmt = WIRE_TYPES[ck][3]
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%s", %s);' % (cnt, fmt, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, maxn, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + 'ret = TdrBufUtil::printArray(destBuf, indent, separator, "[%s]", %s);' % (name, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s != 0) {" % cnt,
                IND * 2 + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 3 + 'ret = destBuf.textize(" 0x%%02x", %s[i]);' % name,
                IND * 3 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "}",
                IND + "}",
                IND + "ret = destBuf.writeChar(separator);",
                IND + "if (ret != " + E() + ") return ret;",
                IND + "ret = destBuf.writeChar('\\0');",
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "typedarr":
            name, elem, maxn, cnt, ck = f[1], f[2], f[3], f[4], f[5]
            fmt = WIRE_TYPES[ck][3]
            efmt = " %d" if elem == "int32_t" or elem == "uint16_t" else " %u"
            esize = 2 if elem == "uint16_t" else 4
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%s", %s);' % (cnt, fmt, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, maxn // esize, err("TDR_ERR_REFER_SURPASS_COUNT")),
                IND + 'ret = TdrBufUtil::printArray(destBuf, indent, separator, "[%s]", %s);' % (name, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s != 0) {" % cnt,
                IND * 2 + "for (uint32_t i = 0; i < %s; ++i) {" % cnt,
                IND * 3 + 'ret = destBuf.textize("%s", %s[i]);' % (efmt, name),
                IND * 3 + "if (ret != " + E() + ") return ret;",
                IND * 2 + "}",
                IND + "}",
                IND + "ret = destBuf.writeChar(separator);",
                IND + "if (ret != " + E() + ") return ret;",
                IND + "ret = destBuf.writeChar('\\0');",
                IND + "if (ret != " + E() + ") return ret;",
            ]
        elif kind == "objarr":
            name, typ, maxn, cnt, ck = f[1], f[2], f[3], f[4], f[5]
            fmt = WIRE_TYPES[ck][3]
            mx = maxn // class_size(typ)
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%s", %s);' % (cnt, fmt, cnt),
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (%s > %d) return %s;" % (cnt, mx, err("TDR_ERR_REFER_SURPASS_COUNT")),
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
        elif kind == "obj":
            name, typ = f[1], f[2]
            L += [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", true);' % name,
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (indent >= 0) {",
            ]
            if cname in UNION_SELECTOR and UNION_SELECTOR[cname][0] == name:
                sel = UNION_SELECTOR[cname][1]
                L += [
                    IND * 2 + "ret = %s.visualize((int64_t)%s, destBuf, indent + 1, separator);" % (name, sel),
                ]
            else:
                L += [
                    IND * 2 + "ret = %s.visualize(destBuf, indent + 1, separator);" % name,
                ]
            L += [
                IND + "} else {",
            ]
            if cname in UNION_SELECTOR and UNION_SELECTOR[cname][0] == name:
                sel = UNION_SELECTOR[cname][1]
                L += [
                    IND * 2 + "ret = %s.visualize((int64_t)%s, destBuf, indent, separator);" % (name, sel),
                ]
            else:
                L += [
                    IND * 2 + "ret = %s.visualize(destBuf, indent, separator);" % name,
                ]
            L += [
                IND + "}",
                IND + "return ret;",
            ]
    L.append(IND + "return " + E() + ";")
    L.append("}")
    return "\n".join(L)


def gen_pack_ptr(cname, is_union):
    if is_union:
        sig = ("TdrError::ErrorType %s::pack(int64_t union_selector, char* buffer, "
               "unsigned int size, unsigned int* usedSize, unsigned int cutVer) const {")
        inner = "pack(union_selector, destBuf, cutVer)"
    else:
        sig = ("TdrError::ErrorType %s::pack(char* buffer, unsigned int size, "
               "unsigned int* usedSize, unsigned int cutVer) const {")
        inner = "pack(destBuf, cutVer)"
    return "\n".join([
        sig % cname,
        IND + "if (buffer == NULL) return %s;" % err("TDR_ERR_NULL_POINTER_PARAMETER"),
        IND + "TdrWriteBuf destBuf(buffer, size);",
        IND + "TdrError::ErrorType ret = %s;" % inner,
        IND + "if (usedSize != NULL) *usedSize = destBuf.size();",
        IND + "return ret;",
        "}",
    ])


def gen_unpack_ptr(cname, is_union):
    if is_union:
        sig = ("TdrError::ErrorType %s::unpack(int64_t union_selector, const char* buffer, "
               "unsigned int size, unsigned int* usedSize, unsigned int cutVer) {")
        inner = "unpack(union_selector, srcBuf, cutVer)"
    else:
        sig = ("TdrError::ErrorType %s::unpack(const char* buffer, unsigned int size, "
               "unsigned int* usedSize, unsigned int cutVer) {")
        inner = "unpack(srcBuf, cutVer)"
    return "\n".join([
        sig % cname,
        IND + "if (buffer == NULL) return %s;" % err("TDR_ERR_NULL_POINTER_PARAMETER"),
        IND + "TdrReadBuf srcBuf(buffer, size);",
        IND + "TdrError::ErrorType ret = %s;" % inner,
        IND + "if (usedSize != NULL) *usedSize = srcBuf.size();",
        IND + "return ret;",
        "}",
    ])


def gen_visualize_ptr(cname, is_union):
    if is_union:
        sig = ("TdrError::ErrorType %s::visualize(int64_t union_selector, char* buffer, "
               "unsigned int size, unsigned int* usedSize, int indent, char separator) const {")
        inner = "visualize(union_selector, destBuf, indent, separator)"
    else:
        sig = ("TdrError::ErrorType %s::visualize(char* buffer, unsigned int size, "
               "unsigned int* usedSize, int indent, char separator) const {")
        inner = "visualize(destBuf, indent, separator)"
    return "\n".join([
        sig % cname,
        IND + "if (buffer == NULL) return %s;" % err("TDR_ERR_NULL_POINTER_PARAMETER"),
        IND + "TdrWriteBuf destBuf(buffer, size);",
        IND + "TdrError::ErrorType ret = %s;" % inner,
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


# -------- union classes

def union_switch_body(cname, verb, classes):
    """Emit the switch dispatch for a union class method."""
    dispatch = UNION_DISPATCH[cname]
    members = dict(UNION_MEMBERS[cname])
    L = ["switch (union_selector) {"]
    cases = sorted(dispatch.keys())
    # SAB range cases 2..18 -> se_data_
    if cname == "SAB_CS_SLN_DATA":
        rlo, rhi, rmname = SAB_RANGE
        typ = members[rmname]
        L.append(IND + "case 2:")
        L.append(IND + "case 3:")
        L.append(IND + "case 4:")
        L.append(IND + "case 5:")
        L.append(IND + "case 6:")
        L.append(IND + "case 7:")
        L.append(IND + "case 8:")
        L.append(IND + "case 9:")
        L.append(IND + "case 10:")
        L.append(IND + "case 11:")
        L.append(IND + "case 12:")
        L.append(IND + "case 13:")
        L.append(IND + "case 14:")
        L.append(IND + "case 15:")
        L.append(IND + "case 16:")
        L.append(IND + "case 17:")
        L.append(IND + "case 18:")
        if verb == "construct":
            body = [IND + "ret = %s.construct();" % rmname]
        elif verb == "pack":
            body = [IND + "ret = %s.pack(destBuf, 1);" % rmname]
        elif verb == "unpack":
            body = [IND + "ret = %s.unpack(srcBuf, 1);" % rmname]
        else:
            body = [
                IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", true);' % rmname,
                IND + "if (ret != " + E() + ") return ret;",
                IND + "if (indent >= 0) {",
                IND * 2 + "ret = %s.visualize(destBuf, indent + 1, separator);" % rmname,
                IND + "} else {",
                IND * 2 + "ret = %s.visualize(destBuf, indent, separator);" % rmname,
                IND + "}",
            ]
        L.extend(body)
        L.append(IND + "break;")
    for case in cases:
        mname = dispatch[case]
        typ = members[mname]
        if verb == "construct":
            if typ in classes:
                body = [IND + "ret = %s.construct();" % mname]
            else:
                body = [IND + "%s = 0;" % mname, IND + "ret = " + E() + ";"]
        elif verb == "pack":
            if typ in classes:
                body = [IND + "ret = %s.pack(destBuf, 1);" % mname]
            else:
                wt = "u32" if typ == "uint32_t" else ("u16" if typ == "uint16_t" else "u8")
                wfn = WIRE_TYPES[wt][1]
                body = [IND + "ret = destBuf.%s(%s);" % (wfn, mname)]
        elif verb == "unpack":
            if typ in classes:
                body = [IND + "ret = %s.unpack(srcBuf, 1);" % mname]
            else:
                wt = "u32" if typ == "uint32_t" else ("u16" if typ == "uint16_t" else "u8")
                rfn = WIRE_TYPES[wt][2]
                body = [IND + "ret = srcBuf.%s(%s);" % (rfn, mname)]
        elif verb == "visualize":
            if typ in classes:
                body = [
                    IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", true);' % mname,
                    IND + "if (ret != " + E() + ") return ret;",
                    IND + "if (indent >= 0) {",
                    IND * 2 + "ret = %s.visualize(destBuf, indent + 1, separator);" % mname,
                    IND + "} else {",
                    IND * 2 + "ret = %s.visualize(destBuf, indent, separator);" % mname,
                    IND + "}",
                ]
            else:
                wt = "u32" if typ == "uint32_t" else ("u16" if typ == "uint16_t" else "u8")
                fmt = WIRE_TYPES[wt][3]
                body = [IND + 'ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[%s]", "%s", %s);' % (mname, fmt, mname)]
        else:
            raise ValueError(verb)
        L.append(IND + "case %d:" % case)
        L.extend(body)
        L.append(IND + "break;")
    L.append(IND + "default:")
    L.append(IND * 2 + "ret = " + E() + ";")
    L.append(IND + "}")
    return "\n".join(L)


def gen_union_construct(cname, classes):
    return "\n".join([
        "TdrError::ErrorType %s::construct(int64_t union_selector) {" % cname,
        IND + "TdrError::ErrorType ret = " + E() + ";",
        union_switch_body(cname, "construct", classes),
        IND + "return ret;",
        "}",
    ])


def gen_union_pack(cname, classes):
    return "\n".join([
        "TdrError::ErrorType %s::pack(int64_t union_selector, TdrWriteBuf& destBuf, unsigned int cutVer) const {" % cname,
        IND + "TdrError::ErrorType ret = " + E() + ";",
        union_switch_body(cname, "pack", classes),
        IND + "return ret;",
        "}",
    ])


def gen_union_unpack(cname, classes):
    return "\n".join([
        "TdrError::ErrorType %s::unpack(int64_t union_selector, TdrReadBuf& srcBuf, unsigned int cutVer) {" % cname,
        IND + "TdrError::ErrorType ret = " + E() + ";",
        union_switch_body(cname, "unpack", classes),
        IND + "return ret;",
        "}",
    ])


def gen_union_visualize(cname, classes):
    return "\n".join([
        "TdrError::ErrorType %s::visualize(int64_t union_selector, TdrWriteBuf& destBuf, int indent, char separator) const {" % cname,
        IND + "TdrError::ErrorType ret = " + E() + ";",
        union_switch_body(cname, "visualize", classes),
        IND + "return ret;",
        "}",
    ])


# -------- header

def gen_header_class(cname, fields, classes):
    if cname in UNION_CLASSES:
        parts = ["class %s {" % cname, "public:"]
        parts.append("    union {")
        for mname, typ in UNION_MEMBERS[cname]:
            parts.append("        %s %s;" % (typ, mname))
        parts.append("    };")
        parts.append("")
        parts.append("    tsf4g_tdr::TdrError::ErrorType construct(int64_t union_selector);")
        parts.append("    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, unsigned int cutVer) const;")
        parts.append("    tsf4g_tdr::TdrError::ErrorType pack(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer) const;")
        parts.append("    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, tsf4g_tdr::TdrReadBuf& srcBuf, unsigned int cutVer);")
        parts.append("    tsf4g_tdr::TdrError::ErrorType unpack(int64_t union_selector, const char* buffer, unsigned int size, unsigned int* usedSize, unsigned int cutVer);")
        parts.append("    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, tsf4g_tdr::TdrWriteBuf& destBuf, int indent, char separator) const;")
        parts.append("    tsf4g_tdr::TdrError::ErrorType visualize(int64_t union_selector, char* buffer, unsigned int size, unsigned int* usedSize, int indent, char separator) const;")
        parts.append("};")
        return "\n".join(parts)
    parts = ["class %s {" % cname, "public:"]
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
    return "\n".join(parts)


def topo_order(classes):
    """Order classes so referenced types are declared first."""
    order = []
    done = set()

    def deps(cname):
        ds = set()
        if cname in UNION_CLASSES:
            for _, typ in UNION_MEMBERS[cname]:
                if typ in classes:
                    ds.add(typ)
        else:
            for f in classify(cname, classes[cname]["members"], classes):
                if f[0] in ("obj", "objarr"):
                    ds.add(f[2])
        return ds

    def visit(cname):
        if cname in done:
            return
        done.add(cname)
        for d in deps(cname):
            visit(d)
        order.append(cname)

    for cname in classes:
        visit(cname)
    return order


def gen_cpp(classes):
    out = [CPP_HEADER]
    order = topo_order(classes)
    for cname in order:
        if cname in UNION_CLASSES:
            out.append(gen_union_construct(cname, classes))
            out.append("")
            out.append(gen_union_pack(cname, classes))
            out.append("")
            out.append(gen_pack_ptr(cname, True))
            out.append("")
            out.append(gen_union_unpack(cname, classes))
            out.append("")
            out.append(gen_unpack_ptr(cname, True))
            out.append("")
            out.append(gen_union_visualize(cname, classes))
            out.append("")
            out.append(gen_visualize_ptr(cname, True))
            out.append("")
        else:
            fields = classify(cname, classes[cname]["members"], classes)
            out.append(gen_construct(cname, fields))
            out.append("")
            out.append(gen_pack(cname, fields, classes))
            out.append("")
            out.append(gen_pack_ptr(cname, False))
            out.append("")
            out.append(gen_unpack(cname, fields, classes))
            out.append("")
            out.append(gen_unpack_ptr(cname, False))
            out.append("")
            out.append(gen_visualize(cname, fields, classes))
            out.append("")
            out.append(gen_visualize_ptr(cname, False))
            out.append("")
            out.append(gen_visualize_ex_buf(cname))
            out.append("")
            out.append(gen_visualize_ex_ptr(cname))
            out.append("")
    out.append("} // namespace sec_proto")
    return "\n".join(out)


def gen_header(classes):
    order = topo_order(classes)
    parts = []
    for cname in order:
        if cname in UNION_CLASSES:
            fields = []
        else:
            fields = classify(cname, classes[cname]["members"], classes)
        parts.append(gen_header_class(cname, fields, classes))
        parts.append("")
    return HEADER.replace("{classes}", "\n".join(parts))


def main():
    global classes
    classes = get_classes()
    print("classes:", len(classes))
    h = gen_header(classes)
    c = gen_cpp(classes)
    open(OUT_H, "w").write(h)
    open(OUT_CPP, "w").write(c)
    print("wrote", OUT_H, len(h.splitlines()), "lines")
    print("wrote", OUT_CPP, len(c.splitlines()), "lines")


if __name__ == "__main__":
    main()
