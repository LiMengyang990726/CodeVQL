enum ASTNodeType {
  UNDEFINED,
  IMPORT_STATEMENT,
  BINARY_EXPR,
};

class ASTNode
{
public:
  ASTNodeType m_type;

  ASTNode(ASTNodeType type = UNDEFINED) : m_type(type) { }
  
  ~ASTNode() { }

  int type() const { return internalGetType(this); }

private:
  static int internalGetType(const ASTNode *node)
  {
    return node ? node->m_type : ASTNodeType::UNDEFINED;
  }
};

class ASTBinary : public ASTNode
{
public:
  enum BinOp {
    BIN_ADD,
    BIN_SUBTRACT,
  };

  ASTBinary(ASTNode &left, ASTNode &right, int op,
            ASTNodeType type = ASTNodeType::BINARY_EXPR)
    : ASTNode(type), m_op(op), m_left(std::move(left)), m_right(std::move(right)) { }

  int op() const { return m_op; }

protected:
  int m_op;

private:
  ASTNode m_left;
  ASTNode m_right;
};
