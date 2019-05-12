#include "9cc.h"

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

//error function
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//test function
int expect(int line, int expected, int actual) {
  if (expected == actual)
    return 0;
  fprintf(stderr, "%d: %d expected, but got %d\n",
          line, expected, actual);
  exit(1);
}

void runtest() {
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++)
    push_vec(vec, (void *)i);

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (long)vec->data[0]);
  expect(__LINE__, 50, (long)vec->data[50]);
  expect(__LINE__, 99, (long)vec->data[99]);

  printf("OK\n");
}
