//
// Created by Loyie King on 2024/3/6.
//

#include "ConfigFileReader.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"

#include <cstdio>
#include <cstring>

CConfigFileReader::CConfigFileReader() {}

void CConfigFileReader::add_tag(const char *tag) {
    this->tags.push_back(std::string(tag));
}

bool CConfigFileReader::CheckCommand(char *cmd) {
    if (cmd[0] == '\0') {
        return 1;
    } else if (cmd[0] == '[' || cmd[0] == '\r' || cmd[0] == '\n') {
        return 1;
    } else {
        // 原始：i <= 0x3fd（有符号 setle 物化循环条件）
        for (int i = 0; i <= 0x3fd; ++i) {
            int comment_match = 0;
            // 原始：j <= 1（有符号；sizeof 无符号会导致 setbe）
            for (int j = 0; j <= 1; ++j) {
                // 原始：直接引用 rodata 字面量 "//"（movzbl "//"(,%eax),%eax），
                // 不能用 static 数组（会产生 _ZZN...COMMENT_TOKEN 数据符号引用）。
                if (cmd[i + j] == "//"[j]) {
                    comment_match += 1;
                }
            }
            if (comment_match == 2) {
                return 1;
            }
        }
        return 0;
    }

    return 0;
}

const char *CConfigFileReader::get_value(const char *_key) {
    // 原始：临时 std::string（find 后立即销毁，先于 end()）
    std::map<std::string, std::string>::const_iterator result = this->configs.find(std::string(_key));
    if (result != this->configs.end()) {
        // 原始：operator-> 结果先存入命名局部（-0xc）再取 c_str（复现 store+reload 形态）
        const std::string *p = &result->second;
        return p->c_str();
    }
    return NULL;
}

bool CConfigFileReader::GetLine(FILE *file, char *output) {
    memset(output, 0, 0x400);
    // 原始：for (pos = 0; pos <= 0x3ff; pos++)（setle 物化循环条件）
    int pos;
    for (pos = 0; pos <= 0x3ff; pos++) {
        int c = fgetc(file);
        // 原始：feof 调用结果直接入条件（test eax,eax; setne al; test al,al; je）
        if (feof(file)) {
            output[pos] = '\0';
            return 0;
        }
        output[pos] = (char)c;
        if (output[pos] == '\n') {
            output[pos] = '\0';
            if (pos > 0 && output[pos - 1] == '\r') {
                output[pos - 1] = '\0';
                return 1;
            }
            return 1;
        }
    }
    return 1;
}

bool CConfigFileReader::Parse(char *data) {
    char *key, *value;

    // Using strtok to get the key and value
    // 原始：delims 存局部变量（mov [ebp-0x10],0x80b27e6）
    const char* delims = "=";
    key = strtok(data, delims);
    value = strtok(NULL, delims);

    Trim(key);
    Trim(value);

    // 原始：const_iterator + do-while + goto check 结构（匹配时也走 check 点）
    std::vector<std::string>::const_iterator config = tags.begin();
    std::vector<std::string>::const_iterator end = tags.end();
    // 原始：while (config != end)（循环尾 ne + jne 回跳；无 ! 取反的 xor 物化）
    while (config != end) {
        // 原始：解引用一次存指针（call de → [ebp-0xc]），c_str/make_pair 复用
        const std::string* tag = &*config;
        if (strcmp(key, tag->c_str()) == 0) {
            configs.insert(std::make_pair(*tag, value));
            break;
        }
        ++config;
    }

    if (config == end) {
        printf("cfg file warning! cannot find tag (%s)!\n", key);
    }
    return 1;
}

int CConfigFileReader::ReadConfigFile(char const *configFilePath) {
    char buffer[1024];

    FILE *file = fopen(configFilePath, "rb");
    if (file == NULL) {
        return 0;
    }
    // 原始：GetLine == false 时 break（fclose 块置于循环后）；CheckCommand/Parse == false 嵌套
    while (1) {
        bool state = GetLine(file, buffer);
        if (state == false) {
            break;
        }
        if (CheckCommand(buffer) == false) {
            if (Parse(buffer) == false) {
                return 0;
            }
        }
    }
    fclose(file);
    return 1;
}

char *CConfigFileReader::Trim(char *str) {
    size_t len = strlen(str);
    // 原始（ORIG 二进制实测）：i 先声明（槽 -0x14）、end 后声明（槽 -0x10），
    // 但赋值语句先 end = 0 后 i = 0（初始化顺序 -0x10 先于 -0x14）。
    // i/end 均为 size_t：unsigned 才生成 lea (%edx,%eax,1) 索引形态与 setb/seta 物化。
    size_t i, end;
    end = 0;
    i = 0;
    while (i < len) {
        end = i;
        if (str[i] > ' ') {
            break;
        }
        if (str[i] < 0) {
            break;
        }
        i++;
    }
    size_t end2 = len;  // 原始：无符号比较（seta）
    i = len - 1;
    while (end2 > end) {
        if (str[i] > ' ') {
            break;
        }
        if (str[i] < 0) {
            break;
        }
        end2--;
        i--;
    }
    char tmp_buffer[1024];
    tmp_buffer[end2 - end] = '\0';
    memcpy(tmp_buffer, str + end, end2 - end);
    strcpy(str, tmp_buffer);
    return str;
}
