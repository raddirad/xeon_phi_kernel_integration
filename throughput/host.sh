#/bin/bash
for i in {1..15}; do
  ./throughput
paste -d ',' {1..15}.txt > dev05_cpucopy.csv
done
