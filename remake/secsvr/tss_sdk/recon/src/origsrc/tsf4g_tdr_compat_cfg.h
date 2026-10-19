// ABI-accurate private model of the tsf4g_tdr XML runtime (TdrXml.h/TdrIO.h/
// TdrParse.h of the ORIG tree) as seen from formmog_conf_tsssdk_dp.cpp in
// libtss_sdk.so.1.0.394 (tag 394).
//
// Provenance (no invented API, no invented layout):
//   * Class layouts are the DW_AT_data_member_location / byte_size values of the
//     CU's own DWARF (cu_layout.py on formmog_conf_tsssdk_dp.cpp), i.e. they are
//     the ORIG's own numbers, including the ORIG's own member spelling:
//       TdrOutStream   { _fp+0, _buf(TdrWriteBuf,16 bytes)+4, _state+20 }        = 24
//       TdrXmlReader   { _buffer+0, _format+4, _state+8,
//                        _ducument(rapidxml::xml_document<char>)+12,
//                        _currRoot+65616, _currNode+65620 }                      = 65624
//       TdrXmlWriter   { _out(TdrOutStream,24)+0, _format+24, _state+28,
//                        _isSimple+32, _isPartial+33, _indentLvl+36,
//                        _indentStr[33]+40 }                                     = 76
//       TdrXmlFile     { _buffer+0, _state+4 }                                   = 8
//       rapidxml::xml_base<char>  { m_name,m_value,m_name_size,m_value_size,
//                                   m_parent }                                   = 20
//       rapidxml::xml_node<char>  base + { m_type+20, m_first_node+24,
//                                   m_last_node+28, m_first_attribute+32,
//                                   m_last_attribute+36, m_prev_sibling+40,
//                                   m_next_sibling+44 }                          = 48
//       rapidxml::memory_pool<char>{ m_begin+0, m_ptr+4, m_end+8,
//                                   m_static_memory[65536]+12,
//                                   m_alloc_func+65548, m_free_func+65552 }      = 65556
//       rapidxml::xml_document<char> = xml_node<char> + memory_pool<char>        = 65604
//     The pool destructor inlined into this CU walks the free list through the
//     4-byte-aligned header (`m_begin + ((-(unsigned)m_begin) & 3)`), calls
//     m_free_func when set and `operator delete[]` otherwise, and then restores
//     m_begin/m_ptr/m_end - exactly the code emitted for ~memory_pool below.
//   * Function declarations are the demangled PLT targets the CU calls
//     (objdump of the ORIG), so every signature here has an existing definition
//     in the ORIG's own TdrXml.cpp/TdrIO.cpp/TdrParse.cpp.
//   * Out-of-line weak definitions that belong to THIS CU in the ORIG are the
//     two `textize` methods (0x000ee1b0 TdrXmlWriter::textize,
//     0x000ee1f0 TdrOutStream::textize, 0x40 bytes each): they are spelled
//     inline here so this TU emits them, as the ORIG does.
#ifndef TSF4G_TDR_COMPAT_CFG_H_
#define TSF4G_TDR_COMPAT_CFG_H_

#include "tsf4g_tdr_compat.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace tsf4g_tdr {

#ifndef TSF4G_TDR_TDRXMLFORMAT_DECLARED
#define TSF4G_TDR_TDRXMLFORMAT_DECLARED
enum TdrXmlFormat {
    LIST_ENTRY = 0,
    ATTR_ENTRY = 1
};
#endif

// The shared ABI model (tsf4g_tdr_compat.h) spells out only the TdrError::
// ErrorType codes that the UIC CU uses.  This CU's ORIG binary also uses the
// tail of the ORIG's own TdrError enum; the enumerators below are that enum's
// DWARF entries of this very CU, with their own names and values:
//   TDR_ERR_VALUE_UNDER_OR_OVER_FLOW = -21, TDR_ERR_OPEN_FILE_READ_FAILED = -22,
//   TDR_ERR_OPEN_FILE_WRITE_FAILED = -23, TDR_ERR_READ_FILE_FAILED = -24,
//   TDR_ERR_WRITE_FILE_FAILED = -25, TDR_ERR_ALLOC_MEMORY_FAILED = -26,
//   TDR_ERR_PARSE_XML_FAILED = -27, TDR_ERR_INVALID_ROOT_NODE = -28,
//   TDR_ERR_FAILED_TO_PARSE_NUMERIC = -29, TDR_ERR_UNDEFINED_MACRO_NAME = -30.
// The shared header is fixed, so the code this CU returns when
// TdrXmlReader::stepIn fails on the root element is provided here under the
// ORIG's own enumerator name and value (documented compensation).
const TdrError::ErrorType TDR_ERR_INVALID_ROOT_NODE = (TdrError::ErrorType)-28;

} // namespace tsf4g_tdr

namespace rapidxml {

template <class Ch>
class xml_node;

template <class Ch>
class xml_base {
public:
    Ch *m_name;              // +0
    Ch *m_value;             // +4
    size_t m_name_size;      // +8
    size_t m_value_size;     // +12
    xml_node<Ch> *m_parent;  // +16
};

template <class Ch>
class xml_attribute : public xml_base<Ch> {
public:
    xml_attribute<Ch> *m_prev_attribute;
    xml_attribute<Ch> *m_next_attribute;
};

template <class Ch>
class xml_node : public xml_base<Ch> {
public:
    enum node_type {
        node_document = 0,
        node_element = 1,
        node_data = 2,
        node_cdata = 3,
        node_comment = 4,
        node_declaration = 5,
        node_doctype = 6,
        node_pi = 7
    };

    node_type m_type;                       // +20
    xml_node<Ch> *m_first_node;             // +24
    xml_node<Ch> *m_last_node;              // +28
    xml_attribute<Ch> *m_first_attribute;   // +32
    xml_attribute<Ch> *m_last_attribute;    // +36
    xml_node<Ch> *m_prev_sibling;           // +40
    xml_node<Ch> *m_next_sibling;           // +44
};

// The pool keeps its free list inside the released blocks themselves: a block
// begins with a 4-byte aligned `char *` link to the previously released block,
// and the static arena is the head sentinel of that list.
template <class Ch>
class memory_pool {
public:
    typedef void *(*alloc_func)(size_t);
    typedef void (*free_func)(void *);

    static char *align(char *ptr) {
        return ptr + ((-(size_t)ptr) & (sizeof(char *) - 1));
    }

    void clear() {
        m_begin = m_ptr = align(m_static_memory);
        m_end = m_static_memory + sizeof(m_static_memory);
    }

    ~memory_pool() {
        while (m_begin != m_static_memory) {
            char *previous_begin = *reinterpret_cast<char **>(align(m_begin));
            if (m_free_func != NULL) {
                m_free_func(m_begin);
            } else {
                delete[] m_begin;
            }
            m_begin = previous_begin;
        }
        clear();
    }

private:
    char *m_begin;                  // +0
    char *m_ptr;                    // +4
    char *m_end;                    // +8
    char m_static_memory[65536];    // +12
    alloc_func m_alloc_func;        // +65548
    free_func m_free_func;          // +65552
};

template <class Ch>
class xml_document : public xml_node<Ch> {
public:
    memory_pool<Ch> m_pool;         // +48
};

} // namespace rapidxml

namespace tsf4g_tdr {

class TdrOutStream {
public:
    TdrError::ErrorType vtextize(const char *format, va_list ap);

    // Out-of-line weak definition in the ORIG (0x000ee1f0, 0x40 bytes).
    TdrError::ErrorType textize(const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        _state = vtextize(format, ap);
        va_end(ap);
        return _state;
    }

    FILE *_fp;                  // +0
    TdrWriteBuf _buf;           // +4
    TdrError::ErrorType _state; // +20
};

class TdrXmlFile {
public:
    TdrXmlFile() {
        _buffer = NULL;
        _state = TdrError::TDR_NO_ERROR;
    }

    ~TdrXmlFile() {
        if (_buffer != NULL) {
            delete[] _buffer;
            _buffer = NULL;
        }
    }

    TdrError::ErrorType load(const char *file);
    char *buf() const { return _buffer; }
    char *content() const { return _buffer; }

private:
    char *_buffer;              // +0
    TdrError::ErrorType _state; // +4
};

class TdrXmlReader {
public:
    enum WalkState {
        WS_NORMAL = 0,
        WS_NO_MORE_NODE = 1
    };

    TdrXmlReader(const char *buf, unsigned int size, TdrXmlFormat format);
    TdrXmlReader(char *buf, TdrXmlFormat format);

    ~TdrXmlReader() {
        if (_buffer != NULL) {
            delete[] _buffer;
            _buffer = NULL;
        }
    }

    TdrError::ErrorType state() const { return _state; }
    WalkState stepIn(const char *name);
    void stepOut(const char *name);
    const char *getAttrValue(const char *name);
    const char *getNodeValue(const char *name);
    const char *getEntryValue(const char *name);

private:
    char *_buffer;                           // +0
    TdrXmlFormat _format;                    // +4
    TdrError::ErrorType _state;              // +8
    rapidxml::xml_document<char> _ducument;  // +12
    rapidxml::xml_node<char> *_currRoot;     // +65616
    rapidxml::xml_node<char> *_currNode;     // +65620
};

class TdrXmlWriter {
public:
    TdrXmlWriter(char *buf, unsigned int size, TdrXmlFormat format);
    TdrXmlWriter(FILE *fp, TdrXmlFormat format);

    TdrError::ErrorType state() const { return _state; }
    unsigned int getUsedSize() const { return (unsigned int)_out._buf.getUsedSize(); }

    TdrError::ErrorType openSimple(const char *name);
    TdrError::ErrorType closeSimple(const char *name);
    TdrError::ErrorType openComplex(const char *name);
    TdrError::ErrorType closeComplex(const char *name);
    TdrError::ErrorType textizeStr(const char *str, bool escape);

    // Out-of-line weak definition in the ORIG (0x000ee1b0, 0x40 bytes).
    TdrError::ErrorType textize(const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        _state = _out.vtextize(format, ap);
        va_end(ap);
        return _state;
    }

    TdrOutStream _out;              // +0
    TdrXmlFormat _format;           // +24
    TdrError::ErrorType _state;     // +28
    bool _isSimple;                 // +32
    bool _isPartial;                // +33
    unsigned int _indentLvl;        // +36
    char _indentStr[33];            // +40
};

class TdrParse {
public:
    typedef TdrError::ErrorType (*TdrParseMacro)(int32_t &, const char *, const char *);

    static TdrError::ErrorType parseUInt8(uint8_t &dest, const char *src, unsigned int *parsed,
                                          uint8_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt8(uint8_t *dest, unsigned int count, const char *src,
                                          unsigned int *parsed, uint8_t _default, TdrParseMacro macro,
                                          const char *groupName);
    static TdrError::ErrorType parseUInt16(uint16_t &dest, const char *src, unsigned int *parsed,
                                           uint16_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt32(uint32_t &dest, const char *src, unsigned int *parsed,
                                           uint32_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt32(uint32_t *dest, unsigned int count, const char *src,
                                           unsigned int *parsed, uint32_t _default, TdrParseMacro macro,
                                           const char *groupName);
};

} // namespace tsf4g_tdr

#endif // TSF4G_TDR_COMPAT_CFG_H_
