// SPDX-License-Identifier: GPL-2.0-only
/**
* Digital Voice Modem - Common Library
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Common Library
* @license GPLv2 License (https://opensource.org/licenses/GPL-2.0)
*
*   Copyright (C) 2022,2024 Bryan Biedenkapp, N2PLL
*
*/
#if !defined(__NXDN_SITE_DATA_H__)
#define  __NXDN_SITE_DATA_H__

#include "common/Defines.h"
#include "common/nxdn/NXDNDefines.h"

namespace nxdn
{
    // ---------------------------------------------------------------------------
    //  Class Declaration
    //      Represents site data for NXDN.
    // ---------------------------------------------------------------------------

    class HOST_SW_API SiteData {
    public:
        /// <summary>Initializes a new instance of the SiteData class.</summary>
        SiteData() :
            m_locId(1U),
            m_channelId(1U),
            m_channelNo(1U),
            m_siteInfo1(defines::SiteInformation1::VOICE_CALL_SVC | defines::SiteInformation1::DATA_CALL_SVC),
            m_siteInfo2(0U),
            m_isAdjSite(false),
            m_callsign("CHANGEME"),
            m_requireReg(false),
            m_netActive(false)
        {
            /* stub */
        }
        /// <summary>Initializes a new instance of the SiteData class.</summary>
        /// <param name="locId">NXDN Location ID.</param>
        /// <param name="channelId">Channel ID.</param>
        /// <param name="channelNo">Channel Number.</param>
        /// <param name="siteInfo1">Site Information 1.</param>
        /// <param name="siteInfo2">Site Information 2.</param>
        /// <param name="requireReg"></param>
        SiteData(uint32_t locId, uint8_t channelId, uint32_t channelNo, uint8_t siteInfo1, uint8_t siteInfo2, bool requireReq) :
            m_locId(locId),
            m_channelId(channelId),
            m_channelNo(channelNo),
            m_siteInfo1(defines::SiteInformation1::VOICE_CALL_SVC | defines::SiteInformation1::DATA_CALL_SVC),
            m_siteInfo2(0U),
            m_isAdjSite(false),
            m_callsign("CHANGEME"),
            m_requireReg(requireReq),
            m_netActive(false)
        {
            if (m_locId > 0xFFFFFFU)
                m_locId = 0xFFFFFFU;

            // channel id clamping
            if (m_channelId > 15U)
                m_channelId = 15U;

            // channel number clamping
            if (m_channelNo == 0U) { // clamp to 1
                m_channelNo = 1U;
            }
            if (m_channelNo > 1023U) { // clamp to 1023
                m_channelNo = 1023U;
            }

            m_siteInfo1 = siteInfo1;
            m_siteInfo2 = siteInfo2;
        }

        /// <summary>Helper to set the site callsign.</summary>
        /// <param name="callsign">Callsign.</param>
        void setCallsign(std::string callsign)
        {
            m_callsign = callsign;
        }

        /// <summary>Helper to set the site network active flag.</summary>
        /// <param name="netActive">Network active.</param>
        void setNetActive(bool netActive)
        {
            m_netActive = netActive;
        }

        /// <summary>Helper to set adjacent site data.</summary>
        /// <param name="locId">NXDN Location ID.</param>
        /// <param name="channelId">Channel ID.</param>
        /// <param name="channelNo">Channel Number.</param>
        /// <param name="siteInfo1">Site Information 1.</param>
        /// <param name="siteInfo2">Site Information 2.</param>
        void setAdjSite(uint32_t locId, uint8_t rfssId, uint8_t siteId, uint8_t channelId, uint32_t channelNo, uint8_t siteInfo1, uint8_t siteInfo2)
        {
            if (m_locId > 0xFFFFFFU)
                m_locId = 0xFFFFFFU;

            // channel id clamping
            if (channelId > 15U)
                channelId = 15U;

            // channel number clamping
            if (channelNo == 0U) { // clamp to 1
                channelNo = 1U;
            }
            if (channelNo > 1023U) { // clamp to 1023
                channelNo = 1023U;
            }

            m_locId = locId;

            m_channelId = channelId;
            m_channelNo = channelNo;

            m_siteInfo1 = siteInfo1;
            m_siteInfo2 = siteInfo2;

            m_isAdjSite = true;

            m_callsign = "ADJSITE ";
            m_netActive = true; // adjacent sites are explicitly network active
        }

        /// <summary>Equals operator.</summary>
        /// <param name="data"></param>
        /// <returns></returns>
        SiteData & operator=(const SiteData & data)
        {
            if (this != &data) {
                m_locId = data.m_locId;

                m_channelId = data.m_channelId;
                m_channelNo = data.m_channelNo;

                m_siteInfo1 = data.m_siteInfo1;
                m_siteInfo2 = data.m_siteInfo2;

                m_isAdjSite = data.m_isAdjSite;

                m_callsign = data.m_callsign;

                m_requireReg = data.m_requireReg;

                m_netActive = data.m_netActive;
            }

            return *this;
        }

    public:
        /// <summary>NXDN location ID.</summary>
        __READONLY_PROPERTY_PLAIN(uint32_t, locId);
        /// <summary>Channel ID.</summary>
        __READONLY_PROPERTY_PLAIN(uint8_t, channelId);
        /// <summary>Channel number.</summary>
        __READONLY_PROPERTY_PLAIN(uint32_t, channelNo);
        /// <summary>Site Information 1.</summary>
        __READONLY_PROPERTY_PLAIN(uint8_t, siteInfo1);
        /// <summary>Site Information 2.</summary>
        __READONLY_PROPERTY_PLAIN(uint8_t, siteInfo2);
        /// <summary>Flag indicating whether this site data is for an adjacent site.</summary>
        __READONLY_PROPERTY_PLAIN(bool, isAdjSite);
        /// <summary>Callsign.</summary>
        __READONLY_PROPERTY_PLAIN(std::string, callsign);
        /// <summary>NXDN require registration.</summary>
        __READONLY_PROPERTY_PLAIN(bool, requireReg);
        /// <summary>Flag indicating whether this site is a linked active network member.</summary>
        __READONLY_PROPERTY_PLAIN(bool, netActive);
    };
} // namespace nxdn

#endif // __NXDN_SITE_DATA_H__
