int
divide(int dividend, int divisor)
{
	int result = 0;
	int remainder = dividend;
	while (remainder >= divisor) {
		result++;
		remainder -= divisor;
	}
	return result;
}

int
compute_volume(int rad)
{
	int rad3 = rad * rad * rad;
	return divide(4*355*rad3, 3*113);
}

int
kmain( void )
{
	int radius = 5;
	int volume;
	volume = compute_volume(radius);
	return volume;
}
