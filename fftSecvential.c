#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <pthread.h>

FILE *inputFile, *outputFile;
char *inputFileName, *outputFileName;
int N, P;
complex *out;
complex *buf;
typedef double complex cplx;
// double PI = atan2(1, 1) * 4;
double PI =M_PI;

void getArgs(int argc, char **argv)
{
	if (argc < 4)
	{
		printf("Not enough paramters: ./program inputValues.txt outputValues.txt numThreads \n");
		exit(1);
	}

	inputFileName = argv[1];
	outputFileName = argv[2];
	P = atoi(argv[3]);
}
void readFile()
{
	inputFile = fopen(inputFileName, "r");
	if (inputFile == NULL)
	{
		fprintf(stdout, "Failed to open the input file.\n");
		exit(1);
	}

	int returnN;
	returnN = fscanf(inputFile, "%d", &N);
	if (returnN != 1)
	{
		fprintf(stdout, "Failed to read N from the input file.\n");
	}

	out = calloc(N,sizeof(complex)) ;
	buf = calloc(N,sizeof(complex));

	if ( out==NULL || buf==NULL)
	{
		printf("malloc failed!");
		exit(1);
	}

	int returnInputs;
	double what_number;
	for (int i = 0; i < N; i++)
	{
		returnInputs = fscanf(inputFile, "%lf", &(what_number));
		if (returnInputs != 1)
		{
			fprintf(stdout, "Failed to read the %dth pair from the input file.\n", i);
			exit(1);
		}
		buf[i]=what_number + 0 *I ;
		out[i]= what_number + 0 *I;
	}

	fclose(inputFile);
}

void writeFile()
{
	outputFile = fopen(outputFileName, "wt");
	if (outputFile == NULL)
	{
		fprintf(stdout, "Failed to open the output file.\n");
		exit(1);
	}
	fprintf(outputFile, "%d\n", N);
	for (int i = 0; i < N; i++)
	{
		fprintf(outputFile, "%lf	%lf\n", creal(buf[i]), cimag(buf[i]));
	}
	fclose(outputFile);
}

void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n)
	{
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);

		for (int i = 0; i < n; i += 2 * step)
		{
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2] = out[i] + t;
			buf[(i + n) / 2] = out[i] - t;
		}
	}
}

void *threadFunction(void *var)
{
	int k, t;
	int thread_id = *(int *)var;

	int start = ceil((double)N / (double)P) * thread_id;
	int end = fmin(N, (thread_id + 1) * ceil((double)N / (double)P));
	
	_fft(buf, out, N, 1);
}

void show(const char *s, complex *buf)
{
	printf("%s", s);
	for (int i = 0; i < N; i++)
		if (cimag(buf[i])==0)
			printf("%g ", creal(buf[i]));
		else
			printf("(%g, %g) ", creal(buf[i]), cimag(buf[i]));
	printf("\n");
}

int main(int argc, char *argv[])
{
	int i;
	getArgs(argc, argv);
	readFile();
	// show("INPUTS", inputs);

	// _fft(buf, out, N, 1);
	// show("Output  ", buf);

	pthread_t tid[P];
	int thread_id[P];
	for (i = 0; i < P; i++)
		thread_id[i] = i;
	for (i = 0; i < P; i++)
	{
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}
	for (i = 0; i < P; i++)
	{
		pthread_join(tid[i], NULL);
	}
	writeFile();
	free(out);
	free(buf);
	return 0;
}
