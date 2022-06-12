make clean
make
rmmod modulo

insmod modulo.ko
mknod /dev/TP_FINAL c 510 0
chmod 777 /dev/TP_FINAL
echo 'Listo el modulo'
#sudo su
#echo "" > /dev/TP_FINAL
