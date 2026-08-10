// Reconstructed from gunnersvr disassembly (TDR-generated config structures).

#include "src/protocol/common/comm_conf_cfgsdk.h"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

namespace conf_cfgsdk {

tsf4g_tdr::TdrError::ErrorType DeployInfo::construct() {
    game_id_ = 0;
    idc_no_ = 0;
    world_ = 0;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType DeployInfo::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType DeployInfo::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType DeployInfo::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DeployInfo") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("DeployInfo");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4game_id_ = reader.getEntryValue("game_id_");
    if (value4game_id_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(game_id_, value4game_id_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        game_id_ = 0;
    }
    const char *value4idc_no_ = reader.getEntryValue("idc_no_");
    if (value4idc_no_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(idc_no_, value4idc_no_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        idc_no_ = 0;
    }
    const char *value4world_ = reader.getEntryValue("world_");
    if (value4world_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(world_, value4world_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        world_ = 0;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType DeployInfo::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType DeployInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType DeployInfo::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openSimple("DeployInfo");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeSimple("DeployInfo");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "game_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "game_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", game_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "game_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "idc_no_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "idc_no_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", idc_no_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "idc_no_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "world_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "world_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", world_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "world_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType DeployInfo::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "game_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", game_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "idc_no_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", idc_no_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "world_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", world_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::construct() {
    slave_cfgsvr_type_ = 0;
    slave_cfgsvr_id_ = 0;
    memset(slave_cfgsvr_ip_, 0, sizeof(slave_cfgsvr_ip_));
    strcpy(slave_cfgsvr_ip_, "0.0.0.0");
    slave_cfgsvr_port_ = 0;
    game_id_ = 0;
    idc_no_ = 0;
    world_ = 0;
    deploy_info_num_ = 0;
    for (uint32_t deploy_info__i = 0; deploy_info__i < 10; ++deploy_info__i) {
        tsf4g_tdr::TdrError::ErrorType ret = deploy_info_[deploy_info__i].construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::fromXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::fromXmlBuffer(const char *buffer, unsigned int size, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::fromXmlString(char *string, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::fromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("cfgsdk_config") == tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            reader.stepOut("cfgsdk_config");
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::entryFromXml(tsf4g_tdr::TdrXmlReader &reader, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    const char *value4slave_cfgsvr_type_ = reader.getEntryValue("slave_cfgsvr_type_");
    if (value4slave_cfgsvr_type_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(slave_cfgsvr_type_, value4slave_cfgsvr_type_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        slave_cfgsvr_type_ = 0;
    }
    const char *value4slave_cfgsvr_id_ = reader.getEntryValue("slave_cfgsvr_id_");
    if (value4slave_cfgsvr_id_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(slave_cfgsvr_id_, value4slave_cfgsvr_id_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        slave_cfgsvr_id_ = 0;
    }
    const char *value4slave_cfgsvr_ip_ = reader.getEntryValue("slave_cfgsvr_ip_");
    if (value4slave_cfgsvr_ip_ != 0) {
        const size_t length4slave_cfgsvr_ip_ = strlen(value4slave_cfgsvr_ip_);
        if (length4slave_cfgsvr_ip_ > 31) return tsf4g_tdr::TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        strncpy(slave_cfgsvr_ip_, value4slave_cfgsvr_ip_, 32);
    } else {
        memset(slave_cfgsvr_ip_, 0, sizeof(slave_cfgsvr_ip_));
        strcpy(slave_cfgsvr_ip_, "0.0.0.0");
    }
    const char *value4slave_cfgsvr_port_ = reader.getEntryValue("slave_cfgsvr_port_");
    if (value4slave_cfgsvr_port_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(slave_cfgsvr_port_, value4slave_cfgsvr_port_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        slave_cfgsvr_port_ = 0;
    }
    const char *value4game_id_ = reader.getEntryValue("game_id_");
    if (value4game_id_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(game_id_, value4game_id_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        game_id_ = 0;
    }
    const char *value4idc_no_ = reader.getEntryValue("idc_no_");
    if (value4idc_no_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(idc_no_, value4idc_no_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        idc_no_ = 0;
    }
    const char *value4world_ = reader.getEntryValue("world_");
    if (value4world_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt16(world_, value4world_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    } else {
        world_ = 0;
    }
    const char *value4deploy_info_num_ = reader.getEntryValue("deploy_info_num_");
    if (value4deploy_info_num_ != 0) {
        ret = tsf4g_tdr::TdrParse::parseUInt32(deploy_info_num_, value4deploy_info_num_, 0, 0, 0, 0);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        if (deploy_info_num_ > 10) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        if (deploy_info_num_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    } else {
        deploy_info_num_ = 10;
    }
    for (uint32_t deploy_info__i = 0; deploy_info__i < deploy_info_num_; ++deploy_info__i) {
        if (reader.stepIn("deploy_info_") != tsf4g_tdr::TdrXmlReader::WS_NORMAL) {
            deploy_info_num_ = deploy_info__i;
            return tsf4g_tdr::TdrError::TDR_NO_ERROR;
        }
        ret = deploy_info_[deploy_info__i].entryFromXml(reader, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        reader.stepOut("deploy_info_");
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::toXmlFile(const char *file, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::toXmlFile(FILE *fp, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::toXml(tsf4g_tdr::TdrXmlWriter &writer, tsf4g_tdr::TdrXmlFormat format, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = writer.openComplex("cfgsdk_config");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    if (format == tsf4g_tdr::ATTR_ENTRY) {
        ret = entryToAttrXml(writer, cutVer);
    } else {
        ret = entryToListXml(writer, cutVer);
    }
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeComplex("cfgsdk_config");
    return ret;
}

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::entryToListXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "slave_cfgsvr_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "slave_cfgsvr_type_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", slave_cfgsvr_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "slave_cfgsvr_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "slave_cfgsvr_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "slave_cfgsvr_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", slave_cfgsvr_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "slave_cfgsvr_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "slave_cfgsvr_ip_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "slave_cfgsvr_ip_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textizeStr(slave_cfgsvr_ip_, true);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "slave_cfgsvr_ip_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "slave_cfgsvr_port_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "slave_cfgsvr_port_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", slave_cfgsvr_port_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "slave_cfgsvr_port_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "game_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "game_id_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", game_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "game_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "idc_no_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "idc_no_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", idc_no_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "idc_no_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "world_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "world_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%d ", world_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "world_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "deploy_info_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
        writer._indentPending = false;
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "deploy_info_num_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    }
    ret = writer.textize("%u ", deploy_info_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("</%s>\n", "deploy_info_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (deploy_info_num_ > 10) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (deploy_info_num_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    for (uint32_t deploy_info__i = 0; deploy_info__i < deploy_info_num_; ++deploy_info__i) {
        ret = writer.openComplex("deploy_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = deploy_info_[deploy_info__i].entryToListXml(writer, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex("deploy_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType cfgsdk_config::entryToAttrXml(tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret = tsf4g_tdr::TdrError::TDR_NO_ERROR;
    ret = writer._out.textize(" %s=\"", "slave_cfgsvr_type_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", slave_cfgsvr_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "slave_cfgsvr_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", slave_cfgsvr_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "slave_cfgsvr_ip_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textizeStr(slave_cfgsvr_ip_, false);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "slave_cfgsvr_port_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", slave_cfgsvr_port_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "game_id_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", game_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "idc_no_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", idc_no_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "world_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%d", world_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer._out.textize(" %s=\"", "deploy_info_num_");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    ret = writer.textize("%u", deploy_info_num_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    ret = writer._out.textize("\"");
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) { writer._state = ret; return ret; }
    if (deploy_info_num_ > 10) return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    if (deploy_info_num_ == 0) return tsf4g_tdr::TdrError::TDR_NO_ERROR;
    for (uint32_t deploy_info__i = 0; deploy_info__i < deploy_info_num_; ++deploy_info__i) {
        ret = writer.openComplex("deploy_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = deploy_info_[deploy_info__i].entryToAttrXml(writer, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
        ret = writer.closeComplex("deploy_info_");
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) return ret;
    }
    return ret;
}

} // namespace conf_cfgsdk
