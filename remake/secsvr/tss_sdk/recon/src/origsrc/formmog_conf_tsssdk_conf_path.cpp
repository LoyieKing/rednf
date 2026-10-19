// Reconstruction of src/protocol/formmog/formmog_conf_tsssdk_conf_path.cpp
// (tag 394) from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// 13 symbols belong to the CU (cu_symbols.py): the 12
// tss_sdk_conf_path_desc::path_config:: functions plus the CU's weak
// TdrOutStream::textize copy at 0x000ee1f0 (0x40 bytes, the same weak
// definition the conf CU emits; see formmog_conf_tsssdk_conf.cpp and
// tsf4g_tdr_compat_cfg_conf.h).
//
// The ORIG emits:
//   * construct(): memset(this->config_path, 0, 1024) and TDR_NO_ERROR.
//   * entryFromXml(): reader.getEntryValue("config_path"); missing value ->
//     config_path[0] = 0; present value -> strlen(value) > 1023 returns
//     TDR_ERR_STR_LEN_TOO_BIG (-3), else strncpy(config_path, value, 1024).
//   * entryToListXml(): openNode/textizeStr(config_path, true)/closeNode, with
//     the writer.state() checks (the ORIG reloads writer._state at every
//     openNode/closeNode site).
//   * entryToAttrXml(): openAttr/textizeStr(config_path, false)/closeAttr.
//   * toXml(): openSimple/closeSimple("path_config").
//   * the fromXml*/toXml* wrappers of the TDR XML generator (TdrXmlReader,
//     TdrXmlWriter, TdrXmlFile::loadState()/content(), declare("GBK"),
//     getUsedSize()).
//
// The root element name is "path_config"; the single field name is
// "config_path" (both literals are PRESENT in the ORIG).

#include "formmog_conf_tsssdk_conf_path.h"

#include <string.h>

namespace tss_sdk_conf_path_desc {

using namespace tsf4g_tdr;

TdrError::ErrorType path_config::construct() {
    memset(this->config_path, 0, sizeof(this->config_path));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType path_config::fromXmlFile(const char *file,
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

TdrError::ErrorType path_config::fromXmlBuffer(const char *buffer,
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

TdrError::ErrorType path_config::fromXmlString(char *string,
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

TdrError::ErrorType path_config::fromXml(TdrXmlReader &reader,
                                         unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("path_config") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("path_config");
        }
    }
    return ret;
}

TdrError::ErrorType path_config::entryFromXml(TdrXmlReader &reader,
                                              unsigned int cutVer) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    const char *value4config_path = reader.getEntryValue("config_path");
    if (value4config_path == NULL) {
        this->config_path[0] = 0;
    } else {
        const size_t length4config_path = strlen(value4config_path);
        if (length4config_path > 1023) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->config_path, value4config_path, 1024);
    }
    return ret;
}

TdrError::ErrorType path_config::toXmlFile(FILE *fp, TdrXmlFormat format,
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

TdrError::ErrorType path_config::toXmlFile(const char *file,
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

TdrError::ErrorType path_config::toXmlBuffer(char *buffer, unsigned int size,
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

TdrError::ErrorType path_config::toXml(TdrXmlWriter &writer,
                                       TdrXmlFormat format,
                                       unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openSimple("path_config");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("path_config");
        }
    }
    return ret;
}

TdrError::ErrorType path_config::entryToListXml(TdrXmlWriter &writer,
                                                unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openNode("config_path");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    ret = writer.textizeStr(this->config_path, true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeNode("config_path");
    if (writer.state() != TdrError::TDR_NO_ERROR) return writer.state();
    return ret;
}

TdrError::ErrorType path_config::entryToAttrXml(TdrXmlWriter &writer,
                                                unsigned int cutVer) const {
    TdrError::ErrorType ret = writer.openAttr("config_path");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.textizeStr(this->config_path, false);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = writer.closeAttr("config_path");
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

} // namespace tss_sdk_conf_path_desc
