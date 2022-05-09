/**
* Digital Voice Modem - Host Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Host Software
*
*/
//
// Based on code from the MMDVMHost project. (https://github.com/g4klx/MMDVMHost)
// Licensed under the GPLv2 License (https://opensource.org/licenses/GPL-2.0)
//
/*
*   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "Defines.h"
#include "edac/Golay2087.h"

using namespace edac;

#include <cstdio>
#include <cassert>

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------

const uint32_t ENCODING_TABLE_2087[] = {
    0x0000U, 0xB08EU, 0xE093U, 0x501DU, 0x70A9U, 0xC027U, 0x903AU, 0x20B4U, 0x60DCU, 0xD052U, 0x804FU, 0x30C1U,
    0x1075U, 0xA0FBU, 0xF0E6U, 0x4068U, 0x7036U, 0xC0B8U, 0x90A5U, 0x202BU, 0x009FU, 0xB011U, 0xE00CU, 0x5082U,
    0x10EAU, 0xA064U, 0xF079U, 0x40F7U, 0x6043U, 0xD0CDU, 0x80D0U, 0x305EU, 0xD06CU, 0x60E2U, 0x30FFU, 0x8071U,
    0xA0C5U, 0x104BU, 0x4056U, 0xF0D8U, 0xB0B0U, 0x003EU, 0x5023U, 0xE0ADU, 0xC019U, 0x7097U, 0x208AU, 0x9004U,
    0xA05AU, 0x10D4U, 0x40C9U, 0xF047U, 0xD0F3U, 0x607DU, 0x3060U, 0x80EEU, 0xC086U, 0x7008U, 0x2015U, 0x909BU,
    0xB02FU, 0x00A1U, 0x50BCU, 0xE032U, 0x90D9U, 0x2057U, 0x704AU, 0xC0C4U, 0xE070U, 0x50FEU, 0x00E3U, 0xB06DU,
    0xF005U, 0x408BU, 0x1096U, 0xA018U, 0x80ACU, 0x3022U, 0x603FU, 0xD0B1U, 0xE0EFU, 0x5061U, 0x007CU, 0xB0F2U,
    0x9046U, 0x20C8U, 0x70D5U, 0xC05BU, 0x8033U, 0x30BDU, 0x60A0U, 0xD02EU, 0xF09AU, 0x4014U, 0x1009U, 0xA087U,
    0x40B5U, 0xF03BU, 0xA026U, 0x10A8U, 0x301CU, 0x8092U, 0xD08FU, 0x6001U, 0x2069U, 0x90E7U, 0xC0FAU, 0x7074U,
    0x50C0U, 0xE04EU, 0xB053U, 0x00DDU, 0x3083U, 0x800DU, 0xD010U, 0x609EU, 0x402AU, 0xF0A4U, 0xA0B9U, 0x1037U,
    0x505FU, 0xE0D1U, 0xB0CCU, 0x0042U, 0x20F6U, 0x9078U, 0xC065U, 0x70EBU, 0xA03DU, 0x10B3U, 0x40AEU, 0xF020U,
    0xD094U, 0x601AU, 0x3007U, 0x8089U, 0xC0E1U, 0x706FU, 0x2072U, 0x90FCU, 0xB048U, 0x00C6U, 0x50DBU, 0xE055U,
    0xD00BU, 0x6085U, 0x3098U, 0x8016U, 0xA0A2U, 0x102CU, 0x4031U, 0xF0BFU, 0xB0D7U, 0x0059U, 0x5044U, 0xE0CAU,
    0xC07EU, 0x70F0U, 0x20EDU, 0x9063U, 0x7051U, 0xC0DFU, 0x90C2U, 0x204CU, 0x00F8U, 0xB076U, 0xE06BU, 0x50E5U,
    0x108DU, 0xA003U, 0xF01EU, 0x4090U, 0x6024U, 0xD0AAU, 0x80B7U, 0x3039U, 0x0067U, 0xB0E9U, 0xE0F4U, 0x507AU,
    0x70CEU, 0xC040U, 0x905DU, 0x20D3U, 0x60BBU, 0xD035U, 0x8028U, 0x30A6U, 0x1012U, 0xA09CU, 0xF081U, 0x400FU,
    0x30E4U, 0x806AU, 0xD077U, 0x60F9U, 0x404DU, 0xF0C3U, 0xA0DEU, 0x1050U, 0x5038U, 0xE0B6U, 0xB0ABU, 0x0025U,
    0x2091U, 0x901FU, 0xC002U, 0x708CU, 0x40D2U, 0xF05CU, 0xA041U, 0x10CFU, 0x307BU, 0x80F5U, 0xD0E8U, 0x6066U,
    0x200EU, 0x9080U, 0xC09DU, 0x7013U, 0x50A7U, 0xE029U, 0xB034U, 0x00BAU, 0xE088U, 0x5006U, 0x001BU, 0xB095U,
    0x9021U, 0x20AFU, 0x70B2U, 0xC03CU, 0x8054U, 0x30DAU, 0x60C7U, 0xD049U, 0xF0FDU, 0x4073U, 0x106EU, 0xA0E0U,
    0x90BEU, 0x2030U, 0x702DU, 0xC0A3U, 0xE017U, 0x5099U, 0x0084U, 0xB00AU, 0xF062U, 0x40ECU, 0x10F1U, 0xA07FU,
    0x80CBU, 0x3045U, 0x6058U, 0xD0D6U };

const uint32_t DECODING_TABLE_1987[] = {
    0x00000U, 0x00001U, 0x00002U, 0x00003U, 0x00004U, 0x00005U, 0x00006U, 0x00007U, 0x00008U, 0x00009U, 0x0000AU, 0x0000BU, 0x0000CU,
    0x0000DU, 0x0000EU, 0x24020U, 0x00010U, 0x00011U, 0x00012U, 0x00013U, 0x00014U, 0x00015U, 0x00016U, 0x00017U, 0x00018U, 0x00019U,
    0x0001AU, 0x0001BU, 0x0001CU, 0x0001DU, 0x48040U, 0x01480U, 0x00020U, 0x00021U, 0x00022U, 0x00023U, 0x00024U, 0x00025U, 0x00026U,
    0x24008U, 0x00028U, 0x00029U, 0x0002AU, 0x24004U, 0x0002CU, 0x24002U, 0x24001U, 0x24000U, 0x00030U, 0x00031U, 0x00032U, 0x08180U,
    0x00034U, 0x00C40U, 0x00036U, 0x00C42U, 0x00038U, 0x43000U, 0x0003AU, 0x43002U, 0x02902U, 0x24012U, 0x02900U, 0x24010U, 0x00040U,
    0x00041U, 0x00042U, 0x00043U, 0x00044U, 0x00045U, 0x00046U, 0x00047U, 0x00048U, 0x00049U, 0x0004AU, 0x02500U, 0x0004CU, 0x0004DU,
    0x48010U, 0x48011U, 0x00050U, 0x00051U, 0x00052U, 0x21200U, 0x00054U, 0x00C20U, 0x48008U, 0x48009U, 0x00058U, 0x00059U, 0x48004U,
    0x48005U, 0x48002U, 0x48003U, 0x48000U, 0x48001U, 0x00060U, 0x00061U, 0x00062U, 0x00063U, 0x00064U, 0x00C10U, 0x10300U, 0x0B000U,
    0x00068U, 0x00069U, 0x01880U, 0x01881U, 0x40181U, 0x40180U, 0x24041U, 0x24040U, 0x00070U, 0x00C04U, 0x00072U, 0x00C06U, 0x00C01U,
    0x00C00U, 0x00C03U, 0x00C02U, 0x05204U, 0x00C0CU, 0x48024U, 0x48025U, 0x05200U, 0x00C08U, 0x48020U, 0x48021U, 0x00080U, 0x00081U,
    0x00082U, 0x00083U, 0x00084U, 0x00085U, 0x00086U, 0x00087U, 0x00088U, 0x00089U, 0x0008AU, 0x50200U, 0x0008CU, 0x0A800U, 0x01411U,
    0x01410U, 0x00090U, 0x00091U, 0x00092U, 0x08120U, 0x00094U, 0x00095U, 0x04A00U, 0x01408U, 0x00098U, 0x00099U, 0x01405U, 0x01404U,
    0x01403U, 0x01402U, 0x01401U, 0x01400U, 0x000A0U, 0x000A1U, 0x000A2U, 0x08110U, 0x000A4U, 0x000A5U, 0x42400U, 0x42401U, 0x000A8U,
    0x000A9U, 0x01840U, 0x01841U, 0x40141U, 0x40140U, 0x24081U, 0x24080U, 0x000B0U, 0x08102U, 0x08101U, 0x08100U, 0x000B4U, 0x08106U,
    0x08105U, 0x08104U, 0x20A01U, 0x20A00U, 0x08109U, 0x08108U, 0x01423U, 0x01422U, 0x01421U, 0x01420U, 0x000C0U, 0x000C1U, 0x000C2U,
    0x000C3U, 0x000C4U, 0x000C5U, 0x000C6U, 0x000C7U, 0x000C8U, 0x000C9U, 0x01820U, 0x01821U, 0x20600U, 0x40120U, 0x16000U, 0x16001U,
    0x000D0U, 0x000D1U, 0x42801U, 0x42800U, 0x03100U, 0x18200U, 0x03102U, 0x18202U, 0x000D8U, 0x000D9U, 0x48084U, 0x01444U, 0x48082U,
    0x01442U, 0x48080U, 0x01440U, 0x000E0U, 0x32000U, 0x01808U, 0x04600U, 0x40109U, 0x40108U, 0x0180CU, 0x4010AU, 0x01802U, 0x40104U,
    0x01800U, 0x01801U, 0x40101U, 0x40100U, 0x01804U, 0x40102U, 0x0A408U, 0x08142U, 0x08141U, 0x08140U, 0x00C81U, 0x00C80U, 0x00C83U,
    0x00C82U, 0x0A400U, 0x0A401U, 0x01810U, 0x01811U, 0x40111U, 0x40110U, 0x01814U, 0x40112U, 0x00100U, 0x00101U, 0x00102U, 0x00103U,
    0x00104U, 0x00105U, 0x00106U, 0x41800U, 0x00108U, 0x00109U, 0x0010AU, 0x02440U, 0x0010CU, 0x0010DU, 0x0010EU, 0x02444U, 0x00110U,
    0x00111U, 0x00112U, 0x080A0U, 0x00114U, 0x00115U, 0x00116U, 0x080A4U, 0x00118U, 0x00119U, 0x15000U, 0x15001U, 0x02822U, 0x02823U,
    0x02820U, 0x02821U, 0x00120U, 0x00121U, 0x00122U, 0x08090U, 0x00124U, 0x00125U, 0x10240U, 0x10241U, 0x00128U, 0x00129U, 0x0012AU,
    0x24104U, 0x09400U, 0x400C0U, 0x02810U, 0x24100U, 0x00130U, 0x08082U, 0x08081U, 0x08080U, 0x31001U, 0x31000U, 0x02808U, 0x08084U,
    0x02806U, 0x0808AU, 0x02804U, 0x08088U, 0x02802U, 0x02803U, 0x02800U, 0x02801U, 0x00140U, 0x00141U, 0x00142U, 0x02408U, 0x00144U,
    0x00145U, 0x10220U, 0x10221U, 0x00148U, 0x02402U, 0x02401U, 0x02400U, 0x400A1U, 0x400A0U, 0x02405U, 0x02404U, 0x00150U, 0x00151U,
    0x00152U, 0x02418U, 0x03080U, 0x03081U, 0x03082U, 0x03083U, 0x09801U, 0x09800U, 0x02411U, 0x02410U, 0x48102U, 0x09804U, 0x48100U,
    0x48101U, 0x00160U, 0x00161U, 0x10204U, 0x10205U, 0x10202U, 0x40088U, 0x10200U, 0x10201U, 0x40085U, 0x40084U, 0x02421U, 0x02420U,
    0x40081U, 0x40080U, 0x10208U, 0x40082U, 0x41402U, 0x080C2U, 0x41400U, 0x080C0U, 0x00D01U, 0x00D00U, 0x10210U, 0x10211U, 0x40095U,
    0x40094U, 0x02844U, 0x080C8U, 0x40091U, 0x40090U, 0x02840U, 0x02841U, 0x00180U, 0x00181U, 0x00182U, 0x08030U, 0x00184U, 0x14400U,
    0x22201U, 0x22200U, 0x00188U, 0x00189U, 0x0018AU, 0x08038U, 0x40061U, 0x40060U, 0x40063U, 0x40062U, 0x00190U, 0x08022U, 0x08021U,
    0x08020U, 0x03040U, 0x03041U, 0x08025U, 0x08024U, 0x40C00U, 0x40C01U, 0x08029U, 0x08028U, 0x2C000U, 0x2C001U, 0x01501U, 0x01500U,
    0x001A0U, 0x08012U, 0x08011U, 0x08010U, 0x40049U, 0x40048U, 0x08015U, 0x08014U, 0x06200U, 0x40044U, 0x30400U, 0x08018U, 0x40041U,
    0x40040U, 0x40043U, 0x40042U, 0x08003U, 0x08002U, 0x08001U, 0x08000U, 0x08007U, 0x08006U, 0x08005U, 0x08004U, 0x0800BU, 0x0800AU,
    0x08009U, 0x08008U, 0x40051U, 0x40050U, 0x02880U, 0x0800CU, 0x001C0U, 0x001C1U, 0x64000U, 0x64001U, 0x03010U, 0x40028U, 0x08C00U,
    0x08C01U, 0x40025U, 0x40024U, 0x02481U, 0x02480U, 0x40021U, 0x40020U, 0x40023U, 0x40022U, 0x03004U, 0x03005U, 0x08061U, 0x08060U,
    0x03000U, 0x03001U, 0x03002U, 0x03003U, 0x0300CU, 0x40034U, 0x30805U, 0x30804U, 0x03008U, 0x40030U, 0x30801U, 0x30800U, 0x4000DU,
    0x4000CU, 0x08051U, 0x08050U, 0x40009U, 0x40008U, 0x10280U, 0x4000AU, 0x40005U, 0x40004U, 0x01900U, 0x40006U, 0x40001U, 0x40000U,
    0x40003U, 0x40002U, 0x14800U, 0x08042U, 0x08041U, 0x08040U, 0x03020U, 0x40018U, 0x08045U, 0x08044U, 0x40015U, 0x40014U, 0x08049U,
    0x08048U, 0x40011U, 0x40010U, 0x40013U, 0x40012U, 0x00200U, 0x00201U, 0x00202U, 0x00203U, 0x00204U, 0x00205U, 0x00206U, 0x00207U,
    0x00208U, 0x00209U, 0x0020AU, 0x50080U, 0x0020CU, 0x0020DU, 0x0020EU, 0x50084U, 0x00210U, 0x00211U, 0x00212U, 0x21040U, 0x00214U,
    0x00215U, 0x04880U, 0x04881U, 0x00218U, 0x00219U, 0x0E001U, 0x0E000U, 0x0021CU, 0x0021DU, 0x04888U, 0x0E004U, 0x00220U, 0x00221U,
    0x00222U, 0x00223U, 0x00224U, 0x00225U, 0x10140U, 0x10141U, 0x00228U, 0x00229U, 0x0022AU, 0x24204U, 0x12401U, 0x12400U, 0x24201U,
    0x24200U, 0x00230U, 0x00231U, 0x00232U, 0x21060U, 0x2A000U, 0x2A001U, 0x2A002U, 0x2A003U, 0x20881U, 0x20880U, 0x20883U, 0x20882U,
    0x05040U, 0x05041U, 0x05042U, 0x24210U, 0x00240U, 0x00241U, 0x00242U, 0x21010U, 0x00244U, 0x46000U, 0x10120U, 0x10121U, 0x00248U,
    0x00249U, 0x0024AU, 0x21018U, 0x20480U, 0x20481U, 0x20482U, 0x20483U, 0x00250U, 0x21002U, 0x21001U, 0x21000U, 0x18081U, 0x18080U,
    0x21005U, 0x21004U, 0x12800U, 0x12801U, 0x21009U, 0x21008U, 0x05020U, 0x05021U, 0x48200U, 0x48201U, 0x00260U, 0x00261U, 0x10104U,
    0x04480U, 0x10102U, 0x10103U, 0x10100U, 0x10101U, 0x62002U, 0x62003U, 0x62000U, 0x62001U, 0x05010U, 0x05011U, 0x10108U, 0x10109U,
    0x0500CU, 0x21022U, 0x21021U, 0x21020U, 0x05008U, 0x00E00U, 0x10110U, 0x10111U, 0x05004U, 0x05005U, 0x05006U, 0x21028U, 0x05000U,
    0x05001U, 0x05002U, 0x05003U, 0x00280U, 0x00281U, 0x00282U, 0x50008U, 0x00284U, 0x00285U, 0x04810U, 0x22100U, 0x00288U, 0x50002U,
    0x50001U, 0x50000U, 0x20440U, 0x20441U, 0x50005U, 0x50004U, 0x00290U, 0x00291U, 0x04804U, 0x04805U, 0x04802U, 0x18040U, 0x04800U,
    0x04801U, 0x20821U, 0x20820U, 0x50011U, 0x50010U, 0x0480AU, 0x01602U, 0x04808U, 0x01600U, 0x002A0U, 0x002A1U, 0x04441U, 0x04440U,
    0x002A4U, 0x002A5U, 0x04830U, 0x04444U, 0x06100U, 0x20810U, 0x50021U, 0x50020U, 0x06104U, 0x20814U, 0x50025U, 0x50024U, 0x20809U,
    0x20808U, 0x13000U, 0x08300U, 0x04822U, 0x2080CU, 0x04820U, 0x04821U, 0x20801U, 0x20800U, 0x20803U, 0x20802U, 0x20805U, 0x20804U,
    0x04828U, 0x20806U, 0x002C0U, 0x002C1U, 0x04421U, 0x04420U, 0x20408U, 0x18010U, 0x2040AU, 0x18012U, 0x20404U, 0x20405U, 0x50041U,
    0x50040U, 0x20400U, 0x20401U, 0x20402U, 0x20403U, 0x18005U, 0x18004U, 0x21081U, 0x21080U, 0x18001U, 0x18000U, 0x04840U, 0x18002U,
    0x20414U, 0x1800CU, 0x21089U, 0x21088U, 0x20410U, 0x18008U, 0x20412U, 0x1800AU, 0x04403U, 0x04402U, 0x04401U, 0x04400U, 0x10182U,
    0x04406U, 0x10180U, 0x04404U, 0x01A02U, 0x0440AU, 0x01A00U, 0x04408U, 0x20420U, 0x40300U, 0x20422U, 0x40302U, 0x04413U, 0x04412U,
    0x04411U, 0x04410U, 0x18021U, 0x18020U, 0x10190U, 0x18022U, 0x20841U, 0x20840U, 0x01A10U, 0x20842U, 0x05080U, 0x05081U, 0x05082U,
    0x05083U, 0x00300U, 0x00301U, 0x00302U, 0x00303U, 0x00304U, 0x00305U, 0x10060U, 0x22080U, 0x00308U, 0x00309U, 0x28800U, 0x28801U,
    0x44402U, 0x44403U, 0x44400U, 0x44401U, 0x00310U, 0x00311U, 0x10C01U, 0x10C00U, 0x00314U, 0x00315U, 0x10070U, 0x10C04U, 0x00318U,
    0x00319U, 0x28810U, 0x10C08U, 0x44412U, 0x00000U, 0x44410U, 0x44411U, 0x00320U, 0x60400U, 0x10044U, 0x10045U, 0x10042U, 0x0C800U,
    0x10040U, 0x10041U, 0x06080U, 0x06081U, 0x06082U, 0x06083U, 0x1004AU, 0x0C808U, 0x10048U, 0x10049U, 0x58008U, 0x08282U, 0x08281U,
    0x08280U, 0x10052U, 0x0C810U, 0x10050U, 0x10051U, 0x58000U, 0x58001U, 0x58002U, 0x08288U, 0x02A02U, 0x02A03U, 0x02A00U, 0x02A01U,
    0x00340U, 0x00341U, 0x10024U, 0x10025U, 0x10022U, 0x10023U, 0x10020U, 0x10021U, 0x34001U, 0x34000U, 0x02601U, 0x02600U, 0x1002AU,
    0x34004U, 0x10028U, 0x10029U, 0x0C400U, 0x0C401U, 0x21101U, 0x21100U, 0x60800U, 0x60801U, 0x10030U, 0x10031U, 0x0C408U, 0x34010U,
    0x21109U, 0x21108U, 0x60808U, 0x60809U, 0x10038U, 0x28420U, 0x10006U, 0x10007U, 0x10004U, 0x10005U, 0x10002U, 0x10003U, 0x10000U,
    0x10001U, 0x1000EU, 0x40284U, 0x1000CU, 0x1000DU, 0x1000AU, 0x40280U, 0x10008U, 0x10009U, 0x10016U, 0x10017U, 0x10014U, 0x10015U,
    0x10012U, 0x10013U, 0x10010U, 0x10011U, 0x05104U, 0x44802U, 0x44801U, 0x44800U, 0x05100U, 0x05101U, 0x10018U, 0x28400U, 0x00380U,
    0x00381U, 0x22005U, 0x22004U, 0x22003U, 0x22002U, 0x22001U, 0x22000U, 0x06020U, 0x06021U, 0x50101U, 0x50100U, 0x11800U, 0x11801U,
    0x22009U, 0x22008U, 0x45001U, 0x45000U, 0x08221U, 0x08220U, 0x04902U, 0x22012U, 0x04900U, 0x22010U, 0x06030U, 0x45008U, 0x08229U,
    0x08228U, 0x11810U, 0x11811U, 0x04908U, 0x22018U, 0x06008U, 0x06009U, 0x08211U, 0x08210U, 0x100C2U, 0x22022U, 0x100C0U, 0x22020U,
    0x06000U, 0x06001U, 0x06002U, 0x06003U, 0x06004U, 0x40240U, 0x06006U, 0x40242U, 0x08203U, 0x08202U, 0x08201U, 0x08200U, 0x08207U,
    0x08206U, 0x08205U, 0x08204U, 0x06010U, 0x20900U, 0x08209U, 0x08208U, 0x61002U, 0x20904U, 0x61000U, 0x61001U, 0x29020U, 0x29021U,
    0x100A4U, 0x22044U, 0x100A2U, 0x22042U, 0x100A0U, 0x22040U, 0x20504U, 0x40224U, 0x0D005U, 0x0D004U, 0x20500U, 0x40220U, 0x0D001U,
    0x0D000U, 0x03204U, 0x18104U, 0x08261U, 0x08260U, 0x03200U, 0x18100U, 0x03202U, 0x18102U, 0x11421U, 0x11420U, 0x00000U, 0x11422U,
    0x03208U, 0x18108U, 0x0D011U, 0x0D010U, 0x29000U, 0x29001U, 0x10084U, 0x04500U, 0x10082U, 0x40208U, 0x10080U, 0x10081U, 0x06040U,
    0x40204U, 0x06042U, 0x40206U, 0x40201U, 0x40200U, 0x10088U, 0x40202U, 0x29010U, 0x08242U, 0x08241U, 0x08240U, 0x10092U, 0x40218U,
    0x10090U, 0x10091U, 0x11401U, 0x11400U, 0x11403U, 0x11402U, 0x40211U, 0x40210U, 0x10098U, 0x40212U, 0x00400U, 0x00401U, 0x00402U,
    0x00403U, 0x00404U, 0x00405U, 0x00406U, 0x00407U, 0x00408U, 0x00409U, 0x0040AU, 0x02140U, 0x0040CU, 0x0040DU, 0x01091U, 0x01090U,
    0x00410U, 0x00411U, 0x00412U, 0x00413U, 0x00414U, 0x00860U, 0x01089U, 0x01088U, 0x00418U, 0x38000U, 0x01085U, 0x01084U, 0x01083U,
    0x01082U, 0x01081U, 0x01080U, 0x00420U, 0x00421U, 0x00422U, 0x00423U, 0x00424U, 0x00850U, 0x42080U, 0x42081U, 0x00428U, 0x00429U,
    0x48801U, 0x48800U, 0x09100U, 0x12200U, 0x24401U, 0x24400U, 0x00430U, 0x00844U, 0x00432U, 0x00846U, 0x00841U, 0x00840U, 0x1C000U,
    0x00842U, 0x00438U, 0x0084CU, 0x010A5U, 0x010A4U, 0x00849U, 0x00848U, 0x010A1U, 0x010A0U, 0x00440U, 0x00441U, 0x00442U, 0x02108U,
    0x00444U, 0x00830U, 0x70001U, 0x70000U, 0x00448U, 0x02102U, 0x02101U, 0x02100U, 0x20280U, 0x20281U, 0x02105U, 0x02104U, 0x00450U,
    0x00824U, 0x00452U, 0x00826U, 0x00821U, 0x00820U, 0x00823U, 0x00822U, 0x24802U, 0x02112U, 0x24800U, 0x02110U, 0x00829U, 0x00828U,
    0x48400U, 0x010C0U, 0x00460U, 0x00814U, 0x04281U, 0x04280U, 0x00811U, 0x00810U, 0x00813U, 0x00812U, 0x54000U, 0x54001U, 0x02121U,
    0x02120U, 0x00819U, 0x00818U, 0x0081BU, 0x0081AU, 0x00805U, 0x00804U, 0x41100U, 0x00806U, 0x00801U, 0x00800U, 0x00803U, 0x00802U,
    0x0A080U, 0x0080CU, 0x0A082U, 0x0080EU, 0x00809U, 0x00808U, 0x0080BU, 0x0080AU, 0x00480U, 0x00481U, 0x00482U, 0x00483U, 0x00484U,
    0x14100U, 0x42020U, 0x01018U, 0x00488U, 0x00489U, 0x01015U, 0x01014U, 0x20240U, 0x01012U, 0x01011U, 0x01010U, 0x00490U, 0x00491U,
    0x0100DU, 0x0100CU, 0x0100BU, 0x0100AU, 0x01009U, 0x01008U, 0x40900U, 0x01006U, 0x01005U, 0x01004U, 0x01003U, 0x01002U, 0x01001U,
    0x01000U, 0x004A0U, 0x004A1U, 0x42004U, 0x04240U, 0x42002U, 0x42003U, 0x42000U, 0x42001U, 0x30102U, 0x30103U, 0x30100U, 0x30101U,
    0x4200AU, 0x01032U, 0x42008U, 0x01030U, 0x25000U, 0x25001U, 0x08501U, 0x08500U, 0x008C1U, 0x008C0U, 0x42010U, 0x01028U, 0x0A040U,
    0x0A041U, 0x01025U, 0x01024U, 0x01023U, 0x01022U, 0x01021U, 0x01020U, 0x004C0U, 0x49000U, 0x04221U, 0x04220U, 0x20208U, 0x20209U,
    0x08900U, 0x08901U, 0x20204U, 0x20205U, 0x02181U, 0x02180U, 0x20200U, 0x20201U, 0x20202U, 0x01050U, 0x0A028U, 0x008A4U, 0x0104DU,
    0x0104CU, 0x008A1U, 0x008A0U, 0x01049U, 0x01048U, 0x0A020U, 0x0A021U, 0x01045U, 0x01044U, 0x20210U, 0x01042U, 0x01041U, 0x01040U,
    0x04203U, 0x04202U, 0x04201U, 0x04200U, 0x00891U, 0x00890U, 0x42040U, 0x04204U, 0x0A010U, 0x0A011U, 0x01C00U, 0x04208U, 0x20220U,
    0x40500U, 0x20222U, 0x40502U, 0x0A008U, 0x00884U, 0x04211U, 0x04210U, 0x00881U, 0x00880U, 0x00883U, 0x00882U, 0x0A000U, 0x0A001U,
    0x0A002U, 0x0A003U, 0x0A004U, 0x00888U, 0x01061U, 0x01060U, 0x00500U, 0x00501U, 0x00502U, 0x02048U, 0x00504U, 0x14080U, 0x00506U,
    0x14082U, 0x00508U, 0x02042U, 0x02041U, 0x02040U, 0x09020U, 0x09021U, 0x44200U, 0x02044U, 0x00510U, 0x00511U, 0x10A01U, 0x10A00U,
    0x4A001U, 0x4A000U, 0x4A003U, 0x4A002U, 0x40880U, 0x40881U, 0x02051U, 0x02050U, 0x40884U, 0x01182U, 0x01181U, 0x01180U, 0x00520U,
    0x60200U, 0x00522U, 0x60202U, 0x09008U, 0x09009U, 0x0900AU, 0x0900BU, 0x09004U, 0x09005U, 0x30080U, 0x02060U, 0x09000U, 0x09001U,
    0x09002U, 0x09003U, 0x41042U, 0x08482U, 0x41040U, 0x08480U, 0x00941U, 0x00940U, 0x41044U, 0x00942U, 0x09014U, 0x09015U, 0x02C04U,
    0x08488U, 0x09010U, 0x09011U, 0x02C00U, 0x02C01U, 0x00540U, 0x0200AU, 0x02009U, 0x02008U, 0x08882U, 0x0200EU, 0x08880U, 0x0200CU,
    0x02003U, 0x02002U, 0x02001U, 0x02000U, 0x02007U, 0x02006U, 0x02005U, 0x02004U, 0x0C200U, 0x0C201U, 0x41020U, 0x02018U, 0x00921U,
    0x00920U, 0x41024U, 0x00922U, 0x02013U, 0x02012U, 0x02011U, 0x02010U, 0x02017U, 0x02016U, 0x02015U, 0x02014U, 0x41012U, 0x0202AU,
    0x41010U, 0x02028U, 0x26000U, 0x00910U, 0x10600U, 0x10601U, 0x02023U, 0x02022U, 0x02021U, 0x02020U, 0x09040U, 0x40480U, 0x02025U,
    0x02024U, 0x41002U, 0x00904U, 0x41000U, 0x41001U, 0x00901U, 0x00900U, 0x41004U, 0x00902U, 0x4100AU, 0x02032U, 0x41008U, 0x02030U,
    0x00909U, 0x00908U, 0x28201U, 0x28200U, 0x00580U, 0x14004U, 0x00582U, 0x14006U, 0x14001U, 0x14000U, 0x08840U, 0x14002U, 0x40810U,
    0x40811U, 0x30020U, 0x020C0U, 0x14009U, 0x14008U, 0x01111U, 0x01110U, 0x40808U, 0x40809U, 0x08421U, 0x08420U, 0x14011U, 0x14010U,
    0x01109U, 0x01108U, 0x40800U, 0x40801U, 0x40802U, 0x01104U, 0x40804U, 0x01102U, 0x01101U, 0x01100U, 0x03801U, 0x03800U, 0x30008U,
    0x08410U, 0x14021U, 0x14020U, 0x42100U, 0x42101U, 0x30002U, 0x30003U, 0x30000U, 0x30001U, 0x09080U, 0x40440U, 0x30004U, 0x30005U,
    0x08403U, 0x08402U, 0x08401U, 0x08400U, 0x08407U, 0x08406U, 0x08405U, 0x08404U, 0x40820U, 0x40821U, 0x30010U, 0x08408U, 0x40824U,
    0x01122U, 0x01121U, 0x01120U, 0x08806U, 0x0208AU, 0x08804U, 0x02088U, 0x08802U, 0x14040U, 0x08800U, 0x08801U, 0x02083U, 0x02082U,
    0x02081U, 0x02080U, 0x20300U, 0x40420U, 0x08808U, 0x02084U, 0x03404U, 0x03405U, 0x08814U, 0x02098U, 0x03400U, 0x03401U, 0x08810U,
    0x08811U, 0x40840U, 0x40841U, 0x02091U, 0x02090U, 0x40844U, 0x01142U, 0x01141U, 0x01140U, 0x04303U, 0x04302U, 0x04301U, 0x04300U,
    0x40409U, 0x40408U, 0x08820U, 0x08821U, 0x40405U, 0x40404U, 0x30040U, 0x020A0U, 0x40401U, 0x40400U, 0x40403U, 0x40402U, 0x41082U,
    0x08442U, 0x41080U, 0x08440U, 0x00981U, 0x00980U, 0x41084U, 0x00982U, 0x0A100U, 0x11200U, 0x0A102U, 0x11202U, 0x40411U, 0x40410U,
    0x40413U, 0x40412U, 0x00600U, 0x00601U, 0x00602U, 0x00603U, 0x00604U, 0x00605U, 0x00606U, 0x00607U, 0x00608U, 0x05800U, 0x0060AU,
    0x05802U, 0x200C0U, 0x12020U, 0x44100U, 0x44101U, 0x00610U, 0x00611U, 0x10901U, 0x10900U, 0x51000U, 0x51001U, 0x51002U, 0x10904U,
    0x00618U, 0x05810U, 0x01285U, 0x01284U, 0x51008U, 0x01282U, 0x01281U, 0x01280U, 0x00620U, 0x60100U, 0x040C1U, 0x040C0U, 0x12009U,
    0x12008U, 0x21800U, 0x21801U, 0x12005U, 0x12004U, 0x12007U, 0x12006U, 0x12001U, 0x12000U, 0x12003U, 0x12002U, 0x00630U, 0x00A44U,
    0x040D1U, 0x040D0U, 0x00A41U, 0x00A40U, 0x21810U, 0x00A42U, 0x12015U, 0x12014U, 0x00000U, 0x12016U, 0x12011U, 0x12010U, 0x12013U,
    0x12012U, 0x00640U, 0x00641U, 0x040A1U, 0x040A0U, 0x20088U, 0x20089U, 0x2008AU, 0x040A4U, 0x20084U, 0x20085U, 0x19000U, 0x02300U,
    0x20080U, 0x20081U, 0x20082U, 0x20083U, 0x0C100U, 0x0C101U, 0x21401U, 0x21400U, 0x00A21U, 0x00A20U, 0x00A23U, 0x00A22U, 0x20094U,
    0x20095U, 0x19010U, 0x21408U, 0x20090U, 0x20091U, 0x20092U, 0x28120U, 0x04083U, 0x04082U, 0x04081U, 0x04080U, 0x00A11U, 0x00A10U,
    0x10500U, 0x04084U, 0x200A4U, 0x0408AU, 0x04089U, 0x04088U, 0x200A0U, 0x12040U, 0x200A2U, 0x12042U, 0x00A05U, 0x00A04U, 0x04091U,
    0x04090U, 0x00A01U, 0x00A00U, 0x00A03U, 0x00A02U, 0x05404U, 0x00A0CU, 0x28105U, 0x28104U, 0x05400U, 0x00A08U, 0x28101U, 0x28100U,
    0x00680U, 0x00681U, 0x04061U, 0x04060U, 0x20048U, 0x20049U, 0x2004AU, 0x04064U, 0x20044U, 0x20045U, 0x50401U, 0x50400U, 0x20040U,
    0x20041U, 0x20042U, 0x01210U, 0x68002U, 0x68003U, 0x68000U, 0x68001U, 0x04C02U, 0x0120AU, 0x04C00U, 0x01208U, 0x20054U, 0x01206U,
    0x01205U, 0x01204U, 0x20050U, 0x01202U, 0x01201U, 0x01200U, 0x18800U, 0x04042U, 0x04041U, 0x04040U, 0x42202U, 0x04046U, 0x42200U,
    0x04044U, 0x20064U, 0x0404AU, 0x04049U, 0x04048U, 0x20060U, 0x12080U, 0x20062U, 0x12082U, 0x18810U, 0x04052U, 0x04051U, 0x04050U,
    0x4C009U, 0x4C008U, 0x42210U, 0x04054U, 0x20C01U, 0x20C00U, 0x20C03U, 0x20C02U, 0x4C001U, 0x4C000U, 0x01221U, 0x01220U, 0x2000CU,
    0x04022U, 0x04021U, 0x04020U, 0x20008U, 0x20009U, 0x2000AU, 0x04024U, 0x20004U, 0x20005U, 0x20006U, 0x04028U, 0x20000U, 0x20001U,
    0x20002U, 0x20003U, 0x2001CU, 0x04032U, 0x04031U, 0x04030U, 0x20018U, 0x18400U, 0x2001AU, 0x18402U, 0x20014U, 0x20015U, 0x20016U,
    0x01244U, 0x20010U, 0x20011U, 0x20012U, 0x01240U, 0x04003U, 0x04002U, 0x04001U, 0x04000U, 0x20028U, 0x04006U, 0x04005U, 0x04004U,
    0x20024U, 0x0400AU, 0x04009U, 0x04008U, 0x20020U, 0x20021U, 0x20022U, 0x0400CU, 0x04013U, 0x04012U, 0x04011U, 0x04010U, 0x00A81U,
    0x00A80U, 0x04015U, 0x04014U, 0x0A200U, 0x11100U, 0x04019U, 0x04018U, 0x20030U, 0x20031U, 0x50800U, 0x50801U, 0x00700U, 0x60020U,
    0x10811U, 0x10810U, 0x4400AU, 0x60024U, 0x44008U, 0x44009U, 0x44006U, 0x02242U, 0x44004U, 0x02240U, 0x44002U, 0x44003U, 0x44000U,
    0x44001U, 0x0C040U, 0x10802U, 0x10801U, 0x10800U, 0x0C044U, 0x10806U, 0x10805U, 0x10804U, 0x23000U, 0x23001U, 0x10809U, 0x10808U,
    0x44012U, 0x44013U, 0x44010U, 0x44011U, 0x60001U, 0x60000U, 0x60003U, 0x60002U, 0x60005U, 0x60004U, 0x10440U, 0x10441U, 0x60009U,
    0x60008U, 0x44024U, 0x6000AU, 0x09200U, 0x12100U, 0x44020U, 0x44021U, 0x60011U, 0x60010U, 0x10821U, 0x10820U, 0x07003U, 0x07002U,
    0x07001U, 0x07000U, 0x23020U, 0x60018U, 0x28045U, 0x28044U, 0x09210U, 0x28042U, 0x28041U, 0x28040U, 0x0C010U, 0x0C011U, 0x02209U,
    0x02208U, 0x10422U, 0x10423U, 0x10420U, 0x10421U, 0x02203U, 0x02202U, 0x02201U, 0x02200U, 0x20180U, 0x20181U, 0x44040U, 0x02204U,
    0x0C000U, 0x0C001U, 0x0C002U, 0x10840U, 0x0C004U, 0x0C005U, 0x0C006U, 0x10844U, 0x0C008U, 0x0C009U, 0x02211U, 0x02210U, 0x0C00CU,
    0x28022U, 0x28021U, 0x28020U, 0x60041U, 0x60040U, 0x10404U, 0x04180U, 0x10402U, 0x10403U, 0x10400U, 0x10401U, 0x02223U, 0x02222U,
    0x02221U, 0x02220U, 0x1040AU, 0x28012U, 0x10408U, 0x28010U, 0x0C020U, 0x0C021U, 0x41200U, 0x41201U, 0x00B01U, 0x00B00U, 0x10410U,
    0x28008U, 0x11081U, 0x11080U, 0x28005U, 0x28004U, 0x28003U, 0x28002U, 0x28001U, 0x28000U, 0x52040U, 0x14204U, 0x22405U, 0x22404U,
    0x14201U, 0x14200U, 0x22401U, 0x22400U, 0x20144U, 0x20145U, 0x44084U, 0x022C0U, 0x20140U, 0x20141U, 0x44080U, 0x44081U, 0x40A08U,
    0x10882U, 0x10881U, 0x10880U, 0x14211U, 0x14210U, 0x1A008U, 0x10884U, 0x40A00U, 0x40A01U, 0x40A02U, 0x01304U, 0x1A002U, 0x01302U,
    0x1A000U, 0x01300U, 0x60081U, 0x60080U, 0x04141U, 0x04140U, 0x60085U, 0x60084U, 0x104C0U, 0x04144U, 0x06400U, 0x06401U, 0x30200U,
    0x30201U, 0x06404U, 0x40640U, 0x30204U, 0x30205U, 0x08603U, 0x08602U, 0x08601U, 0x08600U, 0x00000U, 0x08606U, 0x08605U, 0x08604U,
    0x11041U, 0x11040U, 0x30210U, 0x11042U, 0x11045U, 0x11044U, 0x1A020U, 0x01320U, 0x52000U, 0x52001U, 0x04121U, 0x04120U, 0x20108U,
    0x20109U, 0x08A00U, 0x08A01U, 0x20104U, 0x20105U, 0x02281U, 0x02280U, 0x20100U, 0x20101U, 0x20102U, 0x20103U, 0x0C080U, 0x0C081U,
    0x0C082U, 0x04130U, 0x0C084U, 0x06808U, 0x08A10U, 0x08A11U, 0x11021U, 0x11020U, 0x11023U, 0x11022U, 0x20110U, 0x06800U, 0x20112U,
    0x06802U, 0x04103U, 0x04102U, 0x04101U, 0x04100U, 0x10482U, 0x04106U, 0x10480U, 0x04104U, 0x11011U, 0x11010U, 0x04109U, 0x04108U,
    0x20120U, 0x40600U, 0x20122U, 0x40602U, 0x11009U, 0x11008U, 0x22800U, 0x04110U, 0x1100DU, 0x1100CU, 0x22804U, 0x04114U, 0x11001U,
    0x11000U, 0x11003U, 0x11002U, 0x11005U, 0x11004U, 0x28081U, 0x28080U };

#define X18             0x00040000   /* vector representation of X^{18} */
#define X11             0x00000800   /* vector representation of X^{11} */
#define MASK8           0xfffff800   /* auxiliary vector for testing */
#define GENPOL          0x00000c75   /* generator polinomial, g(x) */

// ---------------------------------------------------------------------------
//  Static Class Members
// ---------------------------------------------------------------------------

/// <summary>
/// Decode Golay (20,8,7) FEC.
/// </summary>
/// <param name="data">Golay FEC encoded data byte array</param>
/// <returns></returns>
uint8_t Golay2087::decode(const uint8_t* data)
{
    assert(data != NULL);

    uint32_t code = (data[0U] << 11) + (data[1U] << 3) + (data[2U] >> 5);
    uint32_t syndrome = getSyndrome1987(code);
    uint32_t error_pattern = DECODING_TABLE_1987[syndrome];

    if (error_pattern != 0x00U)
        code ^= error_pattern;

    return code >> 11;
}

/// <summary>
/// Encode Golay (20,8,7) FEC.
/// </summary>
/// <param name="data">Data to encode with Golay FEC.</param>
void Golay2087::encode(uint8_t* data)
{
    assert(data != NULL);

    uint32_t value = data[0U];

    uint32_t cksum = ENCODING_TABLE_2087[value];

    data[1U] = cksum & 0xFFU;
    data[2U] = cksum >> 8;
}

// ---------------------------------------------------------------------------
//  Private Static Class Members
// ---------------------------------------------------------------------------

/// <summary>
///
/// </summary>
/// <remarks>
/// Compute the syndrome corresponding to the given pattern, i.e., the
/// remainder after dividing the pattern (when considering it as the vector
/// representation of a polynomial) by the generator polynomial, GENPOL.
/// In the program this pattern has several meanings: (1) pattern = infomation
/// bits, when constructing the encoding table; (2) pattern = error pattern,
/// when constructing the decoding table; and (3) pattern = received vector, to
/// obtain its syndrome in decoding.
/// </remarks>
/// <param name="pattern"></param>
/// <returns></returns>
uint32_t Golay2087::getSyndrome1987(uint32_t pattern)
{
    uint32_t aux = X18;

    if (pattern >= X11) {
        while (pattern & MASK8) {
            while (!(aux & pattern))
                aux = aux >> 1;

            pattern ^= (aux / X11) * GENPOL;
        }
    }

    return pattern;
}
