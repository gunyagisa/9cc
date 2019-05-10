#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//トークンの型を表す値
enum {
	TK_NUM = 256, //整数トークン
	TK_EOF,       //入力の終わりを表すトークン
	TK_EQ,        //==
	TK_NE,        //!=
	TK_LE,        //< <=
	TK_GE,        //> >=
};

//トークンの型
typedef struct{
	int ty;
	int val;
	char *input;
} Token;

//トークナイズした結果のトークン列はこの配列に保存する
//１００個以上のトークンは来ないものとする
int pos = 0;

//vector
typedef struct {
	void **data;
	int capacity;
	int len;
} Vector;

Vector *new_vector() {
	Vector *vec = malloc(sizeof(Vector));
	vec->data = malloc(sizeof(void *) * 16);
	vec->capacity = 16;
	vec->len = 0;
	return vec;
}

void push_vec(Vector *vec, void *elem) {
	if (vec->len == vec->capacity) {
		vec->capacity *= 2;
		vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
	}
	vec->data[vec->len++] = elem;
}

//error
void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}


//test function
void printer(Vector *vec) {
	int i;
	printf("capacity:%d, len:%d\n", vec->capacity, vec->len);
	for (i = 0;vec->len > i;i++) {
		Token *token = vec->data[i];
		printf("pos:%d, ty:%d, iniput:%s\n", i, token->ty, token->input);
	}
}



void tokenize(char *p, Vector *vec){
	while(*p){
		Token *token = malloc(sizeof(Token));

		if(isspace(*p)){
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

		if (strncmp(p, "!=", 2) == 0){
			token->ty = TK_NE;
			token->input = "==";
			push_vec(vec, token);
			p += 2;
			continue;
		}

		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||  *p == '(' || *p == ')' || *p == '>' || *p == '<' || *p == '%') {
			token->ty = *p;
			token->input = p;
			push_vec(vec, token);
			p++;
			continue;
		}

		if(isdigit(*p)){
			token->ty = TK_NUM;
			token->input = p;
			token->val = strtol(p, &p, 10);
			push_vec(vec, token);
			continue;
		}


		error("トークナイズできません: %s", p);
		exit(1);
	}
	Token *token = malloc(sizeof(Token));
	token->ty = TK_NUM;
	token->input = p;
	push_vec(vec, token);
}

//構文解析
enum {
	ND_NUM = 256,
};

typedef struct Node {
	int ty;
	struct Node *lhs; //左辺
	struct Node *rhs; //右辺
	int val;
}	Node;

Node *relational(Vector *);
Node *add(Vector *);
Node *term(Vector *);
Node *mul(Vector *);
Node *unary(Vector *);

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
	if (((Token *)vec->data[pos])->ty != ty)
		return 0;
	pos++;
	return 1;

}

Node *equality(Vector *vec) {
	Node *node = relational(vec);

	for (;;) {
		if (consume(TK_EQ, vec))
			node = new_node(TK_EQ, node, relational(vec));
		else if (consume(TK_NE, vec))
			node = new_node(TK_NE, node, relational(vec));
		else
			return node;
	}
}

Node *relational(Vector *vec) {
	Node *node = add(vec);

	for (;;) {
		if (consume(TK_GE, vec))
			node = new_node(TK_LE, add(vec), node);
		else if (consume('>', vec))
			node = new_node('<', add(vec), node);
		else if (consume(TK_LE, vec))
			node = new_node(TK_LE, node, add(vec));
		else if (consume('<', vec))
			node = new_node('<', node, add(vec));
		else
			return node;
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

void gen(Node *node) {
	if (node->ty == ND_NUM) {
		printf("  push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->ty) {
	case '+':
		printf("  add rax, rdi\n");
		break;
	case '-':
		printf("  sub rax, rdi\n");
		break;
	case '*':
		printf("  mul rdi\n");
		break;
	case '/':
		printf("  mov rdx, 0\n");
		printf("  div rdi\n");
		break;
	case '%':
		printf("  mov rdx, 0\n");
		printf("  div rdi\n");
		printf("  mov rax, rdx\n");
		break;
	default:
		printf("  cmp rax, rdi\n");
		char *set;
		switch (node->ty) {
			case TK_EQ:
				set = "sete";
				break;
			case TK_NE:
				set = "setne";
				break;
			case TK_LE:
				set = "setle";
				break;
			case '<':
				set = "setl";
				break;
		}
		printf("  %s al\n", set);
		printf("  movzb rax, al\n");
		break;

	}

printf("  push rax\n");
}



int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}
	
	Vector *vec = new_vector();

	//トークナイズしてパースする
	tokenize(argv[1], vec);

	//Test function
	if (strcmp(argv[1], "-test") == 0) {
			printer(vec);
	}
			
		
	Node *node = equality(vec);

	//アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	//コード生成
	gen(node);

	printf("  pop rax\n");

	printf("  ret\n");
	return 0;
}
