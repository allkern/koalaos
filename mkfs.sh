dd if=/dev/zero of=disk.img bs=1M count=512
sudo losetup /dev/loop32 disk.img
sudo mkfs -v -t ext2 /dev/loop32 -b 1024 -L KOALAOS -d root
sudo losetup -d /dev/loop32
