/* nodes in the abstract syntax tree */
struct ast {
  int nodetype;
  int childrencount;
  std::vector<struct ast *> children;
};
typedef struct ast ast;

struct stringval {
  int nodetype;
  char *value;
};

/* methods for constructing nodes */
struct ast *newast(int nodetype, int childrencount, ...);
struct ast *newstringval(char *value);