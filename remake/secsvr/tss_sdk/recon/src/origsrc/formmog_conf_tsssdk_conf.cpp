// Reconstruction of src/protocol/formmog/formmog_conf_tsssdk_conf.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// The CU reports 74 function symbols (cu_symbols.py on the ORIG): the 72
// tss_sdk_conf_desc:: functions of the CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_conf_tsssdk_conf.cpp
// (6 classes x 12: construct, fromXmlFile, fromXmlBuffer, fromXmlString,
// fromXml, entryFromXml, toXmlFile(FILE*), toXmlFile(const char*),
// toXmlBuffer, toXml, entryToListXml, entryToAttrXml) plus the CU's two weak
// tsf4g_tdr definitions, TdrXmlWriter::textize @ 0x000ee1b0 and
// TdrOutStream::textize @ 0x000ee1f0, 0x40 bytes each, which are inline
// members of the modelled TdrXml.h/TdrIO.h (see tsf4g_tdr_compat_cfg_conf.h)
// that the CU emits out-of-line because GCC 4.1 never inlines a variadic call.
//
// The sequence of every generated function is the ORIG's:
//   * construct(): the constants are the ORIG's stores
//     (LogConfig 3, UserControlconfig 1/20000, UicConfig 1/1228800/1200/...,
//     AddrInfo 0/"127.0.0.1"/0, CommConfig 1/0/0 + 32 x AddrInfo::construct(),
//     config: comm_config_.construct() ... uic_config_.construct()).
//   * entryFromXml(): one reader.getEntryValue("<name>") per scalar, then
//     TdrParse::parseXxx(dest, value, NULL, <construct default>, NULL, NULL);
//     strings are length-checked with strlen() > sizeof(array)-1 and copied
//     with strncpy(dest, value, sizeof(array)); a missing value re-applies the
//     construct() default; nested structs are reader.stepIn()/stepOut() and
//     fall back to construct(); the struct array reads server_num_ elements
//     with stepIn/entryFromXml/stepOut and rewrites server_num_ when a node
//     disappears.
//   * entryToAttrXml(): writer.openAttr()/closeAttr() per field,
//     writer.textize("%d"/"%u", value) for scalars, writer.textizeStr(array,
//     false) for char arrays, writer.openComplex()/closeComplex() around
//     nested structs, after every call `if (ret != TDR_NO_ERROR) return ret;`
//     (the ORIG tests the returned value in EAX, no writer._state reload).
//   * entryToListXml(): writer.openNode()/closeNode() per field,
//     writer.textize("%d "/"%u ", value), writer.textizeStr(array, true).
//     The ORIG checks the LIST tag writers through the writer state
//     (`writer.state()`), which is why every openNode/closeNode site has the
//     `mov %eax,0x1c(%esi); mov 0x1c(%esi),%eax; test` sequence (the ATTR
//     sites have only `test %eax,%eax; mov %eax,0x1c(%esi)`).
//   * toXml(): openSimple/closeSimple for the leaf classes (AddrInfo,
//     LogConfig, UicConfig, UserControlconfig), openComplex/closeComplex for
//     config and CommConfig.
//   * fromXml/fromXmlString/fromXmlBuffer/fromXmlFile and
//     toXml/toXmlBuffer/toXmlFile(FILE*)/toXmlFile(const char*): the
//     TdrXml reader/writer wrappers of the ORIG (TDR_ERR_NULL_POINTER_PARAMETER
//     -19, TDR_ERR_INVALID_ROOT_NODE -28, TDR_ERR_OPEN_FILE_WRITE_FAILED -23,
//     TdrXmlWriter::declare("GBK"), *usedSize = writer.getUsedSize(),
//     TdrXmlFile::loadState()/content()).
//
// ORIG error codes used here: -3 TDR_ERR_STR_LEN_TOO_BIG,
// -7 TDR_ERR_REFER_SURPASS_COUNT.

#include "formmog_conf_tsssdk_conf.h"

#include <string.h>

namespace tss_sdk_conf_desc {

using namespace tsf4g_tdr;

// ---------------------------------------------------------------------------
// AddrInfo (sizeof 38)
// ---------------------------------------------------------------------------

TdrError::ErrorType AddrInfo::construct() {
    this->plugin_id_ = 0;
    strncpy(this->ip_, "127.0.0.1", 32);
    this->port_ = 0;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AddrInfo::fromXmlFile(const char *file,
                                          TdrXmlFormat format,
                                          unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.loadState(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::fromXmlBuffer(const char *buffer,
                                            unsigned int size,
                                            TdrXmlFormat format,
                                            unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::fromXmlString(char *string, TdrXmlFormat format,
                                            unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != NULL) {
        TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::fromXml(TdrXmlReader &reader,
                                      unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("AddrInfo") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("AddrInfo");
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::entryFromXml(TdrXmlReader &reader,
                                           unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *value4plugin_id_ = reader.getEntryValue("plugin_id_");
    if (value4plugin_id_ == NULL) {
        this->plugin_id_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->plugin_id_, value4plugin_id_, NULL, 0,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4ip_ = reader.getEntryValue("ip_");
    if (value4ip_ == NULL) {
        strncpy(this->ip_, "127.0.0.1", 32);
    } else {
        const size_t length4ip_ = strlen(value4ip_);
        if (length4ip_ > 31) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->ip_, value4ip_, 32);
    }
    const char *value4port_ = reader.getEntryValue("port_");
    if (value4port_ == NULL) {
        this->port_ = 0;
    } else {
        ret = TdrParse::parseUInt16(this->port_, value4port_, NULL, 0, NULL,
                                    NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

TdrError::ErrorType AddrInfo::toXmlFile(FILE *fp, TdrXmlFormat format,
                                        unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::toXmlFile(const char *file, TdrXmlFormat format,
                                        unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        FILE *fp = fopen(file, "wb");
        ret = TDR_ERR_OPEN_FILE_WRITE_FAILED;
        if (fp != NULL) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::toXmlBuffer(char *buffer, unsigned int size,
                                          unsigned int *usedSize,
                                          TdrXmlFormat format,
                                          unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == TdrError::TDR_NO_ERROR && usedSize != NULL) {
                    *usedSize = writer.getUsedSize();
                }
            }
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::toXml(TdrXmlWriter &writer, TdrXmlFormat format,
                                    unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openSimple("AddrInfo");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("AddrInfo");
        }
    }
    return ret;
}

TdrError::ErrorType AddrInfo::entryToListXml(TdrXmlWriter &writer,
                                             unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openNode("plugin_id_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->plugin_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("plugin_id_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("ip_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textizeStr(this->ip_, true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("ip_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("port_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("port_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    return ret;
}

TdrError::ErrorType AddrInfo::entryToAttrXml(TdrXmlWriter &writer,
                                             unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openAttr("plugin_id_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->plugin_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("plugin_id_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("ip_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textizeStr(this->ip_, false);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("ip_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("port_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("port_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ---------------------------------------------------------------------------
// CommConfig (sizeof 1226)
// ---------------------------------------------------------------------------

TdrError::ErrorType CommConfig::construct() {
    this->use_ipc_ = 1;
    this->use_socket_ = 0;
    this->server_num_ = 0;
    for (unsigned int i = 0; i < 32; i++) {
        TdrError::ErrorType ret = this->server_addrs_info_[i].construct();
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType CommConfig::fromXmlFile(const char *file,
                                            TdrXmlFormat format,
                                            unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.loadState(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::fromXmlBuffer(const char *buffer,
                                              unsigned int size,
                                              TdrXmlFormat format,
                                              unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::fromXmlString(char *string, TdrXmlFormat format,
                                              unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != NULL) {
        TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::fromXml(TdrXmlReader &reader,
                                        unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("CommConfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("CommConfig");
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::entryFromXml(TdrXmlReader &reader,
                                             unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *value4use_ipc_ = reader.getEntryValue("use_ipc_");
    if (value4use_ipc_ == NULL) {
        this->use_ipc_ = 1;
    } else {
        ret = TdrParse::parseInt32(this->use_ipc_, value4use_ipc_, NULL, 1,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4use_socket_ = reader.getEntryValue("use_socket_");
    if (value4use_socket_ == NULL) {
        this->use_socket_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->use_socket_, value4use_socket_, NULL,
                                   0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4server_num_ = reader.getEntryValue("server_num_");
    if (value4server_num_ == NULL) {
        this->server_num_ = 32;
    } else {
        ret = TdrParse::parseUInt16(this->server_num_, value4server_num_, NULL,
                                    32, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    if (this->server_num_ > 32) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    const char *name4server_addrs_info_ = "server_addrs_info_";
    for (unsigned short i = 0; i < this->server_num_; i++) {
        if (reader.stepIn(name4server_addrs_info_) ==
            TdrXmlReader::WS_NORMAL) {
            ret = this->server_addrs_info_[i].entryFromXml(reader, 1);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
            reader.stepOut(name4server_addrs_info_);
        } else {
            this->server_num_ = i;
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::toXmlFile(FILE *fp, TdrXmlFormat format,
                                          unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::toXmlFile(const char *file,
                                          TdrXmlFormat format,
                                          unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        FILE *fp = fopen(file, "wb");
        ret = TDR_ERR_OPEN_FILE_WRITE_FAILED;
        if (fp != NULL) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::toXmlBuffer(char *buffer, unsigned int size,
                                            unsigned int *usedSize,
                                            TdrXmlFormat format,
                                            unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == TdrError::TDR_NO_ERROR && usedSize != NULL) {
                    *usedSize = writer.getUsedSize();
                }
            }
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::toXml(TdrXmlWriter &writer,
                                      TdrXmlFormat format,
                                      unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openComplex("CommConfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("CommConfig");
        }
    }
    return ret;
}

TdrError::ErrorType CommConfig::entryToListXml(TdrXmlWriter &writer,
                                               unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openNode("use_ipc_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->use_ipc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("use_ipc_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("use_socket_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->use_socket_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("use_socket_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("server_num_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->server_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("server_num_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    if (this->server_num_ > 32) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    const char *name4server_addrs_info_ = "server_addrs_info_";
    for (unsigned short i = 0; i < this->server_num_; i++) {
        ret = writer.openComplex(name4server_addrs_info_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = this->server_addrs_info_[i].entryToListXml(writer, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex(name4server_addrs_info_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

TdrError::ErrorType CommConfig::entryToAttrXml(TdrXmlWriter &writer,
                                               unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openAttr("use_ipc_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->use_ipc_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("use_ipc_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("use_socket_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->use_socket_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("use_socket_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("server_num_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->server_num_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("server_num_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (this->server_num_ > 32) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    const char *name4server_addrs_info_ = "server_addrs_info_";
    for (unsigned short i = 0; i < this->server_num_; i++) {
        ret = writer.openComplex(name4server_addrs_info_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = this->server_addrs_info_[i].entryToAttrXml(writer, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex(name4server_addrs_info_);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

// ---------------------------------------------------------------------------
// LogConfig (sizeof 4)
// ---------------------------------------------------------------------------

TdrError::ErrorType LogConfig::construct() {
    this->log_level_ = 3;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LogConfig::fromXmlFile(const char *file,
                                           TdrXmlFormat format,
                                           unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.loadState(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::fromXmlBuffer(const char *buffer,
                                             unsigned int size,
                                             TdrXmlFormat format,
                                             unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::fromXmlString(char *string, TdrXmlFormat format,
                                             unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != NULL) {
        TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::fromXml(TdrXmlReader &reader,
                                       unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("LogConfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("LogConfig");
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::entryFromXml(TdrXmlReader &reader,
                                            unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *value4log_level_ = reader.getEntryValue("log_level_");
    if (value4log_level_ == NULL) {
        this->log_level_ = 3;
    } else {
        ret = TdrParse::parseInt32(this->log_level_, value4log_level_, NULL, 3,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

TdrError::ErrorType LogConfig::toXmlFile(FILE *fp, TdrXmlFormat format,
                                         unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::toXmlFile(const char *file, TdrXmlFormat format,
                                         unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        FILE *fp = fopen(file, "wb");
        ret = TDR_ERR_OPEN_FILE_WRITE_FAILED;
        if (fp != NULL) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::toXmlBuffer(char *buffer, unsigned int size,
                                           unsigned int *usedSize,
                                           TdrXmlFormat format,
                                           unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == TdrError::TDR_NO_ERROR && usedSize != NULL) {
                    *usedSize = writer.getUsedSize();
                }
            }
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::toXml(TdrXmlWriter &writer,
                                     TdrXmlFormat format,
                                     unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openSimple("LogConfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("LogConfig");
        }
    }
    return ret;
}

TdrError::ErrorType LogConfig::entryToListXml(TdrXmlWriter &writer,
                                              unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openNode("log_level_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->log_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("log_level_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    return ret;
}

TdrError::ErrorType LogConfig::entryToAttrXml(TdrXmlWriter &writer,
                                              unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openAttr("log_level_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->log_level_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("log_level_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ---------------------------------------------------------------------------
// UserControlconfig (sizeof 8)
// ---------------------------------------------------------------------------

TdrError::ErrorType UserControlconfig::construct() {
    this->use_idx_ = 1;
    this->max_client_obj_count_ = 20000;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType UserControlconfig::fromXmlFile(const char *file,
                                                   TdrXmlFormat format,
                                                   unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.loadState(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::fromXmlBuffer(const char *buffer,
                                                     unsigned int size,
                                                     TdrXmlFormat format,
                                                     unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::fromXmlString(char *string,
                                                     TdrXmlFormat format,
                                                     unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != NULL) {
        TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::fromXml(TdrXmlReader &reader,
                                               unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("UserControlconfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("UserControlconfig");
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::entryFromXml(TdrXmlReader &reader,
                                                    unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *value4use_idx_ = reader.getEntryValue("use_idx_");
    if (value4use_idx_ == NULL) {
        this->use_idx_ = 1;
    } else {
        ret = TdrParse::parseInt32(this->use_idx_, value4use_idx_, NULL, 1,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_client_obj_count_ =
        reader.getEntryValue("max_client_obj_count_");
    if (value4max_client_obj_count_ == NULL) {
        this->max_client_obj_count_ = 20000;
    } else {
        ret = TdrParse::parseInt32(this->max_client_obj_count_,
                                   value4max_client_obj_count_, NULL, 20000,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::toXmlFile(FILE *fp, TdrXmlFormat format,
                                                 unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::toXmlFile(const char *file,
                                                 TdrXmlFormat format,
                                                 unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        FILE *fp = fopen(file, "wb");
        ret = TDR_ERR_OPEN_FILE_WRITE_FAILED;
        if (fp != NULL) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::toXmlBuffer(char *buffer,
                                                   unsigned int size,
                                                   unsigned int *usedSize,
                                                   TdrXmlFormat format,
                                                   unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == TdrError::TDR_NO_ERROR && usedSize != NULL) {
                    *usedSize = writer.getUsedSize();
                }
            }
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::toXml(TdrXmlWriter &writer,
                                             TdrXmlFormat format,
                                             unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openSimple("UserControlconfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("UserControlconfig");
        }
    }
    return ret;
}

TdrError::ErrorType UserControlconfig::entryToListXml(
    TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openNode("use_idx_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->use_idx_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("use_idx_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_client_obj_count_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_client_obj_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_client_obj_count_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    return ret;
}

TdrError::ErrorType UserControlconfig::entryToAttrXml(
    TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openAttr("use_idx_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->use_idx_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("use_idx_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_client_obj_count_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_client_obj_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_client_obj_count_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ---------------------------------------------------------------------------
// UicConfig (sizeof 277)
// ---------------------------------------------------------------------------

TdrError::ErrorType UicConfig::construct() {
    this->use_backend_check_ = 1;
    this->user_input_code_ = 0;
    this->master_tss_uic_ip_[0] = 0;
    this->master_tss_uic_port_ = 0;
    this->slave_tss_uic_ip_[0] = 0;
    this->slave_tss_uic_port_ = 0;
    this->max_var_len_queue_len_ = 1228800;
    this->max_fix_len_queue_len_ = 1200;
    this->use_dirty_file_flag_ = 0;
    this->dirty_file_[0] = 0;
    this->bus_id_ = 0;
    this->queue_timeout_ = 300;
    this->connect_timeout_ = 3000;
    this->connect_intval_ = 5;
    this->recv_timeout_ = 0;
    this->send_timeout_ = 50;
    this->max_recv_count_onetime_ = 1000;
    this->max_send_count_onetime_ = 1000;
    this->max_recv_time_gap_onetime_ = 100;
    this->max_send_time_gap_onetime_ = 100;
    this->stop_timeout_ = 5;
    this->tcp_max_recv_buf_len_ = 64000;
    this->tcp_max_send_buf_len_ = 64000;
    this->send_intval_ = 500;
    this->recv_intval_ = 500;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType UicConfig::fromXmlFile(const char *file,
                                           TdrXmlFormat format,
                                           unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.loadState(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::fromXmlBuffer(const char *buffer,
                                             unsigned int size,
                                             TdrXmlFormat format,
                                             unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::fromXmlString(char *string, TdrXmlFormat format,
                                             unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != NULL) {
        TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::fromXml(TdrXmlReader &reader,
                                       unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("UicConfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("UicConfig");
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::entryFromXml(TdrXmlReader &reader,
                                            unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *value4use_backend_check_ =
        reader.getEntryValue("use_backend_check_");
    if (value4use_backend_check_ == NULL) {
        this->use_backend_check_ = 1;
    } else {
        ret = TdrParse::parseInt32(this->use_backend_check_,
                                   value4use_backend_check_, NULL, 1, NULL,
                                   NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4user_input_code_ =
        reader.getEntryValue("user_input_code_");
    if (value4user_input_code_ == NULL) {
        this->user_input_code_ = 0;
    } else {
        ret = TdrParse::parseInt8(this->user_input_code_,
                                  value4user_input_code_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4master_tss_uic_ip_ =
        reader.getEntryValue("master_tss_uic_ip_");
    if (value4master_tss_uic_ip_ == NULL) {
        this->master_tss_uic_ip_[0] = 0;
    } else {
        const size_t length4master_tss_uic_ip_ =
            strlen(value4master_tss_uic_ip_);
        if (length4master_tss_uic_ip_ > 31) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->master_tss_uic_ip_, value4master_tss_uic_ip_, 32);
    }
    const char *value4master_tss_uic_port_ =
        reader.getEntryValue("master_tss_uic_port_");
    if (value4master_tss_uic_port_ == NULL) {
        this->master_tss_uic_port_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->master_tss_uic_port_,
                                   value4master_tss_uic_port_, NULL, 0, NULL,
                                   NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4slave_tss_uic_ip_ =
        reader.getEntryValue("slave_tss_uic_ip_");
    if (value4slave_tss_uic_ip_ == NULL) {
        this->slave_tss_uic_ip_[0] = 0;
    } else {
        const size_t length4slave_tss_uic_ip_ =
            strlen(value4slave_tss_uic_ip_);
        if (length4slave_tss_uic_ip_ > 31) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->slave_tss_uic_ip_, value4slave_tss_uic_ip_, 32);
    }
    const char *value4slave_tss_uic_port_ =
        reader.getEntryValue("slave_tss_uic_port_");
    if (value4slave_tss_uic_port_ == NULL) {
        this->slave_tss_uic_port_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->slave_tss_uic_port_,
                                   value4slave_tss_uic_port_, NULL, 0, NULL,
                                   NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_var_len_queue_len_ =
        reader.getEntryValue("max_var_len_queue_len_");
    if (value4max_var_len_queue_len_ == NULL) {
        this->max_var_len_queue_len_ = 1228800;
    } else {
        ret = TdrParse::parseInt32(this->max_var_len_queue_len_,
                                   value4max_var_len_queue_len_, NULL,
                                   1228800, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_fix_len_queue_len_ =
        reader.getEntryValue("max_fix_len_queue_len_");
    if (value4max_fix_len_queue_len_ == NULL) {
        this->max_fix_len_queue_len_ = 1200;
    } else {
        ret = TdrParse::parseInt32(this->max_fix_len_queue_len_,
                                   value4max_fix_len_queue_len_, NULL, 1200,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4use_dirty_file_flag_ =
        reader.getEntryValue("use_dirty_file_flag_");
    if (value4use_dirty_file_flag_ == NULL) {
        this->use_dirty_file_flag_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->use_dirty_file_flag_,
                                   value4use_dirty_file_flag_, NULL, 0, NULL,
                                   NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4dirty_file_ = reader.getEntryValue("dirty_file_");
    if (value4dirty_file_ == NULL) {
        this->dirty_file_[0] = 0;
    } else {
        const size_t length4dirty_file_ = strlen(value4dirty_file_);
        if (length4dirty_file_ > 127) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->dirty_file_, value4dirty_file_, 128);
    }
    const char *value4bus_id_ = reader.getEntryValue("bus_id_");
    if (value4bus_id_ == NULL) {
        this->bus_id_ = 0;
    } else {
        ret = TdrParse::parseUInt32(this->bus_id_, value4bus_id_, NULL, 0,
                                    NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4queue_timeout_ = reader.getEntryValue("queue_timeout_");
    if (value4queue_timeout_ == NULL) {
        this->queue_timeout_ = 300;
    } else {
        ret = TdrParse::parseInt32(this->queue_timeout_, value4queue_timeout_,
                                   NULL, 300, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4connect_timeout_ =
        reader.getEntryValue("connect_timeout_");
    if (value4connect_timeout_ == NULL) {
        this->connect_timeout_ = 3000;
    } else {
        ret = TdrParse::parseInt32(this->connect_timeout_,
                                   value4connect_timeout_, NULL, 3000, NULL,
                                   NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4connect_intval_ =
        reader.getEntryValue("connect_intval_");
    if (value4connect_intval_ == NULL) {
        this->connect_intval_ = 5;
    } else {
        ret = TdrParse::parseInt32(this->connect_intval_, value4connect_intval_,
                                   NULL, 5, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4recv_timeout_ = reader.getEntryValue("recv_timeout_");
    if (value4recv_timeout_ == NULL) {
        this->recv_timeout_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->recv_timeout_, value4recv_timeout_,
                                   NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4send_timeout_ = reader.getEntryValue("send_timeout_");
    if (value4send_timeout_ == NULL) {
        this->send_timeout_ = 50;
    } else {
        ret = TdrParse::parseInt32(this->send_timeout_, value4send_timeout_,
                                   NULL, 50, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_recv_count_onetime_ =
        reader.getEntryValue("max_recv_count_onetime_");
    if (value4max_recv_count_onetime_ == NULL) {
        this->max_recv_count_onetime_ = 1000;
    } else {
        ret = TdrParse::parseInt32(this->max_recv_count_onetime_,
                                   value4max_recv_count_onetime_, NULL, 1000,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_send_count_onetime_ =
        reader.getEntryValue("max_send_count_onetime_");
    if (value4max_send_count_onetime_ == NULL) {
        this->max_send_count_onetime_ = 1000;
    } else {
        ret = TdrParse::parseInt32(this->max_send_count_onetime_,
                                   value4max_send_count_onetime_, NULL, 1000,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_recv_time_gap_onetime_ =
        reader.getEntryValue("max_recv_time_gap_onetime_");
    if (value4max_recv_time_gap_onetime_ == NULL) {
        this->max_recv_time_gap_onetime_ = 100;
    } else {
        ret = TdrParse::parseInt32(this->max_recv_time_gap_onetime_,
                                   value4max_recv_time_gap_onetime_, NULL, 100,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4max_send_time_gap_onetime_ =
        reader.getEntryValue("max_send_time_gap_onetime_");
    if (value4max_send_time_gap_onetime_ == NULL) {
        this->max_send_time_gap_onetime_ = 100;
    } else {
        ret = TdrParse::parseInt32(this->max_send_time_gap_onetime_,
                                   value4max_send_time_gap_onetime_, NULL, 100,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4stop_timeout_ = reader.getEntryValue("stop_timeout_");
    if (value4stop_timeout_ == NULL) {
        this->stop_timeout_ = 5;
    } else {
        ret = TdrParse::parseInt32(this->stop_timeout_, value4stop_timeout_,
                                   NULL, 5, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4tcp_max_recv_buf_len_ =
        reader.getEntryValue("tcp_max_recv_buf_len_");
    if (value4tcp_max_recv_buf_len_ == NULL) {
        this->tcp_max_recv_buf_len_ = 64000;
    } else {
        ret = TdrParse::parseInt32(this->tcp_max_recv_buf_len_,
                                   value4tcp_max_recv_buf_len_, NULL, 64000,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4tcp_max_send_buf_len_ =
        reader.getEntryValue("tcp_max_send_buf_len_");
    if (value4tcp_max_send_buf_len_ == NULL) {
        this->tcp_max_send_buf_len_ = 64000;
    } else {
        ret = TdrParse::parseInt32(this->tcp_max_send_buf_len_,
                                   value4tcp_max_send_buf_len_, NULL, 64000,
                                   NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4send_intval_ = reader.getEntryValue("send_intval_");
    if (value4send_intval_ == NULL) {
        this->send_intval_ = 500;
    } else {
        ret = TdrParse::parseUInt32(this->send_intval_, value4send_intval_,
                                    NULL, 500, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4recv_intval_ = reader.getEntryValue("recv_intval_");
    if (value4recv_intval_ == NULL) {
        this->recv_intval_ = 500;
    } else {
        ret = TdrParse::parseUInt32(this->recv_intval_, value4recv_intval_,
                                    NULL, 500, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

TdrError::ErrorType UicConfig::toXmlFile(FILE *fp, TdrXmlFormat format,
                                         unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::toXmlFile(const char *file, TdrXmlFormat format,
                                         unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        FILE *fp = fopen(file, "wb");
        ret = TDR_ERR_OPEN_FILE_WRITE_FAILED;
        if (fp != NULL) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::toXmlBuffer(char *buffer, unsigned int size,
                                           unsigned int *usedSize,
                                           TdrXmlFormat format,
                                           unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == TdrError::TDR_NO_ERROR && usedSize != NULL) {
                    *usedSize = writer.getUsedSize();
                }
            }
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::toXml(TdrXmlWriter &writer,
                                     TdrXmlFormat format,
                                     unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openSimple("UicConfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("UicConfig");
        }
    }
    return ret;
}

TdrError::ErrorType UicConfig::entryToListXml(TdrXmlWriter &writer,
                                              unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openNode("use_backend_check_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->use_backend_check_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("use_backend_check_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("user_input_code_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->user_input_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("user_input_code_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("master_tss_uic_ip_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textizeStr(this->master_tss_uic_ip_, true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("master_tss_uic_ip_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("master_tss_uic_port_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->master_tss_uic_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("master_tss_uic_port_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("slave_tss_uic_ip_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textizeStr(this->slave_tss_uic_ip_, true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("slave_tss_uic_ip_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("slave_tss_uic_port_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->slave_tss_uic_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("slave_tss_uic_port_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_var_len_queue_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_var_len_queue_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_var_len_queue_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_fix_len_queue_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_fix_len_queue_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_fix_len_queue_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("use_dirty_file_flag_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->use_dirty_file_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("use_dirty_file_flag_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("dirty_file_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textizeStr(this->dirty_file_, true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("dirty_file_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("bus_id_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%u ", this->bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("bus_id_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("queue_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->queue_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("queue_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("connect_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->connect_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("connect_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("connect_intval_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->connect_intval_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("connect_intval_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("recv_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->recv_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("recv_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("send_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->send_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("send_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_recv_count_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_recv_count_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_recv_count_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_send_count_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_send_count_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_send_count_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_recv_time_gap_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_recv_time_gap_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_recv_time_gap_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("max_send_time_gap_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->max_send_time_gap_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("max_send_time_gap_onetime_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("stop_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->stop_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("stop_timeout_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("tcp_max_recv_buf_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->tcp_max_recv_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("tcp_max_recv_buf_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("tcp_max_send_buf_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%d ", this->tcp_max_send_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("tcp_max_send_buf_len_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("send_intval_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%u ", this->send_intval_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("send_intval_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.openNode("recv_intval_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textize("%u ", this->recv_intval_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("recv_intval_");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    return ret;
}

TdrError::ErrorType UicConfig::entryToAttrXml(TdrXmlWriter &writer,
                                              unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openAttr("use_backend_check_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->use_backend_check_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("use_backend_check_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("user_input_code_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->user_input_code_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("user_input_code_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("master_tss_uic_ip_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textizeStr(this->master_tss_uic_ip_, false);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("master_tss_uic_ip_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("master_tss_uic_port_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->master_tss_uic_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("master_tss_uic_port_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("slave_tss_uic_ip_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textizeStr(this->slave_tss_uic_ip_, false);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("slave_tss_uic_ip_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("slave_tss_uic_port_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->slave_tss_uic_port_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("slave_tss_uic_port_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_var_len_queue_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_var_len_queue_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_var_len_queue_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_fix_len_queue_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_fix_len_queue_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_fix_len_queue_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("use_dirty_file_flag_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->use_dirty_file_flag_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("use_dirty_file_flag_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("dirty_file_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textizeStr(this->dirty_file_, false);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("dirty_file_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("bus_id_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%u", this->bus_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("bus_id_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("queue_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->queue_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("queue_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("connect_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->connect_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("connect_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("connect_intval_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->connect_intval_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("connect_intval_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("recv_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->recv_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("recv_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("send_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->send_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("send_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_recv_count_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_recv_count_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_recv_count_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_send_count_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_send_count_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_send_count_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_recv_time_gap_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_recv_time_gap_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_recv_time_gap_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("max_send_time_gap_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->max_send_time_gap_onetime_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("max_send_time_gap_onetime_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("stop_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->stop_timeout_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("stop_timeout_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("tcp_max_recv_buf_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->tcp_max_recv_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("tcp_max_recv_buf_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("tcp_max_send_buf_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%d", this->tcp_max_send_buf_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("tcp_max_send_buf_len_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("send_intval_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%u", this->send_intval_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("send_intval_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openAttr("recv_intval_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textize("%u", this->recv_intval_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("recv_intval_");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ---------------------------------------------------------------------------
// config (sizeof 1515; comm_config_ 0, log_config_ 1226,
// usercontrol_config_ 1230, uic_config_ 1238)
// ---------------------------------------------------------------------------

TdrError::ErrorType config::construct() {
    TdrError::ErrorType ret = this->comm_config_.construct();
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->log_config_.construct();
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->usercontrol_config_.construct();
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->uic_config_.construct();
    return ret;
}

TdrError::ErrorType config::fromXmlFile(const char *file, TdrXmlFormat format,
                                        unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.loadState(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType config::fromXmlBuffer(const char *buffer,
                                          unsigned int size,
                                          TdrXmlFormat format,
                                          unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType config::fromXmlString(char *string, TdrXmlFormat format,
                                          unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != NULL) {
        TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType config::fromXml(TdrXmlReader &reader,
                                    unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("config") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("config");
        }
    }
    return ret;
}

TdrError::ErrorType config::entryFromXml(TdrXmlReader &reader,
                                         unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *name4comm_config_ = "comm_config_";
    if (reader.stepIn(name4comm_config_) == TdrXmlReader::WS_NORMAL) {
        ret = this->comm_config_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut(name4comm_config_);
    } else {
        ret = this->comm_config_.construct();
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *name4log_config_ = "log_config_";
    if (reader.stepIn(name4log_config_) == TdrXmlReader::WS_NORMAL) {
        ret = this->log_config_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut(name4log_config_);
    } else {
        ret = this->log_config_.construct();
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *name4usercontrol_config_ = "usercontrol_config_";
    if (reader.stepIn(name4usercontrol_config_) == TdrXmlReader::WS_NORMAL) {
        ret = this->usercontrol_config_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut(name4usercontrol_config_);
    } else {
        ret = this->usercontrol_config_.construct();
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    const char *name4uic_config_ = "uic_config_";
    if (reader.stepIn(name4uic_config_) == TdrXmlReader::WS_NORMAL) {
        ret = this->uic_config_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut(name4uic_config_);
    } else {
        ret = this->uic_config_.construct();
        if (ret != TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

TdrError::ErrorType config::toXmlFile(FILE *fp, TdrXmlFormat format,
                                      unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType config::toXmlFile(const char *file, TdrXmlFormat format,
                                      unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        FILE *fp = fopen(file, "wb");
        ret = TDR_ERR_OPEN_FILE_WRITE_FAILED;
        if (fp != NULL) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

TdrError::ErrorType config::toXmlBuffer(char *buffer, unsigned int size,
                                        unsigned int *usedSize,
                                        TdrXmlFormat format,
                                        unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.declare("GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == TdrError::TDR_NO_ERROR && usedSize != NULL) {
                    *usedSize = writer.getUsedSize();
                }
            }
        }
    }
    return ret;
}

TdrError::ErrorType config::toXml(TdrXmlWriter &writer, TdrXmlFormat format,
                                  unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openComplex("config");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("config");
        }
    }
    return ret;
}

TdrError::ErrorType config::entryToListXml(TdrXmlWriter &writer,
                                           unsigned int cutVer) const {
    TdrError::ErrorType ret;
    const char *name4comm_config_ = "comm_config_";
    ret = writer.openComplex(name4comm_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->comm_config_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4comm_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const char *name4log_config_ = "log_config_";
    ret = writer.openComplex(name4log_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->log_config_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4log_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const char *name4usercontrol_config_ = "usercontrol_config_";
    ret = writer.openComplex(name4usercontrol_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->usercontrol_config_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4usercontrol_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const char *name4uic_config_ = "uic_config_";
    ret = writer.openComplex(name4uic_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->uic_config_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4uic_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

TdrError::ErrorType config::entryToAttrXml(TdrXmlWriter &writer,
                                           unsigned int cutVer) const {
    TdrError::ErrorType ret;
    const char *name4comm_config_ = "comm_config_";
    ret = writer.openComplex(name4comm_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->comm_config_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4comm_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const char *name4log_config_ = "log_config_";
    ret = writer.openComplex(name4log_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->log_config_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4log_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const char *name4usercontrol_config_ = "usercontrol_config_";
    ret = writer.openComplex(name4usercontrol_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->usercontrol_config_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4usercontrol_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    const char *name4uic_config_ = "uic_config_";
    ret = writer.openComplex(name4uic_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = this->uic_config_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex(name4uic_config_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

} // namespace tss_sdk_conf_desc
