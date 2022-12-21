num1: SET 9
num2: SET 8
num3: SET 7
ldc num1
ldc num2
sub
brlz 8
ldc num1
ldc num3
sub 
brlz 2
ldc num1
HALT
ldc num3
HALT
ldc num2
ldc num3
sub
brlz 2
ldc num2
HALT
ldc num3
HALT