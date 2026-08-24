#ifndef ARAD_SCRIPT_H
#define ARAD_SCRIPT_H

#include <cstring>
#include <map>
#include <string>

class Arad_Script
{
public:
    Arad_Script();
    explicit Arad_Script(const char* path);
    ~Arad_Script();

    bool Create();
    bool ExpandCol();
    bool ExpandRow();
    bool Load(const char* path);
    bool Load(const char* dir, const char* file);
    const char* make_file_path(const char* file);

    int GetIdx4Col(char* name);
    int GetIdx4Row(char* name);
    int GetInt(int row, int col);
    int GetInt(char* row, char* col);
    int GetInt(char* row, int col);
    int GetInt(int row, char* col);
    float GetFloat(int row, int col);
    float GetFloat(char* row, char* col);
    float GetFloat(char* row, int col);
    float GetFloat(int row, char* col);
    double GetDouble(int row, int col);
    double GetDouble(char* row, char* col);
    double GetDouble(char* row, int col);
    double GetDouble(int row, char* col);
    char* GetString(int row, int col);
    char* GetString(char* row, char* col);
    char* GetString(char* row, int col);
    char* GetString(int row, char* col);
    void SetInt(int row, int col, int value);
    void SetInt(char* row, char* col, int value);
    void SetInt(char* row, int col, int value);
    void SetInt(int row, char* col, int value);
    void SetString(int row, int col, char* value);
    void SetString(char* row, char* col, char* value);
    void SetString(char* row, int col, char* value);
    void SetString(int row, char* col, char* value);
    int GetRowNum();

    int cols_;   // +0x00, Create sets 0x20
    int rows_;   // +0x04, Create sets 0x100
    int usedCols_;
    int usedRows_;
    int bufBytes_;
    char* cells_; // +0x14, cell size 0x80
    std::string pathA_;
    std::string pathB_;
    std::string pathC_;
    std::map<std::string, int> colIndex_; // +0x24
    std::map<std::string, int> rowIndex_; // +0x3c
};

#endif
