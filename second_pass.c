#include "assembler.h"

int second_pass(FILE *exp_file_handler, char *file_name, plw head_IC,
                plw prv_IC) {
  int line_number = 1;
  int is_entry = FALSE;
  int error = FALSE;
  int g_error = FALSE;
  int label_base_val = 0;
  int label_offset_val = 0;
  ARE label_are;
  char line[MAX_LINE];
  char word[MAX_LINE];
  FILE *obj_file_handler;
  FILE *ent_file_handler;
  FILE *ext_file_handler;

  load_file(&obj_file_handler, file_name, "_ps.ob", "w");
  load_file(&ent_file_handler, file_name, "_ps.ent", "w");
  load_file(&ext_file_handler, file_name, "_ps.ext", "w");

  while (fgets(line, MAX_LINE, exp_file_handler)) {
    get_first_token(line, word);
    if (!strcmp(word, ".entry")) {
      is_entry = TRUE;
      get_next_token(line, word);
      error = process_entry_label(word, line_number);

    } else if (word[strlen(word) - 1] == ':')
      get_next_token(line, word);
    else if (!strcmp(word, ".extern")) {
      get_next_token(line, word);
      get_next_token(line, word);
    }

    if (!is_empty_line(line) && !error && found_label(line, word)) {
      error = get_label_values(word, &label_base_val, &label_offset_val,
                               &label_are);
      printf("at line: %d label: %s  base: %d off: %d are: %c\n", line_number,
             word, label_base_val, label_offset_val, label_are);

      if (!error && label_are == E) {
        fprintf(ext_file_handler, "%s BASE %d\n", word,
                set_next_empty(head_IC, label_are, label_base_val));
        fprintf(ext_file_handler, "%s OFFSET %d\n\n", word,
                set_next_empty(head_IC, label_are, label_base_val));
      } else if (!error) {
        if (is_entry) {
          fprintf(ent_file_handler, "%s, %d, %d\n", word, label_base_val,
                  label_offset_val);
          get_next_token(line, word);
          if (!is_empty_line(line)) {
            error = TRUE;
            fprintf(stderr,
                    "Error at line %d: Extended text after entry variable\n",
                    line_number);
          }
          is_entry = FALSE;
        } else {
          set_next_empty(head_IC, label_are, label_base_val);
          set_next_empty(head_IC, label_are, label_base_val);
        }
      }
    }

    if (error) {
      g_error = TRUE;
      error = FALSE;
    }
    line_number++;
  }

  print_listNode(head_IC);
  puts("\n");

  print_labels();
  fclose(obj_file_handler);
  fclose(ent_file_handler);
  fclose(ext_file_handler);

  print_convert_Node(head_IC);
  return g_error;
}
int process_entry_label(char *word, int line_number) {
  int error = FALSE;
  if (is_label_exists(word))
    add_entry_attribute(word);
  else {
    error = TRUE;
    fprintf(stderr, "Error at line %d: '%s' is an illegal label name\n",
            line_number, word);
  }
  return error;
}
