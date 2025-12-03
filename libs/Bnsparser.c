#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Bnsparser.h"

/* ======== GENERIC GLOBALS ======== */

#define LINEBUFF_DEFAULT_SIZE 100
#define TOKENBUFF_DEFAULT_SIZE 10

/* ======== UTILITIES ======== */
char* xmalloc(size_t size) {
    char *buff = malloc(size);
    if (!buff) {
        fprintf(stderr, "ERROR: couldn't allocate memory.\n");
        exit(-1);
    }
    return buff;
}

char* xrealloc(char *buff, size_t newSize) {
    char *newBuff = realloc(buff, newSize);
    if (!newBuff) {
        fprintf(stderr, "ERROR: couldn't reallocate memory.\n");
        exit(-1);
    }
    return newBuff;
}

/* ======== V-TABLE IMPLEMENTATIONS ======== */

const file_vtable FILE_VTABLE = {
    .next = File_next,
};

const line_vtable LINE_VTABLE = {
    .sub = Line_sub,
};

/* Moves the current line * inside of the file * object */
bool File_next(file *self) {
    if (!self || !self->current || !self->current->next) return false;
    
    self->current = self->current->next;
    return true;
}

/* Returns a null-terminated sub-string of the current line * in  the file * object */
line* Line_sub(line *self, size_t a, size_t b) {
    if (!self || !self->buff) return 0;

    if (b > self->length) b = self->length;
    
    line *substr = (line *)xmalloc(sizeof(line));
    substr->vt = &LINE_VTABLE;
    substr->buff = &self->buff[a];
    substr->length = b-a;
    substr->next = nullptr;

    return substr;
}

/* ======== MAIN FUNCTIONS ======== */

/* Cleanup function */
void bnsfreef(file *f_obj) {
    if (!f_obj) return;
    
    line *current = f_obj->first;
    while (current) {
        line *next = current->next;
        free(current->buff);
        free(current);
        current = next;
    }
    free(f_obj);
}

/* Prints the provided line to stdout */
void bnsprintln(line *ln) {
    if (!ln) return;

    fwrite(ln->buff, sizeof(char), ln->length, stdout);
}

file* File_new(line *first_line, size_t count) {
    file *self = (file *)xmalloc(sizeof(file));
    self->vt = &FILE_VTABLE; 
    self->first = first_line;
    self->current = first_line;
    self->lines_count = count;
    return self;
}

bool validate_separator(char c, char *separators_string) {
    for (size_t i = 0; i < strlen(separators_string); i++) { // better not make it fail, LMAO. Pray for it to have a null-terminator
        if (c == separators_string[i]) return true;
    }

    return false;
}

file* bnsreadf(FILE *fp) {
    rewind(fp);
    size_t lines_count = 0;
    line *first_line = nullptr;
    line *last_line = nullptr;

    char *current_buff = nullptr;
    size_t buff_size = 0;
    size_t current_length = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n' || c == '\b') {
            if (current_buff != nullptr) {
                line *new_line = (line *)xmalloc(sizeof(line));
                new_line->length = current_length;
                new_line->buff = current_buff;
                new_line->next = nullptr;
                new_line->vt = &LINE_VTABLE;

                if (!first_line) {
                    first_line = new_line;
                } else {
                    last_line->next = new_line;
                }
                last_line = new_line;
                lines_count++;

                current_buff = nullptr;
                buff_size = 0;
                current_length = 0;
            }
            continue;
        }
        
        if (current_buff == nullptr) {
            buff_size = LINEBUFF_DEFAULT_SIZE;
            current_buff = xmalloc(buff_size * sizeof(char));
            current_length = 0;
        } else if (current_length >= buff_size) {
            buff_size += LINEBUFF_DEFAULT_SIZE;
            current_buff = xrealloc(current_buff, buff_size * sizeof(char));
        }
        
        current_buff[current_length] = (char)c;
        current_length++;
    }

    if (current_buff != nullptr) {
        line *new_line = (line *)xmalloc(sizeof(line));
        new_line->length = current_length;
        new_line->buff = current_buff;
        new_line->next = nullptr;

        if (!first_line) {
            first_line = new_line;
        } else {
            last_line->next = new_line;
        }
        lines_count++;
    }

    return File_new(first_line, lines_count);
}

token** bnstokenise(line *ln, char *separators_string) {// Separators MUST be null terminated
    size_t tokenbuff_capacity = TOKENBUFF_DEFAULT_SIZE;
    token **tokenbuff = (token **)xmalloc(tokenbuff_capacity * sizeof(token *));
    token *first_token = nullptr;
    size_t tokens = 0;

    size_t last = 0;
    bool empty = true; // used for token-emptyness-check; starting from the assumption it is empty
    for (size_t i = 0; i < ln->length; i++) {
        if (i == ln->length-1 || validate_separator(ln->buff[i], separators_string)) { // creating token
            // printf("[[Caught token at index %zu (length: %zu)]]\n", i, i-last);
            
            if (i == last) continue; // since at some point we set last = i+1, so that white spaces aren't caught

            // emptyness checking
            empty = true;
            for (unsigned int j = last; j <= i; j++) {
                if (ln->buff[j] > 32 && ln->buff[j] <= 126) {
                    empty = false;
                    break;
                }
            }
            if (empty) {// if the token is empty then simply ignore it
                last = i;
                continue;
            }

            if (tokens+1 > tokenbuff_capacity) { // expanding tokenbuff
                tokenbuff = (token **)xrealloc((char *)tokenbuff, (tokenbuff_capacity + TOKENBUFF_DEFAULT_SIZE) * sizeof(token *));
                tokenbuff_capacity += TOKENBUFF_DEFAULT_SIZE;
            }

            // creating the token and assigning it to tokenbuff
            token *T = (token *)xmalloc(sizeof(token));
            T->buff = &ln->buff[last];
            T->length = (i == ln->length - 1) ? i - last + 1: i - last; // -1 since we're ignoring the breaking character
            T->next = nullptr;
            
            if (!first_token) {
                first_token = T;
            } else {
                token *T_current = first_token;
                token *prev = first_token;
                while (T_current) {
                    prev = T_current;
                    T_current = T_current->next;
                }
                prev->next = T;
            }
            tokenbuff[tokens++] = T;
            
            last = i+1; // resetting the count
        }
    }

    return tokenbuff;
}

void bnsprinttkn(token *T) {
    if (!T || !T->buff) return;

    fwrite(T->buff, sizeof(char), T->length, stdout);
}

bool bnscmptkn(token *T1, token *T2) {
    if (!T1 || !T2) return false;

    if (T1->length != T2->length) return false;

    bool equal = true;
    for (unsigned int i = 0; i < T1->length; i++) {
        if (T1->buff[i] != T2->buff[i]) {
            equal = false;
            break;
        }
    }

    return equal;
}
