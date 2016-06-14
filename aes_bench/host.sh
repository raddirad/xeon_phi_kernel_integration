#/bin/bash
for i in {1..15}; do
  ./aes_bench
paste -d ',' {1..15}.txt > output.csv
done
