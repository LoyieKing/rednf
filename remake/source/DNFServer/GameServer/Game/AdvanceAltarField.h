#ifndef DNF_GAME_ADVANCE_ALTAR_FIELD_H_
#define DNF_GAME_ADVANCE_ALTAR_FIELD_H_

#include <map>
#include <string>
#include <vector>

namespace advancealtar
{
struct FieldType
{
    enum T
    {
        T_0 = 0
    };
};

struct FieldData
{
    int m_field0;
    int m_value;
    FieldData() : m_field0(0), m_value(0) {}
};

typedef std::map<FieldType::T, FieldData> FieldDataMap;
typedef std::map<int, FieldDataMap> LevelFieldDataMap;
}

#endif
