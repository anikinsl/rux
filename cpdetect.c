/*
 * Copyright ¿ 2005-2014 Vyacheslav Anikin. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "cpdetect.h"
#include "cpinfo.h"

/* The table of the expectancies of character presents in text */
/* Sorted in cptaba order */
float norm[] = {
	.02911861, .00530614, .01529035, .00398345, .01118890, .03256691,
	.00017816, .00325609, .00691263, .02753051, .00546225, .01140450,
	.01529603, .01238457, .02199038, .03914828, .01230602, .01882221,
	.02013654, .02705218, .00879488, .00194710, .00301776, .00160615,
	.00468407, .00157038, .00157740, .00009125, .00802607, .00712522,
	.00116392, .00280951, .00752902, .00009727, .00009627, .00076313,
	.00012368, .00036435, .00037505, .00000100, .00000201, .00025037,
	.00028012, .00000602, .00039677, .00004446, .00014741, .00043120,
	.00040413, .00083867, .00023833, .00041081, .00021995, .00017516,
	.00008323, .00004011, .00001170, .00006685, .00003276, .00000234,
	.00000000, .00000802, .00000468, .00031922, .00000669, .00001972,
};

struct cpinfo *detect_cp(struct cp_detect *detinf, float *cpexp)
{
	int i;
	u_int freq[_CPINFO_TSIZE];	/* frequencies table */
	u_char tab[_CPINFO_TSIZE];	/* alternative table for freq
					   calculations */
	struct cpinfo *cp = detinf->cp_list;
	float cp_exp = .0;		/* code page expectancy */
	int cp_size = cp->cp_size;

	struct cpinfo *cur_cp;
	struct cpinfo *in_cp = detinf->cp_default;

	for (cur_cp = cp; cur_cp->cp_name; cur_cp++) {
		float cur_exp = .0;	/* an expectancy of current code page */
		u_char *p = detinf->cp_databuf + detinf->cp_datalen - 1;

		memset(tab, 255, sizeof(tab));
		memset(freq, 0, sizeof(freq));

		for (i = 0; i < cp_size; i++) {
			tab[cur_cp->cp_data[i]] = i;
		}

		while ((p - detinf->cp_databuf) >= 0) {
			++freq[tab[*p--]];	/* freq[0xff] is a heap :) */
		}

		for (i = 0; i < cp_size; i++) {
			cur_exp += sqrt(freq[i] / (float)detinf->cp_datalen
				* norm[i] * powf(cp_size - i, 2));
		}

		cur_cp->cp_exp += cur_exp;
#ifdef DEBUG
		printf("> > debug: cur_exp = %11.8f, cur_cp->cp_exp = %11.8f, codepage: %s\n",
			cur_exp, cur_cp->cp_exp, cur_cp->cp_name);
#endif

		if (cur_cp->cp_exp > cp_exp) {
			in_cp = cur_cp;
			cp_exp = cur_cp->cp_exp;
		}
	}

	*cpexp = cp_exp;
#ifdef DEBUG
		printf("> > debug: <result> cp_exp = %.8f, codepage: %s\n",
			cp_exp, in_cp->cp_name);
#endif
	return in_cp;
}
