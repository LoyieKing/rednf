#ifndef C_SOURCE_VERSION_MGR_H_
#define C_SOURCE_VERSION_MGR_H_

#include <cstddef>
#include <string>
#include <vector>

class CSourceVersionMgr
{
public:
    class SourceVersion
    {
    public:
        SourceVersion(char* source_file_name, int version);
        SourceVersion(const SourceVersion& other);
        ~SourceVersion();
        SourceVersion& operator=(const SourceVersion& other);

        std::string source_file_name_;
        int version_;
    };

    CSourceVersionMgr();
    void InsertSourceVersion(char* source_file_name, int version);

    std::vector<SourceVersion> source_version_list_;
};

// df_game_r uses the legacy 32-bit libstdc++ string ABI: vector elements are 8 bytes.
static_assert(offsetof(CSourceVersionMgr::SourceVersion, source_file_name_) == 0,
              "SourceVersion::source_file_name_ offset changed");
static_assert(offsetof(CSourceVersionMgr::SourceVersion, version_) == 4,
              "SourceVersion::version_ offset changed");
static_assert(sizeof(CSourceVersionMgr::SourceVersion) == 8,
              "SourceVersion layout must match df_game_r");

#endif // C_SOURCE_VERSION_MGR_H_
