// df_coserver_r — CVersionMgr/CSourceVersionMgr（ORIG version.cpp）
#include <utility>

#include "DNFVersion.h"

CVersionMgr::CVersionMgr(int _Major, int _Minor, int _Release, int _Build)
{
    Major = _Major;
    Minor = _Minor;
    Release = _Release;
    Build = _Build;
}

CSourceVersionMgr::CSourceVersionMgr()
    : source_version_list_()
{
    InsertSourceVersion(".svn/all-wcprops", 0x2c00);
    InsertSourceVersion("DNFServerCommon/.svn/all-wcprops", 0x2c01);
}

void CSourceVersionMgr::InsertSourceVersion(char* source_file_name, int version)
{
    source_version_list_.push_back(SourceVersion(source_file_name, version));
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

CSourceVersionMgr::SourceVersion& CSourceVersionMgr::SourceVersion::operator=(
    const SourceVersion& other)
{
    source_file_name_ = other.source_file_name_;
    version_ = other.version_;
    return *this;
}
