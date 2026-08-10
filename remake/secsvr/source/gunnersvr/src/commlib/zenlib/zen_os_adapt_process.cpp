// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_process.cpp
// Parses /proc/<pid>/stat and /proc/<pid>/statm with ZEN_OS::skip_token.
// Original quirks preserved:
//   - signal(0, ...) returns NULL without calling ::signal;
//   - get_process_perf multiplies the 5th statm field (lib) into data_size_
//     only via skip_token before parsing (i.e. the parse lands on field 6,
//     data); utime/stime/starttime are divided by _SC_CLK_TCK.

#include "src/commlib/zenlib/zen_os_adapt_process.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

namespace ZEN_OS {

int read_file_data(const char *filename, char *buffer, size_t buf_len,
                   size_t *read_len);
const char *skip_token(const char *str);
const timeval get_uptime();
const timeval timeval_sub(const timeval &left, const timeval &right,
                          bool safe);

sighandler_t signal(int signum, sighandler_t func) {
    if (signum == 0) {
        return NULL;
    }
    return ::signal(signum, func);
}

char *getenv(const char *name) {
    return ::getenv(name);
}

pid_t setsid() {
    return ::setsid();
}

pid_t fork() {
    return ::fork();
}

pid_t getppid() {
    return ::getppid();
}

pid_t getpid() {
    return ::getpid();
}

int get_process_perf(pid_t process_id, ZEN_PROCESS_PERFORM *prc_perf_info) {
    int ret;
    char process_dir[4096];
    char buffer[4096];
    size_t read_len = 0;
    const char *in_para;
    char *out_para;
    uint64_t time_data;
    long int cpu_tick_precision;
    long int page_size;
    timeval uptime;

    prc_perf_info->process_id_ = process_id;
    snprintf(process_dir, sizeof(process_dir), "/proc/%u/stat", process_id);
    ret = read_file_data(process_dir, buffer, sizeof(buffer), &read_len);
    if (ret != 0) {
        return ret;
    }
    out_para = NULL;
    cpu_tick_precision = sysconf(_SC_CLK_TCK);
    in_para = skip_token(buffer);
    time_data = strtoull(in_para, &out_para, 10);
    prc_perf_info->run_utime_.tv_sec = time_data / cpu_tick_precision;
    prc_perf_info->run_utime_.tv_usec =
        (time_data % cpu_tick_precision) * (1000000 / cpu_tick_precision);
    time_data = strtoull(out_para, &out_para, 10);
    prc_perf_info->run_stime_.tv_sec = time_data / cpu_tick_precision;
    prc_perf_info->run_stime_.tv_usec =
        (time_data % cpu_tick_precision) * (1000000 / cpu_tick_precision);
    in_para = skip_token(out_para);
    in_para = skip_token(in_para);
    prc_perf_info->priority_ = strtol(in_para, &out_para, 10);
    prc_perf_info->nice_ = strtol(out_para, &out_para, 10);
    in_para = skip_token(out_para);
    in_para = skip_token(in_para);
    time_data = strtoull(in_para, &out_para, 10);
    prc_perf_info->start_time_.tv_sec = time_data / cpu_tick_precision;
    prc_perf_info->start_time_.tv_usec =
        (time_data % cpu_tick_precision) * (1000000 / cpu_tick_precision);
    uptime = get_uptime();
    prc_perf_info->running_time_ =
        timeval_sub(uptime, prc_perf_info->start_time_, 1);

    snprintf(process_dir, sizeof(process_dir), "/proc/%u/statm", process_id);
    ret = read_file_data(process_dir, buffer, sizeof(buffer), &read_len);
    if (ret != 0) {
        return ret;
    }
    out_para = NULL;
    page_size = sysconf(_SC_PAGESIZE);
    prc_perf_info->vm_size_ = strtol(buffer, &out_para, 10) * page_size;
    prc_perf_info->resident_set_ =
        strtol(out_para, &out_para, 10) * page_size;
    prc_perf_info->shared_size_ = strtol(out_para, &out_para, 10) * page_size;
    prc_perf_info->text_size_ = strtol(out_para, &out_para, 10) * page_size;
    in_para = skip_token(out_para);
    prc_perf_info->data_size_ = strtol(in_para, &out_para, 10) * page_size;
    return 0;
}

int get_self_perf(ZEN_PROCESS_PERFORM *prc_perf_info) {
    pid_t process_id = getpid();
    return get_process_perf(process_id, prc_perf_info);
}

} // namespace ZEN_OS
