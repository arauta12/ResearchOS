int gblVar = 1;
const volatile int roVar = 10;

void funct() {
	int x = roVar;
	gblVar = 10;
}

int main() {
	funct();
	return 0;
}
