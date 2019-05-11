#include "9cc.h"

int pos;

void tokenize(char *p, Vector *vec) {
	while (*p) {
		Token *token = malloc(sizeof(Token));

		if (isspace(*p)) {
			p++;
			continue;
		}

		if (strncmp(p, "<=", 2) == 0) {
			token->ty = TK_LE;
			token->input = "<=";
			push_vec(vec, token);
			p += 2;
			continue;
		}

		if (strncmp(p, ">=", 2) == 0) {
			token->ty = TK_GE;
			token->input = ">=";
			push_vec(vec, token);
			p += 2;
			continue;
		}

		if (strncmp(p, "==", 2) == 0) {
			token->ty = TK_EQ;
			token->input = "==";
			push_vec(vec, token);
			p += 2;
			continue;
		}

		if (strncmp(p, "!=", 2) == 0) {
			token->ty = TK_NE;
			token->input = "!=";
			push_vec(vec, token);
			p += 2;
			continue;
		}

		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '>' || *p == '<' || *p == '%') {
			token->ty = *p;
			token->input = p;
			push_vec(vec, token);
			p++;
			continue;
		}

		if (isdigit(*p)) {
			token->ty = TK_NUM;
			token->input =p;
			token->val = strtol(p, &p, 10);
			push_vec(vec, token);
			continue;
		}

		error("can not tokenize: %s\n", p);
		exit(1);
	}

	Token *token = malloc(sizeof(Token));
	token->ty = TK_EOF;
	token->input = p;
	push_vec(vec, token);
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
	Node *node = malloc(sizeof(Node));
	node->ty = ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

int consume(int ty, Vector *vec) {
	if(((Token *)vec->data[pos])->ty != ty)
		return 0;
	pos++;
	return 1;
}

Node *equality(Vector *vec) {
	Node *node = relational(vec);

	for (;;) {
		if (consume(TK_EQ, vec))
			node = new_node(TK_EQ, node, relational(vec));
		else if(consume(TK_NE, vec))
			node = new_node(TK_NE, node, relational(vec));
		else
			return node;
	}
}

Node *relational(Vector *vec) {
	Node *node = add();

	for (;;) {
		if (consume(TK_GE, vec))
			node = new_node(TK_LE, add(vec), node);
		else if (consume('>', vec))
			node = new_node('<', add(vec), node);
		else if (consume(TK_LE, vec))
			node = new_node(TK_LE, node, add(vec));
		else if (consume('<', vec))
			node = new_node('<', node, add(vec));
	}
}

Node *add(Vector *vec) {
	Node *node = mul(vec);

	for (;;) {
		if (consume('+', vec))
			node = new_node('+', node, mul(vec));
		else if (consume('-', vec))
			node = new_node('-', node, mul(vec));
		else
			return node;
	}
}

Node *mul(Vector *vec) {
	Node *node = unary(vec);

	for (;;) {
		if (consume('*', vec))
			node = new_node('*', node, term(vec));
		else if (consume('/', vec))
			node = new_node('/', node, term(vec));
		else if (consume('%', vec))
			node = new_node('%', node, term(vec));
		else
			return node;
	}
}

Node *term(Vector *vec) {
	if (consume('(', vec)) {
		Node *node = add(vec);
		if (!consume(')', vec))
			error("開きカッコの対応する閉じカッコがありません: %s",
						((Token *)vec->data[pos])->input);
			return node;
	}

	if (((Token *)vec->data[pos])->ty == TK_NUM)
		return new_node_num(((Token *)vec->data[pos++])->val);
	error("数値でも開きカッコでもないトークンです: %s",
				((Token *)vec->data[pos])->input);
}

Node *unary(Vector *vec) {
	if (consume('+', vec))
		return term(vec);
	if (consume('-', vec))
		return new_node('-', new_node_num(0), term(vec));
	return term(vec);
}








