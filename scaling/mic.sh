#/bin/bash
for i in {1..80}; do
export OMP_NUM_THREADS=${i}
	for j in {1..15}; do
 	 ./scale.mic >> results/${i}_${j}.txt
	done
done

