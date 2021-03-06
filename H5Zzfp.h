/*
    Copyright (c) 2016, Lawrence Livermore National Security, LLC.
        Produced at the Lawrence Livermore National Laboratory
           Written by Mark C. Miller, miller86@llnl.gov
               LLNL-CODE-707197 All rights reserved.

This file  is part  of H5Z-ZFP.  For details, see
https://github.com/LLNL/H5Z-ZFP.  Please  also  read  the   Additional
BSD Notice.

Redistribution and  use in  source and binary  forms, with  or without
modification, are permitted provided that the following conditions are
met:

* Redistributions  of  source code  must  retain  the above  copyright
  notice, this list of conditions and the disclaimer below.

* Redistributions in  binary form  must reproduce the  above copyright
  notice, this list of conditions  and the disclaimer (as noted below)
  in  the  documentation  and/or  other materials  provided  with  the
  distribution.

* Neither the name of the  LLNS/LLNL nor the names of its contributors
  may  be  used to  endorse  or  promote  products derived  from  this
  software without specific prior written permission.

THIS SOFTWARE  IS PROVIDED BY  THE COPYRIGHT HOLDERS  AND CONTRIBUTORS
"AS  IS" AND  ANY EXPRESS  OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT
LIMITED TO, THE IMPLIED  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A  PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN  NO  EVENT SHALL  LAWRENCE
LIVERMORE  NATIONAL SECURITY, LLC,  THE U.S.  DEPARTMENT OF  ENERGY OR
CONTRIBUTORS BE LIABLE FOR  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR  CONSEQUENTIAL DAMAGES  (INCLUDING, BUT NOT  LIMITED TO,
PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS  OF USE,  DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER  IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING
NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT  OF THE USE  OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Additional BSD Notice

1. This notice is required to  be provided under our contract with the
U.S. Department  of Energy (DOE).  This work was produced  at Lawrence
Livermore  National Laboratory  under  Contract No.  DE-AC52-07NA27344
with the DOE.

2.  Neither  the  United  States  Government  nor  Lawrence  Livermore
National Security, LLC nor any of their employees, makes any warranty,
express or implied, or assumes any liability or responsibility for the
accuracy, completeness,  or usefulness of  any information, apparatus,
product, or  process disclosed, or  represents that its use  would not
infringe privately-owned rights.

3.  Also,  reference  herein  to  any  specific  commercial  products,
process,  or  services  by  trade  name,  trademark,  manufacturer  or
otherwise does  not necessarily  constitute or imply  its endorsement,
recommendation,  or  favoring  by  the  United  States  Government  or
Lawrence Livermore  National Security, LLC. The views  and opinions of
authors expressed herein do not  necessarily state or reflect those of
the United States Government  or Lawrence Livermore National Security,
LLC,  and shall  not be  used for  advertising or  product endorsement
purposes.
*/
#ifndef H5Z_ZFP_H
#define H5Z_ZFP_H

#include "hdf5.h"

/* Filter ID number registered with The HDF Group */
#define H5Z_FILTER_ZFP 32013

#define H5Z_FILTER_ZFP_VERSION_MAJOR 0
#define H5Z_FILTER_ZFP_VERSION_MINOR 3
#define H5Z_FILTER_ZFP_VERSION_PATCH 0

/* Convenient CPP logic to capture H5Z_ZFP Filter version numbers as string and hex number */
#define H5Z_FILTER_ZFP_VERSION_STR__(Maj,Min,Pat) #Maj "." #Min "." #Pat
#define H5Z_FILTER_ZFP_VERSION_STR_(Maj,Min,Pat)  H5Z_FILTER_ZFP_VERSION_STR__(Maj,Min,Pat)
#define H5Z_FILTER_ZFP_VERSION_STR                H5Z_FILTER_ZFP_VERSION_STR_(H5Z_FILTER_ZFP_VERSION_MAJOR,H5Z_FILTER_ZFP_VERSION_MINOR,H5Z_FILTER_ZFP_VERSION_PATCH)

#define H5Z_FILTER_ZFP_VERSION_NO__(Maj,Min,Pat)  (0x0 ## Maj ## Min ## Pat)
#define H5Z_FILTER_ZFP_VERSION_NO_(Maj,Min,Pat)   H5Z_FILTER_ZFP_VERSION_NO__(Maj,Min,Pat)
#define H5Z_FILTER_ZFP_VERSION_NO                 H5Z_FILTER_ZFP_VERSION_NO_(H5Z_FILTER_ZFP_VERSION_MAJOR,H5Z_FILTER_ZFP_VERSION_MINOR,H5Z_FILTER_ZFP_VERSION_PATCH)

#define H5Z_ZFP_CD_NELMTS_MEM ((size_t) 6) /* used in public API to filter */
#define H5Z_ZFP_CD_NELMTS_MAX ((size_t) 6) /* max, over all versions, used in dataset header */

#define H5Z_ZFP_MODE_RATE      1
#define H5Z_ZFP_MODE_PRECISION 2
#define H5Z_ZFP_MODE_ACCURACY  3
#define H5Z_ZFP_MODE_EXPERT    4

/* HDF5 filter cd_vals[] layout (6 unsigned ints)
cd_vals    0       1        2         3         4         5    
----------------------------------------------------------------
rate:      1    unused    rateA     rateB     unused    unused
precision: 2    unused    prec      unused    unused    unused
accuracy:  3    unused    accA      accB      unused    unused
expert:    4    unused    minbits   maxbits   maxprec   minexp

A/B are high/low words of a double.

Note: This is *NOT* the same layout that is ultimately stored
to the file. A wholly different, cd_vals is stored in the file
using zfp_write_header.
*/

#define H5Pset_zfp_rate_cdata(R, N, CD)          \
do { if (N>=4) {double *p = (double *) &CD[2];   \
CD[0]=CD[1]=CD[2]=CD[3]=0;                       \
CD[0]=H5Z_ZFP_MODE_RATE; *p=R; N=4;}} while(0)

#define H5Pget_zfp_rate_cdata(N, CD) \
((double)(((N>=4)&&(CD[0]==H5Z_ZFP_MODE_RATE))?*((double *) &CD[2]):-1))

#define H5Pset_zfp_precision_cdata(P, N, CD)  \
do { if (N>=3) {CD[0]=CD[1]=CD[2];            \
CD[0]=H5Z_ZFP_MODE_PRECISION;                 \
CD[2]=P; N=3;}} while(0)

#define H5Pget_zfp_precision_cdata(N, CD) \
((double)(((N>=3)&&(CD[0]==H5Z_ZFP_MODE_ACCURACY))?CD[2]:-1))

#define H5Pset_zfp_accuracy_cdata(A, N, CD)      \
do { if (N>=4) {double *p = (double *) &CD[2];   \
CD[0]=CD[1]=CD[2]=CD[3]=0;                       \
CD[0]=H5Z_ZFP_MODE_ACCURACY; *p=A; N=4;}} while(0)

#define H5Pget_zfp_accuracy_cdata(N, CD) \
((double)(((N>=4)&&(CD[0]==H5Z_ZFP_MODE_ACCURACY))?*((double *) &CD[2]):-1))

#define H5Pset_zfp_expert_cdata(MiB, MaB, MaP, MiE, N, CD) \
do { if (N>=6) { CD[0]=CD[1]=CD[2]=CD[3]=CD[4]=CD[5]=0;    \
CD[0]=H5Z_ZFP_MODE_EXPERT;                                 \
CD[2]=MiB; CD[3]=MaB; CD[4]=MaP;                           \
CD[5]=(unsigned int)MiE; N=6;}} while(0)

#define H5Pget_zfp_expert_cdata(N, CD, MiB, MaB, MaP, MiE) \
do {                                                    \
    if ((N>=6)&&(CD[0] == H5Z_ZFP_MODE_EXPERT))         \
    {                                                   \
        unsigned int *p; int *q;                        \
        p = MiB; *p = CD[2];                            \
        p = MaB; *p = CD[3];                            \
        p = MaP; *p = CD[4];                            \
        q = MiE; *q = (int) CD[5];                      \
    }                                                   \
} while(0)

#ifdef AS_SILO_BUILTIN
extern void H5Z_zfp_register(void);
extern void H5Z_zfp_finalize(void);
#endif

typedef struct _h5z_zfp_controls_t {
    unsigned int mode;
    union {
        double rate;
        double acc;
        unsigned int prec;
        struct expert_ {
            unsigned int minbits;
            unsigned int maxbits;
            unsigned int maxprec;
            int minexp;
        } expert;
    } details;
} h5z_zfp_controls_t;


#ifdef __cplusplus
extern "C" {
#endif
extern herr_t H5Pset_zfp_rate(hid_t plist, double rate); 
extern herr_t H5Pset_zfp_precision(hid_t plist, unsigned int prec); 
extern herr_t H5Pset_zfp_accuracy(hid_t plist, double acc); 
extern herr_t H5Pset_zfp_expert(hid_t plist, unsigned int minbits, unsigned int maxbits,
    unsigned int maxprec, int minexp); 
#ifdef __cplusplus
}
#endif

#endif
