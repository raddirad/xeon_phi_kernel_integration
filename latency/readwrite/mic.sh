#/bin/bash
for i in {1..15}; do
  ./readwrite.mic >> ${i}.txt
done

