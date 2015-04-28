/*#define TAB_X 7*/
/*#define TAB_Y 4*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"

typedef enum {
	PROB_MIN = 0,
	PROB_MAX
} problem_t;

typedef struct {
	problem_t problem;
	uint8_t nvars; // number of variables
	uint8_t nrest; // number of restrictions

	// matrix
	uint8_t mtx_x_sz, mtx_y_sz;
	float **mtx;
} rcv_args_t;

typedef struct {
	float menor_igual, maior_igual;
} parametros_t;

typedef struct {
	/*
	 * parm1 e 2 são referentes aos valores originais de S
	 * enquanto parm3 e 4 se referem aos valores originais da função L
	 */
	parametros_t *parm1, *parm2, *parm3, *parm4;
} otimizacao_t;


// ==========
// prototypes
// ==========

// verify all the variables in all rows
// for MAX all variables should be >= 0 to pass
// for MIN all variables should be <= 0 to pass
// otherwise another iteration shall be needed
static bool matrix_verify (float **base_matrix, const uint8_t nvars, const uint8_t sz_y);

static void iteration (	float **base_matrix,
						const uint8_t nvars,
						const uint8_t nrest,
						const uint8_t sz_x,
						const uint8_t sz_y);

static void sensibilidade (	float **base_matrix,
							float **copy_matrix,
							const uint8_t nvars,
							const uint8_t sz_x,
							const uint8_t sz_y);

static float **dualidade (	float **copy_matrix,
							const uint8_t nvars,
							const uint8_t nrest,
							const uint8_t sz_x,
							const uint8_t sz_y);

// dynamic matrix management
static float **fmatrix_calloc (const uint32_t sz_x, const uint32_t sz_y);
static void fmatrix_free (float **mtx, const size_t sz_x);

static bool get_parameters (rcv_args_t *rcv_args, int argc, char **argv);

static void print_matrix (float **mtx, const size_t xlen, const size_t ylen);

static void fmtx_cpy (float **dst, float **src, const size_t xlen, const size_t ylen)
{
	size_t x, y;

	for (x = 0; x < xlen; x++) {
		for (y = 0; y < ylen; y++) {
			dst[x][y] = src[x][y];
		}
	}
}

int main (int argc, char **argv)
{
    float **base_matrix, **copy_matrix, **dual;
	rcv_args_t rcv_args;

	PF_DBG("ENTER");
	get_parameters(&rcv_args, argc, argv);

	base_matrix = rcv_args.mtx;
	copy_matrix = fmatrix_calloc(rcv_args.mtx_x_sz, rcv_args.mtx_y_sz);

	// copy matrix
	fmtx_cpy(copy_matrix, base_matrix, rcv_args.mtx_x_sz, rcv_args.mtx_y_sz);

	PF("Initial table:\n");
	print_matrix(base_matrix, rcv_args.mtx_x_sz, rcv_args.mtx_y_sz);

	// do simplex!
	while (!matrix_verify(base_matrix, rcv_args.nvars, rcv_args.mtx_y_sz)) {
		static uint32_t iter_cnt = 1;

		PF("Iteration %u\n", iter_cnt);
		iteration(	base_matrix,
					rcv_args.nvars,
					rcv_args.nrest,
					rcv_args.mtx_x_sz,
					rcv_args.mtx_y_sz);
		iter_cnt++;

		// print new table
		print_matrix(base_matrix, rcv_args.mtx_x_sz, rcv_args.mtx_y_sz);
	}

	sensibilidade(base_matrix, copy_matrix, rcv_args.nvars, rcv_args.mtx_x_sz, rcv_args.mtx_y_sz);

	dual = dualidade(	copy_matrix,
						rcv_args.nvars,
						rcv_args.nrest,
						rcv_args.mtx_x_sz,
						rcv_args.mtx_y_sz);
	PF("\nDual:\n");
	print_matrix(dual, rcv_args.nrest + 1, rcv_args.nvars + 1);

	fmatrix_free(rcv_args.mtx, rcv_args.mtx_x_sz);
	fmatrix_free(copy_matrix, rcv_args.mtx_x_sz);
	fmatrix_free(dual, rcv_args.nrest + 1);
	PF_DBG("EXIT");
	return 0;
}

static void iteration (	float **base_matrix,
						const uint8_t nvars,
						const uint8_t nrest,
						const uint8_t sz_x,
						const uint8_t sz_y)
{
	int x, y, cont = 0;
	int pivot_col = 0;
	float **vet_men = fmatrix_calloc(nrest, 2);
	float abs, abs_cmp;

	PF_DBG("ENTER");

	// passo 1
	abs = base_matrix[1][0] >= 0 ? base_matrix[1][0] : (base_matrix[1][0] * -1);
	pivot_col = 1;
	for (x = 2; x < nvars + 1; x++) {
		abs_cmp = base_matrix[x][0] >= 0 ? base_matrix[x][0] : (base_matrix[x][0] * -1);
		if (abs_cmp > abs) {
			abs = abs_cmp;
			pivot_col = x;
		}
	}

	// passo 2
	for (y = 1; y < nrest + 1; y++) {
		vet_men[cont][0] = (float) base_matrix[sz_x - 1][y] / base_matrix[pivot_col][y]; // divisao do valor para avaliacao
		vet_men[cont][1] = y; // aloca a posicao do valor na matrix
		cont++;
	}

	float menor_pivo = vet_men[0][0];			//seta o primeiro valor do vetor para iniciar os testes
	int y_pivo = vet_men[0][1];					// e a posicao do valor tambem

	for (x = 1; x < nrest - 1; x++) {
		if (vet_men[x][0] >= 0 && menor_pivo > vet_men[x][0]) {		//testa se e maior que 0 e se e se e o menor
			menor_pivo = vet_men[x][0];
			y_pivo = vet_men[x][1];
		}
	}

	//linha pivo ja foi encontrada

	float element_pivo = base_matrix[pivot_col][y_pivo];
	float linha_pivo[sz_x] ;

	for (x = 0; x < sz_x; x++) { //XGH copia o vetor
		linha_pivo[x] = base_matrix[x][y_pivo];
	}

	for (x = 0; x < sz_x; x++) { //realiza a divizao da linha pivo pelo elemento pivo e guarda o resultado na linha da matrix
		linha_pivo[x] = (float) linha_pivo[x] / element_pivo;
		base_matrix[x][y_pivo] = (float) linha_pivo[x];
	}

	//linha pivo ja encontrada

	for (y = 0; y < sz_y; y++) {
		if (y != y_pivo) {						//aplica o pivo nas demais linhas
			float mid_pivo = base_matrix[pivot_col][y]*(-1);		//inverso do valor que e o pivo da multiplicacao

			float cop_linha_pivo[sz_x];
			for (x = 0; x < sz_x; x++) { //XGH  copia a linha pivo original
				cop_linha_pivo[x]=linha_pivo[x];
			}

			for (x = 0; x < sz_x; x++) {				//o multiplicador e aplicado na linha e somado a linha da matrix
				cop_linha_pivo[x] = (float) cop_linha_pivo[x] * mid_pivo;
				base_matrix[x][y] = (float) cop_linha_pivo[x] + base_matrix[x][y];
			}
		}
	}

	fmatrix_free(vet_men, nrest - 1);
	PF_DBG("EXIT");
};

static bool matrix_verify (float **base_matrix, const uint8_t nvars, const uint8_t sz_y)
{
	int x, y, x_start, x_end;

	PF_DBG("ENTER");
	x_start = 1;
	x_end = nvars + x_start;
	// verify matrix
	for (x = x_start; x < x_end; x++) {
		for (y = 1; y < sz_y + 1; y++) {
			if (!(base_matrix[x][y] == 1 || base_matrix[x][y] == 0)) {
				return false;
			}
		}
	}
	PF_DBG("EXIT");
	return true;
};



static void sensibilidade (	float **base_matrix,
							float **copy_matrix,
							const uint8_t nvars,
							const uint8_t sz_x,
							const uint8_t sz_y)
{
	int pivoy_x1 = 0, pivoy_x2 = 0, y;
	otimizacao_t ot;

	PF_DBG("ENTER");
	if (nvars == 2) { //somente 2 variaveis
		float reference_matrix[2][2] = {{0,0},{0,0}}, b_matrix[2][2] = {{0,0},{0,0}}, original_b_matrix[2][2] = {{0,0},{0,0}};
		int marcador_b[2];

		//procura o pivo para as duas variaveis, ou seja, onde a variavel se mostra 1 e por consequencia o S será o valor correto para a solução ideal
		for (y = 0; y < sz_y; y++) {
			if(base_matrix[1][y]==1){
				pivoy_x1=y;
				break;
			}
		}
		for (y = 0; y < sz_y; y++) {
			if(base_matrix[2][y]==1){
				pivoy_x2=y;
				break;
			}
		}

		/*
		reference_matrix = recebe os campos F1 e F2 calculados anteriormente
		b_matrix = recebe os resultados de cada variavel, caso não seja o resultado para a determinada coluna recebe 0
		marcador_b = vetor que marca quais posições da b_matrix são 0 para facilitar a busca subsequente, também ajuda caso algum resultado de 0 por coincidencia
		*/

		if (pivoy_x1 < pivoy_x2) {
			for( y=0;y<2;y++){
				reference_matrix[y][0]=base_matrix[3+y][pivoy_x1];
				reference_matrix[y][1]=base_matrix[3+y][pivoy_x2];
			}

			b_matrix[0][0]=0;
			b_matrix[0][1]=copy_matrix[sz_x - 1][pivoy_x1];
			b_matrix[1][0]=copy_matrix[sz_x - 1][pivoy_x2];
			b_matrix[1][1]=0;

			original_b_matrix[0][0]=0;
			original_b_matrix[0][1]=copy_matrix[1][0];
			original_b_matrix[1][0]=copy_matrix[2][0];
			original_b_matrix[1][1]=0;

			marcador_b[0]=0;
			marcador_b[1]=1;

		} else if (pivoy_x1 > pivoy_x2) {
			for( y=0;y<2;y++){
					reference_matrix[y][0]=base_matrix[3+y][pivoy_x2];
					reference_matrix[y][1]=base_matrix[3+y][pivoy_x1];
				}
			b_matrix[0][0]=copy_matrix[sz_x - 1][pivoy_x2];
			b_matrix[0][1]=0;
			b_matrix[1][0]=0;
			b_matrix[1][1]=copy_matrix[sz_x - 1][pivoy_x1];

			original_b_matrix[0][0]=copy_matrix[1][0];
			original_b_matrix[0][1]=0;
			original_b_matrix[1][0]=0;
			original_b_matrix[1][1]=copy_matrix[2][0];

			marcador_b[0]=1;
			marcador_b[1]=0;
		} else {
			printf("\n ERROR");
		}
		int x;
		parametros_t par1, par2, par3, par4;
		for (x = 0; x < 2; x++) {
			/*
			 * Atribuição dos valores <= e >= a partir das marcações feitas anteriormente
			 */
			if(marcador_b[x]==0){
				if(x==0){
					par1.menor_igual=(float)(reference_matrix[1][0]*b_matrix[x][1]*-1)/(reference_matrix[0][0]);
					par1.maior_igual=(float)(reference_matrix[1][1]*b_matrix[x][1]*-1)/(reference_matrix[0][1]);

					par3.menor_igual=(float)(reference_matrix[1][0]*original_b_matrix[x][1]*-1)/(reference_matrix[0][0]);
					par3.maior_igual=(float)(reference_matrix[1][1]*original_b_matrix[x][1]*-1)/(reference_matrix[0][1]);
				}else{
					//par 2 são invertidos se comparados ao par1
					par2.maior_igual=(float)(reference_matrix[1][0]*b_matrix[x][1]*-1)/(reference_matrix[0][0]);
					par2.menor_igual=(float)(reference_matrix[1][1]*b_matrix[x][1]*-1)/(reference_matrix[0][1]);

					par4.maior_igual=(float)(reference_matrix[1][0]*original_b_matrix[x][1]*-1)/(reference_matrix[0][0]);
					par4.menor_igual=(float)(reference_matrix[1][1]*original_b_matrix[x][1]*-1)/(reference_matrix[0][1]);
				}
			}else{
				if(x==0){
					par1.menor_igual=(float)(reference_matrix[0][0]*b_matrix[x][0]*-1)/(reference_matrix[1][0]);
					par1.maior_igual=(float)(reference_matrix[0][1]*b_matrix[x][0]*-1)/(reference_matrix[1][1]);

					par3.menor_igual=(float)(reference_matrix[0][0]*original_b_matrix[x][0]*-1)/(reference_matrix[1][0]);
					par3.maior_igual=(float)(reference_matrix[0][1]*original_b_matrix[x][0]*-1)/(reference_matrix[1][1]);
				}else{
					par2.maior_igual=(float)(reference_matrix[0][0]*b_matrix[x][0]*-1)/(reference_matrix[1][0]);
					par2.menor_igual=(float)(reference_matrix[0][1]*b_matrix[x][0]*-1)/(reference_matrix[1][1]);

					par4.maior_igual=(float)(reference_matrix[0][0]*original_b_matrix[x][0]*-1)/(reference_matrix[1][0]);
					par4.menor_igual=(float)(reference_matrix[0][1]*original_b_matrix[x][0]*-1)/(reference_matrix[1][1]);
				}
			}
		}

		ot.parm1=&par1; // b1
		ot.parm2=&par2; // b2
		ot.parm3=&par3; // C1
		ot.parm4=&par4; // C2

		PF("\nSensitivity analysis:\n");
		PF("\tb1\n");
		PF("\t\tmajor: %f\n", ot.parm1->maior_igual);
		PF("\t\tminor: %f\n", ot.parm1->menor_igual);
		PF("\tb2\n");
		PF("\t\tmajor: %f\n", ot.parm2->maior_igual);
		PF("\t\tminor: %f\n", ot.parm2->menor_igual);
		PF("\tC1\n");
		PF("\t\tmajor: %f\n", ot.parm3->maior_igual);
		PF("\t\tminor: %f\n", ot.parm3->menor_igual);
		PF("\tC2\n");
		PF("\t\tmajor: %f\n", ot.parm4->maior_igual);
		PF("\t\tminor: %f\n", ot.parm4->menor_igual);
	}
	PF_DBG("EXIT");
}

static float **dualidade (	float **copy_matrix,
							const uint8_t nvars,
							const uint8_t nrest,
							const uint8_t sz_x,
							const uint8_t sz_y)
{
	int x, y;
	float **dual;

	PF_DBG("ENTER");
	dual = fmatrix_calloc(nrest + 1, nvars + 1);

	for (y = 1; y < sz_y; y++) {
		for (x = 1; x < 1 + nvars; x++) {
			dual[y-1][x] = copy_matrix[x][y];
		}
	}
	y = 1;
	for(x = 0; x < nrest; x++) {
		dual[x][0] = copy_matrix[sz_x - 1][y];
		y++;
	}
	dual[nrest][0] = 0;		//zera o ultimo coeficiente q fica fora do loop
	y = 1;
	for (x = 1; x < 1 + nvars; x++) {
		dual[nrest][y] = copy_matrix[y][0];
		y++;
	}

	PF_DBG("EXIT");
	return dual;
}

static float **fmatrix_calloc (const uint32_t sz_x, const uint32_t sz_y)
{
	uint32_t x;
	float **mtx;

	PF_DBG("ENTER");
	mtx = (float **) malloc(sz_x * sizeof(float *));
	if (mtx == NULL) {
		PF_ERR("mallocing matrix");
	}
	for (x = 0; x < sz_x; x++) {
		mtx[x] = (float *) calloc(1, sz_y * sizeof(float));
		if (mtx[x] == NULL) {
			PF_ERR("callocing matrix");
		}
	}
	PF_DBG("EXIT");
	return mtx;
}
static void fmatrix_free (float **mtx, const size_t sz_x)
{
	uint32_t x;

	PF_DBG("ENTER");
	for (x = 0; x < sz_x; x++) {
		free(mtx[x]);
	}
	free(mtx);
	PF_DBG("EXIT");
}

static bool get_parameters (rcv_args_t *rcv_args, int argc, char **argv)
{
	uint8_t x, y, arg_idx;

	PF_DBG("ENTER");
	rcv_args->problem = atoi(argv[1]);
	PF_DBG("rcv problem = %u", rcv_args->problem);
	rcv_args->nvars = atoi(argv[2]);
	PF_DBG("rcv nvars = %u", rcv_args->nvars);
	rcv_args->nrest = atoi(argv[3]);
	PF_DBG("rcv nrest = %u", rcv_args->nrest);
	rcv_args->mtx_x_sz =	1 +					// Z column
							rcv_args->nvars +	// x1, x2, ..., xN
							rcv_args->nrest +	// F1, F2, ..., FN
							1;					// b

	rcv_args->mtx_y_sz =	1 +					// Z row
							rcv_args->nrest;	// restriction rows

	// receive matrix
	rcv_args->mtx = fmatrix_calloc(rcv_args->mtx_x_sz, rcv_args->mtx_y_sz);
	for (y = 0, arg_idx = 4; y < rcv_args->mtx_y_sz; y++) {
		for (x = 0; x < rcv_args->mtx_x_sz; x++, arg_idx++) {
			rcv_args->mtx[x][y] = atof(argv[arg_idx]);
			PF_DBG("rcv mtx[%u][%u] = %.2f", x, y, rcv_args->mtx[x][y]);
		}
	}

	PF_DBG("EXIT");
	return true;
}

static void print_matrix (float **mtx, const size_t xlen, const size_t ylen)
{
	size_t x, y;

	PF_DBG("ENTER");
	for (y = 0; y < ylen; y++) {
		for (x = 0; x < xlen; x++) {
			printf("\t%08.2f", mtx[x][y]);
		}
		printf("\n");
	}
	PF_DBG("EXIT");
}
