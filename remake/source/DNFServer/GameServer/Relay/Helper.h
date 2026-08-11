#ifndef HELPER_H_
#define HELPER_H_

#include <pthread.h>

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&m, 0);
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&m);
    }
    void lock()
    {
        pthread_mutex_lock(&m);
    }
    void unlock()
    {
        pthread_mutex_unlock(&m);
    }
    pthread_mutex_t m;
};

template <typename T>
struct GlobalInstance
{
    static T* inst_ptr();
    static void create();
    static T* m_p;
    static Mutex sync;
};

struct ScriptData;
class Script;
ScriptData* G_ScriptData();
Script* G_Script();

long long get_ms_tick();
char* NumberToString(unsigned int value, int index);
char* NumberToString(unsigned long long value, int index);
void make_dir(char* path);
void WriteLog(const char* msg);
void MonitorAuthLog(unsigned int acc_id);

#endif // HELPER_H_
