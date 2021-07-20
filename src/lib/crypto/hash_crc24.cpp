/*
 * Copyright (c) 2017-2021, [Ribose Inc](https://www.ribose.com).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 * 2.  Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stddef.h>
#include "utils.h"
#include "hash_crc24.h"

static const uint32_t T0[256] = {
  0x00000000, 0x00FB4C86, 0x000DD58A, 0x00F6990C, 0x00E1E693, 0x001AAA15, 0x00EC3319,
  0x00177F9F, 0x003981A1, 0x00C2CD27, 0x0034542B, 0x00CF18AD, 0x00D86732, 0x00232BB4,
  0x00D5B2B8, 0x002EFE3E, 0x00894EC5, 0x00720243, 0x00849B4F, 0x007FD7C9, 0x0068A856,
  0x0093E4D0, 0x00657DDC, 0x009E315A, 0x00B0CF64, 0x004B83E2, 0x00BD1AEE, 0x00465668,
  0x005129F7, 0x00AA6571, 0x005CFC7D, 0x00A7B0FB, 0x00E9D10C, 0x00129D8A, 0x00E40486,
  0x001F4800, 0x0008379F, 0x00F37B19, 0x0005E215, 0x00FEAE93, 0x00D050AD, 0x002B1C2B,
  0x00DD8527, 0x0026C9A1, 0x0031B63E, 0x00CAFAB8, 0x003C63B4, 0x00C72F32, 0x00609FC9,
  0x009BD34F, 0x006D4A43, 0x009606C5, 0x0081795A, 0x007A35DC, 0x008CACD0, 0x0077E056,
  0x00591E68, 0x00A252EE, 0x0054CBE2, 0x00AF8764, 0x00B8F8FB, 0x0043B47D, 0x00B52D71,
  0x004E61F7, 0x00D2A319, 0x0029EF9F, 0x00DF7693, 0x00243A15, 0x0033458A, 0x00C8090C,
  0x003E9000, 0x00C5DC86, 0x00EB22B8, 0x00106E3E, 0x00E6F732, 0x001DBBB4, 0x000AC42B,
  0x00F188AD, 0x000711A1, 0x00FC5D27, 0x005BEDDC, 0x00A0A15A, 0x00563856, 0x00AD74D0,
  0x00BA0B4F, 0x004147C9, 0x00B7DEC5, 0x004C9243, 0x00626C7D, 0x009920FB, 0x006FB9F7,
  0x0094F571, 0x00838AEE, 0x0078C668, 0x008E5F64, 0x007513E2, 0x003B7215, 0x00C03E93,
  0x0036A79F, 0x00CDEB19, 0x00DA9486, 0x0021D800, 0x00D7410C, 0x002C0D8A, 0x0002F3B4,
  0x00F9BF32, 0x000F263E, 0x00F46AB8, 0x00E31527, 0x001859A1, 0x00EEC0AD, 0x00158C2B,
  0x00B23CD0, 0x00497056, 0x00BFE95A, 0x0044A5DC, 0x0053DA43, 0x00A896C5, 0x005E0FC9,
  0x00A5434F, 0x008BBD71, 0x0070F1F7, 0x008668FB, 0x007D247D, 0x006A5BE2, 0x00911764,
  0x00678E68, 0x009CC2EE, 0x00A44733, 0x005F0BB5, 0x00A992B9, 0x0052DE3F, 0x0045A1A0,
  0x00BEED26, 0x0048742A, 0x00B338AC, 0x009DC692, 0x00668A14, 0x00901318, 0x006B5F9E,
  0x007C2001, 0x00876C87, 0x0071F58B, 0x008AB90D, 0x002D09F6, 0x00D64570, 0x0020DC7C,
  0x00DB90FA, 0x00CCEF65, 0x0037A3E3, 0x00C13AEF, 0x003A7669, 0x00148857, 0x00EFC4D1,
  0x00195DDD, 0x00E2115B, 0x00F56EC4, 0x000E2242, 0x00F8BB4E, 0x0003F7C8, 0x004D963F,
  0x00B6DAB9, 0x004043B5, 0x00BB0F33, 0x00AC70AC, 0x00573C2A, 0x00A1A526, 0x005AE9A0,
  0x0074179E, 0x008F5B18, 0x0079C214, 0x00828E92, 0x0095F10D, 0x006EBD8B, 0x00982487,
  0x00636801, 0x00C4D8FA, 0x003F947C, 0x00C90D70, 0x003241F6, 0x00253E69, 0x00DE72EF,
  0x0028EBE3, 0x00D3A765, 0x00FD595B, 0x000615DD, 0x00F08CD1, 0x000BC057, 0x001CBFC8,
  0x00E7F34E, 0x00116A42, 0x00EA26C4, 0x0076E42A, 0x008DA8AC, 0x007B31A0, 0x00807D26,
  0x009702B9, 0x006C4E3F, 0x009AD733, 0x00619BB5, 0x004F658B, 0x00B4290D, 0x0042B001,
  0x00B9FC87, 0x00AE8318, 0x0055CF9E, 0x00A35692, 0x00581A14, 0x00FFAAEF, 0x0004E669,
  0x00F27F65, 0x000933E3, 0x001E4C7C, 0x00E500FA, 0x001399F6, 0x00E8D570, 0x00C62B4E,
  0x003D67C8, 0x00CBFEC4, 0x0030B242, 0x0027CDDD, 0x00DC815B, 0x002A1857, 0x00D154D1,
  0x009F3526, 0x006479A0, 0x0092E0AC, 0x0069AC2A, 0x007ED3B5, 0x00859F33, 0x0073063F,
  0x00884AB9, 0x00A6B487, 0x005DF801, 0x00AB610D, 0x00502D8B, 0x00475214, 0x00BC1E92,
  0x004A879E, 0x00B1CB18, 0x00167BE3, 0x00ED3765, 0x001BAE69, 0x00E0E2EF, 0x00F79D70,
  0x000CD1F6, 0x00FA48FA, 0x0001047C, 0x002FFA42, 0x00D4B6C4, 0x00222FC8, 0x00D9634E,
  0x00CE1CD1, 0x00355057, 0x00C3C95B, 0x003885DD,
};

static const uint32_t T1[256] = {
  0x00000000, 0x00488F66, 0x00901ECD, 0x00D891AB, 0x00DB711C, 0x0093FE7A, 0x004B6FD1,
  0x0003E0B7, 0x00B6E338, 0x00FE6C5E, 0x0026FDF5, 0x006E7293, 0x006D9224, 0x00251D42,
  0x00FD8CE9, 0x00B5038F, 0x006CC771, 0x00244817, 0x00FCD9BC, 0x00B456DA, 0x00B7B66D,
  0x00FF390B, 0x0027A8A0, 0x006F27C6, 0x00DA2449, 0x0092AB2F, 0x004A3A84, 0x0002B5E2,
  0x00015555, 0x0049DA33, 0x00914B98, 0x00D9C4FE, 0x00D88EE3, 0x00900185, 0x0048902E,
  0x00001F48, 0x0003FFFF, 0x004B7099, 0x0093E132, 0x00DB6E54, 0x006E6DDB, 0x0026E2BD,
  0x00FE7316, 0x00B6FC70, 0x00B51CC7, 0x00FD93A1, 0x0025020A, 0x006D8D6C, 0x00B44992,
  0x00FCC6F4, 0x0024575F, 0x006CD839, 0x006F388E, 0x0027B7E8, 0x00FF2643, 0x00B7A925,
  0x0002AAAA, 0x004A25CC, 0x0092B467, 0x00DA3B01, 0x00D9DBB6, 0x009154D0, 0x0049C57B,
  0x00014A1D, 0x004B5141, 0x0003DE27, 0x00DB4F8C, 0x0093C0EA, 0x0090205D, 0x00D8AF3B,
  0x00003E90, 0x0048B1F6, 0x00FDB279, 0x00B53D1F, 0x006DACB4, 0x002523D2, 0x0026C365,
  0x006E4C03, 0x00B6DDA8, 0x00FE52CE, 0x00279630, 0x006F1956, 0x00B788FD, 0x00FF079B,
  0x00FCE72C, 0x00B4684A, 0x006CF9E1, 0x00247687, 0x00917508, 0x00D9FA6E, 0x00016BC5,
  0x0049E4A3, 0x004A0414, 0x00028B72, 0x00DA1AD9, 0x009295BF, 0x0093DFA2, 0x00DB50C4,
  0x0003C16F, 0x004B4E09, 0x0048AEBE, 0x000021D8, 0x00D8B073, 0x00903F15, 0x00253C9A,
  0x006DB3FC, 0x00B52257, 0x00FDAD31, 0x00FE4D86, 0x00B6C2E0, 0x006E534B, 0x0026DC2D,
  0x00FF18D3, 0x00B797B5, 0x006F061E, 0x00278978, 0x002469CF, 0x006CE6A9, 0x00B47702,
  0x00FCF864, 0x0049FBEB, 0x0001748D, 0x00D9E526, 0x00916A40, 0x00928AF7, 0x00DA0591,
  0x0002943A, 0x004A1B5C, 0x0096A282, 0x00DE2DE4, 0x0006BC4F, 0x004E3329, 0x004DD39E,
  0x00055CF8, 0x00DDCD53, 0x00954235, 0x002041BA, 0x0068CEDC, 0x00B05F77, 0x00F8D011,
  0x00FB30A6, 0x00B3BFC0, 0x006B2E6B, 0x0023A10D, 0x00FA65F3, 0x00B2EA95, 0x006A7B3E,
  0x0022F458, 0x002114EF, 0x00699B89, 0x00B10A22, 0x00F98544, 0x004C86CB, 0x000409AD,
  0x00DC9806, 0x00941760, 0x0097F7D7, 0x00DF78B1, 0x0007E91A, 0x004F667C, 0x004E2C61,
  0x0006A307, 0x00DE32AC, 0x0096BDCA, 0x00955D7D, 0x00DDD21B, 0x000543B0, 0x004DCCD6,
  0x00F8CF59, 0x00B0403F, 0x0068D194, 0x00205EF2, 0x0023BE45, 0x006B3123, 0x00B3A088,
  0x00FB2FEE, 0x0022EB10, 0x006A6476, 0x00B2F5DD, 0x00FA7ABB, 0x00F99A0C, 0x00B1156A,
  0x006984C1, 0x00210BA7, 0x00940828, 0x00DC874E, 0x000416E5, 0x004C9983, 0x004F7934,
  0x0007F652, 0x00DF67F9, 0x0097E89F, 0x00DDF3C3, 0x00957CA5, 0x004DED0E, 0x00056268,
  0x000682DF, 0x004E0DB9, 0x00969C12, 0x00DE1374, 0x006B10FB, 0x00239F9D, 0x00FB0E36,
  0x00B38150, 0x00B061E7, 0x00F8EE81, 0x00207F2A, 0x0068F04C, 0x00B134B2, 0x00F9BBD4,
  0x00212A7F, 0x0069A519, 0x006A45AE, 0x0022CAC8, 0x00FA5B63, 0x00B2D405, 0x0007D78A,
  0x004F58EC, 0x0097C947, 0x00DF4621, 0x00DCA696, 0x009429F0, 0x004CB85B, 0x0004373D,
  0x00057D20, 0x004DF246, 0x009563ED, 0x00DDEC8B, 0x00DE0C3C, 0x0096835A, 0x004E12F1,
  0x00069D97, 0x00B39E18, 0x00FB117E, 0x002380D5, 0x006B0FB3, 0x0068EF04, 0x00206062,
  0x00F8F1C9, 0x00B07EAF, 0x0069BA51, 0x00213537, 0x00F9A49C, 0x00B12BFA, 0x00B2CB4D,
  0x00FA442B, 0x0022D580, 0x006A5AE6, 0x00DF5969, 0x0097D60F, 0x004F47A4, 0x0007C8C2,
  0x00042875, 0x004CA713, 0x009436B8, 0x00DCB9DE,
};

static const uint32_t T2[256] = {
  0x00000000, 0x00D70983, 0x00555F80, 0x00825603, 0x0051F286, 0x0086FB05, 0x0004AD06,
  0x00D3A485, 0x0059A88B, 0x008EA108, 0x000CF70B, 0x00DBFE88, 0x00085A0D, 0x00DF538E,
  0x005D058D, 0x008A0C0E, 0x00491C91, 0x009E1512, 0x001C4311, 0x00CB4A92, 0x0018EE17,
  0x00CFE794, 0x004DB197, 0x009AB814, 0x0010B41A, 0x00C7BD99, 0x0045EB9A, 0x0092E219,
  0x0041469C, 0x00964F1F, 0x0014191C, 0x00C3109F, 0x006974A4, 0x00BE7D27, 0x003C2B24,
  0x00EB22A7, 0x00388622, 0x00EF8FA1, 0x006DD9A2, 0x00BAD021, 0x0030DC2F, 0x00E7D5AC,
  0x006583AF, 0x00B28A2C, 0x00612EA9, 0x00B6272A, 0x00347129, 0x00E378AA, 0x00206835,
  0x00F761B6, 0x007537B5, 0x00A23E36, 0x00719AB3, 0x00A69330, 0x0024C533, 0x00F3CCB0,
  0x0079C0BE, 0x00AEC93D, 0x002C9F3E, 0x00FB96BD, 0x00283238, 0x00FF3BBB, 0x007D6DB8,
  0x00AA643B, 0x0029A4CE, 0x00FEAD4D, 0x007CFB4E, 0x00ABF2CD, 0x00785648, 0x00AF5FCB,
  0x002D09C8, 0x00FA004B, 0x00700C45, 0x00A705C6, 0x002553C5, 0x00F25A46, 0x0021FEC3,
  0x00F6F740, 0x0074A143, 0x00A3A8C0, 0x0060B85F, 0x00B7B1DC, 0x0035E7DF, 0x00E2EE5C,
  0x00314AD9, 0x00E6435A, 0x00641559, 0x00B31CDA, 0x003910D4, 0x00EE1957, 0x006C4F54,
  0x00BB46D7, 0x0068E252, 0x00BFEBD1, 0x003DBDD2, 0x00EAB451, 0x0040D06A, 0x0097D9E9,
  0x00158FEA, 0x00C28669, 0x001122EC, 0x00C62B6F, 0x00447D6C, 0x009374EF, 0x001978E1,
  0x00CE7162, 0x004C2761, 0x009B2EE2, 0x00488A67, 0x009F83E4, 0x001DD5E7, 0x00CADC64,
  0x0009CCFB, 0x00DEC578, 0x005C937B, 0x008B9AF8, 0x00583E7D, 0x008F37FE, 0x000D61FD,
  0x00DA687E, 0x00506470, 0x00876DF3, 0x00053BF0, 0x00D23273, 0x000196F6, 0x00D69F75,
  0x0054C976, 0x0083C0F5, 0x00A9041B, 0x007E0D98, 0x00FC5B9B, 0x002B5218, 0x00F8F69D,
  0x002FFF1E, 0x00ADA91D, 0x007AA09E, 0x00F0AC90, 0x0027A513, 0x00A5F310, 0x0072FA93,
  0x00A15E16, 0x00765795, 0x00F40196, 0x00230815, 0x00E0188A, 0x00371109, 0x00B5470A,
  0x00624E89, 0x00B1EA0C, 0x0066E38F, 0x00E4B58C, 0x0033BC0F, 0x00B9B001, 0x006EB982,
  0x00ECEF81, 0x003BE602, 0x00E84287, 0x003F4B04, 0x00BD1D07, 0x006A1484, 0x00C070BF,
  0x0017793C, 0x00952F3F, 0x004226BC, 0x00918239, 0x00468BBA, 0x00C4DDB9, 0x0013D43A,
  0x0099D834, 0x004ED1B7, 0x00CC87B4, 0x001B8E37, 0x00C82AB2, 0x001F2331, 0x009D7532,
  0x004A7CB1, 0x00896C2E, 0x005E65AD, 0x00DC33AE, 0x000B3A2D, 0x00D89EA8, 0x000F972B,
  0x008DC128, 0x005AC8AB, 0x00D0C4A5, 0x0007CD26, 0x00859B25, 0x005292A6, 0x00813623,
  0x00563FA0, 0x00D469A3, 0x00036020, 0x0080A0D5, 0x0057A956, 0x00D5FF55, 0x0002F6D6,
  0x00D15253, 0x00065BD0, 0x00840DD3, 0x00530450, 0x00D9085E, 0x000E01DD, 0x008C57DE,
  0x005B5E5D, 0x0088FAD8, 0x005FF35B, 0x00DDA558, 0x000AACDB, 0x00C9BC44, 0x001EB5C7,
  0x009CE3C4, 0x004BEA47, 0x00984EC2, 0x004F4741, 0x00CD1142, 0x001A18C1, 0x009014CF,
  0x00471D4C, 0x00C54B4F, 0x001242CC, 0x00C1E649, 0x0016EFCA, 0x0094B9C9, 0x0043B04A,
  0x00E9D471, 0x003EDDF2, 0x00BC8BF1, 0x006B8272, 0x00B826F7, 0x006F2F74, 0x00ED7977,
  0x003A70F4, 0x00B07CFA, 0x00677579, 0x00E5237A, 0x00322AF9, 0x00E18E7C, 0x003687FF,
  0x00B4D1FC, 0x0063D87F, 0x00A0C8E0, 0x0077C163, 0x00F59760, 0x00229EE3, 0x00F13A66,
  0x002633E5, 0x00A465E6, 0x00736C65, 0x00F9606B, 0x002E69E8, 0x00AC3FEB, 0x007B3668,
  0x00A892ED, 0x007F9B6E, 0x00FDCD6D, 0x002AC4EE,
};

static const uint32_t T3[256] = {
  0x00000000, 0x00520936, 0x00A4126C, 0x00F61B5A, 0x004825D8, 0x001A2CEE, 0x00EC37B4,
  0x00BE3E82, 0x006B0636, 0x00390F00, 0x00CF145A, 0x009D1D6C, 0x002323EE, 0x00712AD8,
  0x00873182, 0x00D538B4, 0x00D60C6C, 0x0084055A, 0x00721E00, 0x00201736, 0x009E29B4,
  0x00CC2082, 0x003A3BD8, 0x006832EE, 0x00BD0A5A, 0x00EF036C, 0x00191836, 0x004B1100,
  0x00F52F82, 0x00A726B4, 0x00513DEE, 0x000334D8, 0x00AC19D8, 0x00FE10EE, 0x00080BB4,
  0x005A0282, 0x00E43C00, 0x00B63536, 0x00402E6C, 0x0012275A, 0x00C71FEE, 0x009516D8,
  0x00630D82, 0x003104B4, 0x008F3A36, 0x00DD3300, 0x002B285A, 0x0079216C, 0x007A15B4,
  0x00281C82, 0x00DE07D8, 0x008C0EEE, 0x0032306C, 0x0060395A, 0x00962200, 0x00C42B36,
  0x00111382, 0x00431AB4, 0x00B501EE, 0x00E708D8, 0x0059365A, 0x000B3F6C, 0x00FD2436,
  0x00AF2D00, 0x00A37F36, 0x00F17600, 0x00076D5A, 0x0055646C, 0x00EB5AEE, 0x00B953D8,
  0x004F4882, 0x001D41B4, 0x00C87900, 0x009A7036, 0x006C6B6C, 0x003E625A, 0x00805CD8,
  0x00D255EE, 0x00244EB4, 0x00764782, 0x0075735A, 0x00277A6C, 0x00D16136, 0x00836800,
  0x003D5682, 0x006F5FB4, 0x009944EE, 0x00CB4DD8, 0x001E756C, 0x004C7C5A, 0x00BA6700,
  0x00E86E36, 0x005650B4, 0x00045982, 0x00F242D8, 0x00A04BEE, 0x000F66EE, 0x005D6FD8,
  0x00AB7482, 0x00F97DB4, 0x00474336, 0x00154A00, 0x00E3515A, 0x00B1586C, 0x006460D8,
  0x003669EE, 0x00C072B4, 0x00927B82, 0x002C4500, 0x007E4C36, 0x0088576C, 0x00DA5E5A,
  0x00D96A82, 0x008B63B4, 0x007D78EE, 0x002F71D8, 0x00914F5A, 0x00C3466C, 0x00355D36,
  0x00675400, 0x00B26CB4, 0x00E06582, 0x00167ED8, 0x004477EE, 0x00FA496C, 0x00A8405A,
  0x005E5B00, 0x000C5236, 0x0046FF6C, 0x0014F65A, 0x00E2ED00, 0x00B0E436, 0x000EDAB4,
  0x005CD382, 0x00AAC8D8, 0x00F8C1EE, 0x002DF95A, 0x007FF06C, 0x0089EB36, 0x00DBE200,
  0x0065DC82, 0x0037D5B4, 0x00C1CEEE, 0x0093C7D8, 0x0090F300, 0x00C2FA36, 0x0034E16C,
  0x0066E85A, 0x00D8D6D8, 0x008ADFEE, 0x007CC4B4, 0x002ECD82, 0x00FBF536, 0x00A9FC00,
  0x005FE75A, 0x000DEE6C, 0x00B3D0EE, 0x00E1D9D8, 0x0017C282, 0x0045CBB4, 0x00EAE6B4,
  0x00B8EF82, 0x004EF4D8, 0x001CFDEE, 0x00A2C36C, 0x00F0CA5A, 0x0006D100, 0x0054D836,
  0x0081E082, 0x00D3E9B4, 0x0025F2EE, 0x0077FBD8, 0x00C9C55A, 0x009BCC6C, 0x006DD736,
  0x003FDE00, 0x003CEAD8, 0x006EE3EE, 0x0098F8B4, 0x00CAF182, 0x0074CF00, 0x0026C636,
  0x00D0DD6C, 0x0082D45A, 0x0057ECEE, 0x0005E5D8, 0x00F3FE82, 0x00A1F7B4, 0x001FC936,
  0x004DC000, 0x00BBDB5A, 0x00E9D26C, 0x00E5805A, 0x00B7896C, 0x00419236, 0x00139B00,
  0x00ADA582, 0x00FFACB4, 0x0009B7EE, 0x005BBED8, 0x008E866C, 0x00DC8F5A, 0x002A9400,
  0x00789D36, 0x00C6A3B4, 0x0094AA82, 0x0062B1D8, 0x0030B8EE, 0x00338C36, 0x00618500,
  0x00979E5A, 0x00C5976C, 0x007BA9EE, 0x0029A0D8, 0x00DFBB82, 0x008DB2B4, 0x00588A00,
  0x000A8336, 0x00FC986C, 0x00AE915A, 0x0010AFD8, 0x0042A6EE, 0x00B4BDB4, 0x00E6B482,
  0x00499982, 0x001B90B4, 0x00ED8BEE, 0x00BF82D8, 0x0001BC5A, 0x0053B56C, 0x00A5AE36,
  0x00F7A700, 0x00229FB4, 0x00709682, 0x00868DD8, 0x00D484EE, 0x006ABA6C, 0x0038B35A,
  0x00CEA800, 0x009CA136, 0x009F95EE, 0x00CD9CD8, 0x003B8782, 0x00698EB4, 0x00D7B036,
  0x0085B900, 0x0073A25A, 0x0021AB6C, 0x00F493D8, 0x00A69AEE, 0x005081B4, 0x00028882,
  0x00BCB600, 0x00EEBF36, 0x0018A46C, 0x004AAD5A};

#define CRC24_FAST_INIT 0xce04b7L

static inline uint32_t
process8(uint32_t crc, uint8_t data)
{
    return (crc >> 8) ^ T0[(crc & 0xff) ^ data];
}

/*
 * Process 4 bytes in one go
 */
static inline uint32_t
process32(uint32_t crc, uint32_t word)
{
    crc ^= word;
    crc = T3[crc & 0xff] ^ T2[((crc >> 8) & 0xff)] ^ T1[((crc >> 16) & 0xff)] ^
          T0[(crc >> 24) & 0xff];
    return crc;
}

static uint32_t
crc24_update(uint32_t crc, const uint8_t *in, size_t length)
{
    uint32_t d0, d1, d2, d3;

    while (length >= 16) {
        LOAD32LE(d0, &in[0]);
        LOAD32LE(d1, &in[4]);
        LOAD32LE(d2, &in[8]);
        LOAD32LE(d3, &in[12]);

        crc = process32(crc, d0);
        crc = process32(crc, d1);
        crc = process32(crc, d2);
        crc = process32(crc, d3);

        in += 16;
        length -= 16;
    }

    while (length--) {
        crc = process8(crc, *in++);
    }

    return crc & 0xffffff;
}

/* Swap endianness of 32-bit value */
#if defined(__GNUC__) || defined(__clang__)
#define BSWAP32(x) __builtin_bswap32(x)
#else
#define BSWAP32(x)                                                            \
    ((x & 0x000000FF) << 24 | (x & 0x0000FF00) << 8 | (x & 0x00FF0000) >> 8 | \
     (x & 0xFF000000) >> 24)
#endif

static uint32_t
crc24_final(uint32_t crc)
{
    return (BSWAP32(crc) >> 8);
}

bool
pgp_crc24_create(pgp_hash_t *hash)
{
    try {
        hash->handle = new uint32_t(CRC24_FAST_INIT);
    } catch (const std::exception &e) {
        RNP_LOG("Allocation failed: %s", e.what());
        return false;
    }
    hash->_alg = PGP_HASH_UNKNOWN;
    hash->_output_len = 3;
    return true;
}

bool
pgp_crc24_copy(pgp_hash_t *dst, const pgp_hash_t *src)
{
    try {
        dst->handle = new uint32_t(*static_cast<uint32_t *>(src->handle));
    } catch (const std::exception &e) {
        RNP_LOG("Allocation failed: %s", e.what());
        return false;
    }
    dst->_alg = PGP_HASH_UNKNOWN;
    dst->_output_len = 3;
    return true;
}

int
pgp_crc24_add(pgp_hash_t *hash, const void *buf, size_t len)
{
    uint32_t *state = static_cast<uint32_t *>(hash->handle);
    *state = crc24_update(*state, static_cast<const uint8_t *>(buf), len);
    return 0;
}

size_t
pgp_crc24_finish(pgp_hash_t *hash, uint8_t *output)
{
    uint32_t crc_fin = crc24_final(*static_cast<uint32_t *>(hash->handle));
    delete static_cast<uint32_t *>(hash->handle);
    hash->handle = NULL;
    hash->_output_len = 0;
    if (output) {
        output[0] = (crc_fin >> 16) & 0xff;
        output[1] = (crc_fin >> 8) & 0xff;
        output[2] = crc_fin & 0xff;
    }
    return 3;
}
