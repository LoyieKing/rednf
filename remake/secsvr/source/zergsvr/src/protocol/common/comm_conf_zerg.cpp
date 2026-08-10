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


TdrError::ErrorType SvrInfo::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SvrInfo::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType SvrInfo::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType SvrInfo::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType SvrInfo::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("SvrInfo") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("SvrInfo");
        }
    }
    return ret;
}

TdrError::ErrorType SvrInfo::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4svr_type;
    const char *value4svr_id;
    const char *value4use_encrypt;

    value4svr_type = reader.getEntryValue("svr_type");
    if (value4svr_type == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->svr_type = 0;
    } else {
        ret = TdrParse::parseUInt16(this->svr_type, value4svr_type, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4svr_id = reader.getEntryValue("svr_id");
    if (value4svr_id == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->svr_id = 0;
    } else {
        ret = TdrParse::parseUInt32(this->svr_id, value4svr_id, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4use_encrypt = reader.getEntryValue("use_encrypt");
    if (value4use_encrypt == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->use_encrypt = 0;
    } else {
        ret = TdrParse::parseUInt8(this->use_encrypt, value4use_encrypt, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType SvrInfo::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType SvrInfo::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType SvrInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType SvrInfo::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("SvrInfo");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("SvrInfo");
        }
    }
    return ret;
}

TdrError::ErrorType SvrInfo::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_type");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_type");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->svr_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "svr_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_id");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_id");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->svr_id);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "svr_id");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "use_encrypt");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "use_encrypt");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->use_encrypt);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "use_encrypt");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType SvrInfo::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "svr_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->svr_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "svr_id");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->svr_id);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "use_encrypt");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->use_encrypt);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType SelfCfg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SelfCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType SelfCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType SelfCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType SelfCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("SelfCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("SelfCfg");
        }
    }
    return ret;
}

TdrError::ErrorType SelfCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4slave_svr_count;
    uint8_t slave_svrs_i;

    if (reader.stepIn("self_svr_info") != TdrXmlReader::WS_NORMAL) {
        ret = this->self_svr_info.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->self_svr_info.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("self_svr_info");
    }
    value4slave_svr_count = reader.getEntryValue("slave_svr_count");
    if (value4slave_svr_count == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->slave_svr_count = 3;
    } else {
        ret = TdrParse::parseUInt8(this->slave_svr_count, value4slave_svr_count, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (this->slave_svr_count > 3) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->slave_svr_count == 0) {
        return TdrError::TDR_NO_ERROR;
    }
    for (slave_svrs_i = 0; slave_svrs_i < this->slave_svr_count; slave_svrs_i++) {
        if (reader.stepIn("slave_svrs") != TdrXmlReader::WS_NORMAL) {
            this->slave_svr_count = slave_svrs_i;
            break;
        }
        ret = this->slave_svrs[slave_svrs_i].entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("slave_svrs");
    }
    return ret;
}

TdrError::ErrorType SelfCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType SelfCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType SelfCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType SelfCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("SelfCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("SelfCfg");
        }
    }
    return ret;
}

TdrError::ErrorType SelfCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint8_t slave_svrs_i;
    ret = writer.openComplex("self_svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->self_svr_info.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("self_svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "slave_svr_count");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "slave_svr_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->slave_svr_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "slave_svr_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->slave_svr_count > 3) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->slave_svr_count == 0) {
        return TdrError::TDR_NO_ERROR;
    }
    for (slave_svrs_i = 0; slave_svrs_i < this->slave_svr_count; slave_svrs_i++) {
        ret = writer.openComplex("slave_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = this->slave_svrs[slave_svrs_i].entryToListXml(writer, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = writer.closeComplex("slave_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType SelfCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint8_t slave_svrs_i;
    ret = writer._out.textize(" %s=\"", "slave_svr_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->slave_svr_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("self_svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->self_svr_info.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("self_svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (this->slave_svr_count > 3) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->slave_svr_count == 0) {
        return TdrError::TDR_NO_ERROR;
    }
    for (slave_svrs_i = 0; slave_svrs_i < this->slave_svr_count; slave_svrs_i++) {
        ret = writer.openComplex("slave_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = this->slave_svrs[slave_svrs_i].entryToAttrXml(writer, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = writer.closeComplex("slave_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType LogCfg::construct() {
    memset(this->log_level, 0, sizeof(this->log_level));
    strcpy(this->log_level, "debug");
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType LogCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType LogCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType LogCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType LogCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("LogCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("LogCfg");
        }
    }
    return ret;
}

TdrError::ErrorType LogCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4log_level;

    value4log_level = reader.getEntryValue("log_level");
    if (value4log_level == NULL) {
        memset(this->log_level, 0, 32);
        strcpy(this->log_level, "debug");
    } else {
        const size_t length4log_level = strlen(value4log_level);
        if (length4log_level > 31) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->log_level, value4log_level, 32);
    }
    return ret;
}

TdrError::ErrorType LogCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType LogCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType LogCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType LogCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("LogCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("LogCfg");
        }
    }
    return ret;
}

TdrError::ErrorType LogCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "log_level");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "log_level");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->log_level, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "log_level");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType LogCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "log_level");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->log_level, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CommCfg::construct() {
    this->max_frame_len = 0xfc00;
    this->accept_send_buf_size = 0x20;
    this->connect_send_deque_size = 0x100;
    this->connect_timeout = 0xf;
    this->recv_timeout = 0;
    this->is_proxy = 0;
    this->max_accept_svr = 0x400;
    this->is_lock_pipe = 0;
    this->retry_error = 3;
    this->recv_pipe_len = 0x3200000;
    this->send_pipe_len = 0x3200000;
    this->error_pipe_len = 0x3200000;
    this->check_pthread = 1;
    this->insurance = 1;
    this->opt_key1[0] = 0;
    this->opt_key2[0] = 0;
    memset(this->get_svr_info_type, 0, sizeof(this->get_svr_info_type));
    strcpy(this->get_svr_info_type, "cfgfile");
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType CommCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CommCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CommCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CommCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("CommCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("CommCfg");
        }
    }
    return ret;
}

TdrError::ErrorType CommCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4max_frame_len;
    const char *value4accept_send_buf_size;
    const char *value4connect_send_deque_size;
    const char *value4connect_timeout;
    const char *value4recv_timeout;
    const char *value4is_proxy;
    const char *value4max_accept_svr;
    const char *value4is_lock_pipe;
    const char *value4retry_error;
    const char *value4recv_pipe_len;
    const char *value4send_pipe_len;
    const char *value4error_pipe_len;
    const char *value4check_pthread;
    const char *value4insurance;
    const char *value4opt_key1;
    const char *value4opt_key2;
    const char *value4get_svr_info_type;

    value4max_frame_len = reader.getEntryValue("max_frame_len");
    if (value4max_frame_len == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->max_frame_len = 64512;
    } else {
        ret = TdrParse::parseUInt32(this->max_frame_len, value4max_frame_len, NULL, 64512, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4accept_send_buf_size = reader.getEntryValue("accept_send_buf_size");
    if (value4accept_send_buf_size == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->accept_send_buf_size = 32;
    } else {
        ret = TdrParse::parseUInt32(this->accept_send_buf_size, value4accept_send_buf_size, NULL, 32, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4connect_send_deque_size = reader.getEntryValue("connect_send_deque_size");
    if (value4connect_send_deque_size == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->connect_send_deque_size = 256;
    } else {
        ret = TdrParse::parseUInt32(this->connect_send_deque_size, value4connect_send_deque_size, NULL, 256, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4connect_timeout = reader.getEntryValue("connect_timeout");
    if (value4connect_timeout == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->connect_timeout = 15;
    } else {
        ret = TdrParse::parseUInt32(this->connect_timeout, value4connect_timeout, NULL, 15, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4recv_timeout = reader.getEntryValue("recv_timeout");
    if (value4recv_timeout == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->recv_timeout = 0;
    } else {
        ret = TdrParse::parseUInt32(this->recv_timeout, value4recv_timeout, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4is_proxy = reader.getEntryValue("is_proxy");
    if (value4is_proxy == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->is_proxy = 0;
    } else {
        ret = TdrParse::parseUInt8(this->is_proxy, value4is_proxy, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4max_accept_svr = reader.getEntryValue("max_accept_svr");
    if (value4max_accept_svr == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->max_accept_svr = 1024;
    } else {
        ret = TdrParse::parseUInt32(this->max_accept_svr, value4max_accept_svr, NULL, 1024, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4is_lock_pipe = reader.getEntryValue("is_lock_pipe");
    if (value4is_lock_pipe == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->is_lock_pipe = 0;
    } else {
        ret = TdrParse::parseUInt8(this->is_lock_pipe, value4is_lock_pipe, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4retry_error = reader.getEntryValue("retry_error");
    if (value4retry_error == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->retry_error = 3;
    } else {
        ret = TdrParse::parseUInt8(this->retry_error, value4retry_error, NULL, 3, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4recv_pipe_len = reader.getEntryValue("recv_pipe_len");
    if (value4recv_pipe_len == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->recv_pipe_len = 52428800;
    } else {
        ret = TdrParse::parseUInt32(this->recv_pipe_len, value4recv_pipe_len, NULL, 52428800, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4send_pipe_len = reader.getEntryValue("send_pipe_len");
    if (value4send_pipe_len == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->send_pipe_len = 52428800;
    } else {
        ret = TdrParse::parseUInt32(this->send_pipe_len, value4send_pipe_len, NULL, 52428800, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4error_pipe_len = reader.getEntryValue("error_pipe_len");
    if (value4error_pipe_len == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->error_pipe_len = 52428800;
    } else {
        ret = TdrParse::parseUInt32(this->error_pipe_len, value4error_pipe_len, NULL, 52428800, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4check_pthread = reader.getEntryValue("check_pthread");
    if (value4check_pthread == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->check_pthread = 1;
    } else {
        ret = TdrParse::parseUInt8(this->check_pthread, value4check_pthread, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4insurance = reader.getEntryValue("insurance");
    if (value4insurance == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->insurance = 1;
    } else {
        ret = TdrParse::parseUInt8(this->insurance, value4insurance, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4opt_key1 = reader.getEntryValue("opt_key1");
    if (value4opt_key1 == NULL) {
        this->opt_key1[0] = 0;
    } else {
        const size_t length4opt_key1 = strlen(value4opt_key1);
        if (length4opt_key1 > 127) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->opt_key1, value4opt_key1, 128);
    }
    value4opt_key2 = reader.getEntryValue("opt_key2");
    if (value4opt_key2 == NULL) {
        this->opt_key2[0] = 0;
    } else {
        const size_t length4opt_key2 = strlen(value4opt_key2);
        if (length4opt_key2 > 127) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->opt_key2, value4opt_key2, 128);
    }
    value4get_svr_info_type = reader.getEntryValue("get_svr_info_type");
    if (value4get_svr_info_type == NULL) {
        memset(this->get_svr_info_type, 0, 64);
        strcpy(this->get_svr_info_type, "cfgfile");
    } else {
        const size_t length4get_svr_info_type = strlen(value4get_svr_info_type);
        if (length4get_svr_info_type > 63) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->get_svr_info_type, value4get_svr_info_type, 64);
    }
    return ret;
}

TdrError::ErrorType CommCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType CommCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType CommCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType CommCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("CommCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("CommCfg");
        }
    }
    return ret;
}

TdrError::ErrorType CommCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_frame_len");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_frame_len");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_frame_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_frame_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "accept_send_buf_size");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "accept_send_buf_size");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->accept_send_buf_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "accept_send_buf_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "connect_send_deque_size");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "connect_send_deque_size");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->connect_send_deque_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "connect_send_deque_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "connect_timeout");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "connect_timeout");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->connect_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "connect_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "recv_timeout");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "recv_timeout");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->recv_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "recv_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_proxy");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_proxy");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_proxy);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_proxy");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_accept_svr");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_accept_svr");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_accept_svr);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_accept_svr");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_lock_pipe");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_lock_pipe");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_lock_pipe);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_lock_pipe");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "retry_error");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "retry_error");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->retry_error);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "retry_error");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "recv_pipe_len");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "recv_pipe_len");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->recv_pipe_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "recv_pipe_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "send_pipe_len");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "send_pipe_len");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->send_pipe_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "send_pipe_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "error_pipe_len");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "error_pipe_len");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->error_pipe_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "error_pipe_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "check_pthread");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "check_pthread");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->check_pthread);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "check_pthread");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "insurance");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "insurance");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->insurance);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "insurance");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "opt_key1");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "opt_key1");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->opt_key1, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "opt_key1");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "opt_key2");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "opt_key2");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->opt_key2, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "opt_key2");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "get_svr_info_type");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "get_svr_info_type");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->get_svr_info_type, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "get_svr_info_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CommCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "max_frame_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_frame_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "accept_send_buf_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->accept_send_buf_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "connect_send_deque_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->connect_send_deque_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "connect_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->connect_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "recv_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->recv_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_proxy");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_proxy);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "max_accept_svr");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_accept_svr);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_lock_pipe");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_lock_pipe);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "retry_error");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->retry_error);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "recv_pipe_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->recv_pipe_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "send_pipe_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->send_pipe_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "error_pipe_len");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->error_pipe_len);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "check_pthread");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->check_pthread);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "insurance");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->insurance);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "opt_key1");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->opt_key1, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "opt_key2");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->opt_key2, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "get_svr_info_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->get_svr_info_type, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType CfgSvrCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CfgSvrCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("CfgSvrCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("CfgSvrCfg");
        }
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4svr_ip;
    const char *value4svr_port;

    if (reader.stepIn("svr_info") != TdrXmlReader::WS_NORMAL) {
        ret = this->svr_info.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->svr_info.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("svr_info");
    }
    value4svr_ip = reader.getEntryValue("svr_ip");
    if (value4svr_ip == NULL) {
        this->svr_ip[0] = 0;
    } else {
        const size_t length4svr_ip = strlen(value4svr_ip);
        if (length4svr_ip > 15) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->svr_ip, value4svr_ip, 16);
    }
    value4svr_port = reader.getEntryValue("svr_port");
    if (value4svr_port == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->svr_port = 0;
    } else {
        ret = TdrParse::parseUInt16(this->svr_port, value4svr_port, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType CfgSvrCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("CfgSvrCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("CfgSvrCfg");
        }
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->svr_info.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_ip");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_ip");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->svr_ip, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "svr_ip");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "svr_port");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "svr_port");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->svr_port);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "svr_port");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CfgSvrCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "svr_ip");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->svr_ip, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "svr_port");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->svr_port);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->svr_info.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("svr_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::construct() {
    this->use_console = 0;
    memset(this->console_ip, 0, sizeof(this->console_ip));
    strcpy(this->console_ip, "127.0.0.1");
    this->console_port = 8001;
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType ConsoleCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType ConsoleCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("ConsoleCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("ConsoleCfg");
        }
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4use_console;
    const char *value4console_ip;
    const char *value4console_port;

    value4use_console = reader.getEntryValue("use_console");
    if (value4use_console == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->use_console = 0;
    } else {
        ret = TdrParse::parseUInt8(this->use_console, value4use_console, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4console_ip = reader.getEntryValue("console_ip");
    if (value4console_ip == NULL) {
        memset(this->console_ip, 0, 16);
        strcpy(this->console_ip, "127.0.0.1");
    } else {
        const size_t length4console_ip = strlen(value4console_ip);
        if (length4console_ip > 15) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->console_ip, value4console_ip, 16);
    }
    value4console_port = reader.getEntryValue("console_port");
    if (value4console_port == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->console_port = 8001;
    } else {
        ret = TdrParse::parseUInt16(this->console_port, value4console_port, NULL, 8001, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType ConsoleCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("ConsoleCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("ConsoleCfg");
        }
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "use_console");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "use_console");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->use_console);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "use_console");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "console_ip");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "console_ip");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->console_ip, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "console_ip");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "console_port");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "console_port");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->console_port);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "console_port");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType ConsoleCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "use_console");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->use_console);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "console_ip");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->console_ip, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "console_port");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->console_port);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CheckCfg::construct() {
    this->check_frame = 0;
    this->check_rcvid = 0;
    this->num_snd_type = 3;
    memset(this->allow_snd_types, 0, 10);
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType CheckCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CheckCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CheckCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CheckCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("CheckCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("CheckCfg");
        }
    }
    return ret;
}

TdrError::ErrorType CheckCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4check_frame;
    const char *value4check_rcvid;
    const char *value4num_snd_type;
    const char *value4allow_snd_types;
    unsigned int tempCount4allow_snd_types;

    value4check_frame = reader.getEntryValue("check_frame");
    if (value4check_frame == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->check_frame = 0;
    } else {
        ret = TdrParse::parseUInt8(this->check_frame, value4check_frame, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4check_rcvid = reader.getEntryValue("check_rcvid");
    if (value4check_rcvid == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->check_rcvid = 0;
    } else {
        ret = TdrParse::parseUInt8(this->check_rcvid, value4check_rcvid, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4num_snd_type = reader.getEntryValue("num_snd_type");
    if (value4num_snd_type == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->num_snd_type = 10;
    } else {
        ret = TdrParse::parseUInt8(this->num_snd_type, value4num_snd_type, NULL, 3, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (this->num_snd_type > 10) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    value4allow_snd_types = reader.getNodeValue("allow_snd_types");
    if (value4allow_snd_types == NULL) {
        this->num_snd_type = 0;
        return TdrError::TDR_NO_ERROR;
    }
    tempCount4allow_snd_types = 0;
    ret = TdrParse::parseUInt32(this->allow_snd_types, this->num_snd_type, value4allow_snd_types, &tempCount4allow_snd_types, 0, NULL, NULL);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->num_snd_type = tempCount4allow_snd_types;
    return ret;
}

TdrError::ErrorType CheckCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType CheckCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType CheckCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType CheckCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("CheckCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("CheckCfg");
        }
    }
    return ret;
}

TdrError::ErrorType CheckCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint8_t allow_snd_types_i;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "check_frame");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "check_frame");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->check_frame);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "check_frame");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "check_rcvid");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "check_rcvid");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->check_rcvid);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "check_rcvid");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "num_snd_type");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "num_snd_type");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->num_snd_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "num_snd_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->num_snd_type > 10) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "allow_snd_types");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "allow_snd_types");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (allow_snd_types_i = 0; allow_snd_types_i < this->num_snd_type; allow_snd_types_i++) {
        ret = writer.textize("%u ", this->allow_snd_types[allow_snd_types_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "allow_snd_types");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CheckCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint8_t allow_snd_types_i;
    ret = writer._out.textize(" %s=\"", "check_frame");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->check_frame);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "check_rcvid");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->check_rcvid);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "num_snd_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->num_snd_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->num_snd_type > 10) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "allow_snd_types");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "allow_snd_types");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (allow_snd_types_i = 0; allow_snd_types_i < this->num_snd_type; allow_snd_types_i++) {
        ret = writer.textize("%u ", this->allow_snd_types[allow_snd_types_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "allow_snd_types");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType AutoConnectSvr::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType AutoConnectSvr::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("AutoConnectSvr") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("AutoConnectSvr");
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;

    if (reader.stepIn("main_svr") != TdrXmlReader::WS_NORMAL) {
        ret = this->main_svr.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->main_svr.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("main_svr");
    }
    if (reader.stepIn("backup_svr") != TdrXmlReader::WS_NORMAL) {
        return this->backup_svr.construct();
    }
    ret = this->backup_svr.entryFromXml(reader, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    reader.stepOut("backup_svr");
    return ret;
}

TdrError::ErrorType AutoConnectSvr::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType AutoConnectSvr::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("AutoConnectSvr");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("AutoConnectSvr");
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("main_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->main_svr.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("main_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("backup_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->backup_svr.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("backup_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType AutoConnectSvr::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("main_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->main_svr.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("main_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("backup_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->backup_svr.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("backup_svr");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::construct() {
    TdrError::ErrorType ret;
    uint32_t auto_connect_svrs_i;
    this->auto_connect_num = 1;
    for (auto_connect_svrs_i = 0; auto_connect_svrs_i < 100; auto_connect_svrs_i++) {
        ret = this->auto_connect_svrs[auto_connect_svrs_i].construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            break;
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType AutoConnectCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("AutoConnectCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("AutoConnectCfg");
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4auto_connect_num;
    uint32_t auto_connect_svrs_i;

    value4auto_connect_num = reader.getEntryValue("auto_connect_num");
    if (value4auto_connect_num == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->auto_connect_num = 100;
    } else {
        ret = TdrParse::parseUInt32(this->auto_connect_num, value4auto_connect_num, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (this->auto_connect_num > 100) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->auto_connect_num == 0) {
        return TdrError::TDR_NO_ERROR;
    }
    for (auto_connect_svrs_i = 0; auto_connect_svrs_i < this->auto_connect_num; auto_connect_svrs_i++) {
        if (reader.stepIn("auto_connect_svrs") != TdrXmlReader::WS_NORMAL) {
            this->auto_connect_num = auto_connect_svrs_i;
            break;
        }
        ret = this->auto_connect_svrs[auto_connect_svrs_i].entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("auto_connect_svrs");
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType AutoConnectCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("AutoConnectCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("AutoConnectCfg");
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint32_t auto_connect_svrs_i;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "auto_connect_num");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "auto_connect_num");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->auto_connect_num);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "auto_connect_num");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->auto_connect_num > 100) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->auto_connect_num == 0) {
        return TdrError::TDR_NO_ERROR;
    }
    for (auto_connect_svrs_i = 0; auto_connect_svrs_i < this->auto_connect_num; auto_connect_svrs_i++) {
        ret = writer.openComplex("auto_connect_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = this->auto_connect_svrs[auto_connect_svrs_i].entryToListXml(writer, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = writer.closeComplex("auto_connect_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType AutoConnectCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint32_t auto_connect_svrs_i;
    ret = writer._out.textize(" %s=\"", "auto_connect_num");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->auto_connect_num);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->auto_connect_num > 100) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->auto_connect_num == 0) {
        return TdrError::TDR_NO_ERROR;
    }
    for (auto_connect_svrs_i = 0; auto_connect_svrs_i < this->auto_connect_num; auto_connect_svrs_i++) {
        ret = writer.openComplex("auto_connect_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = this->auto_connect_svrs[auto_connect_svrs_i].entryToAttrXml(writer, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ret = writer.closeComplex("auto_connect_svrs");
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType RestrictCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType RestrictCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("RestrictCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("RestrictCfg");
        }
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4allow_ips;
    const char *value4reject_ips;

    value4allow_ips = reader.getEntryValue("allow_ips");
    if (value4allow_ips == NULL) {
        this->allow_ips[0] = 0;
    } else {
        const size_t length4allow_ips = strlen(value4allow_ips);
        if (length4allow_ips > 20479) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->allow_ips, value4allow_ips, 20480);
    }
    value4reject_ips = reader.getEntryValue("reject_ips");
    if (value4reject_ips == NULL) {
        this->reject_ips[0] = 0;
    } else {
        const size_t length4reject_ips = strlen(value4reject_ips);
        if (length4reject_ips > 20479) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->reject_ips, value4reject_ips, 20480);
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType RestrictCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("RestrictCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("RestrictCfg");
        }
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "allow_ips");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "allow_ips");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->allow_ips, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "allow_ips");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "reject_ips");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "reject_ips");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->reject_ips, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "reject_ips");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType RestrictCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "allow_ips");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->allow_ips, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "reject_ips");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->reject_ips, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType MonitorCfg::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType MonitorCfg::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("MonitorCfg") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("MonitorCfg");
        }
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4monitor_num;
    const char *value4monitor_cmds;
    unsigned int tempCount4monitor_cmds;

    value4monitor_num = reader.getEntryValue("monitor_num");
    if (value4monitor_num == NULL) {
        ret = TdrError::TDR_NO_ERROR;
        this->monitor_num = 100;
    } else {
        ret = TdrParse::parseUInt32(this->monitor_num, value4monitor_num, NULL, 100, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (this->monitor_num > 100) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    value4monitor_cmds = reader.getNodeValue("monitor_cmds");
    if (value4monitor_cmds == NULL) {
        this->monitor_num = 0;
        return TdrError::TDR_NO_ERROR;
    }
    tempCount4monitor_cmds = 0;
    ret = TdrParse::parseUInt32(this->monitor_cmds, this->monitor_num, value4monitor_cmds, &tempCount4monitor_cmds, 0, NULL, NULL);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->monitor_num = tempCount4monitor_cmds;
    return ret;
}

TdrError::ErrorType MonitorCfg::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType MonitorCfg::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("MonitorCfg");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("MonitorCfg");
        }
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint32_t monitor_cmds_i;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_num");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_num");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->monitor_num);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "monitor_num");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->monitor_num > 100) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_cmds");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_cmds");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (monitor_cmds_i = 0; monitor_cmds_i < this->monitor_num; monitor_cmds_i++) {
        ret = writer.textize("%u ", this->monitor_cmds[monitor_cmds_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "monitor_cmds");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType MonitorCfg::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    uint32_t monitor_cmds_i;
    ret = writer._out.textize(" %s=\"", "monitor_num");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->monitor_num);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->monitor_num > 100) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_cmds");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_cmds");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (monitor_cmds_i = 0; monitor_cmds_i < this->monitor_num; monitor_cmds_i++) {
        ret = writer.textize("%u ", this->monitor_cmds[monitor_cmds_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "monitor_cmds");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType zerg_config::construct() {
    TdrError::ErrorType ret;
    ret = this->self_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->log_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->comm_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->cfgsvr_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->console_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->check_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->auto_connect_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->restrict_cfg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return this->monitor_cfg.construct();
}

TdrError::ErrorType zerg_config::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlfile;
        ret = xmlfile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlfile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType zerg_config::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType zerg_config::fromXmlString(char *str, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (str != NULL) {
        TdrXmlReader reader(str, format);
        ret = reader.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXml(reader, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType zerg_config::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("zerg_config") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("zerg_config");
        }
    }
    return ret;
}

TdrError::ErrorType zerg_config::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;

    if (reader.stepIn("self_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->self_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->self_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("self_cfg");
    }
    if (reader.stepIn("log_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->log_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->log_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("log_cfg");
    }
    if (reader.stepIn("comm_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->comm_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->comm_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("comm_cfg");
    }
    if (reader.stepIn("cfgsvr_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->cfgsvr_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->cfgsvr_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("cfgsvr_cfg");
    }
    if (reader.stepIn("console_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->console_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->console_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("console_cfg");
    }
    if (reader.stepIn("check_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->check_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->check_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("check_cfg");
    }
    if (reader.stepIn("auto_connect_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->auto_connect_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->auto_connect_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("auto_connect_cfg");
    }
    if (reader.stepIn("restrict_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->restrict_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->restrict_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("restrict_cfg");
    }
    if (reader.stepIn("monitor_cfg") != TdrXmlReader::WS_NORMAL) {
        ret = this->monitor_cfg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->monitor_cfg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("monitor_cfg");
    }
    return ret;
}

TdrError::ErrorType zerg_config::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType zerg_config::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (fp != NULL) {
        TdrXmlWriter writer(fp, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
            if (ret == TdrError::TDR_NO_ERROR) {
                ret = toXml(writer, format, cutVer);
            }
        }
    }
    return ret;
}

TdrError::ErrorType zerg_config::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (buffer != NULL) {
        TdrXmlWriter writer(buffer, size, format);
        ret = writer.state();
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK");
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

TdrError::ErrorType zerg_config::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("zerg_config");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("zerg_config");
        }
    }
    return ret;
}

TdrError::ErrorType zerg_config::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("self_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->self_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("self_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("log_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->log_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("log_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("comm_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->comm_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("comm_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("cfgsvr_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->cfgsvr_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("cfgsvr_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("console_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->console_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("console_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("check_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->check_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("check_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("auto_connect_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->auto_connect_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("auto_connect_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("restrict_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->restrict_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("restrict_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("monitor_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->monitor_cfg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("monitor_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType zerg_config::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("self_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->self_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("self_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("log_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->log_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("log_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("comm_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->comm_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("comm_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("cfgsvr_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->cfgsvr_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("cfgsvr_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("console_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->console_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("console_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("check_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->check_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("check_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("auto_connect_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->auto_connect_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("auto_connect_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("restrict_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->restrict_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("restrict_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("monitor_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->monitor_cfg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("monitor_cfg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

