// SPDX-License-Identifier: BSD-2-Clause
/**
* Digital Voice Modem - Common Library
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Common Library
* @derivedfrom picojson (https://github.com/kazuho/picojson)
* @license BSD-2-Clause License (https://opensource.org/licenses/BSD-2-Clause)
*
*   Copyright 2009-2010 Cybozu Labs, Inc.
*   Copyright 2011-2014 Kazuho Oku., All rights reserved.
*   Copyright (C) 2023,2024 Bryan Biedenkapp, N2PLL
*
*/
#if !defined(__JSON_H__)
#define __JSON_H__

#include "common/Defines.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include <inttypes.h>

// for isnan/isinf
#if __cplusplus >= 201103L
#include <cmath>
#else
extern "C" {
#ifdef _MSC_VER
#include <float.h>
#elif defined(__INTEL_COMPILER)
#include <mathimf.h>
#else
#include <math.h>
#endif
}
#endif

#ifndef PICOJSON_USE_RVALUE_REFERENCE
#if (defined(__cpp_rvalue_references) && __cpp_rvalue_references >= 200610) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#define PICOJSON_USE_RVALUE_REFERENCE 1
#else
#define PICOJSON_USE_RVALUE_REFERENCE 0
#endif
#endif // PICOJSON_USE_RVALUE_REFERENCE

#ifndef PICOJSON_NOEXCEPT
#if PICOJSON_USE_RVALUE_REFERENCE
#define PICOJSON_NOEXCEPT noexcept
#else
#define PICOJSON_NOEXCEPT throw()
#endif
#endif

// to disable the use of localeconv(3), set PICOJSON_USE_LOCALE to 0
#ifndef PICOJSON_USE_LOCALE
#define PICOJSON_USE_LOCALE 1
#endif
#if PICOJSON_USE_LOCALE
extern "C" {
#include <locale.h>
}
#endif

#ifndef PICOJSON_ASSERT
#define PICOJSON_ASSERT(e)                                                          \
  do {                                                                              \
    if (!(e))                                                                       \
      throw std::runtime_error(#e);                                                 \
  } while (0)
#endif

#ifdef _MSC_VER
#define SNPRINTF _snprintf_s
#pragma warning(push)
#pragma warning(disable : 4244) // conversion from int to char
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4702) // unreachable code
#pragma warning(disable : 4706) // assignment within conditional expression
#else
#define SNPRINTF snprintf
#endif

namespace json
{
    enum {
        null_type,
        boolean_type,
        number_type,
        int32_type,
        uint64_type,
        uint32_type,
        uint16_type,
        uint8_type,
        float_type,
        string_type,
        array_type,
        object_type
    };

    enum { INDENT_WIDTH = 2, DEFAULT_MAX_DEPTHS = 100 };

    struct null {};

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //
    // ---------------------------------------------------------------------------

    class value {
    public:
        typedef std::vector<value> array;
        typedef std::map<std::string, value> object;
        union _storage {
            bool boolean_;
            double number_;
            int int32_;
            uint64_t uint64_;
            uint32_t uint32_;
            uint16_t uint16_;
            uint8_t uint8_;
            float float_;
            std::string *string_;
            array *array_;
            object *object_;
        };

    protected:
        int type_;
        _storage u_;

    public:
        value();
        value(int type, bool);
        explicit value(bool b);
        explicit value(double n);
        explicit value(const std::string &s);
        explicit value(const array &a);
        explicit value(const object &o);
#if PICOJSON_USE_RVALUE_REFERENCE
        explicit value(std::string &&s);
        explicit value(array &&a);
        explicit value(object &&o);
#endif
        explicit value(const char *s);
        value(const char *s, size_t len);
        ~value();
        value(const value &x);
        value &operator=(const value &x);
#if PICOJSON_USE_RVALUE_REFERENCE
        value(value &&x) PICOJSON_NOEXCEPT;
        value &operator=(value &&x) PICOJSON_NOEXCEPT;
#endif

        void swap(value &x) PICOJSON_NOEXCEPT;

        template <typename T> bool is() const;

        template <typename T> const T &get() const;
        template <typename T> T &get();

        template <typename T> void set(const T &);
#if PICOJSON_USE_RVALUE_REFERENCE
        template <typename T> void set(T &&);
#endif

        bool evaluate_as_boolean() const;

        const value &get(const size_t idx) const;
        const value &get(const std::string &key) const;
        value &get(const size_t idx);
        value &get(const std::string &key);

        bool contains(const size_t idx) const;
        bool contains(const std::string &key) const;

        std::string to_str() const;
        std::string to_type() const;

        template <typename Iter> void serialize(Iter os, bool prettify = false) const;
        std::string serialize(bool prettify = false) const;

    private:
        template <typename T> value(const T *); // intentionally defined to block implicit conversion of pointer to bool
        template <typename Iter> static void _indent(Iter os, int indent);
        template <typename Iter> void _serialize(Iter os, int indent) const;
        std::string _serialize(int indent) const;
        void clear();
    };

    typedef value::array array;
    typedef value::object object;

    inline value::value() : type_(null_type), u_() {
        /* stub */
    }

    inline value::value(int type, bool) : type_(type), u_() {
    switch (type) {
        #define INIT(p, v)                                                          \
        case p##type:                                                               \
            u_.p = v;                                                               \
            break

        INIT(boolean_, false);
        INIT(number_, 0.0);
        INIT(int32_, 0);
        INIT(uint64_, 0);
        INIT(uint32_, 0);
        INIT(uint16_, 0);
        INIT(uint8_, 0);
        INIT(float_, 0.0f);
        INIT(string_, new std::string());
        INIT(array_, new array());
        INIT(object_, new object());
        #undef INIT
        default:
            break;
        }
    }

    inline value::value(bool b) : type_(boolean_type), u_() {
        u_.boolean_ = b;
    }

    inline value::value(double n) : type_(number_type), u_() {
        if (
#ifdef _MSC_VER
            !_finite(n)
#elif __cplusplus >= 201103L
            std::isnan(n) || std::isinf(n)
#else
            isnan(n) || isinf(n)
#endif
        ) {
            throw std::overflow_error("");
        }
        u_.number_ = n;
    }

    inline value::value(const std::string &s) : type_(string_type), u_() {
        u_.string_ = new std::string(s);
    }

    inline value::value(const array &a) : type_(array_type), u_() {
        u_.array_ = new array(a);
    }

    inline value::value(const object &o) : type_(object_type), u_() {
        u_.object_ = new object(o);
    }

#if PICOJSON_USE_RVALUE_REFERENCE
    inline value::value(std::string &&s) : type_(string_type), u_() {
        u_.string_ = new std::string(std::move(s));
    }

    inline value::value(array &&a) : type_(array_type), u_() {
        u_.array_ = new array(std::move(a));
    }

    inline value::value(object &&o) : type_(object_type), u_() {
        u_.object_ = new object(std::move(o));
    }
#endif

    inline value::value(const char *s) : type_(string_type), u_() {
        u_.string_ = new std::string(s);
    }

    inline value::value(const char *s, size_t len) : type_(string_type), u_() {
        u_.string_ = new std::string(s, len);
    }

    inline void value::clear() {
        switch (type_) {
        #define DEINIT(p)                                                           \
            case p##type:                                                           \
                delete u_.p;                                                        \
                break

        DEINIT(string_);
        DEINIT(array_);
        DEINIT(object_);
        #undef DEINIT
        default:
            break;
        }
    }

    inline value::~value() {
        clear();
    }

    inline value::value(const value &x) : type_(x.type_), u_() {
        switch (type_) {
        #define INIT(p, v)                                                          \
            case p##type:                                                           \
                u_.p = v;                                                           \
                break

        INIT(string_, new std::string(*x.u_.string_));
        INIT(array_, new array(*x.u_.array_));
        INIT(object_, new object(*x.u_.object_));
        #undef INIT
        default:
            u_ = x.u_;
            break;
        }
    }

    inline value &value::operator=(const value &x) {
        if (this != &x) {
            value t(x);
            swap(t);
        }

        return *this;
    }

#if PICOJSON_USE_RVALUE_REFERENCE
    inline value::value(value &&x) PICOJSON_NOEXCEPT : type_(null_type), u_() {
        swap(x);
    }

    inline value &value::operator=(value &&x) PICOJSON_NOEXCEPT {
        swap(x);
        return *this;
    }
#endif
    inline void value::swap(value &x) PICOJSON_NOEXCEPT {
        std::swap(type_, x.type_);
        std::swap(u_, x.u_);
    }

    #define IS(ctype, jtype)                                                        \
        template <> inline bool value::is<ctype>() const {                          \
            return type_ == jtype##_type;                                           \
        }

    #define IS_NUMBER(ctype, jtype)                                                 \
        template <> inline bool value::is<ctype>() const {                          \
            return type_ == jtype##_type || type_ == number_type;                   \
        }

    IS(null, null)
    IS(bool, boolean)
    IS_NUMBER(int, int32)
    IS_NUMBER(uint64_t, uint64)
    IS_NUMBER(uint32_t, uint32)
    IS_NUMBER(uint16_t, uint16)
    IS_NUMBER(uint8_t, uint8)
    IS_NUMBER(float, float)
    IS(std::string, string)
    IS(array, array)
    IS(object, object)
    #undef IS
    #undef IS_NUMBER

    template <> inline bool value::is<double>() const {
        return type_ == number_type;
    }

    #define GET(ctype, var)                                                         \
        template <> inline const ctype &value::get<ctype>() const {                 \
            PICOJSON_ASSERT("type mismatch! call is<type>() before get<type>()" && is<ctype>()); \
            return var;                                                             \
        }                                                                           \
        template <> inline ctype &value::get<ctype>() {                             \
            PICOJSON_ASSERT("type mismatch! call is<type>() before get<type>()" && is<ctype>()); \
            return var;                                                             \
        }

    GET(bool, u_.boolean_)
    GET(std::string, *u_.string_)
    GET(array, *u_.array_)
    GET(object, *u_.object_)
    GET(double, u_.number_)
    GET(int,
        (type_ == number_type && (const_cast<value *>(this)->type_ = int32_type, (const_cast<value *>(this)->u_.int32_ = u_.number_)),
            u_.int32_))
    GET(uint64_t,
        (type_ == number_type && (const_cast<value *>(this)->type_ = uint64_type, (const_cast<value *>(this)->u_.uint64_ = u_.number_)),
            u_.uint64_))
    GET(uint32_t,
        (type_ == number_type && (const_cast<value *>(this)->type_ = uint32_type, (const_cast<value *>(this)->u_.uint32_ = u_.number_)),
            u_.uint32_))
    GET(uint16_t,
        (type_ == number_type && (const_cast<value *>(this)->type_ = uint16_type, (const_cast<value *>(this)->u_.uint16_ = u_.number_)),
            u_.uint16_))
    GET(uint8_t,
        (type_ == number_type && (const_cast<value *>(this)->type_ = uint8_type, (const_cast<value *>(this)->u_.uint8_ = u_.number_)),
            u_.uint8_))
    GET(float,
        (type_ == number_type && (const_cast<value *>(this)->type_ = float_type, (const_cast<value *>(this)->u_.float_ = u_.number_)),
            u_.float_))
    #undef GET

    #define SET(ctype, jtype, setter)                                               \
        template <> inline void value::set<ctype>(const ctype &_val) {              \
            clear();                                                                \
            type_ = jtype##_type;                                                   \
            setter                                                                  \
        }

    SET(bool, boolean, u_.boolean_ = _val;)
    SET(std::string, string, u_.string_ = new std::string(_val);)
    SET(array, array, u_.array_ = new array(_val);)
    SET(object, object, u_.object_ = new object(_val);)
    SET(double, number, u_.number_ = _val;)
    SET(int8_t, int32, u_.int32_ = _val;)
    SET(int, int32, u_.int32_ = _val;)
    SET(uint64_t, uint64, u_.uint64_ = _val;)
    SET(uint32_t, uint32, u_.uint32_ = _val;)
    SET(uint16_t, uint16, u_.uint16_ = _val;)
    SET(uint8_t, uint8, u_.uint8_ = _val;)
    SET(float, float, u_.float_ = _val;)
    #undef SET

#if PICOJSON_USE_RVALUE_REFERENCE
    #define MOVESET(ctype, jtype, setter)                                           \
        template <> inline void value::set<ctype>(ctype && _val) {                  \
            clear();                                                                \
            type_ = jtype##_type;                                                   \
            setter                                                                  \
        }

    MOVESET(std::string, string, u_.string_ = new std::string(std::move(_val));)
    MOVESET(array, array, u_.array_ = new array(std::move(_val));)
    MOVESET(object, object, u_.object_ = new object(std::move(_val));)
    #undef MOVESET
#endif

    inline bool value::evaluate_as_boolean() const {
        switch (type_) {
        case null_type:
            return false;
        case boolean_type:
            return u_.boolean_;
        case number_type:
            return u_.number_ != 0;
        case int32_type:
            return u_.int32_ != 0;
        case uint64_type:
            return u_.uint64_ != 0;
        case uint32_type:
            return u_.uint32_ != 0;
        case uint16_type:
            return u_.uint16_ != 0;
        case uint8_type:
            return u_.uint8_ != 0;
        case float_type:
            return u_.float_ != 0;
        case string_type:
            return !u_.string_->empty();
        default:
            return true;
        }
    }

    inline const value &value::get(const size_t idx) const {
        static value s_null;
        PICOJSON_ASSERT(is<array>());
        return idx < u_.array_->size() ? (*u_.array_)[idx] : s_null;
    }

    inline value &value::get(const size_t idx) {
        static value s_null;
        PICOJSON_ASSERT(is<array>());
        return idx < u_.array_->size() ? (*u_.array_)[idx] : s_null;
    }

    inline const value &value::get(const std::string &key) const {
        static value s_null;
        PICOJSON_ASSERT(is<object>());
        object::const_iterator i = u_.object_->find(key);
        return i != u_.object_->end() ? i->second : s_null;
    }

    inline value &value::get(const std::string &key) {
        static value s_null;
        PICOJSON_ASSERT(is<object>());
        object::iterator i = u_.object_->find(key);
        return i != u_.object_->end() ? i->second : s_null;
    }

    inline bool value::contains(const size_t idx) const {
        PICOJSON_ASSERT(is<array>());
        return idx < u_.array_->size();
    }

    inline bool value::contains(const std::string &key) const {
        PICOJSON_ASSERT(is<object>());
        object::const_iterator i = u_.object_->find(key);
        return i != u_.object_->end();
    }

    inline std::string value::to_str() const {
        switch (type_) {
            case null_type:
                return "null";
            case boolean_type:
                return u_.boolean_ ? "true" : "false";
            case number_type: {
                char buf[256];
                double tmp;
                SNPRINTF(buf, sizeof(buf), fabs(u_.number_) < (1ULL << 53) && modf(u_.number_, &tmp) == 0 ? "%.f" : "%.17g", u_.number_);
#if PICOJSON_USE_LOCALE
                char *decimal_point = localeconv()->decimal_point;
                if (strcmp(decimal_point, ".") != 0) {
                    size_t decimal_point_len = strlen(decimal_point);
                    for (char *p = buf; *p != '\0'; ++p) {
                        if (strncmp(p, decimal_point, decimal_point_len) == 0) {
                            return std::string(buf, p) + "." + (p + decimal_point_len);
                        }
                    }
                }
#endif
                return buf;
            }
            case int32_type: {
                char buf[256];
                SNPRINTF(buf, sizeof(buf), "%d", u_.int32_);
                return buf;
            }
            case uint64_type: {
                char buf[256];
                SNPRINTF(buf, sizeof(buf), "%" PRId64, u_.uint64_);
                return buf;
            }
            case uint32_type: {
                char buf[256];
                SNPRINTF(buf, sizeof(buf), "%u", u_.uint32_);
                return buf;
            }
            case uint16_type: {
                char buf[256];
                SNPRINTF(buf, sizeof(buf), "%u", u_.uint16_);
                return buf;
            }
            case uint8_type: {
                char buf[256];
                SNPRINTF(buf, sizeof(buf), "%u", u_.uint8_);
                return buf;
            }
            case float_type: {
                char buf[256];
                SNPRINTF(buf, sizeof(buf), "%f", u_.float_);
                return buf;
            }
            case string_type:
                return *u_.string_;
            case array_type:
                return "array";
            case object_type:
                return "object";
            default:
                PICOJSON_ASSERT(0);
#ifdef _MSC_VER
                __assume(0);
#endif
        }

        return std::string();
    }

    inline std::string value::to_type() const {
        switch (type_) {
            case null_type:
                return "null";
            case boolean_type:
                return "boolean";
            case number_type:
                return "number";
            case int32_type:
                return "int32";
            case uint64_type:
                return "uint64";
            case uint32_type:
                return "uint32";
            case uint16_type:
                return "uint16";
            case uint8_type:
                return "uint8";
            case float_type:
                return "float";
            case string_type:
                return "string";
            case array_type:
                return "array";
            case object_type:
                return "object";
            default:
                PICOJSON_ASSERT(0);
#ifdef _MSC_VER
                __assume(0);
#endif
        }

        return std::string();
    }

    template <typename Iter> void copy(const std::string &s, Iter oi) {
        std::copy(s.begin(), s.end(), oi);
    }

    template <typename Iter> struct serialize_str_char {
        Iter oi;
        void operator()(char c) {
            switch (c) {
            #define MAP(val, sym)                                                   \
                case val:                                                           \
                    copy(sym, oi);                                                  \
                    break

            MAP('"', "\\\"");
            MAP('\\', "\\\\");
            MAP('/', "\\/");
            MAP('\b', "\\b");
            MAP('\f', "\\f");
            MAP('\n', "\\n");
            MAP('\r', "\\r");
            MAP('\t', "\\t");
            #undef MAP
            default:
                if (static_cast<unsigned char>(c) < 0x20 || c == 0x7f) {
                    char buf[7];
                    SNPRINTF(buf, sizeof(buf), "\\u%04x", c & 0xff);
                    copy(buf, buf + 6, oi);
                } else {
                    *oi++ = c;
                }
                break;
            }
        }
    };

    template <typename Iter> void serialize_str(const std::string &s, Iter oi) {
        *oi++ = '"';
        serialize_str_char<Iter> process_char = {oi};
        std::for_each(s.begin(), s.end(), process_char);
        *oi++ = '"';
    }

    template <typename Iter> void value::serialize(Iter oi, bool prettify) const {
        return _serialize(oi, prettify ? 0 : -1);
    }

    inline std::string value::serialize(bool prettify) const {
        return _serialize(prettify ? 0 : -1);
    }

    template <typename Iter> void value::_indent(Iter oi, int indent) {
        *oi++ = '\n';
        for (int i = 0; i < indent * INDENT_WIDTH; ++i) {
            *oi++ = ' ';
        }
    }

    template <typename Iter> void value::_serialize(Iter oi, int indent) const {
        switch (type_) {
        case string_type:
            serialize_str(*u_.string_, oi);
            break;
        case array_type:
            {
                *oi++ = '[';
                if (indent != -1) {
                    ++indent;
                }

                for (array::const_iterator i = u_.array_->begin(); i != u_.array_->end(); ++i) {
                    if (i != u_.array_->begin()) {
                        *oi++ = ',';
                    }

                    if (indent != -1) {
                        _indent(oi, indent);
                    }

                    i->_serialize(oi, indent);
                }

                if (indent != -1) {
                    --indent;

                    if (!u_.array_->empty()) {
                        _indent(oi, indent);
                    }
                }
                *oi++ = ']';
                break;
            }
        case object_type:
            {
                *oi++ = '{';
                if (indent != -1) {
                    ++indent;
                }

                for (object::const_iterator i = u_.object_->begin(); i != u_.object_->end(); ++i) {
                    if (i != u_.object_->begin()) {
                        *oi++ = ',';
                    }

                    if (indent != -1) {
                        _indent(oi, indent);
                    }

                    serialize_str(i->first, oi);
                    *oi++ = ':';

                    if (indent != -1) {
                        *oi++ = ' ';
                    }

                    i->second._serialize(oi, indent);
                }

                if (indent != -1) {
                    --indent;

                    if (!u_.object_->empty()) {
                        _indent(oi, indent);
                    }
                }
                *oi++ = '}';
                break;
            }
        default:
            copy(to_str(), oi);
            break;
        }

        if (indent == 0) {
            *oi++ = '\n';
        }
    }

    inline std::string value::_serialize(int indent) const {
        std::string s;
        _serialize(std::back_inserter(s), indent);
        return s;
    }

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //
    // ---------------------------------------------------------------------------

    template <typename Iter> class input {
    protected:
        Iter cur_, end_;
        bool consumed_;
        int line_;

    public:
        input(const Iter &first, const Iter &last) : cur_(first), end_(last), consumed_(false), line_(1) { /* stub */ }
        int getc() {
            if (consumed_) {
                if (*cur_ == '\n') {
                    ++line_;
                }
                ++cur_;
            }

            if (cur_ == end_) {
                consumed_ = false;
                return -1;
            }

            consumed_ = true;
            return *cur_ & 0xff;
        }

        void ungetc() {
            consumed_ = false;
        }

        Iter cur() const {
            if (consumed_) {
                input<Iter> *self = const_cast<input<Iter> *>(this);
                self->consumed_ = false;
                ++self->cur_;
            }

            return cur_;
        }

        int line() const {
            return line_;
        }

        void skip_ws() {
            while (1) {
                int ch = getc();
                if (!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) {
                    ungetc();
                    break;
                }
            }
        }

        bool expect(const int expected) {
            skip_ws();
            if (getc() != expected) {
                ungetc();
                return false;
            }
            return true;
        }

        bool match(const std::string &pattern) {
            for (std::string::const_iterator pi(pattern.begin()); pi != pattern.end(); ++pi) {
                if (getc() != *pi) {
                    ungetc();
                    return false;
                }
            }
            return true;
        }
    };

    template <typename Iter> inline int _parse_quadhex(input<Iter> &in) {
        int uni_ch = 0, hex;
        for (int i = 0; i < 4; i++) {
            if ((hex = in.getc()) == -1) {
                return -1;
            }

            if ('0' <= hex && hex <= '9') {
                hex -= '0';
            } else if ('A' <= hex && hex <= 'F') {
                hex -= 'A' - 0xa;
            } else if ('a' <= hex && hex <= 'f') {
                hex -= 'a' - 0xa;
            } else {
                in.ungetc();
                return -1;
            }

            uni_ch = uni_ch * 16 + hex;
        }
        return uni_ch;
    }

    template <typename String, typename Iter> inline bool _parse_codepoint(String &out, input<Iter> &in) {
        int uni_ch;
        if ((uni_ch = _parse_quadhex(in)) == -1) {
            return false;
        }

        if (0xd800 <= uni_ch && uni_ch <= 0xdfff) {
            if (0xdc00 <= uni_ch) {
                // a second 16-bit of a surrogate pair appeared
                return false;
            }

            // first 16-bit of surrogate pair, get the next one
            if (in.getc() != '\\' || in.getc() != 'u') {
                in.ungetc();
                return false;
            }

            int second = _parse_quadhex(in);
            if (!(0xdc00 <= second && second <= 0xdfff)) {
                return false;
            }

            uni_ch = ((uni_ch - 0xd800) << 10) | ((second - 0xdc00) & 0x3ff);
            uni_ch += 0x10000;
        }

        if (uni_ch < 0x80) {
            out.push_back(static_cast<char>(uni_ch));
        } else {
            if (uni_ch < 0x800) {
                out.push_back(static_cast<char>(0xc0 | (uni_ch >> 6)));
            } else {
                if (uni_ch < 0x10000) {
                    out.push_back(static_cast<char>(0xe0 | (uni_ch >> 12)));
                } else {
                    out.push_back(static_cast<char>(0xf0 | (uni_ch >> 18)));
                    out.push_back(static_cast<char>(0x80 | ((uni_ch >> 12) & 0x3f)));
                }
                out.push_back(static_cast<char>(0x80 | ((uni_ch >> 6) & 0x3f)));
            }

            out.push_back(static_cast<char>(0x80 | (uni_ch & 0x3f)));
        }

        return true;
    }

    template <typename String, typename Iter> inline bool _parse_string(String &out, input<Iter> &in) {
        while (1) {
            int ch = in.getc();
            if (ch < ' ') {
                in.ungetc();
                return false;
            } else if (ch == '"') {
                return true;
            } else if (ch == '\\') {
                if ((ch = in.getc()) == -1) {
                    return false;
                }

                switch (ch) {
                #define MAP(sym, val)                                               \
                    case sym:                                                       \
                        out.push_back(val);                                         \
                        break

                MAP('"', '\"');
                MAP('\\', '\\');
                MAP('/', '/');
                MAP('b', '\b');
                MAP('f', '\f');
                MAP('n', '\n');
                MAP('r', '\r');
                MAP('t', '\t');
                #undef MAP
                case 'u':
                    if (!_parse_codepoint(out, in)) {
                        return false;
                    }
                    break;
                default:
                    return false;
                }
            } else {
                out.push_back(static_cast<char>(ch));
            }
        }
        return false;
    }

    template <typename Context, typename Iter> inline bool _parse_array(Context &ctx, input<Iter> &in) {
        if (!ctx.parse_array_start()) {
            return false;
        }

        size_t idx = 0;
        if (in.expect(']')) {
            return ctx.parse_array_stop(idx);
        }

        do {
            if (!ctx.parse_array_item(in, idx)) {
                return false;
            }
            idx++;
        } while (in.expect(','));

        return in.expect(']') && ctx.parse_array_stop(idx);
    }

    template <typename Context, typename Iter> inline bool _parse_object(Context &ctx, input<Iter> &in) {
        if (!ctx.parse_object_start()) {
            return false;
        }

        if (in.expect('}')) {
            return ctx.parse_object_stop();
        }

        do {
            std::string key;
            if (!in.expect('"') || !_parse_string(key, in) || !in.expect(':')) {
                return false;
            }

            if (!ctx.parse_object_item(in, key)) {
                return false;
            }
        } while (in.expect(','));

        return in.expect('}') && ctx.parse_object_stop();
    }

    template <typename Iter> inline std::string _parse_number(input<Iter> &in) {
        std::string num_str;
        while (1) {
            int ch = in.getc();
            if (('0' <= ch && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E') {
                num_str.push_back(static_cast<char>(ch));
            } else if (ch == '.') {
#if PICOJSON_USE_LOCALE
                num_str += localeconv()->decimal_point;
#else
                num_str.push_back('.');
#endif
            } else {
                in.ungetc();
                break;
            }
        }

        return num_str;
    }

    template <typename Context, typename Iter> inline bool _parse(Context &ctx, input<Iter> &in) {
        in.skip_ws();
        int ch = in.getc();
        switch (ch) {
        #define IS(ch, text, op)                                                    \
            case ch:                                                                \
                if (in.match(text) && op) {                                         \
                    return true;                                                    \
                } else {                                                            \
                    return false;                                                   \
                }

        IS('n', "ull", ctx.set_null());
        IS('f', "alse", ctx.set_bool(false));
        IS('t', "rue", ctx.set_bool(true));
        #undef IS
        case '"':
            return ctx.parse_string(in);
        case '[':
            return _parse_array(ctx, in);
        case '{':
            return _parse_object(ctx, in);
        default:
            if (('0' <= ch && ch <= '9') || ch == '-') {
                double f;
                char *endp;
                in.ungetc();
                std::string num_str(_parse_number(in));
                if (num_str.empty()) {
                    return false;
                }

                f = strtod(num_str.c_str(), &endp);
                if (endp == num_str.c_str() + num_str.size()) {
                    ctx.set_number(f);
                    return true;
                }
                return false;
            }
            break;
        }

        in.ungetc();
        return false;
    }

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //
    // ---------------------------------------------------------------------------

    class deny_parse_context {
    public:
        bool set_null() {
            return false;
        }

        bool set_bool(bool) {
            return false;
        }

        bool set_number(double) {
            return false;
        }

        template <typename Iter> bool parse_string(input<Iter> &) {
            return false;
        }

        bool parse_array_start() {
            return false;
        }

        template <typename Iter> bool parse_array_item(input<Iter> &, size_t) {
            return false;
        }

        bool parse_array_stop(size_t) {
            return false;
        }

        bool parse_object_start() {
            return false;
        }

        template <typename Iter> bool parse_object_item(input<Iter> &, const std::string &) {
            return false;
        }
    };

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //
    // ---------------------------------------------------------------------------

    class default_parse_context {
    protected:
        value *out_;
        size_t depths_;

    public:
        default_parse_context(value *out, size_t depths = DEFAULT_MAX_DEPTHS) : out_(out), depths_(depths) { /* stub */ }

        bool set_null() {
            *out_ = value();
            return true;
        }

        bool set_bool(bool b) {
            *out_ = value(b);
            return true;
        }

        bool set_number(double f) {
            *out_ = value(f);
            return true;
        }

        template <typename Iter> bool parse_string(input<Iter> &in) {
            *out_ = value(string_type, false);
            return _parse_string(out_->get<std::string>(), in);
        }

        bool parse_array_start() {
            if (depths_ == 0)
                return false;

            --depths_;
            *out_ = value(array_type, false);
            return true;
        }

        template <typename Iter> bool parse_array_item(input<Iter> &in, size_t) {
            array &a = out_->get<array>();
            a.push_back(value());
            default_parse_context ctx(&a.back(), depths_);
            return _parse(ctx, in);
        }

        bool parse_array_stop(size_t) {
            ++depths_;
            return true;
        }

        bool parse_object_start() {
            if (depths_ == 0)
                return false;

            *out_ = value(object_type, false);
            return true;
        }

        template <typename Iter> bool parse_object_item(input<Iter> &in, const std::string &key) {
            object &o = out_->get<object>();
            default_parse_context ctx(&o[key], depths_);
            return _parse(ctx, in);
        }

        bool parse_object_stop() {
            ++depths_;
            return true;
        }

    private:
        default_parse_context(const default_parse_context &);
        default_parse_context &operator=(const default_parse_context &);
    };

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //
    // ---------------------------------------------------------------------------

    class null_parse_context {
    protected:
        size_t depths_;

    public:
        struct dummy_str {
            void push_back(int) {
            }
        };

    public:
        null_parse_context(size_t depths = DEFAULT_MAX_DEPTHS) : depths_(depths) { /* stub */ }

        bool set_null() {
            return true;
        }

        bool set_bool(bool) {
            return true;
        }

        bool set_number(double) {
            return true;
        }

        template <typename Iter> bool parse_string(input<Iter> &in) {
            dummy_str s;
            return _parse_string(s, in);
        }

        bool parse_array_start() {
            if (depths_ == 0)
                return false;

            --depths_;
            return true;
        }

        template <typename Iter> bool parse_array_item(input<Iter> &in, size_t) {
            return _parse(*this, in);
        }

        bool parse_array_stop(size_t) {
            ++depths_;
            return true;
        }

        bool parse_object_start() {
            if (depths_ == 0)
                return false;

            --depths_;
            return true;
        }

        template <typename Iter> bool parse_object_item(input<Iter> &in, const std::string &) {
            ++depths_;
            return _parse(*this, in);
        }

        bool parse_object_stop() {
            return true;
        }

    private:
        null_parse_context(const null_parse_context &);
        null_parse_context &operator=(const null_parse_context &);
    };

    // obsolete, use the version below
    template <typename Iter> inline std::string parse(value &out, Iter &pos, const Iter &last) {
        std::string err;
        pos = parse(out, pos, last, &err);
        return err;
    }

    template <typename Context, typename Iter> inline Iter _parse(Context &ctx, const Iter &first, const Iter &last, std::string *err) {
        input<Iter> in(first, last);
        if (!_parse(ctx, in) && err != NULL) {
            char buf[64];
            SNPRINTF(buf, sizeof(buf), "syntax error at line %d near: ", in.line());
            *err = buf;
            while (1) {
                int ch = in.getc();
                if (ch == -1 || ch == '\n') {
                    break;
                } else if (ch >= ' ') {
                    err->push_back(static_cast<char>(ch));
                }
            }
        }

        return in.cur();
    }

    template <typename Iter> inline Iter parse(value &out, const Iter &first, const Iter &last, std::string *err) {
        default_parse_context ctx(&out);
        return _parse(ctx, first, last, err);
    }

    inline std::string parse(value &out, const std::string &s) {
        std::string err;
        parse(out, s.begin(), s.end(), &err);
        return err;
    }

    inline std::string parse(value &out, std::istream &is) {
        std::string err;
        parse(out, std::istreambuf_iterator<char>(is.rdbuf()), std::istreambuf_iterator<char>(), &err);
        return err;
    }

    template <typename T> struct last_error_t { static std::string s; };
    template <typename T> std::string last_error_t<T>::s;

    inline void set_last_error(const std::string &s) {
        last_error_t<bool>::s = s;
    }

    inline const std::string &get_last_error() {
        return last_error_t<bool>::s;
    }

    inline bool operator==(const value &x, const value &y) {
        if (x.is<null>())
            return y.is<null>();

        #define PICOJSON_CMP(type)                                                  \
            if (x.is<type>())                                                       \
                return y.is<type>() && x.get<type>() == y.get<type>()

        PICOJSON_CMP(bool);
        PICOJSON_CMP(double);
        PICOJSON_CMP(std::string);
        PICOJSON_CMP(array);
        PICOJSON_CMP(object);
        #undef PICOJSON_CMP
        PICOJSON_ASSERT(0);
#ifdef _MSC_VER
        __assume(0);
#endif
        return false;
    }

    inline bool operator!=(const value &x, const value &y) {
        return !(x == y);
    }
} // namespace json

#if !PICOJSON_USE_RVALUE_REFERENCE
namespace std {
    template <> inline void swap(json::value &x, json::value &y) {
        x.swap(y);
    }
}
#endif

inline std::istream &operator>>(std::istream &is, json::value &x) {
    json::set_last_error(std::string());
    const std::string err(json::parse(x, is));
    if (!err.empty()) {
        json::set_last_error(err);
        is.setstate(std::ios::failbit);
    }
    return is;
}

inline std::ostream &operator<<(std::ostream &os, const json::value &x) {
    x.serialize(std::ostream_iterator<char>(os));
    return os;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // __JSON_H__
