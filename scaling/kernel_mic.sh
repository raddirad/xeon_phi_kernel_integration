#/bin/bash
for j in {1..57}; do
	for i in {1..15}; do
  	./aes_kernel.mic >> ${j}_${i}.txt
	done
done

