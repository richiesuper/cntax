/**
 * Copyright 2022 Richie Seputro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, named LICENSE.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/* use tab size of 4 to view the code as it's intended! */

#include <stdio.h>
#include <string.h>

/* sizes for array */
#define TYPES 8
#define COUNT 2048 /* you can edit this value, especially if FILE is big */
#define ORDER 2
#define POS   2

/* colors */
#define RED "\033[1;31m"
#define CLR "\033[0m"

int
main(int argc, char *argv[])
{
	FILE *fp; /* pointer to FILE */
	char c, lastC; /* c = current char, lastC = last char */
	int idx[TYPES][COUNT][ORDER][POS] = {
		/* [type][count][start, end][line, col] */
		{ { { 0, 0 } } }, /* parentheses indices */
		{ { { 0, 0 } } }, /* brackets indices */
		{ { { 0, 0 } } }, /* braces indices */
		{ { { 0, 0 } } }, /* single quote indices */
		{ { { 0, 0 } } }, /* double quote indices */
		{ { { 0, 0 } } }, /* escape sequence indices */
		{ { { 0, 0 } } }, /* comment indices */
		{ { { 0, 0 } } }, /* semi-colon indices (this doesn't have end value) */
	};
	int cnt[TYPES][ORDER]; /* counter for the start/end indices of each type */
	int line, col; /* counter for line number and column number */
	int hasFoundSQuote, hasFoundDQuote; /* inside a quoted part? */
	int inCmnt; /* inside a comment? */
	int i, j; /* iterators for loops */
	char *output0 = "%s:%d:%d: %serror:%s Unmatched parentheses.\n",
         *output1 = "%s:%d:%d: %serror:%s Unmatched square brackets.\n",
         *output2 = "%s:%d:%d: %serror:%s Unmatched curly braces.\n",
         *output3 = "%s:%d:%d: %serror:%s Unmatched single quotes.\n",
         *output4 = "%s:%d:%d: %serror:%s Unmatched double quotes.\n",
         *output6 = "%s:%d:%d: %serror:%s Unmatched comments.\n";

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}

	if ((fp = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "Failure in opening %s\n", argv[1]);
		return 2;
	}

	/* fill the memory regions of arrays idx and cnt with zeroes */
	memset(idx, 0, sizeof idx);
	memset(cnt, 0, sizeof cnt);

	/* initialize line and col to starting value, set the state of quotes */
	line = 1;
	col = 0;
	hasFoundSQuote = hasFoundDQuote = inCmnt = 0;

	/* initialize the last character as NULL, because we haven't read any */
	lastC = '\0';

	while (fscanf(fp, "%c", &c) != EOF)
	{
		++col;

		switch (c)
		{
			case '\n': /* newline */
				++line;
				col = 0;
				break;
			case '(': /* starting parentheses */
				idx[0][cnt[0][0]][0][0] = line;
				idx[0][cnt[0][0]][0][1] = col;
				++cnt[0][0];
				break;
			case ')': /* ending parentheses */
				idx[0][cnt[0][1]][1][0] = line;
				idx[0][cnt[0][1]][1][1] = col;
				++cnt[0][1];
				break;
			case '[': /* starting brackets */
				idx[1][cnt[1][0]][0][0] = line;
				idx[1][cnt[1][0]][0][1] = col;
				++cnt[1][0];
				break;
			case ']': /* ending brackets */
				idx[1][cnt[1][1]][1][0] = line;
				idx[1][cnt[1][1]][1][1] = col;
				++cnt[1][1];
				break;
			case '{': /* starting braces */
				idx[2][cnt[2][0]][0][0] = line;
				idx[2][cnt[2][0]][0][1] = col;
				++cnt[2][0];
				break;
			case '}': /* ending braces */
				idx[2][cnt[2][1]][1][0] = line;
				idx[2][cnt[2][1]][1][1] = col;
				++cnt[2][1];
				break;
			case '\'': /* single quote (can be starting or ending) */
				if (lastC == '\\')
					break;

				if (hasFoundSQuote == 0) /* starting single quote */
				{
					idx[3][cnt[3][0]][0][0] = line;
					idx[3][cnt[3][0]][0][1] = col;
					++cnt[3][0];
					hasFoundSQuote = 1;
				}
				else /* ending single quote */
				{
					idx[3][cnt[3][1]][1][0] = line;
					idx[3][cnt[3][1]][1][1] = col;
					++cnt[3][1];
					hasFoundSQuote = 0;
				}
				break;
			case '"': /* double quote (can be starting or ending) */
				if (lastC == '\\')
					break;

				if (hasFoundDQuote == 0) /* starting double quote */
				{
					idx[4][cnt[4][0]][0][0] = line;
					idx[4][cnt[4][0]][0][1] = col;
					++cnt[4][0];
					hasFoundDQuote = 1;
				}
				else /* ending double quote */
				{
					idx[4][cnt[4][1]][1][0] = line;
					idx[4][cnt[4][1]][1][1] = col;
					++cnt[4][1];
					hasFoundDQuote = 0;
				}
				break;
			case '*': /* asterisk of the comment beginning */
				if (lastC == '/' && inCmnt == 0)
				{
					idx[6][cnt[6][0]][0][0] = line;
					idx[6][cnt[6][0]][0][1] = col;
					++cnt[6][0];
					inCmnt = 1;
				}
				break;
			case '/': /* forward slash of the comment ending */
				if (lastC == '*' && inCmnt == 1)
				{
					idx[6][cnt[6][1]][1][0] = line;
					idx[6][cnt[6][1]][1][1] = col;
					++cnt[6][1];
					inCmnt = 0;
				}
				break;
			case ';': /* semi colon (start position only) */
				idx[7][cnt[7][0]][0][0] = line;
				idx[7][cnt[7][0]][0][1] = col;
				++cnt[7][0];
				break;
			default:
				break;
		}

		lastC = c;
	}

	for (i = 0; i < TYPES; ++i)
		for (j = 0; j < COUNT; ++j)
		{
			/**
			 * check for unmatched parentheses, brackets, braces,
			 * single quotes, and double quotes
			 */
			if ((i >= 0 && i <= 4) || i == 6)
				/* if the starting character is found... */
				if (idx[i][j][0][0] != 0)
					/* ...but the matching ending character is not */
					if (idx[i][j][1][0] == 0)
						switch (i)
						{
							case 0:
								printf(output0, argv[1],
                                       idx[i][j][0][0], idx[i][j][0][1],
                                       RED, CLR);
								break;
							case 1:
								printf(output1, argv[1],
                                       idx[i][j][0][0], idx[i][j][0][1],
                                       RED, CLR);
								break;
							case 2:
								printf(output2, argv[1],
                                       idx[i][j][0][0], idx[i][j][0][1],
                                       RED, CLR);
								break;
							case 3:
								printf(output3, argv[1],
                                       idx[i][j][0][0], idx[i][j][0][1],
                                       RED, CLR);
								break;
							case 4:
								printf(output4, argv[1],
                                       idx[i][j][0][0], idx[i][j][0][1],
                                       RED, CLR);
								break;
							case 6:
								printf(output6, argv[1],
                                       idx[i][j][0][0], idx[i][j][0][1],
                                       RED, CLR);
								break;
						}
		}

	return 0;
}
