# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include <vector>
# include "ASTNode.h"
# include "nodetype.h"
# include "utils.h"

/* methods for constructing nodes */
struct ast *newast(int nodetype, int childrencount, ...) {
  struct ast *a = (struct ast *)malloc(sizeof(struct ast));
  a->nodetype = nodetype;
  a->childrencount = childrencount;
  va_list ap;
  va_start(ap, childrencount);
  a->children.resize(childrencount);
  for (int i = 0; i < childrencount; i++) {
    struct ast* temp = va_arg(ap, struct ast *);
    a->children[i] = temp;
  }
  va_end(ap);
  return a;
}

struct ast* newstringval(char *value) {
  struct stringval *a = (struct stringval *)malloc(sizeof(struct stringval));
  if (!a) {
    yyerror("out of space for constructing AST");
    exit(0);
  }
  a->nodetype = STRING_NODE;
  a->value = value;
  return (struct ast *)a;
}
