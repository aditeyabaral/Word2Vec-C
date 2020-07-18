#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
struct node
{
    char* word;
    float* wordvector;
    float** onehotvector;
};
typedef struct node NODE;

struct embedding
{
    int context;
    float alpha;
    int dimension;
    int vocab_size;
    int corpus_length;
    int batch_size;
    float **W1;
    float **W2;
    float **b1;
    float **b2;
    float** X;
    float** Y;
    char* vocab;
    char* corpus;
    char* clean_corpus;
    NODE **hashtable;
};
typedef struct embedding EMBEDDING;

float relu(float);
float** softmax(float** M, int m, int n, int axis);
EMBEDDING* initialiseModelParameters(char* corpus, int C, int N, float alpha);
void initialiseModelHashtable(EMBEDDING*);
float** createArray(int, int, int);
float** createZerosArray(int m, int n);
float** createOnesArray(int m, int n);
float** transpose(float**A, int m, int n);
float** multiply(float **M1, float **M2, int m1, int n1, int m2, int n2);
void displayArray(float**, int, int);
void displayHashtable(EMBEDDING*);
void displayModel(EMBEDDING* model);
char* remove_punctuations(char*);
int getVocabularySize(EMBEDDING*);
int getHashvalue(char*, int);
char* trim(char*);
float** createOneHot(NODE* node, EMBEDDING* model);
void createHashtable(EMBEDDING*, char*);
void createXandY(EMBEDDING* model, int random_state);
float** getX(EMBEDDING* model, int m, char* s);
float** getY(EMBEDDING* model, int m, char* s);
void train(char* corpus, int C, int N, float alpha, int random_state);
