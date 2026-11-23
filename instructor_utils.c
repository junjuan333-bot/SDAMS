/*
 * File: instructor_utils.c
 * Module: Instructor Module - Helper & Validation Utilities
 * Description: Provides string manipulation and input buffer clearing functions.
 */
#include "instructor.h"

void inst_clear_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {

    }
}

void inst_trim_spaces(char str[]) {
    int start = 0;
    int end = (int)strlen(str) - 1;
    int i = 0;

    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }
    while (end >= start && isspace((unsigned char)str[end])) {
        end--;
    }
    while (start <= end) {
        str[i++] = str[start++];
    }
    str[i] = '\0';
}

int inst_equals_ignore_case(const char s1[], const char s2[]) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (tolower((unsigned char)s1[i]) != tolower((unsigned char)s2[i])) {
            return 0;
        }
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

int inst_contains_ignore_case(const char haystack[], const char needle[]) {
    int hLen = (int)strlen(haystack);
    int nLen = (int)strlen(needle);
    int i, j;

    if (nLen == 0) return 1;
    if (nLen > hLen) return 0;

    for (i = 0; i <= hLen - nLen; i++) {
        for (j = 0; j < nLen; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) {
                break;
            }
        }
        if (j == nLen) return 1;
    }
    return 0;
}
