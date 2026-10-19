// Reconstruction of src/protocol/formmog/formmog_proto_tss_sdk_qt.cpp (tag 394)
// from the ORIG disassembly + DWARF of libtss_sdk.so.1.0.394.
//
// Every function in this file is one of the 106 sec_proto symbols the ORIG
// linker attributes to CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_proto_tss_sdk_qt.cpp
// plus tsf4g_tdr::TdrWriteBuf::textize (0x00136e90, the CU's only out-of-line
// tsf4g_tdr definition; weak in the ORIG and emitted by every CU whose DWARF
// attributes it).
//
// The "ORIG 0xADDR, N bytes" comments are the ORIG symbol table entries
// (nm -DC -S); the "ORIG .cpp:NNN" comments are DW_AT_call_line values of the
// inlined tsf4g_tdr calls inside each ORIG function (dwarf_inline_names.py), so
// the call sequence below is the ORIG's, not an inference from the header.
//
// The class sections follow the ORIG .cpp order recovered from those call
// lines: Login, EnterChannel, CreateRoom, CreateRoomRes, EnterRoom, ChangeRole,
// ChangePosition, RoomMaster, ChangeRoomType, Dp2QtMsgBody, Dp2QtMsg.
//
// Error codes used by the ORIG: -1 TDR_ERR_SHORT_BUF_FOR_WRITE,
// -2 TDR_ERR_SHORT_BUF_FOR_READ, -19 TDR_ERR_NULL_POINTER_PARAMETER.

#include "formmog_proto_tss_sdk_qt.h"

#include <stdarg.h>
#include <stddef.h>
#include <string.h>

// Compile-time layout assertions: the DWARF byte sizes and member offsets of
// the ORIG CU (cu_layout.py, "## <class>" sections).  A violated assertion is
// a negative array bound, i.e. a compile error, so a clean build of this TU
// proves the reconstructed layouts match the ORIG's.
typedef char assert_qt_Login_size[sizeof(sec_proto::Login) == 265 ? 1 : -1];
typedef char assert_qt_Login_uin[offsetof(sec_proto::Login, uin_) == 0 ? 1 : -1];
typedef char assert_qt_Login_server_ip[
    offsetof(sec_proto::Login, server_ip_) == 4 ? 1 : -1];
typedef char assert_qt_Login_name_len[
    offsetof(sec_proto::Login, name_len_) == 8 ? 1 : -1];
typedef char assert_qt_Login_name[
    offsetof(sec_proto::Login, name_) == 9 ? 1 : -1];

typedef char assert_qt_EnterChannel_size[
    sizeof(sec_proto::EnterChannel) == 1 ? 1 : -1];

typedef char assert_qt_CreateRoom_size[
    sizeof(sec_proto::CreateRoom) == 2 ? 1 : -1];
typedef char assert_qt_CreateRoom_visitor_count[
    offsetof(sec_proto::CreateRoom, visitor_count_) == 1 ? 1 : -1];

typedef char assert_qt_CreateRoomRes_size[
    sizeof(sec_proto::CreateRoomRes) == 2 ? 1 : -1];

typedef char assert_qt_EnterRoom_size[
    sizeof(sec_proto::EnterRoom) == 6 ? 1 : -1];
typedef char assert_qt_EnterRoom_role_position[
    offsetof(sec_proto::EnterRoom, role_position_) == 5 ? 1 : -1];

typedef char assert_qt_ChangeRole_size[
    sizeof(sec_proto::ChangeRole) == 1 ? 1 : -1];

typedef char assert_qt_ChangePosition_size[
    sizeof(sec_proto::ChangePosition) == 2 ? 1 : -1];
typedef char assert_qt_ChangePosition_role_type[
    offsetof(sec_proto::ChangePosition, role_type_) == 1 ? 1 : -1];

// revert_ at +1 is the assertion that proves the byte packing of this CU.
typedef char assert_qt_RoleAction_size[
    sizeof(sec_proto::RoleAction) == 5 ? 1 : -1];
typedef char assert_qt_RoleAction_revert[
    offsetof(sec_proto::RoleAction, revert_) == 1 ? 1 : -1];

typedef char assert_qt_RoomMaster_size[
    sizeof(sec_proto::RoomMaster) == 2 ? 1 : -1];
typedef char assert_qt_RoomMaster_master_role_position[
    offsetof(sec_proto::RoomMaster, master_role_position_) == 1 ? 1 : -1];

typedef char assert_qt_ChangeRoomType_size[
    sizeof(sec_proto::ChangeRoomType) == 1 ? 1 : -1];

typedef char assert_qt_Dp2QtMsgBody_size[
    sizeof(sec_proto::Dp2QtMsgBody) == 265 ? 1 : -1];
typedef char assert_qt_Dp2QtMsgBody_login[
    offsetof(sec_proto::Dp2QtMsgBody, login) == 0 ? 1 : -1];
typedef char assert_qt_Dp2QtMsgBody_enter_room[
    offsetof(sec_proto::Dp2QtMsgBody, enter_room) == 0 ? 1 : -1];
typedef char assert_qt_Dp2QtMsgBody_room_type[
    offsetof(sec_proto::Dp2QtMsgBody, room_type) == 0 ? 1 : -1];

typedef char assert_qt_Dp2QtMsg_size[
    sizeof(sec_proto::Dp2QtMsg) == 266 ? 1 : -1];
typedef char assert_qt_Dp2QtMsg_body[
    offsetof(sec_proto::Dp2QtMsg, body_) == 1 ? 1 : -1];

namespace tsf4g_tdr {

// ORIG 0x00136e90, 0x6c bytes, WEAK in the ORIG (nm -DC: 'W').  The ORIG's
// TdrBuf.h defines this varargs wrapper inside the class body, i.e. implicitly
// inline, so every CU that calls it emits its own weak comdat copy and the
// linker folds them; the calls stay out-of-line (a varargs callee is never
// inlined) and go through the PLT.  The compat header declares it out-of-line,
// so the definition repeats the specifier to reproduce the ORIG binding
// without touching the shared header: binding 'W' here, 'T' otherwise.
// textize() forwards to the inlined vtextize(format, ap) at TdrBuf.h:182.
inline TdrError::ErrorType TdrWriteBuf::textize(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    TdrError::ErrorType ret = vtextize(format, ap);
    va_end(ap);
    return ret;
}

} // namespace tsf4g_tdr

namespace sec_proto {

using namespace tsf4g_tdr;

// ---------------------------------------------------------------------------
// Login (sizeof 265)
// ---------------------------------------------------------------------------

// ORIG 0x001621c0, 0x35 bytes. memset(this, 0, sizeof(*this)): the ORIG passes
// size 0x109 = 265 to memset@plt.
TdrError::ErrorType Login::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00163a00, 0x10e bytes. ORIG .cpp: 73/83 uint32, 93 length, 103 bulk
// name_; cutVer is not read, and (unlike the bounded arrays of the other
// messages) the ORIG emits no length > sizeof(name_) check: the only guards
// are the writeUInt8 single-byte check and the writeBytes capacity check.
TdrError::ErrorType Login::pack(TdrWriteBuf& destBuf,
                                unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(server_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeBytes(name_, name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001646c0, 0x85 bytes. ORIG .cpp: ctor at 43, dtor at 50.
TdrError::ErrorType Login::pack(char* buffer, unsigned int size,
                                unsigned int* usedSize,
                                unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163d70, 0x10e bytes. ORIG .cpp: 153/163 uint32, 173 length, 183
// bulk name_.
TdrError::ErrorType Login::unpack(TdrReadBuf& srcBuf,
                                  unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt32(uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(server_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readBytes(name_, name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00163e80, 0x66 bytes. ORIG .cpp: ctor at 123.
TdrError::ErrorType Login::unpack(const char* buffer, unsigned int size,
                                  unsigned int* usedSize,
                                  unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162330, 0x1bb bytes. Field names and formats are the ORIG's own
// .rodata references: [uin] "%u", [server_ip] "%u", [name_len] "%d",
// printArray [name] and a textize(" %d") loop, then
// writeCharWithNull(separator) as the last statement (ORIG .cpp:258).  No
// length > sizeof(name_) check is emitted before the array dump.
TdrError::ErrorType Login::visualize(TdrWriteBuf& destBuf, int indent,
                                     char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[uin]", "%u",
                                    uin_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[server_ip]",
                                    "%u", server_ip_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[name_len]",
                                    "%d", name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printArray(destBuf, indent, separator, "[name]",
                                 name_len_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (name_len_ != 0) {
        for (uint32_t i = 0; i < name_len_; ++i) {
            ret = destBuf.textize(" %d", name_[i]);
            if (ret != TdrError::TDR_NO_ERROR) return ret;
        }
    }
    ret = destBuf.writeCharWithNull(separator);
    return ret;
}

// ORIG 0x00164600, 0xb6 bytes. ORIG .cpp: ctor at 203, dtor at 210.
TdrError::ErrorType Login::visualize(char* buffer, unsigned int size,
                                     unsigned int* usedSize, int indent,
                                     char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001633c0, 0x8b bytes. ORIG .cpp: 305 getUsedSize, 306 getTotalSize,
// 311 writeChar('\0', pos), 313 getBeginPtr.
const char* Login::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00164530, 0xc9 bytes. ORIG .cpp: ctor at 276, getUsedSize at 279,
// getBeginPtr/dtor at 292.
const char* Login::visualize_ex(char* buffer, unsigned int size,
                                unsigned int* usedSize, int indent,
                                char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// EnterChannel (sizeof 1)
// ---------------------------------------------------------------------------

// ORIG 0x001621b0, 0xd bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType EnterChannel::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00162ec0, 0x3b bytes. ORIG .cpp: 366.
TdrError::ErrorType EnterChannel::pack(TdrWriteBuf& destBuf,
                                       unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001644a0, 0x85 bytes. ORIG .cpp: ctor at 336, dtor at 343.
TdrError::ErrorType EnterChannel::pack(char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162cf0, 0x26 bytes. ORIG .cpp: 416.
TdrError::ErrorType EnterChannel::unpack(TdrReadBuf& srcBuf,
                                         unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(channel_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001631a0, 0x66 bytes. ORIG .cpp: ctor at 386.
TdrError::ErrorType EnterChannel::unpack(const char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162080, 0x50 bytes. ORIG .rodata: [channel_id] "%d".
TdrError::ErrorType EnterChannel::visualize(TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[channel_id]",
                                    "%d", channel_id_);
    return ret;
}

// ORIG 0x001643e0, 0xb6 bytes. ORIG .cpp: ctor at 436, dtor at 443.
TdrError::ErrorType EnterChannel::visualize(char* buffer, unsigned int size,
                                            unsigned int* usedSize, int indent,
                                            char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163330, 0x8b bytes. ORIG .cpp: 499 getUsedSize, 500 getTotalSize,
// 505 writeChar('\0', pos), 507 getBeginPtr.
const char* EnterChannel::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                       char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00165b10, 0xc9 bytes. ORIG .cpp: ctor at 470, getUsedSize at 473,
// getBeginPtr/dtor at 486.
const char* EnterChannel::visualize_ex(char* buffer, unsigned int size,
                                       unsigned int* usedSize, int indent,
                                       char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// CreateRoom (sizeof 2)
// ---------------------------------------------------------------------------

// ORIG 0x001621a0, 0xf bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType CreateRoom::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00162e60, 0x5c bytes. ORIG .cpp: 560/570.
TdrError::ErrorType CreateRoom::pack(TdrWriteBuf& destBuf,
                                     unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(visitor_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00165a80, 0x85 bytes. ORIG .cpp: ctor at 540, dtor at 547.
TdrError::ErrorType CreateRoom::pack(char* buffer, unsigned int size,
                                     unsigned int* usedSize,
                                     unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162ca0, 0x44 bytes. ORIG .cpp: 620/630.
TdrError::ErrorType CreateRoom::unpack(TdrReadBuf& srcBuf,
                                       unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(visitor_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00163130, 0x66 bytes. ORIG .cpp: ctor at 590.
TdrError::ErrorType CreateRoom::unpack(const char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161fe0, 0x92 bytes. ORIG .rodata: [room_type] "%d",
// [visitor_count] "%d" (the format operand is shared by both calls).
TdrError::ErrorType CreateRoom::visualize(TdrWriteBuf& destBuf, int indent,
                                          char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_type]",
                                    "%d", room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[visitor_count]", "%d", visitor_count_);
    return ret;
}

// ORIG 0x001659c0, 0xb6 bytes. ORIG .cpp: ctor at 650, dtor at 657.
TdrError::ErrorType CreateRoom::visualize(char* buffer, unsigned int size,
                                          unsigned int* usedSize, int indent,
                                          char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001632a0, 0x8b bytes. ORIG .cpp: 721 getUsedSize, 722 getTotalSize,
// 727 writeChar('\0', pos), 729 getBeginPtr.
const char* CreateRoom::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                     char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x001658f0, 0xc9 bytes. ORIG .cpp: ctor at 692, getUsedSize at 695,
// getBeginPtr/dtor at 708.
const char* CreateRoom::visualize_ex(char* buffer, unsigned int size,
                                     unsigned int* usedSize, int indent,
                                     char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// CreateRoomRes (sizeof 2)
// ---------------------------------------------------------------------------

// ORIG 0x00162190, 0xf bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType CreateRoomRes::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00161b40, 0x4e bytes. ORIG .cpp: 782.
TdrError::ErrorType CreateRoomRes::pack(TdrWriteBuf& destBuf,
                                        unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00165860, 0x85 bytes. ORIG .cpp: ctor at 752, dtor at 759.
TdrError::ErrorType CreateRoomRes::pack(char* buffer, unsigned int size,
                                        unsigned int* usedSize,
                                        unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161b90, 0x3a bytes. ORIG .cpp: 832.
TdrError::ErrorType CreateRoomRes::unpack(TdrReadBuf& srcBuf,
                                          unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001630c0, 0x66 bytes. ORIG .cpp: ctor at 802.
TdrError::ErrorType CreateRoomRes::unpack(const char* buffer,
                                          unsigned int size,
                                          unsigned int* usedSize,
                                          unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161f90, 0x50 bytes. ORIG .rodata: [room_id] "%d".
TdrError::ErrorType CreateRoomRes::visualize(TdrWriteBuf& destBuf, int indent,
                                             char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id]",
                                    "%d", room_id_);
    return ret;
}

// ORIG 0x001657a0, 0xb6 bytes. ORIG .cpp: ctor at 852, dtor at 859.
TdrError::ErrorType CreateRoomRes::visualize(char* buffer, unsigned int size,
                                             unsigned int* usedSize,
                                             int indent,
                                             char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163210, 0x8b bytes. ORIG .cpp: 915 getUsedSize, 916 getTotalSize,
// 921 writeChar('\0', pos), 923 getBeginPtr.
const char* CreateRoomRes::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                        char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x001656d0, 0xc9 bytes. ORIG .cpp: ctor at 886, getUsedSize at 889,
// getBeginPtr/dtor at 902.
const char* CreateRoomRes::visualize_ex(char* buffer, unsigned int size,
                                        unsigned int* usedSize, int indent,
                                        char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// EnterRoom (sizeof 6)
// ---------------------------------------------------------------------------

// ORIG 0x00162170, 0x16 bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType EnterRoom::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x001637b0, 0xb0 bytes. ORIG .cpp: 976 room_id_, 986/996/1006/1016.
TdrError::ErrorType EnterRoom::pack(TdrWriteBuf& destBuf,
                                    unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(visitor_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00165640, 0x85 bytes. ORIG .cpp: ctor at 946, dtor at 953.
TdrError::ErrorType EnterRoom::pack(char* buffer, unsigned int size,
                                    unsigned int* usedSize,
                                    unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163860, 0x9b bytes. ORIG .cpp: 1066 room_id_, 1076/1086/1096/1106.
TdrError::ErrorType EnterRoom::unpack(TdrReadBuf& srcBuf,
                                      unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt16(room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(visitor_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00163900, 0x66 bytes. ORIG .cpp: ctor at 1036.
TdrError::ErrorType EnterRoom::unpack(const char* buffer, unsigned int size,
                                      unsigned int* usedSize,
                                      unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161e50, 0x134 bytes. ORIG .rodata: [room_id] "%d", then
// [room_type], [visitor_count], [role_type], [role_position], all with the
// same "%d" format operand.
TdrError::ErrorType EnterRoom::visualize(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_id]",
                                    "%d", room_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_type]",
                                    "%d", room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[visitor_count]", "%d", visitor_count_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[role_type]",
                                    "%d", role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[role_position]", "%d", role_position_);
    return ret;
}

// ORIG 0x00165580, 0xb6 bytes. ORIG .cpp: ctor at 1126, dtor at 1133.
TdrError::ErrorType EnterRoom::visualize(char* buffer, unsigned int size,
                                         unsigned int* usedSize, int indent,
                                         char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162b10, 0x8b bytes. ORIG .cpp: 1221 getUsedSize, 1222 getTotalSize,
// 1227 writeChar('\0', pos), 1229 getBeginPtr.
const char* EnterRoom::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                    char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x001654b0, 0xc9 bytes. ORIG .cpp: ctor at 1192, getUsedSize at 1195,
// getBeginPtr/dtor at 1208.
const char* EnterRoom::visualize_ex(char* buffer, unsigned int size,
                                    unsigned int* usedSize, int indent,
                                    char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// ChangeRole (sizeof 1)
// ---------------------------------------------------------------------------

// ORIG 0x00162160, 0xd bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType ChangeRole::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00162e20, 0x3b bytes. ORIG .cpp: 1282.
TdrError::ErrorType ChangeRole::pack(TdrWriteBuf& destBuf,
                                     unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00165420, 0x85 bytes. ORIG .cpp: ctor at 1252, dtor at 1259.
TdrError::ErrorType ChangeRole::pack(char* buffer, unsigned int size,
                                     unsigned int* usedSize,
                                     unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162c70, 0x26 bytes. ORIG .cpp: 1332.
TdrError::ErrorType ChangeRole::unpack(TdrReadBuf& srcBuf,
                                       unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00163050, 0x66 bytes. ORIG .cpp: ctor at 1302.
TdrError::ErrorType ChangeRole::unpack(const char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161e00, 0x50 bytes. ORIG .rodata: [role_position] "%d".
TdrError::ErrorType ChangeRole::visualize(TdrWriteBuf& destBuf, int indent,
                                          char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[role_position]", "%d", role_position_);
    return ret;
}

// ORIG 0x00165360, 0xb6 bytes. ORIG .cpp: ctor at 1352, dtor at 1359.
TdrError::ErrorType ChangeRole::visualize(char* buffer, unsigned int size,
                                          unsigned int* usedSize, int indent,
                                          char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163720, 0x8b bytes. ORIG .cpp: 1415 getUsedSize, 1416 getTotalSize,
// 1421 writeChar('\0', pos), 1423 getBeginPtr.
const char* ChangeRole::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                     char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00165290, 0xc9 bytes. ORIG .cpp: ctor at 1386, getUsedSize at 1389,
// getBeginPtr/dtor at 1402.
const char* ChangeRole::visualize_ex(char* buffer, unsigned int size,
                                     unsigned int* usedSize, int indent,
                                     char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// ChangePosition (sizeof 2)
// ---------------------------------------------------------------------------

// ORIG 0x00162150, 0xf bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType ChangePosition::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00162dc0, 0x5c bytes. ORIG .cpp: 1476/1486.
TdrError::ErrorType ChangePosition::pack(TdrWriteBuf& destBuf,
                                         unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00165200, 0x85 bytes. ORIG .cpp: ctor at 1446, dtor at 1453.
TdrError::ErrorType ChangePosition::pack(char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162c20, 0x44 bytes. ORIG .cpp: 1536/1546.
TdrError::ErrorType ChangePosition::unpack(TdrReadBuf& srcBuf,
                                           unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00162fe0, 0x66 bytes. ORIG .cpp: ctor at 1506.
TdrError::ErrorType ChangePosition::unpack(const char* buffer,
                                           unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161d60, 0x92 bytes. ORIG .rodata: [role_position] "%d",
// [role_type] with the same "%d" operand.
TdrError::ErrorType ChangePosition::visualize(TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[role_position]", "%d", role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[role_type]",
                                    "%d", role_type_);
    return ret;
}

// ORIG 0x00165140, 0xb6 bytes. ORIG .cpp: ctor at 1566, dtor at 1573.
TdrError::ErrorType ChangePosition::visualize(char* buffer, unsigned int size,
                                              unsigned int* usedSize,
                                              int indent,
                                              char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163690, 0x8b bytes. ORIG .cpp: 1637 getUsedSize, 1638 getTotalSize,
// 1643 writeChar('\0', pos), 1645 getBeginPtr.
const char* ChangePosition::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00165070, 0xc9 bytes. ORIG .cpp: ctor at 1608, getUsedSize at 1611,
// getBeginPtr/dtor at 1624.
const char* ChangePosition::visualize_ex(char* buffer, unsigned int size,
                                         unsigned int* usedSize, int indent,
                                         char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// RoleAction (sizeof 5: role_action_ at 0, revert_ at +1)
// ---------------------------------------------------------------------------

// ORIG 0x00162130, 0x14 bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType RoleAction::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00163970, 0x8f bytes. ORIG .cpp: 1698 role_action_, 1708 revert_.
TdrError::ErrorType RoleAction::pack(TdrWriteBuf& destBuf,
                                     unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(role_action_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt32(revert_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00164fe0, 0x85 bytes. ORIG .cpp: ctor at 1668, dtor at 1675.
TdrError::ErrorType RoleAction::pack(char* buffer, unsigned int size,
                                     unsigned int* usedSize,
                                     unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163ef0, 0x76 bytes. ORIG .cpp: 1758 role_action_, 1768 revert_.
TdrError::ErrorType RoleAction::unpack(TdrReadBuf& srcBuf,
                                       unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(role_action_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt32(revert_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x001642a0, 0x66 bytes. ORIG .cpp: ctor at 1728.
TdrError::ErrorType RoleAction::unpack(const char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161cc0, 0x94 bytes. ORIG .rodata: [role_action] "%d" (uint8_t),
// [revert] "%u" (uint32_t).
TdrError::ErrorType RoleAction::visualize(TdrWriteBuf& destBuf, int indent,
                                          char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[role_action]",
                                    "%d", role_action_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[revert]",
                                    "%u", revert_);
    return ret;
}

// ORIG 0x00164f20, 0xb6 bytes. ORIG .cpp: ctor at 1788, dtor at 1795.
TdrError::ErrorType RoleAction::visualize(char* buffer, unsigned int size,
                                          unsigned int* usedSize, int indent,
                                          char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163600, 0x8b bytes. ORIG .cpp: 1859 getUsedSize, 1860 getTotalSize,
// 1865 writeChar('\0', pos), 1867 getBeginPtr.
const char* RoleAction::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                     char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00164e50, 0xc9 bytes. ORIG .cpp: ctor at 1830, getUsedSize at 1833,
// getBeginPtr/dtor at 1846.
const char* RoleAction::visualize_ex(char* buffer, unsigned int size,
                                     unsigned int* usedSize, int indent,
                                     char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// RoomMaster (sizeof 2)
// ---------------------------------------------------------------------------

// ORIG 0x00162120, 0xf bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType RoomMaster::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00162d60, 0x5c bytes. ORIG .cpp: 1920/1930.
TdrError::ErrorType RoomMaster::pack(TdrWriteBuf& destBuf,
                                     unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(master_role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = destBuf.writeUInt8(master_role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00164dc0, 0x85 bytes. ORIG .cpp: ctor at 1890, dtor at 1897.
TdrError::ErrorType RoomMaster::pack(char* buffer, unsigned int size,
                                     unsigned int* usedSize,
                                     unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162bd0, 0x44 bytes. ORIG .cpp: 1980/1990.
TdrError::ErrorType RoomMaster::unpack(TdrReadBuf& srcBuf,
                                       unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(master_role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = srcBuf.readUInt8(master_role_position_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00162f70, 0x66 bytes. ORIG .cpp: ctor at 1950.
TdrError::ErrorType RoomMaster::unpack(const char* buffer, unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161c20, 0x92 bytes. ORIG .rodata: [master_role_type] "%d",
// [master_role_position] with the same "%d" operand.
TdrError::ErrorType RoomMaster::visualize(TdrWriteBuf& destBuf, int indent,
                                          char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[master_role_type]", "%d",
                                    master_role_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                    "[master_role_position]", "%d",
                                    master_role_position_);
    return ret;
}

// ORIG 0x00164d00, 0xb6 bytes. ORIG .cpp: ctor at 2010, dtor at 2017.
TdrError::ErrorType RoomMaster::visualize(char* buffer, unsigned int size,
                                          unsigned int* usedSize, int indent,
                                          char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163570, 0x8b bytes. ORIG .cpp: 2081 getUsedSize, 2082 getTotalSize,
// 2087 writeChar('\0', pos), 2089 getBeginPtr.
const char* RoomMaster::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                     char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00164c30, 0xc9 bytes. ORIG .cpp: ctor at 2052, getUsedSize at 2055,
// getBeginPtr/dtor at 2068.
const char* RoomMaster::visualize_ex(char* buffer, unsigned int size,
                                     unsigned int* usedSize, int indent,
                                     char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// ChangeRoomType (sizeof 1)
// ---------------------------------------------------------------------------

// ORIG 0x00162110, 0xd bytes. memset(this, 0, sizeof(*this)).
TdrError::ErrorType ChangeRoomType::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00162d20, 0x3b bytes. ORIG .cpp: 2142.
TdrError::ErrorType ChangeRoomType::pack(TdrWriteBuf& destBuf,
                                         unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00164ba0, 0x85 bytes. ORIG .cpp: ctor at 2112, dtor at 2119.
TdrError::ErrorType ChangeRoomType::pack(char* buffer, unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162ba0, 0x26 bytes. ORIG .cpp: 2192.
TdrError::ErrorType ChangeRoomType::unpack(TdrReadBuf& srcBuf,
                                           unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(room_type_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    return ret;
}

// ORIG 0x00162f00, 0x66 bytes. ORIG .cpp: ctor at 2162.
TdrError::ErrorType ChangeRoomType::unpack(const char* buffer,
                                           unsigned int size,
                                           unsigned int* usedSize,
                                           unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00161bd0, 0x50 bytes. ORIG .rodata: [room_type] "%d".
TdrError::ErrorType ChangeRoomType::visualize(TdrWriteBuf& destBuf, int indent,
                                              char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[room_type]",
                                    "%d", room_type_);
    return ret;
}

// ORIG 0x00164ae0, 0xb6 bytes. ORIG .cpp: ctor at 2212, dtor at 2219.
TdrError::ErrorType ChangeRoomType::visualize(char* buffer, unsigned int size,
                                              unsigned int* usedSize,
                                              int indent,
                                              char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x001634e0, 0x8b bytes. ORIG .cpp: 2275 getUsedSize, 2276 getTotalSize,
// 2281 writeChar('\0', pos), 2283 getBeginPtr.
const char* ChangeRoomType::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                         char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00164a10, 0xc9 bytes. ORIG .cpp: ctor at 2246, getUsedSize at 2249,
// getBeginPtr/dtor at 2262.
const char* ChangeRoomType::visualize_ex(char* buffer, unsigned int size,
                                         unsigned int* usedSize, int indent,
                                         char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

// ---------------------------------------------------------------------------
// Dp2QtMsgBody (anonymous union of the ten bodies, sizeof 265)
//
// Union arm selectors are 1..10 in declaration order (login, enter_channel,
// create_room, create_room_res, enter_room, change_role, change_position,
// role_action, room_master, room_type); unknown selectors return
// TDR_NO_ERROR, as in the ORIG (xor $0x1..$0xa on the low selector word
// OR-ed with the high word).
//
// Unlike the uic CU's union dispatcher there is no cutVer clamp here: the ORIG
// never reads its cutVer parameter and hands the constant 1 to every arm call
// (ORIG: movl $0x1,0x8(%esp) in all twenty pack/unpack cases).  The secsvr
// generator family emits the same literal for its union arms
// (formmog_proto_antibot_client.cpp: TROJAN_REPORT::pack -> trojans_.pack(destBuf, 1)).
// ---------------------------------------------------------------------------

// ORIG 0x00162200, 0x130 bytes.
//
// Codegen compensation: the ORIG expands this ten-arm dispatcher as a decision
// tree (ORIG: `mov %edx,%eax; xor $0x1,%eax; or %ecx,%eax; je <arm>` for every
// arm, with all ten comparisons before the arm bodies).  GCC 4.1.2 instead
// turns the same ten-case 64-bit switch into a jump table (measured 261 bytes
// against the ORIG's 304), so the dispatcher is spelled as the equivalent
// if/else-if chain; the arms, the selector values and the default result are
// unchanged.  The union_selector argument has no side effects and both forms
// evaluate each arm's comparison at most once.
TdrError::ErrorType Dp2QtMsgBody::construct(int64_t union_selector) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (1 == union_selector) {
        ret = login.construct();
    } else if (2 == union_selector) {
        ret = enter_channel.construct();
    } else if (3 == union_selector) {
        ret = create_room.construct();
    } else if (4 == union_selector) {
        ret = create_room_res.construct();
    } else if (5 == union_selector) {
        ret = enter_room.construct();
    } else if (6 == union_selector) {
        ret = change_role.construct();
    } else if (7 == union_selector) {
        ret = change_position.construct();
    } else if (8 == union_selector) {
        ret = role_action.construct();
    } else if (9 == union_selector) {
        ret = room_master.construct();
    } else if (10 == union_selector) {
        ret = room_type.construct();
    } else {
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG 0x00163b10, 0x1d4 bytes.  Same decision-tree compensation as
// construct() above; every arm call passes the ORIG's constant cutVer 1.
TdrError::ErrorType Dp2QtMsgBody::pack(int64_t union_selector,
                                       TdrWriteBuf& destBuf,
                                       unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (1 == union_selector) {
        ret = login.pack(destBuf, 1);
    } else if (2 == union_selector) {
        ret = enter_channel.pack(destBuf, 1);
    } else if (3 == union_selector) {
        ret = create_room.pack(destBuf, 1);
    } else if (4 == union_selector) {
        ret = create_room_res.pack(destBuf, 1);
    } else if (5 == union_selector) {
        ret = enter_room.pack(destBuf, 1);
    } else if (6 == union_selector) {
        ret = change_role.pack(destBuf, 1);
    } else if (7 == union_selector) {
        ret = change_position.pack(destBuf, 1);
    } else if (8 == union_selector) {
        ret = role_action.pack(destBuf, 1);
    } else if (9 == union_selector) {
        ret = room_master.pack(destBuf, 1);
    } else if (10 == union_selector) {
        ret = room_type.pack(destBuf, 1);
    } else {
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG 0x00164970, 0x93 bytes. ORIG .cpp: ctor at 2395, dtor at 2402.
TdrError::ErrorType Dp2QtMsgBody::pack(int64_t union_selector, char* buffer,
                                       unsigned int size,
                                       unsigned int* usedSize,
                                       unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(union_selector, destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163f70, 0x1d4 bytes.  Same decision-tree compensation as
// construct() above.
TdrError::ErrorType Dp2QtMsgBody::unpack(int64_t union_selector,
                                         TdrReadBuf& srcBuf,
                                         unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    if (1 == union_selector) {
        ret = login.unpack(srcBuf, 1);
    } else if (2 == union_selector) {
        ret = enter_channel.unpack(srcBuf, 1);
    } else if (3 == union_selector) {
        ret = create_room.unpack(srcBuf, 1);
    } else if (4 == union_selector) {
        ret = create_room_res.unpack(srcBuf, 1);
    } else if (5 == union_selector) {
        ret = enter_room.unpack(srcBuf, 1);
    } else if (6 == union_selector) {
        ret = change_role.unpack(srcBuf, 1);
    } else if (7 == union_selector) {
        ret = change_position.unpack(srcBuf, 1);
    } else if (8 == union_selector) {
        ret = role_action.unpack(srcBuf, 1);
    } else if (9 == union_selector) {
        ret = room_master.unpack(srcBuf, 1);
    } else if (10 == union_selector) {
        ret = room_type.unpack(srcBuf, 1);
    } else {
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG 0x00164220, 0x7a bytes. ORIG .cpp: ctor at 2526.
TdrError::ErrorType Dp2QtMsgBody::unpack(int64_t union_selector,
                                         const char* buffer,
                                         unsigned int size,
                                         unsigned int* usedSize,
                                         unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(union_selector, srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x001624f0, 0x547 bytes. ORIG .rodata arm names: [login],
// [enter_channel], [create_room], [create_room_res], [enter_room],
// [change_role], [change_position], [role_action], [room_master], [room_type];
// each case prints the arm name with printVariable(..., name, true) and then
// descends with indent >= 0 ? indent + 1 : indent.
TdrError::ErrorType Dp2QtMsgBody::visualize(int64_t union_selector,
                                            TdrWriteBuf& destBuf, int indent,
                                            char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    switch (union_selector) {
    case 1:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[login]",
                                        true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = login.visualize(destBuf, indent + 1, separator);
        } else {
            ret = login.visualize(destBuf, indent, separator);
        }
        break;
    case 2:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[enter_channel]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = enter_channel.visualize(destBuf, indent + 1, separator);
        } else {
            ret = enter_channel.visualize(destBuf, indent, separator);
        }
        break;
    case 3:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[create_room]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = create_room.visualize(destBuf, indent + 1, separator);
        } else {
            ret = create_room.visualize(destBuf, indent, separator);
        }
        break;
    case 4:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[create_room_res]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = create_room_res.visualize(destBuf, indent + 1, separator);
        } else {
            ret = create_room_res.visualize(destBuf, indent, separator);
        }
        break;
    case 5:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[enter_room]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = enter_room.visualize(destBuf, indent + 1, separator);
        } else {
            ret = enter_room.visualize(destBuf, indent, separator);
        }
        break;
    case 6:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[change_role]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = change_role.visualize(destBuf, indent + 1, separator);
        } else {
            ret = change_role.visualize(destBuf, indent, separator);
        }
        break;
    case 7:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[change_position]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = change_position.visualize(destBuf, indent + 1, separator);
        } else {
            ret = change_position.visualize(destBuf, indent, separator);
        }
        break;
    case 8:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[role_action]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = role_action.visualize(destBuf, indent + 1, separator);
        } else {
            ret = role_action.visualize(destBuf, indent, separator);
        }
        break;
    case 9:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[room_master]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = room_master.visualize(destBuf, indent + 1, separator);
        } else {
            ret = room_master.visualize(destBuf, indent, separator);
        }
        break;
    case 10:
        ret = TdrBufUtil::printVariable(destBuf, indent, separator,
                                        "[room_type]", true);
        if (ret != TdrError::TDR_NO_ERROR) return ret;
        if (indent >= 0) {
            ret = room_type.visualize(destBuf, indent + 1, separator);
        } else {
            ret = room_type.visualize(destBuf, indent, separator);
        }
        break;
    default:
        ret = TdrError::TDR_NO_ERROR;
    }
    return ret;
}

// ORIG 0x001648a0, 0xc6 bytes. ORIG .cpp: ctor at 2657, dtor at 2664.
TdrError::ErrorType Dp2QtMsgBody::visualize(int64_t union_selector,
                                            char* buffer, unsigned int size,
                                            unsigned int* usedSize, int indent,
                                            char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(union_selector, destBuf, indent,
                                        separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ---------------------------------------------------------------------------
// Dp2QtMsg (cmd_id_ at 0, body_ at 1, sizeof 266)
// ---------------------------------------------------------------------------

// ORIG 0x001620d0, 0x35 bytes. memset(this, 0, sizeof(*this)): the ORIG passes
// size 0x10a = 266 to memset@plt.
TdrError::ErrorType Dp2QtMsg::construct() {
    memset(this, 0, sizeof(*this));
    return TdrError::TDR_NO_ERROR;
}

// ORIG 0x00163cf0, 0x73 bytes. ORIG .cpp: 2956 cmd_id_; the union body is
// dispatched with the wire selector cmd_id_ widened to int64_t and the same
// constant 1 cutVer as the arm calls use.
TdrError::ErrorType Dp2QtMsg::pack(TdrWriteBuf& destBuf,
                                   unsigned int cutVer) const throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = destBuf.writeUInt8(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.pack((int64_t)cmd_id_, destBuf, 1);
    return ret;
}

// ORIG 0x00164810, 0x85 bytes. ORIG .cpp: ctor at 2926, dtor at 2933.
TdrError::ErrorType Dp2QtMsg::pack(char* buffer, unsigned int size,
                                   unsigned int* usedSize,
                                   unsigned int cutVer) const throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00164150, 0x5f bytes. ORIG .cpp: 3017 cmd_id_; no pkg_len/version
// peek (this message has no head), unlike the uic CU's UserInputCheckPkg.
TdrError::ErrorType Dp2QtMsg::unpack(TdrReadBuf& srcBuf,
                                     unsigned int cutVer) throw() {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = srcBuf.readUInt8(cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = body_.unpack((int64_t)cmd_id_, srcBuf, 1);
    return ret;
}

// ORIG 0x001641b0, 0x66 bytes. ORIG .cpp: ctor at 2987.
TdrError::ErrorType Dp2QtMsg::unpack(const char* buffer, unsigned int size,
                                     unsigned int* usedSize,
                                     unsigned int cutVer) throw() {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrReadBuf srcBuf(buffer, size);
    TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) *usedSize = srcBuf.getUsedSize();
    return ret;
}

// ORIG 0x00162a40, 0xcc bytes. ORIG .rodata: [cmd_id] "%d", [body] as a
// nested object (printVariable(..., bool)); the body is visualized with the
// wire selector (int64_t)cmd_id_.
TdrError::ErrorType Dp2QtMsg::visualize(TdrWriteBuf& destBuf, int indent,
                                        char separator) const {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[cmd_id]",
                                    "%d", cmd_id_);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    ret = TdrBufUtil::printVariable(destBuf, indent, separator, "[body]",
                                    true);
    if (ret != TdrError::TDR_NO_ERROR) return ret;
    if (indent >= 0) {
        ret = body_.visualize((int64_t)cmd_id_, destBuf, indent + 1,
                              separator);
    } else {
        ret = body_.visualize((int64_t)cmd_id_, destBuf, indent, separator);
    }
    return ret;
}

// ORIG 0x00164750, 0xb6 bytes. ORIG .cpp: ctor at 3048, dtor at 3055.
TdrError::ErrorType Dp2QtMsg::visualize(char* buffer, unsigned int size,
                                        unsigned int* usedSize, int indent,
                                        char separator) const {
    if (buffer == NULL) return TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    TdrWriteBuf destBuf(buffer, size);
    TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return ret;
}

// ORIG 0x00163450, 0x8b bytes. ORIG .cpp: 3134 getUsedSize, 3135 getTotalSize,
// 3140 writeChar('\0', pos), 3142 getBeginPtr.
const char* Dp2QtMsg::visualize_ex(TdrWriteBuf& destBuf, int indent,
                                   char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) return "";
    visualize(destBuf, indent, separator);
    size_t usedSize = destBuf.getUsedSize();
    size_t totalSize = destBuf.getTotalSize();
    destBuf.writeChar('\0', usedSize < totalSize ? usedSize : totalSize - 1);
    return destBuf.getBeginPtr();
}

// ORIG 0x00164310, 0xc9 bytes. ORIG .cpp: ctor at 3105, getUsedSize at 3108,
// getBeginPtr/dtor at 3121.
const char* Dp2QtMsg::visualize_ex(char* buffer, unsigned int size,
                                   unsigned int* usedSize, int indent,
                                   char separator) const {
    if (buffer == NULL || size == 0) return "";
    TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.getUsedSize();
    const size_t totalSize = size;
    if (targetPos >= totalSize) targetPos = totalSize - 1;
    buffer[targetPos] = '\0';
    if (usedSize != NULL) *usedSize = destBuf.getUsedSize();
    return destBuf.getBeginPtr();
}

} // namespace sec_proto
