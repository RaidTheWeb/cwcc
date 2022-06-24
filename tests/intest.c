
void printsub(int a, int b) {
    printint(a - b);

    printint(a - b);

    printint(a - b);

    printint(a - b);
}

int subab(int a, int b) {
    return a - b;
}

int addab(int a, int b) {
    return a + b;
}

int main() {
    int x;
    x = addab(1, 2);
	printint(x);
    int y;
    y = subab(4, 2);
    printint(y);

    printsub(10, 8);
	return 0;
}
