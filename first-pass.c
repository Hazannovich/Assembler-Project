#include "assembler.h"
#define MAX_LABEL_LENGTH 32
#define LABEL_INIT(labels_array)                                       \
                                                                       \
    if ((labels_array = calloc(1, sizeof(TypeLabel))) == NULL)         \
    {                                                                  \
        fprintf(stderr, RED "FAILED!\n" NRM "Error: Out of memory\n"); \
        exit(EXIT_FAILURE);                                            \
    }
#define LABEL_ADD(labels_array, count, word, address, array_size)                         \
    if ((labels_array[count].name = (char *)malloc(MAX_LABEL_LENGTH)) == NULL)            \
    {                                                                                     \
        fprintf(stderr, RED "FAILED!\n" NRM "Error: Out of memory\n");                    \
        exit(EXIT_FAILURE);                                                               \
    }                                                                                     \
    strcpy(labels_array[count].name, word);                                               \
    labels_array[count++].address = address;                                              \
    if ((labels_array = realloc(labels_array, ++array_size * sizeof(TypeLabel))) == NULL) \
    {                                                                                     \
        fprintf(stderr, RED "FAILED!\n" NRM "Error: Out of memory\n");                    \
        exit(EXIT_FAILURE);                                                               \
    }                                                                                     \
    labels_array[count].address = 0;
TypeLabel *labels_array;
void removeColon(char *);
int isValidLabelName(char *);
int labelExist(char *);

int firstPass(FILE *expanded_file_ptr)
{
    int count = 0, address = 100;
    size_t array_size = 1;
    char line[MAX_LINE] = "", word[MAX_LINE] = "";
    LABEL_INIT(labels_array)

    while (fgets(line, MAX_LINE, expanded_file_ptr) != NULL)
    {
        firstWord(line, word);
        if (word[strlen(word) - 1] == ':' && !isCommentLine(line))
        {
            removeColon(word);
            if (!isValidLabelName(word))
            {
                fprintf(stderr, RED "FAILED!\n" NRM "Error: %s is an illigal label name\n", word);
                return 0;
            }
            else if (isLabelExist(word))
            {
                fprintf(stderr, RED "FAILED!\n" NRM "Error: %s label already exists\n", word);
                return 0;
            }
            if ((labels_array[count].name = (char *)malloc(MAX_LABEL_LENGTH)) == NULL)
            {
                fprintf(stderr, RED "FAILED!\n" NRM "Error: Out of memory\n");
                exit(EXIT_FAILURE);
            }
            LABEL_ADD(labels_array, count, word, address, array_size)
        }
        if (!isSpaceLine(line) && !isCommentLine())
            address += countWords(line);
        if (address > MAX_ADDRESS)
        {
            fprintf(stderr, RED "FAILED!\n" NRM "Error: Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
    return 1;
}
int countWords(line)
{
}
int isSpaceLine(char *line)
{
    unsigned int i = 0;
    while (line[i] != '\0')
        if (!isspace(line[i++]))
            return 0;
    return 1;
}
int isCommentLine(char *line)
{
    unsigned int i = 0;
    while (isspace(line[i++]))
        ;
    if (line[--i] == ';')
        return 1;
    return 0;
}
void removeColon(char *word)
{
    unsigned int i = 0;
    while (word[i++] != ':')
        ;
    word[--i] = '\0';
}
int isValidLabelName(char *word)
{
    unsigned int i;
    unsigned int arrays_length = 16;
    const char *registers_array[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    const char *operations_array[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};
    if (!isalpha(word[0]))
        return 0;
    if (strlen(word) > MAX_LABEL_LENGTH)
        return 0;
    for (i = 0; i < arrays_length; i++)
    {
        if (!strcmp(word, registers_array[i]) || !strcmp(word, operations_array[i]))
            return 0;
    }
    if (!strcmp(word, "A") || !strcmp(word, "R") || !strcmp(word, "E"))
        return 0;
    return 1;
}

int labelExist(char *word)
{
    unsigned int i;
    for (i = 0; labels_array[i].address != 0; i++)
    {
        if (!strcmp(labels_array[i].name, word))
            return 1;
    }
    return 0;
}
