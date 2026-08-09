// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_config_getopt.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_GETOPT_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_GETOPT_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include <bits/stl_vector.h>
#include <bits/stringfwd.h>

// sizeof = 72
struct ZEN_Get_Option : public ZEN_NON_Copyable { // line 7
    enum OPTION_ARG_MODE {
        NO_ARG = 0,
        ARG_REQUIRED = 1,
        ARG_OPTIONAL = 2,
    };
    // sizeof = 12
    struct ZEN_GETOPT_LONG_OPTION {
    public:
    string name_;
    int has_arg_;
    int val_;
    ZEN_GETOPT_LONG_OPTION(); // line 322
    ZEN_GETOPT_LONG_OPTION(const char *arg0, int arg1, int arg2); // line 327
    ~ZEN_GETOPT_LONG_OPTION(); // line 330
    bool operator<(const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg0); // line 332
    };
public:
int argc_;
char **argv_;
int optind;
int opterr;
char *optarg;
private:
string optstring_;
int long_only_;
int has_colon_;
string last_option_;
char *nextchar_;
int optopt_;
int ordering_;
int nonopt_start_;
int nonopt_end_;
ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION *long_option_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg1); // line 298
    __normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > begin(); // line 332
    __normal_iterator<const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > begin() const; // line 341
    __normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > end(); // line 350
    __normal_iterator<const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*, std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*, std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*, std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*, std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & operator[](unsigned int arg0); // line 479
    const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & at(unsigned int arg0); // line 519
    const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & at(unsigned int arg0) const; // line 537
    ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & front(); // line 548
    const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & front() const; // line 556
    ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & back(); // line 564
    const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & back() const; // line 572
    ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION * data(); // line 583
    const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION * data() const; // line 587
    void push_back(const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > insert(__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg0, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg1); // line 93
    void insert(__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg0, unsigned int arg1, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg2); // line 657
    __normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > erase(__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg0); // line 110
    __normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > erase(__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg0, __normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg1); // line 171
    void _M_fill_insert(__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg0, unsigned int arg1, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg2); // line 311
    void _M_insert_aux(__normal_iterator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION*,std::vector<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION, std::allocator<ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION> > > arg0, const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg1); // line 249
} long_opts_;
void ZEN_Get_Option_Init(const char *arg0); // line 136
public:
ZEN_Get_Option(int arg0, char **arg1, const char *arg2, int arg3, int arg4, int arg5, int arg6); // line 144
~ZEN_Get_Option(); // line 147
int operator()(); // line 185
char * opt_arg() const; // line 192
int opt_opt(); // line 200
int & opt_ind(); // line 214
int long_option(const char *arg0, ZEN_Get_Option::OPTION_ARG_MODE arg1); // line 228
int long_option(const char *arg0, int arg1, ZEN_Get_Option::OPTION_ARG_MODE arg2); // line 246
const char * long_option() const; // line 250
int argc() const; // line 253
char ** argv() const; // line 256
private:
int nextchar_i(); // line 350
int long_option_i(); // line 353
int short_option_i(); // line 356
void permute_args(); // line 361
int permute(); // line 364
};
namespace ZEN_Get_Option {
    enum OPTION_ARG_MODE {
        NO_ARG = 0,
        ARG_REQUIRED = 1,
        ARG_OPTIONAL = 2,
    };
    // sizeof = 12
    struct ZEN_GETOPT_LONG_OPTION { // line 320
    public:
    string name_;
    int has_arg_;
    int val_;
    ZEN_GETOPT_LONG_OPTION(); // line 322
    ZEN_GETOPT_LONG_OPTION(const char *arg0, int arg1, int arg2); // line 327
    ~ZEN_GETOPT_LONG_OPTION(); // line 330
    bool operator<(const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg0); // line 332
    ZEN_GETOPT_LONG_OPTION(const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg0);
    ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION & operator=(const ZEN_Get_Option::ZEN_GETOPT_LONG_OPTION &arg0);
    };
} // namespace ZEN_Get_Option

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_GETOPT_H_H_
