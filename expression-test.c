#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_SIZE 100

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
        }
        else if (expr[i] == '(')
        {
            push(&ops, expr[i]);
        }
        else if (expr[i] == ')')
        {
            while (ops.top != -1 && ops.data[ops.top] != '(')
            {
                double val2 = pop(&values);
                double val1 = pop(&values);
                char op = (char)pop(&ops);
                push(&values, applyOp(val1, val2, op));
            }
            pop(&ops); // Remove '('
        }
        else
        {
            // Handle operators
            while (ops.top != -1 && precedence(ops.data[ops.top]) >= precedence(expr[i]))
            {
                double val2 = pop(&values);
                char op = (char)pop(&ops);
                double val1;
                if (values.top != -1)
                {
                    val1 = pop(&values);
                }
                else
                {
                    val1 = defaultOpValue(op);
                }
                push(&values, applyOp(val1, val2, op));
            }
            push(&ops, expr[i]);
        }
    }

    // Evaluate remaining operations
    while (ops.top != -1)
    {
        double val2 = pop(&values);
        char op = (char)pop(&ops);
        double val1;
        if (values.top != -1)
        {
            val1 = pop(&values);
        }
        else
        {
            val1 = defaultOpValue(op);
        }
        push(&values, applyOp(val1, val2, op));
    }

    return pop(&values);
}

int main()
{
    double vars[256] = {0}; // ASCII-based variable storage

    vars['x'] = 0;
    vars['y'] = 1;
    vars['z'] = 2;

    printf("Variables: x = %lf, y = %lf\n", vars['x'], vars['y']);

    char *expr[] = {
        "x + y",               // 0 + 1 = 1
        "x - y - z",           // 0 - 1 - 2 = -3
        "(x + y) * z",         // (0 + 1) * 2 = 2
        "y + (z * x)",         // 1 + (2 * 0) = 1
        "-y + z",              // -1 + 2 = 1
        "-(y + z)",            // -(1 + 2) = -3
        "x / y + z",           // 0 / 1 + 2 = 2 (assuming division by zero is handled)
        "z / (y - x)",         // 2 / (1 - 0) = 2
        "(z + y) * (x - y)",   // (2 + 1) * (0 - 1) = -3
        "x * y * z",           // 0 * 1 * 2 = 0
        "y - (z + x)",         // 1 - (2 + 0) = -1
        "-(x + y + z)",        // -(0 + 1 + 2) = -3
        "x + (y * z) - y",     // 0 + (1 * 2) - 1 = 1
        "(x + z) / y",         // (0 + 2) / 1 = 2
        "z - y - x",           // 2 - 1 - 0 = 1
        "x + y * z",           // 0 + 1 * 2 = 2
        "(x + y) * (z - y)",   // (0 + 1) * (2 - 1) = 1
        "-(x - y) + z",        // -(0 - 1) + 2 = 3
        "x + y + z",           // 0 + 1 + 2 = 3
        "x - (y + z)"          // 0 - (1 + 2) = -3
    };
    
    double results[] = {
        1,    // x + y
        -3,   // x - y - z
        2,    // (x + y) * z
        1,    // y + (z * x)
        1,    // -y + z
        -3,   // -(y + z)
        2,    // x / y + z
        2,    // z / (y - x)
        -3,   // (z + y) * (x - y)
        0,    // x * y * z
        -1,   // y - (z + x)
        -3,   // -(x + y + z)
        1,    // x + (y * z) - y
        2,    // (x + z) / y
        1,    // z - y - x
        2,    // x + y * z
        1,    // (x + y) * (z - y)
        3,    // -(x - y) + z
        3,    // x + y + z
        -3    // x - (y + z)
    };

    for (int i = 0; i < sizeof(expr) / sizeof(expr[0]); i++)
    {
        double result = evaluate(expr[i], vars);
        printf("Calculating %s Result: %lf (expected %lf)\n", expr[i], result, results[i]);
    }
}