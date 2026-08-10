// ZEN_Server_Toolkit 还原实现（语义对照 gunnersvr oracle，2026-08-10）。
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_server_toolkit.cpp
// 说明：
//  - /proc 采集由 ZEN_OS::get_self_perf / get_system_perf（zen_os_adapt_*.cpp 其他 TU）完成，
//    本文件仅做差值、比例与内存水位计算，公式与 oracle 逐条指令一致。
//  - mem_use_ratio_ 口径（oracle 0x08096898..0x08096986）：
//    used = cachedram+freeram+bufferram；ratio = (totalram - used) * 1000 / totalram（无符号 64 位）。
//  - fmt3 中展示的 cpu 比值为 10.0f / ratio（原版如此，保留）。

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "src/commlib/zenlib/zen_os_adapt_process.h"
#include "src/commlib/zenlib/zen_os_adapt_sysinfo.h"
#include "src/commlib/zenlib/zen_server_toolkit.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"

namespace ZEN_OS {
pid_t getpid();
int socket_init(int version_high, int version_low);
ZEN_HANDLE open(const char *filename, int open_mode, mode_t perms);
int close(ZEN_HANDLE handle);
int flock(ZEN_HANDLE file_hadle, int operation);
int ftruncate(ZEN_HANDLE file_handle, size_t offset);
ssize_t write(ZEN_HANDLE file_handle, const void *buf, size_t count);
int get_self_perf(ZEN_PROCESS_PERFORM *prc_perf_info);
int get_system_performance(ZEN_SYSTEM_PERFORMANCE *zen_system_perf);
uint64_t total_milliseconds(const timeval &tv);
const timeval timeval_add(const timeval &left, const timeval &right);
const timeval timeval_sub(const timeval &left, const timeval &right, bool safe);
}

ZEN_Server_Toolkit::ZEN_Server_Toolkit() {
    pid_handle_ = -1;
    self_pid_ = 0;
    check_leak_times_ = 0;
    mem_checkpoint_size_ = 0;
    cur_mem_usesize_ = 0;
    process_cpu_ratio_ = 0;
    system_cpu_ratio_ = 0;
    mem_use_ratio_ = 0;
    memset(&last_process_perf_, 0, sizeof(ZEN_PROCESS_PERFORM));
    memset(&now_process_perf_, 0, sizeof(ZEN_PROCESS_PERFORM));
    memset(&last_system_perf_, 0, sizeof(ZEN_SYSTEM_PERFORMANCE));
    memset(&now_system_perf_, 0, sizeof(ZEN_SYSTEM_PERFORMANCE));
}

ZEN_Server_Toolkit::~ZEN_Server_Toolkit() {
    if (pid_handle_ != -1) {
        ZEN_OS::flock(pid_handle_, 8);
        ZEN_OS::close(pid_handle_);
    }
}

int ZEN_Server_Toolkit::socket_init() {
    return ZEN_OS::socket_init(2, 2);
}

int ZEN_Server_Toolkit::out_pid_file(const char *pragramname, bool lock_pid) {
    int ret;
    std::string filename = pragramname;
    const size_t BUFFER_LEN = 128;
    char tmpbuff[BUFFER_LEN];
    int len;

    filename.append(".pid", 4);
    pid_handle_ = ZEN_OS::open(filename.c_str(), 0x42, 0x1a4);
    if (pid_handle_ == -1) {
        return -1;
    }
    self_pid_ = ZEN_OS::getpid();
    len = snprintf(tmpbuff, BUFFER_LEN, "%u", self_pid_);
    ZEN_OS::ftruncate(pid_handle_, len);
    if (lock_pid) {
        ret = ZEN_OS::flock(pid_handle_, 6);
        if (ret != 0) {
            return ret;
        }
    }
    ZEN_OS::write(pid_handle_, tmpbuff, len);
    return 0;
}

uint32_t ZEN_Server_Toolkit::get_sys_cpu_ratio() const {
    return system_cpu_ratio_;
}

uint32_t ZEN_Server_Toolkit::get_app_cpu_ratio() const {
    return process_cpu_ratio_;
}

uint32_t ZEN_Server_Toolkit::get_sys_mem_ratio() const {
    return mem_use_ratio_;
}

uint64_t ZEN_Server_Toolkit::get_can_use_mem_size() const {
    return can_use_size_;
}

int ZEN_Server_Toolkit::watch_dog_status(bool first_record) {
    int ret;
    size_t vary_mem_size;
    timeval last_to_now;
    timeval proc_utime;
    timeval proc_stime;
    timeval proc_cpu_time;
    timeval sys_idletime;
    timeval sys_cputime;

    if (!first_record) {
        memcpy(&last_process_perf_, &now_process_perf_, sizeof(ZEN_PROCESS_PERFORM));
        memcpy(&last_system_perf_, &now_system_perf_, sizeof(ZEN_SYSTEM_PERFORMANCE));
    }
    ret = ZEN_OS::get_self_perf(&now_process_perf_);
    if (ret != 0) {
        return ret;
    }
    ret = ZEN_OS::get_system_performance(&now_system_perf_);
    if (ret != 0) {
        return ret;
    }

    cur_mem_usesize_ = now_process_perf_.vm_size_;
    if (first_record) {
        mem_checkpoint_size_ = cur_mem_usesize_;
        return ret;
    }

    if (cur_mem_usesize_ < mem_checkpoint_size_) {
        mem_checkpoint_size_ = cur_mem_usesize_;
        vary_mem_size = 0;
    } else {
        vary_mem_size = cur_mem_usesize_ - mem_checkpoint_size_;
        if (vary_mem_size > MEMORY_LEAK_THRESHOLD) {
            ++check_leak_times_;
            ZEN_Trace_LogMsg::debug_errorex(
                "[zenlib] [WATCHDOG][PID:%u] Monitor could memory leak,mem_checkpoint_size_ =[%u],run_mem_size_=[%u].",
                self_pid_, mem_checkpoint_size_, cur_mem_usesize_);
            if (check_leak_times_ > MAX_RECORD_MEMORY_NUMBER) {
                check_leak_times_ = 0;
                mem_checkpoint_size_ = cur_mem_usesize_;
            }
        }
    }

    last_to_now = ZEN_OS::timeval_sub(now_system_perf_.up_time_,
                                      last_system_perf_.up_time_, true);
    proc_utime = ZEN_OS::timeval_sub(now_process_perf_.run_utime_,
                                     last_process_perf_.run_utime_, true);
    proc_stime = ZEN_OS::timeval_sub(now_process_perf_.run_stime_,
                                     last_process_perf_.run_stime_, true);
    proc_cpu_time = ZEN_OS::timeval_add(proc_utime, proc_stime);

    if (ZEN_OS::total_milliseconds(last_to_now) != 0) {
        process_cpu_ratio_ = (uint32_t)(ZEN_OS::total_milliseconds(proc_cpu_time)
                                        * 1000 / ZEN_OS::total_milliseconds(last_to_now));
    } else {
        process_cpu_ratio_ = 0;
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zenlib] [WATCHDOG][PID:%u] cpu ratio[%u] totoal process user/sys[%lld/%lld] milliseconds leave last point all/usr/sys[%lld/%lld/%lld] milliseconds memory use//add [%ld/%ld].",
        self_pid_, process_cpu_ratio_,
        ZEN_OS::total_milliseconds(now_process_perf_.run_utime_),
        ZEN_OS::total_milliseconds(now_process_perf_.run_stime_),
        ZEN_OS::total_milliseconds(last_to_now),
        ZEN_OS::total_milliseconds(proc_utime),
        ZEN_OS::total_milliseconds(proc_stime),
        cur_mem_usesize_, vary_mem_size);

    sys_idletime = ZEN_OS::timeval_sub(now_system_perf_.idle_time_,
                                       last_system_perf_.idle_time_, true);
    sys_cputime = ZEN_OS::timeval_sub(last_to_now, sys_idletime, true);
    if (ZEN_OS::total_milliseconds(last_to_now) != 0) {
        system_cpu_ratio_ = (uint32_t)(ZEN_OS::total_milliseconds(sys_cputime)
                                       * 1000 / ZEN_OS::total_milliseconds(last_to_now));
    } else {
        ZEN_Trace_LogMsg::debug_errorex(
            "system_uptime = %llu, process_start_time = %llu",
            ZEN_OS::total_milliseconds(now_system_perf_.up_time_),
            ZEN_OS::total_milliseconds(now_process_perf_.start_time_));
        system_cpu_ratio_ = 0;
    }

    if (process_cpu_ratio_ > PROCESS_CPU_RATIO_THRESHOLD
        || system_cpu_ratio_ > SYSTEM_CPU_RATIO_THRESHOLD) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] [WATCHDOG][PID:%u] point[%u] vm_size[%u] process cpu ratio [%f] threshold [%f], system cpu ratio[%f] threshold[%f] totoal process user/sys[%lld/%lld] milliseconds leave last point all/usr/sys[%lld/%lld/%lld] milliseconds.",
            self_pid_, mem_checkpoint_size_, cur_mem_usesize_,
            10.0f / process_cpu_ratio_, 60.0,
            10.0f / system_cpu_ratio_, 75.0,
            ZEN_OS::total_milliseconds(proc_utime),
            ZEN_OS::total_milliseconds(proc_stime),
            ZEN_OS::total_milliseconds(last_to_now),
            ZEN_OS::total_milliseconds(now_process_perf_.run_utime_),
            ZEN_OS::total_milliseconds(now_process_perf_.run_stime_));
    }

    int64_t temp_total_size =
        (int64_t)(now_system_perf_.cachedram_size_ + now_system_perf_.freeram_size_
                  + now_system_perf_.bufferram_size_);
    can_use_size_ = (uint64_t)temp_total_size;
    if ((int64_t)now_system_perf_.totalram_size_ <= 0) {
        mem_use_ratio_ = 0;
    } else {
        mem_use_ratio_ =
            (uint32_t)(((uint64_t)((int64_t)now_system_perf_.totalram_size_
                                   - temp_total_size) * 1000)
                       / (uint64_t)now_system_perf_.totalram_size_);
    }
    ZEN_Trace_LogMsg::debug_infoex(
        "[zenlib] [WATCHDOG][SYSTEM] cpu radio [%u] totoal usr/nice/sys/idle/iowait/hardirq/softirq [%lld/%lld/%lld/%lld/%lld/%lld/%lld] millisecondsleave last point all/use/idle[%lld/%lld/%lld] milliseconds mem ratio[%u] [totoal/can use/free/buffer/cache] [%lld/%lld/%lld/%lld/%lld] bytes",
        system_cpu_ratio_,
        ZEN_OS::total_milliseconds(now_system_perf_.user_time_),
        ZEN_OS::total_milliseconds(now_system_perf_.nice_time_),
        ZEN_OS::total_milliseconds(now_system_perf_.system_time_),
        ZEN_OS::total_milliseconds(now_system_perf_.idle_time_),
        ZEN_OS::total_milliseconds(now_system_perf_.iowait_time_),
        ZEN_OS::total_milliseconds(now_system_perf_.hardirq_time_),
        ZEN_OS::total_milliseconds(now_system_perf_.softirq_time_),
        ZEN_OS::total_milliseconds(last_to_now),
        ZEN_OS::total_milliseconds(sys_cputime),
        ZEN_OS::total_milliseconds(sys_idletime),
        mem_use_ratio_,
        now_system_perf_.totalram_size_,
        can_use_size_,
        now_system_perf_.freeram_size_,
        now_system_perf_.bufferram_size_,
        now_system_perf_.cachedram_size_);
    return ret;
}
