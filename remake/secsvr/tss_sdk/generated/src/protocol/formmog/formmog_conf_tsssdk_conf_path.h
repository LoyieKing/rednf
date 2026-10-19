// Auto-generated header stub from DWARF info
// Original path: src/protocol/formmog/formmog_conf_tsssdk_conf_path.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_PATH_H_H_
#define SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_PATH_H_H_

typedef struct {
    public:
    char config_path[];
    tsf4g_tdr::TdrError::ErrorType construct(); // line 40
    tsf4g_tdr::TdrError::ErrorType fromXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 42
    tsf4g_tdr::TdrError::ErrorType fromXmlBuffer(const char *arg0, size_t arg1, tsf4g_tdr::TdrXmlFormat arg2, unsigned int arg3); // line 43
    tsf4g_tdr::TdrError::ErrorType fromXmlString(char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2); // line 45
    tsf4g_tdr::TdrError::ErrorType fromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 46
    tsf4g_tdr::TdrError::ErrorType entryFromXml(/*anon struct*/ int &arg0, unsigned int arg1); // line 47
    tsf4g_tdr::TdrError::ErrorType toXmlFile(const char *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 49
    tsf4g_tdr::TdrError::ErrorType toXmlFile(FILE *arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 50
    tsf4g_tdr::TdrError::ErrorType toXmlBuffer(char *arg0, size_t arg1, size_t *arg2, tsf4g_tdr::TdrXmlFormat arg3, unsigned int arg4) const; // line 52
    tsf4g_tdr::TdrError::ErrorType toXml(/*anon struct*/ int &arg0, tsf4g_tdr::TdrXmlFormat arg1, unsigned int arg2) const; // line 53
    tsf4g_tdr::TdrError::ErrorType entryToListXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 54
    tsf4g_tdr::TdrError::ErrorType entryToAttrXml(/*anon struct*/ int &arg0, unsigned int arg1) const; // line 55
} PATH_CONFIG;

#endif // SECSVR_SRC_PROTOCOL_FORMMOG_FORMMOG_CONF_TSSSDK_CONF_PATH_H_H_
