// Restored from DWARF + disassembly (gunnersvr oracle), semantics verified.

#include "src/protocol/common/comm_conf_svcid.h"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

namespace conf_svcid {

tsf4g_tdr::TdrError::ErrorType ServiceInfo::construct() {
    svr_type_ = 0;
    svr_id_ = 0;
    memset(svr_ip_, 0, sizeof(svr_ip_));
    strcpy(svr_ip_, "0.0.0.0");
    svr_port_ = 0;
    svr_idc_ = 0;
    svr_info_[0] = 0;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != 0) {
        tsf4g_tdr::TdrXmlFile xmlFile;
        ret = xmlFile.load(file);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != 0) {
        tsf4g_tdr::TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != 0) {
        tsf4g_tdr::TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("ServiceInfo") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("ServiceInfo");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4svr_type_ = reader.getEntryValue("svr_type_");
    if (value4svr_type_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(svr_type_, value4svr_type_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        svr_type_ = 0;
    }
    const char *value4svr_id_ = reader.getEntryValue("svr_id_");
    if (value4svr_id_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(svr_id_, value4svr_id_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        svr_id_ = 0;
    }
    const char *value4svr_ip_ = reader.getEntryValue("svr_ip_");
    if (value4svr_ip_ != 0) {
        if (strlen(value4svr_ip_) > 15) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        strncpy(svr_ip_, value4svr_ip_, 16);
    } else {
        memset(svr_ip_, 0, sizeof(svr_ip_));
        strcpy(svr_ip_, "0.0.0.0");
    }
    const char *value4svr_port_ = reader.getEntryValue("svr_port_");
    if (value4svr_port_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(svr_port_, value4svr_port_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        svr_port_ = 0;
    }
    const char *value4svr_idc_ = reader.getEntryValue("svr_idc_");
    if (value4svr_idc_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(svr_idc_, value4svr_idc_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        svr_idc_ = 0;
    }
    const char *value4svr_info_ = reader.getEntryValue("svr_info_");
    if (value4svr_info_ != 0) {
        if (strlen(value4svr_info_) > 31) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        strncpy(svr_info_, value4svr_info_, 32);
    } else {
        svr_info_[0] = 0;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != 0) {
        FILE *fp = fopen(file, "wb");
        if (fp != 0) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != 0) {
        tsf4g_tdr::TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != 0) {
        tsf4g_tdr::TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR && usedSize != 0) *usedSize = writer.getUsedSize();
            }
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openSimple("ServiceInfo");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeSimple("ServiceInfo");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", svr_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "svr_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", svr_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "svr_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_ip_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_ip_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textizeStr(svr_ip_, true);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "svr_ip_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_port_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_port_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", svr_port_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "svr_port_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_idc_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_idc_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", svr_idc_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "svr_idc_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textizeStr(svr_info_, true);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "svr_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType ServiceInfo::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "svr_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", svr_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "svr_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", svr_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "svr_ip_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textizeStr(svr_ip_, false);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "svr_port_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", svr_port_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "svr_idc_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", svr_idc_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "svr_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textizeStr(svr_info_, false);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::construct() {
    service_num_ = 0;
    for (uint32_t i = 0; i < 2048; ++i) {
        tsf4g_tdr::TdrError::ErrorType ret = service_info_[i].construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != 0) {
        tsf4g_tdr::TdrXmlFile xmlFile;
        ret = xmlFile.load(file);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.content(), format, cutVer);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != 0) {
        tsf4g_tdr::TdrXmlReader reader(buffer, size, format);
        ret = reader.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (string != 0) {
        tsf4g_tdr::TdrXmlReader reader(string, format);
        ret = reader.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("svcid_config") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("svcid_config");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4service_num_ = reader.getEntryValue("service_num_");
    if (value4service_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(service_num_, value4service_num_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        if (service_num_ > 2048) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        if (service_num_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    } else {
        service_num_ = 2048;
    }
    for (uint32_t i = 0; i < service_num_; ++i) {
        if (reader.stepIn("service_info_") != tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
            service_num_ = i;
            return tsf4g_tdr::TdrError::TDR_NO_ERROR;
        }
        ret = service_info_[i].entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("service_info_");
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != 0) {
        FILE *fp = fopen(file, "wb");
        if (fp != 0) {
            ret = toXmlFile(fp, format, cutVer);
            fclose(fp);
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != 0) {
        tsf4g_tdr::TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != 0) {
        tsf4g_tdr::TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
                if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR && usedSize != 0) *usedSize = writer.getUsedSize();
            }
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openComplex("svcid_config");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("svcid_config");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "service_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._isPartial = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "service_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", service_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "service_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (service_num_ > 2048) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (service_num_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    for (uint32_t i = 0; i < service_num_; ++i) {
        ret = writer.openComplex("service_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = service_info_[i].entryToListXml(writer, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex("service_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType svcid_config::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "service_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", service_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (service_num_ > 2048) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (service_num_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    for (uint32_t i = 0; i < service_num_; ++i) {
        ret = writer.openComplex("service_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = service_info_[i].entryToAttrXml(writer, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex("service_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

} // namespace conf_svcid
