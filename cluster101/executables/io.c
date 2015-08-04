#include <stdio.h>
int main()
{
	FILE *fp;
	char buf[1024]="fhhkh kjjjjjjjjjjjjjjjjjjj hkdddddddddddddddddh kjkdhk            hfkkfhkhjdfh h4k4jkhk4k4k4khjkhkhfkhfkjhkfhfkjllll"
					"hfkjhkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"
					"jhfljlllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll";
	
	int i=0;
	while(1)
	{
	fp=fopen("io.txt","w");
	fwrite(buf,1,1024,fp);
	fclose(fp);
	i++;
	}
	return 0;
}
