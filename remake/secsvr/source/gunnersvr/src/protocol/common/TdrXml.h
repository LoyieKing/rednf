// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRXML_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRXML_H_H_

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrIO.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

namespace tsf4g_tdr {

enum TdrXmlFormat {
    LIST_ENTRY = 0,
    ATTR_ENTRY = 1,
};

// Layout confirmed from gunnersvr disassembly:
//   +0x00 char* data_            (owned copy of input, or NULL when parsing in place)
//   +0x04 TdrXmlFormat format_
//   +0x08 TdrError::ErrorType state_
//   +0x0c rapidxml::xml_document<char> doc_  (xml_node 0x30 + memory_pool 0x10014)
//         pool: +0x3c m_begin, +0x40 m_ptr, +0x44 m_end,
//               +0x48 m_static_memory[0x10000], +0x10048 m_alloc_func, +0x1004c m_free_func
//   +0x10050 xml_node<char>* current_
//   +0x10054 xml_node<char>* pending_
//   sizeof = 0x10058
class TdrXmlReader {
public:
    enum WalkState {
        WS_NORMAL = 0,
        WS_NO_MORE_NODE = 1,
    };

    TdrXmlReader(const char *buf, unsigned int size, TdrXmlFormat format);
    TdrXmlReader(char *buf, TdrXmlFormat format);
    ~TdrXmlReader() {
        if (data_ != NULL) {
            delete[] data_;
            data_ = NULL;
        }
    }

    TdrError::ErrorType state() const { return state_; }
    WalkState stepIn(const char *name);
    void stepOut(const char *name);
    const char *getAttrValue(const char *name);
    const char *getNodeValue(const char *name);
    const char *getEntryValue(const char *name);

private:
    TdrError::ErrorType _parse(char *buf);

    char *data_;
    TdrXmlFormat format_;
    TdrError::ErrorType state_;
    unsigned char doc_[0x10044];
    void *current_;
    void *pending_;
};

char *strip(char *str, char *prefix);

// Layout confirmed from gunnersvr disassembly:
//   +0x00 TdrOutStream _out   (fp_/data_/pos_/limit_/allocated_/error_, sizeof 0x18)
//   +0x18 TdrXmlFormat format_
//   +0x1c TdrError::ErrorType _state
//   +0x20 bool _isPartial
//   +0x21 bool _indentPending
//   +0x24 unsigned int _indentLen
//   +0x28 char _indentStr[32]
//   +0x48 bool _reserved
//   sizeof = 0x4c
class TdrXmlWriter {
public:
    TdrXmlWriter(char *buf, unsigned int size, TdrXmlFormat format);
    TdrXmlWriter(FILE *fp, TdrXmlFormat format);

    TdrError::ErrorType state() const { return _state; }
    unsigned int getUsedSize() const { return (unsigned int)_out.pos_; }

    TdrError::ErrorType openSimple(const char *name);
    TdrError::ErrorType closeSimple(const char *name);
    TdrError::ErrorType openComplex(const char *name);
    TdrError::ErrorType closeComplex(const char *name);
    TdrError::ErrorType textize(const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        _state = _out.vtextize(format, ap);
        va_end(ap);
        return _state;
    }
    TdrError::ErrorType textizeStr(const char *str, bool escape);
    TdrError::ErrorType textizeIP(tdr_ip_t ip, bool escape);
    TdrError::ErrorType textizeDate(tdr_date_t date, bool escape);
    TdrError::ErrorType textizeTime(tdr_time_t time, bool escape);
    TdrError::ErrorType textizeDateTime(tdr_datetime_t datetime, bool escape);

    TdrOutStream _out;
    TdrXmlFormat format_;
    TdrError::ErrorType _state;
    bool _isPartial;
    bool _indentPending;
    unsigned int _indentLen;
    char _indentStr[32];
    bool _reserved;
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRXML_H_H_
