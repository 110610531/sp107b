#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void BLOCK();

int tempIdx = 0, labelIdx = 0;

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)
#define emit printf

int isNext(char *set) {
  char eset[SMAX], etoken[SMAX];
  sprintf(eset, " %s ", set);
  sprintf(etoken, " %s ", tokens[tokenIdx]);
  return (tokenIdx < tokenTop && strstr(eset, etoken) != NULL);
}

int isEnd() {
  return tokenIdx >= tokenTop;
}

char *next() {
  // printf("token[%d]=%s\n", tokenIdx, tokens[tokenIdx]);
  return tokens[tokenIdx++];
}

char *skip(char *set) {
  if (isNext(set)) {
    return next();
  } else {
    printf("skip(%s) got %s fail!\n", set, next());
    assert(0);
  }
}

// F = (E) | Number | Id
int F() {
  int f;
  if (isNext("(")) { // '(' E ')'//有括號讀括號,但是if已經把括號除掉了,所以不用讀
    next(); // (
    f = E();
    next(); // )
  } else { // Number | Id
    f = nextTemp();//這個是t前面的數字
    char *item = next();//這個事t等於的東西
    emit("t%d = %s\n", f, item);//把f,item所算出來的東西丟到這個算式
  }
  return f;
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > =  == >= <=")) {
    char *op = next();//*op是上一行的運算符號
    int i2 = E();
    int i = nextTemp();//nextTemp要加１,因為上一個數字已經用過了
    emit("t%d = t%d %s t%d\n", i, i1, op, i2);//把之前所算出來的直帶進去
    i1 = i;
  }
  return i1;
}

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();
  skip("=");
  int e = E();
  skip(";");
  emit("%s = t%d\n", id, e);
}

// while (E) STMT
void WHILE() {
  int whileBegin = nextLabel();
  int whileEnd = nextLabel();
  emit("(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  emit("if not T%d goto L%d\n", e, whileEnd);
  skip(")");
  STMT();
  emit("goto L%d\n", whileBegin);
  emit("(L%d)\n", whileEnd);
}

//IF = if (E) STMT (else STMT)?
void IF(){
  int elseLabel = nextLabel();
  int endifLabel = nextLabel();
  skip("if");
  skip("(");
  int e = E();
  skip(")");
  emit("ifnot t%d goto L%d\n", e, elseLabel);
  STMT();
  if(isNext("else")){
    emit("(L%d)\n",elseLabel);
    skip("else");
    STMT();
  }
  emit("(L%d)\n", endifLabel);
}

void STMT() {
  if (isNext("while"))
    return WHILE();
  else if (isNext("if"))
    IF();
  else if (isNext("{"))
    BLOCK();
  else
    ASSIGN();
}

void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// { STMT* }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

void PROG() {
  STMTS();
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}
