#/bin/bash
for i in {1..15}; do
  ./throughput.mic >> ${i}.txt
done

