#!/bin/bash

# #pocet cisel bud zadam nebo 10 :)
# if [ $# -lt 1 ];then 
#     numbers=10;
# else
#     numbers=$1;
# fi;

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o parkmeans parkmeans.cc


#vyrobeni souboru s random cisly
# dd if=/dev/random bs=1 count=$numbers of=numbers

#spusteni
# mpirun --prefix /usr/local/share/OpenMPI -np $numbers parsplit
mpirun --prefix /usr/local/share/OpenMPI -np 10 parkmeans

#uklid
# rm -f oems numbers
