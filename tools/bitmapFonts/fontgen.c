/*
* Tool to generate bitmap fonts for a small LED array
*/

#include <stdio.h>
#include <assert.h>


#define FONTS_VERSION   "1.0.0"

#define EXTENDED_ASCII

#define NUM_FONTS       4
#define MAX_CHAR_COLS   8

#define NUM_ROWS        7
#define NUM_CHARS       128

#define MISSING_BITS    0x55

typedef struct {
    char letter;
    char code[NUM_ROWS][MAX_CHAR_COLS];
    char width;
} Font;

#include "wideFont.h"
#include "skinnyFont.h"
#include "verySkinnyFont.h"
#include "symbolsFont.h"


Font *fonts[] = {wideFont, skinnyFont, verySkinnyFont, symbolsFont};


//// TODO add font name and description fields
typedef struct {
    unsigned char code[NUM_ROWS];
    unsigned char columns;
} CharBitmap;

CharBitmap bitmapFont[NUM_FONTS][NUM_CHARS];


int main() {
    unsigned char bits;
    Font *fptr;

    for (int f = 0; f < NUM_FONTS; f++) {
        for (int c = 0; c < NUM_CHARS; c++) {
            fptr = 0;
            for (int i = 0; fonts[f][i].letter != 0; i++) {
                if (fonts[f][i].letter == c) {
                    fptr = &fonts[f][i];
                    break;
                }
            }
            if (fptr == 0) {
                // use a blank for missing characters
                for (int i = 0; i < NUM_ROWS; i++) {
                    bitmapFont[f][c].code[i] = MISSING_BITS;
                }
                bitmapFont[f][c].columns = MAX_CHAR_COLS;
            } else {
                // build bitmap version of the char
                for (int i = 0; i < NUM_ROWS; i++) {
                    bits = 0;
                    for (int j = 0; j < fptr->width; j++) {
                        bits = (bits << 1) | (fptr->code[i][j] != 32);
                    }
                    bitmapFont[f][c].code[i] = bits;
                }
                assert(fptr->width <= MAX_CHAR_COLS); // Number of columns exceeds max value
                bitmapFont[f][c].columns = fptr->width;
            }
        }
    }

    //// TODO add name and description for each font
    printf("/* Auto-generated bitmap fonts file */\n\n");

    printf("#ifndef FONTS_H\n");
    printf("#define FONTS_H\n\n");

    printf("#define FONTS_VERSION   \"%s\"\n", FONTS_VERSION);
    printf("#define NUM_FONTS       %d\n", NUM_FONTS);
    printf("#define NUM_ROWS        %d\n", NUM_ROWS);
    printf("#define MAX_CHAR_COLS   %d\n", MAX_CHAR_COLS);
    printf("#define NUM_CHARS       %d\n\n", NUM_CHARS);

    //// FIXME find a better way of doing this
    printf("String fontNames[NUM_FONTS] = {String(\"Wide\"), String(\"Skinny\"), String(\"VerySkinny\"), String(\"Symbols\")};\n\n");

    printf("typedef struct {\n");
    printf("    unsigned char code[NUM_ROWS];\n");
    printf("    unsigned char columns;\n");
    printf("} CharBitmap;\n\n");

    printf("CharBitmap fonts[NUM_FONTS][NUM_CHARS] = {\n");
    for (int f = 0; f < NUM_FONTS; f++) {
        printf("  {\n");
        for (int c = 0; c < NUM_CHARS; c++) {
            printf("    {\n");
            printf("      {\n");
            for (int i = 0; i < NUM_ROWS; i++) {
                printf("        0x%X,\n", bitmapFont[f][c].code[i]);
            }
            printf("      },\n");
            printf("      %d\n", bitmapFont[f][c].columns);
            printf("    },\n");
        }
        printf("  },\n");
    }
    printf("};\n");
    printf("#endif /*FONTS_H*/\n");
    return 0;
}
