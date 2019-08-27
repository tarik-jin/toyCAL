extern int testGlb;

int funTest()
{
	int a = 1, b = a + 2, c = a * b, d = b * c, e = 10;
	int i = testGlb;

	if(a > b) {
		c = a * b;
		d = b / c;
		e = c % d;
	}

	do {
		c = a * b;
		d = b * c;
		e = c * d;
	} while(i--);

	return a + e;
}
