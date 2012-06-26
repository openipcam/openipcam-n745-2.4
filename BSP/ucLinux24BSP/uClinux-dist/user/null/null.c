
main()
{
	volatile unsigned char *sp;

	sp = (volatile unsigned char *) 0xa0000000;
	*sp = 0;
}
