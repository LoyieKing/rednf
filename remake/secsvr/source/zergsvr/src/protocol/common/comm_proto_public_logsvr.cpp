// Reconstructed from gunnersvr disassembly (DWARF offsets + behavior check).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/comm_proto_public_logsvr.cpp

#include "src/protocol/common/comm_proto_public_logsvr.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <string.h>

namespace sec_proto {

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::construct() {
    tsf4g_tdr::TdrError::ErrorType ret;
    //string_buffer_
    memset(string_buffer_, 0, sizeof(string_buffer_));
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::pack(tsf4g_tdr::TdrWriteBuf &destBuf, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret;
    //string_buffer_
    const size_t sizePos4string_buffer_ = destBuf.size();
    const size_t beginPos4string_buffer_ = destBuf.size() + 4;
    const_cast<char *>(string_buffer_)[sizeof(string_buffer_) - 1] = '\0';
    ret = destBuf.writeString(string_buffer_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    const size_t size4string_buffer_ = destBuf.size() - sizePos4string_buffer_ - 4;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::pack(char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) const {
    if (buffer == NULL) {
        return tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    }
    tsf4g_tdr::TdrWriteBuf destBuf(buffer, size);
    tsf4g_tdr::TdrError::ErrorType ret = pack(destBuf, cutVer);
    if (usedSize != NULL) {
        *usedSize = destBuf.size();
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::unpack(tsf4g_tdr::TdrReadBuf &srcBuf, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret;
    //string_buffer_
    ret = srcBuf.readString(string_buffer_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::unpack(const char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) {
    if (buffer == NULL) {
        return tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    }
    tsf4g_tdr::TdrReadBuf srcBuf(buffer, size);
    tsf4g_tdr::TdrError::ErrorType ret = unpack(srcBuf, cutVer);
    if (usedSize != NULL) {
        *usedSize = srcBuf.size();
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::visualize(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const {
    tsf4g_tdr::TdrError::ErrorType ret;
    //string_buffer_
    ret = tsf4g_tdr::TdrBufUtil::printString(destBuf, indent, separator, "[string_buffer_]", string_buffer_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType LogSvrCommBillRecord::visualize(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const {
    if (buffer == NULL) {
        return tsf4g_tdr::TdrError::TDR_ERR_NULL_POINTER_PARAMETER;
    }
    tsf4g_tdr::TdrWriteBuf destBuf(buffer, size);
    tsf4g_tdr::TdrError::ErrorType ret = visualize(destBuf, indent, separator);
    if (usedSize != NULL) {
        *usedSize = destBuf.size();
    }
    return ret;
}

const char * LogSvrCommBillRecord::visualize_ex(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const {
    if (destBuf.data() == NULL || destBuf.capacity() == 0) {
        return "";
    }
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) {
        targetPos = totalSize - 1;
    }
    const_cast<char *>(destBuf.data())[targetPos] = '\0';
    return destBuf.data();
}

const char * LogSvrCommBillRecord::visualize_ex(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const {
    if (buffer == NULL || size == 0) {
        return "";
    }
    tsf4g_tdr::TdrWriteBuf destBuf(buffer, size);
    visualize(destBuf, indent, separator);
    size_t targetPos = destBuf.size();
    const size_t totalSize = destBuf.capacity();
    if (targetPos >= totalSize) {
        targetPos = totalSize - 1;
    }
    const_cast<char *>(destBuf.data())[targetPos] = '\0';
    if (usedSize != NULL) {
        *usedSize = destBuf.size();
    }
    return destBuf.data();
}

} // namespace sec_proto
