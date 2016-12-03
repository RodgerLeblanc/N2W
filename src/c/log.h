#include <pebble.h>

/*
  * log.h -- macros for logging and debuging.
  *
  * Examples:
  *
  *     char *s = "foo bar";
  *     logs(s); // will print `[main: Mem 5100/8828] s == "foo bar"`
  *
  *     int i = NULL;
  *     logi(i); // will print `[main: Mem 5100/8828] i == 0`
  *
  *     logb(i == NULL); // will print `[main: Mem 5100/8828] (i == NULL) == true`
  */

#ifndef LOG_H
#define LOG_H

// no value
#define log() \
    printf("[%s: Mem:%d/%d]", __func__, heap_bytes_used(),  heap_bytes_free())

// int
#define logi(value) \
    printf("[%s: Mem:%d/%d] " #value " == %i", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// int
#define logd(value) \
    printf("[%s: Mem:%d/%d] " #value " == %d", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// boolean
#define logb(value) \
    printf("[%s: Mem:%d/%d] (" #value ") == %s", __func__, heap_bytes_used(),  heap_bytes_free(), (value ? "true" : "false"))

// unsigned
#define logu(value) \
    printf("[%s: Mem:%d/%d] " #value " == %u", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// double, fixed-point notation
#define logf(value) \
    printf("[%s: Mem:%d/%d] " #value " == %f", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// double, standard form
#define loge(value) \
    printf("[%s: Mem:%d/%d] " #value " == %e", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// double, normal or exponential notation, whichever is more appropriate
#define logg(value) \
    printf("[%s: Mem:%d/%d] " #value " == %g", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// unsigned, hexadecimal
#define logx(value) \
    printf("[%s: Mem:%d/%d] " #value " == %x", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// unsigned, octal
#define logo(value) \
    printf("[%s: Mem:%d/%d] " #value " == %o", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// string
#define logs(value) \
    printf("[%s: Mem:%d/%d] " #value " == \"%s\"", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// char
#define logc(value) \
    printf("[%s: Mem:%d/%d] " #value " == \'%c\'", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// pointer
#define logp(value) \
    printf("[%s: Mem:%d/%d] " #value " == %p", __func__, heap_bytes_used(),  heap_bytes_free(), value)

// int and string tuple
#define log_string_tuple(key, value) \
    printf("[%s: Mem:%d/%d] key == %i, value == \"%s\"", __func__, heap_bytes_used(),  heap_bytes_free(), key, value)

// int and int tuple
#define log_int_tuple(key, value) \
    printf("[%s: Mem:%d/%d] key == %i, value == %i", __func__, heap_bytes_used(),  heap_bytes_free(), key, value)


#endif