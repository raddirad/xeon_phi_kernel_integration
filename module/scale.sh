#/bin/bash
for i in {1..15}; do
  sudo insmod scaling.ko && sudo rmmod scaling.ko
done
