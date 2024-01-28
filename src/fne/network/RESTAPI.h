// SPDX-License-Identifier: GPL-2.0-only
/**
* Digital Voice Modem - Converged FNE Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Converged FNE Software
* @license GPLv2 License (https://opensource.org/licenses/GPL-2.0)
*
*   Copyright (C) 2024 Bryan Biedenkapp, N2PLL
*
*/
#if !defined(__REST_API_H__)
#define __REST_API_H__

#include "fne/Defines.h"
#include "common/network/UDPSocket.h"
#include "common/network/rest/RequestDispatcher.h"
#include "common/network/rest/http/HTTPServer.h"
#include "common/lookups/RadioIdLookup.h"
#include "common/lookups/TalkgroupRulesLookup.h"
#include "common/Thread.h"
#include "fne/network/RESTDefines.h"

#include <vector>
#include <string>
#include <random>

// ---------------------------------------------------------------------------
//  Class Prototypes
// ---------------------------------------------------------------------------

class HOST_SW_API HostFNE;
namespace network { class HOST_SW_API FNENetwork; }

// ---------------------------------------------------------------------------
//  Class Declaration
//      Implements the REST API server logic.
// ---------------------------------------------------------------------------

class HOST_SW_API RESTAPI : private Thread {
public:
    /// <summary>Initializes a new instance of the RESTAPI class.</summary>
    RESTAPI(const std::string& address, uint16_t port, const std::string& password, HostFNE* host, bool debug);
    /// <summary>Finalizes a instance of the RESTAPI class.</summary>
    ~RESTAPI() override;

    /// <summary>Sets the instances of the Radio ID and Talkgroup ID lookup tables.</summary>
    void setLookups(::lookups::RadioIdLookup* ridLookup, ::lookups::TalkgroupRulesLookup* tidLookup);
    /// <summary>Sets the instance of the FNE network.</summary>
    void setNetwork(::network::FNENetwork* network);

    /// <summary>Opens connection to the network.</summary>
    bool open();

    /// <summary>Closes connection to the network.</summary>
    void close();

private:
    typedef network::rest::RequestDispatcher<network::rest::http::HTTPPayload, network::rest::http::HTTPPayload> RESTDispatcherType;
    typedef network::rest::http::HTTPPayload HTTPPayload;
    RESTDispatcherType m_dispatcher;
    network::rest::http::HTTPServer<RESTDispatcherType> m_restServer;

    std::mt19937 m_random;

    std::string m_password;
    uint8_t* m_passwordHash;
    bool m_debug;

    HostFNE* m_host;
    network::FNENetwork *m_network;

    ::lookups::RadioIdLookup* m_ridLookup;
    ::lookups::TalkgroupRulesLookup* m_tidLookup;

    typedef std::unordered_map<std::string, uint64_t>::value_type AuthTokenValueType;
    std::unordered_map<std::string, uint64_t> m_authTokens;

    /// <summary></summary>
    void entry() override;

    /// <summary>Helper to initialize REST API endpoints.</summary>
    void initializeEndpoints();

    /// <summary></summary>
    void invalidateHostToken(const std::string host);
    /// <summary></summary>
    bool validateAuth(const HTTPPayload& request, HTTPPayload& reply);

    /// <summary></summary>
    void restAPI_PutAuth(const HTTPPayload& request, HTTPPayload& reply, const network::rest::RequestMatch& match);

    /// <summary></summary>
    void restAPI_GetVersion(const HTTPPayload& request, HTTPPayload& reply, const network::rest::RequestMatch& match);
    /// <summary></summary>
    void restAPI_GetStatus(const HTTPPayload& request, HTTPPayload& reply, const network::rest::RequestMatch& match);
    /// <summary></summary>
    void restAPI_GetPeerList(const HTTPPayload& request, HTTPPayload& reply, const network::rest::RequestMatch& match);
    /// <summary></summary>
    void restAPI_GetTGIDList(const HTTPPayload& request, HTTPPayload& reply, const network::rest::RequestMatch& match);

    /// <summary></summary>
    void restAPI_GetForceUpdate(const HTTPPayload& request, HTTPPayload& reply, const network::rest::RequestMatch& match);
};

#endif // __REST_API_H__
