// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_BUS_MMAP_PIPE_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_BUS_MMAP_PIPE_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include <stddef.h>
#include <stdint.h>

// sizeof = 10268
struct ZEN_Bus_MMAPPipe : public ZEN_NON_Copyable { // line 17
    // sizeof = 4104
    struct ZEN_BUS_PIPE_HEAD {
    public:
    uint32_t size_of_sizet_;
    size_t number_of_pipe_;
    size_t size_of_pipe_[];
    size_t size_of_room_[];
    ZEN_BUS_PIPE_HEAD(); // line 29
    ~ZEN_BUS_PIPE_HEAD(); // line 30
    };
public:
static const size_t MAX_NUMBER_OF_PIPE;
protected:
char bus_mmap_name_[];
ZEN_Bus_MMAPPipe::ZEN_BUS_PIPE_HEAD bus_head_;
/*anon struct*/ int *bus_pipe_pointer_[];
ZEN_ShareMem_Mmap mmap_file_;
static ZEN_Bus_MMAPPipe *instance_;
public:
ZEN_Bus_MMAPPipe(); // line 66
~ZEN_Bus_MMAPPipe(); // line 68
int initialize(const char *arg0, size_t arg1, size_t *arg2, size_t arg3, bool arg4, bool arg5); // line 78
int initialize(const char *arg0, size_t arg1); // line 82
bool is_exist_bus(size_t arg0); // line 87
int push_back_bus(size_t arg0, const /*anon struct*/ int *arg1); // line 89
int pop_front_bus(size_t arg0, /*anon struct*/ int *arg1); // line 91
int read_front_bus(size_t arg0, /*anon struct*/ int *&arg1); // line 93
int pop_front_bus(size_t arg0); // line 95
int get_front_nodesize(size_t arg0, size_t &arg1); // line 97
bool is_full_bus(size_t arg0); // line 101
bool is_empty_bus(size_t arg0); // line 103
void get_bus_freesize(size_t arg0, size_t &arg1, size_t &arg2); // line 105
protected:
int init_all_pipe(size_t arg0, bool arg1, bool arg2); // line 113
public:
static void instance(ZEN_Bus_MMAPPipe *arg0); // line 119
static ZEN_Bus_MMAPPipe * instance(); // line 121
static void clean_instance(); // line 123
};
extern const size_t MAX_NUMBER_OF_PIPE; // line 22
extern bool bret; // line 159
namespace ZEN_Bus_MMAPPipe {
    // sizeof = 4104
    struct ZEN_BUS_PIPE_HEAD { // line 28
    public:
    uint32_t size_of_sizet_;
    size_t number_of_pipe_;
    size_t size_of_pipe_[];
    size_t size_of_room_[];
    ZEN_BUS_PIPE_HEAD(); // line 29
    ~ZEN_BUS_PIPE_HEAD(); // line 30
    };
} // namespace ZEN_Bus_MMAPPipe

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_BUS_MMAP_PIPE_H_H_
