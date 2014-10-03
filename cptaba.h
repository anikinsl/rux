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

/*
 * There are codepages list.
 * Note: letters were placed in order 'a..ya A..YA'
 */

#include <sys/types.h>
#include "cpinfo.h"

/*
wchar_t cptab_utf8[66] = {
	0xd0b0, 0xd0b1, 0xd0b2, 0xd0b3, 0xd0b4, 0xd0b5, 
	0xd191, 0xd0b6, 0xd0b7, 0xd0b8, 0xd0b9, 0xd0ba, 
	0xd0bb, 0xd0bc, 0xd0bd, 0xd0be, 0xd0bf, 0xd180, 
	0xd181, 0xd182, 0xd183, 0xd184, 0xd185, 0xd186, 
	0xd187, 0xd188, 0xd189, 0xd18a, 0xd18b, 0xd18c, 
	0xd18d, 0xd18e, 0xd18f, 0xd090, 0xd091, 0xd092, 
	0xd093, 0xd094, 0xd095, 0xd081, 0xd096, 0xd097, 
	0xd098, 0xd099, 0xd09a, 0xd09b, 0xd09c, 0xd09d, 
	0xd09e, 0xd09f, 0xd0a0, 0xd0a1, 0xd0a2, 0xd0a3, 
	0xd0a4, 0xd0a5, 0xd0a6, 0xd0a7, 0xd0a8, 0xd0a9, 
	0xd0aa, 0xd0ab, 0xd0ac, 0xd0ad, 0xd0ae, 0xd0af,
};
*/

u_char cptaba[][66] = {
/*cp866*/ {'\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5',
	'\xf1', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa',
	'\xab', '\xac', '\xad', '\xae', '\xaf', '\xe0',
	'\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6',
	'\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec',
	'\xed', '\xee', '\xef', '\x80', '\x81', '\x82',
	'\x83', '\x84', '\x85', '\xf0', '\x86', '\x87',
	'\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d',
	'\x8e', '\x8f', '\x90', '\x91', '\x92', '\x93',
	'\x94', '\x95', '\x96', '\x97', '\x98', '\x99',
	'\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f'},

/*cp1251*/ {'\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5',
	'\xb8', '\xe6', '\xe7', '\xe8', '\xe9', '\xea',
	'\xeb', '\xec', '\xed', '\xee', '\xef', '\xf0',
	'\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6',
	'\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc',
	'\xfd', '\xfe', '\xff', '\xc0', '\xc1', '\xc2',
	'\xc3', '\xc4', '\xc5', '\xa8', '\xc6', '\xc7',
	'\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd',
	'\xce', '\xcf', '\xd0', '\xd1', '\xd2', '\xd3',
	'\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9',
	'\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf'},

/*koi8-r*/ {'\xc1', '\xc2', '\xd7', '\xc7', '\xc4', '\xc5',
	'\xa3', '\xd6', '\xda', '\xc9', '\xca', '\xcb',
	'\xcc', '\xcd', '\xce', '\xcf', '\xd0', '\xd2',
	'\xd3', '\xd4', '\xd5', '\xc6', '\xc8', '\xc3',
	'\xde', '\xdb', '\xdd', '\xdf', '\xd9', '\xd8',
	'\xdc', '\xc0', '\xd1', '\xe1', '\xe2', '\xf7',
	'\xe7', '\xe4', '\xe5', '\xb3', '\xf6', '\xfa',
	'\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee',
	'\xef', '\xf0', '\xf2', '\xf3', '\xf4', '\xf5',
	'\xe6', '\xe8', '\xe3', '\xfe', '\xfb', '\xfd',
	'\xff', '\xf9', '\xf8', '\xfc', '\xe0', '\xf1'},

/*iso8859-5*/ {'\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5',
	'\xf1', '\xd6', '\xd7', '\xd8', '\xd9', '\xda',
	'\xdb', '\xdc', '\xdd', '\xde', '\xdf', '\xe0',
	'\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6',
	'\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec',
	'\xed', '\xee', '\xef', '\xb0', '\xb1', '\xb2',
	'\xb3', '\xb4', '\xb5', '\xa1', '\xb6', '\xb7',
	'\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd',
	'\xbe', '\xbf', '\xc0', '\xc1', '\xc2', '\xc3',
	'\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9',
	'\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf'},

/*mac*/ {'\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5',
	'\xde', '\xe6', '\xe7', '\xe8', '\xe9', '\xea',
	'\xeb', '\xec', '\xed', '\xee', '\xef', '\xf0',
	'\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6',
	'\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc',
	'\xfd', '\xfe', '\xdf', '\x80', '\x81', '\x82',
	'\x83', '\x84', '\x85', '\xdd', '\x86', '\x87',
	'\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d',
	'\x8e', '\x8f', '\x90', '\x91', '\x92', '\x93',
	'\x94', '\x95', '\x96', '\x97', '\x98', '\x99',
	'\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f'},

/*utf8*/ {'\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5',
	'\x91', '\xb6', '\xb7', '\xb8', '\xb9', '\xba',
	'\xbb', '\xbc', '\xbd', '\xbe', '\xbf', '\x80',
	'\x81', '\x82', '\x83', '\x84', '\x85', '\x86',
	'\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c',
	'\x8d', '\x8e', '\x8f', '\x90', '\x91', '\x92',
	'\x93', '\x94', '\x95', '\x81', '\x96', '\x97',
	'\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d',
	'\x9e', '\x9f', '\xa0', '\xa1', '\xa2', '\xa3',
	'\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9',
	'\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf'},
/*utf8s*/ {'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0',
	'\xd1', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0',
	'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd1',
	'\xd1', '\xd1', '\xd1', '\xd1', '\xd1', '\xd1',
	'\xd1', '\xd1', '\xd1', '\xd1', '\xd1', '\xd1',
	'\xd1', '\xd1', '\xd1', '\xd0', '\xd0', '\xd0',
	'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0',
	'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0',
	'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0',
	'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0',
	'\xd0', '\xd0', '\xd0', '\xd0', '\xd0', '\xd0'},
/*trans*/ {'a', 'b', 'v', 'g', 'd', 'e',
	'e', 'z', 'z', 'i', 'y', 'k',
	'l', 'm', 'n', 'o', 'p', 'r',
	's', 't', 'u', 'f', 'h', 'c',
	'c', 's', 's', '\'', 'y', '\'',
	'e', 'u', 'a', 'A', 'B', 'V',
	'G', 'D', 'E', 'E', 'J', 'Z',
	'I', 'I', 'K', 'L', 'M', 'N',
	'O', 'P', 'R', 'S', 'T', 'U',
	'F', 'H', 'C', 'C', 'S', 'S',
	'\'', 'Y', '\'', 'E', 'U', 'A'}
};

u_char boxdraw[][49] = {
	{'\xd9', '\xc0', '\xbf', '\xda', '\xc3', '\xb4', '\xc2',
	'\xc1', '\xc5', '\xbd', '\xbe', '\xd3', '\xd4', '\xb7',
	'\xb8', '\xd3', '\xd4', '\xb5', '\xb6', '\xc6', '\xc7',
	'\xd2', '\xd1', '\xd0', '\xd7', '\xd8', '\xbc', '\xc8',
	'\xbb', '\xc9', '\xcc', '\xb9', '\xcb', '\xca', '\xce',
	'\xc4', '\xcd', '\xb3', '\xba', '\xdf', '\xdc', '\xdd',
	'\xde', '\xdb', '\xb2', '\xb1', '\xb0', '\xfe', '\xf9'},

	{'\x85', '\x84', '\x83', '\x82', '\x86', '\x87', '\x88',
	'\x89', '\x8a', '\xa6', '\xa7', '\xa2', '\xa4', '\xac',
	'\xad', '\xa9', '\xaa', '\xb2', '\xb4', '\xaf', '\xb0',
	'\xb7', '\xb6', '\xba', '\xb9', '\xbc', '\xae', '\xab',
	'\xa8', '\xa5', '\xb1', '\xb5', '\xb8', '\xbb', '\xbe',
	'\x80', '\xa0', '\x81', '\xa1', '\x8b', '\x8c', '\x8e',
	'\x8f', '\x8d', '\x92', '\x91', '\x90', '\x94', '\x95'}
};

u_char boxdr_map[] = {
	'+', '+', '+', '+', '+', '+', '+',
	'+', '+', '+', '+', '+', '+', '+',
	'+', '+', '+', '+', '+', '+', '+',
	'+', '+', '+', '+', '+', '+', '+',
	'+', '+', '+', '+', '+', '+', '+',
	'-', '-', '|', '|', '#', '#', '#',
	'#', '#', '#', '#', '#', '#', 'o',
};

#define CP866	"cp866"
#define CP1251	"cp1251"
#define KOI8R	"koi8-r"
#define ISO5	"iso-8859-5"
#define MACCYR	"mac-cyrillic"
#define UTF8	"utf-8"
#define UTF8S	"<test>"
#define TR	"tr"

struct cpinfo codepage[] = {
	{CP866,	"DOS cyrillic charset", sizeof(cptaba[0]), sizeof(boxdraw[0]), cptaba[0], boxdraw[0], .0, sizeof(u_char)},
	{CP1251, "Windows cyrillic charset",	sizeof(cptaba[1]), 0, cptaba[1], NULL, .0, sizeof(u_char)},
	{KOI8R, "RFC russian charset",	sizeof(cptaba[2]), sizeof(boxdraw[1]), cptaba[2], boxdraw[1], .0, sizeof(u_char)},
	{ISO5, "ISO cyrillic charset",	sizeof(cptaba[3]), 0, cptaba[3], NULL, .0, sizeof(u_char)},
	{MACCYR, "Apple Mac cyrillic charset",		sizeof(cptaba[4]), 0, cptaba[4], NULL, .0, sizeof(u_char)},
	{UTF8, "Unicode 8-bit charset, Read-Only",	sizeof(cptaba[5]), 0, cptaba[5], NULL, .0, sizeof(wchar_t)},
	/*{TR, "ASCII transliteration, Read-Only",	sizeof(cptaba[6]), 0, cptaba[6], NULL, .0},*/
	/*{UTF8S, "<test>",	sizeof(cptaba[6]), 0, cptaba[6], NULL, .0},*/
	{NULL, NULL, 0, 0, NULL, NULL, .0}
};
