// Reconstructed from secagent disassembly (TDR-generated config structures)

#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/protocol/common/TdrXml.h"
#include "src/protocol/common/TdrParse.h"

#include <string.h>

using namespace tsf4g_tdr;
using namespace secagent_config;


TdrError::ErrorType SanlixChecker::construct() {
    this->is_use_ = 0;
    this->is_distribute_tenparty_data_ = 1;
    strncpy(this->tp_data_path_, "./cfg/base_cfg/Tenparty.dat", 256);
    this->is_distribute_ts_data_ = 1;
    strncpy(this->ts_data_path_, "./cfg/sanlix/TS0001.dat", 256);
    this->is_distribute_te_data_ = 1;
    strncpy(this->te_data_path_, "./cfg/base_cfg/TE0001.dat", 256);
    this->is_distribute_tk_data_ = 0;
    strncpy(this->tk_data_path_, "./cfg/base_cfg/TK0001.dat", 256);
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType SanlixChecker::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType SanlixChecker::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType SanlixChecker::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType SanlixChecker::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("SanlixChecker") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("SanlixChecker");
        }
    }
    return ret;
}

TdrError::ErrorType SanlixChecker::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4is_use_;
    const char *value4is_distribute_tenparty_data_;
    const char *value4tp_data_path_;
    const char *value4is_distribute_ts_data_;
    const char *value4ts_data_path_;
    const char *value4is_distribute_te_data_;
    const char *value4te_data_path_;
    const char *value4is_distribute_tk_data_;
    const char *value4tk_data_path_;

    value4is_use_ = reader.getEntryValue("is_use_");
    if (value4is_use_ == NULL) {
        this->is_use_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->is_use_, value4is_use_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4is_distribute_tenparty_data_ = reader.getEntryValue("is_distribute_tenparty_data_");
    if (value4is_distribute_tenparty_data_ == NULL) {
        this->is_distribute_tenparty_data_ = 1;
    } else {
        ret = TdrParse::parseInt32(this->is_distribute_tenparty_data_, value4is_distribute_tenparty_data_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4tp_data_path_ = reader.getEntryValue("tp_data_path_");
    if (value4tp_data_path_ == NULL) {
        strncpy(this->tp_data_path_, "./cfg/base_cfg/Tenparty.dat", 256);
    } else {
        const size_t length4tp_data_path_ = strlen(value4tp_data_path_);
        if (length4tp_data_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->tp_data_path_, value4tp_data_path_, 256);
    }
    value4is_distribute_ts_data_ = reader.getEntryValue("is_distribute_ts_data_");
    if (value4is_distribute_ts_data_ == NULL) {
        this->is_distribute_ts_data_ = 1;
    } else {
        ret = TdrParse::parseInt32(this->is_distribute_ts_data_, value4is_distribute_ts_data_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4ts_data_path_ = reader.getEntryValue("ts_data_path_");
    if (value4ts_data_path_ == NULL) {
        strncpy(this->ts_data_path_, "./cfg/sanlix/TS0001.dat", 256);
    } else {
        const size_t length4ts_data_path_ = strlen(value4ts_data_path_);
        if (length4ts_data_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->ts_data_path_, value4ts_data_path_, 256);
    }
    value4is_distribute_te_data_ = reader.getEntryValue("is_distribute_te_data_");
    if (value4is_distribute_te_data_ == NULL) {
        this->is_distribute_te_data_ = 1;
    } else {
        ret = TdrParse::parseInt32(this->is_distribute_te_data_, value4is_distribute_te_data_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4te_data_path_ = reader.getEntryValue("te_data_path_");
    if (value4te_data_path_ == NULL) {
        strncpy(this->te_data_path_, "./cfg/base_cfg/TE0001.dat", 256);
    } else {
        const size_t length4te_data_path_ = strlen(value4te_data_path_);
        if (length4te_data_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->te_data_path_, value4te_data_path_, 256);
    }
    value4is_distribute_tk_data_ = reader.getEntryValue("is_distribute_tk_data_");
    if (value4is_distribute_tk_data_ == NULL) {
        this->is_distribute_tk_data_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->is_distribute_tk_data_, value4is_distribute_tk_data_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4tk_data_path_ = reader.getEntryValue("tk_data_path_");
    if (value4tk_data_path_ == NULL) {
        strncpy(this->tk_data_path_, "./cfg/base_cfg/TK0001.dat", 256);
    } else {
        const size_t length4tk_data_path_ = strlen(value4tk_data_path_);
        if (length4tk_data_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->tk_data_path_, value4tk_data_path_, 256);
    }
    return ret;
}

TdrError::ErrorType SanlixChecker::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType SanlixChecker::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType SanlixChecker::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType SanlixChecker::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("SanlixChecker");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("SanlixChecker");
        }
    }
    return ret;
}

TdrError::ErrorType SanlixChecker::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_use_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_use_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->is_use_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_use_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_distribute_tenparty_data_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_distribute_tenparty_data_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->is_distribute_tenparty_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_distribute_tenparty_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "tp_data_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "tp_data_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->tp_data_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "tp_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_distribute_ts_data_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_distribute_ts_data_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->is_distribute_ts_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_distribute_ts_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "ts_data_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "ts_data_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->ts_data_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "ts_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_distribute_te_data_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_distribute_te_data_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->is_distribute_te_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_distribute_te_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "te_data_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "te_data_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->te_data_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "te_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_distribute_tk_data_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_distribute_tk_data_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->is_distribute_tk_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_distribute_tk_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "tk_data_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "tk_data_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->tk_data_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "tk_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType SanlixChecker::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "is_use_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->is_use_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_distribute_tenparty_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->is_distribute_tenparty_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "tp_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->tp_data_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_distribute_ts_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->is_distribute_ts_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "ts_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->ts_data_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_distribute_te_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->is_distribute_te_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "te_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->te_data_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_distribute_tk_data_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->is_distribute_tk_data_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "tk_data_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->tk_data_path_, false);
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

TdrError::ErrorType CmdListInfo::construct() {
    this->cmd_list_count_ = 1;
    memset(this->cmd_, 0, 30);
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType CmdListInfo::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CmdListInfo::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CmdListInfo::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CmdListInfo::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("CmdListInfo") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("CmdListInfo");
        }
    }
    return ret;
}

TdrError::ErrorType CmdListInfo::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4cmd_list_count_;
    const char *value4cmd_;
    unsigned int tempCount4cmd_;

    value4cmd_list_count_ = reader.getEntryValue("cmd_list_count_");
    if (value4cmd_list_count_ == NULL) {
        this->cmd_list_count_ = 30;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_list_count_, value4cmd_list_count_, NULL, 30, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_ = reader.getNodeValue("cmd_");
    if (value4cmd_ == NULL) {
        this->cmd_list_count_ = 0;
        return ret;
    }
    tempCount4cmd_ = 0;
    ret = TdrParse::parseUInt16(this->cmd_, this->cmd_list_count_, value4cmd_, &tempCount4cmd_, 0, NULL, NULL);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->cmd_list_count_ = tempCount4cmd_;
    return ret;
}

TdrError::ErrorType CmdListInfo::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType CmdListInfo::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType CmdListInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType CmdListInfo::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("CmdListInfo");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("CmdListInfo");
        }
    }
    return ret;
}

TdrError::ErrorType CmdListInfo::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned int cmd__i;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_list_count_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_list_count_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_list_count_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_list_count_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->cmd_list_count_ > 30) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->cmd_list_count_ != 0) {
        for (cmd__i = 0; cmd__i < this->cmd_list_count_; cmd__i++) {
            ret = writer.textize("%d ", this->cmd_[cmd__i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "cmd_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CmdListInfo::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned int cmd__i;
    ret = writer._out.textize(" %s=\"", "cmd_list_count_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_list_count_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->cmd_list_count_ > 30) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->cmd_list_count_ != 0) {
        for (cmd__i = 0; cmd__i < this->cmd_list_count_; cmd__i++) {
            ret = writer.textize("%d ", this->cmd_[cmd__i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "cmd_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::construct() {
    TdrError::ErrorType ret;
    this->is_use_ = 0;
    this->crypt_test_gap_ = 75;
    ret = this->up_cmdlist_info_.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmdlist_info_.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    strncpy(this->dib_base_path_, "./cfg/dib/", 256);
    strncpy(this->exe_path_, "./cfg/game.exe", 256);
    strncpy(this->exe_version_path_, "./cfg/verlist.ini", 256);
    this->antibot_error_thread_ = 5;
    return ret;
}

TdrError::ErrorType CryptCheckerConf::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CryptCheckerConf::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType CryptCheckerConf::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("CryptCheckerConf") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("CryptCheckerConf");
        }
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4is_use_;
    const char *value4crypt_test_gap_;
    const char *value4dib_base_path_;
    const char *value4exe_path_;
    const char *value4exe_version_path_;
    const char *value4antibot_error_thread_;

    value4is_use_ = reader.getEntryValue("is_use_");
    if (value4is_use_ == NULL) {
        this->is_use_ = 0;
    } else {
        ret = TdrParse::parseInt32(this->is_use_, value4is_use_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4crypt_test_gap_ = reader.getEntryValue("crypt_test_gap_");
    if (value4crypt_test_gap_ == NULL) {
        this->crypt_test_gap_ = 75;
    } else {
        ret = TdrParse::parseInt32(this->crypt_test_gap_, value4crypt_test_gap_, NULL, 75, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (reader.stepIn("up_cmdlist_info_") != TdrXmlReader::WS_NORMAL) {
        ret = this->up_cmdlist_info_.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->up_cmdlist_info_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("up_cmdlist_info_");
    }
    if (reader.stepIn("down_cmdlist_info_") != TdrXmlReader::WS_NORMAL) {
        ret = this->down_cmdlist_info_.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->down_cmdlist_info_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("down_cmdlist_info_");
    }
    value4dib_base_path_ = reader.getEntryValue("dib_base_path_");
    if (value4dib_base_path_ == NULL) {
        strncpy(this->dib_base_path_, "./cfg/dib/", 256);
    } else {
        const size_t length4dib_base_path_ = strlen(value4dib_base_path_);
        if (length4dib_base_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->dib_base_path_, value4dib_base_path_, 256);
    }
    value4exe_path_ = reader.getEntryValue("exe_path_");
    if (value4exe_path_ == NULL) {
        strncpy(this->exe_path_, "./cfg/game.exe", 256);
    } else {
        const size_t length4exe_path_ = strlen(value4exe_path_);
        if (length4exe_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->exe_path_, value4exe_path_, 256);
    }
    value4exe_version_path_ = reader.getEntryValue("exe_version_path_");
    if (value4exe_version_path_ == NULL) {
        strncpy(this->exe_version_path_, "./cfg/verlist.ini", 256);
    } else {
        const size_t length4exe_version_path_ = strlen(value4exe_version_path_);
        if (length4exe_version_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->exe_version_path_, value4exe_version_path_, 256);
    }
    value4antibot_error_thread_ = reader.getEntryValue("antibot_error_thread_");
    if (value4antibot_error_thread_ == NULL) {
        this->antibot_error_thread_ = 5;
    } else {
        ret = TdrParse::parseUInt32(this->antibot_error_thread_, value4antibot_error_thread_, NULL, 5, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType CryptCheckerConf::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType CryptCheckerConf::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("CryptCheckerConf");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeComplex("CryptCheckerConf");
        }
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_use_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_use_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->is_use_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_use_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "crypt_test_gap_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "crypt_test_gap_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->crypt_test_gap_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "crypt_test_gap_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("up_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->up_cmdlist_info_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("up_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("down_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmdlist_info_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("down_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "dib_base_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "dib_base_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->dib_base_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "dib_base_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "exe_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "exe_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->exe_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "exe_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "exe_version_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "exe_version_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->exe_version_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "exe_version_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "antibot_error_thread_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "antibot_error_thread_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->antibot_error_thread_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "antibot_error_thread_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType CryptCheckerConf::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "is_use_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->is_use_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "crypt_test_gap_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->crypt_test_gap_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "dib_base_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->dib_base_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "exe_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->exe_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "exe_version_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->exe_version_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "antibot_error_thread_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->antibot_error_thread_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("up_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->up_cmdlist_info_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("up_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("down_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmdlist_info_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("down_cmdlist_info_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType config::construct() {
    TdrError::ErrorType ret;
    this->is_for_mmog_ = 1;
    this->if_restore_gmsvr_channel_ = 1;
    this->if_check_pthread_ = 1;
    this->if_restore_dpsdk_channel_ = 1;
    this->if_restore_user_info_ = 1;
    this->log_priority_ = 4;
    this->expire_time_ = 300;
    this->table_node_num_ = 1000000;
    strncpy(this->dp_config_path_, "dp_config.xml", 256);
    this->report_statistic_time_ = 5000;
    strncpy(this->restrict_file_, "./cfg/restrict.txt", 256);
    ret = this->crypt_checker_conf_.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->max_package_size_ = 0;
    this->if_rpcode_with_key_ = 0;
    this->rpcode_keyinfo_key_[0] = 0;
    this->if_open_cs_channel_ = 0;
    ret = this->sanlix_checker_conf_.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->total_max_send_count_ = 1000;
    this->if_check_rpcode_hash_ = 0;
    this->gamesvr_channel_num_ = 12;
    this->channel_recv_times_ = 1;
    return ret;
}

TdrError::ErrorType config::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    if (file != NULL) {
        TdrXmlFile xmlFile;
        ret = xmlFile.load(file);
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = fromXmlString(xmlFile.buf(), format, cutVer);
        }
    }
    return ret;
}

TdrError::ErrorType config::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType config::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType config::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("config") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("config");
        }
    }
    return ret;
}

TdrError::ErrorType config::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4is_for_mmog_;
    const char *value4if_restore_gmsvr_channel_;
    const char *value4if_check_pthread_;
    const char *value4if_restore_dpsdk_channel_;
    const char *value4if_restore_user_info_;
    const char *value4log_priority_;
    const char *value4expire_time_;
    const char *value4table_node_num_;
    const char *value4dp_config_path_;
    const char *value4report_statistic_time_;
    const char *value4restrict_file_;
    const char *value4max_package_size_;
    const char *value4if_rpcode_with_key_;
    const char *value4rpcode_keyinfo_key_;
    const char *value4if_open_cs_channel_;
    const char *value4total_max_send_count_;
    const char *value4if_check_rpcode_hash_;
    const char *value4gamesvr_channel_num_;
    const char *value4channel_recv_times_;

    value4is_for_mmog_ = reader.getEntryValue("is_for_mmog_");
    if (value4is_for_mmog_ == NULL) {
        this->is_for_mmog_ = 1;
    } else {
        ret = TdrParse::parseUInt8(this->is_for_mmog_, value4is_for_mmog_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4if_restore_gmsvr_channel_ = reader.getEntryValue("if_restore_gmsvr_channel_");
    if (value4if_restore_gmsvr_channel_ == NULL) {
        this->if_restore_gmsvr_channel_ = 1;
    } else {
        ret = TdrParse::parseUInt8(this->if_restore_gmsvr_channel_, value4if_restore_gmsvr_channel_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4if_check_pthread_ = reader.getEntryValue("if_check_pthread_");
    if (value4if_check_pthread_ == NULL) {
        this->if_check_pthread_ = 1;
    } else {
        ret = TdrParse::parseUInt8(this->if_check_pthread_, value4if_check_pthread_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4if_restore_dpsdk_channel_ = reader.getEntryValue("if_restore_dpsdk_channel_");
    if (value4if_restore_dpsdk_channel_ == NULL) {
        this->if_restore_dpsdk_channel_ = 1;
    } else {
        ret = TdrParse::parseUInt8(this->if_restore_dpsdk_channel_, value4if_restore_dpsdk_channel_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4if_restore_user_info_ = reader.getEntryValue("if_restore_user_info_");
    if (value4if_restore_user_info_ == NULL) {
        this->if_restore_user_info_ = 1;
    } else {
        ret = TdrParse::parseUInt8(this->if_restore_user_info_, value4if_restore_user_info_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4log_priority_ = reader.getEntryValue("log_priority_");
    if (value4log_priority_ == NULL) {
        this->log_priority_ = 4;
    } else {
        ret = TdrParse::parseUInt32(this->log_priority_, value4log_priority_, NULL, 4, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4expire_time_ = reader.getEntryValue("expire_time_");
    if (value4expire_time_ == NULL) {
        this->expire_time_ = 300;
    } else {
        ret = TdrParse::parseUInt32(this->expire_time_, value4expire_time_, NULL, 300, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4table_node_num_ = reader.getEntryValue("table_node_num_");
    if (value4table_node_num_ == NULL) {
        this->table_node_num_ = 1000000;
    } else {
        ret = TdrParse::parseUInt32(this->table_node_num_, value4table_node_num_, NULL, 1000000, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4dp_config_path_ = reader.getEntryValue("dp_config_path_");
    if (value4dp_config_path_ == NULL) {
        strncpy(this->dp_config_path_, "dp_config.xml", 256);
    } else {
        const size_t length4dp_config_path_ = strlen(value4dp_config_path_);
        if (length4dp_config_path_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->dp_config_path_, value4dp_config_path_, 256);
    }
    value4report_statistic_time_ = reader.getEntryValue("report_statistic_time_");
    if (value4report_statistic_time_ == NULL) {
        this->report_statistic_time_ = 5000;
    } else {
        ret = TdrParse::parseUInt32(this->report_statistic_time_, value4report_statistic_time_, NULL, 5000, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4restrict_file_ = reader.getEntryValue("restrict_file_");
    if (value4restrict_file_ == NULL) {
        strncpy(this->restrict_file_, "./cfg/restrict.txt", 256);
    } else {
        const size_t length4restrict_file_ = strlen(value4restrict_file_);
        if (length4restrict_file_ > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->restrict_file_, value4restrict_file_, 256);
    }
    if (reader.stepIn("crypt_checker_conf_") != TdrXmlReader::WS_NORMAL) {
        ret = this->crypt_checker_conf_.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->crypt_checker_conf_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("crypt_checker_conf_");
    }
    value4max_package_size_ = reader.getEntryValue("max_package_size_");
    if (value4max_package_size_ == NULL) {
        this->max_package_size_ = 0;
    } else {
        ret = TdrParse::parseUInt32(this->max_package_size_, value4max_package_size_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4if_rpcode_with_key_ = reader.getEntryValue("if_rpcode_with_key_");
    if (value4if_rpcode_with_key_ == NULL) {
        this->if_rpcode_with_key_ = 0;
    } else {
        ret = TdrParse::parseUInt32(this->if_rpcode_with_key_, value4if_rpcode_with_key_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4rpcode_keyinfo_key_ = reader.getEntryValue("rpcode_keyinfo_key_");
    if (value4rpcode_keyinfo_key_ == NULL) {
        this->rpcode_keyinfo_key_[0] = 0;
    } else {
        const size_t length4rpcode_keyinfo_key_ = strlen(value4rpcode_keyinfo_key_);
        if (length4rpcode_keyinfo_key_ > 63) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->rpcode_keyinfo_key_, value4rpcode_keyinfo_key_, 64);
    }
    value4if_open_cs_channel_ = reader.getEntryValue("if_open_cs_channel_");
    if (value4if_open_cs_channel_ == NULL) {
        this->if_open_cs_channel_ = 0;
    } else {
        ret = TdrParse::parseUInt8(this->if_open_cs_channel_, value4if_open_cs_channel_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (reader.stepIn("sanlix_checker_conf_") != TdrXmlReader::WS_NORMAL) {
        ret = this->sanlix_checker_conf_.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->sanlix_checker_conf_.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("sanlix_checker_conf_");
    }
    value4total_max_send_count_ = reader.getEntryValue("total_max_send_count_");
    if (value4total_max_send_count_ == NULL) {
        this->total_max_send_count_ = 1000;
    } else {
        ret = TdrParse::parseUInt32(this->total_max_send_count_, value4total_max_send_count_, NULL, 1000, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4if_check_rpcode_hash_ = reader.getEntryValue("if_check_rpcode_hash_");
    if (value4if_check_rpcode_hash_ == NULL) {
        this->if_check_rpcode_hash_ = 0;
    } else {
        ret = TdrParse::parseUInt8(this->if_check_rpcode_hash_, value4if_check_rpcode_hash_, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4gamesvr_channel_num_ = reader.getEntryValue("gamesvr_channel_num_");
    if (value4gamesvr_channel_num_ == NULL) {
        this->gamesvr_channel_num_ = 12;
    } else {
        ret = TdrParse::parseUInt32(this->gamesvr_channel_num_, value4gamesvr_channel_num_, NULL, 12, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4channel_recv_times_ = reader.getEntryValue("channel_recv_times_");
    if (value4channel_recv_times_ == NULL) {
        this->channel_recv_times_ = 1;
    } else {
        ret = TdrParse::parseUInt8(this->channel_recv_times_, value4channel_recv_times_, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType config::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType config::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType config::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType config::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openComplex("config");
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

TdrError::ErrorType config::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_for_mmog_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_for_mmog_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_for_mmog_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_for_mmog_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_restore_gmsvr_channel_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_restore_gmsvr_channel_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->if_restore_gmsvr_channel_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_restore_gmsvr_channel_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_check_pthread_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_check_pthread_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->if_check_pthread_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_check_pthread_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_restore_dpsdk_channel_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_restore_dpsdk_channel_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->if_restore_dpsdk_channel_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_restore_dpsdk_channel_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_restore_user_info_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_restore_user_info_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->if_restore_user_info_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_restore_user_info_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "log_priority_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "log_priority_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->log_priority_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "log_priority_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "expire_time_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "expire_time_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->expire_time_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "expire_time_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "table_node_num_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "table_node_num_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->table_node_num_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "table_node_num_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "dp_config_path_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "dp_config_path_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->dp_config_path_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "dp_config_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "report_statistic_time_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "report_statistic_time_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->report_statistic_time_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "report_statistic_time_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "restrict_file_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "restrict_file_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->restrict_file_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "restrict_file_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("crypt_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->crypt_checker_conf_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("crypt_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_package_size_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_package_size_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_package_size_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_package_size_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_rpcode_with_key_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_rpcode_with_key_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->if_rpcode_with_key_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_rpcode_with_key_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "rpcode_keyinfo_key_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "rpcode_keyinfo_key_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->rpcode_keyinfo_key_, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "rpcode_keyinfo_key_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_open_cs_channel_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_open_cs_channel_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->if_open_cs_channel_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_open_cs_channel_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("sanlix_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->sanlix_checker_conf_.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("sanlix_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "total_max_send_count_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "total_max_send_count_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->total_max_send_count_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "total_max_send_count_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "if_check_rpcode_hash_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "if_check_rpcode_hash_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->if_check_rpcode_hash_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "if_check_rpcode_hash_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "gamesvr_channel_num_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "gamesvr_channel_num_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->gamesvr_channel_num_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "gamesvr_channel_num_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._indentPending) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "channel_recv_times_");
        writer._indentPending = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "channel_recv_times_");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->channel_recv_times_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "channel_recv_times_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType config::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer._out.textize(" %s=\"", "is_for_mmog_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_for_mmog_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_restore_gmsvr_channel_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->if_restore_gmsvr_channel_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_check_pthread_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->if_check_pthread_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_restore_dpsdk_channel_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->if_restore_dpsdk_channel_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_restore_user_info_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->if_restore_user_info_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "log_priority_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->log_priority_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "expire_time_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->expire_time_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "table_node_num_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->table_node_num_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "dp_config_path_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->dp_config_path_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "report_statistic_time_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->report_statistic_time_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "restrict_file_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->restrict_file_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "max_package_size_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_package_size_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_rpcode_with_key_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->if_rpcode_with_key_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "rpcode_keyinfo_key_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->rpcode_keyinfo_key_, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_open_cs_channel_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->if_open_cs_channel_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "total_max_send_count_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->total_max_send_count_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "if_check_rpcode_hash_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->if_check_rpcode_hash_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "gamesvr_channel_num_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->gamesvr_channel_num_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "channel_recv_times_");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->channel_recv_times_);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("crypt_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->crypt_checker_conf_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("crypt_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("sanlix_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->sanlix_checker_conf_.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("sanlix_checker_conf_");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}
