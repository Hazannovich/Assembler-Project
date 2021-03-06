#include "assembler.h"

/*get line of string process the line and add the string to DC*/
int command_string_process(plw *prv_DC, char *line, int line_number)
{
    int error = FALSE;
    int i = STRING_LENGTH, j = strlen(line) - 1;
    while (line[i] == ' ')
        i++;
    while (line[j] == ' ' || line[j] == '\n')
        j--;

    if (line[i] == '\"' && line[j] == '\"')
    {
        i++;
        j--;
        while (i <= j)
        {
            add_num_to_list(prv_DC, A, line[i++]);
        }
        add_num_to_list(prv_DC, A, '\0');
    }
    else if (!is_empty_line(line))
    {
        fprintf(stdout, "Error at line %d: '%s' is an illegal string\n",
                line_number, line);
        error = TRUE;
    }

    return error;
}

/*get data line to add the values to DC*/
int command_data_process(plw *prv_DC, char *line, int line_number)
{
    int i = 0;
    int error = FALSE;
    char command[MAX_LINE] = "";
    char **s_line;
    strcat(command, line);
    s_line = split_line(command);

    while (s_line[++i] != NULL && !error) /*start the loop from i=1 because the the numbers in the data start from 1*/
    {
        /*in case it's number*/
        if (is_only_digits(s_line[i]) == 1 && i % 2 != 0)
        {
            add_num_to_list(prv_DC, A, atoi(s_line[i]));
        }
        else if ((i % 2 == 0 && strcmp(s_line[i], ",") != 0) || (i % 2 != 0 && is_only_digits(s_line[i]) == 0))
        {
            error = TRUE;
            fprintf(stdout, "Error at line %d: '%s' is an illegal operator\n",
                    line_number, s_line[i]);
        }
    }
    if (strcmp(s_line[i - 1], ",") == 0)
    {
        fprintf(stdout, "Error at line %d: Illegal extended text\n", line_number);
        error = TRUE;
    }
    free(s_line);
    return error;
}

/*check if the op char is one of the operators*/
int is_operator(char *op)
{
    int result = -1;
    char *operators_array[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};
    int i;
    for (i = 0; i < SUM_OPERATIONS; i++)
    {
        if (strcmp(op, operators_array[i]) == 0)
            result = i;
    }
    return result;
}

/*process the line of code type */
int command_code_process(plw *prv_IC, char *line, int line_number)
{
    int error = FALSE;
    int operator;
    char **s_line;
    char command[MAX_LINE] = "";
    strcat(command, line);
    s_line = split_line(command);

    operator= is_operator(s_line[0]);

    switch (operator)
    {
    case mov:
        error = add_parameters(prv_IC, s_line + 1, o_mov, f_mov, two_op, line_number);
        break;

    case cmp:
        error = add_parameters(prv_IC, s_line + 1, o_cmp, f_cmp, full_two_op, line_number);
        break;
    case add:
        error = add_parameters(prv_IC, s_line + 1, o_add, f_add, two_op, line_number);
        break;
    case sub:
        error = add_parameters(prv_IC, s_line + 1, o_sub, f_sub, two_op, line_number);
        break;
    case lea:
        error = add_parameters(prv_IC, s_line + 1, o_lea, f_lea, min_two_op, line_number);
        break;
    case clr:
        error = add_parameters(prv_IC, s_line + 1, o_clr, f_clr, target_op, line_number);
        break;
    case not:
        error = add_parameters(prv_IC, s_line + 1, o_not, f_not, target_op, line_number);
        break;
    case inc:
        error = add_parameters(prv_IC, s_line + 1, o_inc, f_inc, target_op, line_number);
        break;
    case dec:
        error = add_parameters(prv_IC, s_line + 1, o_dec, f_dec, target_op, line_number);
        break;
    case jmp:
        error = add_parameters(prv_IC, s_line + 1, o_jmp, f_jmp, min_target_op, line_number);
        break;
    case bne:
        error = add_parameters(prv_IC, s_line + 1, o_bne, f_bne, min_target_op, line_number);
        break;
    case jsr:
        error = add_parameters(prv_IC, s_line + 1, o_jsr, f_jsr, min_target_op, line_number);
        break;
    case red:
        error = add_parameters(prv_IC, s_line + 1, o_red, f_red, target_op, line_number);
        break;
    case prn:
        error = add_parameters(prv_IC, s_line + 1, o_prn, f_prn, full_target_op, line_number);
        break;
    case rts:
        error = add_parameters(prv_IC, s_line + 1, rts, 0, no_op, line_number);
        break;
    case stop:
        error = add_parameters(prv_IC, s_line + 1, stop, 0, no_op, line_number);
        break;
    default:
        error = TRUE;
        fprintf(stdout, "Error at line %d: '%s' is an illegal command\n",
                line_number, *s_line);
        break;
    }

    free(s_line);
    return error;
}

/*process the parameters of the operators*/
int add_parameters(plw *prv, char **comm, opcode opcode, Funct funct, Valid_operator op, int line_number)
{
    int error = FALSE;
    registers source_r = 0, target_r = 0;
    sortType source_sort = 0, target_sort = 0;
    ARE are = 0;
    /*1) get the size of element*/
    int i = 0, size = 0;
    add_base_word(prv, A, opcode);

    while (comm[i] != NULL)
    {
        i++;
        size++;
        if (strcmp(comm[i - 1], "") == 0)
            size--;
    }

    if (size == 3 && *comm[1] == ',') /*case two operators*/
    {

        error |= !set_sort_and_register(comm[0], &source_r, &source_sort, &are);
        error |= !set_sort_and_register(comm[2], &target_r, &target_sort, &are);

        add_std_word(prv, are, funct, source_r, source_sort, target_r, target_sort);

        error |= add_word_by_source(prv, comm[0], source_sort, op, line_number);
        error |= add_word_by_target(prv, comm[2], target_sort, op, line_number);
    }
    else if (size == 1) /*case one operator*/
    {
        error |= !set_sort_and_register(comm[0], &target_r, &target_sort, &are);
        error |= !add_std_word(prv, are, funct, source_r, source_sort, target_r, target_sort);
        error |= add_word_by_target(prv, comm[0], target_sort, op, line_number);
    }
    else if (size != 0)
    {
        fprintf(stdout, "Error at line %d: Illegal command\n", line_number);
        error = TRUE;
    }

    return error;
}

/*for adding words for the source*/
int add_word_by_source(plw *prv, char *comm, sortType source_sort, Valid_operator op, int line_number)
{
    int error = FALSE;
    if (op >= target_op)
    {
        error = TRUE;
        fprintf(stdout, "Error at line %d: '%s' is an illegal operand\n",
                line_number, comm);
    }
    else
    {
        switch (source_sort)
        {
        case immediate:
            if ((op == full_two_op || op == two_op) && is_only_digits(comm + 1))
            {
                add_num_to_list(prv, A, atoi(comm + 1));
            }
            else
            {
                error = TRUE;
                fprintf(stdout, "Error at line %d: '%s' is an illegal operand\n",
                        line_number, comm);
            }
            break;
        case direct:
        case index_sort:
            add_to_list(prv, 0);
            add_to_list(prv, 0);
        case register_direct:
            if (op == min_two_op && source_sort == register_direct)
            {
                error = TRUE;
                fprintf(stdout, "Error at line %d: '%s' is an illegal operand\n",
                        line_number, comm);
            }
            break;
        }
    }
    return error;
}

/*for adding words for the target*/
int add_word_by_target(plw *prv, char *comm, sortType target_sort, Valid_operator op, int line_number)
{
    int error = FALSE;
    switch (target_sort)
    {
    case immediate:
        if ((op == full_two_op || op == full_target_op) && is_only_digits(comm + 1))
        {
            add_num_to_list(prv, A, atoi(comm + 1));
        }
        else
        {
            error = TRUE;
            fprintf(stdout, "Error at line %d: '%s' is an illegal operand\n", line_number, comm);
        }
        break;
    case direct:
    case index_sort:
        if (op != no_op)
        {
            add_to_list(prv, 0);
            add_to_list(prv, 0);
        }
        else
        {
            error = TRUE;
            fprintf(stdout, "Error at line %d: '%s' is an illegal operand\n",
                    line_number, comm);
        }
        break;
    case register_direct:
        if ((op == min_target_op || op == no_op) && target_sort == register_direct)
        {
            error = TRUE;
            fprintf(stdout, "Error at line %d: '%s' is an illegal operand\n",
                    line_number, comm);
        }
        break;
    }

    return error;
}
/*process the sort type and the registers*/
int set_sort_and_register(char *operator, registers * r, sortType *sort, ARE *are)
{

    int result = 1;
    int temp;
    *are = A;
    /*case it's register*/
    temp = atoi(operator+ 1);
    if (operator[0] == 'r' && operator[1] != '\0' && is_only_digits(operator+ 1) && temp >= r0 && temp <= r15)
    {
        *r = temp;
        *sort = register_direct;
    }
    /*case it's number*/
    else if (operator[0] == '#')
    {
        *r = r0;
        *sort = immediate;
    }
    /*case it's variable*/
    else if (isalpha(operator[0]))
    {
        /*case illegal variable*/
        if (isdigit(operator[0]) || is_operator(operator) >= 0 || is_special_char(operator))
        {
            result = 0;
        }
        else
        {
            int parenthesis = 0;
            int reg;
            int i = 0;
            while (operator[i] != '\0')
            {
                if (parenthesis == 1 && operator[i] == 'r' && is_sub_digits(operator+ i, ']') == 0 && (reg = atoi(operator+ i + 1)) <= r15 && reg >= r0)
                {
                    *r = reg;
                    *sort = index_sort;
                    parenthesis++;
                }
                if (operator[i] == '[' || operator[i] == ']')
                    parenthesis++;
                i++;
            }
            if (!parenthesis)
            {
                *r = r0;
                *sort = direct;
            }
            else if (parenthesis != 3)
                result = 0;
        }
    }
    else
        result = 0;

    return result;
}