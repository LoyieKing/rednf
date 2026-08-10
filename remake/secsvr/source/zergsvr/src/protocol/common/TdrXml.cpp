// Reconstructed from gunnersvr disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrXml.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/protocol/common/TdrXml.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrIO.h"
#include "src/protocol/common/TdrBuf.h"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml.hpp"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <ctype.h>
#include <new>
#include <stdio.h>
#include <string.h>

namespace tsf4g_tdr {

// line 24
char *strip(char *head, char *tail) {
    while (*head != '\0' && isspace(*head)) {
        ++head;
    }
    if (*head == '\0') {
        return NULL;
    }
    if (!isspace(*tail)) {
        return head;
    }
    while (tail > head && isspace(tail[-1])) {
        --tail;
    }
    *tail = '\0';
    return (*head == '\0') ? NULL : head;
}

// line 51
TdrXmlReader::TdrXmlReader(char *buf, TdrXmlFormat format) {
    data_ = NULL;
    format_ = format;
    state_ = TdrError::TDR_NO_ERROR;
    rapidxml::xml_document<char> *doc = reinterpret_cast<rapidxml::xml_document<char> *>(doc_);
    new (doc) rapidxml::xml_document<char>();
    state_ = _parse(buf);
}

// line 59
TdrXmlReader::TdrXmlReader(const char *buf, unsigned int size, TdrXmlFormat format) {
    data_ = NULL;
    format_ = format;
    state_ = TdrError::TDR_NO_ERROR;
    rapidxml::xml_document<char> *doc = reinterpret_cast<rapidxml::xml_document<char> *>(doc_);
    new (doc) rapidxml::xml_document<char>();
    data_ = new (std::nothrow) char[size + 1];
    if (data_ == NULL) {
        state_ = TdrError::TDR_ERR_ALLOC_MEMORY_FAILED;
        return;
    }
    memcpy(data_, buf, size);
    data_[size] = '\0';
    state_ = _parse(data_);
}

// line 78
TdrError::ErrorType TdrXmlReader::_parse(char *buf) {
    rapidxml::xml_document<char> *doc = reinterpret_cast<rapidxml::xml_document<char> *>(doc_);
    char *text = buf;

    doc->remove_all_nodes();
    doc->remove_all_attributes();

    if (static_cast<unsigned char>(text[0]) == 0xEF &&
        static_cast<unsigned char>(text[1]) == 0xBB &&
        static_cast<unsigned char>(text[2]) == 0xBF) {
        text += 3;
    }

    try {
        for (;;) {
            while (rapidxml::internal::lookup_tables<0>::lookup_whitespace[
                       static_cast<unsigned char>(*text)] != 0) {
                ++text;
            }
            if (*text == '\0') {
                break;
            }
            if (*text != '<') {
                throw rapidxml::parse_error("expected <", text);
            }
            ++text;
            rapidxml::xml_node<char> *node = doc->parse_node<0>(text);
            if (node != NULL) {
                doc->append_node(node);
            }
        }
    } catch (rapidxml::parse_error &) {
        return TdrError::TDR_ERR_PARSE_XML_FAILED;
    }

    current_ = doc;
    pending_ = NULL;
    return TdrError::TDR_NO_ERROR;
}

// line 103
TdrXmlReader::WalkState TdrXmlReader::stepIn(const char *name) {
    rapidxml::xml_node<char> *node;

    if (pending_ != NULL) {
        rapidxml::xml_node<char> *pending = static_cast<rapidxml::xml_node<char> *>(pending_);
        if (name == NULL) {
            node = pending->next_sibling();
            if (node == NULL) {
                return WS_NO_MORE_NODE;
            }
            current_ = node;
            pending_ = NULL;
            return WS_NORMAL;
        }
        if (strcmp(name, pending->name()) == 0) {
            size_t len = strlen(name);
            for (node = pending->next_sibling(); node != NULL; node = node->next_sibling()) {
                if (node->name_size() != len) {
                    continue;
                }
                if (memcmp(node->name(), name, len) == 0) {
                    current_ = node;
                    pending_ = NULL;
                    return WS_NORMAL;
                }
            }
            return WS_NO_MORE_NODE;
        }
    }

    rapidxml::xml_node<char> *current = static_cast<rapidxml::xml_node<char> *>(current_);
    if (name == NULL) {
        node = current->first_node();
        if (node == NULL) {
            return WS_NO_MORE_NODE;
        }
        current_ = node;
        pending_ = NULL;
        return WS_NORMAL;
    }

    size_t len = strlen(name);
    for (node = current->first_node(); node != NULL; node = node->next_sibling()) {
        if (node->name_size() != len) {
            continue;
        }
        if (memcmp(node->name(), name, len) == 0) {
            current_ = node;
            pending_ = NULL;
            return WS_NORMAL;
        }
    }
    return WS_NO_MORE_NODE;
}

// line 129
void TdrXmlReader::stepOut(const char *name) {
    rapidxml::xml_node<char> *current = static_cast<rapidxml::xml_node<char> *>(current_);
    pending_ = current;
    current_ = current->parent();
}

// line 140
const char *TdrXmlReader::getEntryValue(const char *name) {
    if (format_ == ATTR_ENTRY) {
        return getAttrValue(name);
    }
    return getNodeValue(name);
}

// line 154
const char *TdrXmlReader::getNodeValue(const char *name) {
    if (stepIn(name) == WS_NO_MORE_NODE) {
        return NULL;
    }
    rapidxml::xml_node<char> *node = static_cast<rapidxml::xml_node<char> *>(current_);
    char *value = node->value();
    size_t valueSize = node->value_size();
    if (value == NULL) {
        stepOut(name);
        return NULL;
    }
    stepOut(name);
    return strip(value, value + valueSize - 1);
}

// line 178
const char *TdrXmlReader::getAttrValue(const char *name) {
    rapidxml::xml_node<char> *node = static_cast<rapidxml::xml_node<char> *>(current_);
    rapidxml::xml_attribute<char> *attr;

    if (name == NULL) {
        attr = node->first_attribute();
        if (attr == NULL) {
            return NULL;
        }
    } else {
        size_t len = strlen(name);
        for (attr = node->first_attribute(); attr != NULL; attr = attr->next_attribute()) {
            if (attr->name_size() != len) {
                continue;
            }
            if (memcmp(attr->name(), name, len) == 0) {
                break;
            }
        }
        if (attr == NULL) {
            return NULL;
        }
    }

    char *value = attr->value();
    if (value == NULL) {
        return NULL;
    }
    return strip(value, value + attr->value_size() - 1);
}

// line 200
TdrXmlWriter::TdrXmlWriter(FILE *fp, TdrXmlFormat format) : _out(fp) {
    _out.allocated_ = false;
    format_ = format;
    _state = TdrError::TDR_NO_ERROR;
    _isPartial = false;
    _indentPending = false;
    _indentLen = 0;
    _indentStr[0] = '\0';
    _reserved = false;
}

// line 210
TdrXmlWriter::TdrXmlWriter(char *buf, unsigned int size, TdrXmlFormat format) : _out() {
    _out.fp_ = NULL;
    _out.data_ = buf;
    _out.pos_ = 0;
    _out.limit_ = (buf != NULL) ? size : 0;
    _out.allocated_ = false;
    format_ = format;
    _state = TdrError::TDR_NO_ERROR;
    _isPartial = false;
    _indentPending = false;
    _indentLen = 0;
    _indentStr[0] = '\0';
    _reserved = false;
}

// line 220
TdrError::ErrorType TdrXmlWriter::openSimple(const char *name) {
    _isPartial = true;
    if (_indentPending) {
        if (format_ == ATTR_ENTRY) {
            _state = _out.textize(">\n%s<%s", _indentStr, name);
        } else {
            _state = _out.textize(">\n%s<%s>\n", _indentStr, name);
            if (_indentLen <= 31) {
                _indentStr[_indentLen] = '\t';
            }
            ++_indentLen;
            if (_indentLen <= 31) {
                _indentStr[_indentLen] = '\0';
            }
        }
        _indentPending = false;
        return _state;
    }
    if (format_ == ATTR_ENTRY) {
        _state = _out.textize("%s<%s", _indentStr, name);
        return _state;
    }
    _state = _out.textize("%s<%s>\n", _indentStr, name);
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\t';
    }
    ++_indentLen;
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\0';
    }
    return _state;
}

// line 255
TdrError::ErrorType TdrXmlWriter::closeSimple(const char *name) {
    if (format_ == ATTR_ENTRY) {
        _state = _out.textize("/>\n");
        _isPartial = false;
        return _state;
    }
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\t';
    }
    --_indentLen;
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\0';
    }
    _state = _out.textize("%s</%s>\n", _indentStr, name);
    _isPartial = false;
    return _state;
}

// line 276
TdrError::ErrorType TdrXmlWriter::openComplex(const char *name) {
    _isPartial = false;
    if (_indentPending) {
        _state = _out.textize(">\n%s<%s", _indentStr, name);
        _indentPending = false;
    } else {
        _state = _out.textize("%s<%s", _indentStr, name);
    }
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\t';
    }
    ++_indentLen;
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\0';
    }
    _indentPending = true;
    return _state;
}

// line 300
TdrError::ErrorType TdrXmlWriter::closeComplex(const char *name) {
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\t';
    }
    --_indentLen;
    if (_indentLen <= 31) {
        _indentStr[_indentLen] = '\0';
    }
    if (_indentPending) {
        _state = _out.textize("/>\n");
        _indentPending = false;
        _isPartial = false;
    } else {
        _state = _out.textize("%s</%s>\n", _indentStr, name);
        _isPartial = false;
    }
    return _state;
}

// line 322
TdrError::ErrorType TdrXmlWriter::textizeStr(const char *str, bool escape) {
    const char *p;
    while ((p = strpbrk(str, "<>&'\"")) != NULL) {
        if (p != str) {
            _state = _out.output(str, p - str);
            if (_state != TdrError::TDR_NO_ERROR) {
                return _state;
            }
        }
        switch (*p) {
        case '&':
            _state = _out.output("&amp;", 5);
            break;
        case '\'':
            _state = _out.output("&apos;", 6);
            break;
        case '"':
            _state = _out.output("&quot;", 6);
            break;
        case '<':
            _state = _out.output("&lt;", 4);
            break;
        case '>':
            _state = _out.output("&gt;", 4);
            break;
        default:
            _state = _out.output(p, 1);
            break;
        }
        if (_state != TdrError::TDR_NO_ERROR) {
            return _state;
        }
        str = p + 1;
    }
    _state = _out.textize(escape ? "%s " : "%s", str);
    return _state;
}

// line 395
TdrError::ErrorType TdrXmlWriter::textizeIP(tdr_ip_t ip, bool escape) {
    char temp[64];
    _state = TdrTypeUtil::tdrIp2Str(temp, sizeof(temp), ip);
    if (_state != TdrError::TDR_NO_ERROR) {
        return _state;
    }
    _state = _out.textize(escape ? "%s " : "%s", temp);
    return _state;
}

// line 415
TdrError::ErrorType TdrXmlWriter::textizeDate(tdr_date_t date, bool escape) {
    char temp[64];
    TdrDate d;
    memcpy(&d, &date, sizeof(d));
    int n = snprintf(temp, sizeof(temp), "%4d-%02d-%02d", (int)(int16_t)d.year, d.month, d.day);
    if (n < 0 || n > 63) {
        _state = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return _state;
    }
    _state = TdrError::TDR_NO_ERROR;
    _state = _out.textize(escape ? "%s " : "%s", temp);
    return _state;
}

// line 435
TdrError::ErrorType TdrXmlWriter::textizeTime(tdr_time_t time, bool escape) {
    char temp[64];
    TdrTime t;
    memcpy(&t, &time, sizeof(t));
    int n = snprintf(temp, sizeof(temp), "%02d:%02d:%02d", (int)(int16_t)t.hour, t.minute, t.second);
    if (n < 0 || n > 63) {
        _state = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return _state;
    }
    _state = TdrError::TDR_NO_ERROR;
    _state = _out.textize(escape ? "%s " : "%s", temp);
    return _state;
}

// line 455
TdrError::ErrorType TdrXmlWriter::textizeDateTime(tdr_datetime_t datetime, bool escape) {
    char temp[64];
    TdrDateTime dt;
    memcpy(&dt, &datetime, sizeof(dt));
    int n = snprintf(temp, sizeof(temp), "%4d-%02d-%02d", (int)(int16_t)dt.year, dt.month, dt.day);
    if (n < 0 || n > (int)sizeof(temp)) {
        _state = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return _state;
    }
    temp[n] = ' ';
    ++n;
    if (n < 0 || n > (int)sizeof(temp)) {
        _state = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return _state;
    }
    if ((int)sizeof(temp) - n == 0) {
        _state = TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
        return _state;
    }
    int m = snprintf(temp + n, sizeof(temp) - n, "%02d:%02d:%02d",
                     (int)(int16_t)dt.hour, dt.minute, dt.second);
    if (m < 0 || m > 63) {
        _state = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return _state;
    }
    _state = TdrError::TDR_NO_ERROR;
    _state = _out.textize(escape ? "%s " : "%s", temp);
    return _state;
}

} // namespace tsf4g_tdr

// line 474
inline rapidxml::parse_error::~parse_error() throw() {
}
