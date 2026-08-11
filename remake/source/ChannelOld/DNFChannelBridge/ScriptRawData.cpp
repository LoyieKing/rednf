#include "ScriptRawData.h"
#include <string.h>
#include <assert.h>

ScriptRawData::ScriptRawData(char* key, char* val, int val_size)
    : m_val(NULL), m_valsize(0)
{
    strncpy(m_key, key, 0xff);
    if (val != NULL)
    {
        m_val = new char[val_size + 1];
#line 16 "ScriptRawData.cpp"
        assert(m_val);
        strncpy(m_val, val, val_size);
        m_val[val_size] = '\0';
    }
    m_valsize = val_size;
}

ScriptRawData::~ScriptRawData()
{
    if (m_val != NULL)
    {
        delete m_val;
    }
}

char* ScriptRawData::get()
{
    return m_val;
}

int ScriptRawData::get_val_size()
{
    return m_valsize;
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
        if (s != NULL)
        {
            return s;
        }
        iter++;
    }
    return NULL;
}

ScriptRawData* ScriptRawData::find(char* parent_key, char* child_key)
{
    ScriptRawData* p = find(parent_key);
    if (p == NULL)
    {
        return NULL;
    }
    p = p->find(child_key);
    return p;
}

char* ScriptRawData::get_data(char* key)
{
    ScriptRawData* s = find(key);
    if (s == NULL)
    {
        return NULL;
    }
    return s->m_val;
}

char* ScriptRawData::get_data(char* parent_key, char* child_key)
{
    ScriptRawData* s = find(parent_key, child_key);
    if (s == NULL)
    {
        return NULL;
    }
    return s->m_val;
}

bool ScriptRawData::push_child(ScriptRawData* sd)
{
#line 65 "ScriptRawData.cpp"
    assert(sd);
    m_child_list.push_back(sd);
    return true;
}

bool ScriptRawData::push_child(char* key, ScriptRawData* sd)
{
    if ((key == NULL) || (sd == NULL))
    {
#line 73 "ScriptRawData.cpp"
        __assert_fail("key && sd", "ScriptRawData.cpp", 0x49, __PRETTY_FUNCTION__);
    }
    else
    {
        ScriptRawData* parent = find(key);
        if (parent == NULL)
        {
            push_child(sd);
        }
        else
        {
            parent->push_child(sd);
        }
        return true;
    }
}
