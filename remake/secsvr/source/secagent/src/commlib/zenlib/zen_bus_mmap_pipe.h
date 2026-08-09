// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_bus_mmap_pipe.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_MMAP_PIPE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_MMAP_PIPE_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"
#include <stddef.h>
#include <stdint.h>

// sizeof = 10292
struct ZEN_Bus_MMAPPipe : public ZEN_NON_Copyable { // line 18
    // sizeof = 4104
    struct ZEN_BUS_PIPE_HEAD {
    public:
    uint32_t size_of_sizet_;
    size_t number_of_pipe_;
    size_t size_of_pipe_[];
    size_t size_of_room_[];
    ZEN_BUS_PIPE_HEAD(); // line 36
    ~ZEN_BUS_PIPE_HEAD(); // line 37
    };
public:
static const size_t MAX_NUMBER_OF_PIPE;
protected:
char bus_mmap_name_[];
ZEN_Bus_MMAPPipe::ZEN_BUS_PIPE_HEAD bus_head_;
shm_dequechunk *bus_pipe_pointer_[];
ZEN_ShareMem_Mmap mmap_file_;
ZEN_ShareMem_Posix posix_file_;
void *mmap_addr_;
static ZEN_Bus_MMAPPipe *instance_;
uint8_t sharemem_type_;
public:
ZEN_Bus_MMAPPipe(); // line 78
~ZEN_Bus_MMAPPipe(); // line 80
int initialize(const char *arg0, size_t arg1, size_t *arg2, size_t arg3, bool arg4, bool arg5, uint8_t arg6); // line 91
int initialize(const char *arg0, size_t arg1, uint8_t arg2); // line 96
bool is_exist_bus(size_t arg0); // line 101
int push_back_bus(size_t arg0, const /*anon struct*/ int *arg1); // line 103
int pop_front_bus(size_t arg0, /*anon struct*/ int *arg1); // line 105
int read_front_bus(size_t arg0, /*anon struct*/ int *&arg1); // line 107
int pop_front_bus(size_t arg0); // line 109
int get_front_nodesize(size_t arg0, size_t &arg1); // line 111
bool is_full_bus(size_t arg0); // line 115
bool is_empty_bus(size_t arg0); // line 117
void get_bus_freesize(size_t arg0, size_t &arg1, size_t &arg2); // line 119
protected:
int init_all_pipe(size_t arg0, bool arg1, bool arg2); // line 127
public:
static void instance(ZEN_Bus_MMAPPipe *arg0); // line 133
static ZEN_Bus_MMAPPipe * instance(); // line 135
static void clean_instance(); // line 137
};
extern const size_t MAX_NUMBER_OF_PIPE; // line 23
extern bool bret; // line 173
namespace ZEN_Bus_MMAPPipe {
    // sizeof = 4104
    struct ZEN_BUS_PIPE_HEAD { // line 35
    public:
    uint32_t size_of_sizet_;
    size_t number_of_pipe_;
    size_t size_of_pipe_[];
    size_t size_of_room_[];
    ZEN_BUS_PIPE_HEAD(); // line 36
    ~ZEN_BUS_PIPE_HEAD(); // line 37
    };
} // namespace ZEN_Bus_MMAPPipe

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_MMAP_PIPE_H_H_
