#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
//Git Commit
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
}

// unit: (structDef | fnDef | varDef)* END
bool unit()
{
    for (;;)
    {
        if (structDef())
        {
        }
        else if (fnDef())
        {
        }
        else if (varDef())
        {
        }
        else
            break;
    }
    if (consume(END))
    {
        return true;
    }
    return false;
}

// structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef()
{
    Token *start = iTk;
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            if (consume(LACC))
            {
                while (varDef())
                {
                }
                if (consume(RACC))
                {
                    if (consume(SEMICOLON))
                    {
                        return true;
                    }
                    tkerr("lipseste ; dupa }");
                }
                tkerr("lipseste } la definirea structurii");
            }
            tkerr("lipseste { dupa definirea structurii");
        }
        tkerr("lipseste numele structurii dupa STRUCT");
    }
    iTk = start;
    return false;
}

// varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef()
{
    Token *start = iTk;
    if (typeBase())
    {
        if (consume(ID))
        {
            arrayDecl();
            if (consume(SEMICOLON))
            {
                return true;
            }
            tkerr("lipseste ; la definirea variabilei");
        }
        tkerr("lipseste numele variabilei");
    }
    iTk = start;
    return false;
}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase()
{
    if (consume(TYPE_INT))
        return true;
    if (consume(TYPE_DOUBLE))
        return true;
    if (consume(TYPE_CHAR))
        return true;
    if (consume(STRUCT))
    {
        if (consume(ID))
            return true;
        tkerr("lipseste numele structurii dupa STRUCT");
    }
    return false;
}

// arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl()
{
    Token *start = iTk;
    if (consume(LBRACKET))
    {
        consume(INT);
        if (consume(RBRACKET))
        {
            return true;
        }
        tkerr("lipseste ] la declararea array-ului");
    }
    iTk = start;
    return false;
}

// fnDef: (typeBase | VOID) ID LPAR (fnParam (COMMA fnParam)*)? RPAR stmCompound
bool fnDef()
{
    Token *start = iTk;
    if (typeBase() || consume(VOID))
    {
        if (consume(ID))
        {
            if (consume(LPAR))
            {
                if (fnParam())
                {
                    while (consume(COMMA))
                    {
                        if (!fnParam())
                        {
                            tkerr("lipseste parametrul dupa ,");
                        }
                    }
                }
                if (consume(RPAR))
                {
                    if (stmCompound())
                    {
                        return true;
                    }
                    tkerr("lipseste corpul functiei");
                }
                tkerr("lipseste ) la definirea functiei");
            }
            tkerr("lipseste ( la definirea functiei");
        }
        tkerr("lipseste numele functiei");
    }
    iTk = start;
    return false;
}

// fnParam: typeBase ID arrayDecl?
bool fnParam()
{
    Token *start = iTk;
    if (typeBase())
    {
        if (consume(ID))
        {
            arrayDecl();
            return true;
        }
        tkerr("lipseste numele parametrului");
    }
    iTk = start;
    return false;
}

// stm: stmCompound | IF LPAR expr RPAR stm (ELSE stm)? | WHILE LPAR expr RPAR stm | RETURN expr? SEMICOLON | expr? SEMICOLON
bool stm()
{
    Token *start = iTk;
    if (stmCompound())
    {
        return true;
    }
    if (consume(IF))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                if (consume(RPAR))
                {
                    if (stm())
                    {
                        if (consume(ELSE))
                        {
                            if (stm())
                            {
                                return true;
                            }
                            tkerr("lipseste instructiunea dupa else");
                        }
                        return true;
                    }
                    tkerr("lipseste instructiunea pentru if");
                }
                tkerr("lipseste ) dupa conditia if");
            }
            tkerr("conditie invalida pentru if");
        }
        tkerr("lipseste ( dupa if");
    }
    if (consume(WHILE))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                if (consume(RPAR))
                {
                    if (stm())
                    {
                        return true;
                    }
                    tkerr("lipseste instructiunea pentru while");
                }
                tkerr("lipseste ) dupa conditia while ");
            }
            tkerr("conditie invalida pentru while");
        }
        tkerr("lipseste ( dupa while");
    }
    if (consume(RETURN))
    {
        expr();
        if (consume(SEMICOLON))
        {
            return true;
        }
        tkerr("lipseste ; dupa return");
    }
    if (expr())
    {
        if (consume(SEMICOLON))
        {
            return true;
        }
        tkerr("lipseste ; dupa expresie");
    }
    if (consume(SEMICOLON))
    {
        return true;
    }
    iTk = start;
    return false;
}

// stmCompound: LACC (varDef | stm)* RACC
bool stmCompound()
{
    Token *start = iTk;
    if (consume(LACC))
    {
        while (varDef() || stm())
        {
        }
        if (consume(RACC))
        {
            return true;
        }
        tkerr("lipseste } la sfarsitul blocului");
    }
    iTk = start;
    return false;
}

// expr: exprAssign
bool expr()
{
    return exprAssign();
}

// exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign()
{
    Token *start = iTk;
    if (exprUnary())
    {
        if (consume(ASSIGN))
        {
            if (exprAssign())
            {
                return true;
            }
            tkerr("lipseste expresia din dreapta asignarii");
        }
    }
    iTk = start;
    return exprOr();
}

// exprOr: exprAnd exprOrPrim
bool exprOr()
{
    Token *start = iTk;
    if (exprAnd())
    {
        if (exprOrPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprOrPrim: OR exprAnd exprOrPrim | ε
bool exprOrPrim()
{
    Token *start = iTk;
    if (consume(OR))
    {
        if (exprAnd())
        {
            if (exprOrPrim())
            {
                return true;
            }
        }
        tkerr("lipseste expresia dupa ||");
    }
    iTk = start;
    return true;
}

// exprAnd: exprEq exprAndPrim (implicit transformation)
bool exprAnd()
{
    Token *start = iTk;
    if (exprEq())
    {
        while (consume(AND))
        {
            if (!exprEq())
            {
                tkerr("lipseste expresia dupa &&");
            }
        }
        return true;
    }
    iTk = start;
    return false;
}

// exprEq: exprRel exprEqPrim (implicit transformation)
bool exprEq()
{
    Token *start = iTk;
    if (exprRel())
    {
        while (consume(EQUAL) || consume(NOTEQ))
        {
            if (!exprRel())
            {
                tkerr("lipseste expresia dupa == sau !=");
            }
        }
        return true;
    }
    iTk = start;
    return false;
}

// exprRel: exprAdd exprRelPrim (implicit transformation)
bool exprRel()
{
    Token *start = iTk;
    if (exprAdd())
    {
        while (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
        {
            if (!exprAdd())
            {
                tkerr("lipseste expresia dupa operator relational");
            }
        }
        return true;
    }
    iTk = start;
    return false;
}

// exprAdd: exprMul exprAddPrim (implicit transformation)
bool exprAdd()
{
    Token *start = iTk;
    if (exprMul())
    {
        while (consume(ADD) || consume(SUB))
        {
            if (!exprMul())
            {
                tkerr("lipseste expresia dupa + sau -");
            }
        }
        return true;
    }
    iTk = start;
    return false;
}

// exprMul: exprCast exprMulPrim (implicit transformation)
bool exprMul()
{
    Token *start = iTk;
    if (exprCast())
    {
        while (consume(MUL) || consume(DIV))
        {
            if (!exprCast())
            {
                tkerr("lipseste expresia dupa * sau /");
            }
        }
        return true;
    }
    iTk = start;
    return false;
}

// exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast()
{
    Token *start = iTk;
    if (consume(LPAR))
    {
        if (typeBase())
        {
            arrayDecl();
            if (consume(RPAR))
            {
                if (exprCast())
                {
                    return true;
                }
                tkerr("lipseste expresia dupa cast");
            }
            tkerr("lipseste ) la cast");
        }
        tkerr("tip invalid pentru cast");
    }
    iTk = start;
    return exprUnary();
}

// exprUnary: (SUB | NOT) exprUnary | exprPostfix
bool exprUnary()
{
    Token *start = iTk;
    if (consume(SUB) || consume(NOT))
    {
        if (exprUnary())
        {
            return true;
        }
        tkerr("lipseste expresia dupa operator unar");
    }
    iTk = start;
    return exprPostfix();
}

// exprPostfix: exprPostfix LBRACKET expr RBRACKET | exprPostfix DOT ID | exprPrimary
bool exprPostfix()
{
    Token *start = iTk;
    if (!exprPrimary())
    {
        iTk = start;
        return false;
    }
    for (;;)
    {
        if (consume(LBRACKET))
        {
            if (expr())
            {
                if (consume(RBRACKET))
                {
                    continue;
                }
                tkerr("lipseste ] la accesarea array-ului");
            }
            tkerr("lipseste expresia intre [ ]");
        }
        if (consume(DOT))
        {
            if (consume(ID))
            {
                continue;
            }
            tkerr("lipseste numele campului dupa .");
        }
        break;
    }
    return true;
}

// exprPrimary: ID (LPAR (expr (COMMA expr)*)? RPAR)? | INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
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
                    if (!expr())
                    {
                        tkerr("lipseste expresia dupa , in apelul functiei");
                    }
                }
            }
            if (consume(RPAR))
            {
                return true;
            }
            tkerr("lipseste ) la apelul functiei");
        }
        return true;
    }
    if (consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(STRING))
    {
        return true;
    }
    if (consume(LPAR))
    {
        if (expr())
        {
            if (consume(RPAR))
            {
                return true;
            }
            tkerr("lipseste ) in expresie");
        }
        tkerr("lipseste expresia intre paranteze");
    }
    iTk = start;
    return false;
}