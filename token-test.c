#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPRESSIONS 10
#define MAX_OPERATORS 10
#define MAX_LINE_LENGTH 100

typedef struct
{
    char command[20];
    char expressions[MAX_EXPRESSIONS][20];
    char operators[MAX_OPERATORS][5];
    int expr_count;
    int op_count;
} ParsedLine;

void parseLine(const char *line, ParsedLine *parsed)
{
    char buffer[MAX_LINE_LENGTH];
    strcpy(buffer, line);

    char *token = strtok(buffer, " ");
    if (token)
    {
        strcpy(parsed->command, token);
    }

    parsed->expr_count = 0;
    parsed->op_count = 0;

    while ((token = strtok(NULL, " ")) != NULL)
    {
        if (strchr("=<>+-*/^,", token[0]) ||
            strcmp(token, "AND") == 0 ||
            strcmp(token, "OR") == 0 ||
            strcmp(token, "XOR") == 0 ||
            strcmp(token, "MOD") == 0)
        {
            strcpy(parsed->operators[parsed->op_count++], token);
        }
        else
        {
            strcpy(parsed->expressions[parsed->expr_count++], token);
        }
    }
}

void printParsedLine(const ParsedLine *parsed)
{
    printf("Command: %s\n", parsed->command);
    printf("Expressions: ");
    for (int i = 0; i < parsed->expr_count; i++)
    {
        printf("%s ", parsed->expressions[i]);
    }
    printf("\nOperators: ");
    for (int i = 0; i < parsed->op_count; i++)
    {
        printf("%s ", parsed->operators[i]);
    }
    printf("\n");
}

int main()
{
    char *line[] = {
        "1 END",
        "2 CLEAR",
        "3 NEXT",
        "4 WEND",
        "5 DO ",
        "6 GOTO 10",
        "7 Goto a",
        "8 WHILE var >= 10",
        "9 UNTIL c = d",
        "10 PRINT 0, 255",
        "11 INPUT e , var",
        "12 LET Var = 120",
        "13 POKE 220, Value",
        "14 FOR Var = 1 TO b",
        "15 IF t <> 100 THEN 10 ELSE c",
        "16 a = PEEK 100",
        "17 b = ABS b",
        "18 c = SGN -100",
        "19 d = INT 10",
        "20 e = SQR a",
        "21 f = EXP 10",
        "22 g = LOG Value",
        "23 h = LOG10 Value",
        "24 i = SIN Value",
        "25 Value = COS Value",
        "26 Value = DEG Value",
        "27 Value = RAD Value",
        "28 Value = NOT Value",
        "29 Value = SHL Value , 100",
        "30 Value = SHR 20,20",
        "31 Value = Value+Val2",
        "32 Value = a-100",
        "33 Value = Value / Value",
        "34 Value = Value * Value",
        "35 Value = Value MOD Value",
        "36 Value = Value AND Value",
        "37 Value = Value OR Value",
        "38 Value = Value XOR Value",
        "39 c = SGN -100",
        "40 d = INT - 10",
        "41 e = a-b",
        "42 f = a- b",
        "43 g = a -b",
        "44 h = a - 10",
        "45 i = a -10",
        "46 Value = a-10",
        "47 GOTO -10",
        "48 GOTO - 10",
        "49  WHILE var >= -100",
        "50  UNTIL c =-2",
        "51 PRINT 0, -255"
        "52 INPUT e ,-10"
        "53 LET Var =-120"
        "54 POKE 220, Value"
        "55 FOR Var = -10 TO -1"
        "56 IF t <> -100 THEN 10 ELSE c",
    };

    ParsedLine parsed;

    for (int i = 0; i < sizeof(line) / sizeof(line[0]); i++)
    {
        line[strcspn(line[i], "\n")] = 0; // Remove newline character
        parseLine(line[i], &parsed);
        printParsedLine(&parsed);
    }

    return 0;
}