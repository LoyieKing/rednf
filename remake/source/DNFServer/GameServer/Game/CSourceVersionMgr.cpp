// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CSourceVersionMgrC2Ev`（ORIG 0x86b2388）；ctor 已是首强符号。
#include <iostream>

#include "CSourceVersionMgr.h"

CSourceVersionMgr::CSourceVersionMgr()
    : source_version_list_()
{
    InsertSourceVersion(".", 0x16bdc);
    InsertSourceVersion("../ServerCommon", 0x16bdc);
    InsertSourceVersion("../../DNFShared", 0x16bdc);
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
    , version_(other.version_)
{
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
