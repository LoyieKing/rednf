// ABI-accurate private model of the tsf4g_tdr XML/TdrParse runtime
// (TdrXml.h, TdrIO.h, TdrParse.h and the rapidxml.hpp revision they pull in)
// used by the TDR-generated configuration TUs
//   src/protocol/formmog/formmog_conf_tsssdk_conf.cpp
//   src/protocol/formmog/formmog_conf_tsssdk_conf_path.cpp
// of libtss_sdk.so.1.0.394 (tag 394).
//
// Provenance (no invented API):
//   * Every class here is a DW_TAG_class_type/structure of the two CUs; the
//     member names, offsets and sizes are the DWARF ones
//     (cu_layout.py / dwarf_full_extract.py on the ORIG):
//       TdrXmlFile   8      : _buffer(+0) _state(+4)
//       TdrOutStream 24     : _fp(+0) _buf(TdrWriteBuf,+4) _state(+20)
//       TdrXmlReader 65624  : _buffer(+0) _format(+4) _state(+8)
//                             _ducument(xml_document<char>, +12)
//                             _currRoot(+65616) _currNode(+65620)
//       TdrXmlWriter 76     : _out(TdrOutStream,+0) _format(+24) _state(+28)
//                             _isSimple(+32) _isPartial(+33) _indentLvl(+36)
//                             _indentStr[33](+40)
//       rapidxml::xml_base<char>      20
//       rapidxml::xml_node<char>      48  (xml_base + node_type + 6 pointers)
//       rapidxml::memory_pool<char>   65556 (m_begin m_ptr m_end
//                                            m_static_memory[65536]
//                                            m_alloc_func m_free_func)
//       rapidxml::xml_document<char>  65604 (xml_node 48 + memory_pool 65556)
//   * The inlined helpers are the ones the ORIG CU records as
//     DW_TAG_inlined_subroutine with their DW_AT_call_line
//     (dwarf_inl_dbg.py): TdrXmlWriter::{openNode,closeNode,openAttr,closeAttr,
//     declare,state,getUsedSize}, TdrXmlReader::{state,~TdrXmlReader},
//     rapidxml::{clear,init,~memory_pool,~xml_document}, TdrXmlFile
//     {TdrXmlFile,~TdrXmlFile,content}.
//   * The bodies of openNode/closeNode/openAttr/closeAttr/declare and of the
//     TdrXmlReader/TdrXmlFile destructors are modelled so that the emitted
//     32-bit code is the ORIG's: the literal pool, the argument order
//     (writer._out.textize(fmt, ...) for the tag writers, writer.textize(fmt,
//     ...) for the values) and the writer._state updates are taken from the ORIG
//     disassembly of every entryToListXml/entryToAttrXml/toXmlBuffer/toXmlFile.
//   * rapidxml::memory_pool::clear()/init()/align() are the rapidxml 1.13
//     bodies (see third_party/rapidxml-1.13/rapidxml.hpp); the ORIG inlines
//     exactly their instruction sequence into ~TdrXmlReader
//     (delete[] _buffer, then the pool walk with ((n) - (p & (n-1))) & (n-1)
//     alignment of 4, then init()'s m_ptr/m_end stores).
//   * TdrParse declares only the four overloads the two CUs call; the
//     signature `TdrError::ErrorType (*)(int &, const char *, const char *)`
//     is the ORIG's mangled macro parameter
//     (tsf4g_tdr::TdrParse::parseInt32(int&, char const*, unsigned int*, int,
//     tsf4g_tdr::TdrError::ErrorType (*)(int&, char const*, char const*),
//     char const*)).
//
// Codegen compensations (NOT ORIG source attributes, see AGENT_REPORT_genconf):
//   * TdrXmlFile::loadState() - the ORIG's generated code stores the imported
//     TdrXmlFile::load() result into _state at every call site (TdrXmlFile's
//     DWARF has _state at +4 and the ORIG emits `mov %eax,4(%ebx)` right after
//     the PLT call).  The wrapper that did this in the ORIG build was an inline
//     function of TdrIO.h that the ORIG inlined at every call site, so its name
//     is not recoverable from the binary; it is modelled here as an inline
//     wrapper so that the emitted code is the ORIG's.  It is fully inlined:
//     no symbol of that name reaches the object file.
#ifndef TSF4G_TDR_COMPAT_CFG_CONF_H_
#define TSF4G_TDR_COMPAT_CFG_CONF_H_

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tsf4g_tdr_compat.h"

namespace tsf4g_tdr {

enum TdrXmlFormat {
    LIST_ENTRY = 0,
    ATTR_ENTRY = 1,
};

// The shared binary-TDR model (tsf4g_tdr_compat.h) enumerates ErrorType down
// to -20 only.  The ORIG DWARF enumerates the XML layer's values -21..-30
// (TdrError::ErrorType, TdrXml.h:24); the two the config TUs return are
// modelled here under their ORIG names.
static const TdrError::ErrorType TDR_ERR_OPEN_FILE_WRITE_FAILED =
    (TdrError::ErrorType)-23;
static const TdrError::ErrorType TDR_ERR_INVALID_ROOT_NODE =
    (TdrError::ErrorType)-28;

// ---------------------------------------------------------------------------
// rapidxml.hpp (only the part the reader holds inline)
// ---------------------------------------------------------------------------

#define RAPIDXML_ALIGNMENT 4
#define RAPIDXML_STATIC_POOL_SIZE (64 * 1024)
#define RAPIDXML_DYNAMIC_POOL_SIZE (64 * 1024)

namespace rapidxml {

template <class Ch> class xml_node;
template <class Ch> class xml_attribute;

// DWARF: rapidxml::xml_base<char> size=20
template <class Ch> class xml_base {
public:
    xml_base() : m_name(0), m_value(0), m_parent(0) {}

    Ch *m_name;
    size_t m_name_size;
    Ch *m_value;
    size_t m_value_size;
    xml_node<Ch> *m_parent;
};

// DWARF: rapidxml::xml_node<char> size=48
template <class Ch> class xml_node : public xml_base<Ch> {
public:
    enum node_type {
        node_document = 0,
        node_element = 1,
        node_data = 2,
        node_cdata = 3,
        node_comment = 4,
        node_declaration = 5,
        node_doctype = 6,
        node_pi = 7,
    };

    explicit xml_node(node_type type) : m_type(type), m_first_node(0),
        m_last_node(0), m_first_attribute(0), m_last_attribute(0),
        m_prev_sibling(0), m_next_sibling(0) {}

    node_type m_type;
    xml_node<Ch> *m_first_node;
    xml_node<Ch> *m_last_node;
    xml_attribute<Ch> *m_first_attribute;
    xml_attribute<Ch> *m_last_attribute;
    xml_node<Ch> *m_prev_sibling;
    xml_node<Ch> *m_next_sibling;
};

// DWARF: rapidxml::memory_pool<char> size=65556.  clear()/init()/align() are
// the rapidxml 1.13 bodies; ~TdrXmlReader inlines exactly their code.
template <class Ch> class memory_pool {
public:
    typedef void *(alloc_func)(size_t);
    typedef void (free_func)(void *);

    memory_pool() : m_alloc_func(0), m_free_func(0) {
        init();
    }

    ~memory_pool() {
        clear();
    }

    void clear() {
        while (m_begin != m_static_memory) {
            char *previous_begin =
                reinterpret_cast<header *>(align(m_begin))->previous_begin;
            if (m_free_func)
                m_free_func(m_begin);
            else
                delete[] m_begin;
            m_begin = previous_begin;
        }
        init();
    }

private:
    struct header {
        char *previous_begin;
    };

    void init() {
        m_begin = m_static_memory;
        m_ptr = align(m_begin);
        m_end = m_static_memory + sizeof(m_static_memory);
    }

    char *align(char *ptr) {
        size_t alignment = ((RAPIDXML_ALIGNMENT -
                             (size_t(ptr) & (RAPIDXML_ALIGNMENT - 1))) &
                            (RAPIDXML_ALIGNMENT - 1));
        return ptr + alignment;
    }

    char *m_begin;
    char *m_ptr;
    char *m_end;
    char m_static_memory[RAPIDXML_STATIC_POOL_SIZE];
    alloc_func *m_alloc_func;
    free_func *m_free_func;
};

// DWARF: rapidxml::xml_document<char> size=65604 = xml_node(48)+memory_pool.
// The ORIG has no ~xml_document body of its own (no remove_all_nodes() call is
// emitted anywhere in the CU), only the memory_pool base destructor.
template <class Ch> class xml_document : public xml_node<Ch>,
                                         public memory_pool<Ch> {
public:
    xml_document() : xml_node<Ch>(xml_node<Ch>::node_document) {}
};

} // namespace rapidxml

// ---------------------------------------------------------------------------
// TdrIO.h
// ---------------------------------------------------------------------------

// DWARF: TdrOutStream size=24; _state(+20) is written by textize().
class TdrOutStream {
public:
    // Out-of-line in the ORIG (tsf4g_tdr runtime), imported by the CU.
    TdrError::ErrorType vtextize(const char *format, va_list ap);

    // Inline member of TdrIO.h in the ORIG; the CU emits the weak out-of-line
    // copy (0x000ee1f0, 0x40 bytes) because GCC never inlines a variadic call.
    TdrError::ErrorType textize(const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        _state = vtextize(format, ap);
        va_end(ap);
        return _state;
    }

    TdrError::ErrorType state() const {
        return _state;
    }

    size_t getUsedSize() const {
        return _buf.getUsedSize();
    }

    FILE *_fp;
    TdrWriteBuf _buf;
    TdrError::ErrorType _state;
};

// DWARF: TdrXmlFile size=8, load() defined in TdrIO.cpp:17 (imported here).
class TdrXmlFile {
public:
    TdrXmlFile() : _buffer(0), _state(TdrError::TDR_NO_ERROR) {}
    ~TdrXmlFile() {
        if (_buffer != NULL) {
            delete[] _buffer;
            _buffer = NULL;
        }
    }

    // ORIG: tsf4g_tdr::TdrXmlFile::load(char const*) @ 0x00174ac0.
    TdrError::ErrorType load(const char *file);

    // See the header comment: codegen compensation for the inline TdrIO.h
    // wrapper the ORIG's generated code called.
    TdrError::ErrorType loadState(const char *file) {
        return _state = load(file);
    }

    char *buf() const {
        return _buffer;
    }

    char *content() const {
        return _buffer;
    }

private:
    char *_buffer;
    TdrError::ErrorType _state;
};

// ---------------------------------------------------------------------------
// TdrXml.h
// ---------------------------------------------------------------------------

// DWARF: TdrXmlReader size=65624; the destructor is inline (the ORIG inlines
// the whole delete[] _buffer + ~xml_document chain at every call site).
class TdrXmlReader {
public:
    enum WalkState {
        WS_NORMAL = 0,
        WS_NO_MORE_NODE = 1,
    };

    // Out-of-line in the ORIG (tsf4g_tdr runtime), imported by the CU.
    TdrXmlReader(const char *buf, unsigned int size, TdrXmlFormat format);
    TdrXmlReader(char *buf, TdrXmlFormat format);

    ~TdrXmlReader() {
        if (_buffer != NULL) {
            delete[] _buffer;
            _buffer = NULL;
        }
    }

    TdrError::ErrorType state() const {
        return _state;
    }

    WalkState stepIn(const char *name);
    void stepOut(const char *name);
    const char *getAttrValue(const char *name);
    const char *getNodeValue(const char *name);
    const char *getEntryValue(const char *name);

private:
    char *_buffer;
    TdrXmlFormat _format;
    TdrError::ErrorType _state;
    rapidxml::xml_document<char> _ducument;
    rapidxml::xml_node<char> *_currRoot;
    rapidxml::xml_node<char> *_currNode;
};

// DWARF: TdrXmlWriter size=76.  open/closeSimple and open/closeComplex are
// out-of-line in the ORIG (imported); open/closeNode, open/closeAttr and
// declare are inline members that the ORIG inlines at every call site.
class TdrXmlWriter {
public:
    TdrXmlWriter(char *buf, unsigned int size, TdrXmlFormat format);
    TdrXmlWriter(FILE *fp, TdrXmlFormat format);

    TdrError::ErrorType state() const {
        return _state;
    }

    unsigned int getUsedSize() const {
        return (unsigned int)_out.getUsedSize();
    }

    TdrError::ErrorType openSimple(const char *name);
    TdrError::ErrorType closeSimple(const char *name);
    TdrError::ErrorType openComplex(const char *name);
    TdrError::ErrorType closeComplex(const char *name);

    // LIST_ENTRY element: "<name>" (or ">\n<indent><name>" once a pending
    // tag has to be closed first).
    TdrError::ErrorType openNode(const char *name) {
        TdrError::ErrorType ret;
        if (_isPartial) {
            ret = _out.textize(">\n%s<%s> ", _indentStr, name);
            _isPartial = false;
            _state = ret;
            if (_state != TdrError::TDR_NO_ERROR) {
                return _state;
            }
        } else {
            ret = _out.textize("%s<%s> ", _indentStr, name);
            _state = ret;
            if (_state != TdrError::TDR_NO_ERROR) {
                return _state;
            }
        }
        return _state;
    }

    TdrError::ErrorType closeNode(const char *name) {
        TdrError::ErrorType ret = _out.textize("</%s>\n", name);
        _state = ret;
        if (_state != TdrError::TDR_NO_ERROR) {
            return _state;
        }
        return _state;
    }

    // ATTR_ENTRY attribute: name="..." ; the closing quote carries no name.
    TdrError::ErrorType openAttr(const char *name) {
        TdrError::ErrorType ret = _out.textize(" %s=\"", name);
        _state = ret;
        if (_state != TdrError::TDR_NO_ERROR) {
            return _state;
        }
        return _state;
    }

    TdrError::ErrorType closeAttr(const char *name) {
        TdrError::ErrorType ret = _out.textize("\"");
        _state = ret;
        if (_state != TdrError::TDR_NO_ERROR) {
            return _state;
        }
        return _state;
    }

    // XML prologue of toXmlFile/toXmlBuffer (the "GBK" literal is the
    // caller's, see the CU).
    TdrError::ErrorType declare(const char *encoding) {
        return _out.textize(
            "<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n",
            encoding);
    }

    // Inline member of TdrXml.h in the ORIG; the CU emits the weak out-of-line
    // copy (0x000ee1b0, 0x40 bytes), again because of the variadic call.
    TdrError::ErrorType textize(const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        _state = _out.vtextize(format, ap);
        va_end(ap);
        return _state;
    }

    TdrError::ErrorType textizeStr(const char *str, bool escape);

    TdrOutStream _out;
    TdrXmlFormat _format;
    TdrError::ErrorType _state;
    bool _isSimple;
    bool _isPartial;
    unsigned int _indentLvl;
    char _indentStr[33];
};

// ---------------------------------------------------------------------------
// TdrParse.h
// ---------------------------------------------------------------------------

class TdrParse {
public:
    typedef TdrError::ErrorType (*TdrParseMacro)(int32_t &, const char *,
                                                 const char *);

    static TdrError::ErrorType parseInt8(int8_t &dest, const char *src,
                                         unsigned int *parsed, int8_t _default,
                                         TdrParseMacro macro,
                                         const char *groupName);
    static TdrError::ErrorType parseInt32(int32_t &dest, const char *src,
                                          unsigned int *parsed,
                                          int32_t _default,
                                          TdrParseMacro macro,
                                          const char *groupName);
    static TdrError::ErrorType parseUInt16(uint16_t &dest, const char *src,
                                           unsigned int *parsed,
                                           uint16_t _default,
                                           TdrParseMacro macro,
                                           const char *groupName);
    static TdrError::ErrorType parseUInt32(uint32_t &dest, const char *src,
                                           unsigned int *parsed,
                                           uint32_t _default,
                                           TdrParseMacro macro,
                                           const char *groupName);
};

} // namespace tsf4g_tdr

#endif // TSF4G_TDR_COMPAT_CFG_CONF_H_
