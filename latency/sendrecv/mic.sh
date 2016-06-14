#/bin/bash
for i in {1..15}; do
  ./kernel.mic >> ${i}.txt
done

