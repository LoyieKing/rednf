// Reconstructed from gunnersvr disassembly (TDR-generated config structures).

#include "src/protocol/common/comm_conf_gunnersvr.h"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

namespace conf_gunnersvr {

tsf4g_tdr::TdrError::ErrorType IpAddr::construct() {
    memset(ip_, 0, sizeof(ip_));
    strcpy(ip_, "0.0.0.0");
    port_ = 0;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType IpAddr::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType IpAddr::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType IpAddr::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType IpAddr::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("IpAddr") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("IpAddr");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType IpAddr::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4ip_ = reader.getEntryValue("ip_");
    if (value4ip_ != 0) {
        const size_t length4ip_ = strlen(value4ip_);
        if (length4ip_ > 31) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        strncpy(ip_, value4ip_, 32);
    } else {
        memset(ip_, 0, sizeof(ip_));
        strcpy(ip_, "0.0.0.0");
    }
    const char *value4port_ = reader.getEntryValue("port_");
    if (value4port_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(port_, value4port_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        port_ = 0;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType IpAddr::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType IpAddr::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType IpAddr::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType IpAddr::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openSimple("IpAddr");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeSimple("IpAddr");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType IpAddr::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "ip_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "ip_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textizeStr(ip_, true);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "ip_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "port_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "port_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", port_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "port_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType IpAddr::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "ip_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textizeStr(ip_, false);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "port_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", port_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType MonitorInfo::construct() {
    tsf4g_tdr::TdrError::ErrorType ret = net_addr_.construct();
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    app_id_ = 0;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorInfo::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType MonitorInfo::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType MonitorInfo::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType MonitorInfo::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("MonitorInfo") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("MonitorInfo");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType MonitorInfo::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (reader.stepIn("net_addr_") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = net_addr_.entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("net_addr_");
    } else {
        ret = net_addr_.construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4app_id_ = reader.getEntryValue("app_id_");
    if (value4app_id_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(app_id_, value4app_id_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        app_id_ = 0;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType MonitorInfo::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType MonitorInfo::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType MonitorInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType MonitorInfo::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openComplex("MonitorInfo");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("MonitorInfo");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType MonitorInfo::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer.openComplex("net_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = net_addr_.entryToListXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("net_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "app_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "app_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", app_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "app_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType MonitorInfo::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "app_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", app_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.openComplex("net_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = net_addr_.entryToAttrXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("net_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

tsf4g_tdr::TdrError::ErrorType config::construct() {
    log_priority_ = 3;
    tsf4g_tdr::TdrError::ErrorType ret = default_monitor_addr_.construct();
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    monitor_info_count_ = 0;
    for (uint32_t monitor_info__i = 0; monitor_info__i < 64; ++monitor_info__i) {
        ret = monitor_info_[monitor_info__i].construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType config::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType config::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType config::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType config::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("config") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("config");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType config::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4log_priority_ = reader.getEntryValue("log_priority_");
    if (value4log_priority_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt8(log_priority_, value4log_priority_, 0, 3, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        log_priority_ = 3;
    }
    if (reader.stepIn("default_monitor_addr_") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = default_monitor_addr_.entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("default_monitor_addr_");
    } else {
        ret = default_monitor_addr_.construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    const char *value4monitor_info_count_ = reader.getEntryValue("monitor_info_count_");
    if (value4monitor_info_count_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(monitor_info_count_, value4monitor_info_count_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        if (monitor_info_count_ > 64) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        if (monitor_info_count_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    } else {
        monitor_info_count_ = 64;
    }
    for (uint32_t monitor_info__i = 0; monitor_info__i < monitor_info_count_; ++monitor_info__i) {
        if (reader.stepIn("monitor_info_") != tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
            monitor_info_count_ = monitor_info__i;
            return tsf4g_tdr::TdrError::TDR_NO_ERROR;
        }
        ret = monitor_info_[monitor_info__i].entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("monitor_info_");
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType config::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType config::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType config::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType config::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openComplex("config");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("config");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType config::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "log_priority_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "log_priority_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("0x%02x ", log_priority_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "log_priority_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.openComplex("default_monitor_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = default_monitor_addr_.entryToListXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("default_monitor_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_info_count_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_info_count_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", monitor_info_count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "monitor_info_count_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (monitor_info_count_ > 64) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (monitor_info_count_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    for (uint32_t monitor_info__i = 0; monitor_info__i < monitor_info_count_; ++monitor_info__i) {
        ret = writer.openComplex("monitor_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = monitor_info_[monitor_info__i].entryToListXml(writer, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex("monitor_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType config::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "log_priority_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("0x%02x", log_priority_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "monitor_info_count_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", monitor_info_count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.openComplex("default_monitor_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = default_monitor_addr_.entryToAttrXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("default_monitor_addr_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (monitor_info_count_ > 64) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (monitor_info_count_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    for (uint32_t monitor_info__i = 0; monitor_info__i < monitor_info_count_; ++monitor_info__i) {
        ret = writer.openComplex("monitor_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = monitor_info_[monitor_info__i].entryToAttrXml(writer, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex("monitor_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

} // namespace conf_gunnersvr
