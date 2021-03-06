#include "header.h"

void initialiseModelParameters(EMBEDDING* model, int C, int N, float alpha, int epochs)
{
    if (C > 0)
        model->context = C;

    if (N > 0)
        model->dimension = N;

    if (alpha > 0)
        model->alpha = alpha;

    if (epochs > 0)
        model->epochs = epochs;
}

void initialiseModelCorpus(EMBEDDING* model, char* corpus)
{
    int len = strlen(corpus);
    model->corpus_length = len;
    model->corpus = (char*)malloc(sizeof(char)*len);
    model->clean_corpus = (char*)malloc(sizeof(char)*len);
    
    char* no_punkt = remove_punctuations(corpus);
    char* cleaned_corpus = trim(no_punkt);
    strcpy(model->corpus, corpus);
    strcpy(model->clean_corpus, cleaned_corpus);
    
    free(no_punkt);
    free(cleaned_corpus);
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
    printf("Obtaining X matrix...\n");
    model->X = getX(model, m, X_words);
    printf("Obtaining y matrix...\n\n");
    model->Y = getY(model, m, y_words);

    free(temp1);
    free(temp2);
    free(X_words);
    free(y_words);
}

EMBEDDING* createModel()
{
    EMBEDDING* model = (EMBEDDING*)malloc(sizeof(EMBEDDING));

    model->context = 2;
    model->dimension = 100;
    model->alpha = 0.01;
    model->epochs = 100;
    model->batch_size = 0;
    model->corpus_length = 0;
    model->vocab_size = 0;
    model->A1 = NULL;
    model->b1 = NULL;
    model->b2 = NULL;
    model->clean_corpus = NULL;
    model->corpus = NULL;
    model->hashtable = NULL;
    model->vocab = NULL;
    model->W1 = NULL;
    model->W2 = NULL;
    model->X = NULL;
    model->Y = NULL;
    model->A2 = NULL;

    return model;
}

void extractEmbeddings(EMBEDDING* model)
{
    double** W2T = transpose(model->W2, model->vocab_size, model->dimension);
    double** W1_add_W2T = add(model->W1, W2T, model->dimension, model->vocab_size);
    double** W = multiply_scalar(W1_add_W2T, 0.5, model->dimension, model->vocab_size);
    for(int i=0; i< model-> vocab_size;i++)
    {
        if (model->hashtable[i]!=NULL)
        {
            if (model->hashtable[i]->wordvector == NULL)
                model->hashtable[i]->wordvector = createZerosArray(1, model->dimension);
            for(int j=0; j<model->dimension; j++)
                model->hashtable[i]->wordvector[0][j] = W[j][i];
        }
    }
    free2D(W2T, model->dimension, model->vocab_size);
    free2D(W1_add_W2T, model->dimension, model->vocab_size);
    free2D(W, model->dimension, model->vocab_size);
}

void train(EMBEDDING* model, char* corpus, int C, int N, float alpha, int epochs, int random_state, bool save)
{
    printf("Initialising hyperparameters...\n");
    initialiseModelParameters(model, C, N, alpha, epochs);
    
    if(corpus != NULL)
    {
        printf("Creating Vocabulary...\n");
        initialiseModelCorpus(model, corpus);
        createHashtable(model, corpus);
        
        model->W1 = createArray(model->dimension, model->vocab_size, random_state);
        model->W2 = createArray(model->vocab_size, model->dimension, random_state);
        model->b1 = createArray(model->dimension, 1, random_state);
        model->b2 = createArray(model->vocab_size, 1, random_state);
        
        printf("Calculating X and y...\n");
        createXandY(model, random_state);
    }
    
    printf("Initiating Training...\n");
    gradientDescent(model, save);
    extractEmbeddings(model);
}
