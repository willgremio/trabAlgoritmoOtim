#!/bin/bash

# edit the matrix according with X and Y axis sizes
declare -A mtx

# exemplo MAX aula
#problem_type="max"	# (string) 	choose between "min" or "max"
#variable_cnt=2		# (number)	number of variables
#restiction_cnt=3	# (number)	number of restrictions
#matrix_x_size=7		# (number)	matrix x axis length
#matrix_y_size=4		# (number)	matrix y axis length
# Z			x1				x2				F1			F2			F3			b
#mtx[0,0]=1;	mtx[1,0]=-3;	mtx[2,0]=-5;	mtx[3,0]=0;	mtx[4,0]=0;	mtx[5,0]=0;	mtx[6,0]=0;
#mtx[0,1]=0;	mtx[1,1]=2;		mtx[2,1]=4;		mtx[3,1]=1;	mtx[4,1]=0;	mtx[5,1]=0;	mtx[6,1]=10;
#mtx[0,2]=0;	mtx[1,2]=6;		mtx[2,2]=1;		mtx[3,2]=0;	mtx[4,2]=1;	mtx[5,2]=0;	mtx[6,2]=20;
#mtx[0,3]=0;	mtx[1,3]=1;		mtx[2,3]=-1;	mtx[3,3]=0;	mtx[4,3]=0;	mtx[5,3]=1;	mtx[6,3]=30;

# exemplo MIN aula
#problem_type="min"	# (string) 	choose between "min" or "max"
#variable_cnt=2		# (number)	number of variables
#restiction_cnt=3	# (number)	number of restrictions
#matrix_x_size=7		# (number)	matrix x axis length
#matrix_y_size=4		# (number)	matrix y axis length
# Z			x1				x2				F1			F2			F3			b
#mtx[0,0]=1;	mtx[1,0]=5;		mtx[2,0]=4; 	mtx[3,0]=0;	mtx[4,0]=0;	mtx[5,0]=0;	mtx[6,0]=0;
#mtx[0,1]=0;	mtx[1,1]=2;		mtx[2,1]=2;		mtx[3,1]=1;	mtx[4,1]=0;	mtx[5,1]=0;	mtx[6,1]=14;
#mtx[0,2]=0;	mtx[1,2]=6;		mtx[2,2]=3;		mtx[3,2]=0;	mtx[4,2]=1;	mtx[5,2]=0;	mtx[6,2]=36;
#mtx[0,3]=0;	mtx[1,3]=5;		mtx[2,3]=10;	mtx[3,3]=0;	mtx[4,3]=0;	mtx[5,3]=1;	mtx[6,3]=60;

# trabalho
problem_type="min"	# (string) 	choose between "min" or "max"
variable_cnt=2		# (number)	number of variables
restiction_cnt=5	# (number)	number of restrictions
matrix_x_size=9		# (number)	matrix x axis length
matrix_y_size=6		# (number)	matrix y axis length
#Z			x1				x2				F1			F2			F3			F4			F5			b
mtx[0,0]=1;	mtx[1,0]=-1;	mtx[2,0]=-1;	mtx[3,0]=0;	mtx[4,0]=0;	mtx[5,0]=0;	mtx[6,0]=0;	mtx[7,0]=0;	mtx[8,0]=0;
mtx[0,1]=0;	mtx[1,1]=-0.6;	mtx[2,1]=0.4;	mtx[3,1]=1;	mtx[4,1]=0;	mtx[5,1]=0;	mtx[6,1]=0;	mtx[7,1]=0;	mtx[8,1]=0;
mtx[0,2]=0;	mtx[1,2]=0.2;	mtx[2,2]=0.1;	mtx[3,2]=0;	mtx[4,2]=1;	mtx[5,2]=0;	mtx[6,2]=0;	mtx[7,2]=0;	mtx[8,2]=14;
mtx[0,3]=0;	mtx[1,3]=0.25;	mtx[2,3]=0.6;	mtx[3,3]=0;	mtx[4,3]=0;	mtx[5,3]=1;	mtx[6,3]=0;	mtx[7,3]=0;	mtx[8,3]=30;
mtx[0,4]=0;	mtx[1,4]=0.1;	mtx[2,4]=0.15;	mtx[3,4]=0;	mtx[4,4]=0;	mtx[5,4]=0;	mtx[6,4]=1;	mtx[7,4]=0;	mtx[8,4]=10;
mtx[0,5]=0;	mtx[1,5]=0.15;	mtx[2,5]=0.1;	mtx[3,5]=0;	mtx[4,5]=0;	mtx[5,5]=0;	mtx[6,5]=0;	mtx[7,5]=1;	mtx[8,5]=8;

# set max or min
param1=0
if [[ $problem_type == "min" ]]; then
	param1=0
elif [[ $problem_type == "max" ]]; then
	param1=1
else
	echo Error: 'problem_type' should be 'min' or 'max'
fi

# set variable count
param2=""
if [[ $variable_cnt -gt 0 ]]; then
	param2=${variable_cnt}
else
	echo Error: 'variable_cnt' should be > 0
fi

# set restriction count
param3=""
if [[ $restiction_cnt -gt 0 ]]; then
	param3=${restiction_cnt}
else
	echo Error: 'restiction_cnt' should be > 0
fi

# set matrix
param_matrix=""
for (( y = 0; y < matrix_y_size; y++ )); do
	for (( x = 0; x < matrix_x_size; x++ )); do
		if [[ -z ${mtx[$x,$y]} ]]; then
			echo Error: empty parameter in matrix x = $x and y = $y
			exit 1
		fi
		param_matrix="$param_matrix ${mtx[$x,$y]}"
	done
done

parameters="$param1 $param2 $param3 $param_matrix"
echo $parameters
./a.out $parameters
