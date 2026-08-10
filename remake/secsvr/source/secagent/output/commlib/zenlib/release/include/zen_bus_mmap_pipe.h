// Restored from gunnersvr DWARF (original: src/commlib/zenlib/zen_bus_mmap_pipe.h)
// 布局与 gunnersvr 二进制一致：sizeof = 10292。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_MMAP_PIPE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_MMAP_PIPE_H_H_

#include <stddef.h>
#include <stdint.h>
#include <string>
using std::string;

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"

namespace ZEN_LIB {
struct dequechunk_node;
struct shm_dequechunk;
}

// sizeof = 10292
struct ZEN_Bus_MMAPPipe : public ZEN_NON_Copyable { // line 18
    // sizeof = 4104
    struct ZEN_BUS_PIPE_HEAD { // line 35
    public:
        uint32_t size_of_sizet_;
        size_t number_of_pipe_;
        size_t size_of_pipe_[512];
        size_t size_of_room_[512];
        ZEN_BUS_PIPE_HEAD(); // line 36
        ~ZEN_BUS_PIPE_HEAD(); // line 37
    };
public:
    static const size_t MAX_NUMBER_OF_PIPE = 512; // line 23
protected:
    char bus_mmap_name_[4097]; // line 54
    ZEN_BUS_PIPE_HEAD bus_head_; // line 57
    ZEN_LIB::shm_dequechunk *bus_pipe_pointer_[512]; // line 60
    ZEN_ShareMem_Mmap mmap_file_; // line 63
    ZEN_ShareMem_Posix posix_file_; // line 64
    void *mmap_addr_; // line 66
    static ZEN_Bus_MMAPPipe *instance_; // line 71
    uint8_t sharemem_type_; // line 73
public:
    ZEN_Bus_MMAPPipe(); // line 78
    ~ZEN_Bus_MMAPPipe(); // line 80
    int initialize(const char *bus_mmap_name, size_t number_of_pipe, size_t *size_of_pipe, size_t max_frame_len, bool if_restore, bool if_check_pthread, uint8_t mem_type); // line 91
    int initialize(const char *bus_mmap_name, size_t max_frame_len, uint8_t mem_type); // line 96
    bool is_exist_bus(size_t index); // line 101
    int push_back_bus(size_t index, const ZEN_LIB::dequechunk_node *node); // line 103
    int pop_front_bus(size_t index, ZEN_LIB::dequechunk_node *node); // line 105
    int read_front_bus(size_t index, ZEN_LIB::dequechunk_node *&node); // line 107
    int pop_front_bus(size_t index); // line 109
    int get_front_nodesize(size_t index, size_t &nodesize); // line 111
    bool is_full_bus(size_t index); // line 115
    bool is_empty_bus(size_t index); // line 117
    void get_bus_freesize(size_t index, size_t &freesize, size_t &size); // line 119
protected:
    int init_all_pipe(size_t max_frame_len, bool if_restore, bool if_check_pthread); // line 127
public:
    static void instance(ZEN_Bus_MMAPPipe *pinstatnce); // line 133
    static ZEN_Bus_MMAPPipe * instance(); // line 135
    static void clean_instance(); // line 137
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_BUS_MMAP_PIPE_H_H_
