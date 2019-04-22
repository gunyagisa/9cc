#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//トークンの型を表す値
enum {
	TK_NUM = 256, //整数トークン
	TK_EOF,       //入力の終わりを表すトークン
};

//トークンの型
typedef struct{
	int ty;
	int val;
	char *input;
} Token;

//トークナイズした結果のトークン列はこの配列に保存する
//１００個以上のトークンは来ないものとする
Token tokens[100];

void tokenize(char *p){
	int i = 0;
	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p == '+' || *p == '-'){
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if(isdigit(*p)){
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}

		fprintf(stderr, "トークナイズできません: %s\n", p);
		exit(1);
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}

void error(int i){
	fprintf(stderr, "予期しないトークンです: %s\n", tokens[i].input);
	exit(1);
}
//構文解析
enum {
	ND_NUM = 256;
};

typedef struct Node {
	int ty;
	struct Node *lhs; //左辺
	struct Node *rhs; //右辺
	int val;
}	Node;

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

int consume(int ty) {
	if (tokens[pos].ty != ty)
		return 0;
	pos++;
	return 1;

}

Node *add() {
	Node *node = mul();

	for (;;) {
		if (consume('+'))
			node = new_node('+', node, mul());
		else if (consume('-'))
			node = new_node('-', node, mul());
		else
			return node;
	}
}

Node *mul() {
	Node *node =
	for (;;) {
		if (consume('*'))
			node = new_node('*', node, term());
			else if (consume('/'))
				node = new_node('/', node, term());
			else
				return node;
	}
}

Node *term() {
	if (consume('(')) {
		Node *node = add();
		if (!consume(')'))
			error("開きカッコの対応する閉じカッコがありません: %s",
						tokens[pos].input);
			return node;
	}

	if (tokens[pos].ty == TK_NUM)
		return new_node_num(tokens[pos++].val);
	error("数値でも開きカッコでもないトークンです: %s",
				tokens[pos].input);
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
	printf(" mov rdx, 0\n");
	printf("  div rdi\n");
}

printf("  push rax\n");
}



int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	//トークナイズしてパースする
	tokenize(argv[1]);
	Node *node = add();

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
