num1: SET 10
num2: SET 20
num3: SET 5
ldc num1
ldc num2
sub
brlz la
ldc num2
ldc num3
sub
brlz lb
ldc num1
HALT
lb:
ldc num3
HALT
la:
ldc num2
ldc num3
sub
brlz lc
ldc num2
HALT
lc:
ldc num1
HALT