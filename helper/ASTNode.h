/* nodes in the abstract syntax tree */
struct ast {
  int nodetype;
  int childrencount;
  vector<struct ast *> children;
};
typedef struct ast ast;

struct stringval {
  int nodetype;
  char *value;
};

/* methods for constructing nodes */
struct ast *newast(int nodetype, int childrencount, struct ast *child, ...);
struct ast *newstringval(char *value);