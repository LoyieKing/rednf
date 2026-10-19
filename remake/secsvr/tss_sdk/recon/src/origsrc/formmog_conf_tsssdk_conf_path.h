// Private protocol header reconstructed for
// src/protocol/formmog/formmog_conf_tsssdk_conf_path.cpp of
// libtss_sdk.so.1.0.394 (tag 394).
//
// Layout source: the DWARF of the ORIG translation unit
// (cu_layout.py on CU
//   /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/
//   src/protocol/formmog/formmog_conf_tsssdk_conf_path.cpp
// ):
//   path_config  sizeof 1024 : char config_path[1024] at +0
//
// The 12 member functions are declared in the ORIG's declaration order
// (DW_AT_decl_line of each DW_TAG_subprogram of the class DIE):
// construct, fromXmlFile, fromXmlBuffer, fromXmlString, fromXml,
// entryFromXml, toXmlFile(FILE*), toXmlFile(const char*), toXmlBuffer, toXml,
// entryToListXml, entryToAttrXml.
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_PATH_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_PATH_H_H_

#include <stdint.h>

#include "tsf4g_tdr_compat_cfg_conf.h"

namespace tss_sdk_conf_path_desc {

class path_config {
public:
    char config_path[1024];

    tsf4g_tdr::TdrError::ErrorType construct();
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *file,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *buffer,
                                                 unsigned int size,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *string,
                                                 tsf4g_tdr::TdrXmlFormat format,
                                                 unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType fromXml(tsf4g_tdr::TdrXmlReader &reader,
                                           unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType entryFromXml(tsf4g_tdr::TdrXmlReader &reader,
                                                unsigned int cutVer);
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *fp,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *file,
                                             tsf4g_tdr::TdrXmlFormat format,
                                             unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *buffer, unsigned int size,
                                               unsigned int *usedSize,
                                               tsf4g_tdr::TdrXmlFormat format,
                                               unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType toXml(tsf4g_tdr::TdrXmlWriter &writer,
                                         tsf4g_tdr::TdrXmlFormat format,
                                         unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToListXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(
        tsf4g_tdr::TdrXmlWriter &writer, unsigned int cutVer) const;
};

// Layout assertion: DW_AT_byte_size of the CU's path_config DIE.
typedef char path_config_size_must_be_1024[sizeof(path_config) == 1024 ? 1 : -1];
typedef char path_config_config_path_offset_must_be_0[
    offsetof(path_config, config_path) == 0 ? 1 : -1];

} // namespace tss_sdk_conf_path_desc

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_PATH_H_H_
