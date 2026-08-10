// Reconstructed from gunnersvr disassembly (DWARF offsets + behavior check).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/comm_proto_public_monitorsvr.cpp

#include "src/protocol/common/comm_proto_public_monitorsvr.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <string.h>

namespace sec_proto {

tsf4g_tdr::TdrError::ErrorType MonitorItem::construct() {
    tsf4g_tdr::TdrError::ErrorType ret;
    feature_id_ = 0;
    app_id_ = 0;
    classify_id_ = 0;
    feature_value_ = 0;
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorItem::pack(tsf4g_tdr::TdrWriteBuf &destBuf, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret;
    //feature_id_
    ret = destBuf.writeUInt32(feature_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //app_id_
    ret = destBuf.writeUInt32(app_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //classify_id_
    ret = destBuf.writeUInt32(classify_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //feature_value_
    ret = destBuf.writeUInt64(feature_value_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorItem::pack(char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType MonitorItem::unpack(tsf4g_tdr::TdrReadBuf &srcBuf, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret;
    //feature_id_
    ret = srcBuf.readUInt32(feature_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //app_id_
    ret = srcBuf.readUInt32(app_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //classify_id_
    ret = srcBuf.readUInt32(classify_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //feature_value_
    ret = srcBuf.readUInt64(feature_value_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorItem::unpack(const char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType MonitorItem::visualize(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const {
    tsf4g_tdr::TdrError::ErrorType ret;
    //feature_id_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[feature_id_]", "%u", feature_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //app_id_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[app_id_]", "%u", app_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //classify_id_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[classify_id_]", "%u", classify_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //feature_value_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[feature_value_]", "%llu", feature_value_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorItem::visualize(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const {
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

const char * MonitorItem::visualize_ex(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const {
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

const char * MonitorItem::visualize_ex(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const {
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

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::construct() {
    tsf4g_tdr::TdrError::ErrorType ret;
    uint16_t item__i;
    version_ = 1;
    report_time_ = 0;
    service_type_ = 0;
    service_id_ = 0;
    send_ip_ = 0;
    count_ = 0;
    for (item__i = 0; item__i < 2048; ++item__i) {
        ret = item_[item__i].construct();
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return ret;
}

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::pack(tsf4g_tdr::TdrWriteBuf &destBuf, unsigned int cutVer) const {
    tsf4g_tdr::TdrError::ErrorType ret;
    uint16_t item__i;
    //version_
    ret = destBuf.writeUInt16(version_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //report_time_
    ret = destBuf.writeUInt32(report_time_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //service_type_
    ret = destBuf.writeUInt32(service_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //service_id_
    ret = destBuf.writeUInt32(service_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //send_ip_
    ret = destBuf.writeUInt32(send_ip_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //count_
    ret = destBuf.writeUInt16(count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (count_ > 2048) {
        return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    for (item__i = 0; item__i < count_; ++item__i) {
        ret = item_[item__i].pack(destBuf, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::pack(char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) const {
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

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::unpack(tsf4g_tdr::TdrReadBuf &srcBuf, unsigned int cutVer) {
    tsf4g_tdr::TdrError::ErrorType ret;
    uint16_t item__i;
    //version_
    ret = srcBuf.readUInt16(version_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //report_time_
    ret = srcBuf.readUInt32(report_time_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //service_type_
    ret = srcBuf.readUInt32(service_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //service_id_
    ret = srcBuf.readUInt32(service_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //send_ip_
    ret = srcBuf.readUInt32(send_ip_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //count_
    ret = srcBuf.readUInt16(count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (count_ > 2048) {
        return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    for (item__i = 0; item__i < count_; ++item__i) {
        ret = item_[item__i].unpack(srcBuf, 1);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::unpack(const char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) {
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

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::visualize(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const {
    tsf4g_tdr::TdrError::ErrorType ret;
    uint16_t item__i;
    //version_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[version_]", "%d", version_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //report_time_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[report_time_]", "%u", report_time_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //service_type_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[service_type_]", "%u", service_type_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //service_id_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[service_id_]", "%u", service_id_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //send_ip_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[send_ip_]", "%u", send_ip_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    //count_
    ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[count_]", "%d", count_);
    if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (count_ > 2048) {
        return tsf4g_tdr::TdrError::TDR_ERR_REFER_SURPASS_COUNT;
    }
    for (item__i = 0; item__i < count_; ++item__i) {
        ret = tsf4g_tdr::TdrBufUtil::printVariable(destBuf, indent, separator, "[item_]", item__i, true);
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (indent >= 0) {
            ret = item_[item__i].visualize(destBuf, indent + 1, separator);
        } else {
            ret = item_[item__i].visualize(destBuf, indent, separator);
        }
        if (ret != tsf4g_tdr::TdrError::TDR_NO_ERROR) {
            return ret;
        }
    }
    return tsf4g_tdr::TdrError::TDR_NO_ERROR;
}

tsf4g_tdr::TdrError::ErrorType MonitorSvrReport::visualize(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const {
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

const char * MonitorSvrReport::visualize_ex(tsf4g_tdr::TdrWriteBuf &destBuf, int indent, char separator) const {
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

const char * MonitorSvrReport::visualize_ex(char *buffer, size_t size, size_t *usedSize, int indent, char separator) const {
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
