// df_statics_r — Version（ORIG Version.o 拆分）
#include <utility>

#include "Version.h"

CSourceVersionMgr::CSourceVersionMgr()
    : source_version_list_()
{
    InsertSourceVersion(".", 0x19daa);
    InsertSourceVersion("../ServerCommon", 0x19daa);
}
CVersionMgr::CVersionMgr(int _Major, int _Minor, int _Release, int _Build)
{
    Major = _Major;
    Minor = _Minor;
    Release = _Release;
    Build = _Build;
}
CSourceVersionMgr::SourceVersion::SourceVersion(char* source_file_name, int version)
    : source_file_name_()
{
    source_file_name_ = source_file_name;
    version_ = version;
}
CSourceVersionMgr::SourceVersion::SourceVersion(const SourceVersion& other)
    : source_file_name_(other.source_file_name_)
{
    version_ = other.version_;
}
CSourceVersionMgr::SourceVersion::~SourceVersion()
{
}
void CSourceVersionMgr::InsertSourceVersion(char* source_file_name, int version)
{
    source_version_list_.push_back(SourceVersion(source_file_name, version));
}
CSourceVersionMgr::SourceVersion& CSourceVersionMgr::SourceVersion::operator=(
    const SourceVersion& other)
{
    source_file_name_ = other.source_file_name_;
    version_ = other.version_;
    return *this;
}
