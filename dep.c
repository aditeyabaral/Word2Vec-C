#include "header.h"

EMBEDDING* initialiseModelParameters(char* corpus, int C, int N, float alpha)
{
    EMBEDDING* model = (EMBEDDING*)malloc(sizeof(EMBEDDING));
    int len = strlen(corpus);
    model->corpus_length = len;
    model->corpus = (char*)malloc(sizeof(char)*len);
    model->clean_corpus = (char*)malloc(sizeof(char)*len);
    char* cleaned_corpus = trim(remove_punctuations(corpus));
    strcpy(model->clean_corpus, cleaned_corpus);
    strcpy(model->corpus, corpus);
    model->hashtable = NULL;

    if (C > 0)
        model->context = C;
    else
        model->context = 2;

    if (N > 0)
        model->dimension = N;
    else
        model->dimension = 100;

    if (alpha > 0)
        model->alpha = alpha;
    else
        model->alpha = 0.01;
    return model;
}

void initialiseModelHashtable(EMBEDDING* model)
{
    model->hashtable = (NODE**)malloc(sizeof(NODE*)*model->vocab_size);
    for(int i = 0; i<model->vocab_size; i++)
        model->hashtable[i] = NULL;
}

void createXandY(EMBEDDING* model, int random_state)
{
    int num_words = 0;
    char* token1 , *save1;
    char* temp1 = (char*)malloc(sizeof(char)*model->corpus_length);
    strcpy(temp1, model->clean_corpus);
    token1 = strtok_r(temp1, " ", &save1);
    while(token1 != NULL)
    {
        num_words++;
        token1 = strtok_r(NULL, " ", &save1);
    }
    if ((2*model->context+1) >= num_words)
    {
        printf("Not enough words available for context. Window >= number of words.\n");
        return;
    }
    int ctr1 = 1, ctr2 = 1;
    char* temp2 = (char*)malloc(sizeof(char)*model->corpus_length);
    strcpy(temp1, model->clean_corpus);
    token1 = strtok_r(temp1, " ", &save1);
    char* token2, *save2;

    char* X_words = (char*)malloc(sizeof(char)*INT_MAX);
    char* y_words = (char*)malloc(sizeof(char)*INT_MAX);

    while(token1 != NULL && ctr1 <= (num_words-2*model->context))
    {
        ctr2 = 1;
        strcpy(temp2, model->clean_corpus);
        token2 = strtok_r(temp2, " ", &save2);
        while(token2 != NULL && ctr2!=ctr1)
        {
            token2 = strtok_r(NULL, " ", &save2);
            ctr2++;
        }
        for(int i=1;i<=model->context;i++)
        {
            strcat(X_words, token2);
            strcat(X_words, " ");
            token2 = strtok_r(NULL, " ", &save2);
        }
        strcat(y_words, token2);
        strcat(y_words, "\n");
        token2 = strtok_r(NULL, " ", &save2);
        for(int i=1;i<=model->context;i++)
        {
            strcat(X_words, token2);
            strcat(X_words, " ");
            token2 = strtok_r(NULL, " ", &save2);
        }
        X_words[strlen(X_words)-1] = '\n';
        ctr1++;
        token1 = strtok_r(NULL, " ", &save1);
    }

    int m = ctr1-1;
    model->batch_size = m;
    //printf("%s\n\n", X_words);
    //printf("%s\n\n", y_words);
    model->X = getX(model, m, X_words);;
    model->Y = getY(model, m, y_words);
}

void train(char* corpus, int C, int N, float alpha, int random_state)
{
    EMBEDDING* model = initialiseModelParameters(corpus, C, N, alpha);
    createHashtable(model, corpus);
    model->W1 = createArray(model->dimension, model->vocab_size, random_state);
    model->W2 = createArray(model->vocab_size, model->dimension, random_state);
    model->b1 = createArray(model->dimension, 1, random_state);
    model->b2 = createArray(model->vocab_size, 1, random_state);
    createXandY(model, random_state);
    displayModel(model);
}
