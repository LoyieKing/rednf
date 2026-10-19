// ABI-accurate private model of the tsf4g_tdr runtime used by
// libtss_sdk.so.1.0.394 (tag 394), reconstructed for the
// formmog_proto_tss_sdk_uic.cpp translation unit.
//
// Provenance (no invented API):
//   * Every method name below appears as a DW_TAG_inlined_subroutine callee of
//     the uic CU in the ORIG DWARF (see dwarf_inline_names.py output in the
//     AGENT_REPORT), e.g. writeUInt16(unsigned short),
//     writeUInt16(unsigned short, unsigned int), readUInt16(unsigned short&, unsigned int),
//     writeBytes(void const*, unsigned int), writeCharWithNull(char),
//     getUsedSize(), getTotalSize(), getBeginPtr(), _set(char*, unsigned int),
//     _free(), vtextize(char const*, char*), set(char const*, unsigned int).
//   * TdrWriteBuf layout {data_, pos_, limit_, allocated_} and TdrReadBuf layout
//     {data_, pos_, limit_} are the offsets the ORIG codegen uses
//     (data_=+0, pos_=+4, limit_=+8, allocated_=+0xc; the ctor/dtor bodies
//     write exactly those four slots).
//   * The ErrorType values are the ORIG's TDR_ERR_* distances used by the
//     reconstructed functions (-1 short write, -2 short read, -7 refer
//     surpass, -9 cutver too small, -19 null pointer parameter,
//     -20 net cutver invalid).
//   * The per-method bounds checks reproduce the ORIG instruction shapes:
//     `limit_-pos_ <= N-1` for the fixed-width writers/readers,
//     `pos_ == limit_` for the single-byte writers, and the two-step
//     `pos > limit_` + `limit_-pos_ <= N-1` pair for the offset-addressed
//     accessors used for the package length/version fix-ups.
#ifndef TSF4G_TDR_COMPAT_H_
#define TSF4G_TDR_COMPAT_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// The ORIG declares tsf4g_tdr::TdrError as a namespace carrying ErrorType.
// Generated TUs compile without the protocol include root, so this header
// provides the same enumerators as a class. The shared guard keeps the two
// declaration forms from colliding when a TU includes both this header and
// TdrError.h (whichever comes first wins; both expose the same enumerators).
namespace tsf4g_tdr {

#ifndef TSF4G_TDR_TDRERROR_DECLARED
#define TSF4G_TDR_TDRERROR_DECLARED
class TdrError {
public:
    enum ErrorType {
        TDR_NO_ERROR = 0,
        TDR_ERR_SHORT_BUF_FOR_WRITE = -1,
        TDR_ERR_SHORT_BUF_FOR_READ = -2,
        TDR_ERR_STR_LEN_TOO_BIG = -3,
        TDR_ERR_STR_LEN_TOO_SMALL = -4,
        TDR_ERR_STR_LEN_CONFLICT = -5,
        TDR_ERR_MINUS_REFER_VALUE = -6,
        TDR_ERR_REFER_SURPASS_COUNT = -7,
        TDR_ERR_ARG_POINTER_IS_NULL = -8,
        TDR_ERR_CUTVER_TOO_SMALL = -9,
        TDR_ERR_CUTVER_CONFILICT = -10,
        TDR_ERR_PARSE_TDRIP_FAILED = -11,
        TDR_ERR_INVALID_TDRIP_VALUE = -12,
        TDR_ERR_INVALID_TDRTIME_VALUE = -13,
        TDR_ERR_INVALID_TDRDATE_VALUE = -14,
        TDR_ERR_INVALID_TDRDATETIME_VALUE = -15,
        TDR_ERR_FUNC_LOCALTIME_FAILED = -16,
        TDR_ERR_INVALID_HEX_STR_LEN = -17,
        TDR_ERR_INVALID_HEX_STR_FORMAT = -18,
        TDR_ERR_NULL_POINTER_PARAMETER = -19,
        TDR_ERR_NET_CUTVER_INVALID = -20,
        TDR_ERR_VALUE_UNDER_OR_OVER_FLOW = -21,
        TDR_ERR_OPEN_FILE_READ_FAILED = -22,
        TDR_ERR_OPEN_FILE_WRITE_FAILED = -23,
        TDR_ERR_READ_FILE_FAILED = -24,
        TDR_ERR_WRITE_FILE_FAILED = -25,
        TDR_ERR_ALLOC_MEMORY_FAILED = -26,
        TDR_ERR_PARSE_XML_FAILED = -27,
        TDR_ERR_INVALID_ROOT_NODE = -28,
        TDR_ERR_FAILED_TO_PARSE_NUMERIC = -29,
        TDR_ERR_UNDEFINED_MACRO_NAME = -30
    };
};
#endif

// TdrWriteBuf/TdrReadBuf/TdrBufUtil are shared with
// tsf4g_tdr_compat_client.h.  Both copies now declare the same members and
// methods; this guard makes them co-includable in either order.
#ifndef TSF4G_TDR_BUF_DECLARED
#define TSF4G_TDR_BUF_DECLARED

class TdrWriteBuf {
public:
    friend class TdrBufUtil;

    TdrWriteBuf(char* buf, unsigned int size) {
        _set(buf, size);
    }

    TdrWriteBuf() {
        _set(NULL, 0);
    }

    ~TdrWriteBuf() {
        _free();
    }

    void _set(char* buf, unsigned int size) {
        data_ = buf;
        pos_ = 0;
        limit_ = size;
        allocated_ = false;
    }

    void _free() {
        if (allocated_ && data_ != NULL) {
            delete[] data_;
        }
    }

    TdrError::ErrorType writeUInt8(uint8_t v) {
        if (pos_ == limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = (char)v;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeInt8(int8_t v) {
        if (pos_ == limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = (char)v;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeChar(char v) {
        if (pos_ == limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = v;
        return TdrError::TDR_NO_ERROR;
    }

    // The ORIG inlines these as a byte swap plus one 16/32-bit store, so the
    // header goes through a swapped staging value + memcpy rather than
    // per-byte stores (the per-byte form is measurably larger; AGENT_REPORT).
    TdrError::ErrorType writeUInt16(uint16_t v) {
        if (limit_ - pos_ <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        uint16_t be = (uint16_t)((v >> 8) | (v << 8));
        memcpy(data_ + pos_, &be, 2);
        pos_ += 2;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeUInt32(uint32_t v) {
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        uint32_t be = ((v >> 24) & 0xff) | (((v >> 16) & 0xff) << 8) |
                      (((v >> 8) & 0xff) << 16) | ((v & 0xff) << 24);
        memcpy(data_ + pos_, &be, 4);
        pos_ += 4;
        return TdrError::TDR_NO_ERROR;
    }

    // Bulk byte write: the ORIG pack() inlines a memmove here.
    TdrError::ErrorType writeBytes(const void* src, unsigned int len) {
        if (limit_ - pos_ < len) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        memmove(data_ + pos_, src, len);
        pos_ += len;
        return TdrError::TDR_NO_ERROR;
    }

    // Offset-addressed single byte: position is absolute, pos_ is untouched.
    TdrError::ErrorType writeUInt8(uint8_t v, unsigned int pos) {
        if (pos > limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        if (pos == limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos] = (char)v;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeChar(char v, unsigned int pos) {
        return writeUInt8((uint8_t)v, pos);
    }

    // Offset-addressed uint16: used for the head_ version / pkg_len_ fix-ups.
    // Bounds checks the ORIG emits: pos > limit_, then limit_ - pos <= 1.
    TdrError::ErrorType writeUInt16(uint16_t v, unsigned int pos) {
        if (pos > limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        if (limit_ - pos <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        uint16_t be = (uint16_t)((v >> 8) | (v << 8));
        memcpy(data_ + pos, &be, 2);
        return TdrError::TDR_NO_ERROR;
    }

    // Writes v followed by a NUL terminator; the ORIG consumers ignore the
    // returned position, only the two-byte capacity check is observable.
    TdrError::ErrorType writeCharWithNull(char v) {
        if (limit_ - pos_ <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = v;
        data_[pos_] = '\0';
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType vtextize(const char* format, va_list ap) {
        int n = vsnprintf(data_ + pos_, limit_ - pos_, format, ap);
        if (n < 0) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        if ((size_t)n >= limit_ - pos_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        pos_ += n;
        return TdrError::TDR_NO_ERROR;
    }

    // Out-of-line weak symbol in the ORIG (0x00136e90, 0x6c bytes); defined in
    // formmog_proto_tss_sdk_uic.cpp.
    TdrError::ErrorType textize(const char* format, ...);

    size_t getUsedSize() const { return pos_; }
    size_t getTotalSize() const { return limit_; }
    char* getBeginPtr() const { return data_; }

    size_t size() const { return pos_; }
    size_t capacity() const { return limit_; }
    const char* data() const { return data_; }

private:
    char* data_;
    size_t pos_;
    size_t limit_;
    bool allocated_;
};

class TdrReadBuf {
public:
    TdrReadBuf(const char* buf, unsigned int size) {
        set(buf, size);
    }

    TdrReadBuf() {
        set(NULL, 0);
    }

    TdrError::ErrorType set(const char* buf, unsigned int size) {
        data_ = buf;
        pos_ = 0;
        limit_ = size;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readUInt8(uint8_t& v) {
        if (pos_ == limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = (uint8_t)data_[pos_++];
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readInt8(int8_t& v) {
        if (pos_ == limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = (int8_t)data_[pos_++];
        return TdrError::TDR_NO_ERROR;
    }

    // As above: the ORIG inlines a single 16/32-bit load followed by a byte
    // swap (never four per-byte loads).
    TdrError::ErrorType readUInt16(uint16_t& v) {
        if (limit_ - pos_ <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        uint16_t be;
        memcpy(&be, data_ + pos_, 2);
        v = (uint16_t)((be >> 8) | (be << 8));
        pos_ += 2;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readUInt32(uint32_t& v) {
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        uint32_t be;
        memcpy(&be, data_ + pos_, 4);
        v = ((be >> 24) & 0xff) | (((be >> 16) & 0xff) << 8) |
            (((be >> 8) & 0xff) << 16) | ((be & 0xff) << 24);
        pos_ += 4;
        return TdrError::TDR_NO_ERROR;
    }

    // Offset-addressed read: peeks without advancing pos_.
    TdrError::ErrorType readUInt16(uint16_t& v, unsigned int pos) {
        if (pos > limit_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        if (limit_ - pos <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        uint16_t be;
        memcpy(&be, data_ + pos, 2);
        v = (uint16_t)((be >> 8) | (be << 8));
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readBytes(void* dest, unsigned int len) {
        if (limit_ - pos_ < len) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        memmove(dest, data_ + pos_, len);
        pos_ += len;
        return TdrError::TDR_NO_ERROR;
    }

    size_t getUsedSize() const { return pos_; }
    size_t getTotalSize() const { return limit_; }
    const char* getBeginPtr() const { return data_; }

    size_t size() const { return pos_; }
    size_t capacity() const { return limit_; }
    const char* data() const { return data_; }

private:
    const char* data_;
    size_t pos_;
    size_t limit_;
};

// Only the three overloads this CU calls; bodies live in the tsf4g_tdr
// runtime outside this translation unit (PLT calls in the ORIG).
class TdrBufUtil {
public:
    static TdrError::ErrorType printVariable(TdrWriteBuf& destBuf, int indent,
                                             char separator, const char* name,
                                             const char* format, ...);
    static TdrError::ErrorType printVariable(TdrWriteBuf& destBuf, int indent,
                                             char separator, const char* name,
                                             bool isStruct);
    // Array-of-struct elements: one indexed printVariable per element.
    static TdrError::ErrorType printVariable(TdrWriteBuf& destBuf, int indent,
                                             char separator, const char* name,
                                             unsigned int arrIdx, bool isStruct);
    static TdrError::ErrorType printArray(TdrWriteBuf& destBuf, int indent,
                                          char separator, const char* name,
                                          int64_t count);
};

#endif  // TSF4G_TDR_BUF_DECLARED

} // namespace tsf4g_tdr

#endif // TSF4G_TDR_COMPAT_H_
