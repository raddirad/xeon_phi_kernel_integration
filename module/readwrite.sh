#/bin/bash
for i in {1..15}; do
  sudo insmod readwrite.ko && sudo rmmod readwrite.ko
done
