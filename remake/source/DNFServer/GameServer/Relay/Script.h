#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <stdio.h>
#include "Token.h"

class ScriptRawData;
class Script;
Script* G_Script();

// Script：data@0 / parent_tag[0x100]@4 / fp@0x104 / Token@0x108 / depth@0x540
class Script
{
public:
    Script();
    ~Script();
    int load(char* path);
    int parse(char* line);
    int parse_channel_script();
    int get_server_section();
    int get_aradauth_section();
    int get_key_val(char* line);
    bool on_parent_tag(char* tag);
    bool on_keyval_tag(char* key, char* value);
    int remove_comment(char* line);
    void destroy_raw_script();
    int fgetln(FILE* fp, char* buf);

private:
    ScriptRawData* data;
    char parent_tag[0x100];
    FILE* fp;
    Token tok;
    int depth;
};

#endif // SCRIPT_H_
