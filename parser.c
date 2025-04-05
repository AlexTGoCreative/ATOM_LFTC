#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"

Token *iTk;        // iterator in lista de atomi
Token *consumedTk; // ultimul atom consumat

void tkerr(const char *fmt, ...)
{
    fprintf(stderr, "error in line %d: ", iTk->line);
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

bool consume(int code)
{
    if (iTk->code == code)
    {
        consumedTk = iTk;
        iTk = iTk->next;
        return true;
    }
    return false;
}

void parse(Token *tokens)
{
    iTk = tokens;
    if (!unit())
    {
        tkerr("syntax error");
    }
    if (iTk->code != END) // Ensure we consumed everything up to END
    {
        tkerr("extra tokens after program end");
    }
}

// unit: (structDef | fnDef | varDef)* END
bool unit()
{
    while (structDef() || fnDef() || varDef())
    {
        // Continue parsing definitions
    }
    return consume(END);
}

// structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef()
{
    Token *start = iTk;
    if (!consume(STRUCT)) return false;
    
    if (!consume(ID))
    {
        iTk = start;
        return false;
    }
    
    if (!consume(LACC))
    {
        iTk = start;
        return false;
    }
    
    while (varDef())
    {
        // Consume variable definitions
    }
    
    if (!consume(RACC))
    {
        tkerr("missing } in struct definition");
    }
    
    if (!consume(SEMICOLON))
    {
        tkerr("missing ; after struct definition");
    }
    
    return true;
}

// varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef()
{
    Token *start = iTk;
    if (!typeBase()) return false;
    
    if (!consume(ID))
    {
        iTk = start;
        return false;
    }
    
    arrayDecl(); // Optional
    
    if (!consume(SEMICOLON))
    {
        iTk = start;
        return false;
    }
    
    return true;
}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase()
{
    Token *start = iTk;
    if (consume(TYPE_INT) || consume(TYPE_DOUBLE) || consume(TYPE_CHAR))
    {
        return true;
    }
    
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            return true;
        }
        iTk = start;
    }
    return false;
}

// arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl()
{
    Token *start = iTk;
    if (!consume(LBRACKET)) return false;
    
    consume(INT); // Optional
    
    if (!consume(RBRACKET))
    {
        tkerr("missing ] in array declaration");
    }
    return true;
}

// fnDef: (typeBase | VOID) ID LPAR (fnParam (COMMA fnParam)*)? RPAR stmCompound
bool fnDef()
{
    Token *start = iTk;
    if (!typeBase() && !consume(VOID)) return false;
    
    if (!consume(ID))
    {
        iTk = start;
        return false;
    }
    
    if (!consume(LPAR))
    {
        iTk = start;
        return false;
    }
    
    if (fnParam())
    {
        while (consume(COMMA))
        {
            if (!fnParam())
            {
                tkerr("missing parameter after comma");
            }
        }
    }
    
    if (!consume(RPAR))
    {
        tkerr("missing ) in function definition");
    }
    
    if (!stmCompound())
    {
        tkerr("missing function body");
    }
    
    return true;
}

// fnParam: typeBase ID arrayDecl?
bool fnParam()
{
    Token *start = iTk;
    if (!typeBase()) return false;
    
    if (!consume(ID))
    {
        iTk = start;
        return false;
    }
    
    arrayDecl(); // Optional
    return true;
}

// stm: stmCompound | IF LPAR expr RPAR stm (ELSE stm)? | WHILE LPAR expr RPAR stm | RETURN expr? SEMICOLON | expr? SEMICOLON
bool stm()
{
    Token *start = iTk;
    
    if (stmCompound()) return true;
    
    if (consume(IF))
    {
        if (!consume(LPAR)) tkerr("missing ( after if");
        if (!expr()) tkerr("invalid condition in if");
        if (!consume(RPAR)) tkerr("missing ) in if condition");
        if (!stm()) tkerr("missing statement in if");
        if (consume(ELSE))
        {
            if (!stm()) tkerr("missing statement after else");
        }
        return true;
    }
    
    if (consume(WHILE))
    {
        if (!consume(LPAR)) tkerr("missing ( after while");
        if (!expr()) tkerr("invalid condition in while");
        if (!consume(RPAR)) tkerr("missing ) in while condition");
        if (!stm()) tkerr("missing statement in while");
        return true;
    }
    
    if (consume(RETURN))
    {
        expr(); // Optional
        if (!consume(SEMICOLON)) tkerr("missing ; after return");
        return true;
    }
    
    expr(); // Optional
    if (consume(SEMICOLON)) return true;
    
    iTk = start;
    return false;
}

// stmCompound: LACC (varDef | stm)* RACC
bool stmCompound()
{
    Token *start = iTk;
    if (!consume(LACC)) return false;
    
    while (varDef() || stm())
    {
        // Continue parsing
    }
    
    if (!consume(RACC))
    {
        tkerr("missing } in compound statement");
    }
    return true;
}

// Expression parsing functions
bool expr() { return exprAssign(); }

bool exprAssign()
{
    Token *start = iTk;
    if (!exprUnary()) return false;
    
    if (consume(ASSIGN))
    {
        if (!exprAssign()) tkerr("missing expression after =");
        return true;
    }
    
    iTk = start;
    return exprOr();
}

bool exprOr()
{
    Token *start = iTk;
    if (!exprAnd()) return false;
    
    while (consume(OR))
    {
        if (!exprAnd()) tkerr("missing expression after ||");
    }
    return true;
}

bool exprAnd()
{
    Token *start = iTk;
    if (!exprEq()) return false;
    
    while (consume(AND))
    {
        if (!exprEq()) tkerr("missing expression after &&");
    }
    return true;
}

bool exprEq()
{
    Token *start = iTk;
    if (!exprRel()) return false;
    
    while (consume(EQUAL) || consume(NOTEQ))
    {
        if (!exprRel()) tkerr("missing expression after == or !=");
    }
    return true;
}

bool exprRel()
{
    Token *start = iTk;
    if (!exprAdd()) return false;
    
    while (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
    {
        if (!exprAdd()) tkerr("missing expression after relational operator");
    }
    return true;
}

bool exprAdd()
{
    Token *start = iTk;
    if (!exprMul()) return false;
    
    while (consume(ADD) || consume(SUB))
    {
        if (!exprMul()) tkerr("missing expression after + or -");
    }
    return true;
}

bool exprMul()
{
    Token *start = iTk;
    if (!exprCast()) return false;
    
    while (consume(MUL) || consume(DIV))
    {
        if (!exprCast()) tkerr("missing expression after * or /");
    }
    return true;
}

bool exprCast()
{
    Token *start = iTk;
    if (consume(LPAR))
    {
        if (!typeBase()) tkerr("invalid type in cast");
        arrayDecl(); // Optional
        if (!consume(RPAR)) tkerr("missing ) in cast");
        if (!exprCast()) tkerr("missing expression after cast");
        return true;
    }
    return exprUnary();
}

bool exprUnary()
{
    Token *start = iTk;
    if (consume(SUB) || consume(NOT))
    {
        if (!exprUnary()) tkerr("missing expression after unary operator");
        return true;
    }
    return exprPostfix();
}

bool exprPostfix()
{
    Token *start = iTk;
    if (!exprPrimary()) return false;
    
    while (true)
    {
        if (consume(LBRACKET))
        {
            if (!expr()) tkerr("missing expression in array access");
            if (!consume(RBRACKET)) tkerr("missing ] in array access");
        }
        else if (consume(DOT))
        {
            if (!consume(ID)) tkerr("missing field name after .");
        }
        else break;
    }
    return true;
}

bool exprPrimary()
{
    Token *start = iTk;
    
    if (consume(ID))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                while (consume(COMMA))
                {
                    if (!expr()) tkerr("missing expression after , in function call");
                }
            }
            if (!consume(RPAR)) tkerr("missing ) in function call");
        }
        return true;
    }
    
    if (consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(STRING))
    {
        return true;
    }
    
    if (consume(LPAR))
    {
        if (!expr()) tkerr("missing expression between parentheses");
        if (!consume(RPAR)) tkerr("missing ) in expression");
        return true;
    }
    
    iTk = start;
    return false;
}