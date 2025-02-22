#include <unity.h>
#include <vector>
#include "detector.h"

std::vector<int16_t> test_input = {
183,   178,   178,   175,   175,   177,   176,   178,   179,   174,   172,   177,   178,   172,   171,   170,  // b[0]
168,   169,   168,   165,   166,   168,   169,   170,   171,   172,   173,   173,   172,   170,   170,   171,  // b[16]
172,   169,   166,   168,   170,   170,   169,   167,   169,   167,   169,   173,   170,   169,   170,   168,  // b[32]
166,   167,   166,   166,   166,   166,   167,   167,   168,   167,   168,   168,   166,   164,   167,   168,  // b[48]
166,   165,   167,   169,   169,   166,   166,   165,   166,   168,   166,   167,   168,   167,   165,   168,  // b[64]
169,   168,   170,   167,   165,   168,   168,   168,   168,   167,   170,   168,   166,   169,   169,   168,  // b[80]
166,   167,   169,   167,   168,   168,   167,   168,   165,   163,   166,   167,   167,   166,   165,   168,  // b[96]
170,   166,   167,   169,   166,   167,   168,   168,   170,   168,   167,   168,   167,   167,   168,   167,  // b[112]
166,   166,   166,   167,   167,   166,   167,   168,   169,   169,   168,   168,   167,   168,   167,   166,  // b[128]
169,   167,   166,   168,   166,   166,   168,   167,   167,   168,   167,   168,   169,   166,   166,   168,  // b[144]
167,   165,   167,   167,   166,   167,   167,   167,   167,   167,   166,   168,   168,   165,   166,   171,  // b[160]
169,   167,   169,   167,   167,   167,   166,   168,   169,   170,   173,   172,   171,   170,   169,   172,  // b[176]
171,   170,   169,   169,   170,   173,   174,   170,   169,   170,   168,   170,   173,   171,   171,   171,  // b[192]
170,   171,   171,   170,   165,   164,   167,   168,   167,   168,   166,   166,   171,   169,   164,   166,  // b[208]
169,   168,   165,   167,   169,   166,   169,   173,   174,   178,   183,   194,   211,   213,   205,   208,  // b[224]
209,   208,   214,   218,   224,   229,   219,   206,   197,   193,   195,   196,   196,   212,   234,   232,  // b[240]
223,   206,   183,   190,   204,   199,   194,   186,   173,   170,   172,   170,   166,   164,   166,   168,  // b[256]
170,   168,   164,   168,   170,   167,   166,   168,   168,   168,   170,   167,   165,   167,   167,   167,  // b[272]
169,   168,   167,   168,   168,   166,   166,   169,   166,   164,   168,   168,   166,   167,   166,   166,  // b[288]
167,   166,   167,   168,   166,   166,   167,   169,   168,   166,   168,   170,   169,   166,   166,   167,  // b[304]
167,   167,   168,   167,   165,   167,   166,   166,   168,   166,   166,   169,   168,   166,   166,   168,  // b[320]
167,   167,   169,   167,   166,   169,   167,   165,   168,   167,   167,   167,   167,   167,   168,   167,  // b[336]
165,   165,   167,   168,   166,   166,   169,   167,   165,   167,   167,   167,   168,   164,   165,   168,  // b[352]
167,   167,   167,   168,   167,   164,   167,   169,   167,   168,   167,   168,   170,   168,   167,   167,  // b[368]
166,   167,   168,   166,   168,   171,   169,   167,   166,   166,   168,   167,   167,   166,   166,   168,  // b[384]
168,   167,   168,   167,   168,   170,   168,   168,   169,   166,   165,   165,   168,   170,   166,   167,  // b[400]
169,   165,   166,   168,   165,   167,   168,   167,   169,   168,   166,   169,   170,   166,   167,   170,  // b[416]
172,   171,   168,   168,   171,   170,   169,   168,   167,   168,   169,   169,   171,   170,   168,   171,  // b[432]
172,   169,   167,   166,   168,   169,   167,   166,   166,   167,   169,   170,   170,   167,   166,   168,  // b[448]
167,   167,   169,   170,   168,   168,   169,   169,   167,   165,   166,   167,   167,   166,   167,   168,  // b[464]
167,   168,   168,   167,   167,   165,   165,   166,   168,   169,   168,   167,   167,   167,   167,   168,  // b[480]
167,   167,   166,   166,   166,   167,   168,   166,   166,   167,   167,   166,   167,   169,   167,   165,  // b[496]
168,   168,   166,   166,   169,   168,   166,   167,   166,   166,   169,   168,   166,   165,   166,   169,  // b[512]
167,   165,   166,   167,   166,   167,   170,   169,   166,   165,   167,   169,   166,   164,   165,   169,  // b[528]
170,   165,   166,   169,   169,   166,   167,   167,   166,   166,   166,   169,   169,   166,   167,   169,  // b[544]
167,   164,   164,   168,   169,   167,   167,   165,   166,   168,   168,   167,   167,   168,   168,   166,  // b[560]
166,   168,   168,   167,   169,   167,   166,   168,   169,   167,   166,   167,   168,   167,   166,   167,  // b[576]
168,   166,   167,   167,   168,   170,   167,   166,   168,   168,   166,   166,   166,   167,   168,   168,  // b[592]
167,   168,   167,   167,   169,   168,   166,   169,   170,   167,   166,   167,   166,   168,   168,   166,  // b[608]
167,   166,   167,   169,   167,   168,   167,   166,   169,   167,   165,   167,   168,   167,   165,   168,  // b[624]
169,   168,   168,   165,   165,   168,   166,   166,   170,   170,   165,   167,   170,   166,   165,   168,  // b[640]
167,   166,   167,   167,   167,   167,   169,   168,   166,   168,   168,   166,   167,   167,   166,   167,  // b[656]
166,   165,   167,   169,   168,   167,   167,   166,   168,   167,   165,   172,   175,   170,   170,   173,  // b[672]
172,   170,   174,   175,   173,   171,   170,   171,   171,   169,   169,   173,   176,   175,   173,   171,  // b[688]
170,   170,   171,   171,   172,   170,   168,   168,   167,   166,   168,   169,   168,   167,   166,   166,  // b[704]
166,   166,   167,   168,   167,   166,   166,   167,   169,   168,   167,   166,   168,   167,   164,   167,  // b[720]
171,   168,   168,   169,   167,   167,   167,   168,   169,   168,   168,   168,   166,   167,   169,   167,  // b[736]
167,   168,   167,   166,   168,   169,   167,   169,   169,   167,   167,   167,   165,   167,   170,   167,  // b[752]
164,   166,   170,   168,   166,   167,   166,   166,   168,   168,   168,   168,   168,   169,   174,   182,  // b[768]
185,   211,   276,   293,   313,   409,   396,   285,   226,   202,   272,   370,   357,   358,   382,   331,  // b[784]
287,   258,   248,   308,   392,   456,   444,   332,   267,   364,   434,   348,   271,   270,   266,   228,  // b[800]
187,   171,   166,   166,   170,   169,   165,   165,   166,   167,   168,   168,   169,   167,   166,   165,  // b[816]
165,   167,   168,   168,   169,   168,   166,   168,   167,   167,   169,   168,   168,   169,   167,   166,  // b[832]
168,   169,   169,   168,   167,   166,   166,   166,   166,   168,   167,   165,   166,   168,   166,   167,  // b[848]
166,   166,   168,   168,   165,   165,   166,   166,   166,   167,   167,   167,   167,   169,   169,   167,  // b[864]
167,   166,   166,   168,   167,   166,   168,   169,   167,   167,   166,   166,   169,   168,   168,   168,  // b[880]
167,   166,   167,   167,   166,   166,   168,   169,   168,   167,   167,   167,   166,   166,   168,   169,  // b[896]
167,   166,   166,   166,   168,   169,   169,   169,   167,   166,   166,   166,   165,   167,   168,   167,  // b[912]
167,   167,   167,   166,   166,   167,   168,   167,   167,   168,   168,   167,   166,   167,   167,   167,  // b[928]
167,   168,   168,   168,   166,   164,   166,   170,   168,   167,   167,   167,   167,   167,   167,   169,  // b[944]
170,   168,   165,   166,   168,   165,   165,   168,   168,   168,   168,   167,   166,   166,   169,   170,  // b[960]
168,   167,   168,   166,   165,   167,   165,   164,   168,   168,   165,   167,   168,   165,   165,   168,  // b[976]
168,   166,   166,   166,   166,   167,   168,   168,   167,   166,   167,   169,   169,   166,   166,   170,  // b[992]
169,   166,   168,   168,   166,   170,   174,   172,   171,   169,   168,   171,   172,   172,   173,   171,  // b[1008]
172,   175,   171,   169,   170,   170,   170,   170,   169,   170,   170,   168,   169,   169,   166,   170,  // b[1024]
173,   169,   166,   167,   167,   166,   167,   167,   169,   166,   166,   169,   167,   166,   168,   166,  // b[1040]
166,   168,   169,   168,   167,   168,   167,   167,   168,   167,   166,   168,   168,   167,   168,   167,  // b[1056]
166,   168,   167,   166,   167,   168,   168,   166,   167,   167,   166,   167,   168,   166,   167,   168,  // b[1072]
167,   167,   167,   164,   166,   168,   166,   167,   169,   167,   166,   167,   166,   166,   168,   170,  // b[1088]
168,   166,   167,   169,   167,   166,   168,   168,   168,   167,   167,   166,   166,   169,   167,   166,  // b[1104]
167,   167,   167,   169,   168,   167,   168,   168,   167,   167,   168,   165,   165,   167,   166,   167,  // b[1120]
168,   166,   166,   168,   167,   167,   167,   166,   167,   167,   166,   169,   168,   165,   165,   168,  // b[1136]
167,   166,   167,   167,   168,   168,   167,   166,   166,   167,   168,   167,   167,   167,   166,   166,  // b[1152]
168,   167,   168,   168,   167,   167,   167,   168,   166,   166,   167,   170,   170,   167,   166,   167,  // b[1168]
169,   168,   166,   166,   166,   167,   168,   169,   168,   168,   168,   168,   168,   168,   169,   168,  // b[1184]
167,   167,   168,   169,   170,   171,   171,   170,   168,   168,   168,   169,   168,   165,   168,   171,  // b[1200]
170,   169,   168,   169,   169,   167,   167,   168,   169,   169,   168,   166,   166,   167,   167,   169,  // b[1216]
168,   166,   168,   168,   168,   166,   166,   170,   168,   164,   166,   168,   167,   166,   168,   166,  // b[1232]
166,   168,   167,   167,   167,   164,   165,   168,   169,   167,   165,   166,   165,   164,   165,   166,  // b[1248]
167,   167,   168,   169,   167,   166,   167,   167,   167,   167,   167,   167,   168,   168,   168,   167,  // b[1264]
165,   166,   169,   166,   165,   167,   168,   167,   167,   166,   166,   166,   169,   168,   167,   168,  // b[1280]
165,   164,   167,   168,   168,   169,   168,   167,   168,   167,   167,   168,   168,   167,   167,   168,  // b[1296]
168,   167,   167,   167,   166,   165,   167,   166,   167,   168,   167,   167,   166,   167,   168,   167,  // b[1312]
168,   167,   166,   168,   168,   166,   166,   165,   166,   168,   167,   168,   167,   168,   169,   166,  // b[1328]
167,   168,   167,   167,   168,   168,   168,   167,   166,   166,   167,   167,   167,   168,   169,   168,  // b[1344]
167,   166,   167,   168,   167,   166,   166,   167,   166,   167,   168,   168,   166,   166,   168,   167,  // b[1360]
166,   168,   167,   166,   167,   169,   169,   168,   168,   168,   168,   167,   167,   168,   168,   167,  // b[1376]
167,   166,   168,   168,   166,   166,   166,   166,   167,   168,   167,   167,   168,   167,   166,   168,  // b[1392]
167,   167,   168,   166,   165,   167,   168,   168,   168,   166,   166,   165,   166,   169,   169,   167,  // b[1408]
167,   167,   169,   169,   167,   167,   165,   165,   169,   169,   167,   167,   167,   168,   168,   166,  // b[1424]
164,   165,   168,   168,   168,   170,   168,   165,   166,   168,   168,   165,   166,   167,   167,   167,  // b[1440]
167,   168,   167,   166,   165,   166,   168,   166,   166,   166,   165,   167,   167,   167,   166,   165,  // b[1456]
167,   167,   167,   169,   168,   167,   168,   167,   168,   168,   168,   168,   168,   167,   166,   165,  // b[1472]
166,   169,   167,   167,   169,   167,   168,   169,   167,   166,   166,   167,   168,   168,   168,   167,  // b[1488]
167,   167,   168,   169,   167,   164,   167,   168,   164,   166,   169,   167,   168,   168,   168,   168,  // b[1504]
167,   167,   168,   169,   169,   168,   166,   166,   168,   169,   168,   165,   165,   166,   166,   168,  // b[1520]
168,   168,   167,   168,   170,   167,   166,   166,   166,   168,   168,   166,   168,   168,   167,   169,  // b[1536]
168,   166,   167,   167,   166,   167,   169,   168,   166,   168,   168,   167,   169,   170,   170,   169,  // b[1552]
168,   167,   167,   166,   169,   171,   166,   165,   167,   167,   169,   169,   165,   164,   167,   169,  // b[1568]
168,   166,   166,   168,   168,   167,   166,   168,   168,   166,   165,   167,   168,   166,   167,   169,  // b[1584]
166,   166,   168,   167,   168,   168,   168,   168,   166,   166,   167,   167,   168,   166,   167,   167,  // b[1600]
165,   167,   170,   168,   165,   167,   168,   166,   168,   167,   165,   169,   169,   165,   168,   169,  // b[1616]
167,   168,   167,   167,   168,   170,   167,   166,   168,   169,   170,   168,   165,   168,   168,   167,  // b[1632]
167,   166,   167,   167,   166,   165,   164,   166,   168,   168,   167,   165,   167,   168,   166,   168,  // b[1648]
170,   168,   164,   164,   167,   166,   168,   168,   164,   166,   170,   170,   168,   165,   166,   166,  // b[1664]
164,   167,   169,   167,   166,   167,   167,   165,   167,   169,   167,   168,   168,   168,   168,   168,  // b[1680]
168,   169,   168,   167,   167,   168,   169,   166,   165,   167,   167,   167,   168,   167,   166,   167,  // b[1696]
167,   166,   166,   166,   168,   169,   167,   166,   167,   166,   166,   169,   169,   167,   166,   167,  // b[1712]
167,   168,   168,   169,   170,   169,   168,   168,   167,   169,   168,   167,   168,   166,   166,   169,  // b[1728]
166,   163,   167,   167,   168,   169,   167,   169,   167,   166,   169,   167,   166,   167,   167,   168,  // b[1744]
168,   167,   167,   167,   169,   171,   169,   167,   169,   169,   167,   167,   167,   167,   169,   168,  // b[1760]
168,   167,   167,   168,   168,   170,   173,   169,   166,   167,   168,   168,   166,   166,   168,   167,  // b[1776]
167,   168,   166,   166,   170,   172,   169,   166,   165,   166,   167,   168,   167,   167,   167,   167,  // b[1792]
167,   166,   166,   169,   169,   168,   166,   167,   169,   169,   168,   167,   167,   167,   164,   167,  // b[1808]
169,   168,   169,   174,   178,   173,   170,   170,   167,   172,   180,   181,   182,   183,   175,   172,  // b[1824]
182,   194,   198,   189,   179,   174,   167,   168,   181,   185,   182,   188,   188,   174,   169,   168,  // b[1840]
166,   168,   167,   164,   167,   171,   168,   166,   168,   167,   167,   168,   169,   170,   168,   166,  // b[1856]
167,   168,   167,   166,   168,   168,   166,   167,   169,   167,   167,   168,   168,   170,   168,   167,  // b[1872]
168,   168,   167,   166,   166,   168,   168,   167,   169,   167,   166,   170,   169,   166,   167,   167,  // b[1888]
167,   167,   166,   165,   164,   165,   169,   167,   166,   167,   167,   167,   168,   166,   165,   165,  // b[1904]
169,   170,   166,   165,   167,   167,   167,   167,   165,   166,   168,   169,   167,   166,   169,   169,  // b[1920]
166,   168,   168,   166,   167,   167,   169,   170,   167,   168,   168,   170,   175,   178,   194,   205,  // b[1936]
198,   206,   226,   220,   206,   202,   194,   185,   182,   182,   179,   174,   182,   205,   220,   222,  // b[1952]
235,   234,   213,   210,   209,   212,   214,   191,   172,   167,   165,   168,   168,   168,   169,   169,  // b[1968]
167,   166,   166,   167,   166,   165,   166,   167,   168,   168,   166,   167,   169,   168,   169,   167,  // b[1984]
167,   168,   166,   164,   167,   169,   167,   167,   167,   166,   167,   168,   168,   165,   166,   168,  // b[2000]
167,   166,   169,   168,   166,   168,   167,   167,   167,   168,   168,   168,   167,   166,   168,   169,  // b[2016]
167,   168,   168,   168,   167,   166,   166,   167,   165,   167,   169,   166,   167,   170,   168,   166,  // b[2032]
167,   168,   168,   168,   167,   167,   168,   168,   168,   167,   167,   169,   168,   167,   166,   167,  // b[2048]
167,   165,   167,   168,   165,   167,   168,   165,   167,   168,   165,   165,   167,   166,   165,   168,  // b[2064]
169,   166,   166,   168,   168,   166,   165,   167,   166,   164,   166,   167,   167,   167,   167,   168,  // b[2080]
171,   170,   166,   166,   168,   168,   169,   170,   168,   165,   165,   168,   169,   167,   166,   166,  // b[2096]
168,   167,   166,   164,   167,   170,   168,   167,   168,   166,   165,   168,   168,   167,   168,   168,  // b[2112]
167,   167,   168,   167,   167,   168,   168,   167,   166,   168,   166,   167,   169,   166,   167,   170,  // b[2128]
166,   165,   168,   166,   167,   169,   169,   169,   166,   167,   168,   167,   167,   165,   165,   167,  // b[2144]
169,   168,   166,   167,   168,   167,   168,   168,   166,   166,   167,   168,   168,   167,   165,   166,  // b[2160]
167,   166,   169,   170,   165,   164,   167,   168,   167,   166,   167,   168,   167,   167,   167,   168,  // b[2176]
169,   167,   165,   169,   166,   165,   169,   170,   167,   166,   166,   167,   168,   167,   165,   166,  // b[2192]
166,   167,   169,   169,   168,   167,   167,   168,   168,   169,   168,   169,   167,   164,   166,   168,  // b[2208]
166,   168,   168,   167,   167,   167,   167,   165,   166,   167,   166,   168,   167,   167,   170,   168,  // b[2224]
165,   167,   170,   169,   167,   167,   165,   166,   168,   167,   167,   168,   168,   168,   165,   165,  // b[2240]
168,   169,   167,   167,   167,   165,   165,   168,   168,   166,   168,   168,   166,   167,   168,   166,  // b[2256]
168,   170,   166,   166,   168,   167,   167,   168,   167,   167,   168,   169,   166,   167,   169,   167,  // b[2272]
167,   167,   166,   166,   166,   167,   170,   168,   167,   169,   171,   170,   170,   170,   171,   174,  // b[2288]
177,   181,   186,   183,   182,   188,   184,   173,   173,   183,   184,   182,   186,   184,   181,   181,  // b[2304]
175,   171,   169,   166,   167,   167,   166,   166,   167,   167,   166,   166,   168,   168,   168,   169,  // b[2320]
165,   165,   167,   166,   167,   168,   167,   167,   167,   165,   165,   170,   170,   166,   168,   167,  // b[2336]
166,   168,   165,   166,   169,   167,   167,   169,   167,   166,   169,   166,   166,   170,   167,   165,  // b[2352]
167,   166,   166,   167,   167,   167,   168,   167,   166,   166,   168,   168,   167,   168,   169,   169,  // b[2368]
168,   168,   167,   166,   168,   169,   169,   168,   165,   166,   168,   166,   166,   168,   168,   169,  // b[2384]
168,   166,   168,   169,   167,   167,   166,   167,   169,   168,   167,   167,   167,   168,   168,   166,  // b[2400]
168,   168,   166,   167,   168,   166,   167,   168,   167,   168,   169,   167,   167,   166,   165,   167,  // b[2416]
167,   167,   170,   169,   166,   166,   168,   167,   167,   168,   169,   168,   167,   169,   170,   166,  // b[2432]
166,   171,   171,   165,   165,   167,   166,   167,   167,   165,   167,   171,   169,   166,   166,   168,  // b[2448]
171,   170,   165,   164,   168,   169,   165,   165,   166,   167,   168,   169,   169,   167,   168,   169,  // b[2464]
168,   169,   169,   167,   165,   167,   168,   168,   167,   168,   170,   169,   168,   167,   167,   169,  // b[2480]
168,   167,   167,   166,   166,   169,   166,   164,   167,   168,   167,   168,   168,   166,   166,   166,  // b[2496]
166,   168,   170,   168,   166,   166,   168,   166,   165,   166,   168,   168,   168,   167,   165,   167,  // b[2512]
169,   168,   167,   167,   168,   168,   167,   166,   167,   167,   165,   166,   169,   168,   167,   166,  // b[2528]
165,   166,   167,   167,   167,   167,   168,   167,   167,   169,   168,   167,   168,   168,   168,   165,  // b[2544]
165,   168,   169,   167,   166,   164,   166,   168,   168,   166,   166,   169,   169,   166,   166,   167,  // b[2560]
167,   169,   169,   166,   168,   168,   168,   169,   166,   165,   168,   168,   167,   168,   168,   167,  // b[2576]
169,   169,   165,   166,   169,   169,   168,   166,   166,   167,   165,   164,   167,   166,   166,   166,  // b[2592]
165,   168,   171,   168,   165,   165,   167,   169,   167,   167,   168,   166,   164,   167,   170,   168,  // b[2608]
166,   167,   168,   166,   165,   167,   166,   167,   168,   167,   167,   167,   168,   166,   167,   169,  // b[2624]
169,   168,   166,   165,   166,   167,   166,   166,   166,   167,   166,   165,   169,   169,   167,   166,  // b[2640]
171,   177,   173,   171,   178,   180,   181,   185,   183,   177,   176,   174,   174,   182,   184,   178,  // b[2656]
181,   187,   183,   177,   176,   178,   183,   186,   180,   173,   171,   170,   168,   166,   167,   168,  // b[2672]
167,   166,   169,   168,   167,   169,   167,   165,   167,   167,   167,   168,   167,   167,   166,   166,  // b[2688]
170,   169,   167,   167,   168,   167,   166,   167,   166,   166,   169,   168,   166,   168,   167,   166,  // b[2704]
165,   166,   168,   168,   167,   168,   169,   167,   167,   167,   166,   167,   168,   165,   166,   169,  // b[2720]
167,   166,   168,   167,   167,   169,   167,   166,   167,   168,   167,   167,   168,   167,   166,   169,  // b[2736]
167,   165,   168,   169,   168,   167,   165,   166,   167,   166,   167,   169,   170,   166,   165,   169,  // b[2752]
168,   167,   167,   168,   169,   167,   168,   169,   170,   172,   171,   171,   175,   190,   212,   220,  // b[2768]
204,   182,   172,   170,   170,   169,   170,   173,   174,   181,   189,   194,   192,   183,   182,   181,  // b[2784]
172,   170,   174,   179,   184,   176,   165,   166,   165,   166,   170,   166,   166,   169,   168,   167,  // b[2800]
167,   167,   166,   167,   168,   167,   166,   167,   169,   167,   167,   169,   168,   168,   167,   166,  // b[2816]
166,   168,   168,   166,   167,   169,   168,   166,   169,   168,   167,   167,   167,   168,   168,   167,  // b[2832]
167,   167,   167,   168,   168,   169,   167,   165,   169,   169,   167,   168,   169,   168,   166,   164,  // b[2848]
166,   167,   167,   169,   168,   167,   169,   169,   167,   167,   167,   166,   165,   166,   168,   167,  // b[2864]
166,   167,   167,   166,   167,   168,   166,   166,   170,   168,   167,   170,   168,   168,   168,   166,  // b[2880]
166,   167,   169,   169,   167,   167,   167,   168,   169,   168,   169,   167,   165,   166,   167,   168,  // b[2896]
168,   167,   167,   165,   165,   168,   168,   166,   167,   168,   167,   166,   166,   166,   167,   168,  // b[2912]
168,   168,   167,   165,   166,   166,   166,   168,   168,   167,   168,   168,   167,   166,   167,   169,  // b[2928]
168,   167,   165,   167,   168,   169,   169,   168,   167,   168,   167,   166,   165,   166,   167,   167,  // b[2944]
168,   167,   166,   167,   165,   166,   168,   166,   165,   168,   168,   165,   165,   169,   169,   167,  // b[2960]
167,   167,   168,   168,   166,   166,   166,   167,   167,   167,   168,   166,   165,   168,   167,   166,  // b[2976]
168,   167,   166,   168,   169,   167,   167,   167,   166,   166,   167,   168,   169,   168,   167,   167,  // b[2992]
168,   167,   164,   164,   167,   167,   168,   168,   167,   168,   169,   169,   169,   169,   165,   164,  // b[3008]
167,   167,   167,   166,   166,   166,   168,   168,   165,   166,   169,   165,   164,   170,   169,   166,  // b[3024]
168,   167,   165,   166,   167,   167,   167,   166,   167,   167,   165,   166,   167,   166,   168,   168,  // b[3040]
167,   167,   168,   166,   165,   168,   168,   166,   169,   169,   166,   168,   168,   167,   168,   169,  // b[3056]
169,   166,   166,   169,   166,   166,   167,   167,   167,   167,   166,   166,   167,   167,   166,   167,  // b[3072]
167,   166,   166,   167,   168,   166,   167,   169,   166,   166,   168,   166,   166,   167,   168,   168,  // b[3088]
166,   168,   169,   167,   165,   167,   167,   165,   165,   165,   167,   170,   168,   166,   166,   166,  // b[3104]
170,   171,   168,   167,   168,   168,   168,   167,   166,   166,   167,   166,   166,   166,   166,   167,  // b[3120]
166,   166,   167,   168,   167,   166,   167,   169,   167,   166,   168,   168,   166,   165,   167,   168,  // b[3136]
165,   166,   168,   167,   169,   169,   166,   165,   165,   166,   168,   168,   168,   166,   164,   165,  // b[3152]
168,   168,   167,   167,   166,   167,   169,   167,   169,   172,   170,   171,   175,   173,   174,   176,  // b[3168]
173,   171,   171,   171,   175,   177,   176,   177,   177,   175,   172,   174,   180,   177,   173,   174,  // b[3184]
};

using namespace detector;

void setUp(void) {
}

void tearDown(void) {
}

void test_detector() {
    auto data = test_input.data();
    auto data_len = test_input.size();

    int32_t sum = 0;
    for (size_t n = 0; n < data_len; n++)
        sum += data[n];

    ObjectDetector det(8, 10);
    // Preinitialize with average so that it does not converge long to zero offset
    det.preinit(sum/data_len);
    
    // Write in chunks to make sure write() is working well with multiple calls
    int rem_len = data_len;
    auto rem_data{data};
    while (rem_len) { 
        size_t to_write = std::min(rem_len, 32);
        det.write(rem_data, to_write);
        rem_data += to_write;
        rem_len -= to_write;
    }

    // ['(233..264)', '(783..817)', '(1950..1963)', '(1964..1976)']

    TEST_ASSERT_EQUAL(4, det.results.size());

    TEST_ASSERT_EQUAL(233, det.results[0].start);
    TEST_ASSERT_EQUAL(264 - 233, det.results[0].len);
    TEST_ASSERT_EQUAL(783, det.results[1].start);
    TEST_ASSERT_EQUAL(817 - 783, det.results[1].len);
    TEST_ASSERT_EQUAL(1950, det.results[2].start);
    TEST_ASSERT_EQUAL(1963 - 1950, det.results[2].len);
    TEST_ASSERT_EQUAL(1964, det.results[3].start);
    TEST_ASSERT_EQUAL(1976 - 1964, det.results[3].len);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_detector);
 
    UNITY_END();
}
