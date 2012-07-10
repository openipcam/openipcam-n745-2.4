#Ensure starting in right position.

cd /home/nuvoton/uClinux-dist/user/gpio/gpio 
make clean
#cd ../reseteth
#make clean

cd ../../
make

cd user/gpio
flthdr -z gpio

#cd ../reseteth
#flthdr -z reseteth


rm -rf /tmp/new
rm -rf /tmp/cam

mkdir /tmp/new
mkdir /tmp/cam

mount -o loop -t romfs /mnt/hgfs/Documents/openipcam-n745-2.4/Original\ Firmware/romfs_2.4_from_camera.img  /tmp/cam
cd /tmp/cam
cp * -Rp ../new

#Add in our new binaries, plus essentials
rm -r /tmp/new/bin/*
cp /tmp/cam/bin/init /tmp/new/bin/ -p
cp /tmp/cam/bin/sh /tmp/new/bin/ -p
cp /home/nuvoton/uClinux-dist/user/gpio/gpio /tmp/new/bin
#cp /home/nuvoton/uClinux-dist/user/reseteth/reseteth /tmp/new/bin

#replace camera with No Camera!
sed -i 's/camera/# no camera/g' /tmp/new/bin/init

#Make babies
cd /tmp
genromfs -f myromfs.img -d new

#Copy babies -> romfs folder for upload
cp myromfs.img /mnt/hgfs/Documents/openipcam-n745-2.4/Original\ Firmware/

#cleanup our mess
umount /tmp/cam
rm -rf /tmp/new
rm -rf /tmp/cam

echo Open up minicom and send the new file now...

