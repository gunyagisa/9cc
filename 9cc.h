#include <ctupe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


extern int pos;

enum {
		TK_NUM = 256,	//
		TK_EOF,			//
		TK_EQ,			//==
		TK_NE,			//!=
		TK_LE,			//< <=
		TK_GE,			//> >=
};

enum {
		ND_NUM = 256,
};

//Token struct
typedef struct {
	int ty;		//token type
	int val;	//value (if type == TK_NUM)
	char *input;
} Token;

//Vector struct
typedef struct {
	void **data;
	int capacity;
	int len;
} Vector;

//Node struct
typedef struct Node {
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
} Node;

Vector *new_vector();
void push_vec(Vector *vec, void *elem);

void error(char *fmt, ...);

void tokenize(char *p, Vector *vec);

Node *equality(Vector *vec);
Node *relational(Vector *vec);
Node *add(Vector *vec);
Node *term(Vector *vec);
Node *mul(Vector *vec);
Node *unary(Vector *vec);

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);

int consume(int ty, Vector *vec);

void gen(Node *node);
