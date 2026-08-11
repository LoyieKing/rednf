// df_relay_r — ScriptRawData（独立 TU，与 Script 分开以匹配原版 EH 清理形态）
#include <stdlib.h>
#include <string.h>

#include "ScriptRawData.h"

ScriptRawData::ScriptRawData(char* key, char* val, int val_size)
    : m_val(0), m_err_msg(0), m_valsize(val_size)
{
    strncpy(m_key, key, 0xff);
    if (val != 0)
    {
        m_val = new char[val_size + 1];
        strncpy(m_val, val, val_size);
        m_val[val_size] = 0;
    }
}

ScriptRawData::~ScriptRawData()
{
    if (m_val != 0)
    {
        delete m_val;
    }
}

char* ScriptRawData::get()
{
    return m_val;
}

ScriptRawData* ScriptRawData::find(char* key)
{
    if (strcmp(m_key, key) == 0)
    {
        return this;
    }
    std::list<ScriptRawData*>::iterator iter;
    iter = m_child_list.begin();
    while (iter != m_child_list.end())
    {
        ScriptRawData* s = (*iter)->find(key);
        if (s != 0)
        {
            return s;
        }
        iter++;
    }
    return 0;
}

ScriptRawData* ScriptRawData::find(char* parent_key, char* child_key)
{
    ScriptRawData* p = find(parent_key);
    if (p == 0)
    {
        return 0;
    }
    p = p->find(child_key);
    return p;
}

char* ScriptRawData::get_data(char* key)
{
    ScriptRawData* s = find(key);
    if (s == 0)
    {
        return 0;
    }
    return s->m_val;
}

char* ScriptRawData::get_data(char* parent_key, char* child_key)
{
    ScriptRawData* s = find(parent_key, child_key);
    if (s == 0)
    {
        return 0;
    }
    return s->m_val;
}

int ScriptRawData::get_val_size()
{
    return m_valsize;
}

bool ScriptRawData::push_child(ScriptRawData* sd)
{
    m_child_list.push_back(sd);
    return true;
}

bool ScriptRawData::push_child(char* key, ScriptRawData* sd)
{
    ScriptRawData* parent = find(key);
    if (parent == 0)
    {
        push_child(sd);
    }
    else
    {
        parent->push_child(sd);
    }
    return true;
}
