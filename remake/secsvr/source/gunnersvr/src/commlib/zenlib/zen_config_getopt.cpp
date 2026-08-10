// Restored from gunnersvr binary (reverse-engineered, behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_config_getopt.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_config_getopt.h"
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

// TU-local declaration; real class lives in zen_trace_log_msg.{h,cpp}
// (its generated stub header is still being restored by another task).
struct ZEN_Trace_LogMsg {
    static void debug_errorex(const char *str_format, ...);
};

namespace ZEN_OS {
uint32_t gcd(uint32_t x, uint32_t y);
char *getenv(const char *name);
}

ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION::ZEN_GETOPT_LONG_OPTION()
    : name_(), has_arg_(0), val_(0)
{
}

ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION::ZEN_GETOPT_LONG_OPTION(
    const char *name, int has_arg, int val)
    : name_(name), has_arg_(has_arg), val_(val)
{
}

ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION::~ZEN_GETOPT_LONG_OPTION()
{
}

ZEN_Get_Option::ZEN_Get_Option(int argc, char **argv, const char *optstring,
                               int optind_arg, int opterr_arg, int ordering,
                               int long_only)
{
    argc_ = argc;
    argv_ = argv;
    optind = optind_arg;
    opterr = opterr_arg;
    optarg = NULL;
    long_only_ = long_only;
    has_colon_ = 0;
    nextchar_ = NULL;
    optopt_ = 0;
    ordering_ = ordering;
    nonopt_start_ = optind;
    nonopt_end_ = optind;
    long_option_ = NULL;

    if (ZEN_OS::getenv("POSIXLY_CORRECT") != NULL)
        ordering_ = 1;

    if (optstring != NULL) {
        const char *s = optstring;
        while (*s == '-' || *s == ':' || *s == '+') {
            if (*s == '-')
                ordering_ = 3;
            else if (*s == ':')
                has_colon_ = 1;
            else if (*s == '+')
                ordering_ = 1;
            s++;
        }
        optstring_.assign(optstring, strlen(optstring));
    }
}

ZEN_Get_Option::~ZEN_Get_Option()
{
}

void ZEN_Get_Option::permute_args()
{
    uint32_t nnonopts = (uint32_t)(nonopt_end_ - nonopt_start_);
    uint32_t nopts = (uint32_t)(optind - nonopt_end_);
    uint32_t cyclelen = ZEN_OS::gcd(nnonopts, nopts);
    uint32_t ncycle = (uint32_t)(optind - nonopt_start_) / cyclelen;
    int cstart;
    int pos;
    char *swap;

    optind -= (int)nnonopts;

    for (uint32_t i = 0; i < cyclelen; i++) {
        cstart = nonopt_end_ + (int)i;
        pos = cstart;
        for (uint32_t j = 0; j < ncycle; j++) {
            if (pos >= nonopt_end_)
                pos -= (int)nnonopts;
            else
                pos += (int)nopts;
            swap = argv_[pos];
            argv_[pos] = argv_[cstart];
            argv_[cstart] = swap;
        }
    }
}

int ZEN_Get_Option::permute()
{
    if (nonopt_start_ != nonopt_end_ && nonopt_start_ != optind)
        permute_args();

    int opt_end = optind;
    nonopt_start_ = optind;
    int j = optind;
    if (optind < argc_) {
        for (;;) {
            if (argv_[j][0] == '-' && argv_[j][1] != '\0') {
                opt_end = optind;
                break;
            }
            j++;
            optind = j;
            if (j >= argc_) {
                opt_end = optind;
                break;
            }
        }
    }
    nonopt_end_ = j;

    if (opt_end != argc_) {
        if (strcmp(argv_[opt_end], "--") == 0) {
            optind = opt_end + 1;
            if (nonopt_start_ != j && optind != j)
                permute_args();
            if (optind != argc_)
                return optind;
        } else {
            return 0;
        }
    }

    if (nonopt_start_ != nonopt_end_)
        optind = nonopt_start_;
    return -1;
}

int ZEN_Get_Option::nextchar_i()
{
    if (ordering_ == 2) {
        int retval = permute();
        if (retval + 1 == 0)
            return -1;
    }

    if (optind >= argc_) {
        nextchar_ = NULL;
        return -1;
    }

    nextchar_ = argv_[optind];
    if (nextchar_[0] != '-' || nextchar_[1] == '\0') {
        if ((unsigned int)(ordering_ - 1) <= 1) {
            nextchar_ = NULL;
            return -1;
        }
        optarg = argv_[optind];
        nextchar_ = NULL;
        optind++;
        return 1;
    }

    nextchar_++;
    if (nextchar_[-1] == '-') {
        if (nextchar_[1] == '\0') {
            optind++;
            nextchar_ = NULL;
            return -1;
        }
        if (long_opts_.size() == 0)
            return 0;
        nextchar_++;
        return 0;
    }
    return 0;
}

int ZEN_Get_Option::long_option_i()
{
    char *s = nextchar_;
    size_t len;
    if (*s != '\0' && *s != '=') {
        s++;
        while (*s != '\0' && *s != '=')
            s++;
        len = (size_t)(s - nextchar_);
    } else {
        len = 0;
    }

    last_option_.assign(nextchar_, len);

    size_t size = long_opts_.size();
    ZEN_GETOPT_LONG_OPTION p;
    int hits = 0;
    size_t i;
    ZEN_GETOPT_LONG_OPTION *pfound = NULL;
    int exact = 1;

    if (size != 0) {
        for (i = 0; i < size; i++) {
            p = long_opts_[i];
            if (strncmp(p.name_.c_str(), nextchar_, len) == 0) {
                hits++;
                if (len == p.name_.length()) {
                    exact = 0;
                    pfound = &p;
                    break;
                }
                pfound = &p;
            }
        }
    }

    if (hits > 1 && exact) {
        if (opterr)
            ZEN_Trace_LogMsg::debug_errorex(
                "[zenlib] %s: option `%s' is ambiguous", argv_[0], argv_[optind]);
        nextchar_ = NULL;
        optind++;
        return '?';
    }

    if (pfound == NULL) {
        if (long_only_ && argv_[optind][1] != '-' &&
            optstring_.find(nextchar_[0], 0) != std::string::npos) {
            return short_option_i();
        }
        if (opterr)
            ZEN_Trace_LogMsg::debug_errorex(
                "[zenlib] %s: illegal long option '--%s'", argv_[0], nextchar_);
        optind++;
        nextchar_ = NULL;
        return '?';
    }

    optind++;
    if (*s == '\0') {
        if (pfound->has_arg_ == ARG_REQUIRED) {
            if (optind < argc_) {
                optarg = argv_[optind];
                optind++;
            } else {
                if (opterr)
                    ZEN_Trace_LogMsg::debug_errorex(
                        "[zenlib] %s: long option '--%s' requires an argument\n",
                        argv_[0], pfound->name_.c_str());
                nextchar_ = NULL;
                optopt_ = pfound->val_;
                return has_colon_ ? ':' : '?';
            }
        }
        nextchar_ = NULL;
        long_option_ = pfound;
        optopt_ = pfound->val_;
        return pfound->val_;
    } else {
        if (pfound->has_arg_ == NO_ARG) {
            if (opterr)
                ZEN_Trace_LogMsg::debug_errorex(
                    "[zenlib] %s: long option `--%s' doesn't allow an argument\n",
                    argv_[0], pfound->name_.c_str());
        } else {
            optarg = s + 1;
        }
        nextchar_ = NULL;
        long_option_ = pfound;
        optopt_ = pfound->val_;
        return pfound->val_;
    }
}

int ZEN_Get_Option::short_option_i()
{
    char opt = *nextchar_;
    nextchar_++;
    last_option_.assign(1, opt);

    char *oli = strchr(optstring_.c_str(), opt);
    if (*nextchar_ == '\0')
        optind++;

    if (oli == NULL || opt == ':') {
        if (opterr)
            ZEN_Trace_LogMsg::debug_errorex(
                "[zenlib] %s: illegal short option -- %c\n", argv_[0], opt);
        return '?';
    }

    if (opt == 'W') {
        if (oli[1] == ';') {
            if (*nextchar_ == '\0')
                nextchar_ = argv_[optind];
            return long_option_i();
        }
    }

    optopt_ = *oli;
    if (oli[1] == ':') {
        if (oli[2] == ':') {
            if (*nextchar_ != '\0') {
                optind++;
                optarg = nextchar_;
            } else {
                optarg = NULL;
            }
            nextchar_ = NULL;
            return opt;
        }
        if (*nextchar_ != '\0') {
            optind++;
            optarg = nextchar_;
        } else if (optind < argc_) {
            optarg = argv_[optind];
            optind++;
        } else {
            if (opterr)
                ZEN_Trace_LogMsg::debug_errorex(
                    "[zenlib] %s: short option requires an argument -- %c",
                    argv_[0], opt);
            nextchar_ = NULL;
            return has_colon_ ? ':' : '?';
        }
        nextchar_ = NULL;
        return opt;
    }
    return opt;
}

int ZEN_Get_Option::operator()()
{
    optarg = NULL;
    long_option_ = NULL;

    if (argv_ == NULL) {
        optind = 0;
        return -1;
    }

    if (nextchar_ != NULL && *nextchar_ != '\0') {
        if (argv_[optind][0] == '-') {
            if (argv_[optind][1] == '-')
                return long_option_i();
        }
        return long_only_ ? long_option_i() : short_option_i();
    }

    int retval = nextchar_i();
    if (retval != 0)
        return retval;

    if (argv_[optind][0] != '-')
        return long_only_ ? long_option_i() : short_option_i();
    if (argv_[optind][1] == '-')
        return long_option_i();
    return long_only_ ? long_option_i() : short_option_i();
}

const char *ZEN_Get_Option::long_option() const
{
    return long_option_ ? long_option_->name_.c_str() : NULL;
}

// mangled: _ZN14ZEN_Get_Option11long_optionEPKcNS_15OPTION_ARG_MODEE
int ZEN_Get_Option::long_option(const char *name, OPTION_ARG_MODE has_arg)
{
    return long_option(name, 0, has_arg);
}

// mangled: _ZN14ZEN_Get_Option11long_optionEPKciNS_15OPTION_ARG_MODEE
int ZEN_Get_Option::long_option(const char *name, int short_option,
                                OPTION_ARG_MODE has_arg)
{
    if (isalnum(short_option)) {
        const char *p = strchr(optstring_.c_str(), short_option);
        if (p != NULL) {
            if (p[1] == ':') {
                if (p[2] == ':') {
                    if (has_arg != ARG_OPTIONAL) {
                        if (opterr)
                            ZEN_Trace_LogMsg::debug_errorex(
                                "[zenlib] Existing short option '%c' takes "
                                "optional argument; adding %s requires "
                                "ARG_OPTIONAL\n",
                                short_option, name);
                        return -1;
                    }
                } else {
                    if (has_arg != ARG_REQUIRED) {
                        if (opterr)
                            ZEN_Trace_LogMsg::debug_errorex(
                                "[zenlib] Existing short option '%c' requires "
                                "an argument; adding %s requires "
                                "ARG_REQUIRED.\n",
                                short_option, name);
                        return -1;
                    }
                }
            } else {
                if (has_arg != NO_ARG) {
                    if (opterr)
                        ZEN_Trace_LogMsg::debug_errorex(
                            "[zenlib] Existing short option '%c' does not "
                            "accept an argument; adding %s requires NO_ARG\n",
                            short_option, name);
                    return -1;
                }
            }
        } else {
            optstring_ += (char)short_option;
            if (has_arg == ARG_REQUIRED)
                optstring_ += ":";
            else if (has_arg == ARG_OPTIONAL)
                optstring_ += "::";
        }
    }

    ZEN_GETOPT_LONG_OPTION opt(name, has_arg, short_option);
    long_opts_.push_back(opt);
    return 0;
}
