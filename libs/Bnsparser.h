#pragma once
#include <stdio.h>

/* ======== UTILITIES ======== */

/* Safe malloc. Terminates program immediately upon failure. */
char* xmalloc(size_t size);

/* Safe realloc. Terminates program immediately upon failure */
char* xrealloc(char *buff, size_t newSize);

/* ======== OBJECTS ======== */

// used to make intellisense and compiler shut up
typedef struct file file;
typedef struct file_vtable file_vtable;
extern const file_vtable FILE_VTABLE;

typedef struct line line;
typedef struct line_vtable line_vtable;
extern const line_vtable LINE_VTABLE; 

struct file { // file object
    const file_vtable *vt;
    line *first;
    line *current;
    size_t lines_count;
};

struct file_vtable {
    bool (*next)(file *self);
};

/* Moves the current line * inside of the file * object */
bool File_next(file *self);

/* Reads a file and stores it, line by line, in a file * object. */
file* bnsreadf(FILE *fp);

struct line_vtable {
    line* (*sub)(line *self, size_t a, size_t b);
};

struct line { // line object used in file object
    const line_vtable *vt;    
    size_t length;
    char *buff;
    struct line *next;
};

/* Returns a null-terminated sub-string of the current line * in  the file * object */
line* Line_sub(line *self, size_t a, size_t b);


typedef struct token { // `token` object. Does NOT have v-table
    size_t length;
    char *buff;
    struct token *next;
} token;

/* Returns an array of tokens from a given `line` and a separtor string (MUST BE NULL-TERMINATED!) */
token** bnstokenise(line *ln, char *separator_string);

/* Prints the passed token `T`'s buffer to `stdout`. */
void bnsprinttkn(token *T);

/* Compares 2 given tokens. */
bool bnscmptkn(token *T1, token *T2);

/* Checks if the passed character (NOT string) is a separator */
bool validate_separator(char c, char *separators_string);