/*****************************************************************/
/*          Biblioteca vetor | PROG2 | MIEEC | 2017/18           */
/*****************************************************************/

#include "vetor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

vetor *vetor_novo()
{
	vetor *vec;

	/* aloca memoria para a estrutura vetor */
	vec = (vetor *)malloc(sizeof(vetor));
	if (vec == NULL)
		return NULL;

	vec->tamanho = 0;
	vec->capacidade = 0;
	vec->elementos = NULL;

	return vec;
}

void vetor_apaga(vetor *vec)
{
	int i;

	if (vec == NULL)
		return;

	/* liberta memoria de cada string */
	for (i = 0; i < vec->tamanho; i++)
	{
		free(vec->elementos[i].account);
	}

	/* liberta memoria dos apontares para as strings */
	free(vec->elementos);

	/* liberta memoria da estrutura vetor */
	free(vec);
}

int vetor_atribui(vetor *vec, int pos, const bank_account_t *valor)
{
	unsigned int len_valor;

	if (vec == NULL || pos < 0 || pos >= vec->tamanho)
		return -1;

	vec->elementos[pos].account->account_id = valor->account_id;
	strcpy(vec->elementos[pos].account->hash, valor->hash);
	strcpy(vec->elementos[pos].account->salt, valor->salt);
	vec->elementos[pos].account->balance = valor->balance;

	return pos;
}

int vetor_tamanho(vetor *vec)
{
	if (vec == NULL)
		return -1;

	return vec->tamanho;
}

const bank_account_t *vetor_elemento(vetor *vec, int indice)
{
	if (vec == NULL || indice < 0 || indice >= vec->tamanho)
		return NULL;

	return vec->elementos[indice].account;
}

int vetor_insere(vetor *vec, const bank_account_t *valor, int pos)
{
	int i;

	if (vec == NULL || pos < -1 || pos > vec->tamanho)
		return -1;

	/* aumenta elementos do vetor se capacidade nao for suficiente */
	if (vec->tamanho == vec->capacidade)
	{
		if (vec->capacidade == 0)
			vec->capacidade = 1;
		else
			vec->capacidade *= 2;

		vec->elementos = (v_elemento *)realloc(vec->elementos, vec->capacidade * sizeof(v_elemento));
		if (vec->elementos == NULL)
			return -1;
	}

	/* se pos=-1 insere no fim do vetor */
	if (pos == -1)
		pos = vec->tamanho;

	/* copia todos os elementos a partir da posicao pos ate' ao fim do vetor para pos+1 */
	for (i = vec->tamanho - 1; i >= pos; i--)
	{
		vec->elementos[i + 1] = vec->elementos[i];
	}

	/* aloca espaco para a nova string na posicao pos */
	vec->elementos[pos].account = (bank_account_t *)malloc(sizeof(bank_account_t));
	if (vec->elementos[pos].account == NULL)
		return -1;

	/* copia valor */
	vec->elementos[pos].account->account_id = valor->account_id;
	strcpy(vec->elementos[pos].account->hash, valor->hash);
	strcpy(vec->elementos[pos].account->salt, valor->salt);
	vec->elementos[pos].account->balance = valor->balance;

	vec->tamanho++;

	return pos;
}

int vetor_remove(vetor *vec, int pos)
{
	int i;

	if (vec == NULL || pos < 0 || pos >= vec->tamanho)
		return -1;

	/* liberta string na posicao a remover */
	free(vec->elementos[pos].account);

	/* copia todos os elementos a partir da posicao pos+1 ate' ao fim do vetor para pos */
	for (i = pos + 1; i < vec->tamanho; i++)
	{
		vec->elementos[i - 1] = vec->elementos[i];
	}

	vec->tamanho--;

	return 0;
}

int vetor_pesquisa(vetor *vec, const bank_account_t *valor)
{
	int i;

	if (vec == NULL)
		return -1;

	/* pesquisa sequencial */
	for (i = 0; i < vec->tamanho; i++)
	{
		if (valor->account_id == vec->elementos[i].account->account_id)
		{
			if (strcmp(valor->hash, vec->elementos[i].account->hash) == 0)
			{
				if (strcmp(valor->salt, vec->elementos[i].account->salt) == 0)
				{
					return 0;
				}
			}
		}
	}

	return -1;
}

int vetor_free(vetor *vec)
{

	for (int i = 0; i < vec->tamanho; i++)
	{
		if (vetor_remove(vec, i) != 0)
		{
			return -1;
		}
	}
	return 0;
}
