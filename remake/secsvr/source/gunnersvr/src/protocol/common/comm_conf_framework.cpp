// Reconstructed from gunnersvr disassembly (TDR-generated config structures).

#include "src/protocol/common/comm_conf_framework.h"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

namespace conf_framework {

tsf4g_tdr::TdrError::ErrorType LogInfo::construct() {
    log_output_ = 1;
    log_div_type_ = 205;
    max_log_file_num_ = 2;
    max_log_file_size_ = 0x7a120000;
    bill_output_ = 3;
    bill_div_type_ = 205;
    max_bill_file_num_ = 1;
    max_bill_file_size_ = 0x7a120000;
    monitor_uin_count_ = 0;
    memset(monitor_uin_list_, 0, sizeof(monitor_uin_list_));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType LogInfo::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType LogInfo::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType LogInfo::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType LogInfo::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("LogInfo") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("LogInfo");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType LogInfo::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4log_output_ = reader.getEntryValue("log_output_");
    if (value4log_output_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(log_output_, value4log_output_, 0, 1, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        log_output_ = 1;
    }
    const char *value4log_div_type_ = reader.getEntryValue("log_div_type_");
    if (value4log_div_type_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(log_div_type_, value4log_div_type_, 0, 205, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        log_div_type_ = 205;
    }
    const char *value4max_log_file_num_ = reader.getEntryValue("max_log_file_num_");
    if (value4max_log_file_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(max_log_file_num_, value4max_log_file_num_, 0, 2, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        max_log_file_num_ = 2;
    }
    const char *value4max_log_file_size_ = reader.getEntryValue("max_log_file_size_");
    if (value4max_log_file_size_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(max_log_file_size_, value4max_log_file_size_, 0, 0x7a120000, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        max_log_file_size_ = 0x7a120000;
    }
    const char *value4bill_output_ = reader.getEntryValue("bill_output_");
    if (value4bill_output_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(bill_output_, value4bill_output_, 0, 3, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        bill_output_ = 3;
    }
    const char *value4bill_div_type_ = reader.getEntryValue("bill_div_type_");
    if (value4bill_div_type_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(bill_div_type_, value4bill_div_type_, 0, 205, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        bill_div_type_ = 205;
    }
    const char *value4max_bill_file_num_ = reader.getEntryValue("max_bill_file_num_");
    if (value4max_bill_file_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(max_bill_file_num_, value4max_bill_file_num_, 0, 1, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        max_bill_file_num_ = 1;
    }
    const char *value4max_bill_file_size_ = reader.getEntryValue("max_bill_file_size_");
    if (value4max_bill_file_size_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(max_bill_file_size_, value4max_bill_file_size_, 0, 0x7a120000, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        max_bill_file_size_ = 0x7a120000;
    }
    const char *value4monitor_uin_count_ = reader.getEntryValue("monitor_uin_count_");
    if (value4monitor_uin_count_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(monitor_uin_count_, value4monitor_uin_count_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        if (monitor_uin_count_ > 100) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    } else {
        monitor_uin_count_ = 100;
    }
    const char *value4monitor_uin_list_ = reader.getNodeValue("monitor_uin_list_");
    if (value4monitor_uin_list_ != 0) {
        unsigned int tempCount4monitor_uin_list_ = 0;
        ret = tsf4g_tdr::TdrParse::parseUInt32(monitor_uin_list_, monitor_uin_count_, value4monitor_uin_list_, &tempCount4monitor_uin_list_, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        monitor_uin_count_ = tempCount4monitor_uin_list_;
    } else {
        monitor_uin_count_ = 0;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType LogInfo::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType LogInfo::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType LogInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType LogInfo::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openComplex("LogInfo");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("LogInfo");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType LogInfo::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "log_output_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "log_output_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", log_output_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "log_output_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "log_div_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "log_div_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", log_div_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "log_div_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_log_file_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_log_file_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", max_log_file_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "max_log_file_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_log_file_size_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_log_file_size_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", max_log_file_size_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "max_log_file_size_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "bill_output_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "bill_output_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", bill_output_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "bill_output_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "bill_div_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "bill_div_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", bill_div_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "bill_div_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_bill_file_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_bill_file_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", max_bill_file_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "max_bill_file_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_bill_file_size_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_bill_file_size_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", max_bill_file_size_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "max_bill_file_size_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_uin_count_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_uin_count_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", monitor_uin_count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "monitor_uin_count_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (monitor_uin_count_ > 100) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_uin_list_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_uin_list_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    for (uint32_t monitor_uin_list__i = 0; monitor_uin_list__i < monitor_uin_count_; ++monitor_uin_list__i) {
        ret = writer.textize("%u ", monitor_uin_list_[monitor_uin_list__i]);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    ret = writer._out.textize("</%s>\n", "monitor_uin_list_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType LogInfo::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "log_output_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", log_output_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "log_div_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", log_div_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "max_log_file_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", max_log_file_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "max_log_file_size_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", max_log_file_size_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "bill_output_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", bill_output_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "bill_div_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", bill_div_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "max_bill_file_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", max_bill_file_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "max_bill_file_size_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", max_bill_file_size_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "monitor_uin_count_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", monitor_uin_count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (monitor_uin_count_ > 100) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_uin_list_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_uin_list_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    for (uint32_t monitor_uin_list__i = 0; monitor_uin_list__i < monitor_uin_count_; ++monitor_uin_list__i) {
        ret = writer.textize("%u ", monitor_uin_list_[monitor_uin_list__i]);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    ret = writer._out.textize("</%s>\n", "monitor_uin_list_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TaskInfo::construct() {
    task_thread_num_ = 100;
    task_thread_stack_size_ = 0x200000;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType TaskInfo::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType TaskInfo::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType TaskInfo::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType TaskInfo::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("TaskInfo") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("TaskInfo");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TaskInfo::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4task_thread_num_ = reader.getEntryValue("task_thread_num_");
    if (value4task_thread_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(task_thread_num_, value4task_thread_num_, 0, 100, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        task_thread_num_ = 100;
    }
    const char *value4task_thread_stack_size_ = reader.getEntryValue("task_thread_stack_size_");
    if (value4task_thread_stack_size_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(task_thread_stack_size_, value4task_thread_stack_size_, 0, 0x200000, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        task_thread_stack_size_ = 0x200000;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TaskInfo::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType TaskInfo::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType TaskInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType TaskInfo::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openSimple("TaskInfo");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeSimple("TaskInfo");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TaskInfo::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "task_thread_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "task_thread_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", task_thread_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "task_thread_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "task_thread_stack_size_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "task_thread_stack_size_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", task_thread_stack_size_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "task_thread_stack_size_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TaskInfo::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "task_thread_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", task_thread_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "task_thread_stack_size_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", task_thread_stack_size_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TransInfo::construct() {
    trans_num_ = 0x100000;
    trans_cmd_num_ = 0x800;
    func_cmd_num_ = 0x800;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType TransInfo::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType TransInfo::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType TransInfo::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType TransInfo::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("TransInfo") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("TransInfo");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TransInfo::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4trans_num_ = reader.getEntryValue("trans_num_");
    if (value4trans_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(trans_num_, value4trans_num_, 0, 0x100000, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        trans_num_ = 0x100000;
    }
    const char *value4trans_cmd_num_ = reader.getEntryValue("trans_cmd_num_");
    if (value4trans_cmd_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(trans_cmd_num_, value4trans_cmd_num_, 0, 0x800, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        trans_cmd_num_ = 0x800;
    }
    const char *value4func_cmd_num_ = reader.getEntryValue("func_cmd_num_");
    if (value4func_cmd_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(func_cmd_num_, value4func_cmd_num_, 0, 0x800, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        func_cmd_num_ = 0x800;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TransInfo::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType TransInfo::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType TransInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType TransInfo::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openSimple("TransInfo");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeSimple("TransInfo");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TransInfo::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "trans_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "trans_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", trans_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "trans_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "trans_cmd_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "trans_cmd_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", trans_cmd_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "trans_cmd_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "func_cmd_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "func_cmd_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", func_cmd_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "func_cmd_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType TransInfo::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "trans_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", trans_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "trans_cmd_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", trans_cmd_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "func_cmd_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", func_cmd_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType framework_config::construct() {
    tsf4g_tdr::TdrError::ErrorType ret = trans_info_.construct();
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = task_info_.construct();
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = log_info_.construct();
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType framework_config::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType framework_config::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType framework_config::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType framework_config::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("framework_config") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("framework_config");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType framework_config::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (reader.stepIn("trans_info_") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = trans_info_.entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("trans_info_");
    } else {
        ret = trans_info_.construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    if (reader.stepIn("task_info_") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = task_info_.entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("task_info_");
    } else {
        ret = task_info_.construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    if (reader.stepIn("log_info_") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = log_info_.entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("log_info_");
    } else {
        ret = log_info_.construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType framework_config::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType framework_config::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType framework_config::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType framework_config::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openComplex("framework_config");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("framework_config");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType framework_config::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer.openComplex("trans_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = trans_info_.entryToListXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("trans_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openComplex("task_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = task_info_.entryToListXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("task_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openComplex("log_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = log_info_.entryToListXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("log_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

tsf4g_tdr::TdrError::ErrorType framework_config::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer.openComplex("trans_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = trans_info_.entryToAttrXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("trans_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openComplex("task_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = task_info_.entryToAttrXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("task_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.openComplex("log_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = log_info_.entryToAttrXml(writer, 1);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("log_info_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

} // namespace conf_framework
