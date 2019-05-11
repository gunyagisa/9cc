#include "9cc.h"

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
