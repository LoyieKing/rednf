//
// Created by Loyie King on 2024/3/6.
//
#pragma once
#include "ReverseEngineerLib.h"
#include "map"
#include "string"
#include "vector"

class CConfigFileReader {
private:
    std::map<std::string, std::string> configs;
    std::vector<std::string> tags;

public:
    CConfigFileReader();  // 原始：显示定义于 ConfigFileReader.cpp（TU 首函数，生成 C1/C2）

    void add_tag(const char *tag);

    bool CheckCommand(char *cmd);

    const char *get_value(const char *key);

    bool GetLine(FILE *file, char *output);

    bool Parse(char *data);

    int ReadConfigFile(char const *configFilePath);

    char *Trim(char *str);
};

TEST_CLASS_SIZE(CConfigFileReader, 36)
