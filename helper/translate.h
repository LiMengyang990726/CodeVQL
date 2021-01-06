void translateImportStmt(struct ast *);

void translateDefineStmt(struct ast *);
vector<string> translateMultipleVersionType1Opts(struct ast *a, vector<string>& initial);
void translateDefineOpts(struct ast *a);

void translateFrom(struct ast *);
void translateRange(struct ast *);
void translateWhere(struct ast *);
void translateSelect(struct ast *);

void translateFormula(struct ast *);
void translateCall(struct ast *);
void translateComparison(struct ast *);

void eval(struct ast *);