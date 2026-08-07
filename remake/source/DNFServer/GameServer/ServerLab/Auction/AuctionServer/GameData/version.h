#ifndef AUCTION_VERSION_H_
#define AUCTION_VERSION_H_

#include <string>
#include <vector>

class SourceVersion;

class CVersionMgr
{
public:
    CVersionMgr(int _Major, int _Minor, int _Release, int _Build);

    int Major;
    int Minor;
    int Release;
    int Build;
};

class CSourceVersionMgr
{
public:
    CSourceVersionMgr();
    ~CSourceVersionMgr();
    void InsertSourceVersion(char* source_file_name, int version);

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

    std::vector<SourceVersion> source_version_list_;
};

#endif // AUCTION_VERSION_H_
