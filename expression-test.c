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

int main()
{
    double vars[256] = {0}; // ASCII-based variable storage

    vars['x'] = 0;
    vars['y'] = 1;
    vars['z'] = 2;

    printf("Variables: x = %lf, y = %lf, z = %lf\n", vars['x'], vars['y'], vars['z']);

    char *expr[] = {
        "-(x + y) * -z", // 2
        //"-(x + y) + -z", //?
        //"-z*(-y --z)-z/z*(y-z)", //-1
        //"-z*(-y --z)", //-2
        //"-z/z*(y-z)" //1
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