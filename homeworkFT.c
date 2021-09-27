#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <pthread.h>

FILE *inputFile, *outputFile;
char *inputFileName, *outputFileName;
int N, P;
double *inputs, *outputsRe, *outputsIm;

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

	inputs = malloc(sizeof(double) * N);
	outputsRe = malloc(sizeof(double) * N);
	outputsIm = malloc(sizeof(double) * N);

	if (inputs == NULL)
	{
		printf("malloc failed!");
		exit(1);
	}

	int returnInputs;
	for (int i = 0; i < N; i++)
	{
		returnInputs = fscanf(inputFile, "%lf", &inputs[i]);
		if (returnInputs != 1)
		{
			fprintf(stdout, "Failed to read the %dth pair from the input file.\n", i);
			exit(1);
		}
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
		fprintf(outputFile, "%lf	%lf\n", outputsRe[i], outputsIm[i]);
	}
	fclose(outputFile);
}

void *threadFunction(void *var)
{
	int k, t;
	int thread_id = *(int *)var;

	int start = ceil((double)N / (double)P) * thread_id;
	int end = fmin(N, (thread_id + 1) * ceil((double)N / (double)P));

	for (k = start; k < end; k++)
	{
		complex double sum = 0.0;
		for (t = 0; t < N; t++)
		{
			double angle = 2 * M_PI * t * k / N;
			sum += inputs[t] * cexp(-angle * I);
		}
		outputsRe[k] = creall(sum);
		outputsIm[k] = cimagl(sum);
	}
}

int main(int argc, char *argv[])
{
	int i;
	getArgs(argc, argv);
	readFile();

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
	return 0;
}
