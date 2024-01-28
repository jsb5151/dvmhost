// SPDX-License-Identifier: BSL-1.0
/**
* Digital Voice Modem - Common Library
* BSL-1.0 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Common Library
* @derivedfrom CRUD (https://github.com/venediktov/CRUD)
* @license BSL-1.0 License (https://opensource.org/license/bsl1-0-html)
*
*   Copyright (c) 2003-2013 Christopher M. Kohlhoff
*   Copyright (C) 2023 Bryan Biedenkapp, N2PLL
*
*/
#if !defined(__REST_HTTP__HTTP_LEXER_H__)
#define __REST_HTTP__HTTP_LEXER_H__

#include "common/Defines.h"

#include <tuple>
#include <vector>

namespace network
{
    namespace rest
    {
        namespace http
        {

            // ---------------------------------------------------------------------------
            //  Class Prototypes
            // ---------------------------------------------------------------------------

            struct HTTPPayload;

            // ---------------------------------------------------------------------------
            //  Class Declaration
            //      This class implements the lexer for incoming payloads.
            // ---------------------------------------------------------------------------

            class HTTPLexer {
            public:
                enum ResultType { GOOD, BAD, INDETERMINATE };

                /// <summary>Initializes a new instance of the HTTPLexer class.</summary>
                HTTPLexer(bool clientLexer);

                /// <summary>Reset to initial parser state.</summary>
                void reset();

                /// <summary>Parse some data. The enum return value is good when a complete request has
                /// been parsed, bad if the data is invalid, indeterminate when more data is
                /// required. The InputIterator return value indicates how much of the input
                /// has been consumed.</summary>
                template <typename InputIterator>
                std::tuple<ResultType, InputIterator> parse(HTTPPayload& payload, InputIterator begin, InputIterator end)
                {
                    while (begin != end) {
                        ResultType result = consume(payload, *begin++);
                        if (result == GOOD || result == BAD)
                            return std::make_tuple(result, begin);
                    }
                    return std::make_tuple(INDETERMINATE, begin);
                }

            private:
                /// <summary>Handle the next character of input.</summary>
                ResultType consume(HTTPPayload& payload, char input);

                /// <summary>Check if a byte is an HTTP character.</summary>
                static bool isChar(int c);
                /// <summary>Check if a byte is an HTTP control character.</summary>
                static bool isControl(int c);
                /// <summary>Check if a byte is an HTTP special character.</summary>
                static bool isSpecial(int c);
                /// <summary>Check if a byte is an digit.</summary>
                static bool isDigit(int c);

                struct LexedHeader
                {
                    std::string name;
                    std::string value;

                    LexedHeader() { /* stub */ }
                    LexedHeader(const std::string& name, const std::string& value) : name(name), value(value) {}
                };

                std::vector<LexedHeader> m_headers;
                uint16_t m_status;
                bool m_clientLexer = false;

                enum state
                {
                    METHOD_START,
                    METHOD,
                    URI,
                    HTTP_VERSION_H,
                    HTTP_VERSION_T_1,
                    HTTP_VERSION_T_2,
                    HTTP_VERSION_P,
                    HTTP_VERSION_SLASH,
                    HTTP_VERSION_MAJOR_START,
                    HTTP_VERSION_MAJOR,
                    HTTP_VERSION_MINOR_START,
                    HTTP_VERSION_MINOR,
                    HTTP_STATUS_1,
                    HTTP_STATUS_2,
                    HTTP_STATUS_3,
                    HTTP_STATUS_END,
                    HTTP_STATUS_MESSAGE_START,
                    HTTP_STATUS_MESSAGE,
                    EXPECTING_NEWLINE_1,
                    HEADER_LINE_START,
                    HEADER_LWS,
                    HEADER_NAME,
                    SPACE_BEFORE_HEADER_VALUE,
                    HEADER_VALUE,
                    EXPECTING_NEWLINE_2,
                    EXPECTING_NEWLINE_3
                } m_state;
            };
        } // namespace http
    } // namespace rest
} // namespace network

#endif // __REST_HTTP__HTTP_LEXER_H__
