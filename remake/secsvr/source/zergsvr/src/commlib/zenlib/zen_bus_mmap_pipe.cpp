// Restored from zergsvr binary (DWARF + objdump), 语义与 zergsvr 一致。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_bus_mmap_pipe.cpp
// 相对 gunnersvr 的差异：去掉了 mem_type/sharemem_type_ 与 ZEN_ShareMem_Posix，
// initialize 两个重载均少一个 unsigned char 参数，类不再有 mmap_addr_ 成员；
// 管道包装方法（is_exist_bus 等）在 zergsvr 头文件中仅声明、未定义（二进制中无符号）。

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "src/commlib/zenlib/zen_bus_mmap_pipe.h"

// ---------------------------------------------------------------------------
// 外部依赖：只声明、不定义；链接时由 zen_os_adapt_file / zen_os_adapt_shm /
// zen_shm_lockfree_deque 提供同名符号（mangled 名与二进制一致）。
// ---------------------------------------------------------------------------
namespace ZEN_OS {
int stat(const char *file_name, struct stat *buf);       // _ZN6ZEN_OS4statEPKcP4stat
int unlink(const char *file_name);                       // _ZN6ZEN_OS6unlinkEPKc
int shm_stat(char *shm_name, struct stat *buf);          // _ZN6ZEN_OS8shm_statEPcP4stat
int shm_unlink(const char *shm_name);                    // _ZN6ZEN_OS10shm_unlinkEPKc
}

namespace ZEN_LIB {
struct dequechunk_node {
    unsigned int size_of_node_;
    char chunkdata[];
};

struct shm_dequechunk {
    static size_t getallocsize(unsigned int size_of_pipe); // _ZN7ZEN_LIB14shm_dequechunk12getallocsizeEj
    static shm_dequechunk *initialize(unsigned int size_of_pipe,
                                      unsigned int max_frame_len,
                                      char *pt_pipe,
                                      bool if_restore,
                                      bool if_check_pthread); // _ZN7ZEN_LIB14shm_dequechunk10initializeEjjPcbb
    bool push_end(const dequechunk_node *node);               // _ZN7ZEN_LIB14shm_dequechunk8push_endEPKNS_15dequechunk_nodeE
    int pop_front(dequechunk_node *node);                     // _ZN7ZEN_LIB14shm_dequechunk9pop_frontEPNS_15dequechunk_nodeE
    int read_front_new(dequechunk_node *&node);               // _ZN7ZEN_LIB14shm_dequechunk14read_front_newERPNS_15dequechunk_nodeE
    int discard_frond();                                      // _ZN7ZEN_LIB14shm_dequechunk13discard_frondEv
    size_t freesize();                                        // _ZN7ZEN_LIB14shm_dequechunk8freesizeEv
    bool empty();                                             // _ZN7ZEN_LIB14shm_dequechunk5emptyEv
    bool full();                                              // _ZN7ZEN_LIB14shm_dequechunk4fullEv
    ~shm_dequechunk(); // _ZN7ZEN_LIB14shm_dequechunkD1Ev
};
}

// 真实符号为 debug_errorex(char const*, ...)（mangled 带 'z'）；
// DWARF 桩头把省略号丢了，这里用 asm label 直接绑定到原符号。
extern void zen_debug_errorex(const char *str_format, ...)
    __asm__("_ZN16ZEN_Trace_LogMsg13debug_errorexEPKcz");

#define ZEN_ERREX(...) zen_debug_errorex(__VA_ARGS__)

// ---------------------------------------------------------------------------
// 静态成员
// ---------------------------------------------------------------------------
ZEN_Bus_MMAPPipe *ZEN_Bus_MMAPPipe::instance_ = NULL;

// ---------------------------------------------------------------------------
// ZEN_BUS_PIPE_HEAD
// ---------------------------------------------------------------------------
ZEN_Bus_MMAPPipe::ZEN_BUS_PIPE_HEAD::ZEN_BUS_PIPE_HEAD() {
    size_of_sizet_ = sizeof(size_t);
    number_of_pipe_ = 0;
    memset(size_of_pipe_, 0, sizeof(size_of_pipe_));
    memset(size_of_room_, 0, sizeof(size_of_room_));
}

ZEN_Bus_MMAPPipe::ZEN_BUS_PIPE_HEAD::~ZEN_BUS_PIPE_HEAD() {
}

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------
ZEN_Bus_MMAPPipe::ZEN_Bus_MMAPPipe() {
    bus_mmap_name_[0] = '\0';
    bus_mmap_name_[sizeof(bus_mmap_name_) - 1] = '\0';
    memset(bus_pipe_pointer_, 0, sizeof(bus_pipe_pointer_));
}

ZEN_Bus_MMAPPipe::~ZEN_Bus_MMAPPipe() {
    size_t i = 0;

    mmap_file_.flush();

    for (i = 0; i < bus_head_.number_of_pipe_; ++i) {
        if (bus_pipe_pointer_[i] != NULL) {
            delete bus_pipe_pointer_[i];
        }
        bus_pipe_pointer_[i] = NULL;
        bus_head_.size_of_pipe_[i] = 0;
        bus_head_.size_of_room_[i] = 0;
    }
}

// ---------------------------------------------------------------------------
// 单例
// ---------------------------------------------------------------------------
ZEN_Bus_MMAPPipe * ZEN_Bus_MMAPPipe::instance() {
    if (instance_ == NULL) {
        instance_ = new ZEN_Bus_MMAPPipe();
    }
    return instance_;
}

void ZEN_Bus_MMAPPipe::instance(ZEN_Bus_MMAPPipe *pinstatnce) {
    clean_instance();
    instance_ = pinstatnce;
}

void ZEN_Bus_MMAPPipe::clean_instance() {
    if (instance_ != NULL) {
        delete instance_;
    }
    instance_ = NULL;
}

// ---------------------------------------------------------------------------
// initialize：指定管道数量与尺寸
// ---------------------------------------------------------------------------
int ZEN_Bus_MMAPPipe::initialize(const char *bus_mmap_name, size_t number_of_pipe,
                                 size_t *size_of_pipe, size_t max_frame_len,
                                 bool if_restore, bool if_check_pthread) {
    int ret = 0;
    size_t sz_malloc = 0;
    size_t i = 0;
    struct stat mmapfile_stat;
    size_t sz_room = 0;
    ZEN_BUS_PIPE_HEAD *pipe_head = NULL;

    if (number_of_pipe == 0) {
        return -1;
    }

    strncpy(bus_mmap_name_, bus_mmap_name, sizeof(bus_mmap_name_) - 1);
    bus_head_.number_of_pipe_ = number_of_pipe;
    for (i = 0; i < number_of_pipe; ++i) {
        bus_head_.size_of_pipe_[i] = size_of_pipe[i];
    }

    if (!if_restore) {
        ret = ZEN_OS::unlink(bus_mmap_name_);
    } else {
        ret = ZEN_OS::stat(bus_mmap_name_, &mmapfile_stat);
        if (ret != 0) {
            if_restore = false;
        }
    }

    sz_malloc = sizeof(ZEN_BUS_PIPE_HEAD);
    for (i = 0; i < bus_head_.number_of_pipe_; ++i) {
        sz_room = ZEN_LIB::shm_dequechunk::getallocsize(bus_head_.size_of_pipe_[i]);
        bus_head_.size_of_room_[i] = sz_room;
        sz_malloc += sz_room;
    }

    ret = mmap_file_.open(bus_mmap_name_, sz_malloc, if_restore, false, true, NULL, 0);

    if (ret != 0) {
        ZEN_ERREX("[zenlib] MMAP map a file (%s) to share memory fail,ret =%d, last error=%d|%s.",
                  bus_mmap_name_, ret, errno, strerror(errno));
        return -1;
    }

    if (if_restore) {
        pipe_head = (ZEN_BUS_PIPE_HEAD *)mmap_file_.addr();
        if (pipe_head->size_of_sizet_ == bus_head_.size_of_sizet_ &&
            pipe_head->number_of_pipe_ == bus_head_.number_of_pipe_) {
            for (i = 0; i < pipe_head->number_of_pipe_; ++i) {
                if (pipe_head->size_of_pipe_[i] == bus_head_.size_of_pipe_[i] &&
                    pipe_head->size_of_room_[i] == bus_head_.size_of_room_[i]) {
                    continue;
                }
                ZEN_ERREX("[zenlib] ZEN_Bus_MMAPPipe::initialize pipe fail. "
                          "ZEN_BUS_PIPE_HEAD <%u> old size_t_len[%u] numpipe[%u],"
                          "new size_t_len[%u],numpipe[%u] .",
                          i, pipe_head->size_of_pipe_[i], pipe_head->size_of_room_[i],
                          bus_head_.size_of_pipe_[i], bus_head_.size_of_room_[i]);
                return -1;
            }
        } else {
            ZEN_ERREX("[zenlib] ZEN_Bus_MMAPPipe::initialize pipe fail. "
                      "ZEN_BUS_PIPE_HEAD old size_t_len[%u] numpipe[%u],"
                      "new size_t_len[%u],numpipe[%u] ",
                      pipe_head->size_of_sizet_, pipe_head->number_of_pipe_,
                      bus_head_.size_of_sizet_, bus_head_.number_of_pipe_);
            return -1;
        }
    }

    memcpy(&bus_head_, mmap_file_.addr(), sizeof(ZEN_BUS_PIPE_HEAD));
    return init_all_pipe(max_frame_len, if_restore, if_check_pthread);
}

// ---------------------------------------------------------------------------
// initialize：挂接已有共享内存文件
// ---------------------------------------------------------------------------
int ZEN_Bus_MMAPPipe::initialize(const char *bus_mmap_name, size_t max_frame_len) {
    int ret = 0;
    struct stat mmapfile_stat;

    strncpy(bus_mmap_name_, bus_mmap_name, sizeof(bus_mmap_name_) - 1);

    ret = ZEN_OS::stat(bus_mmap_name_, &mmapfile_stat);
    if (ret != 0) {
        return -1;
    }
    if ((unsigned long)mmapfile_stat.st_size <= sizeof(ZEN_BUS_PIPE_HEAD)) {
        return -1;
    }

    ret = mmap_file_.open(bus_mmap_name_, mmapfile_stat.st_size, true, false, true, NULL, 0);

    if (ret != 0) {
        ZEN_ERREX("[zenlib] MMAP map a file (%s) to share memory fail,ret =%d, last error=%d|%s.",
                  bus_mmap_name_, ret, errno, strerror(errno));
        return -1;
    }

    memcpy(&bus_head_, mmap_file_.addr(), sizeof(ZEN_BUS_PIPE_HEAD));
    return init_all_pipe(max_frame_len, true, false);
}

// ---------------------------------------------------------------------------
// init_all_pipe：在 mmap 内存中初始化每条管道（shm_dequechunk）
// ---------------------------------------------------------------------------
int ZEN_Bus_MMAPPipe::init_all_pipe(size_t max_frame_len, bool if_restore, bool if_check_pthread) {
    size_t file_offset = sizeof(ZEN_BUS_PIPE_HEAD);
    size_t i = 0;
    char *pt_pipe = NULL;
    size_t sz_room = 0;

    for (i = 0; i < bus_head_.number_of_pipe_; ++i) {
        if (i > 0) {
            sz_room = ZEN_LIB::shm_dequechunk::getallocsize(bus_head_.size_of_pipe_[i - 1]);
            file_offset += sz_room;
        }
        pt_pipe = (char *)mmap_file_.addr() + file_offset;
        bus_pipe_pointer_[i] = ZEN_LIB::shm_dequechunk::initialize(
            bus_head_.size_of_pipe_[i], max_frame_len, pt_pipe, if_restore, if_check_pthread);
        if (bus_pipe_pointer_[i] == NULL) {
            ZEN_ERREX("[zenlib] ZEN_Bus_MMAPPipe::initialize pipe[%u] size[%u] room[%u] fail.",
                      i, bus_head_.size_of_pipe_[i], bus_head_.size_of_room_[i]);
            return -1;
        }
    }
    return 0;
}
