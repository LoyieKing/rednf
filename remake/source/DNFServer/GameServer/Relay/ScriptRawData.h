#ifndef SCRIPTRAWDATA_H_
#define SCRIPTRAWDATA_H_

#include <list>

// ScriptRawData：m_key[0x100]@0 / m_val@0x100 / m_err_msg@0x104 / m_valsize@0x108 /
//              m_child_list@0x10c（std::list 8B），总 0x114
class ScriptRawData
{
public:
    ScriptRawData(char* key, char* val, int val_size);
    ~ScriptRawData();
    char* get();
    char* get_data(char* key);
    char* get_data(char* parent_key, char* child_key);
    int get_val_size();
    bool push_child(ScriptRawData* sd);
    bool push_child(char* key, ScriptRawData* sd);
    ScriptRawData* find(char* key);
    ScriptRawData* find(char* parent_key, char* child_key);

private:
    char m_key[0x100];
    char* m_val;
    char* m_err_msg;
    int m_valsize;
    std::list<ScriptRawData*> m_child_list;
};

#endif // SCRIPTRAWDATA_H_
