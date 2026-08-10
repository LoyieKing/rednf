// Restored from gunnersvr oracle binary (DWARF + disassembly).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_trace_log_basic.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// 语义要点（与 0x0809d250..0x0809fc90 反汇编一致）：
//  - 默认构造：div=LOGDEVIDE_NONE、output_way=5、thread_synchro=false、
//    max_size=40960000、reserve=100、record_info=3、current_click=1、
//    out_level_mask=RS_TRACE、if_output_log=true，三个 string reserve(0x1020)。
//  - make_configure：max_size 钳制到 [4096000, 4096000000]；dirname 取目录、
//    mkdir_recurse(0755) 失败时 printf "mkdir %s fail. err=%s\n"；
//    按 div 生成 log_file_name_（BY_SIZE 用 create_id_logname(0) 并读 filesize，
//    NONE 用 prefix+".log"，时间型用 time/3600 的 current_click_ + create_time_logname）。
//  - create_time_logname：QUARTER 复用 5 分钟格式、SIXHOUR 复用小时格式（跳转表）。
//  - stringbuf_loghead：位 1=时间戳(26 字节)、位 2=级别标签、位 4=PID、位 8=TID；
//    原版怪癖：级别标签按 outlevel-2 索引（DEBUG→"[TRACE]"…ALERT→"[ERROR]"，
//    TRACE/FATAL 无标签）；snprintf 后 "剩余" 按 edi=edi-sz_use_len 更新（非减去本次长度）。
//  - output_log_info：0x1 写文件（先 make_new_logfile，state&(badbit|failbit)==0 才写）、
//    0x2 写 cout、0x4 写 cerr；thread_synchro 时加锁。
//  - make_new_logfile：BY_SIZE 时 to_new_file=(size>max)；时间型按 current_click_ 变化
//    决定换文件；is_open 时 (to_new_file || rdstate()) 才重开并复位 size_log_file_。
//  - finalize：关闭文件并复位各配置。

#include <iostream> // 保留：原 TU 包含 <iostream>，产生同款 ios_base::Init 静态初始化

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

// TU-local declarations; real definitions live in zen_os_adapt_*.cpp.
namespace ZEN_OS {
unsigned int getpid();
unsigned int pthread_self();
const char * timestamp(const timeval *timeval, char *str_date_time,
                       size_t datetime_strlen);
const char * dirname(const char *path_name, char *dir_name, size_t buf_len);
int mkdir_recurse(const char *pathname, mode_t mode);
int filesize(const char *filename, size_t *file_size);
DIR * opendir(const char *dir_name);
dirent * readdir(DIR *dir_handle);
int closedir(DIR *dir_handle);
} // namespace ZEN_OS

const size_t ZEN_LogTrace_Basic::LOG_TMP_BUFFER_SIZE = 4096;
const size_t ZEN_LogTrace_Basic::DEFAULT_RESERVE_FILENUM = 100;
const size_t ZEN_LogTrace_Basic::MIN_LOG_SIZE = 4096000;
const size_t ZEN_LogTrace_Basic::MAX_LOG_SIZE = 0xf4240000; // 4096000000
const size_t ZEN_LogTrace_Basic::DEFAULT_LOG_SIZE = 40960000;
const size_t ZEN_LogTrace_Basic::LEN_LOG_POSTFIX = 4;
const char ZEN_LogTrace_Basic::STR_LOG_POSTFIX[] = ".log";

// secagent (tag435)：二进制 ctor 调用 _ZN16ZEN_Thread_MutexC1Ev（默认构造），
// 无 const char* 构造器（gunnersvr 版用 protect_lock_(NULL)）。
ZEN_LogTrace_Basic::ZEN_LogTrace_Basic() : protect_lock_() {
    div_log_file_ = LOGDEVIDE_NONE;
    output_way_ = 5;
    if_thread_synchro_ = false;
    auto_new_line_ = true;
    max_size_log_file_ = DEFAULT_LOG_SIZE;
    reserve_file_num_ = DEFAULT_RESERVE_FILENUM;
    record_info_ = 3;
    current_click_ = 1;
    out_level_mask_ = RS_TRACE;
    size_log_file_ = 0;
    if_output_log_ = true;
    log_file_name_.reserve(0x1020);
    log_file_dir_.reserve(0x1020);
    log_file_prefix_.reserve(0x1020);
}

ZEN_LogTrace_Basic::~ZEN_LogTrace_Basic() {
    finalize();
}

int ZEN_LogTrace_Basic::init_time_log(ZEN_LOGFILE_DEVIDE div_log_file,
                                      const char *log_file_prefix,
                                      bool if_thread_synchro,
                                      bool auto_new_line,
                                      size_t reserve_file_num,
                                      unsigned int output_way,
                                      unsigned int head_record) {
    return initialize(div_log_file, log_file_prefix, if_thread_synchro,
                      auto_new_line, 0, reserve_file_num, output_way,
                      head_record);
}

int ZEN_LogTrace_Basic::init_size_log(const char *log_file_prefix,
                                      bool if_thread_synchro,
                                      bool auto_new_line,
                                      size_t max_size_log_file,
                                      unsigned int reserve_file_num,
                                      unsigned int output_way,
                                      unsigned int head_record) {
    ZEN_LOGFILE_DEVIDE div_log_file =
        (max_size_log_file != 0) ? LOGDEVIDE_BY_SIZE : LOGDEVIDE_NONE;
    return initialize(div_log_file, log_file_prefix, if_thread_synchro,
                      auto_new_line, max_size_log_file, reserve_file_num,
                      output_way, head_record);
}

int ZEN_LogTrace_Basic::init_stdout(bool if_thread_synchro, bool use_err_out,
                                    bool auto_new_line,
                                    unsigned int head_record) {
    unsigned int output_way;
    if (use_err_out) {
        output_way = 4;
    } else {
        output_way = 2;
    }
    return initialize(LOGDEVIDE_NONE, "", if_thread_synchro, auto_new_line,
                      0, 0, output_way, head_record);
}

int ZEN_LogTrace_Basic::initialize(ZEN_LOGFILE_DEVIDE div_log_file,
                                   const char *log_file_prefix,
                                   bool if_thread_synchro, bool auto_new_line,
                                   size_t max_size_log_file,
                                   size_t reserve_file_num,
                                   unsigned int output_way,
                                   unsigned int head_record) {
    div_log_file_ = div_log_file;
    output_way_ = output_way;
    if_thread_synchro_ = if_thread_synchro;
    auto_new_line_ = auto_new_line;
    max_size_log_file_ = max_size_log_file;
    reserve_file_num_ = reserve_file_num;
    current_click_ = 1;
    out_level_mask_ = RS_TRACE;
    size_log_file_ = 0;
    record_info_ = head_record;
    if_output_log_ = true;

    if (log_file_prefix != NULL) {
        log_file_prefix_.assign(log_file_prefix, strlen(log_file_prefix));
    }
    make_configure();
    make_new_logfile(time(NULL), true);
    return 0;
}

void ZEN_LogTrace_Basic::finalize() {
    if (log_file_handle_.is_open()) {
        if (log_file_handle_.rdbuf()->close() == NULL) {
            log_file_handle_.clear(log_file_handle_.rdstate()
                                   | std::ios::failbit);
        }
    }
    current_click_ = 1;
    out_level_mask_ = RS_TRACE;
    div_log_file_ = LOGDEVIDE_NONE;
    output_way_ = 5;
    size_log_file_ = 0;
    if_output_log_ = true;
}

void ZEN_LogTrace_Basic::enable_output_logmsg() {
    if_output_log_ = true;
}

void ZEN_LogTrace_Basic::disable_output_logmsg() {
    if_output_log_ = false;
}

ZEN_LOG_PRIORITY ZEN_LogTrace_Basic::set_log_priority(
    ZEN_LOG_PRIORITY outlevel) {
    ZEN_LOG_PRIORITY oldlevel = out_level_mask_;
    out_level_mask_ = outlevel;
    return oldlevel;
}

ZEN_LOG_PRIORITY ZEN_LogTrace_Basic::get_log_priority() {
    return out_level_mask_;
}

unsigned int ZEN_LogTrace_Basic::set_log_head(unsigned int recdinfo) {
    // 原版行为：返回新值本身（不回读旧 record_info_）。
    unsigned int tmprecdinfo = recdinfo;
    record_info_ = tmprecdinfo;
    return recdinfo;
}

unsigned int ZEN_LogTrace_Basic::get_log_head() {
    return record_info_;
}

unsigned int ZEN_LogTrace_Basic::set_output_way(unsigned int output_way) {
    unsigned int tmpsynchr = output_way_;
    output_way_ = output_way;
    return tmpsynchr;
}

unsigned int ZEN_LogTrace_Basic::get_output_way() {
    return output_way_;
}

void ZEN_LogTrace_Basic::set_thread_synchro(bool if_thread_synchro) {
    if_thread_synchro_ = if_thread_synchro;
}

bool ZEN_LogTrace_Basic::get_thread_synchro() {
    return if_thread_synchro_;
}

void ZEN_LogTrace_Basic::set_reserve_file_num(unsigned int file_num) {
    reserve_file_num_ = file_num;
    del_old_logfile(time(NULL), true);
}

unsigned int ZEN_LogTrace_Basic::get_reserve_file_num() const {
    return reserve_file_num_;
}

ZEN_LOG_PRIORITY ZEN_LogTrace_Basic::log_priorities(const char *str_priority) {
    if (strcasecmp(str_priority, "TRACE") == 0) {
        return RS_TRACE;
    }
    if (strcasecmp(str_priority, "DEBUG") == 0) {
        return RS_DEBUG;
    }
    if (strcasecmp(str_priority, "INFO") == 0) {
        return RS_INFO;
    }
    if (strcasecmp(str_priority, "ERROR") == 0) {
        return RS_ERROR;
    }
    if (strcasecmp(str_priority, "ALERT") == 0) {
        return RS_ALERT;
    }
    if (strcasecmp(str_priority, "FATAL") == 0) {
        return RS_FATAL;
    }
    // 原版怪癖：未识别级别默认返回 RS_DEBUG。
    return RS_DEBUG;
}

void ZEN_LogTrace_Basic::stringbuf_loghead(ZEN_LOG_PRIORITY outlevel,
                                           const timeval &now_time,
                                           char *log_tmp_buffer,
                                           size_t sz_buf_len,
                                           size_t &sz_use_len) {
    int len_of_out;
    sz_use_len = 0;

    if ((record_info_ & 0x1) != 0) {
        ZEN_OS::timestamp(&now_time, log_tmp_buffer, sz_buf_len);
        sz_use_len = 26;
        sz_buf_len -= 26;
    }

    if ((record_info_ & 0x2) != 0 && outlevel <= RS_FATAL) {
        const char *str_priority = NULL;
        // 原版怪癖：级别标签按 outlevel-2 索引，TRACE(1)/FATAL(6) 无标签。
        switch (outlevel) {
        case RS_DEBUG:
            str_priority = "[TRACE]";
            break;
        case RS_INFO:
            str_priority = "[DEBUG]";
            break;
        case RS_ERROR:
            str_priority = "[INFO]";
            break;
        case RS_ALERT:
            str_priority = "[ERROR]";
            break;
        default:
            break;
        }
        if (str_priority != NULL) {
            len_of_out = snprintf(log_tmp_buffer + sz_use_len,
                                  sz_buf_len, "%s", str_priority);
            sz_use_len += len_of_out;
            // 原版怪癖："剩余" 更新为 edi - 新总长（非减本次长度），予以保留。
            sz_buf_len -= sz_use_len;
        }
    }

    if ((record_info_ & 0x4) != 0) {
        len_of_out = snprintf(log_tmp_buffer + sz_use_len, sz_buf_len,
                              "[PID:%u]", ZEN_OS::getpid());
        sz_use_len += len_of_out;
        sz_buf_len -= sz_use_len;
    }

    if ((record_info_ & 0x8) != 0) {
        len_of_out = snprintf(log_tmp_buffer + sz_use_len, sz_buf_len,
                              "[TID:%u]", ZEN_OS::pthread_self());
        sz_use_len += len_of_out;
    }
}

void ZEN_LogTrace_Basic::create_id_logname(size_t logfileid,
                                           string &idlogfilename) {
    char tmpbuf[31];
    if (logfileid == 0) {
        snprintf(tmpbuf, sizeof(tmpbuf), "%s", STR_LOG_POSTFIX);
    } else {
        snprintf(tmpbuf, sizeof(tmpbuf), "%s.%u", STR_LOG_POSTFIX,
                 (unsigned int)logfileid);
    }
    idlogfilename.assign(log_file_prefix_);
    idlogfilename.append(tmpbuf, strlen(tmpbuf));
}

void ZEN_LogTrace_Basic::create_time_logname(time_t tmt,
                                             string &logfilename) {
    tm curtm;
    char tmpbuf[65];
    size_t buflen;

    curtm = *localtime(&tmt);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    switch (div_log_file_) {
    case LOGDEVIDE_BY_FIVEMINUTE:
    case LOGDEVIDE_BY_QUARTER:
        strftime(tmpbuf, sizeof(tmpbuf) - 1, "_%Y%m%d_%H_%M", &curtm);
        break;
    case LOGDEVIDE_BY_HOUR:
    case LOGDEVIDE_BY_SIXHOUR:
        strftime(tmpbuf, sizeof(tmpbuf) - 1, "_%Y%m%d_%H", &curtm);
        break;
    case LOGDEVIDE_BY_DATE:
        strftime(tmpbuf, sizeof(tmpbuf) - 1, "_%Y%m%d", &curtm);
        break;
    case LOGDEVIDE_BY_MONTH:
        strftime(tmpbuf, sizeof(tmpbuf) - 1, "_%Y%m", &curtm);
        break;
    case LOGDEVIDE_BY_YEAR:
        strftime(tmpbuf, sizeof(tmpbuf) - 1, "_%Y", &curtm);
        break;
    default:
        break;
    }
    strcat(tmpbuf, STR_LOG_POSTFIX);
    logfilename.assign(log_file_prefix_);
    logfilename.append(tmpbuf, strlen(tmpbuf));
}

void ZEN_LogTrace_Basic::del_old_id_logfile() {
    string dellogfname;
    string oldlogfilename;
    string renamefilename;
    size_t i;

    dellogfname.reserve(0x1010);
    create_id_logname(reserve_file_num_ - 1, dellogfname);
    remove(dellogfname.c_str());

    oldlogfilename.reserve(0x200);
    renamefilename.reserve(0x200);
    for (i = reserve_file_num_ - 1; i > 0; --i) {
        create_id_logname(i - 1, oldlogfilename);
        create_id_logname(i, renamefilename);
        rename(oldlogfilename.c_str(), renamefilename.c_str());
    }
}

void ZEN_LogTrace_Basic::make_configure() {
    int ret;
    char dir_name[0x1001];
    time_t cur_time;

    if (max_size_log_file_ <= 0x3e7fff) {
        max_size_log_file_ = 0x3e8000;
    } else if (max_size_log_file_ > 0xf4240000) {
        max_size_log_file_ = 0xf4240000;
    }

    dir_name[0] = 0;
    ZEN_OS::dirname(log_file_prefix_.c_str(), dir_name, sizeof(dir_name));
    log_file_dir_.assign(dir_name, strlen(dir_name));
    ret = ZEN_OS::mkdir_recurse(log_file_dir_.c_str(), 0x1ed);
    if (ret != 0) {
        printf("mkdir %s fail. err=%s\n", log_file_dir_.c_str(),
               strerror(errno));
    }

    switch (div_log_file_) {
    case LOGDEVIDE_BY_SIZE:
        create_id_logname(0, log_file_name_);
        if (div_log_file_ == LOGDEVIDE_BY_SIZE) {
            if (ZEN_OS::filesize(log_file_name_.c_str(), &size_log_file_)
                != 0) {
                size_log_file_ = 0;
            }
        }
        break;
    case LOGDEVIDE_NONE: {
        string oldlogfilename(log_file_prefix_);
        oldlogfilename.append(STR_LOG_POSTFIX, strlen(STR_LOG_POSTFIX));
        log_file_name_.assign(oldlogfilename);
        break;
    }
    default:
        if (div_log_file_ >= LOGDEVIDE_BY_FIVEMINUTE
            && div_log_file_ <= LOGDEVIDE_BY_YEAR) {
            cur_time = time(NULL);
            current_click_ = cur_time / 3600;
            create_time_logname(cur_time, log_file_name_);
        }
        break;
    }
}

void ZEN_LogTrace_Basic::del_old_time_logfile(time_t cur_time, bool init) {
    string del_logfile_name;
    time_t del_file_time;
    DIR *dir_handle;
    dirent *find_file;
    int ret_cmp;
    string process_file_name;
    size_t filename_len;

    del_logfile_name.reserve(0x1010);
    switch (div_log_file_) {
    case LOGDEVIDE_BY_FIVEMINUTE:
        del_file_time = cur_time - reserve_file_num_ * 300;
        break;
    case LOGDEVIDE_BY_QUARTER:
        del_file_time = cur_time - reserve_file_num_ * 900;
        break;
    case LOGDEVIDE_BY_HOUR:
        del_file_time = cur_time - reserve_file_num_ * 3600;
        break;
    case LOGDEVIDE_BY_SIXHOUR:
        del_file_time = cur_time - reserve_file_num_ * 21600;
        break;
    case LOGDEVIDE_BY_DATE:
        del_file_time = cur_time - reserve_file_num_ * 86400;
        break;
    case LOGDEVIDE_BY_MONTH:
        del_file_time = cur_time - reserve_file_num_ * 2678400;
        break;
    case LOGDEVIDE_BY_YEAR:
        del_file_time = cur_time - reserve_file_num_ * 31622400;
        break;
    case LOGDEVIDE_NONE: {
        string oldlogfilename(log_file_prefix_);
        oldlogfilename.append(STR_LOG_POSTFIX, strlen(STR_LOG_POSTFIX));
        log_file_name_.assign(oldlogfilename);
        del_file_time = 0;
        break;
    }
    default:
        del_file_time = 0;
        break;
    }

    create_time_logname(del_file_time, del_logfile_name);

    if (init == false) {
        remove(del_logfile_name.c_str());
        return;
    }

    dir_handle = ZEN_OS::opendir(log_file_dir_.c_str());
    if (dir_handle == NULL) {
        return;
    }
    while ((find_file = ZEN_OS::readdir(dir_handle)) != NULL) {
        process_file_name = log_file_dir_;
        process_file_name.append(find_file->d_name,
                                 strlen(find_file->d_name));
        if (strncmp(process_file_name.c_str(), log_file_prefix_.c_str(),
                    log_file_prefix_.length()) != 0) {
            continue;
        }
        filename_len = strlen(find_file->d_name);
        if (filename_len <= LEN_LOG_POSTFIX) {
            continue;
        }
        if (strcmp(find_file->d_name + filename_len - LEN_LOG_POSTFIX,
                   STR_LOG_POSTFIX) != 0) {
            continue;
        }
        ret_cmp = strcmp(process_file_name.c_str(), del_logfile_name.c_str());
        if (ret_cmp <= 0) {
            remove(process_file_name.c_str());
        }
    }
    ZEN_OS::closedir(dir_handle);
}

void ZEN_LogTrace_Basic::del_old_logfile(time_t cur_time, bool init) {
    if (reserve_file_num_ != 0) {
        if (div_log_file_ >= LOGDEVIDE_BY_FIVEMINUTE
            && div_log_file_ <= LOGDEVIDE_BY_YEAR) {
            del_old_time_logfile(cur_time, init);
        } else if (div_log_file_ == LOGDEVIDE_BY_SIZE) {
            del_old_id_logfile();
        }
        errno = 0;
    }
}

void ZEN_LogTrace_Basic::make_new_logfile(time_t cur_time, bool init) {
    bool to_new_file = false;
    time_t cur_click;
    string new_file_name;

    if (div_log_file_ == LOGDEVIDE_BY_SIZE) {
        to_new_file = (size_log_file_ > max_size_log_file_);
    } else if (div_log_file_ == LOGDEVIDE_NONE) {
        string oldlogfilename(log_file_prefix_);
        oldlogfilename.append(STR_LOG_POSTFIX, strlen(STR_LOG_POSTFIX));
        log_file_name_.assign(oldlogfilename);
    } else if (div_log_file_ >= LOGDEVIDE_BY_HOUR
               && div_log_file_ <= LOGDEVIDE_BY_YEAR) {
        cur_click = cur_time / 3600;
        if (cur_click != current_click_) {
            current_click_ = cur_click;
            new_file_name.reserve(0x200);
            create_time_logname(cur_time, new_file_name);
            if (new_file_name != log_file_name_) {
                log_file_name_ = new_file_name;
                to_new_file = true;
            }
        }
    }

    if (log_file_handle_.is_open()) {
        if (to_new_file || log_file_handle_.rdstate()) {
            if (log_file_handle_.rdbuf()->close() == NULL) {
                log_file_handle_.clear(log_file_handle_.rdstate()
                                       | std::ios::failbit);
            }
            del_old_logfile(cur_time, init);
            log_file_handle_.clear();
            log_file_handle_.open(log_file_name_.c_str(), std::ios::app);
            if (!log_file_handle_) {
                log_file_handle_.clear(log_file_handle_.rdstate()
                                       | std::ios::failbit);
            } else {
                log_file_handle_.clear();
            }
            size_log_file_ = 0;
        }
    } else if (to_new_file) {
        del_old_logfile(cur_time, init);
        log_file_handle_.clear();
        log_file_handle_.open(log_file_name_.c_str(), std::ios::app);
        if (!log_file_handle_) {
            log_file_handle_.clear(log_file_handle_.rdstate()
                                   | std::ios::failbit);
        }
    }
}

void ZEN_LogTrace_Basic::output_log_info(const timeval &now_time,
                                         char *log_tmp_buffer,
                                         size_t sz_use_len) {
    if (if_thread_synchro_) {
        protect_lock_.lock();
    }
    if ((output_way_ & 0x1) != 0) {
        make_new_logfile(now_time.tv_sec, false);
        if ((log_file_handle_.rdstate()
             & (std::ios::badbit | std::ios::failbit)) == 0) {
            log_file_handle_.write(log_tmp_buffer, (int)sz_use_len);
            log_file_handle_.flush();
            size_log_file_ += sz_use_len;
        }
    }
    if ((output_way_ & 0x2) != 0) {
        std::cout.write(log_tmp_buffer, (int)sz_use_len);
    }
    if ((output_way_ & 0x4) != 0) {
        std::cerr.write(log_tmp_buffer, (int)sz_use_len);
    }
    if (if_thread_synchro_) {
        protect_lock_.unlock();
    }
}
