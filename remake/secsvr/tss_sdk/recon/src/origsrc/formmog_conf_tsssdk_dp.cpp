// Reconstruction of src/protocol/formmog/formmog_conf_tsssdk_dp.cpp (tag 394)
// from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// This TU provides exactly the 228 tss_dp_comm symbols the ORIG linker
// attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_conf_tsssdk_dp.cpp
// (cu_symbols.py), plus the CU's two out-of-line tsf4g_tdr weak definitions
// (TdrXmlWriter::textize 0x000ee1b0, TdrOutStream::textize 0x000ee1f0, both
// 0x40 bytes) which come from the inline bodies in tsf4g_tdr_compat_cfg.h.
//
// Per class the twelve methods are the generator's standard set:
// construct / fromXmlFile / fromXmlBuffer / fromXmlString / fromXml /
// entryFromXml / toXmlFile(const char*) / toXmlFile(FILE*) / toXmlBuffer /
// toXml / entryToListXml / entryToAttrXml.
//
// Field order, XML entry names, parse calls, defaults, limits and the writer's
// format strings are the values read out of the ORIG code of each function
// (AGENT_REPORT_genconf_dp.md): attributes use " %s=\"" with
// "0x%02x"/"%d"/"%u" for uint8/uint16/uint32, list entries use
// "%s<%s> "/">\n%s<%s> " + "<fmt> " + "</%s>\n", strings use
// textizeStr(str, false/true), and nested objects use
// openComplex/closeComplex with the child's entry*Xml at cutVer 1.

#include "formmog_conf_tsssdk_dp.h"

#include <string.h>

namespace tss_dp_comm {

using namespace tsf4g_tdr;


// ------------------------------------------------------------- DPGameCmdStruct

TdrError::ErrorType DPGameCmdStruct::construct() {
    memset(this, 0, sizeof(DPGameCmdStruct));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPGameCmdStruct::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPGameCmdStruct::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPGameCmdStruct::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPGameCmdStruct::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPGameCmdStruct") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPGameCmdStruct");
        }
    }
    return ret;
}

TdrError::ErrorType DPGameCmdStruct::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPGameCmdStruct::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPGameCmdStruct::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPGameCmdStruct::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPGameCmdStruct");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPGameCmdStruct");
        }
    }
    return ret;
}

TdrError::ErrorType DPGameCmdStruct::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4cmd_id_type;
    const char *value4cmd_id_offset;

    value4cmd_id_type = reader.getEntryValue("cmd_id_type");
    if (value4cmd_id_type == NULL) {
        this->cmd_id_type = 0;
    } else {
        ret = TdrParse::parseUInt8(this->cmd_id_type, value4cmd_id_type, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_id_offset = reader.getEntryValue("cmd_id_offset");
    if (value4cmd_id_offset == NULL) {
        this->cmd_id_offset = 0;
    } else {
        ret = TdrParse::parseUInt8(this->cmd_id_offset, value4cmd_id_offset, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPGameCmdStruct::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_id_type");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_id_type");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->cmd_id_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_id_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_id_offset");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_id_offset");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->cmd_id_offset);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_id_offset");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPGameCmdStruct::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "cmd_id_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->cmd_id_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_id_offset");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->cmd_id_offset);
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

// ------------------------------------------------------------- DPValueArray_8

TdrError::ErrorType DPValueArray_8::construct() {
    memset(this, 0, sizeof(DPValueArray_8));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPValueArray_8::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPValueArray_8::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPValueArray_8::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPValueArray_8::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPValueArray_8") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPValueArray_8");
        }
    }
    return ret;
}

TdrError::ErrorType DPValueArray_8::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPValueArray_8::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPValueArray_8::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPValueArray_8::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPValueArray_8");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPValueArray_8");
        }
    }
    return ret;
}

TdrError::ErrorType DPValueArray_8::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4length;
    const char *value4values;
    unsigned int tempCount4values;
    unsigned short values_i;

    value4length = reader.getEntryValue("length");
    if (value4length == NULL) {
        this->length = 8;
    } else {
        ret = TdrParse::parseUInt16(this->length, value4length, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->length > 8) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    value4values = reader.getNodeValue("values");
    if (value4values == NULL) {
        this->length = 0;
    } else {
        tempCount4values = 0;
        ret = TdrParse::parseUInt32(this->values, this->length, value4values, &tempCount4values, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        this->length = (uint16_t)tempCount4values;
    }
    return ret;
}

TdrError::ErrorType DPValueArray_8::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "length");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "length");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPValueArray_8::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;

    ret = writer._out.textize(" %s=\"", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

// ------------------------------------------------------------- DPValueArray_128

TdrError::ErrorType DPValueArray_128::construct() {
    memset(this, 0, sizeof(DPValueArray_128));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPValueArray_128::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPValueArray_128::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPValueArray_128::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPValueArray_128::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPValueArray_128") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPValueArray_128");
        }
    }
    return ret;
}

TdrError::ErrorType DPValueArray_128::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPValueArray_128::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPValueArray_128::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPValueArray_128::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPValueArray_128");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPValueArray_128");
        }
    }
    return ret;
}

TdrError::ErrorType DPValueArray_128::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4length;
    const char *value4values;
    unsigned int tempCount4values;
    unsigned short values_i;

    value4length = reader.getEntryValue("length");
    if (value4length == NULL) {
        this->length = 128;
    } else {
        ret = TdrParse::parseUInt16(this->length, value4length, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->length > 128) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    value4values = reader.getNodeValue("values");
    if (value4values == NULL) {
        this->length = 0;
    } else {
        tempCount4values = 0;
        ret = TdrParse::parseUInt32(this->values, this->length, value4values, &tempCount4values, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        this->length = (uint16_t)tempCount4values;
    }
    return ret;
}

TdrError::ErrorType DPValueArray_128::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "length");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "length");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPValueArray_128::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;

    ret = writer._out.textize(" %s=\"", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

// ------------------------------------------------------------- DPCmdLists

TdrError::ErrorType DPCmdLists::construct() {
    memset(this, 0, sizeof(DPCmdLists));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPCmdLists::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCmdLists::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCmdLists::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCmdLists::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPCmdLists") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPCmdLists");
        }
    }
    return ret;
}

TdrError::ErrorType DPCmdLists::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPCmdLists::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCmdLists::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCmdLists::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPCmdLists");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPCmdLists");
        }
    }
    return ret;
}

TdrError::ErrorType DPCmdLists::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4cur_posi;
    const char *value4lists_count;
    const char *value4cur_crc;
    const char *value4cmd_lists;
    unsigned short cmd_lists_i;

    value4cur_posi = reader.getEntryValue("cur_posi");
    if (value4cur_posi == NULL) {
        this->cur_posi = 0;
    } else {
        ret = TdrParse::parseUInt16(this->cur_posi, value4cur_posi, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4lists_count = reader.getEntryValue("lists_count");
    if (value4lists_count == NULL) {
        this->lists_count = 4;
    } else {
        ret = TdrParse::parseUInt16(this->lists_count, value4lists_count, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->lists_count > 4) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    value4cur_crc = reader.getEntryValue("cur_crc");
    if (value4cur_crc == NULL) {
        this->cur_crc = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cur_crc, value4cur_crc, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (this->lists_count > 4) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->lists_count != 0) {
        value4cmd_lists = "cmd_lists";
        for (cmd_lists_i = 0; cmd_lists_i < this->lists_count; cmd_lists_i++) {
            if (reader.stepIn(value4cmd_lists) != TdrXmlReader::WS_NORMAL) {
                this->lists_count = (uint16_t)cmd_lists_i;
                return ret;
            }
            ret = this->cmd_lists[cmd_lists_i].entryFromXml(reader, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            reader.stepOut(value4cmd_lists);
        }
    }
    return ret;
}

TdrError::ErrorType DPCmdLists::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short cmd_lists_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cur_posi");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cur_posi");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->cur_posi);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cur_posi");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "lists_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "lists_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->lists_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "lists_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cur_crc");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cur_crc");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cur_crc);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cur_crc");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->lists_count > 4) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->lists_count != 0) {
        for (cmd_lists_i = 0; cmd_lists_i < this->lists_count; cmd_lists_i++) {
            ret = writer.openComplex("cmd_lists");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->cmd_lists[cmd_lists_i].entryToListXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("cmd_lists");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    return ret;
}

TdrError::ErrorType DPCmdLists::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short cmd_lists_i;

    ret = writer._out.textize(" %s=\"", "cur_posi");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->cur_posi);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "lists_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->lists_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cur_crc");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cur_crc);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->lists_count > 4) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->lists_count != 0) {
        for (cmd_lists_i = 0; cmd_lists_i < this->lists_count; cmd_lists_i++) {
            ret = writer.openComplex("cmd_lists");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->cmd_lists[cmd_lists_i].entryToAttrXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("cmd_lists");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    return ret;
}

// ------------------------------------------------------------- DPSubCmdList

TdrError::ErrorType DPSubCmdList::construct() {
    memset(this, 0, sizeof(DPSubCmdList));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPSubCmdList::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPSubCmdList::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPSubCmdList::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPSubCmdList::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPSubCmdList") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPSubCmdList");
        }
    }
    return ret;
}

TdrError::ErrorType DPSubCmdList::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPSubCmdList::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPSubCmdList::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPSubCmdList::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPSubCmdList");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPSubCmdList");
        }
    }
    return ret;
}

TdrError::ErrorType DPSubCmdList::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4is_accept;
    const char *value4cmd_id_type;
    const char *value4cmd_id;

    value4is_accept = reader.getEntryValue("is_accept");
    if (value4is_accept == NULL) {
        this->is_accept = 0;
    } else {
        ret = TdrParse::parseUInt8(this->is_accept, value4is_accept, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_id_type = reader.getEntryValue("cmd_id_type");
    if (value4cmd_id_type == NULL) {
        this->cmd_id_type = 0;
    } else {
        ret = TdrParse::parseUInt8(this->cmd_id_type, value4cmd_id_type, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (reader.stepIn("dp_subcmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_subcmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_subcmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_subcmd_hack");
    }
    value4cmd_id = reader.getEntryValue("cmd_id");
    if (value4cmd_id == NULL) {
        this->cmd_id = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_id, value4cmd_id, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (reader.stepIn("subcmd_list") != TdrXmlReader::WS_NORMAL) {
        ret = this->subcmd_list.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->subcmd_list.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("subcmd_list");
    }
    return ret;
}

TdrError::ErrorType DPSubCmdList::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_accept");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_accept");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_accept);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_accept");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_id_type");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_id_type");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->cmd_id_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_id_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("dp_subcmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_subcmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_subcmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_id");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_id");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_id);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_id");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("subcmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->subcmd_list.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("subcmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPSubCmdList::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "is_accept");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_accept);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_id_type");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->cmd_id_type);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_id");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_id);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("dp_subcmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_subcmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_subcmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("subcmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->subcmd_list.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("subcmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

// ------------------------------------------------------------- DPHackFragment

TdrError::ErrorType DPHackFragment::construct() {
    memset(this, 0, sizeof(DPHackFragment));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPHackFragment::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPHackFragment::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPHackFragment::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPHackFragment::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPHackFragment") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPHackFragment");
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFragment::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPHackFragment::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPHackFragment::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPHackFragment::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPHackFragment");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPHackFragment");
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFragment::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4cmd_start;
    const char *value4cmd_end;
    const char *value4hack_list;
    unsigned int hack_list_i;

    value4cmd_start = reader.getEntryValue("cmd_start");
    if (value4cmd_start == NULL) {
        this->cmd_start = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_start, value4cmd_start, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_end = reader.getEntryValue("cmd_end");
    if (value4cmd_end == NULL) {
        this->cmd_end = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_end, value4cmd_end, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4hack_list = reader.getNodeValue("hack_list");
    if (value4hack_list == NULL) {
        memset(this->hack_list, 0, sizeof(this->hack_list));
    } else {
        ret = TdrParse::parseUInt8(this->hack_list, 8, value4hack_list, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFragment::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short hack_list_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_start");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_start");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_start);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_start");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_end");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_end");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_end);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_end");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "hack_list");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "hack_list");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (hack_list_i = 0; hack_list_i < 8; hack_list_i++) {
        ret = writer.textize("0x%02x ", this->hack_list[hack_list_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "hack_list");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPHackFragment::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short hack_list_i;

    ret = writer._out.textize(" %s=\"", "cmd_start");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_start);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_end");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_end);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "hack_list");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "hack_list");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (hack_list_i = 0; hack_list_i < 8; hack_list_i++) {
        ret = writer.textize("0x%02x ", this->hack_list[hack_list_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "hack_list");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

// ------------------------------------------------------------- DPHackFilter

TdrError::ErrorType DPHackFilter::construct() {
    memset(this, 0, sizeof(DPHackFilter));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPHackFilter::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPHackFilter::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPHackFilter::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPHackFilter::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPHackFilter") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPHackFilter");
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFilter::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPHackFilter::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPHackFilter::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPHackFilter::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPHackFilter");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPHackFilter");
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFilter::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4fragment_count;
    const char *value4hack_fragments;
    unsigned short hack_fragments_i;

    value4fragment_count = reader.getEntryValue("fragment_count");
    if (value4fragment_count == NULL) {
        this->fragment_count = 128;
    } else {
        ret = TdrParse::parseUInt16(this->fragment_count, value4fragment_count, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->fragment_count > 128) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    if (this->fragment_count > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->fragment_count != 0) {
        value4hack_fragments = "hack_fragments";
        for (hack_fragments_i = 0; hack_fragments_i < this->fragment_count; hack_fragments_i++) {
            if (reader.stepIn(value4hack_fragments) != TdrXmlReader::WS_NORMAL) {
                this->fragment_count = (uint16_t)hack_fragments_i;
                return ret;
            }
            ret = this->hack_fragments[hack_fragments_i].entryFromXml(reader, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            reader.stepOut(value4hack_fragments);
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFilter::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short hack_fragments_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "fragment_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "fragment_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->fragment_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "fragment_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->fragment_count > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->fragment_count != 0) {
        for (hack_fragments_i = 0; hack_fragments_i < this->fragment_count; hack_fragments_i++) {
            ret = writer.openComplex("hack_fragments");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->hack_fragments[hack_fragments_i].entryToListXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("hack_fragments");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    return ret;
}

TdrError::ErrorType DPHackFilter::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short hack_fragments_i;

    ret = writer._out.textize(" %s=\"", "fragment_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->fragment_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->fragment_count > 128) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->fragment_count != 0) {
        for (hack_fragments_i = 0; hack_fragments_i < this->fragment_count; hack_fragments_i++) {
            ret = writer.openComplex("hack_fragments");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->hack_fragments[hack_fragments_i].entryToAttrXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("hack_fragments");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    return ret;
}

// ------------------------------------------------------------- DPLogConf

TdrError::ErrorType DPLogConf::construct() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    this->bill_level = 2;
    this->game_id = 0;
    strncpy(this->bill_path, "../log/", 256);
    strncpy(this->monitor_address, "10.129.139.156", 32);
    return ret;
}

TdrError::ErrorType DPLogConf::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPLogConf::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPLogConf::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPLogConf::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPLogConf") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPLogConf");
        }
    }
    return ret;
}

TdrError::ErrorType DPLogConf::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPLogConf::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPLogConf::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPLogConf::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPLogConf");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPLogConf");
        }
    }
    return ret;
}

TdrError::ErrorType DPLogConf::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4bill_level;
    const char *value4game_id;
    const char *value4bill_path;
    const char *value4monitor_address;

    value4bill_level = reader.getEntryValue("bill_level");
    if (value4bill_level == NULL) {
        this->bill_level = 2;
    } else {
        ret = TdrParse::parseUInt32(this->bill_level, value4bill_level, NULL, 2, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4game_id = reader.getEntryValue("game_id");
    if (value4game_id == NULL) {
        this->game_id = 0;
    } else {
        ret = TdrParse::parseUInt32(this->game_id, value4game_id, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4bill_path = reader.getEntryValue("bill_path");
    if (value4bill_path == NULL) {
        strncpy(this->bill_path, "../log/", 256);
    } else {
        const size_t length4bill_path = strlen(value4bill_path);
        if (length4bill_path > 255) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->bill_path, value4bill_path, 256);
    }
    value4monitor_address = reader.getEntryValue("monitor_address");
    if (value4monitor_address == NULL) {
        strncpy(this->monitor_address, "10.129.139.156", 32);
    } else {
        const size_t length4monitor_address = strlen(value4monitor_address);
        if (length4monitor_address > 31) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        strncpy(this->monitor_address, value4monitor_address, 32);
    }
    return ret;
}

TdrError::ErrorType DPLogConf::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "bill_level");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "bill_level");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->bill_level);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "bill_level");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "game_id");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "game_id");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->game_id);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "game_id");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "bill_path");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "bill_path");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->bill_path, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "bill_path");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "monitor_address");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "monitor_address");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textizeStr(this->monitor_address, true);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "monitor_address");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPLogConf::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "bill_level");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->bill_level);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "game_id");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->game_id);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "bill_path");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->bill_path, false);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "monitor_address");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textizeStr(this->monitor_address, false);
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

// ------------------------------------------------------------- DPTimeConfig

TdrError::ErrorType DPTimeConfig::construct() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    this->max_init_pkg_timeout = 30;
    this->max_static_pkg_timeout = 200;
    this->max_confirm_pkg_timeout = 50;
    this->max_send_test_pkg_timeout = 200;
    this->check_time = 10;
    this->send_bin_file_gap = 54400;
    this->send_bin_code_gap = 12200;
    return ret;
}

TdrError::ErrorType DPTimeConfig::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPTimeConfig::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPTimeConfig::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPTimeConfig::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPTimeConfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPTimeConfig");
        }
    }
    return ret;
}

TdrError::ErrorType DPTimeConfig::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPTimeConfig::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPTimeConfig::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPTimeConfig::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPTimeConfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPTimeConfig");
        }
    }
    return ret;
}

TdrError::ErrorType DPTimeConfig::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4max_init_pkg_timeout;
    const char *value4max_static_pkg_timeout;
    const char *value4max_confirm_pkg_timeout;
    const char *value4max_send_test_pkg_timeout;
    const char *value4check_time;
    const char *value4send_bin_file_gap;
    const char *value4send_bin_code_gap;

    value4max_init_pkg_timeout = reader.getEntryValue("max_init_pkg_timeout");
    if (value4max_init_pkg_timeout == NULL) {
        this->max_init_pkg_timeout = 30;
    } else {
        ret = TdrParse::parseUInt32(this->max_init_pkg_timeout, value4max_init_pkg_timeout, NULL, 30, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4max_static_pkg_timeout = reader.getEntryValue("max_static_pkg_timeout");
    if (value4max_static_pkg_timeout == NULL) {
        this->max_static_pkg_timeout = 200;
    } else {
        ret = TdrParse::parseUInt32(this->max_static_pkg_timeout, value4max_static_pkg_timeout, NULL, 200, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4max_confirm_pkg_timeout = reader.getEntryValue("max_confirm_pkg_timeout");
    if (value4max_confirm_pkg_timeout == NULL) {
        this->max_confirm_pkg_timeout = 50;
    } else {
        ret = TdrParse::parseUInt32(this->max_confirm_pkg_timeout, value4max_confirm_pkg_timeout, NULL, 50, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4max_send_test_pkg_timeout = reader.getEntryValue("max_send_test_pkg_timeout");
    if (value4max_send_test_pkg_timeout == NULL) {
        this->max_send_test_pkg_timeout = 200;
    } else {
        ret = TdrParse::parseUInt32(this->max_send_test_pkg_timeout, value4max_send_test_pkg_timeout, NULL, 200, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4check_time = reader.getEntryValue("check_time");
    if (value4check_time == NULL) {
        this->check_time = 10;
    } else {
        ret = TdrParse::parseUInt32(this->check_time, value4check_time, NULL, 10, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4send_bin_file_gap = reader.getEntryValue("send_bin_file_gap");
    if (value4send_bin_file_gap == NULL) {
        this->send_bin_file_gap = 54400;
    } else {
        ret = TdrParse::parseUInt32(this->send_bin_file_gap, value4send_bin_file_gap, NULL, 54400, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4send_bin_code_gap = reader.getEntryValue("send_bin_code_gap");
    if (value4send_bin_code_gap == NULL) {
        this->send_bin_code_gap = 12200;
    } else {
        ret = TdrParse::parseUInt32(this->send_bin_code_gap, value4send_bin_code_gap, NULL, 12200, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPTimeConfig::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_init_pkg_timeout");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_init_pkg_timeout");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_init_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_init_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_static_pkg_timeout");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_static_pkg_timeout");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_static_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_static_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_confirm_pkg_timeout");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_confirm_pkg_timeout");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_confirm_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_confirm_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_send_test_pkg_timeout");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_send_test_pkg_timeout");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_send_test_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_send_test_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "check_time");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "check_time");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->check_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "check_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "send_bin_file_gap");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "send_bin_file_gap");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->send_bin_file_gap);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "send_bin_file_gap");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "send_bin_code_gap");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "send_bin_code_gap");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->send_bin_code_gap);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "send_bin_code_gap");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPTimeConfig::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "max_init_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_init_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "max_static_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_static_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "max_confirm_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_confirm_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "max_send_test_pkg_timeout");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_send_test_pkg_timeout);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "check_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->check_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "send_bin_file_gap");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->send_bin_file_gap);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "send_bin_code_gap");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->send_bin_code_gap);
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

// ------------------------------------------------------------- DPConfigBase

TdrError::ErrorType DPConfigBase::construct() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    this->is_use_dp = 1;
    this->is_restore_usr_space = 0;
    this->is_use_client_version = 0;
    this->disable_anticheat_pkg = 0;
    this->use_punish_notify = 0;
    this->dp_record_using_time = 0;
    return ret;
}

TdrError::ErrorType DPConfigBase::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPConfigBase::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPConfigBase::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPConfigBase::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPConfigBase") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPConfigBase");
        }
    }
    return ret;
}

TdrError::ErrorType DPConfigBase::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPConfigBase::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPConfigBase::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPConfigBase::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPConfigBase");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPConfigBase");
        }
    }
    return ret;
}

TdrError::ErrorType DPConfigBase::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4is_use_dp;
    const char *value4is_restore_usr_space;
    const char *value4is_use_client_version;
    const char *value4disable_anticheat_pkg;
    const char *value4use_punish_notify;
    const char *value4dp_record_using_time;

    value4is_use_dp = reader.getEntryValue("is_use_dp");
    if (value4is_use_dp == NULL) {
        this->is_use_dp = 1;
    } else {
        ret = TdrParse::parseUInt8(this->is_use_dp, value4is_use_dp, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4is_restore_usr_space = reader.getEntryValue("is_restore_usr_space");
    if (value4is_restore_usr_space == NULL) {
        this->is_restore_usr_space = 0;
    } else {
        ret = TdrParse::parseUInt8(this->is_restore_usr_space, value4is_restore_usr_space, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4is_use_client_version = reader.getEntryValue("is_use_client_version");
    if (value4is_use_client_version == NULL) {
        this->is_use_client_version = 0;
    } else {
        ret = TdrParse::parseUInt8(this->is_use_client_version, value4is_use_client_version, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4disable_anticheat_pkg = reader.getEntryValue("disable_anticheat_pkg");
    if (value4disable_anticheat_pkg == NULL) {
        this->disable_anticheat_pkg = 0;
    } else {
        ret = TdrParse::parseUInt8(this->disable_anticheat_pkg, value4disable_anticheat_pkg, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4use_punish_notify = reader.getEntryValue("use_punish_notify");
    if (value4use_punish_notify == NULL) {
        this->use_punish_notify = 0;
    } else {
        ret = TdrParse::parseUInt8(this->use_punish_notify, value4use_punish_notify, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4dp_record_using_time = reader.getEntryValue("dp_record_using_time");
    if (value4dp_record_using_time == NULL) {
        this->dp_record_using_time = 0;
    } else {
        ret = TdrParse::parseUInt8(this->dp_record_using_time, value4dp_record_using_time, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPConfigBase::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_use_dp");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_use_dp");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_use_dp);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_use_dp");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_restore_usr_space");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_restore_usr_space");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_restore_usr_space);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_restore_usr_space");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "is_use_client_version");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "is_use_client_version");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->is_use_client_version);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "is_use_client_version");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "disable_anticheat_pkg");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "disable_anticheat_pkg");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->disable_anticheat_pkg);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "disable_anticheat_pkg");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "use_punish_notify");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "use_punish_notify");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->use_punish_notify);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "use_punish_notify");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "dp_record_using_time");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "dp_record_using_time");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->dp_record_using_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "dp_record_using_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPConfigBase::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "is_use_dp");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_use_dp);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_restore_usr_space");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_restore_usr_space);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "is_use_client_version");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->is_use_client_version);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "disable_anticheat_pkg");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->disable_anticheat_pkg);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "use_punish_notify");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->use_punish_notify);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "dp_record_using_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->dp_record_using_time);
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

// ------------------------------------------------------------- DPProtoConfig

TdrError::ErrorType DPProtoConfig::construct() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    this->dp_fragmentation_size = 61000;
    this->dp_max_encrypt_size = 15001;
    this->use_sequence_check = 1;
    this->proto_check_switch = 1;
    this->seq_check_switch = 1;
    this->raw_check_switch = 1;
    this->dp_decrypt_fail_protect = 0;
    this->explicit_use_dynamic_encrypt = 0;
    return ret;
}

TdrError::ErrorType DPProtoConfig::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPProtoConfig::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPProtoConfig::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPProtoConfig::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPProtoConfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPProtoConfig");
        }
    }
    return ret;
}

TdrError::ErrorType DPProtoConfig::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPProtoConfig::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPProtoConfig::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPProtoConfig::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPProtoConfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPProtoConfig");
        }
    }
    return ret;
}

TdrError::ErrorType DPProtoConfig::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4dp_fragmentation_size;
    const char *value4dp_max_encrypt_size;
    const char *value4use_sequence_check;
    const char *value4proto_check_switch;
    const char *value4seq_check_switch;
    const char *value4raw_check_switch;
    const char *value4dp_decrypt_fail_protect;
    const char *value4explicit_use_dynamic_encrypt;

    value4dp_fragmentation_size = reader.getEntryValue("dp_fragmentation_size");
    if (value4dp_fragmentation_size == NULL) {
        this->dp_fragmentation_size = 61000;
    } else {
        ret = TdrParse::parseUInt16(this->dp_fragmentation_size, value4dp_fragmentation_size, NULL, 61000, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4dp_max_encrypt_size = reader.getEntryValue("dp_max_encrypt_size");
    if (value4dp_max_encrypt_size == NULL) {
        this->dp_max_encrypt_size = 14999;
    } else {
        ret = TdrParse::parseUInt16(this->dp_max_encrypt_size, value4dp_max_encrypt_size, NULL, 14999, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4use_sequence_check = reader.getEntryValue("use_sequence_check");
    if (value4use_sequence_check == NULL) {
        this->use_sequence_check = 1;
    } else {
        ret = TdrParse::parseUInt8(this->use_sequence_check, value4use_sequence_check, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4proto_check_switch = reader.getEntryValue("proto_check_switch");
    if (value4proto_check_switch == NULL) {
        this->proto_check_switch = 1;
    } else {
        ret = TdrParse::parseUInt8(this->proto_check_switch, value4proto_check_switch, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4seq_check_switch = reader.getEntryValue("seq_check_switch");
    if (value4seq_check_switch == NULL) {
        this->seq_check_switch = 1;
    } else {
        ret = TdrParse::parseUInt8(this->seq_check_switch, value4seq_check_switch, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4raw_check_switch = reader.getEntryValue("raw_check_switch");
    if (value4raw_check_switch == NULL) {
        this->raw_check_switch = 1;
    } else {
        ret = TdrParse::parseUInt8(this->raw_check_switch, value4raw_check_switch, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4dp_decrypt_fail_protect = reader.getEntryValue("dp_decrypt_fail_protect");
    if (value4dp_decrypt_fail_protect == NULL) {
        this->dp_decrypt_fail_protect = 0;
    } else {
        ret = TdrParse::parseUInt8(this->dp_decrypt_fail_protect, value4dp_decrypt_fail_protect, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4explicit_use_dynamic_encrypt = reader.getEntryValue("explicit_use_dynamic_encrypt");
    if (value4explicit_use_dynamic_encrypt == NULL) {
        this->explicit_use_dynamic_encrypt = 0;
    } else {
        ret = TdrParse::parseUInt8(this->explicit_use_dynamic_encrypt, value4explicit_use_dynamic_encrypt, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPProtoConfig::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "dp_fragmentation_size");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "dp_fragmentation_size");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->dp_fragmentation_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "dp_fragmentation_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "dp_max_encrypt_size");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "dp_max_encrypt_size");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->dp_max_encrypt_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "dp_max_encrypt_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "use_sequence_check");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "use_sequence_check");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->use_sequence_check);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "use_sequence_check");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "proto_check_switch");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "proto_check_switch");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->proto_check_switch);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "proto_check_switch");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "seq_check_switch");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "seq_check_switch");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->seq_check_switch);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "seq_check_switch");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "raw_check_switch");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "raw_check_switch");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->raw_check_switch);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "raw_check_switch");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "dp_decrypt_fail_protect");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "dp_decrypt_fail_protect");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->dp_decrypt_fail_protect);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "dp_decrypt_fail_protect");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "explicit_use_dynamic_encrypt");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "explicit_use_dynamic_encrypt");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->explicit_use_dynamic_encrypt);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "explicit_use_dynamic_encrypt");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPProtoConfig::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "dp_fragmentation_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->dp_fragmentation_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "dp_max_encrypt_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->dp_max_encrypt_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "use_sequence_check");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->use_sequence_check);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "proto_check_switch");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->proto_check_switch);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "seq_check_switch");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->seq_check_switch);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "raw_check_switch");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->raw_check_switch);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "dp_decrypt_fail_protect");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->dp_decrypt_fail_protect);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "explicit_use_dynamic_encrypt");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->explicit_use_dynamic_encrypt);
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

// ------------------------------------------------------------- DPCaptchaMethod

TdrError::ErrorType DPCaptchaMethod::construct() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    this->max_timeout_count = 2;
    this->min_try_count = 10;
    this->min_try_time = 1800;
    this->waitting_response = 10;
    return ret;
}

TdrError::ErrorType DPCaptchaMethod::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCaptchaMethod::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCaptchaMethod::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCaptchaMethod::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPCaptchaMethod") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPCaptchaMethod");
        }
    }
    return ret;
}

TdrError::ErrorType DPCaptchaMethod::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPCaptchaMethod::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCaptchaMethod::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCaptchaMethod::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPCaptchaMethod");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPCaptchaMethod");
        }
    }
    return ret;
}

TdrError::ErrorType DPCaptchaMethod::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4max_timeout_count;
    const char *value4min_try_count;
    const char *value4min_try_time;
    const char *value4waitting_response;

    value4max_timeout_count = reader.getEntryValue("max_timeout_count");
    if (value4max_timeout_count == NULL) {
        this->max_timeout_count = 2;
    } else {
        ret = TdrParse::parseUInt32(this->max_timeout_count, value4max_timeout_count, NULL, 2, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4min_try_count = reader.getEntryValue("min_try_count");
    if (value4min_try_count == NULL) {
        this->min_try_count = 10;
    } else {
        ret = TdrParse::parseUInt32(this->min_try_count, value4min_try_count, NULL, 10, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4min_try_time = reader.getEntryValue("min_try_time");
    if (value4min_try_time == NULL) {
        this->min_try_time = 1800;
    } else {
        ret = TdrParse::parseUInt32(this->min_try_time, value4min_try_time, NULL, 1800, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4waitting_response = reader.getEntryValue("waitting_response");
    if (value4waitting_response == NULL) {
        this->waitting_response = 10;
    } else {
        ret = TdrParse::parseUInt32(this->waitting_response, value4waitting_response, NULL, 10, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPCaptchaMethod::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "max_timeout_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "max_timeout_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->max_timeout_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "max_timeout_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "min_try_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "min_try_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->min_try_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "min_try_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "min_try_time");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "min_try_time");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->min_try_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "min_try_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "waitting_response");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "waitting_response");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->waitting_response);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "waitting_response");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPCaptchaMethod::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "max_timeout_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->max_timeout_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "min_try_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->min_try_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "min_try_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->min_try_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "waitting_response");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->waitting_response);
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

// ------------------------------------------------------------- DPBanMode

TdrError::ErrorType DPBanMode::construct() {
    memset(this, 0, sizeof(DPBanMode));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPBanMode::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPBanMode::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPBanMode::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPBanMode::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPBanMode") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPBanMode");
        }
    }
    return ret;
}

TdrError::ErrorType DPBanMode::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPBanMode::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPBanMode::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPBanMode::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPBanMode");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPBanMode");
        }
    }
    return ret;
}

TdrError::ErrorType DPBanMode::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4length;
    const char *value4values;
    unsigned int tempCount4values;
    unsigned short values_i;

    value4length = reader.getEntryValue("length");
    if (value4length == NULL) {
        this->length = 32;
    } else {
        ret = TdrParse::parseUInt16(this->length, value4length, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->length > 32) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    value4values = reader.getNodeValue("values");
    if (value4values == NULL) {
        this->length = 0;
    } else {
        tempCount4values = 0;
        ret = TdrParse::parseUInt32(this->values, this->length, value4values, &tempCount4values, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        this->length = (uint16_t)tempCount4values;
    }
    return ret;
}

TdrError::ErrorType DPBanMode::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "length");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "length");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 32) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPBanMode::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;

    ret = writer._out.textize(" %s=\"", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 32) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

// ------------------------------------------------------------- DPDropCmd

TdrError::ErrorType DPDropCmd::construct() {
    memset(this, 0, sizeof(DPDropCmd));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPDropCmd::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPDropCmd::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPDropCmd::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPDropCmd::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPDropCmd") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPDropCmd");
        }
    }
    return ret;
}

TdrError::ErrorType DPDropCmd::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPDropCmd::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPDropCmd::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPDropCmd::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPDropCmd");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPDropCmd");
        }
    }
    return ret;
}

TdrError::ErrorType DPDropCmd::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4length;
    const char *value4values;
    unsigned int tempCount4values;
    unsigned short values_i;
    const char *value4actions;
    unsigned int actions_i;

    value4length = reader.getEntryValue("length");
    if (value4length == NULL) {
        this->length = 8;
    } else {
        ret = TdrParse::parseUInt16(this->length, value4length, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->length > 8) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    value4values = reader.getNodeValue("values");
    if (value4values == NULL) {
        this->length = 0;
    } else {
        tempCount4values = 0;
        ret = TdrParse::parseUInt32(this->values, this->length, value4values, &tempCount4values, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        this->length = (uint16_t)tempCount4values;
    }
    value4actions = reader.getNodeValue("actions");
    if (value4actions == NULL) {
        memset(this->actions, 0, sizeof(this->actions));
    } else {
        ret = TdrParse::parseUInt8(this->actions, 8, value4actions, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPDropCmd::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;
    unsigned short actions_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "length");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "length");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "actions");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "actions");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (actions_i = 0; actions_i < 8; actions_i++) {
        ret = writer.textize("0x%02x ", this->actions[actions_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "actions");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPDropCmd::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short values_i;
    unsigned short actions_i;

    ret = writer._out.textize(" %s=\"", "length");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->length);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->length > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "values");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "values");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->length != 0) {
        for (values_i = 0; values_i < this->length; values_i++) {
            ret = writer.textize("%u ", this->values[values_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "values");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "actions");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "actions");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    for (actions_i = 0; actions_i < 8; actions_i++) {
        ret = writer.textize("0x%02x ", this->actions[actions_i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = writer._out.textize("</%s>\n", "actions");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

// ------------------------------------------------------------- DPDelayCmd

TdrError::ErrorType DPDelayCmd::construct() {
    TdrError::ErrorType ret;
    this->delay_cmd_time = 1;
    ret = this->safemode_drop_cmdlist.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPDelayCmd::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPDelayCmd::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPDelayCmd::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPDelayCmd::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPDelayCmd") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPDelayCmd");
        }
    }
    return ret;
}

TdrError::ErrorType DPDelayCmd::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPDelayCmd::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPDelayCmd::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPDelayCmd::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPDelayCmd");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPDelayCmd");
        }
    }
    return ret;
}

TdrError::ErrorType DPDelayCmd::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4delay_cmd_time;

    value4delay_cmd_time = reader.getEntryValue("delay_cmd_time");
    if (value4delay_cmd_time == NULL) {
        this->delay_cmd_time = 1;
    } else {
        ret = TdrParse::parseUInt8(this->delay_cmd_time, value4delay_cmd_time, NULL, 1, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (reader.stepIn("safemode_drop_cmdlist") != TdrXmlReader::WS_NORMAL) {
        ret = this->safemode_drop_cmdlist.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->safemode_drop_cmdlist.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("safemode_drop_cmdlist");
    }
    return ret;
}

TdrError::ErrorType DPDelayCmd::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "delay_cmd_time");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "delay_cmd_time");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->delay_cmd_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "delay_cmd_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("safemode_drop_cmdlist");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->safemode_drop_cmdlist.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("safemode_drop_cmdlist");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPDelayCmd::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "delay_cmd_time");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->delay_cmd_time);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("safemode_drop_cmdlist");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->safemode_drop_cmdlist.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("safemode_drop_cmdlist");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

// ------------------------------------------------------------- DPPropertyCmd

TdrError::ErrorType DPPropertyCmd::construct() {
    memset(this, 0, sizeof(DPPropertyCmd));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPPropertyCmd::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPPropertyCmd::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPPropertyCmd::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPPropertyCmd::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPPropertyCmd") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPPropertyCmd");
        }
    }
    return ret;
}

TdrError::ErrorType DPPropertyCmd::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPPropertyCmd::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPPropertyCmd::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPPropertyCmd::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPPropertyCmd");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPPropertyCmd");
        }
    }
    return ret;
}

TdrError::ErrorType DPPropertyCmd::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4cmd_role_uid;
    const char *value4cmd_create_team;
    const char *value4cmd_drop_item;
    const char *value4cmd_create_role;
    const char *value4cmd_enter_private_store;
    const char *value4cmd_trade_accept;
    const char *value4cmd_trade_confirm;
    const char *value4cmd_trade_put_item;
    const char *value4cmd_enchant_put_item;
    const char *value4cmd_enchant_confirm;
    const char *value4cmd_simple_mail;
    const char *value4cmd_complex_mail;

    value4cmd_role_uid = reader.getEntryValue("cmd_role_uid");
    if (value4cmd_role_uid == NULL) {
        this->cmd_role_uid = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_role_uid, value4cmd_role_uid, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_create_team = reader.getEntryValue("cmd_create_team");
    if (value4cmd_create_team == NULL) {
        this->cmd_create_team = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_create_team, value4cmd_create_team, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_drop_item = reader.getEntryValue("cmd_drop_item");
    if (value4cmd_drop_item == NULL) {
        this->cmd_drop_item = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_drop_item, value4cmd_drop_item, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_create_role = reader.getEntryValue("cmd_create_role");
    if (value4cmd_create_role == NULL) {
        this->cmd_create_role = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_create_role, value4cmd_create_role, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_enter_private_store = reader.getEntryValue("cmd_enter_private_store");
    if (value4cmd_enter_private_store == NULL) {
        this->cmd_enter_private_store = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_enter_private_store, value4cmd_enter_private_store, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_trade_accept = reader.getEntryValue("cmd_trade_accept");
    if (value4cmd_trade_accept == NULL) {
        this->cmd_trade_accept = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_trade_accept, value4cmd_trade_accept, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_trade_confirm = reader.getEntryValue("cmd_trade_confirm");
    if (value4cmd_trade_confirm == NULL) {
        this->cmd_trade_confirm = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_trade_confirm, value4cmd_trade_confirm, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_trade_put_item = reader.getEntryValue("cmd_trade_put_item");
    if (value4cmd_trade_put_item == NULL) {
        this->cmd_trade_put_item = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_trade_put_item, value4cmd_trade_put_item, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_enchant_put_item = reader.getEntryValue("cmd_enchant_put_item");
    if (value4cmd_enchant_put_item == NULL) {
        this->cmd_enchant_put_item = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_enchant_put_item, value4cmd_enchant_put_item, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_enchant_confirm = reader.getEntryValue("cmd_enchant_confirm");
    if (value4cmd_enchant_confirm == NULL) {
        this->cmd_enchant_confirm = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_enchant_confirm, value4cmd_enchant_confirm, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_simple_mail = reader.getEntryValue("cmd_simple_mail");
    if (value4cmd_simple_mail == NULL) {
        this->cmd_simple_mail = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_simple_mail, value4cmd_simple_mail, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cmd_complex_mail = reader.getEntryValue("cmd_complex_mail");
    if (value4cmd_complex_mail == NULL) {
        this->cmd_complex_mail = 0;
    } else {
        ret = TdrParse::parseUInt32(this->cmd_complex_mail, value4cmd_complex_mail, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

TdrError::ErrorType DPPropertyCmd::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_role_uid");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_role_uid");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_role_uid);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_role_uid");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_create_team");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_create_team");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_create_team);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_create_team");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_drop_item");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_drop_item");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_drop_item);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_drop_item");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_create_role");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_create_role");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_create_role);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_create_role");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_enter_private_store");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_enter_private_store");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_enter_private_store);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_enter_private_store");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_trade_accept");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_trade_accept");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_trade_accept);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_trade_accept");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_trade_confirm");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_trade_confirm");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_trade_confirm);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_trade_confirm");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_trade_put_item");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_trade_put_item");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_trade_put_item);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_trade_put_item");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_enchant_put_item");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_enchant_put_item");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_enchant_put_item);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_enchant_put_item");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_enchant_confirm");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_enchant_confirm");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_enchant_confirm);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_enchant_confirm");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_simple_mail");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_simple_mail");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_simple_mail);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_simple_mail");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_complex_mail");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_complex_mail");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%u ", this->cmd_complex_mail);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_complex_mail");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    return ret;
}

TdrError::ErrorType DPPropertyCmd::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer._out.textize(" %s=\"", "cmd_role_uid");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_role_uid);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_create_team");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_create_team);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_drop_item");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_drop_item);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_create_role");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_create_role);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_enter_private_store");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_enter_private_store);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_trade_accept");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_trade_accept);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_trade_confirm");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_trade_confirm);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_trade_put_item");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_trade_put_item);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_enchant_put_item");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_enchant_put_item);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_enchant_confirm");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_enchant_confirm);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_simple_mail");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_simple_mail);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cmd_complex_mail");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%u", this->cmd_complex_mail);
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

// ------------------------------------------------------------- DPCodeInfo

TdrError::ErrorType DPCodeInfo::construct() {
    memset(this, 0, sizeof(DPCodeInfo));
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType DPCodeInfo::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCodeInfo::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCodeInfo::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCodeInfo::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPCodeInfo") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPCodeInfo");
        }
    }
    return ret;
}

TdrError::ErrorType DPCodeInfo::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPCodeInfo::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCodeInfo::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCodeInfo::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPCodeInfo");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPCodeInfo");
        }
    }
    return ret;
}

TdrError::ErrorType DPCodeInfo::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4cmd_list_buf_size;
    const char *value4cmd_list_buf;
    unsigned int tempCount4cmd_list_buf;
    unsigned short cmd_list_buf_i;
    const char *value4code_dir_count;
    const char *value4code_dir_path;
    const char *tempValue4code_dir_path;
    unsigned short code_dir_path_i;

    value4cmd_list_buf_size = reader.getEntryValue("cmd_list_buf_size");
    if (value4cmd_list_buf_size == NULL) {
        this->cmd_list_buf_size = 10240;
    } else {
        ret = TdrParse::parseUInt16(this->cmd_list_buf_size, value4cmd_list_buf_size, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->cmd_list_buf_size > 10240) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    value4cmd_list_buf = reader.getNodeValue("cmd_list_buf");
    if (value4cmd_list_buf == NULL) {
        this->cmd_list_buf_size = 0;
    } else {
        tempCount4cmd_list_buf = 0;
        ret = TdrParse::parseUInt8(this->cmd_list_buf, this->cmd_list_buf_size, value4cmd_list_buf, &tempCount4cmd_list_buf, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        this->cmd_list_buf_size = (uint16_t)tempCount4cmd_list_buf;
    }
    if (reader.stepIn("up_cmd_list") != TdrXmlReader::WS_NORMAL) {
        ret = this->up_cmd_list.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->up_cmd_list.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("up_cmd_list");
    }
    if (reader.stepIn("down_cmd_list") != TdrXmlReader::WS_NORMAL) {
        ret = this->down_cmd_list.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->down_cmd_list.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("down_cmd_list");
    }
    value4code_dir_count = reader.getEntryValue("code_dir_count");
    if (value4code_dir_count == NULL) {
        this->code_dir_count = 2;
    } else {
        ret = TdrParse::parseUInt16(this->code_dir_count, value4code_dir_count, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->code_dir_count > 2) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    if (this->code_dir_count > 2) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->code_dir_count != 0) {
        value4code_dir_path = "code_dir_path";
        for (code_dir_path_i = 0; code_dir_path_i < this->code_dir_count; code_dir_path_i++) {
            tempValue4code_dir_path = reader.getNodeValue(value4code_dir_path);
            if (tempValue4code_dir_path == NULL) {
                this->code_dir_count = (uint16_t)code_dir_path_i;
                return ret;
            }
            const size_t length4code_dir_path = strlen(tempValue4code_dir_path);
            if (length4code_dir_path > 255) {
                return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
            }
            strncpy(this->code_dir_path[code_dir_path_i], tempValue4code_dir_path, 256);
        }
    }
    return ret;
}

TdrError::ErrorType DPCodeInfo::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short cmd_list_buf_i;
    unsigned short code_dir_path_i;

    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_list_buf_size");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_list_buf_size");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->cmd_list_buf_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cmd_list_buf_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->cmd_list_buf_size > 10240) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_list_buf");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_list_buf");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->cmd_list_buf_size != 0) {
        for (cmd_list_buf_i = 0; cmd_list_buf_i < this->cmd_list_buf_size; cmd_list_buf_i++) {
            ret = writer.textize("0x%02x ", this->cmd_list_buf[cmd_list_buf_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "cmd_list_buf");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("up_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->up_cmd_list.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("up_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("down_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmd_list.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("down_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "code_dir_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "code_dir_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->code_dir_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "code_dir_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->code_dir_count > 2) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->code_dir_count != 0) {
        for (code_dir_path_i = 0; code_dir_path_i < this->code_dir_count; code_dir_path_i++) {
        if (writer._isPartial) {
            ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "code_dir_path");
            writer._isPartial = false;
            writer._state = ret;
            if (writer._state != TdrError::TDR_NO_ERROR) {
                return writer._state;
            }
        } else {
            ret = writer._out.textize("%s<%s> ", writer._indentStr, "code_dir_path");
            writer._state = ret;
            if (writer._state != TdrError::TDR_NO_ERROR) {
                return writer._state;
            }
        }
            ret = writer.textizeStr(this->code_dir_path[code_dir_path_i], true);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        ret = writer._out.textize("</%s>\n", "code_dir_path");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
        }
    }
    return ret;
}

TdrError::ErrorType DPCodeInfo::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short cmd_list_buf_i;
    unsigned short code_dir_path_i;

    ret = writer._out.textize(" %s=\"", "cmd_list_buf_size");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->cmd_list_buf_size);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "code_dir_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->code_dir_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->cmd_list_buf_size > 10240) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cmd_list_buf");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cmd_list_buf");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    if (this->cmd_list_buf_size != 0) {
        for (cmd_list_buf_i = 0; cmd_list_buf_i < this->cmd_list_buf_size; cmd_list_buf_i++) {
            ret = writer.textize("0x%02x ", this->cmd_list_buf[cmd_list_buf_i]);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer._out.textize("</%s>\n", "cmd_list_buf");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("up_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->up_cmd_list.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("up_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("down_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmd_list.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("down_cmd_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (this->code_dir_count > 2) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->code_dir_count != 0) {
        for (code_dir_path_i = 0; code_dir_path_i < this->code_dir_count; code_dir_path_i++) {
        if (writer._isPartial) {
            ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "code_dir_path");
            writer._isPartial = false;
            writer._state = ret;
            if (writer._state != TdrError::TDR_NO_ERROR) {
                return writer._state;
            }
        } else {
            ret = writer._out.textize("%s<%s> ", writer._indentStr, "code_dir_path");
            writer._state = ret;
            if (writer._state != TdrError::TDR_NO_ERROR) {
                return writer._state;
            }
        }
            ret = writer.textizeStr(this->code_dir_path[code_dir_path_i], true);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        ret = writer._out.textize("</%s>\n", "code_dir_path");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
        }
    }
    return ret;
}

// ------------------------------------------------------------- DPCmdHack

TdrError::ErrorType DPCmdHack::construct() {
    TdrError::ErrorType ret;
    ret = this->delay_cmd.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->safemode_to_client = 0;
    this->safemode_hint_client = 0;
    this->cf_room_qtalk_enable = 0;
    ret = this->up_cmd_hack.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmd_hack.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->raw_cmd_hack.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->captch_method.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    this->drop_subcmd_count = 0;
    for (unsigned int drop_subcmd_list_i = 0; drop_subcmd_list_i < 8; drop_subcmd_list_i++) {
        ret = this->drop_subcmd_list[drop_subcmd_list_i].construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    this->forward_subcmd_count = 0;
    for (unsigned int forward_subcmd_list_i = 0; forward_subcmd_list_i < 8; forward_subcmd_list_i++) {
        ret = this->forward_subcmd_list[forward_subcmd_list_i].construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    ret = this->ban_mode.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->drop_cmd.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->property_cmd.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPCmdHack::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCmdHack::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCmdHack::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPCmdHack::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPCmdHack") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPCmdHack");
        }
    }
    return ret;
}

TdrError::ErrorType DPCmdHack::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPCmdHack::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCmdHack::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPCmdHack::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPCmdHack");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPCmdHack");
        }
    }
    return ret;
}

TdrError::ErrorType DPCmdHack::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;
    const char *value4safemode_to_client;
    const char *value4safemode_hint_client;
    const char *value4cf_room_qtalk_enable;
    const char *value4drop_subcmd_count;
    const char *value4drop_subcmd_list;
    unsigned short drop_subcmd_list_i;
    const char *value4forward_subcmd_count;
    const char *value4forward_subcmd_list;
    unsigned short forward_subcmd_list_i;

    if (reader.stepIn("delay_cmd") != TdrXmlReader::WS_NORMAL) {
        ret = this->delay_cmd.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->delay_cmd.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("delay_cmd");
    }
    value4safemode_to_client = reader.getEntryValue("safemode_to_client");
    if (value4safemode_to_client == NULL) {
        this->safemode_to_client = 0;
    } else {
        ret = TdrParse::parseUInt8(this->safemode_to_client, value4safemode_to_client, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4safemode_hint_client = reader.getEntryValue("safemode_hint_client");
    if (value4safemode_hint_client == NULL) {
        this->safemode_hint_client = 0;
    } else {
        ret = TdrParse::parseUInt8(this->safemode_hint_client, value4safemode_hint_client, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    value4cf_room_qtalk_enable = reader.getEntryValue("cf_room_qtalk_enable");
    if (value4cf_room_qtalk_enable == NULL) {
        this->cf_room_qtalk_enable = 0;
    } else {
        ret = TdrParse::parseUInt8(this->cf_room_qtalk_enable, value4cf_room_qtalk_enable, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    if (reader.stepIn("up_cmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->up_cmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->up_cmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("up_cmd_hack");
    }
    if (reader.stepIn("down_cmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->down_cmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->down_cmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("down_cmd_hack");
    }
    if (reader.stepIn("raw_cmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->raw_cmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->raw_cmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("raw_cmd_hack");
    }
    if (reader.stepIn("captch_method") != TdrXmlReader::WS_NORMAL) {
        ret = this->captch_method.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->captch_method.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("captch_method");
    }
    value4drop_subcmd_count = reader.getEntryValue("drop_subcmd_count");
    if (value4drop_subcmd_count == NULL) {
        this->drop_subcmd_count = 8;
    } else {
        ret = TdrParse::parseUInt16(this->drop_subcmd_count, value4drop_subcmd_count, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->drop_subcmd_count > 8) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    if (this->drop_subcmd_count > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->drop_subcmd_count != 0) {
        value4drop_subcmd_list = "drop_subcmd_list";
        for (drop_subcmd_list_i = 0; drop_subcmd_list_i < this->drop_subcmd_count; drop_subcmd_list_i++) {
            if (reader.stepIn(value4drop_subcmd_list) != TdrXmlReader::WS_NORMAL) {
                this->drop_subcmd_count = (uint16_t)drop_subcmd_list_i;
                return ret;
            }
            ret = this->drop_subcmd_list[drop_subcmd_list_i].entryFromXml(reader, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            reader.stepOut(value4drop_subcmd_list);
        }
    }
    value4forward_subcmd_count = reader.getEntryValue("forward_subcmd_count");
    if (value4forward_subcmd_count == NULL) {
        this->forward_subcmd_count = 8;
    } else {
        ret = TdrParse::parseUInt16(this->forward_subcmd_count, value4forward_subcmd_count, NULL, 0, NULL, NULL);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (this->forward_subcmd_count > 8) {
            return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
        }
    }
    if (this->forward_subcmd_count > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->forward_subcmd_count != 0) {
        value4forward_subcmd_list = "forward_subcmd_list";
        for (forward_subcmd_list_i = 0; forward_subcmd_list_i < this->forward_subcmd_count; forward_subcmd_list_i++) {
            if (reader.stepIn(value4forward_subcmd_list) != TdrXmlReader::WS_NORMAL) {
                this->forward_subcmd_count = (uint16_t)forward_subcmd_list_i;
                return ret;
            }
            ret = this->forward_subcmd_list[forward_subcmd_list_i].entryFromXml(reader, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            reader.stepOut(value4forward_subcmd_list);
        }
    }
    if (reader.stepIn("ban_mode") != TdrXmlReader::WS_NORMAL) {
        ret = this->ban_mode.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->ban_mode.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("ban_mode");
    }
    if (reader.stepIn("drop_cmd") != TdrXmlReader::WS_NORMAL) {
        ret = this->drop_cmd.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->drop_cmd.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("drop_cmd");
    }
    if (reader.stepIn("property_cmd") != TdrXmlReader::WS_NORMAL) {
        ret = this->property_cmd.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->property_cmd.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("property_cmd");
    }
    return ret;
}

TdrError::ErrorType DPCmdHack::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short drop_subcmd_list_i;
    unsigned short forward_subcmd_list_i;

    ret = writer.openComplex("delay_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->delay_cmd.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("delay_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "safemode_to_client");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "safemode_to_client");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->safemode_to_client);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "safemode_to_client");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "safemode_hint_client");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "safemode_hint_client");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->safemode_hint_client);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "safemode_hint_client");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "cf_room_qtalk_enable");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "cf_room_qtalk_enable");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("0x%02x ", this->cf_room_qtalk_enable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "cf_room_qtalk_enable");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->up_cmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("raw_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->raw_cmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("raw_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("captch_method");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->captch_method.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("captch_method");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "drop_subcmd_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "drop_subcmd_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->drop_subcmd_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "drop_subcmd_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->drop_subcmd_count > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->drop_subcmd_count != 0) {
        for (drop_subcmd_list_i = 0; drop_subcmd_list_i < this->drop_subcmd_count; drop_subcmd_list_i++) {
            ret = writer.openComplex("drop_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->drop_subcmd_list[drop_subcmd_list_i].entryToListXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("drop_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    if (writer._isPartial) {
        ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "forward_subcmd_count");
        writer._isPartial = false;
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    } else {
        ret = writer._out.textize("%s<%s> ", writer._indentStr, "forward_subcmd_count");
        writer._state = ret;
        if (writer._state != TdrError::TDR_NO_ERROR) {
            return writer._state;
        }
    }
    ret = writer.textize("%d ", this->forward_subcmd_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("</%s>\n", "forward_subcmd_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    if (this->forward_subcmd_count > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->forward_subcmd_count != 0) {
        for (forward_subcmd_list_i = 0; forward_subcmd_list_i < this->forward_subcmd_count; forward_subcmd_list_i++) {
            ret = writer.openComplex("forward_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->forward_subcmd_list[forward_subcmd_list_i].entryToListXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("forward_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer.openComplex("ban_mode");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->ban_mode.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("ban_mode");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("drop_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->drop_cmd.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("drop_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("property_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->property_cmd.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("property_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPCmdHack::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    unsigned short drop_subcmd_list_i;
    unsigned short forward_subcmd_list_i;

    ret = writer._out.textize(" %s=\"", "safemode_to_client");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->safemode_to_client);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "safemode_hint_client");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->safemode_hint_client);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "cf_room_qtalk_enable");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("0x%02x", this->cf_room_qtalk_enable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "drop_subcmd_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->drop_subcmd_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer._out.textize(" %s=\"", "forward_subcmd_count");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.textize("%d", this->forward_subcmd_count);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer._out.textize("\"");
    writer._state = ret;
    if (writer._state != TdrError::TDR_NO_ERROR) {
        return writer._state;
    }
    ret = writer.openComplex("delay_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->delay_cmd.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("delay_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->up_cmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->down_cmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("raw_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->raw_cmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("raw_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("captch_method");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->captch_method.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("captch_method");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (this->drop_subcmd_count > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->drop_subcmd_count != 0) {
        for (drop_subcmd_list_i = 0; drop_subcmd_list_i < this->drop_subcmd_count; drop_subcmd_list_i++) {
            ret = writer.openComplex("drop_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->drop_subcmd_list[drop_subcmd_list_i].entryToAttrXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("drop_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    if (this->forward_subcmd_count > 8) {
        return TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    if (this->forward_subcmd_count != 0) {
        for (forward_subcmd_list_i = 0; forward_subcmd_list_i < this->forward_subcmd_count; forward_subcmd_list_i++) {
            ret = writer.openComplex("forward_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = this->forward_subcmd_list[forward_subcmd_list_i].entryToAttrXml(writer, 1);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            ret = writer.closeComplex("forward_subcmd_list");
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
        }
    }
    ret = writer.openComplex("ban_mode");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->ban_mode.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("ban_mode");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("drop_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->drop_cmd.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("drop_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("property_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->property_cmd.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("property_cmd");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

// ------------------------------------------------------------- DPConfig

TdrError::ErrorType DPConfig::construct() {
    TdrError::ErrorType ret;
    ret = this->dp_base_conf.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_proto_conf.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_log_conf.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_time_confg.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_code_info.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->uin_monitor_list.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_up_cmd_hack.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_down_cmd_hack.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->cmd_hack.construct();
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPConfig::fromXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPConfig::fromXmlBuffer(const char *buffer, unsigned int size, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPConfig::fromXmlString(char *string, TdrXmlFormat format, unsigned int cutVer) {
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

TdrError::ErrorType DPConfig::fromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret = TDR_ERR_INVALID_ROOT_NODE;
    if (reader.stepIn("DPConfig") == TdrXmlReader::WS_NORMAL) {
        ret = entryFromXml(reader, cutVer);
        if (ret == TdrError::TDR_NO_ERROR) {
            reader.stepOut("DPConfig");
        }
    }
    return ret;
}

TdrError::ErrorType DPConfig::toXmlFile(const char *file, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    FILE *fp;
    fp = fopen(file, "wb");
    if (fp != NULL) {
        ret = toXmlFile(fp, format, cutVer);
        fclose(fp);
    }
    return ret;
}

TdrError::ErrorType DPConfig::toXmlFile(FILE *fp, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPConfig::toXmlBuffer(char *buffer, unsigned int size, unsigned int *usedSize, TdrXmlFormat format, unsigned int cutVer) const {
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

TdrError::ErrorType DPConfig::toXml(TdrXmlWriter &writer, TdrXmlFormat format, unsigned int cutVer) const {
    TdrError::ErrorType ret;
    ret = writer.openSimple("DPConfig");
    if (ret == TdrError::TDR_NO_ERROR) {
        if (format == ATTR_ENTRY) {
            ret = entryToAttrXml(writer, cutVer);
        } else {
            ret = entryToListXml(writer, cutVer);
        }
        if (ret == TdrError::TDR_NO_ERROR) {
            ret = writer.closeSimple("DPConfig");
        }
    }
    return ret;
}

TdrError::ErrorType DPConfig::entryFromXml(TdrXmlReader &reader, unsigned int cutVer) {
    TdrError::ErrorType ret;

    if (reader.stepIn("dp_base_conf") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_base_conf.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_base_conf.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_base_conf");
    }
    if (reader.stepIn("dp_proto_conf") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_proto_conf.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_proto_conf.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_proto_conf");
    }
    if (reader.stepIn("dp_log_conf") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_log_conf.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_log_conf.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_log_conf");
    }
    if (reader.stepIn("dp_time_confg") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_time_confg.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_time_confg.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_time_confg");
    }
    if (reader.stepIn("dp_code_info") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_code_info.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_code_info.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_code_info");
    }
    if (reader.stepIn("uin_monitor_list") != TdrXmlReader::WS_NORMAL) {
        ret = this->uin_monitor_list.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->uin_monitor_list.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("uin_monitor_list");
    }
    if (reader.stepIn("dp_up_cmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_up_cmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_up_cmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_up_cmd_hack");
    }
    if (reader.stepIn("dp_down_cmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->dp_down_cmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->dp_down_cmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("dp_down_cmd_hack");
    }
    if (reader.stepIn("cmd_hack") != TdrXmlReader::WS_NORMAL) {
        ret = this->cmd_hack.construct();
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
    } else {
        ret = this->cmd_hack.entryFromXml(reader, 1);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        reader.stepOut("cmd_hack");
    }
    return ret;
}

TdrError::ErrorType DPConfig::entryToListXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer.openComplex("dp_base_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_base_conf.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_base_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_proto_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_proto_conf.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_proto_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_log_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_log_conf.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_log_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_time_confg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_time_confg.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_time_confg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_code_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_code_info.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_code_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("uin_monitor_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->uin_monitor_list.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("uin_monitor_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_up_cmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_down_cmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->cmd_hack.entryToListXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

TdrError::ErrorType DPConfig::entryToAttrXml(TdrXmlWriter &writer, unsigned int cutVer) const {
    TdrError::ErrorType ret;

    ret = writer.openComplex("dp_base_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_base_conf.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_base_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_proto_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_proto_conf.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_proto_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_log_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_log_conf.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_log_conf");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_time_confg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_time_confg.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_time_confg");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_code_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_code_info.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_code_info");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("uin_monitor_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->uin_monitor_list.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("uin_monitor_list");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_up_cmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_up_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("dp_down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->dp_down_cmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("dp_down_cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.openComplex("cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = this->cmd_hack.entryToAttrXml(writer, 1);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = writer.closeComplex("cmd_hack");
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return ret;
}

} // namespace tss_dp_comm
