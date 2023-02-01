#include "common.h"
#include "file.h"

extern void print_vector (FLOAT *output, int cols)
{
	FILE *fp;
	fp = fopen("vector.txt", "w" );
	for (int i=0;i<cols;i++)
		fprintf(fp,"%g ",output[i]);
	fprintf(fp,"\n");
	fclose(fp);
		
}
extern void print_determinant (FLOAT *output, int cols)
{
	FILE *fp;
	fp = fopen("determinante.txt", "w" );
	for (int i=0;i<cols;i++)
	{
		for (int j=0;j<cols;j++)
			fprintf(fp,"%f ",output[i*cols+j]);
		fprintf(fp,"\n");
	}
	fclose(fp);

}

/**
 * extract number of columns
 * @param char - column separator
 */
int extract_columns (char *buffer, char separator)
{
        int c_t = 1, i = 0;
        int len = strlen(buffer);
	while (buffer[i] == separator) i++;
        char c = buffer[i];
        while ((c != '\0') && (i < len))
        {
                if (c == separator) 
		{
			while (c == separator) 
			{  
				i++; 
				c = buffer[i]; 
			}
			c_t++;
		}
		else
		{
                	i++;
                	c = buffer[i];
		}
        }
        return c_t;
}

extern void exam_data (char *input, int *rows, int *columns, char separator) {

        int r_t = 1;
        char buffer[MAX_LINE+1];// = (char *)malloc(sizeof(char)*MAX_LINE);
        char *ignored = (char *)malloc(sizeof(char)*MAX_LINE);

        FILE *fp;
        if ((fp =  fopen(input,"r")) == NULL) {
                printf("ReadInput(): Can't open file \"%s\"\n", input);
                exit(1);
        }
        ignored = fgets(buffer,MAX_LINE,fp);
	//printf("LINE:%s\n",buffer);
        *columns = extract_columns(buffer,separator);

        while(fgets(buffer,MAX_LINE,fp) != NULL ) r_t++;
        *rows = r_t;
        printf("%u %u %u\n",*rows,*columns,*rows * *columns);
        //exit(0);
        fclose(fp);
}
