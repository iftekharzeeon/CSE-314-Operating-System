echo "Running h20 with pthread"
gcc h20_pthread.c zemaphore.c -o h20_pthread -lpthread
./h20_pthread $1 $2
echo "---------------------------------"
echo "Running h20 with zemaphore only"
gcc h20_zem.c zemaphore.c -o h20_zem -lpthread
./h20_zem $1 $2
rm h20_pthread
rm h20_zem