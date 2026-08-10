// Restored from secagent binary (reverse-engineered, behavior matched).
// Original path: src/commlib/zenlib/zen_digit_list.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DIGIT_LIST_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DIGIT_LIST_H_H_

#include <set>

// sizeof = 24
struct DigitList {
    enum EDigitType {
        EDT_10 = 0,
        EDT_16 = 1,
    };
public:
    DigitList();
    ~DigitList();
    int load(const char *conf_file_path, DigitList::EDigitType type);
    bool is_exist(unsigned int value) const;
    const std::set<unsigned int> &get_list();
private:
    unsigned int str_to_digit(const char *str, DigitList::EDigitType type);
    std::set<unsigned int> values_;
    DigitList(const DigitList &);
    DigitList &operator=(const DigitList &);
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DIGIT_LIST_H_H_
