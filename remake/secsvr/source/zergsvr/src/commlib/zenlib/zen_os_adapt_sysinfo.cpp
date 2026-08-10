// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_sysinfo.cpp
// sysinfo()/sysconf() plus /proc parsing for system performance.
// Original quirks preserved:
//   - read_proc_get_stat divides the idle CPU time by _SC_NPROCESSORS_CONF
//     (sec and usec) while user/nice/system are left un-divided;
//   - read_proc_get_systemperf walks a fixed number of /proc/meminfo lines
//     after SwapCached (8 skip_line calls) before reading totalswap/freeswap;
//   - load averages are scaled by 1/65536 (float constant 1.52587890625e-05).

#include "src/commlib/zenlib/zen_os_adapt_sysinfo.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <unistd.h>

namespace ZEN_OS {

int read_file_data(const char *filename, char *buffer, size_t buf_len,
                   size_t *read_len);
const char *skip_line(const char *str);
const char *skip_token(const char *str, char separator_char);
const char *skip_separator(const char *str, char separator_char);
const char *skip_token(const char *str);
const timeval get_uptime();

int get_system_info(ZEN_SYSTEM_INFO *zen_system_info) {
    struct sysinfo info;
    int ret = ::sysinfo(&info);
    if (ret != 0) {
        return -1;
    }
    zen_system_info->nprocs_conf_ = sysconf(_SC_NPROCESSORS_CONF);
    zen_system_info->nprocs_av_ = sysconf(_SC_NPROCESSORS_ONLN);
    zen_system_info->totalram_size_ = (int64_t)info.totalram * info.mem_unit;
    zen_system_info->freeram_size_ = (int64_t)info.freeram * info.mem_unit;
    zen_system_info->shared_size_ = (int64_t)info.sharedram * info.mem_unit;
    zen_system_info->bufferram_size_ =
        (int64_t)info.bufferram * info.mem_unit;
    zen_system_info->totalswap_size_ = (int64_t)info.totalswap * info.mem_unit;
    zen_system_info->freeswap_size_ = (int64_t)info.freeswap * info.mem_unit;
    return 0;
}

int read_proc_get_uptime(ZEN_SYSTEM_PERFORMANCE *info) {
    const char *PROC_FILENAME_UPTIME = "/proc/uptime";
    const uint32_t SEC_PER_USEC = 1000000;
    char buffer[4096];
    size_t read_len = 0;
    int ret;
    char *out_para;
    double uptime;

    ret = read_file_data(PROC_FILENAME_UPTIME, buffer, sizeof(buffer),
                         &read_len);
    if (ret != 0) {
        return ret;
    }
    out_para = NULL;
    uptime = strtod(buffer, &out_para);
    info->up_time_.tv_sec = (long)uptime;
    info->up_time_.tv_usec =
        (long)((uptime - (double)info->up_time_.tv_sec) * SEC_PER_USEC);
    return 0;
}

int read_proc_get_stat(ZEN_SYSTEM_PERFORMANCE *info) {
    const char *PROC_FILENAME_STAT = "/proc/stat";
    const uint64_t SEC_PER_USEC = 1000000;
    char buffer[4096];
    size_t read_len = 0;
    int ret;
    const char *in_para;
    char *out_para;
    long int cpu_tick_precision;
    long int cpu_config_num;
    uint64_t time_data;

    ret = read_file_data(PROC_FILENAME_STAT, buffer, sizeof(buffer), &read_len);
    if (ret != 0) {
        return ret;
    }
    out_para = NULL;
    cpu_tick_precision = sysconf(_SC_CLK_TCK);
    cpu_config_num = sysconf(_SC_NPROCESSORS_CONF);
    in_para = skip_token(buffer);

    time_data = strtoull(in_para, &out_para, 10);
    info->user_time_.tv_sec = time_data / cpu_tick_precision;
    info->user_time_.tv_usec =
        (time_data % cpu_tick_precision) * (SEC_PER_USEC / cpu_tick_precision);

    time_data = strtoull(out_para, &out_para, 10);
    info->nice_time_.tv_sec = time_data / cpu_tick_precision;
    info->nice_time_.tv_usec =
        (time_data % cpu_tick_precision) * (SEC_PER_USEC / cpu_tick_precision);

    time_data = strtoull(out_para, &out_para, 10);
    info->system_time_.tv_sec = time_data / cpu_tick_precision;
    info->system_time_.tv_usec =
        (time_data % cpu_tick_precision) * (SEC_PER_USEC / cpu_tick_precision);

    time_data = strtoull(out_para, &out_para, 10);
    info->idle_time_.tv_sec =
        (time_data / cpu_tick_precision) / cpu_config_num;
    info->idle_time_.tv_usec =
        ((time_data % cpu_tick_precision) *
         (SEC_PER_USEC / cpu_tick_precision)) /
        cpu_config_num;

    time_data = strtoull(out_para, &out_para, 10);
    info->iowait_time_.tv_sec = time_data / cpu_tick_precision;
    info->iowait_time_.tv_usec =
        (time_data % cpu_tick_precision) * (SEC_PER_USEC / cpu_tick_precision);

    time_data = strtoull(out_para, &out_para, 10);
    info->hardirq_time_.tv_sec = time_data / cpu_tick_precision;
    info->hardirq_time_.tv_usec =
        (time_data % cpu_tick_precision) * (SEC_PER_USEC / cpu_tick_precision);

    time_data = strtoull(out_para, &out_para, 10);
    info->softirq_time_.tv_sec = time_data / cpu_tick_precision;
    info->softirq_time_.tv_usec =
        (time_data % cpu_tick_precision) * (SEC_PER_USEC / cpu_tick_precision);

    info->up_time_ = get_uptime();
    return 0;
}

int read_fun_get_systemperf(ZEN_SYSTEM_PERFORMANCE *zen_system_perf) {
    struct sysinfo info;
    int ret = ::sysinfo(&info);
    if (ret != 0) {
        return -1;
    }
    ret = read_proc_get_stat(zen_system_perf);
    if (ret != 0) {
        return -1;
    }
    zen_system_perf->sys_loads_[0] = (double)info.loads[0] * (1.0 / 65536.0);
    zen_system_perf->sys_loads_[1] = (double)info.loads[1] * (1.0 / 65536.0);
    zen_system_perf->sys_loads_[2] = (double)info.loads[2] * (1.0 / 65536.0);
    zen_system_perf->processes_num_ = info.procs;
    zen_system_perf->running_num_ = 0;
    zen_system_perf->totalram_size_ = (int64_t)info.totalram * info.mem_unit;
    zen_system_perf->freeram_size_ = (int64_t)info.freeram * info.mem_unit;
    zen_system_perf->shared_size_ = (int64_t)info.sharedram * info.mem_unit;
    zen_system_perf->bufferram_size_ =
        (int64_t)info.bufferram * info.mem_unit;
    zen_system_perf->cachedram_size_ = 0;
    zen_system_perf->totalswap_size_ = (int64_t)info.totalswap * info.mem_unit;
    zen_system_perf->freeswap_size_ = (int64_t)info.freeswap * info.mem_unit;
    zen_system_perf->swapcached_size_ = 0;
    zen_system_perf->up_time_ = get_uptime();
    return 0;
}

int read_proc_get_systemperf(ZEN_SYSTEM_PERFORMANCE *info) {
    char buffer[4096];
    size_t read_len = 0;
    int ret;
    const char *in_para;
    char *out_para;
    int i;
    uint64_t time_data;

    ret = read_file_data("/proc/loadavg", buffer, sizeof(buffer), &read_len);
    if (ret != 0) {
        return ret;
    }
    out_para = NULL;
    info->sys_loads_[0] = strtod(buffer, &out_para);
    info->sys_loads_[1] = strtod(out_para, &out_para);
    info->sys_loads_[2] = strtod(out_para, &out_para);
    info->running_num_ = strtoul(out_para, &out_para, 0);
    info->processes_num_ =
        strtoul(skip_separator(out_para, '/'), &out_para, 0);

    ret = read_file_data("/proc/meminfo", buffer, sizeof(buffer), &read_len);
    if (ret != 0) {
        return ret;
    }
    out_para = NULL;
    in_para = skip_token(buffer);
    time_data = strtoull(in_para, &out_para, 10);
    info->totalram_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    info->freeram_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    info->bufferram_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    info->cachedram_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    info->swapcached_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    for (i = 0; i < 7; ++i) {
        in_para = skip_line(in_para);
    }
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    info->totalswap_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    info->freeswap_size_ = time_data * 1024;
    in_para = skip_line(in_para);
    return read_proc_get_stat(info);
}

int get_system_performance(ZEN_SYSTEM_PERFORMANCE *zen_system_perf) {
    return read_proc_get_systemperf(zen_system_perf);
}

} // namespace ZEN_OS
