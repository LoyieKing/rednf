// Auto-generated header stub from DWARF info
// Original path: import/include/opensource/rapidxml/rapidxml/rapidxml.hpp
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_IMPORT_INCLUDE_OPENSOURCE_RAPIDXML_RAPIDXML_RAPIDXML_HPP_H_
#define SECSVR_IMPORT_INCLUDE_OPENSOURCE_RAPIDXML_RAPIDXML_RAPIDXML_HPP_H_

#include <stddef.h>

// sizeof = 1
struct attribute_value_pred<'\''> { // line 1484
public:
static unsigned char test(char arg0); // line 1485
};
// sizeof = 1
struct attribute_value_pred<'\"'> { // line 1484
public:
static unsigned char test(char arg0); // line 1485
};
// sizeof = 1
struct attribute_value_pure_pred<'\''> { // line 1498
public:
static unsigned char test(char arg0); // line 1499
};
// sizeof = 1
struct attribute_value_pure_pred<'\"'> { // line 1498
public:
static unsigned char test(char arg0); // line 1499
};
extern const char *end; // line 326
extern unsigned char digit; // line 1657
extern const unsigned char lookup_whitespace[]; // line 294
extern const unsigned char lookup_node_name[]; // line 295
extern const unsigned char lookup_text[]; // line 296
extern const unsigned char lookup_text_pure_no_ws[]; // line 297
extern const unsigned char lookup_text_pure_with_ws[]; // line 298
extern const unsigned char lookup_attribute_name[]; // line 299
extern const unsigned char lookup_attribute_data_1[]; // line 300
extern const unsigned char lookup_attribute_data_1_pure[]; // line 301
extern const unsigned char lookup_attribute_data_2[]; // line 302
extern const unsigned char lookup_attribute_data_2_pure[]; // line 303
extern const unsigned char lookup_digits[]; // line 304
extern const unsigned char lookup_upcase[]; // line 305
extern const char *tmp; // line 312
extern char *value; // line 2276
extern char *result; // line 602
extern size_t alignment; // line 575
extern char *previous_begin; // line 529
extern xml_attribute<char> *attribute; // line 1031
extern xml_node<char> *sibling; // line 1011
extern xml_node<char> *child; // line 942
extern void *memory; // line 419
extern xml_node<char> *node; // line 420
extern size_t pool_size; // line 608
extern size_t alloc_size; // line 613
extern char *raw_memory; // line 614
extern char *pool; // line 617
extern /*anon struct*/ int *new_header; // line 618
extern char *name; // line 2243
extern char quote; // line 2270
extern const int AttFlags; // line 2277
extern char *src; // line 1585
extern char *dest; // line 1586
extern long unsigned int code; // line 1653
extern char *contents_start; // line 2173
extern char next_char; // line 2175
extern xml_node<char> *data; // line 1974
extern char ch; // line 1987
extern xml_node<char> *element; // line 2040
extern xml_node<char> *declaration; // line 1755
extern xml_node<char> *pi; // line 1886
extern xml_node<char> *comment; // line 1801
extern xml_node<char> *cdata; // line 2024
extern int depth; // line 1831
namespace rapidxml {
    enum node_type {
        node_document = 0,
        node_element = 1,
        node_data = 2,
        node_cdata = 3,
        node_comment = 4,
        node_declaration = 5,
        node_doctype = 6,
        node_pi = 7,
        node_document = 0,
        node_element = 1,
        node_data = 2,
        node_cdata = 3,
        node_comment = 4,
        node_declaration = 5,
        node_doctype = 6,
        node_pi = 7,
    };
    extern const int parse_no_data_nodes; // line 163
    extern const int parse_no_element_values; // line 172
    extern const int parse_no_string_terminators; // line 179
    extern const int parse_no_entity_translation; // line 186
    extern const int parse_no_utf8; // line 193
    extern const int parse_declaration_node; // line 200
    extern const int parse_comment_nodes; // line 207
    extern const int parse_doctype_node; // line 215
    extern const int parse_pi_nodes; // line 222
    extern const int parse_validate_closing_tags; // line 230
    extern const int parse_trim_whitespace; // line 238
    extern const int parse_normalize_whitespace; // line 247
    extern const int parse_default; // line 259
    extern const int parse_non_destructive; // line 269
    extern const int parse_fastest; // line 274
    extern const int parse_full; // line 280
    namespace internal {
        extern size_t measure<char>(const char *arg0); // line 310
        extern bool compare<char>(const char *arg0, unsigned int arg1, const char *arg2, unsigned int arg3, bool arg4); // line 320
    } // namespace internal
} // namespace rapidxml

#endif // SECSVR_IMPORT_INCLUDE_OPENSOURCE_RAPIDXML_RAPIDXML_RAPIDXML_HPP_H_
