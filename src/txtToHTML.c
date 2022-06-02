#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 *
 */

#define VERSION "0.43"

typedef struct {
    int position;
    int size;
    char *data;
} Buffer;

struct closeThings {
    bool Bold;
    bool Italics;
    bool Strikethrough;
    bool Monospace;
    bool Underline;
};

void reallocBuffer(Buffer *buffer) {
    buffer->size += 1024;
    buffer->data = realloc(buffer->data,buffer->size);
    if (!buffer->data) {
        printf("Error realloc-ing buffer!");
        exit(1);
    }
}

void appendToBuffer(Buffer *buffer, Buffer *dataBuffer) {
    for (int i = 0;i < dataBuffer->size;i++) {
        if (dataBuffer->data[i] == 0x00) break; /* stop on NULL */
        buffer->data[buffer->position] = dataBuffer->data[i];
        buffer->position++;
        if (buffer->position > buffer->size-64) {
            reallocBuffer(buffer);
        }
    }
}

int main(int argc, char **argv) {

    if (argc > 1) {
        printf("Reeses static site generator V %s\n",VERSION);
        printf("    Usage:\n");
        printf("        cat coolthing.txt | staticSiteGen > coolthing.html\n");
        printf("    Special characters:\n");
        printf("        * = bold\n");
        printf("        / = italic\n");
        printf("        ` = monospace\n");
        printf("        ~ = strikethrough\n");
        printf("        _ = underline\n");
        printf("        markdown style links are available\n");
        printf("        you can escape any character with \\ and it will not be used for formatting\n");
        exit(1);
    }

    Buffer buffer;
    buffer.size     = 1024;
    buffer.data     = calloc(buffer.size,sizeof(char));
    buffer.position = 0;

    char character = 'a';
    bool skipChar = false;

    struct closeThings close;

    while(character != EOF) {

        character = fgetc(stdin);

        if (buffer.position > buffer.size-64) {
            reallocBuffer(&buffer);
        }

        if (skipChar) {
            skipChar = false;
            buffer.data[buffer.position] = character;
            buffer.position++;
            continue;
        } else if (character == '\\') {
            skipChar = true;

        } else if (character == '*') {
            if (close.Bold) {
                appendToBuffer(&buffer,&(Buffer){0,4,"</b>"});
                close.Bold = false;
            } else {
                appendToBuffer(&buffer,&(Buffer){0,3,"<b>"});
                close.Bold = true;
            }

        } else if (character == '/') {
            if (close.Italics) {
                appendToBuffer(&buffer,&(Buffer){0,4,"</i>"});
                close.Italics = false;
            } else {
                appendToBuffer(&buffer,&(Buffer){0,3,"<i>"});
                close.Italics = true;
            }

        } else if (character == '`') {
            if (close.Monospace) {
                appendToBuffer(&buffer,&(Buffer){0,7,"</code>"});
                close.Monospace = false;
            } else {
                appendToBuffer(&buffer,&(Buffer){0,6,"<code>"});
                close.Monospace = true;
            }

        } else if (character == '~') {
                        if (close.Strikethrough) {
                appendToBuffer(&buffer,&(Buffer){0,6,"</del>"});
                close.Strikethrough = false;
            } else {
                appendToBuffer(&buffer,&(Buffer){0,5,"<del>"});
                close.Strikethrough = true;
            }

        } else if (character == '_') {
            if (close.Underline) {
                appendToBuffer(&buffer,&(Buffer){0,6,"</ins>"});
                close.Underline = false;
            } else {
                appendToBuffer(&buffer,&(Buffer){0,5,"<ins>"});
                close.Underline = true;
            }

        } else if (character == '[') {
            Buffer linkTxt = {0,256,calloc(256,sizeof(char))};
            Buffer linkLoc = {0,256,calloc(256,sizeof(char))};
            Buffer linkOut = {0,256,calloc(256,sizeof(char))};
            appendToBuffer(&linkOut,&(Buffer){0,10,"<a href=\""});

            while (character != 0x00) {
                character = fgetc(stdin);
                if (character == ']') break;
                linkTxt.data[linkTxt.position] = character;
                linkTxt.position++;
            }
            character = fgetc(stdin); /* this remove the '(' */
            while (character != 0x00) {
                character = fgetc(stdin);
                if (character == ')') break;
                linkLoc.data[linkLoc.position] = character;
                linkLoc.position++;
            }

            appendToBuffer(&linkOut,&linkLoc);
            appendToBuffer(&linkOut,&(Buffer){0,2,"\">"});
            appendToBuffer(&linkOut,&linkTxt);
            appendToBuffer(&linkOut,&(Buffer){0,4,"</a>"});

            appendToBuffer(&buffer,&linkOut);
            free(linkOut.data);
            free(linkTxt.data);
            free(linkLoc.data);

        } else if (character == '\n') {
            appendToBuffer(&buffer,&(Buffer){0,5,"<br>\n"});

        } else {
            buffer.data[buffer.position] = character;
            buffer.position++;
        }
    }

    for (int i = 0;i < buffer.position-1;i++) {
        printf("%c",buffer.data[i]);
    }

    return 0;
}
