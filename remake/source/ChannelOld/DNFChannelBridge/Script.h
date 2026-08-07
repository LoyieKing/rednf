#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "stdafx.h"
#include "Token.h"
#include "ScriptRawData.h"

class Script
{
public:
    Script();
    ~Script();
    bool load(char* filename);
    bool parse(char* line);
    bool fgetln(FILE* fp, char* buf);
    bool remove_comment(char* line);
    bool get_key_val(char* line);
    bool on_parent_tag(char* key);
    bool on_keyval_tag(char* key, char* val);
    bool get_server_section();
    bool get_db_section();
    bool parse_channel_script();
    void destroy_raw_script();

    ScriptRawData* data;
    char parent_tag[256];
    FILE* fp;
    Token tok;
    int depth;
};

#endif // SCRIPT_H_
