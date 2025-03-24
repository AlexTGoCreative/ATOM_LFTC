#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

Token *tokens;
Token *lastTk;
int line = 1;

// Adaugă un token la lista de tokeni
Token *addTk(int code)
{
    Token *tk = safeAlloc(sizeof(Token));
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastTk)
    {
        lastTk->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastTk = tk;
    return tk;
}

// Extrage un substring
char *extract(const char *begin, const char *end)
{
    size_t length = end - begin;
    char *result = (char *)safeAlloc(length + 1);
    strncpy(result, begin, length);
    result[length] = '\0';
    return result;
}

// Funcția principală de tokenizare
Token *tokenize(const char *pch)
{
    const char *start;
    Token *tk;

    for (;;)
    {
        switch (*pch)
        {
        case ' ':
        case '\t':
            pch++;
            break;
        case '\r':
            if (pch[1] == '\n')
                pch++;
        case '\n':
            line++;
            pch++;
            break;
        case '\0':
            addTk(END);
            return tokens;
        case ',':
            addTk(COMMA);
            pch++;
            break;
        case ';':
            addTk(SEMICOLON);
            pch++;
            break;
        case '(':
            addTk(LPAR);
            pch++;
            break;
        case ')':
            addTk(RPAR);
            pch++;
            break;
        case '{':
            addTk(LACC);
            pch++;
            break;
        case '}':
            addTk(RACC);
            pch++;
            break;
        case '[':
            addTk(LBRACKET);
            pch++;
            break;
        case ']':
            addTk(RBRACKET);
            pch++;
            break;
        case '+':
            addTk(ADD);
            pch++;
            break;
        case '-':
            addTk(SUB);
            pch++;
            break;
        case '*':
            addTk(MUL);
            pch++;
            break;
        case '/':
            if (pch[1] == '/')
            { // Linie comentariu
                while (*pch != '\n' && *pch != '\0')
                    pch++;
            }
            else
            {
                addTk(DIV);
                pch++;
            }
            break;
        case '&':
            if (pch[1] == '&')
            {
                addTk(AND);
                pch += 2;
            }
            else
                err("Invalid character: & or missing &");
            break;
        case '|':
            if (pch[1] == '|')
            {
                addTk(OR);
                pch += 2;
            }
            else
                err("Invalid character: | or missing |");
            break;
        case '!':
            if (pch[1] == '=')
            {
                addTk(NOTEQ);
                pch += 2;
            }
            else
            {
                addTk(NOT);
                pch++;
            }
            break;
        case '<':
            if (pch[1] == '=')
            {
                addTk(LESSEQ);
                pch += 2;
            }
            else
            {
                addTk(LESS);
                pch++;
            }
            break;
        case '>':
            if (pch[1] == '=')
            {
                addTk(GREATEREQ);
                pch += 2;
            }
            else
            {
                addTk(GREATER);
                pch++;
            }
            break;
        case '=':
            if (pch[1] == '=')
            {
                addTk(EQUAL);
                pch += 2;
            }
            else
            {
                addTk(ASSIGN);
                pch++;
            }
            break;
        case '\'':
            pch++;
            tk = addTk(CHAR);
            tk->c = *pch++;
            if (*pch != '\'')
                err("Missing closing ' for CHAR");
            pch++;
            break;
        case '"':
            pch++;
            start = pch;
            while (*pch != '"' && *pch != '\0')
                pch++;
            if (*pch != '"')
                err("Missing closing \" for STRING");
            tk = addTk(STRING);
            tk->text = extract(start, pch);
            pch++;
            break;
        default:
            if (isalpha(*pch) || *pch == '_')
            {
                start = pch++;
                while (isalnum(*pch) || *pch == '_')
                    pch++;
                char *text = extract(start, pch);
                // Verificare pentru cuvinte cheie
                if (strcmp(text, "int") == 0)
                    addTk(TYPE_INT);
                else if (strcmp(text, "char") == 0)
                    addTk(TYPE_CHAR);
                else if (strcmp(text, "else") == 0)
                    addTk(ELSE);
                else if (strcmp(text, "if") == 0)
                    addTk(IF);
                else if (strcmp(text, "return") == 0)
                    addTk(RETURN);
                else if (strcmp(text, "struct") == 0)
                    addTk(STRUCT);
                else if (strcmp(text, "void") == 0)
                    addTk(VOID);
                else if (strcmp(text, "while") == 0)
                    addTk(WHILE);
                else
                {
                    tk = addTk(ID);
                    tk->text = text;
                }
            }
            else if (isdigit(*pch))
            {
                start = pch++;
                while (isdigit(*pch))
                    pch++;
                if (*pch == '.')
                {
                    pch++; 
                    while (isdigit(*pch))
                        pch++;
                    if (*pch == 'e' || *pch == 'E')
                    {
                        pch++;
                        if (*pch == '+' || *pch == '-' )
                        {
                            pch++;
                        }
                        if(isdigit(*pch))
                        {
                            while (isdigit(*pch))
                                pch++;
                        }
                        else
                        {
                            err("Invalid character: %c", *pch); 
                        }
                    }
                    tk = addTk(DOUBLE);
                    tk->d = atof(extract(start, pch)); 
                }
                else if (*pch == 'e' || *pch == 'E')
                {
                    pch++;
                    if (*pch == '+' || *pch == '-')
                    {
                        pch++;
                    }
                    if (isdigit(*pch))
                    {
                        while (isdigit(*pch))
                            pch++;
                    }
                    else
                    {
                        err("Invalid character: %c", *pch);
                    }
                    tk = addTk(DOUBLE);
                    tk->d = atof(extract(start, pch));
                }
                else
                {
                    tk = addTk(INT);
                    tk->i = atoi(extract(start, pch)); 
                }
            }
            else
            {
                err("Invalid character: %c", *pch);
            }
        }
    }
}

const char *tokenTypeToString(int code)
{
    switch (code)
    {
    case ID:
        return "ID";
    case TYPE_CHAR:
        return "TYPE_CHAR";
    case TYPE_DOUBLE:
        return "TYPE_DOUBLE";
    case ELSE:
        return "ELSE";
    case IF:
        return "IF";
    case TYPE_INT:
        return "TYPE_INT";
    case RETURN:
        return "RETURN";
    case STRUCT:
        return "STRUCT";
    case VOID:
        return "VOID";
    case WHILE:
        return "WHILE";
    case INT:
        return "INT";
    case DOUBLE:
        return "DOUBLE";
    case CHAR:
        return "CHAR";
    case STRING:
        return "STRING";
    case COMMA:
        return "COMMA";
    case SEMICOLON:
        return "SEMICOLON";
    case LPAR:
        return "LPAR";
    case RPAR:
        return "RPAR";
    case LBRACKET:
        return "LBRACKET";
    case RBRACKET:
        return "RBRACKET";
    case LACC:
        return "LACC";
    case RACC:
        return "RACC";
    case END:
        return "END";
    case ADD:
        return "ADD";
    case SUB:
        return "SUB";
    case MUL:
        return "MUL";
    case DIV:
        return "DIV";
    case DOT:
        return "DOT";
    case AND:
        return "AND";
    case OR:
        return "OR";
    case NOT:
        return "NOT";
    case ASSIGN:
        return "ASSIGN";
    case EQUAL:
        return "EQUAL";
    case NOTEQ:
        return "NOTEQ";
    case LESS:
        return "LESS";
    case LESSEQ:
        return "LESSEQ";
    case GREATER:
        return "GREATER";
    case GREATEREQ:
        return "GREATEREQ";
    default:
        return "UNKNOWN";
    }
}

// Afișare tokeni
void showTokens(const Token *tokens, FILE *out)
{
    for (const Token *tk = tokens; tk; tk = tk->next)
    {
        fprintf(out, "%d   %s", tk->line, tokenTypeToString(tk->code));
        switch (tk->code)
        {
        case ID:
            fprintf(out, " : %s", tk->text);
            break;
        case INT:
            fprintf(out, " : %d", tk->i);
            break;
        case DOUBLE:
            fprintf(out, " : %lf", tk->d);
            break;
        case CHAR:
            fprintf(out, " : '%c'", tk->c);
            break;
        case STRING:
            fprintf(out, " : \"%s\"", tk->text);
            break;
        default:
            break;
        }
        fprintf(out, "\n");
    }
}
