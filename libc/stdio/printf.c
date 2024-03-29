/*
 * This file is part of the swerve libc.
 *
 * Copyright (C) 2019 the swerve development team (see AUTHORS.md).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int print(const char *data, size_t len)
{
	const unsigned char *bytes = (const unsigned char *) data;
	for (size_t i = 0; i < len; i++) {
		if (putchar(bytes[i]) == EOF)
			return 0;
	}
	return 1;
}

/*
 * Partial implementation of printf(3p)
 *
 * TODO numbers for hex output
 * TODO octal output
 * FIXME use algorithm to figure out exact hex string length
 * FIXME decimal output is broken, opposite endian
 */
int printf(const char * restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;
		int checking = 0;
		if (*format == '%') {
			checking = 1;
			format++;
		}

		char c;
		int i, n;
		char *str;
		size_t len;
		if (checking) {
			switch (*format) {
				case 'c':
					c = (char) va_arg(parameters, int);

					if (!maxrem)
						/* TODO: EOVERFLOW */
						return -1;
					if (!print(&c, sizeof(c)))
						return -1;
					written++;
					break;
				case 's':
					str = va_arg(parameters, char *);
					len = strlen(str);

					if (maxrem < len)
						/* TODO: EOVERFLOW */
						return -1;
					if (!print(str, len))
						return -1;

					written += len;
					break;
				case 'd':
					n = va_arg(parameters, int);
					while (n > 10) {
						putchar((n % 10) + 0x30);
						n = n / 10;
					}
					putchar(n + 0x30);
					break;
				case '%':
					print("%", 1);
				case 'x':
					i = 23;
					n = va_arg(parameters, int);
					str = malloc(24);
					str[i] = '\0';
					i--;
					while (n > 16) {
						if ((n % 16) > 9)
							str[i] = ((n % 16) - 9) + 0x40;
						else
							str[i] = ((n % 16) - 9) + 0x30;
						n = n / 16;
						i--;
					}
					if (n > 9)
						str[i] = (n - 9) + 0x40;
					else
						str[i] = n + 0x30;
					puts(&(str[i]));
				default: break;
			}
		} else {
			print(format, 1);
		}
		format++;
	}

	va_end(parameters);
	return written;
}
