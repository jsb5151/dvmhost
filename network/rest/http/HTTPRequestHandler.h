/**
* Digital Voice Modem - Host Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Host Software
*
*/
//
// Based on code from the CRUD project. (https://github.com/venediktov/CRUD)
// Licensed under the BPL-1.0 License (https://opensource.org/license/bsl1-0-html)
//
/*
*   Copyright (c) 2003-2013 Christopher M. Kohlhoff
*   Copyright (C) 2023 by Bryan Biedenkapp N2PLL
*
*   Permission is hereby granted, free of charge, to any person or organization 
*   obtaining a copy of the software and accompanying documentation covered by 
*   this license (the “Software”) to use, reproduce, display, distribute, execute, 
*   and transmit the Software, and to prepare derivative works of the Software, and
*   to permit third-parties to whom the Software is furnished to do so, all subject
*   to the following:
*
*   The copyright notices in the Software and this entire statement, including the
*   above license grant, this restriction and the following disclaimer, must be included
*   in all copies of the Software, in whole or in part, and all derivative works of the
*   Software, unless such copies or derivative works are solely in the form of
*   machine-executable object code generated by a source language processor.
*
*   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
*   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
*   PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE
*   DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
*   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
*   OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#if !defined(__REST_HTTP__HTTP_REQUEST_HANDLER_H__)
#define __REST_HTTP__HTTP_REQUEST_HANDLER_H__

#include "Defines.h"

#include <string>

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
            //      This class implements the common handler for all incoming requests.
            // ---------------------------------------------------------------------------

            class HTTPRequestHandler
            {
            public:
                /// <summary>Initializes a new instance of the HTTPRequestHandler class.</summary>
                explicit HTTPRequestHandler(const std::string& docRoot);
                /// <summary>Initializes a copy instance of the HTTPRequestHandler class.</summary>
                HTTPRequestHandler(const HTTPRequestHandler&) = delete;
                /// <summary></summary>
                HTTPRequestHandler(HTTPRequestHandler&&) = default;
                
                /// <summary></summary>
                HTTPRequestHandler& operator=(const HTTPRequestHandler&) = delete;
                /// <summary></summary>
                HTTPRequestHandler& operator=(HTTPRequestHandler&&) = default;

                /// <summary>Handle a request and produce a reply.</summary>
                void handleRequest(const HTTPPayload& req, HTTPPayload& reply);

            private:
                /// <summary>Perform URL-decoding on a string. Returns false if the encoding was
                /// invalid.</summary>
                static bool urlDecode(const std::string& in, std::string& out);

                std::string m_docRoot;
            };
        } // namespace http
    } // namespace rest
} // namespace network

#endif // __REST_HTTP__HTTP_REQUEST_HANDLER_H__
