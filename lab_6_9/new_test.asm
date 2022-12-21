start:SET 50
ldc 20	
adc 5	
ldc result
a2sp 
adj 1
val: SET 75
ldc     val
stl 1
triangle:adj -3
stl 1
adc -1
call triangle
ldl 1
stl 0
result: data 100
