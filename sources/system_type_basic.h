/**
 * @file type_basic.h
 * @brief
 * Définitions des types basiques.
 */

#ifndef TYPES_H
#define TYPES_H

 /*  _______   ______  _____ ____    ____    _    ____ ___ ___  _   _ _____ ____   */
 /* |_   _\ \ / /  _ \| ____/ ___|  | __ )  / \  / ___|_ _/ _ \| | | | ____/ ___|  */
 /*   | |  \ V /| |_) |  _| \___ \  |  _ \ / _ \ \___ \| | | | | | | |  _| \___ \  */
 /*   | |   | | |  __/| |___ ___) | | |_) / ___ \ ___) | | |_| | |_| | |___ ___) | */
 /*   |_|   |_| |_|   |_____|____/  |____/_/   \_\____/___\__\_\\___/|_____|____/  */

#include "string.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#endif
