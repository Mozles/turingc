# Turing C
just a simple turing machine written in C

there are 2 versions:
- main.exe \[program file\] \[data file\]
- main_2.exe \[program_file\] data

## program file structure
\[table width\] \[table height\]
 
\[row name, column name or field\]

## field structure
0 1 L
0 - write 0 
1 - set state to 1
L - go left

## directions
L - go left
R - go right
N - do nothing

## example
simple base-3 incrementer
1 - increment
2 - return
|   |   1   |   2   |
|---|-------|-------|
| $ | 1 3 R | $ 1 L |
| 0 | 1 3 R | 0 3 R |
| 1 | 2 3 R | 1 3 R |
| 2 | 0 2 L | 2 3 R |

is going to be written as 
### increment.txt
```text
4 3
 
$
0
1
2
1
1 3 R
1 3 R
2 3 R
0 2 L
2
$ 1 L
0 2 R
1 2 R
2 2 R
```
