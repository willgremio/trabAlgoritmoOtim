#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAB_X 7
#define TAB_Y 4

#define MATRIX_TYPE float
static MATRIX_TYPE **base_matrix;

static void iteration(int numb_rest, int numb_var, int min_max);

int main()
{
    int numb_rest=3, x;

	// malloc base matrix
	base_matrix = (MATRIX_TYPE **) malloc(TAB_X * sizeof(MATRIX_TYPE *));
	for (x = 0; x < TAB_X; x++) {
		base_matrix[x] = (MATRIX_TYPE *) malloc(TAB_Y * sizeof(MATRIX_TYPE));
	}

	base_matrix[0][0]=1;
	base_matrix[1][0]=-3;
	base_matrix[2][0]=-5;
	base_matrix[3][0]=0;
	base_matrix[4][0]=0;
	base_matrix[5][0]=0;
	base_matrix[6][0]=0;

	base_matrix[0][1]=0;
	base_matrix[1][1]=2;
	base_matrix[2][1]=4;
	base_matrix[3][1]=1;
	base_matrix[4][1]=0;
	base_matrix[5][1]=0;
	base_matrix[6][1]=10;

	base_matrix[0][2]=0;
	base_matrix[1][2]=6;
	base_matrix[2][2]=1;
	base_matrix[3][2]=0;
	base_matrix[4][2]=1;
	base_matrix[5][2]=0;
	base_matrix[6][2]=20;

	base_matrix[0][3]=0;
	base_matrix[1][3]=1;
	base_matrix[2][3]=-1;
	base_matrix[3][3]=0;
	base_matrix[4][3]=0;
	base_matrix[5][3]=1;
	base_matrix[6][3]=30;

	int numb_var=2;
	int min_max=1; // min=0, max=1

	iteration(numb_rest, numb_var,min_max);

	for (x = 0; x < TAB_X; x++) {
		free(base_matrix[x]);
	}
	free(base_matrix);

	return 0;
}

static void iteration(int numb_rest, int numb_var, int min_max)
{
	int x;
	int ref_x=0,ref_y=0;
		for(x=1;x<=numb_var;x++){
			int base =0;		//var que serve para buscar o min ou o max
			int y;

			for(y=0;y<numb_rest;y++){
				if(min_max==1&&base_matrix[x][y]<base){ //max
					base=base_matrix[x][y];
					ref_x=x;
					ref_y=y;
				}else if(min_max==0&&base_matrix[x][y]>base){  //min
					base = base_matrix[x][y];
					ref_x=x;
					ref_y=y;
				}
			}
		}


		float vet_men[numb_rest-1][numb_rest-1];
		int y, cont =0;
		for(y=0;y<=numb_rest;y++){
			if(!y==ref_y){  //não testa a linha com menor ou maior valor
				vet_men[cont][0]=(float)base_matrix[TAB_X-1][y]/base_matrix[ref_x][y];		//divisao do valor para avaliacao
				vet_men[cont][1]=y;					// aloca a posicao do valor na matrix
				cont++;
			}
		}

		float menor_pivo =vet_men[0][0];			//seta o primeiro valor do vetor para iniciar os testes

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
					printf("%f%s",base_matrix[x][y]," ");
				}
				printf("\n");
			}
		}
};

