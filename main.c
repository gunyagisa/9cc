#include "9cc.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	Vector *vec = new_vector();
	
	//tokenize
	tokenize(argv[1], vec);

	Node *node = equality(vec);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	//generate code
	gen(node);

	ptintf("	pop rax\n");
	printf("	ret\n");

	return 0;
}
