#/bin/bash
for i in {1..15}; do
  ./aes_bench.mic >> ${i}.txt
done

