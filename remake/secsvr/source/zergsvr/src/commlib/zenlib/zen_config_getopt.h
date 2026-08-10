// Restored from binary (gunnersvr) + DWARF type info.
// Original path: src/commlib/zenlib/zen_config_getopt.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_GETOPT_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_GETOPT_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include <string>
#include <vector>

// sizeof = 72
struct ZEN_Get_Option : public ZEN_NON_Copyable {
    enum OPTION_ARG_MODE {
        NO_ARG = 0,
        ARG_REQUIRED = 1,
        ARG_OPTIONAL = 2,
    };
    // sizeof = 12
    struct ZEN_GETOPT_LONG_OPTION {
    public:
        std::string name_;
        int has_arg_;
        int val_;
        ZEN_GETOPT_LONG_OPTION();
        ZEN_GETOPT_LONG_OPTION(const char *name, int has_arg, int val);
        ~ZEN_GETOPT_LONG_OPTION();
    };
public:
    int argc_;
    char **argv_;
    int optind;
    int opterr;
    char *optarg;
private:
    std::string optstring_;
    int long_only_;
    int has_colon_;
    std::string last_option_;
    char *nextchar_;
    int optopt_;
    int ordering_;
    int nonopt_start_;
    int nonopt_end_;
    ZEN_GETOPT_LONG_OPTION *long_option_;
    std::vector<ZEN_GETOPT_LONG_OPTION> long_opts_;
public:
    ZEN_Get_Option(int argc, char **argv, const char *optstring,
                   int optind_arg, int opterr_arg, int ordering, int long_only);
    ~ZEN_Get_Option();
    int operator()();
    char *opt_arg() const { return optarg; }
    int opt_opt() { return optopt_; }
    int &opt_ind() { return optind; }
    int long_option(const char *name, OPTION_ARG_MODE has_arg);
    int long_option(const char *name, int short_option, OPTION_ARG_MODE has_arg);
    const char *long_option() const;
    int argc() const { return argc_; }
    char **argv() const { return argv_; }
private:
    int nextchar_i();
    int long_option_i();
    int short_option_i();
    void permute_args();
    int permute();
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_GETOPT_H_H_
