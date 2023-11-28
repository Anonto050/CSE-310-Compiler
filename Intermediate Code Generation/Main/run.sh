yacc -d -y 1905050.y
echo "y.tab.c created"
g++ -w -c -o y.o y.tab.c
flex 1905050.l
echo "lex.yy.c created"
g++ -fpermissive -w -c -o l.o lex.yy.c
g++ y.o l.o -lfl -o 1905050
./1905050 input.c
