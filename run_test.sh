#Rodar N vezes...
for i in {1..5000}; do
echo "-run: $i"
#Passando a quantidade de trabalho minima
./exec $1
done