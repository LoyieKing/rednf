#ifndef SCRIPTRAWDATA_H_
#define SCRIPTRAWDATA_H_

#include "stdafx.h"
#include <list>

class ScriptRawData
{
public:
    ScriptRawData(char* key, char* val, int val_size);
    ~ScriptRawData();
    char* get();
    int get_val_size();
    char* get_data(char* key);
    char* get_data(char* parent_key, char* child_key);
    bool push_child(ScriptRawData* sd);
    bool push_child(char* key, ScriptRawData* sd);
    ScriptRawData* find(char* key);
    ScriptRawData* find(char* parent_key, char* child_key);

private:
    char m_key[256];
    char* m_val;
    char* err_msg;
    int m_valsize;
    std::list<ScriptRawData*> m_child_list;
};

#endif // SCRIPTRAWDATA_H_
