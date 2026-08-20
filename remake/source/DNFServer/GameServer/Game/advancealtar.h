// advancealtar 基础类型声明。
#pragma once

namespace advancealtar
{

class TimeLine
{
public:
    TimeLine();
    ~TimeLine();
    TimeLine& operator=(const TimeLine& other);

private:
    char m_pad[0x1c];
};

}
