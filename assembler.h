
#ifndef ASSEMBLER_DOT_H
#define ASSEMBLER_DOT_H
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 7
#define MAX_LINE 81
#define MAX_ADDRESS 8191
#define MAX_EXTEN_LEN 5
#define FIRST_LINE 1
#define BASE_ARR_SIZE 3
#define BASE_MODE 16
#define BASE_IC 100
#define BASE_DC 0
#define TRUE 1
#define FALSE 0
#define EXTERN_VAL 0
#define SUM_OPERATIONS 16
#define WORD_SIZE 1 << 19
#define SET_TARG_R(x) x = x << 2
#define SET_SOUR_SORT(x) x = x << 6
#define SET_SOUR_R(x) x = x << 8
#define SET_FUNCT(x) x = x << 12
#define SET_ARE(x) x = x << 16
typedef struct TypeMacro TypeMacro;
typedef struct TypeLabel TypeLabel;
typedef struct link_words *plw;
typedef struct link_words
{
  plw next;
  unsigned int word;

  int stock_index;
} wordsNode;

typedef enum
{
  A = 4,
  R = 2,
  E = 1
} ARE;
/*WORD-LIST*/
#define IS_NULL(x)                                \
  if (x == NULL)                                  \
  {                                               \
    fprintf(stdout, "Error: index not exists\n"); \
    exit(EXIT_FAILURE);                           \
  }

typedef enum
{
  full_two_op,
  two_op,
  min_two_op,
  target_op,
  min_target_op,
  full_target_op,
  no_op
} Valid_operator;

typedef enum
{
  f_mov = 0,
  f_cmp = 0,
  f_add = 10,
  f_sub = 11,
  f_lea = 0,
  f_clr = 10,
  f_not = 11,
  f_inc = 12,
  f_dec = 13,
  f_jmp = 10,
  f_bne = 11,
  f_jsr = 12,
  f_red = 0,
  f_prn = 0

} Funct;

typedef enum
{
  mov,
  cmp,
  add,
  sub,
  lea,
  clr,
  not,
  inc,
  dec,
  jmp,
  bne,
  jsr,
  red,
  prn
} operations;

typedef enum
{
  o_mov = 0,
  o_cmp = 1,
  o_add = 2,
  o_sub = 2,
  o_lea = 4,
  o_clr = 5,
  o_not = 5,
  o_inc = 5,
  o_dec = 5,
  o_jmp = 9,
  o_bne = 9,
  o_jsr = 9,
  o_red = 12,
  o_prn = 13,
  rts = 14,
  stop = 15
} opcode;

typedef enum
{
  immediate,
  direct,
  index_sort,
  register_direct
} sortType;

typedef enum
{
  r0,
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15
} registers;

typedef enum
{
  convert_A = 16,
  convert_B = 12,
  convert_C = 8,
  convert_D = 4
} converts;

/*Functions from macros_struct.c*/
int is_macro_name(char *, TypeMacro *);
void insert_macro(FILE *, TypeMacro *, char *, int);
void macros_init(int, TypeMacro **);
void macro_add(char *, char *, int, size_t *, FILE *, TypeMacro **);
/*Functions from pre-assembler.c*/
int pre_assembler(FILE **, char *);
/*Functions label_struct.c*/
int is_valid_label_name(char *);
int base_address(int);
void label_init(int, TypeLabel **);
void label_add(int, char *, int, char *, size_t *, TypeLabel **);
void update_data_labels_address(int, TypeLabel *);
void add_entry_attribute(char *, TypeLabel *);
int is_label_exists(char *, TypeLabel *);
int get_label_values(char *, int *, int *, int, TypeLabel *);
int found_label(char *, char *, TypeLabel *);
int is_defined_ent(char *, TypeLabel *);
int is_defined_ext(char *, TypeLabel *);
/*Functions from first-pass.c*/
int first_pass(FILE *, plw *, plw *, plw *, plw *, TypeLabel **);
/*Functions from second-pass.c*/
int second_pass(FILE *, char *, plw, plw, TypeLabel *);
/*General use functions from functions.c*/
int load_file(FILE **, char *, char *, char *);
void remove_file(char *, char *);
int is_comment_line(char *);
int is_empty_line(char *);
int is_operation_name(char *);
void get_first_token(char *, char *);
int get_next_token_index(char *, int);
void get_next_token(char *, char *);
void remove_colon(char *);
void remove_signs(char *);
char **split_line(char *);
int is_sub_digits(char *, char);
int is_special_char(char *);
int is_only_digits(char *);
/*Functions from data_struct_words.c*/
void initialize_list(plw *h, plw *p, int);
void add_to_list(plw *prv, int n);
int get_word(plw h, int index);
int add_std_word(plw *, ARE are, Funct funct, registers source_r,
                 sortType source_sort, registers target_r,
                 sortType target_sort);

void convert_word(int, FILE *);
void load_obj_file(plw, FILE *);
void free_list(plw);
int add_base_word(plw *, ARE, opcode);
void add_num_to_list(plw *, ARE, int);
void update_address(plw, int);
int set_next_empty(plw, ARE, int);
int get_length(plw);
/*Functions from command_process.c*/
int command_string_process(plw *, char *, int);
int command_data_process(plw *, char *, int);
int command_code_process(plw *, char *, int);
int add_parameters(plw *, char **, opcode, Funct, Valid_operator, int);
int set_sort_and_register(char *, registers *, sortType *, ARE *);
int add_word_by_target(plw *, char *, sortType, Valid_operator, int);
int add_word_by_source(plw *, char *, sortType, Valid_operator, int);
int get_current_address(plw);
#endif
