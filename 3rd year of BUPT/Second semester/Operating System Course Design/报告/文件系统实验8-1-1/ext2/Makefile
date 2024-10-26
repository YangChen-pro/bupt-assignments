

ext2fs_std: main.o super.o group.o chdc.o inode.o file.o ascii.o blockbitmap.o inodebitmap.o
	gcc -o ext2fs_std main.o super.o group.o chdc.o inode.o file.o ascii.o blockbitmap.o inodebitmap.o

main.o: main.c ext2fs.h
	gcc -c main.c

super.o: super.c ext2fs.h
	gcc -c super.c

group.o: group.c ext2fs.h
	gcc -c group.c

chdc.o: chdc.c ext2fs.h
	gcc -c chdc.c

inode.o: inode.c ext2fs.h
	gcc -c inode.c

file.o: file.c ext2fs.h
	gcc -c file.c

ascii.o: ascii.c ext2fs.h
	gcc -c ascii.c

blockbitmap.o: blockbitmap.c ext2fs.h
	gcc -c blockbitmap.c

inodebitmap.o: inodebitmap.c ext2fs.h
	gcc -c inodebitmap.c

clean:
	rm *.o


