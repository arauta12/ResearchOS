const int roVar = 10;
int gblVar = 1;
int bssVar;

int func(int x) {
	return x * x / 2;
}

int main() {
	gblVar = func(3);
	return 0;
}
