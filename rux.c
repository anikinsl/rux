/*-
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

/* $Id: rux.c,v 1.49 2009/05/26 21:00:50 ghos Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#define DEBUG

#include "version.h"
static char *version = VERSION;

#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
 /* #include <libgen.h> *//* Uncomment if you want to use basename() */

#ifdef USE_ICONV
#include <iconv.h>
#endif

#ifdef MDEBUG
#include "dmalloc.h"
#else
void *my_alloc(size_t size);

#define dmalloc	my_alloc
#define dfree	free
#endif					/* MDEBUG */

#include "cptaba.h"
#include "cpdetect.h"

struct cpinfo *detect_codepage(struct cp_detect *detinf, FILE *in, FILE *out, int fdoconv, float *cpexp);
void doconv_all(u_char *conv_tab, FILE *in, FILE *out);
struct cpinfo *get_cp_info(const char *cpid);
u_char *init_tab(u_char *tab, int len, struct cpinfo *in, struct cpinfo *out);

void print_usage(void);
void print_cplist(struct cpinfo *cplist);

void add_buf(u_char *buf, int size);
void free_bufs();
int recode_buf(u_char *conv_tab, u_char *buf, int len, FILE *out, char *rch);
int pwarnf(const char *fmt,...);


/* Default flags' values */
static int f_graph = 0;			/* Recode pseudo-graphics too */
static int f_quiet = 1;			/* Quiet mode, shut stderr */
static int f_doconv = 1;		/* Do convert by default */
static int f_detect = 1;		/* Auto detect by default */
static int f_lax = 0;			/* Don't touch lax-files */
static int f_compl = 1;			/* Touch all including supported characters */

static struct cpinfo *i_cp = NULL;
static struct cpinfo *o_cp = NULL;

/* Emanate on minimal file block */
static int fbuf_size = 512;

struct databuf {
	u_char *data;
	int size;
	struct databuf *next;
};

static struct databuf *head = NULL;
static struct databuf *last = NULL;

/*
 * OK! Are you want to recode or detect code page of some file or files?
 * No problem. I'll do it for you!
 */

int main(int argc, char **argv)
{
	u_char conv_tab[_CPINFO_TSIZE];

	int readstdin = 0;		/* Don't read from stdin by default */

	FILE *in = stdin;
	FILE *out = stdout;

	extern char *optarg;
	extern int optind;

	u_char *iobuf = NULL;		/* File input buffer */
	int flag;			/* An option */

#ifndef DEFAULT_OUTPUT_CP
#  ifdef USE_ICONV
#    define DEFAULT_OUTPUT_CP 5
#  else
#    define DEFAULT_OUTPUT_CP 2
#  endif
#endif
	struct cpinfo *def_ocp = &codepage[DEFAULT_OUTPUT_CP];

	/* Info about detection of a code page */
	struct cp_detect detinf;

	o_cp = def_ocp;			/* Initialize o_cp using default cp */

	while ((flag = getopt(argc, argv, "i:o:thegTs:qvlc")) != -1) {
		switch (flag) {
		case 'e':		/* Universal recoding pseudo-graphics */
			f_graph = 0;
			break;
		case 'g':		/* Normal recoding pseudo-graphics */
			f_graph = 1;
			break;
		case 'i':		/* An input code page */
			i_cp = get_cp_info(optarg);
			f_detect = 0;
			break;
		case 'o':		/* An output code page */
			o_cp = get_cp_info(optarg);
			break;
		case 't':		/* Detect and recode */
			f_doconv = 1;
			f_detect = 1;
			break;
		case 'T':		/* Detect only */
			f_doconv = 0;
			f_detect = 1;
			break;
		case 's':		/* Block size in bytes */
			/* Using -s option is deprecated! */
			break;
		case 'q':		/* Suspress all warnings */
			f_quiet = 1;
			break;
		case 'v':		/* Verbose meaning */
			f_quiet = 0;
			break;
		case 'l':
			f_lax = 1;
			break;
		case 'c':
			f_compl = 0;
			break;
		case 'h':
		default:
			print_usage();
			return 0;
		}
	}

	argc -= optind;
	argv += optind;

	/* Brings to default output code page if it doesn't specified. */
	if (o_cp == NULL) {
		pwarnf("Brings to default output code page (%s).\n",
			def_ocp->cp_name);
		o_cp = def_ocp;
	}
	if (f_detect == 0) {
		if (i_cp == NULL) {
			pwarnf("Could not continue without input code page. "
					"Specify -i or -t option.\n");
			return -1;
		}
	} else {
		if ((iobuf = (u_char *) dmalloc(fbuf_size)) == NULL) {
			perror(NULL);
			return errno;
		}
		detinf.cp_list = (struct cpinfo *)&codepage;
		detinf.cp_default = o_cp;
		detinf.cp_databuf = (u_char *) iobuf;
	}

	if (argc == 0) {
		readstdin = 1;		/* read from stdin if not enough any
					   arguments */
	}
	/* This cycle is controlled by return and break ops. Cycle breaks
	   after first loop if we reading from stdin (flag readstdin is on) */
	while (1) {
		const char *fname = *argv++;
#ifdef DEBUG
		printf("\nDEBUG: %s\n", readstdin ? "<stdin>" : /* basename */ (fname));
#endif

		if (readstdin) {
			in = stdin;
		} else if ((in = fopen(fname, "r")) == NULL) {
			perror(fname);

			if (*argv == NULL) {
				break;
			}
			continue;
		}
		if (f_detect != 0) {
			float cpexp = .0;

			i_cp = detect_codepage(&detinf, in, out, f_doconv, &cpexp);

			if (f_doconv == 0) {
#ifdef DEBUG
				printf("%s: %s (%.8f)\n", readstdin ? "<stdin>" :
					 /* basename */ (fname),
					cpexp == .0 ? "us-ascii (no hits)" :
					i_cp->cp_name, i_cp->cp_exp);
#else
				printf("%s: %s\n", readstdin ? "<stdin>" :
					 /* basename */ (fname),
					cpexp == .0 ? "us-ascii (no hits)" :
					i_cp->cp_name);
#endif
			}
		}
		if (f_doconv != 0) {
			init_tab(conv_tab, sizeof(conv_tab), i_cp, o_cp);
			doconv_all(conv_tab, in, out);
		}
		fclose(in);

		if (readstdin) {
			break;
		}
		if (*argv == NULL) {
			break;
		}
	}

	if (f_detect != 0) {
		dfree(iobuf);
	}
	return 0;
}

/* This routine is used for determine a code page info from an identifier */

struct cpinfo *
       get_cp_info(const char *cpid)
{
	int i = 0;

	while (1) {
		if (codepage[i].cp_name == NULL) {
			break;
		}
		if (strcmp(codepage[i].cp_name, cpid) == 0) {
			return &codepage[i];
		}
		++i;
	}

	pwarnf("Wrong code page identifier: %s\n", cpid);
	print_cplist(codepage);
	return NULL;
}

/* Table completion routine. Used for filling up the alternative table
   within additional data */

void compl_tab(u_char * tab, struct cpinfo *incp, struct cpinfo *outcp)
{
	int i;
	int t_gr = f_graph;

	/* The pseudo-graphics will be lost (or segmentation fault occure) if
	   we'll recode using -g option from the code page with
	   pseudo-graphics chars to without. */

	if ((f_graph) && (incp->cp_graphsize != 0) && (outcp->cp_graphsize == 0)) {
		pwarnf("Output code page has no pseudo-graphics. Assuming a "
			"-g option off.\n");
		f_graph = 0;
	}
	if ((i = incp->cp_graphsize) != 0) {
		for (--i; i >= 0; i--) {
			tab[incp->cp_gdata[i]] = f_graph ? outcp->cp_gdata[i] :
				boxdr_map[i];
		}
	}
	/* Always restore an old f_graph value. We'll don't loss any time to
	   additional check. */
	f_graph = t_gr;
}

/* The alternative table initialization routine. Used for speed up recoding */

u_char *
init_tab(u_char * tab, int len, struct cpinfo *in, struct cpinfo *out)
{
	int i = len - 1;

	for (; i >= 0; i--) {
		tab[i] = i;
	}

	for (i = out->cp_size; i >= 0; i--) {
		tab[in->cp_data[i]] = out->cp_data[i];
	}

	if (f_compl) {
		compl_tab(tab, in, out);
	}

	return tab;
}

void print_usage(void)
{
	/* DATETIME was defined in version.h */
	printf (" rux %s -- " DATETIME "\n", version);

	/* Characters specific help message */
	printf (" Usage: rux [-egT] [-i incp] [-o outcp] [file ...]\n\n"
		"    -e   -- Replace box-drawing characters by non-graphic. Default.\n"
		"    -g   -- Inverse of -e option. (Overrides any previous -e option).\n"
		"    -c   -- Work with alphabet characters only. Skips non-russian characters\n"
	);

	/* Codepage specific help message*/
	printf (/*"    -t   -- Attempt to detect a code page of the input files. Default.\n"*/
		"    -T   -- Just show the code pages of the input files.\n\n"
		"    -i   -- Specify an input code page. Overrides by -[tT]\n"
		"    -o   -- Specify an output code page. Default is `%s'.\n\n",
		codepage[DEFAULT_OUTPUT_CP].cp_name);

	/* How to make it quiet and How to get help */
	printf (/*"    -q   -- Quiet mode (suspress warnings)\n"*/
		"    -v   -- Be verbose. Show all warnings.\n\n"
		"    -h   -- Show this help message.\n\n");

	print_cplist(codepage);
	printf("\nSample: cat book.html | rux | less");
	printf("\n        rux book.html | less");
	printf("\n        rux -T *");
	printf("\n--\nVyacheslav Anikin <anikinsl@gmail.com>\n");
}

void print_cplist(struct cpinfo *cplist)
{
	struct cpinfo *curcp = cplist;

	printf("The valid identifiers of the code pages are:\n");

	for (; curcp->cp_name; curcp++) {
		/*printf(" %s\t(%s)\n", curcp->cp_name, curcp->cp_desc);*/
		printf(" %s", curcp->cp_name);
	}

	printf("\n");
}

struct cpinfo *
       detect_codepage(struct cp_detect *detinf, FILE * in, FILE * out, int fdoconv, float *cpexp)
{
	int plank = 20;
	float cp_exp = .0;

	struct cpinfo *cur = codepage;

	for (; cur->cp_name; cur++) {
		cur->cp_exp = .0;
	}

	while (cp_exp < plank) {
		detinf->cp_datalen = fread(detinf->cp_databuf, 1, fbuf_size, in);
		i_cp = detect_cp(detinf, &cp_exp);

		if (cp_exp < 0.78 && f_lax != 0) {
#ifdef DEBUG
			printf("> debug: used an -l option. Assuming cp_exp = .0\n");
#endif
			cp_exp = .0;
		}
#ifdef DEBUG
		printf("> debug: cpexp    = %12.8f\n", i_cp->cp_exp);
		printf("> debug: codepage = %s\n", i_cp->cp_name);
		printf("> debug: plank    = %d\n", plank);
#endif
		if (fdoconv != 0) {
			if (cp_exp == .0) {
#ifdef DEBUG
				printf("> debug: output buffer: %d, cause cpexp = %11.8f\n",
					detinf->cp_datalen, i_cp->cp_exp);
#else
				fwrite(detinf->cp_databuf, detinf->cp_datalen, 1, out);
#endif
			} else {
				add_buf(detinf->cp_databuf, detinf->cp_datalen);
			}
		}
		if (feof(in))
			break;

		if (cp_exp != .0)
			plank -= 3;
	}

	*cpexp = i_cp->cp_exp;
	return i_cp;
}

/* Routine for converting of all buffers (if they exists)
 * and continuous converting of remaining part of file.
 */

void doconv_all(u_char * conv_tab, FILE * in, FILE * out)
{
	int c;				/* Input/output buffer */

#ifdef USE_ICONV
	char inbuf[4096 + 4096];
	size_t inbufrest = 0;
	char outbuf[4096];
	int status = 0;

	iconv_t enc = iconv_open(o_cp->cp_name, i_cp->cp_name);
	iconv(enc, NULL, NULL, NULL, NULL);

	if (head != NULL) {
		struct databuf *cur = head;
		int rest = 0;

		while (cur) {
			u_char rch;
#ifdef DEBUG
			printf("\n!!buf!!\n");
			printf("\ncur->data=%x", *cur->data);
			printf("\nrest=%d, cur->data+rest=%x\n\n", rest, *(cur->data + rest));
#endif
			rest = recode_buf(conv_tab, cur->data + rest,
				cur->size - rest, out, &rch);
			if (rest > 0) {
				u_char char_buf;
				if (cur->next) {
					char_buf = *cur->next->data;
				} else {
					int r = fread(&char_buf, 1, 1, in);
					if (r <= 0) {
						/**/
					}
				}
#ifdef DEBUG
				printf("\n\nrch=%x, char_buf=%x\n\n", rch, char_buf);
#endif
				unsigned short wch = (char_buf << 8) | rch;
#ifdef DEBUG
				printf("\n\nwch=%x\n\n", (unsigned short)wch);
#endif
				const char *inptr = (char *)&wch;
				size_t insize = sizeof(wch);
				char *outptr = outbuf;
				size_t outsize = sizeof(outbuf);
#ifdef DEBUG
				printf("\ninsize=%d, outsize=%d\n", insize, outsize);
#endif
				size_t res = iconv(enc, (char **)&inptr, &insize, &outptr, &outsize);
#ifdef DEBUG
				printf("\ninsize=%d, outsize=%d\n", insize, outsize);
#endif
#ifdef DEBUG
				printf("> debug: output buffer: %d\n",
					outptr - outbuf);
#else
				fwrite(outbuf, 1, outptr - outbuf, out);
#endif
#ifdef DEBUG
				printf("\noutbuf=%x, %d\n\n", *(unsigned short *)(outbuf), outptr - outbuf);
#endif
			}
			cur = cur->next;
		}
		free_bufs();
	}

#if 0
	int r = 0;
	char inbuf[4096 + 4096];
	int rest = 0;
	while (r = fread(inbuf + 4096, 1, 4096, in)) {
		if (r > 0) {
			//printf("\nrest = %d\n", rest);
			rest = recode_buf(NULL, inbuf + 4096, r, out);
			//printf("\nrest = %d\n", rest);
		}
	}
#endif

	for (;;) {
		size_t inbufsize = fread(inbuf + 4096, 1, 4096, in);
		if (inbufsize == 0) {
			if (inbufrest == 0)
				break;
			else {
				return;
			}
		} else {
			const char *inptr = inbuf + 4096 - inbufrest;
			size_t insize = inbufrest + inbufsize;
			inbufrest = 0;
			while (insize > 0) {
				char *outptr = outbuf;
				size_t outsize = sizeof(outbuf);
				size_t res = iconv(enc, (char **)&inptr, &insize, &outptr, &outsize);
				if (outptr != outbuf) {
#ifdef DEBUG
				printf("> debug: output buffer: %d\n", outptr - outbuf);
#else
					if (fwrite (outbuf, 1, outptr - outbuf, stdout) < outptr - outbuf) {
						return;
					}
#endif
				}
				if (res == (size_t)(-1)) {
					if (errno == EILSEQ) {
						/* discard unconvertible */
					} else if (errno == EINVAL) {
						if (inbufsize == 0 || insize > 4096) {
							return;
						} else {
							inbufrest = insize;
							if (insize > 0) {
								char *restptr = inbuf + 4096 - insize;
								do { *restptr++ = *inptr++; } while (--insize > 0);
							}
							break;
						}
					} else if (errno != E2BIG) {
						return;
					}
				}
			}
		}
	}
	{
	}
	iconv_close(enc);
#else /* USE_ICONV */
	/* XXX */
	if (strcmp(i_cp->cp_name, UTF8) == 0) {
		/*char hook[] = {'£', 'â'};*/
		char hook[4];
	       	hook[0] = o_cp->cp_data[6];
		hook[1] = o_cp->cp_data[34];
		hook[2] = o_cp->cp_data[18];
		hook[3] = o_cp->cp_data[39];

		while ((c = getc(in)) != EOF) {
			if (c == (u_char) 0xd0 || c == (u_char) 0xd1) {
				conv_tab[0x91] = c == 0xd1 ? hook[0] : hook[1];
				conv_tab[0x81] = c == 0xd1 ? hook[2] : hook[3];
			} else {
				putc(conv_tab[(u_char) c], out);
			}
		}
	} else {
		while ((c = getc(in)) != EOF) {
			putc(conv_tab[(u_char) c], out);
		}
	}
#endif
}

/* Program standard error/warning format output */
int pwarnf(const char *fmt,...)
{
	int vf_ret;
	va_list args;

	if (f_quiet) {
		return 0;
	}
	va_start(args, fmt);
	fprintf(stderr, "rux: ");
	vf_ret = vfprintf(stderr, fmt, args);
	va_end(args);

	/* We need to flush stderr output before fatal exit :-) */
	fflush(stderr);
	return vf_ret;
}

/*
 * Functions for supporting recoding buffers. Also there's
 * implemented the simplest one-way list routines (adding one node
 * and clearing of all).
 */

int recode_buf(u_char * conv_tab, u_char * buf, int len, FILE * out, char *rch)
{
#ifdef USE_ICONV
	int j=0;
	//printf("\n\n");
	for (; j < 2; j++) {
		//printf("0x%02x ", buf[j]);
	}
	//printf(" .. ");
	for (j = len - 1; j > len - 3; j--) {
		//printf("0x%02x ", buf[j]);
	}
	//printf("\n\n");

	iconv_t enc = iconv_open(o_cp->cp_name, i_cp->cp_name);
	char *inbuf = buf;
	size_t inbufrest = 0;
	char outbuf[4096];
	int status = 0;

	iconv(enc, NULL, NULL, NULL, NULL);

	size_t inbufsize = len;
	if (inbufsize == 0) {
			return;
	} else {
		const char *inptr = inbuf - inbufrest;
		size_t insize = inbufrest + inbufsize;
		inbufrest = 0;
		while (insize > 0) {
			char *outptr = outbuf;
			size_t outsize = sizeof(outbuf);
			size_t res = iconv(enc, (char **)&inptr, &insize, &outptr, &outsize);
#ifdef DEBUG
			printf("\ninsize=%d\n", insize);
#endif
			if (outptr != outbuf) {
#ifdef DEBUG
				printf("\noutptr != outbuf\n");
#endif
#ifdef DEBUG
				printf("> debug: output buffer: %d\n", outptr - outbuf);
#else
				if (fwrite (outbuf, 1, outptr - outbuf, stdout) < outptr - outbuf) {
					return 0;
				}
#endif
			}
			if (res == (size_t)(-1)) {
#ifdef DEBUG
				printf("\nres == (size_t)(-1)\n");
#endif
				if (errno == EILSEQ) {
#ifdef DEBUG
					printf("\nerrno == EILSEQ\n");
#endif
					return 0;
					//int one = 1;
					//iconvctl(enc,4,&one);
				} else if (errno == EINVAL) {
#ifdef DEBUG
					printf("\nerrno == EINVAL\n");
#endif
					if (inbufsize == 0 || insize > 4096) {
#ifdef DEBUG
						printf("\nreturn after insize>4096\n");
#endif
						return 0;
					} else {
#ifdef DEBUG
						printf("\ninbufsize=%d, insize=%d\n", inbufsize, insize);
#endif
						inbufrest = insize;
						if (insize > 0) {
							*rch = *inptr;
							//char *restptr = inbuf - insize;
							//do { *restptr++ = *inptr++; } while (--insize > 0);
							return inbufrest;
							return 0;
						}
						break;
					}
				} else if (errno != E2BIG) {
#ifdef DEBUG
					printf("\nerrno != E2BIG\n");
#endif
					return 0;
				}
			}
		}
	}
	{
	}
	iconv_close(enc);
#if 0
		iconv_t enc = iconv_open(o_cp->cp_name, i_cp->cp_name);
		char outbuf[4096];
		int inbufrest = 0;

		iconv(enc, NULL, NULL, NULL, NULL);

		size_t inbufsize = len;
		const char *inptr = buf;
		size_t insize = len + inbufrest;

		while (insize > 0) {
			printf("\nAAA\n");
			char *outptr = outbuf;
			size_t outsize = sizeof(outbuf);
			size_t res = iconv(enc, (char **)&inptr, &insize, &outptr, &outsize);
			if (outptr != outbuf) {
				if (fwrite (outbuf, 1, outptr - outbuf, stdout) < outptr - outbuf) {
					return;
				}
			}
			if (res == (size_t)(-1)) {
				if (errno == EILSEQ) {
					/* discard unconvertible */
				} else if (errno == EINVAL) {
					if (inbufsize == 0 || insize > 4096) {
						return;
					} else {
						inbufrest = insize;
						if (insize > 0) {
							char *restptr = buf + 4096 - insize;
							do { *restptr++ = *inptr++; } while (--insize > 0);
						}
						break;
					}
				} else if (errno != E2BIG) {
					return;
				}
			}
		}

		{
		}
		iconv_close(enc);
#endif
#if 0
	//printf("%s, %s\n", i_cp->cp_name, o_cp->cp_name);
	iconv_t enc = iconv_open(o_cp->cp_name, i_cp->cp_name);
	char outbuf[2048];
	char *outs = outbuf;
	int insize = len;
	int outsize, bufsize;
	outsize = sizeof outbuf;

	int k = iconv(enc, &buf, &insize, &outs, &outsize);
	int lout = 2048 - outsize;
	printf("%s", outbuf);
	//fwrite(outbuf, 1, lout, out);
	iconv_close(enc);
#endif
#else
	if (strcmp(i_cp->cp_name, UTF8) == 0) {
		char hook[4];
	       	hook[0] = o_cp->cp_data[6];
		hook[1] = o_cp->cp_data[34];
		hook[2] = o_cp->cp_data[18];
		hook[3] = o_cp->cp_data[39];

		while (len--) {
			if (*buf == (u_char) 0xd0 || *buf == (u_char) 0xd1) {
				conv_tab[0x91] = *buf == 0xd1 ? hook[0] : hook[1];
				conv_tab[0x81] = *buf == 0xd1 ? hook[2] : hook[3];

				++buf;
			} else {
				putc(conv_tab[(u_char) *buf++], out);
			}
		}
	} else
	{
		while (len--) {
			putc(conv_tab[*buf++], out);
		}
	}
#endif
}

void add_buf(u_char * buf, int size)
{
#ifdef DEBUG
	printf("> debug: buffering...\n");
#endif
	struct databuf *t = (struct databuf *)dmalloc(sizeof(struct databuf));

	t->data = (u_char *) dmalloc(size);
	t->size = size;
	t->next = NULL;
	memcpy(t->data, buf, size);

	if (head == NULL) {
		head = t;
	} else {
		last->next = t;
	}

	last = t;
}

void free_bufs()
{
	struct databuf *cur = head;

	while (cur) {
		struct databuf *t = cur;

		cur = cur->next;
		dfree(t->data);
		dfree(t);
		t = NULL;
	}
	head = NULL;
	last = NULL;
}


void *my_alloc(size_t size)
{
	void *ret = (void *)malloc(size);

	if (ret == NULL)
		perror("rux");
	return ret;
}
