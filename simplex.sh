#!/bin/bash

# =============
# user editable
# =============
problem_type="min"	# (string) 	choose between "min" or "max"
variable_cnt=2		# (number)	number of variables
restiction_cnt=5	# (number)	number of restrictions
matrix_x_size=3		# (number)	matrix x axis length
matrix_y_size=3		# (number)	matrix y axis length

# edit the matrix according with X and Y axis sizes
declare -A matrix

matrix[0,0]="1.0"
matrix[0,1]="1.1"
matrix[0,2]="1.2"

matrix[1,0]="2.0"
matrix[1,1]="2.1"
matrix[1,2]="2.2"

matrix[2,0]="3.0"
matrix[2,1]="3.1"
matrix[2,2]="3.2"


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
	echo "error, 'problem_type' should be 'min' or 'max'"
fi

# set variable count
param2=""
if [[ $variable_cnt -gt 0 ]]; then
	param2=${variable_cnt}
else
	echo "error, 'variable_cnt' should be > 0"
fi

# set restriction count
param3=""
if [[ $restiction_cnt -gt 0 ]]; then
	param3=${restiction_cnt}
else
	echo "error, 'restiction_cnt' should be > 0"
fi

# set matrix
param_matrix=""
for (( x = 0; x < matrix_x_size; x++ )); do
	for (( y = 0; y < matrix_y_size; y++ )); do
		#echo "${x} ${y}"
		param_matrix="$param_matrix ${matrix[$x,$y]}"
	done
done

echo "$param1 $param2 $param3 ${param_matrix}"
#./a.out "$param1 $param2 $param3 ${param_matrix}"
