#!/bin/bash

# =============
# user editable
# =============
problem_type="max"	# (string) 	choose between "min" or "max"
variable_cnt=2		# (number)	number of variables
restiction_cnt=3	# (number)	number of restrictions
matrix_x_size=7		# (number)	matrix x axis length
matrix_y_size=4		# (number)	matrix y axis length

# edit the matrix according with X and Y axis sizes
declare -A mtx

#Z			x1				x2				F1			F2			F3			b
mtx[0,0]=1;	mtx[1,0]=-3;	mtx[2,0]=-5;	mtx[3,0]=0;	mtx[4,0]=0;	mtx[5,0]=0;	mtx[6,0]=0;
mtx[0,1]=0;	mtx[1,1]=2;		mtx[2,1]=4;		mtx[3,1]=1;	mtx[4,1]=0;	mtx[5,1]=0;	mtx[6,1]=10;
mtx[0,2]=0;	mtx[1,2]=6;		mtx[2,2]=1;		mtx[3,2]=0;	mtx[4,2]=1;	mtx[5,2]=0;	mtx[6,2]=20;
mtx[0,3]=0;	mtx[1,3]=1;		mtx[2,3]=-1;	mtx[3,3]=0;	mtx[4,3]=0;	mtx[5,3]=1;	mtx[6,3]=30;


# =================
# user not editable
# =================

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
