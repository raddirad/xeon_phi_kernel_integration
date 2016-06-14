#/bin/bash
for i in {1..15}; do
  sudo insmod scif_dma.ko && sudo rmmod scif_dma.ko
done
