#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "AnalysisSwitch.h"
#include "stdlibs.h"
#include <openssl/evp.h>

// typedef rsid_t promise_id_t;  // hexadecimal
typedef long int call_id_t;
typedef std::string function_id_t;

typedef int env_id_t;
typedef int var_id_t;

typedef long long int timestamp_t;
typedef long int promise_id_t;

extern const timestamp_t UNDEFINED_TIMESTAMP;

extern const promise_id_t UNASSIGNED_DENOTED_VALUE_ID;

extern const char UNIT_SEPARATOR;
extern const char RECORD_SEPARATOR;

struct eval_depth_t {
    int call_depth;
    int promise_depth;
    int nested_promise_depth;
};

extern const eval_depth_t ESCAPED_PROMISE_EVAL_DEPTH;
extern const eval_depth_t UNASSIGNED_PROMISE_EVAL_DEPTH;

extern const size_t PROMISE_MAPPING_BUCKET_COUNT;
extern const size_t FUNCTION_MAPPING_BUCKET_SIZE;


#define failwith(format, ...)                                                  \
    failwith_impl(__FILE__, __LINE__, format, __VA_ARGS__)

#define failwith_impl(file, line, format, ...)                                 \
    do {                                                                       \
        fprintf(stderr, "ERROR [%s:%d] " format, file, line, __VA_ARGS__);     \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

int get_file_size(std::ifstream &file);

std::string readfile(std::ifstream &file);

bool file_exists(const std::string &filepath);

char *copy_string(char *destination, const char *source, size_t buffer_size);

bool sexp_to_bool(SEXP value);

int sexp_to_int(SEXP value);

std::string sexp_to_string(SEXP value);

template <typename T>
typename std::underlying_type<T>::type to_underlying_type(const T &enum_val) {
    return static_cast<typename std::underlying_type<T>::type>(enum_val);
}

std::string compute_hash(const char *data);

const char* get_name(SEXP call);
std::string get_definition_location_cpp(SEXP op);
std::string get_callsite_cpp(int);

// char *to_string(SEXP var);
std::string get_expression(SEXP e);
std::string escape(const std::string &s);
const char *remove_null(const char *value);
std::string clock_ticks_to_string(clock_t ticks);
AnalysisSwitch to_analysis_switch(SEXP env);
std::string to_string(const char *str);

inline std::string check_string(const char *s) {
    return s == NULL ? "<unknown>" : s;
}

inline void *malloc_or_die(std::size_t size) {
    void *data = std::malloc(size);
    if (data == nullptr) {
        failwith("memory allocation error: unable to allocate %lu bytes.\n",
                 size);
    }
    return data;
}

inline void *calloc_or_die(std::size_t num, std::size_t size) {
    void *data = std::calloc(num, size);
    if (data == nullptr) {
        failwith("memory allocation error: unable to allocate %lu bytes.\n",
                 size);
    }
    return data;
}

inline void *realloc_or_die(void * ptr, std::size_t size) {
    void *data = std::realloc(ptr, size);
    if (data == nullptr) {
        failwith("memory allocation error: unable to reallocate %lu bytes.\n",
                 size);
    }
    return data;
}

inline bool timestamp_is_undefined(const timestamp_t timestamp) {
    return timestamp == UNDEFINED_TIMESTAMP;
}

#endif /* __UTILITIES_H__ */
