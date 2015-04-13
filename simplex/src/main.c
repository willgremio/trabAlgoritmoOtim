#define TAB_X 7
#define TAB_Y 4

struct paramentros{
	float menor_igual,maior_igual;
};
struct otimizacao{
	struct paramentros parm1,parm2;
};

//void iteration(float **base_matrix, int numb_rest, int numb_var, int min_max);
struct otimizacao sensibilidade(float base_matrix[TAB_X][TAB_Y],float copy_matrix[TAB_X][TAB_Y], int numb_var);
float** dualidade(float copy_matrix[TAB_X][TAB_Y], int numb_var,int numb_rest);

int main(){
    int numb_rest=3;
    float base_matrix[TAB_X][TAB_Y], copy_matrix[TAB_X][TAB_Y];

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

	int numb_var=2;
	int min_max=1; // min=0, max=1
	//strcpy(copy_matrix,base_matrix);		//copy_matrix=base_matrix;


	while(verificaMatriz(base_matrix,numb_var)!=1){
		iteration(base_matrix, numb_rest, numb_var,min_max);
	}

	sensibilidade(base_matrix,copy_matrix,numb_var);

	float** dual = dualidade(copy_matrix,numb_var,numb_rest);


	int x,y;

	/*for(y=0;y<TAB_Y;y++){
			for(x=0;x<TAB_X;x++){
				printf("%f%s",base_matrix[x][y]," ");
			}
			printf("\n");
		}
	printf("\n\n");
*/
	for(y=0;y<TAB_Y;y++){
		for(x=0;x<TAB_X;x++){
			printf("%f%s",copy_matrix[x][y]," ");
		}
		printf("\n");
	}
	for(y=0;y<TAB_Y;y++){
			for(x=0;x<TAB_X;x++){
				printf("%f%s",dual[x][y]," ");
			}
			printf("\n");
		}

	return 0;

}

void iteration(float base_matrix[TAB_X][TAB_Y], int numb_rest, int numb_var, int min_max){
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
					//printf("%f%s",base_matrix[x][y]," ");
				}
				//printf("\n");
			}
		}
};

int verificaMatriz(float base_matrix[TAB_X][TAB_Y], int numb_var) {
	int x = 1;
	int y = 1;
	for(x=1; x < 1+numb_var; x++) {
		for(y=0; y < TAB_Y; y++) {
			if(!(base_matrix[x][y] == 0 || base_matrix[x][y] == 1)) {
					return 0;
				}
		}
	}

	return 1;
};



struct otimizacao sensibilidade(float base_matrix[TAB_X][TAB_Y],float copy_matrix[TAB_X][TAB_Y], int numb_var){
	struct otimizacao ot;
	if(numb_var==2){		//somente 2 variaveis
		int pivoy_x1,pivoy_x2,y;
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

		float reference_matrix[2][2], b_matrix[2][2];
		int marcador_b[2];
		if(pivoy_x1<pivoy_x2){
			for( y=0;y<2;y++){
				reference_matrix[y][0]=base_matrix[3+y][pivoy_x1];
				reference_matrix[y][1]=base_matrix[3+y][pivoy_x2];
			}

			b_matrix[0][0]=0;
			b_matrix[0][1]=copy_matrix[TAB_X-1][pivoy_x1];
			b_matrix[1][0]=copy_matrix[TAB_X-1][pivoy_x2];
			b_matrix[1][1]=0;

			marcador_b[0]=0;
			marcador_b[1]=1;

		}else if(pivoy_x1>pivoy_x2){
			for( y=0;y<2;y++){
					reference_matrix[y][0]=base_matrix[3+y][pivoy_x2];
					reference_matrix[y][1]=base_matrix[3+y][pivoy_x1];
				}
			b_matrix[0][0]=copy_matrix[TAB_X-1][pivoy_x2];
			b_matrix[0][1]=0;
			b_matrix[1][0]=0;
			b_matrix[1][1]=copy_matrix[TAB_X-1][pivoy_x1];

			marcador_b[0]=1;
			marcador_b[1]=0;
		}else{
			printf("\n ERROR");
		}
		int x;
		struct paramentros par1,par2;
		for(x=0;x<2;x++){

			if(marcador_b[x]==0){
				if(x==0){
					par1.menor_igual=(float)(reference_matrix[1][0]*b_matrix[x][1]*-1)/(reference_matrix[0][0]);
					par1.maior_igual=(float)(reference_matrix[1][1]*b_matrix[x][1]*-1)/(reference_matrix[0][1]);

				}else{
					par2.menor_igual=(float)(reference_matrix[1][0]*b_matrix[x][1]*-1)/(reference_matrix[0][0]);
					par2.maior_igual=(float)(reference_matrix[1][1]*b_matrix[x][1]*-1)/(reference_matrix[0][1]);
					//printf("%f%s%f%s%f",reference_matrix[0][1]," ",reference_matrix[1][1]," ",b_matrix[x][1]);

				}
			}else{
				if(x==0){
					par1.menor_igual=(float)(reference_matrix[0][0]*b_matrix[x][0]*-1)/(reference_matrix[1][0]);
					par1.maior_igual=(float)(reference_matrix[0][1]*b_matrix[x][0]*-1)/(reference_matrix[1][1]);

				}else{


					par2.menor_igual=(float)(reference_matrix[0][0]*b_matrix[x][0]*-1)/(reference_matrix[1][0]);
					par2.maior_igual=(float)(reference_matrix[0][1]*b_matrix[x][0]*-1)/(reference_matrix[1][1]);

				}
			}
		}

		ot.parm1=par1;
		ot.parm2=par2;


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


	return ot;

}

float** dualidade(float copy_matrix[TAB_X][TAB_Y], int numb_var,int numb_rest){
	int x, y;
	float dual[numb_rest+1][numb_var+1];

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

	for(y=0;y<numb_var+1;y++){
				for(x=0;x<numb_rest+1;x++){
					printf("%f%s",dual[x][y]," ");
				}
				printf("\n");
			}
	return dual;
}
