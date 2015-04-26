#define TAB_X 7
#define TAB_Y 4

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"

typedef struct {
	float menor_igual, maior_igual;
} parametros_t;

typedef struct {
	/*
	 * parm1 e 2 são referentes aos valores originais de S
	 * enquanto parm3 e 4 se referem aos valores originais da função L
	 */
	parametros_t parm1, parm2, parm3, parm4;
} otimizacao_t;

// ==========
// prototypes
// ==========
static bool verificaMatriz(float base_matrix[TAB_X][TAB_Y], int numb_var);

static void iteration (	float base_matrix[TAB_X][TAB_Y],
						int numb_rest,
						int numb_var,
						int min_max);

static otimizacao_t sensibilidade(	float base_matrix[TAB_X][TAB_Y],
									float copy_matrix[TAB_X][TAB_Y],
									int numb_var);

static float **dualidade(float copy_matrix[TAB_X][TAB_Y], int numb_var, int numb_rest);

// =========================
// dynamic matrix management
// =========================
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
static void fmatrix_free (float **mtx, const uint32_t sz_x)
{
	uint32_t x;

	PF_DBG("ENTER");
	for (x = 0; x < sz_x; x++) {
		free(mtx[x]);
	}
	free(mtx);
	PF_DBG("EXIT");
}

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

rcv_args_t rcv_args;

static bool get_parameters (int argc, char **argv)
{
	uint8_t x, y, arg_idx;

	PF_DBG("ENTER");
	rcv_args.problem = atoi(argv[1]);
	rcv_args.nvars = atoi(argv[2]);
	rcv_args.nrest = atoi(argv[3]);
	rcv_args.mtx_x_sz =	1 +					// Z
						rcv_args.nvars +	// x1, x2, ..., xN
						rcv_args.nrest +	// F1, F2, ..., FN
						1;					// b

	rcv_args.mtx_y_sz =	1 +					// Z row
						rcv_args.nrest;		// restriction rows

	// receive matrix
	rcv_args.mtx = fmatrix_calloc(rcv_args.mtx_x_sz, rcv_args.mtx_y_sz);
	for (y = 0, arg_idx = 4; y < rcv_args.mtx_y_sz; y++) {
		for (x = 0; x < rcv_args.mtx_x_sz; x++, arg_idx++) {
			rcv_args.mtx[x][y] = atof(argv[arg_idx]);
			PF_DBG("rcv mtx[%u][%u] = %.2f", x, y, rcv_args.mtx[x][y]);
		}
	}

	PF_DBG("EXIT");
	return true;
}

int main (int argc, char **argv)
{
    float base_matrix[TAB_X][TAB_Y], copy_matrix[TAB_X][TAB_Y];
	float **dual;
    int numb_rest = 3;
	int numb_var = 2;
	int min_max = 1; // min=0, max=1
	int x, y;

	PF_DBG("ENTER");
	get_parameters(argc, argv);

/*#if 0*/
	base_matrix[0][0]=copy_matrix[0][0]=1;
	base_matrix[1][0]=copy_matrix[1][0]=-3;
	base_matrix[2][0]=copy_matrix[2][0]=-5;
	base_matrix[3][0]=copy_matrix[3][0]=0;
	base_matrix[4][0]=copy_matrix[4][0]=0;
	base_matrix[5][0]=copy_matrix[5][0]=0;
	base_matrix[6][0]=copy_matrix[6][0]=0;

	base_matrix[0][1]=copy_matrix[0][1]=0;
	base_matrix[1][1]=copy_matrix[1][1]=2;
	base_matrix[2][1]=copy_matrix[2][1]=4;
	base_matrix[3][1]=copy_matrix[3][1]=1;
	base_matrix[4][1]=copy_matrix[4][1]=0;
	base_matrix[5][1]=copy_matrix[5][1]=0;
	base_matrix[6][1]=copy_matrix[6][1]=10;

	base_matrix[0][2]=copy_matrix[0][2]=0;
	base_matrix[1][2]=copy_matrix[1][2]=6;
	base_matrix[2][2]=copy_matrix[2][2]=1;
	base_matrix[3][2]=copy_matrix[3][2]=0;
	base_matrix[4][2]=copy_matrix[4][2]=1;
	base_matrix[5][2]=copy_matrix[5][2]=0;
	base_matrix[6][2]=copy_matrix[6][2]=20;

	base_matrix[0][3]=copy_matrix[0][3]=0;
	base_matrix[1][3]=copy_matrix[1][3]=1;
	base_matrix[2][3]=copy_matrix[2][3]=-1;
	base_matrix[3][3]=copy_matrix[3][3]=0;
	base_matrix[4][3]=copy_matrix[4][3]=0;
	base_matrix[5][3]=copy_matrix[5][3]=1;
	base_matrix[6][3]=copy_matrix[6][3]=30;
/*#endif*/

	//strcpy(copy_matrix,base_matrix);		//copy_matrix=base_matrix;

	while(verificaMatriz(base_matrix,numb_var)!=1){
		iteration(base_matrix, numb_rest, numb_var,min_max);
	}

	sensibilidade(base_matrix,copy_matrix,numb_var);

	dual = dualidade(copy_matrix,numb_var,numb_rest);

	/*
	for(y=0;y<TAB_Y;y++){
			for(x=0;x<TAB_X;x++){
				printf("%f%s",base_matrix[x][y]," ");
			}
			printf("\n");
		}
	printf("\n\n");
	*/
	for (y = 0; y < TAB_Y; y++) {
		for (x = 0; x < TAB_X; x++) {
			printf("%02.2f\t", copy_matrix[x][y]);
		}
		printf("\n");
	}
	for (y = 0; y < TAB_Y; y++) {
		for (x = 0; x < TAB_X; x++) {
			/*PF_DBG("dump here");*/
			/*printf("%02.2f\t", dual[x][y]);*/
			printf("%02.2f\t", dual[y][x]);
		}
		printf("\n");
	}

	fmatrix_free(rcv_args.mtx, rcv_args.mtx_x_sz);
	fmatrix_free(dual, numb_rest + 1);
	PF_DBG("EXIT");
	return 0;
}

static void iteration (	float base_matrix[TAB_X][TAB_Y],
						const int numb_rest,
						const int numb_var,
						const int min_max)
{
	int x, y, cont = 0;
	int ref_x = 0, ref_y = 0;
	float vet_men[numb_rest - 1][numb_rest - 1];

	PF_DBG("ENTER");
	for (x = 1; x <= numb_var; x++) {
		int base = 0; //var que serve para buscar o min ou o max

		for (y = 0; y < numb_rest; y++){
			if (min_max == 1 && base_matrix[x][y] < base) { //max
				base = base_matrix[x][y];
				ref_x = x;
				ref_y = y;
			} else if (min_max == 0 && base_matrix[x][y] > base) { //min
				base = base_matrix[x][y];
				ref_x = x;
				ref_y = y;
			}
		}
	}

	for (y = 0; y <= numb_rest; y++) {
		if (!y == ref_y) { //não testa a linha com menor ou maior valor
			vet_men[cont][0] = (float) base_matrix[TAB_X-1][y] / base_matrix[ref_x][y]; //divisao do valor para avaliacao
			vet_men[cont][1] = y; // aloca a posicao do valor na matrix
			cont++;
		}
	}

	float menor_pivo = vet_men[0][0];			//seta o primeiro valor do vetor para iniciar os testes
	int y_pivo = vet_men[0][1];					// e a posicao do valor tambem

	for(x=1;x<numb_rest-1;x++){
		if(vet_men[x][0]>=0&&menor_pivo>vet_men[x][0]){		//testa se e maior que 0 e se e se e o menor
			menor_pivo=vet_men[x][0];
			y_pivo=vet_men[x][1];
		}
	}

	//linha pivo ja foi encontrada

	float element_pivo = base_matrix[ref_x][y_pivo];
	float linha_pivo[TAB_X] ;

	for(x=0;x<TAB_X;x++){//XGH  copia o vetor
		linha_pivo[x]=base_matrix[x][y_pivo];
	}

	for(x=0;x<TAB_X;x++){			//realiza a divizao da linha pivo pelo elemento pivo e guarda o resultado na linha da matrix
		linha_pivo[x]=(float)linha_pivo[x]/element_pivo;
		base_matrix[x][y_pivo]=(float)linha_pivo[x];
	}

	//linha pivo ja encontrada

	for(y=0;y<TAB_Y;y++){
		if(y!=y_pivo){						//aplica o pivo nas demais linhas
			float mid_pivo =base_matrix[ref_x][y]*(-1);		//inverso do valor que e o pivo da multiplicacao

			float cop_linha_pivo[TAB_X];
			for(x=0;x<TAB_X;x++){//XGH  copia a linha pivo original
				cop_linha_pivo[x]=linha_pivo[x];
			}

			for(x=0;x<TAB_X;x++){				//o multiplicador e aplicado na linha e somado a linha da matrix
				cop_linha_pivo[x]=(float)cop_linha_pivo[x]*mid_pivo;
				base_matrix[x][y]=(float)cop_linha_pivo[x]+base_matrix[x][y];
				//printf("%f%s",base_matrix[x][y]," ");
			}
			//printf("\n");
		}
	}
	PF_DBG("EXIT");
};

static bool verificaMatriz(float base_matrix[TAB_X][TAB_Y], int numb_var)
{
	int x, y;

	PF_DBG("ENTER");
	/*
	 * Testa se os valores de X... encontram-se binarios indicando o fim das iterações
	 */
	for (x = 1; x < (1 + numb_var); x++) {
		for (y = 0; y < TAB_Y; y++) {
			if (!(base_matrix[x][y] == 0 || base_matrix[x][y] == 1)) {
				return false;
			}
		}
	}
	PF_DBG("EXIT");
	return true;
};



static otimizacao_t sensibilidade(	float base_matrix[TAB_X][TAB_Y],
									float copy_matrix[TAB_X][TAB_Y],
									int numb_var)
{
	int pivoy_x1 = 0, pivoy_x2 = 0, y;
	otimizacao_t ot;

	PF_DBG("ENTER");
	if(numb_var==2){		//somente 2 variaveis
		float reference_matrix[2][2], b_matrix[2][2], original_b_matrix[2][2];
		int marcador_b[2];

		//
		//procura o pivo para as duas variaveis, ou seja, onde a variavel se mostra 1 e por consequencia o S será o valor correto para a solução ideal
		//
		for(y=0;y<TAB_Y;y++){
			if(base_matrix[1][y]==1){
				pivoy_x1=y;
				break;
			}
		}
		for(y=0;y<TAB_Y;y++){
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
			b_matrix[0][1]=copy_matrix[TAB_X-1][pivoy_x1];
			b_matrix[1][0]=copy_matrix[TAB_X-1][pivoy_x2];
			b_matrix[1][1]=0;

			original_b_matrix[0][0]=0;
			original_b_matrix[0][1]=copy_matrix[1][0];
			original_b_matrix[1][0]=copy_matrix[2][0];
			original_b_matrix[1][1]=0;

			printf("\n L: %f %f\n",copy_matrix[1][0], copy_matrix[2][0]);

			marcador_b[0]=0;
			marcador_b[1]=1;

		} else if (pivoy_x1 > pivoy_x2) {
			for( y=0;y<2;y++){
					reference_matrix[y][0]=base_matrix[3+y][pivoy_x2];
					reference_matrix[y][1]=base_matrix[3+y][pivoy_x1];
				}
			b_matrix[0][0]=copy_matrix[TAB_X-1][pivoy_x2];
			b_matrix[0][1]=0;
			b_matrix[1][0]=0;
			b_matrix[1][1]=copy_matrix[TAB_X-1][pivoy_x1];

			original_b_matrix[0][0]=copy_matrix[1][0];
			original_b_matrix[0][1]=0;
			original_b_matrix[1][0]=0;
			original_b_matrix[1][1]=copy_matrix[2][0];

			printf("\n L: %f %f\n",copy_matrix[1][0], copy_matrix[2][0]);

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

		ot.parm1=par1;
		ot.parm2=par2;
		ot.parm3=par3;
		ot.parm4=par4;


		/*for(y=0;y<2;y++){
			for(x=0;x<2;x++){
				printf("%f%s",b_matrix[x][y]," ");
			}
			printf("\n");
		}
		printf("\n\n");
		for(y=0;y<2;y++){
			for(x=0;x<2;x++){
				printf("%f%s",reference_matrix[x][y]," ");
			}
			printf("\n");
		}
		printf("\n\n\n");*/

		/*printf("%s%f%s%f","menor igual: ",ot.parm1.menor_igual, "  maior igual: ",ot.parm1.maior_igual);
		printf("\n");
		printf("%s%f%s%f","menor igual: ",ot.parm2.menor_igual, "  maior igual: ",ot.parm2.maior_igual);*/
	}


	PF_DBG("EXIT");
	return ot;
}

static float **dualidade(float copy_matrix[TAB_X][TAB_Y], int numb_var, int numb_rest)
{
	int x, y;
	float **dual;

	PF_DBG("ENTER");
	dual = fmatrix_calloc(numb_rest + 1, numb_var + 1);

	for(y=1;y<TAB_Y;y++){
		for(x=1;x<1+numb_var;x++){
			dual[y-1][x]=copy_matrix[x][y];
		}
	}
	y=1;
	for(x=0;x<numb_rest;x++){
		dual[x][0]=copy_matrix[TAB_X-1][y];
		y++;
	}
	dual[numb_rest][0]=0;		//zera o ultimo coeficiente q fica fora do loop
	y=1;
	for(x=1;x<1+numb_var;x++){
		dual[numb_rest][y]=copy_matrix[y][0];
		y++;
	}

	/*
	for(y=0;y<numb_var+1;y++){
		for(x=0;x<numb_rest+1;x++){
			printf("%02.2f\t",dual[x][y]);
		}
		printf("\n");
	}
	*/

	PF_DBG("EXIT");
	return dual;
}
