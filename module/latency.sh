#/bin/bash
for i in {1..15}; do
  sudo insmod latency.ko && sudo rmmod latency.ko
done

