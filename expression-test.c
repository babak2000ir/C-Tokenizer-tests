#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_SIZE 100
int debug = 0;

// Stack for operators and operands
typedef struct
{
    double data[MAX_EXPR_SIZE];
    int top;
} Stack;

void initStack(Stack *s)
{
    s->top = -1;
}

void push(Stack *s, double value)
{
    if (s->top < MAX_EXPR_SIZE - 1)
    {
        s->data[++(s->top)] = value;
    }
    else
    {
        printf("Stack overflow!\n");
        exit(1);
    }
}

double pop(Stack *s)
{
    if (s->top >= 0)
    {
        return s->data[(s->top)--];
    }
    else
    {
        printf("Stack underflow!\n");
        exit(1);
    }
}

int precedence(char op)
{
    switch (op)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    case '_':
        return 4;
    default:
        return 0;
    }
}

double applyOp(double a, double b, char op)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
    case '_':
        return a - b;
    case '*':
        return a * b;
    case '/':
        return a / b;
    case '^':
        return pow(a, b);
    default:
        return 0;
    }
}

double defaultOpValue(char op)
{
    switch (op)
    {
    case '+':
        return 0;
    case '-':
    case '_':
        return 0;
    case '*':
        return 1;
    case '/':
        return 1;
    case '^':
        return 1;
    default:
        return 0;
    }
}

void printStack(Stack *s)
{
    if (!debug)
        return;

    printf("[ ");
    for (int i = 0; i <= s->top; i++)
    {
        if (s->data[i] > 10)
            printf("%c", (int)(s->data[i]));
        else
            printf("%i", (int)s->data[i]);

        if (i < s->top)
            printf(" ");
    }
    printf(" ]\n");
}

int is_right_associative(char op)
{
    return (op == '^' || op == '_');
}

void calculate(Stack *values, Stack *ops)
{
    double val1 = pop(values);
    char op = (char)pop(ops);
    double val2;
    if (values->top != -1 && op != '_')
    {
        val2 = pop(values);
    }
    else
    {
        val2 = defaultOpValue(op);
    }
    push(values, applyOp(val2, val1, op));

    if (!debug)
        return;
    printf("%i %c %i = %li", (int)val2, op, (int)val1, (int)applyOp(val2, val1, op));
    printf("\t\t\t");
    printStack(values);
    printf("\t\t\t\t\t\t");
    printStack(ops);
}

double evaluate(char *expr, double vars[256])
{
    Stack values, ops;
    initStack(&values);
    initStack(&ops);

    for (int i = 0; expr[i]; i++)
    {
        if (isspace(expr[i]))
            continue;

        if (isalpha(expr[i]))
        {
            // Handle variables
            push(&values, vars[(int)expr[i]]);

            if (debug)
            {
                printf("%c = %i \t\t->\t\t", (int)expr[i], (int)vars[(int)expr[i]]);
                printStack(&values);
            }
        }
        else if (isdigit(expr[i]))
        {
            // Handle numbers
            double val = 0;
            while (expr[i] && isdigit(expr[i]))
            {
                val = (val * 10) + (expr[i] - '0');
                i++;
            }
            i--;
            push(&values, val);

            if (debug)
            {
                printf("%i \t\t->\t", (int)val);
                printStack(&values);
            }
        }
        else if (expr[i] == '(')
        {
            push(&ops, expr[i]);

            if (debug)
            {
                printf("%c\t\t->\t\t\t\t", expr[i]);
                printStack(&ops);
            }
        }
        else if (expr[i] == ')')
        {
            while (ops.top != -1 && ops.data[ops.top] != '(')
            {
                calculate(&values, &ops);
            }
            pop(&ops); // Remove '('

            if (debug)
            {
                printf("%c\t\t<-\t\t\t\t", expr[i]);
                printStack(&ops);
            }
        }
        else
        {
            // Handle operators
            while (ops.top != -1 &&
                   ((is_right_associative(expr[i]) &&
                     precedence(ops.data[ops.top]) > precedence(expr[i])) ||
                    (!is_right_associative(expr[i]) &&
                     precedence(ops.data[ops.top]) >= precedence(expr[i]))))
            {
                calculate(&values, &ops);
            }
            push(&ops, expr[i]);

            if (debug)
            {
                printf("%c\t\t->\t\t\t\t", expr[i]);
                printStack(&ops);
            }
        }
    }

    // Evaluate remaining operations
    while (ops.top != -1)
    {
        calculate(&values, &ops);
    }

    return pop(&values);
}

char getPreviousChar(char *expr, int index)
{
    if (index == 0)
    {
        return '\0';
    }
    while (index > 0 && isspace(expr[index]))
    {
        index--;
    }
    return expr[index];
}

int isUnaryMinus(char *expr, int index)
{
    if (index == 0 || strchr("+-*/(", getPreviousChar(expr, index - 1)))
    {
        return 1;
    }
    return 0;
}

void replace_unary_minus(char *expr)
{
    int len = strlen(expr);
    int j = 0;

    for (int i = 0; i < len; i++)
    {
        if (expr[i] == '-')
        {
            // Check if it's unary (start of expression or after operator/parenthesis)
            if (i == 0 || strchr("+-*/(", getPreviousChar(expr, i - 1)))
            {
                expr[j++] = '_';
                continue; // Skip normal '-' placement
            }
        }
        expr[j++] = expr[i];
    }
    expr[j] = '\0'; // Null-terminate string
}

int main(int argc, char *argv[])
{
    // debug parameter from command line

    if (argc > 1 && strcmp(argv[1], "debug") == 0)
    {
        debug = 1;
    }

    double vars[256] = {0}; // ASCII-based variable storage

    vars['x'] = 0;
    vars['y'] = 1;
    vars['z'] = 2;

    printf("Variables: x = %lf, y = %lf, z = %lf\n", vars['x'], vars['y'], vars['z']);

    char *expr[] = {
        "-(x + y) * -z",         // 2
        "-(x + y) + -z",         //-3
        "-z*(-y --z)-z/z*(y-z)", //-1
        "-z*(-y --z)",           //-2
        "-z/z*(y-z)"             // 1
    };

    for (int i = 0; i < sizeof(expr) / sizeof(expr[0]); i++)
    {
        // Replace unary - with _
        replace_unary_minus(expr[i]);

        printf("Expression with uniary minus _ : %s\n", expr[i]);

        double result = evaluate(expr[i], vars);
        printf("Calculating %s Result: %lf\n", expr[i], result);
    }
}