#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"

Token *iTk;        // the iterator in the tokens list
Token *consumedTk; // the last consumed token
char *identifier[] = {"ID", "TYPE_CHAR", "TYPE_DOUBLE", "ELSE", "IF", "TYPE_INT", "RETURN", "STRUCT", "VOID", "WHILE", "INT", "DOUBLE", "CHAR", "STRING", "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END", "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "ASSIGN", "EQUAL", "NOTEQ", "LESS", "LESSEQ", "GREATER", "GREATEREQ"};

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

char *tkCodeName(int code)
{
    return identifier[code];
}

bool consume(int code)
{
    printf("consume(%s)", tkCodeName(code));
    if (iTk->code == code || identifier[code] == "END")
    {
        consumedTk = iTk;
        iTk = iTk->next;
        printf(" => consumed\n");
        return true;
    }
    printf(" => found %s\n", tkCodeName(iTk->code));
    return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit()
{
    printf("# unit\n");
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
    printf("# structDef\n");
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
                    else
                    {
                        tkerr("lipseste ; dupa }");
                    }
                }
                else
                {
                    tkerr("lipseste } dupa {");
                }
            }
        }
        else
        {
            tkerr("lipseste numele structurii dupa struct");
        }
    }
    iTk = start;
    return false;
}

// varDef: typeBase ID arrayDecl ? SEMICOLON
bool varDef()
{
    printf("# varDef\n");
    Token *start = iTk;
    if (typeBase())
    {
        if (consume(ID))
        {
            if ((arrayDecl()))
            {
            }
            if (consume(SEMICOLON))
            {
                return true;
            }
            else
            {
                tkerr("lipseste ; la final de linie");
            }
        }
        else
        {
            tkerr("lipseste identificatorul dupa tip");
        }
    }
    iTk = start;
    return false;
}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase()
{
    printf("# typeBase\n");
    if (consume(TYPE_INT))
    {
        return true;
    }
    if (consume(TYPE_DOUBLE))
    {
        return true;
    }
    if (consume(TYPE_CHAR))
    {
        return true;
    }
    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            return true;
        }
        tkerr("lipseste numele structurii dupa struct");
    }
    return false;
}

// arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl()
{
    printf("# arrayDecl\n");
    Token *start = iTk;
    if (consume(LBRACKET))
    {
        if (consume(INT))
        {
        }
        if (consume(RBRACKET))
        {
            return true;
        }
        else
        {
            tkerr("lipseste ] dupa [ sau dupa dimensiunea specificata");
        }
    }
    iTk = start;
    return false;
}

// fnDef: ( typeBase | VOID ) ID LPAR(fnParam(COMMA fnParam)*) ? RPAR stmCompound
bool fnDef()
{
    printf("# fnDef\n");
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
                        if (fnParam())
                        {
                        }
                        else
                        {
                            tkerr("lipseste parametru dupa ,");
                        }
                    }
                }
                if (consume(RPAR))
                {
                    if (stmCompound())
                    {
                        return true;
                    }
                    else
                    {
                        tkerr("lipseste corpul functiei");
                    }
                }
                else
                {
                    tkerr("conditie invalida pentru declararea functiei sau lipseste ) dupa (");
                }
            }
        }
    }
    iTk = start;
    return false;
}

// fnParam: typeBase ID arrayDecl?
bool fnParam()
{
    printf("# fnParam\n");
    Token *start = iTk;
    if (typeBase())
    {
        if (consume(ID))
        {
            if (arrayDecl())
            {
            }
            return true;
        }
        else
        {
            tkerr("lipseste identificatorul parametrului in antetul functiei");
        }
    }
    iTk = start;
    return false;
}

// stm: stmCompound | IF LPAR expr RPAR stm(ELSE stm) ? | WHILE LPAR expr RPAR stm | RETURN expr ? SEMICOLON | expr ? SEMICOLON
bool stm()
{
    printf("# stm\n");
    if (stmCompound())
    {
        return true;
    }
    Token *start = iTk;
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
                            }
                            else
                            {
                                tkerr("lipseste continutul ramurii else");
                            }
                        }
                        return true;
                    }
                    else
                    {
                        tkerr("lipseste continului ramurii if");
                    }
                }
                else
                {
                    tkerr("conditie invalida pentru if sau lipseste ) dupa expresia din if");
                }
            }
            else
            {
                tkerr("lipseste expresia de dupa ( din if");
            }
        }
        else
        {
            tkerr("lipseste ( dupa if");
        }
    }
    iTk = start;
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
                    else
                    {
                        tkerr("lipseste corpul lui while");
                    }
                }
                else
                {
                    tkerr("conditie invalida pentru while sau lipseste ) dupa expresia din while");
                }
            }
            else
            {
                tkerr("lipseste expresia de dupa ( din while");
            }
        }
        else
        {
            tkerr("lipseste ( dupa while");
        }
    }
    iTk = start;
    if (consume(RETURN))
    {
        if (expr())
        {
        }
        if (consume(SEMICOLON))
        {
            return true;
        }
        else
        {
            tkerr("lipseste ; dupa return sau expresie return");
        }
    }
    iTk = start;
    if (expr())
    {
    }
    if (consume(SEMICOLON))
    {
        return true;
    }
    iTk = start;
    return false;
}

// stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound()
{
    printf("# stmCompound\n");
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
        else
        {
            tkerr("lipseste } dupa {");
        }
    }
    iTk = start;
    return false;
}

// expr: exprAssign
bool expr()
{
    printf("# expr\n");
    if (exprAssign())
    {
        return true;
    }
    return false;
}

// exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign()
{
    printf("# exprAssign\n");
    Token *start = iTk;
    if (exprUnary())
    {
        if (consume(ASSIGN))
        {
            if (exprAssign())
            {
                return true;
            }
            else
            {
                tkerr("lipseste expresie dupa =");
            }
        }
    }
    iTk = start;
    if (exprOr())
    {
        return true;
    }
    return false;
}

// exprOr: exprOr OR exprAnd | exprAnd
//  exprOrPrim: OR exprAnd exprOrPrim | ε  echivalent cu: exprOrPrim: ( OR exprAnd exprOrPrim )?
//  exrOr: exprAnd exprOrPrim
bool exprOrPrim()
{
    printf("# exprOrPrim\n");
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
        tkerr("lipseste expresie dupa ||");
    }
    iTk = start;
    return true;
}

bool exprOr()
{
    printf("# exprOr\n");
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

// exprAnd: exprAnd AND exprEq | exprEq
// exprAndPrim: AND exprEq exprAndPrim | ε echivalent cu: exprAndPrim: ( AND exprEq exprAndPrim )?
// exprAnd: exprEq exprAndPrim
bool exprAndPrim()
{
    printf("# exprAndPrim\n");
    Token *start = iTk;
    if (consume(AND))
    {
        if (exprEq())
        {
            if (exprAndPrim())
            {
                return true;
            }
        }
        tkerr("lipseste expresie dupa &&");
    }
    iTk = start;
    return true;
}

bool exprAnd()
{
    printf("# exprAnd\n");
    Token *start = iTk;
    if (exprEq())
    {
        if (exprAndPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel
// exprEqPrim: ( EQUAL | NOTEQ ) exprRel exprEqPrim | ε echivalent cu: exprEqPrim: (( EQUAL | NOTEQ ) exprRel exprEqPrim )?
// exprEq: exprRel exprEqPrim
bool exprEqPrim()
{
    printf("# exprEqPrim\n");
    Token *start = iTk;
    if (consume(EQUAL) || consume(NOTEQ))
    {
        if (exprRel())
        {
            if (exprEqPrim())
            {
                return true;
            }
        }
        tkerr("lipseste expresie dupa == sau !=");
    }
    iTk = start;
    return true;
}

bool exprEq()
{
    printf("# exprEq\n");
    Token *start = iTk;
    if (exprRel())
    {
        if (exprEqPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprRel: exprRel(LESS | LESSEQ | GREATER | GREATEREQ) exprAdd | exprAdd
// exprRelPrim: (LESS | LESSEQ | GREATER | GREATEREQ) exprAdd exprRelPrim | ε echivalent cu:
// exprRelPrim: ((LESS | LESSEQ | GREATER | GREATEREQ) exprAdd exprRelPrim)?
// exprRel: exprAdd exprRelPrim
bool exprRelPrim()
{
    printf("# exprRelPrim\n");
    Token *start = iTk;
    if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
    {
        if (exprAdd())
        {
            if (exprRelPrim())
            {
                return true;
            }
        }
        tkerr("lipseste expresie dupa < sau <= sau > sau >=");
    }
    iTk = start;
    return true;
}

bool exprRel()
{
    printf("# exprRel\n");
    Token *start = iTk;
    if (exprAdd())
    {
        if (exprRelPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul
// exprAddPrim: ( ADD | SUB ) exprMul exprAddPrim | ε echivalent cu: exprAddPrim: (( ADD | SUB ) exprMul exprAddPrim)?
// exprAdd: exprMul exprAddPrim
bool exprAddPrim()
{
    printf("# exprAddPrim\n");
    Token *start = iTk;
    if (consume(ADD) || consume(SUB))
    {
        if (exprMul())
        {
            if (exprAddPrim())
            {
                return true;
            }
        }
        tkerr("lipseste expresie dupa + sau -");
    }
    iTk = start;
    return true;
}

bool exprAdd()
{
    printf("# exprAdd\n");
    Token *start = iTk;
    if (exprMul())
    {
        if (exprAddPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprMul: exprMul ( MUL | DIV ) exprCast | exprCast
// exprMulPrim: ( MUL | DIV ) exprCast exprMulPrim | ε echivalent cu: exprMulPrim: (( MUL | DIV ) exprCast exprMulPrim)?
// exprMul: exprCast exprMulPrim
bool exprMulPrim()
{
    printf("# exprMulPrim\n");
    Token *start = iTk;
    if (consume(MUL) || consume(DIV))
    {
        if (exprCast())
        {
            if (exprMulPrim())
            {
                return true;
            }
        }
        tkerr("lipseste expresie dupa * sau /");
    }
    iTk = start;
    return true;
}

bool exprMul()
{
    printf("# exprMul\n");
    Token *start = iTk;
    if (exprCast())
    {
        if (exprMulPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast()
{
    printf("# exprCast\n");
    Token *start = iTk;
    if (consume(LPAR))
    {
        if (typeBase())
        {
            if (arrayDecl())
            {
            }
            if (consume(RPAR))
            {
                if (exprCast())
                {
                    return true;
                }
            }
            else
            {
                tkerr("lipseste ) dupa tipul spre care se face conversia");
            }
        }
    }
    iTk = start;
    if (exprUnary())
    {
        return true;
    }
    iTk = start;
    return false;
}

// exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary()
{
    printf("# exprUnary\n");
    Token *start = iTk;
    if (consume(SUB) || consume(NOT))
    {
        if (exprUnary())
        {
            return true;
        }
    }
    iTk = start;
    if (exprPostfix())
    {
        return true;
    }
    iTk = start;
    return false;
}

// exprPostfix: exprPostfix LBRACKET expr RBRACKET | exprPostfix DOT ID | exprPrimary
// exprPostfixPrim: LBRACKET expr RBRACKET exprPostfixPrim | DOT ID exprPostfixPrim | ε echivalent cu:
// exprPostfixPrim: ( LBRACKET expr RBRACKET exprPostfixPrim | DOT ID exprPostfixPrim )?
// exprPostfix: exprPrimary exprPostfixPrim
bool exprPostfixPrim()
{
    printf("# exprPostfixPrim\n");
    Token *start = iTk;
    if (consume(LBRACKET))
    {
        if (expr())
        {
            if (consume(RBRACKET))
            {
                if (exprPostfixPrim())
                {
                    return true;
                }
            }
            tkerr("lipseste ] dupa expresie");
        }
        tkerr("lipseste expresie dupa [");
    }
    iTk = start;
    if (consume(DOT))
    {
        if (consume(ID))
        {
            if (exprPostfixPrim())
            {
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

bool exprPostfix()
{
    printf("# exprPostfix\n");
    Token *start = iTk;
    if (exprPrimary())
    {
        if (exprPostfixPrim())
        {
            return true;
        }
    }
    iTk = start;
    return false;
}

// exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )? | INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
bool exprPrimary()
{
    printf("# exprPrimary\n");
    Token *start = iTk;
    if (consume(ID))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                while (consume(COMMA))
                {
                    if (expr())
                    {
                    }
                    else
                    {
                        tkerr("lipseste expresie dupa ,");
                    }
                }
            }
            if (consume(RPAR))
            {
                return true;
            }
            else
            {
                tkerr("apel de functie incorect sau lipseste ) dupa (");
            }
        }
        return true;
    }
    iTk = start;
    if (consume(INT))
    {
        return true;
    }
    if (consume(DOUBLE))
    {
        return true;
    }
    if (consume(CHAR))
    {
        return true;
    }
    if (consume(STRING))
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
        }
    }
    iTk = start;
    return false;
}

void parse(Token *tokens)
{
    iTk = tokens;
    if (!unit())
        tkerr("syntax error");
}